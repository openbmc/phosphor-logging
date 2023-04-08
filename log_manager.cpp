#include "config.h"

#include "log_manager.hpp"

#include "elog_entry.hpp"
#include "elog_meta.hpp"
#include "elog_serialize.hpp"
#include "extensions.hpp"
#include "util.hpp"

#include <systemd/sd-bus.h>
#include <systemd/sd-journal.h>
#include <unistd.h>

#include <phosphor-logging/lg2.hpp>
#include <sdbusplus/vtable.hpp>
#include <xyz/openbmc_project/State/Host/server.hpp>

#include <cassert>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>

using namespace std::chrono;
extern const std::map<
    phosphor::logging::metadata::Metadata,
    std::function<phosphor::logging::metadata::associations::Type>>
    meta;

namespace phosphor
{
namespace logging
{
namespace internal
{
inline auto getLevel(const std::string& errMsg)
{
    auto reqLevel = Entry::Level::Error; // Default to Error

    auto levelmap = g_errLevelMap.find(errMsg);
    if (levelmap != g_errLevelMap.end())
    {
        reqLevel = static_cast<Entry::Level>(levelmap->second);
    }

    return reqLevel;
}

int Manager::getRealErrSize()
{
    return realErrors.size();
}

int Manager::getInfoErrSize()
{
    return infoErrors.size();
}

uint32_t Manager::commit(uint64_t transactionId, std::string errMsg)
{
    auto level = getLevel(errMsg);
    _commit(transactionId, std::move(errMsg), level);
    return entryId;
}

uint32_t Manager::commitWithLvl(uint64_t transactionId, std::string errMsg,
                                uint32_t errLvl)
{
    _commit(transactionId, std::move(errMsg),
            static_cast<Entry::Level>(errLvl));
    return entryId;
}

void Manager::_commit(uint64_t transactionId [[maybe_unused]],
                      std::string&& errMsg, Entry::Level errLvl)
{
    std::vector<std::string> additionalData{};

    // When running as a test-case, the system may have a LOT of journal
    // data and we may not have permissions to do some of the journal sync
    // operations.  Just skip over them.
    if (!IS_UNIT_TEST)
    {
        static constexpr auto transactionIdVar =
            std::string_view{"TRANSACTION_ID"};
        // Length of 'TRANSACTION_ID' string.
        static constexpr auto transactionIdVarSize = transactionIdVar.size();
        // Length of 'TRANSACTION_ID=' string.
        static constexpr auto transactionIdVarOffset = transactionIdVarSize + 1;

        // Flush all the pending log messages into the journal
        util::journalSync();

        sd_journal* j = nullptr;
        int rc = sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
        if (rc < 0)
        {
            lg2::error("Failed to open journal: {ERROR}", "ERROR",
                       strerror(-rc));
            return;
        }

        std::string transactionIdStr = std::to_string(transactionId);
        std::set<std::string> metalist;
        auto metamap = g_errMetaMap.find(errMsg);
        if (metamap != g_errMetaMap.end())
        {
            metalist.insert(metamap->second.begin(), metamap->second.end());
        }

        // Add _PID field information in AdditionalData.
        metalist.insert("_PID");

        // Read the journal from the end to get the most recent entry first.
        // The result from the sd_journal_get_data() is of the form
        // VARIABLE=value.
        SD_JOURNAL_FOREACH_BACKWARDS(j)
        {
            const char* data = nullptr;
            size_t length = 0;

            // Look for the transaction id metadata variable
            rc = sd_journal_get_data(j, transactionIdVar.data(),
                                     (const void**)&data, &length);
            if (rc < 0)
            {
                // This journal entry does not have the TRANSACTION_ID
                // metadata variable.
                continue;
            }

            // journald does not guarantee that sd_journal_get_data() returns
            // NULL terminated strings, so need to specify the size to use to
            // compare, use the returned length instead of anything that relies
            // on NULL terminators like strlen(). The data variable is in the
            // form of 'TRANSACTION_ID=1234'. Remove the TRANSACTION_ID
            // characters plus the (=) sign to do the comparison. 'data +
            // transactionIdVarOffset' will be in the form of '1234'. 'length -
            // transactionIdVarOffset' will be the length of '1234'.
            if ((length <= (transactionIdVarOffset)) ||
                (transactionIdStr.compare(
                     0, transactionIdStr.size(), data + transactionIdVarOffset,
                     length - transactionIdVarOffset) != 0))
            {
                // The value of the TRANSACTION_ID metadata is not the requested
                // transaction id number.
                continue;
            }

            // Search for all metadata variables in the current journal entry.
            for (auto i = metalist.cbegin(); i != metalist.cend();)
            {
                rc = sd_journal_get_data(j, (*i).c_str(), (const void**)&data,
                                         &length);
                if (rc < 0)
                {
                    // Metadata variable not found, check next metadata
                    // variable.
                    i++;
                    continue;
                }

                // Metadata variable found, save it and remove it from the set.
                additionalData.emplace_back(data, length);
                i = metalist.erase(i);
            }
            if (metalist.empty())
            {
                // All metadata variables found, break out of journal loop.
                break;
            }
        }
        if (!metalist.empty())
        {
            // Not all the metadata variables were found in the journal.
            for (auto& metaVarStr : metalist)
            {
                lg2::info("Failed to find metadata: {META_FIELD}", "META_FIELD",
                          metaVarStr);
            }
        }

        sd_journal_close(j);
    }
    createEntry(errMsg, errLvl, additionalData);
}

void Manager::createEntry(std::string errMsg, Entry::Level errLvl,
                          std::vector<std::string> additionalData,
                          const FFDCEntries& ffdc)
{
    if (!Extensions::disableDefaultLogCaps())
    {
        if (errLvl < Entry::sevLowerLimit)
        {
            if (realErrors.size() >= ERROR_CAP)
            {
                erase(realErrors.front());
            }
        }
        else
        {
            if (infoErrors.size() >= ERROR_INFO_CAP)
            {
                erase(infoErrors.front());
            }
        }
    }

    entryId++;
    if (errLvl >= Entry::sevLowerLimit)
    {
        infoErrors.push_back(entryId);
    }
    else
    {
        realErrors.push_back(entryId);
    }
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::system_clock::now().time_since_epoch())
                  .count();
    auto objPath = std::string(OBJ_ENTRY) + '/' + std::to_string(entryId);

    AssociationList objects{};
    processMetadata(errMsg, additionalData, objects);

    auto e = std::make_unique<Entry>(
        busLog, objPath, entryId,
        ms, // Milliseconds since 1970
        errLvl, std::move(errMsg), std::move(additionalData),
        std::move(objects), fwVersion, getEntrySerializePath(entryId), *this);

    serialize(*e);

    if (isQuiesceOnErrorEnabled() && (errLvl < Entry::sevLowerLimit) &&
        isCalloutPresent(*e))
    {
        quiesceOnError(entryId);
    }

    // Add entry before calling the extensions so that they have access to it
    entries.insert(std::make_pair(entryId, std::move(e)));

    doExtensionLogCreate(*entries.find(entryId)->second, ffdc);

    // Note: No need to close the file descriptors in the FFDC.
}

bool Manager::isQuiesceOnErrorEnabled()
{
    // When running under tests, the Logging.Settings service will not be
    // present.  Assume false.
    if (IS_UNIT_TEST)
    {
        return false;
    }

    std::variant<bool> property;

    auto method = this->busLog.new_method_call(
        "xyz.openbmc_project.Settings", "/xyz/openbmc_project/logging/settings",
        "org.freedesktop.DBus.Properties", "Get");

    method.append("xyz.openbmc_project.Logging.Settings", "QuiesceOnHwError");

    try
    {
        auto reply = this->busLog.call(method);
        reply.read(property);
    }
    catch (const sdbusplus::exception_t& e)
    {
        lg2::error("Error reading QuiesceOnHwError property: {ERROR}", "ERROR",
                   e);
        throw;
    }

    return std::get<bool>(property);
}

bool Manager::isCalloutPresent(const Entry& entry)
{
    for (const auto& c : entry.additionalData())
    {
        if (c.find("CALLOUT_") != std::string::npos)
        {
            return true;
        }
    }

    return false;
}

void Manager::findAndRemoveResolvedBlocks()
{
    for (auto& entry : entries)
    {
        if (entry.second->resolved())
        {
            checkAndRemoveBlockingError(entry.first);
        }
    }
}

void Manager::onEntryResolve(sdbusplus::message_t& msg)
{
    using Interface = std::string;
    using Property = std::string;
    using Value = std::string;
    using Properties = std::map<Property, std::variant<Value>>;

    Interface interface;
    Properties properties;

    msg.read(interface, properties);

    for (const auto& p : properties)
    {
        if (p.first == "Resolved")
        {
            findAndRemoveResolvedBlocks();
            return;
        }
    }
}

void Manager::checkAndQuiesceHost()
{
    using Host = sdbusplus::xyz::openbmc_project::State::server::Host;

    // First check host state
    std::variant<Host::HostState> property;

    auto method = this->busLog.new_method_call(
        "xyz.openbmc_project.State.Host", "/xyz/openbmc_project/state/host0",
        "org.freedesktop.DBus.Properties", "Get");

    method.append("xyz.openbmc_project.State.Host", "CurrentHostState");

    try
    {
        auto reply = this->busLog.call(method);
        reply.read(property);
    }
    catch (const sdbusplus::exception_t& e)
    {
        // Quiescing the host is a "best effort" type function. If unable to
        // read the host state or it comes back empty, just return.
        // The boot block object will still be created and the associations to
        // find the log will be present. Don't want a dependency with
        // phosphor-state-manager service
        lg2::info("Error reading QuiesceOnHwError property: {ERROR}", "ERROR",
                  e);
        return;
    }

    auto hostState = std::get<Host::HostState>(property);
    if (hostState != Host::HostState::Running)
    {
        return;
    }

    auto quiesce = this->busLog.new_method_call(
        "org.freedesktop.systemd1", "/org/freedesktop/systemd1",
        "org.freedesktop.systemd1.Manager", "StartUnit");

    quiesce.append("obmc-host-graceful-quiesce@0.target");
    quiesce.append("replace");

    this->busLog.call_noreply(quiesce);
}

void Manager::quiesceOnError(const uint32_t entryId)
{
    // Verify we don't already have this entry blocking
    auto it = find_if(this->blockingErrors.begin(), this->blockingErrors.end(),
                      [&](const std::unique_ptr<Block>& obj) {
        return obj->entryId == entryId;
    });
    if (it != this->blockingErrors.end())
    {
        // Already recorded so just return
        lg2::debug(
            "QuiesceOnError set and callout present but entry already logged");
        return;
    }

    lg2::info("QuiesceOnError set and callout present");

    auto blockPath = std::string(OBJ_LOGGING) + "/block" +
                     std::to_string(entryId);
    auto blockObj = std::make_unique<Block>(this->busLog, blockPath, entryId);
    this->blockingErrors.push_back(std::move(blockObj));

    // Register call back if log is resolved
    using namespace sdbusplus::bus::match::rules;
    auto entryPath = std::string(OBJ_ENTRY) + '/' + std::to_string(entryId);
    auto callback = std::make_unique<sdbusplus::bus::match_t>(
        this->busLog,
        propertiesChanged(entryPath, "xyz.openbmc_project.Logging.Entry"),
        std::bind(std::mem_fn(&Manager::onEntryResolve), this,
                  std::placeholders::_1));

    propChangedEntryCallback.insert(
        std::make_pair(entryId, std::move(callback)));

    checkAndQuiesceHost();
}

void Manager::doExtensionLogCreate(const Entry& entry, const FFDCEntries& ffdc)
{
    // Make the association <endpointpath>/<endpointtype> paths
    std::vector<std::string> assocs;
    for (const auto& [forwardType, reverseType, endpoint] :
         entry.associations())
    {
        std::string e{endpoint};
        e += '/' + reverseType;
        assocs.push_back(e);
    }

    for (auto& create : Extensions::getCreateFunctions())
    {
        try
        {
            create(entry.message(), entry.id(), entry.timestamp(),
                   entry.severity(), entry.additionalData(), assocs, ffdc);
        }
        catch (const std::exception& e)
        {
            lg2::error(
                "An extension's create function threw an exception: {ERROR}",
                "ERROR", e);
        }
    }
}

void Manager::processMetadata(const std::string& /*errorName*/,
                              const std::vector<std::string>& additionalData,
                              AssociationList& objects) const
{
    // additionalData is a list of "metadata=value"
    constexpr auto separator = '=';
    for (const auto& entryItem : additionalData)
    {
        auto found = entryItem.find(separator);
        if (std::string::npos != found)
        {
            auto metadata = entryItem.substr(0, found);
            auto iter = meta.find(metadata);
            if (meta.end() != iter)
            {
                (iter->second)(metadata, additionalData, objects);
            }
        }
    }
}

void Manager::checkAndRemoveBlockingError(uint32_t entryId)
{
    // First look for blocking object and remove
    auto it = find_if(blockingErrors.begin(), blockingErrors.end(),
                      [&](const std::unique_ptr<Block>& obj) {
        return obj->entryId == entryId;
    });
    if (it != blockingErrors.end())
    {
        blockingErrors.erase(it);
    }

    // Now remove the callback looking for the error to be resolved
    auto resolveFind = propChangedEntryCallback.find(entryId);
    if (resolveFind != propChangedEntryCallback.end())
    {
        propChangedEntryCallback.erase(resolveFind);
    }

    return;
}

void Manager::erase(uint32_t entryId)
{
    auto entryFound = entries.find(entryId);
    if (entries.end() != entryFound)
    {
        for (auto& func : Extensions::getDeleteProhibitedFunctions())
        {
            try
            {
                bool prohibited = false;
                func(entryId, prohibited);
                if (prohibited)
                {
                    // Future work remains to throw an error here.
                    return;
                }
            }
            catch (const std::exception& e)
            {
                lg2::error("An extension's deleteProhibited function threw an "
                           "exception: {ERROR}",
                           "ERROR", e);
            }
        }

        // Delete the persistent representation of this error.
        fs::path errorPath(ERRLOG_PERSIST_PATH);
        errorPath /= std::to_string(entryId);
        fs::remove(errorPath);

        auto removeId = [](std::list<uint32_t>& ids, uint32_t id) {
            auto it = std::find(ids.begin(), ids.end(), id);
            if (it != ids.end())
            {
                ids.erase(it);
            }
        };
        if (entryFound->second->severity() >= Entry::sevLowerLimit)
        {
            removeId(infoErrors, entryId);
        }
        else
        {
            removeId(realErrors, entryId);
        }
        entries.erase(entryFound);

        checkAndRemoveBlockingError(entryId);

        for (auto& remove : Extensions::getDeleteFunctions())
        {
            try
            {
                remove(entryId);
            }
            catch (const std::exception& e)
            {
                lg2::error("An extension's delete function threw an exception: "
                           "{ERROR}",
                           "ERROR", e);
            }
        }
    }
    else
    {
        lg2::error("Invalid entry ID ({ID}) to delete", "ID", entryId);
    }
}

void Manager::restore()
{
    auto sanity = [](const auto& id, const auto& restoredId) {
        return id == restoredId;
    };

    fs::path dir(ERRLOG_PERSIST_PATH);
    if (!fs::exists(dir) || fs::is_empty(dir))
    {
        return;
    }

    for (auto& file : fs::directory_iterator(dir))
    {
        auto id = file.path().filename().c_str();
        auto idNum = std::stol(id);
        auto e = std::make_unique<Entry>(
            busLog, std::string(OBJ_ENTRY) + '/' + id, idNum, *this);
        if (deserialize(file.path(), *e))
        {
            // validate the restored error entry id
            if (sanity(static_cast<uint32_t>(idNum), e->id()))
            {
                e->path(file.path(), true);
                if (e->severity() >= Entry::sevLowerLimit)
                {
                    infoErrors.push_back(idNum);
                }
                else
                {
                    realErrors.push_back(idNum);
                }

                entries.insert(std::make_pair(idNum, std::move(e)));
            }
            else
            {
                lg2::error(
                    "Failed in sanity check while restoring error entry. "
                    "Ignoring error entry {ID_NUM}/{ENTRY_ID}.",
                    "ID_NUM", idNum, "ENTRY_ID", e->id());
            }
        }
    }

    if (!entries.empty())
    {
        entryId = entries.rbegin()->first;
    }
}

std::string Manager::readFWVersion()
{
    auto version = util::getOSReleaseValue("VERSION_ID");

    if (!version)
    {
        lg2::error("Unable to read BMC firmware version");
    }

    return version.value_or("");
}

void Manager::create(const std::string& message, Entry::Level severity,
                     const std::map<std::string, std::string>& additionalData)
{
    // Convert the map into a vector of "key=value" strings
    std::vector<std::string> ad;
    metadata::associations::combine(additionalData, ad);

    createEntry(message, severity, ad);
}

void Manager::createWithFFDC(
    const std::string& message, Entry::Level severity,
    const std::map<std::string, std::string>& additionalData,
    const FFDCEntries& ffdc)
{
    // Convert the map into a vector of "key=value" strings
    std::vector<std::string> ad;
    metadata::associations::combine(additionalData, ad);

    createEntry(message, severity, ad, ffdc);
}

} // namespace internal
} // namespace logging
} // namespace phosphor
