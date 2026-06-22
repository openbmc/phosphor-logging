#include "config.h"

#include "log_manager.hpp"

#include "elog_entry.hpp"
#include "elog_meta.hpp"
#include "elog_serialize.hpp"
#include "extensions.hpp"
#include "lib/lg2_commit.hpp"
#include "paths.hpp"
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
#include <functional>
#include <map>
#include <ranges>
#include <set>
#include <string>
#include <string_view>
#include <vector>

using namespace std::chrono;
extern const std::map<
    phosphor::logging::metadata::Metadata,
    std::function<phosphor::logging::metadata::associations::Type>>
    meta;

static constexpr auto mapperBusName = "xyz.openbmc_project.ObjectMapper";
static constexpr auto mapperObjPath = "/xyz/openbmc_project/object_mapper";
static constexpr auto mapperIntf = "xyz.openbmc_project.ObjectMapper";
static constexpr auto dbusProperty = "org.freedesktop.DBus.Properties";
static constexpr auto policyInterface = "xyz.openbmc_project.Logging.Settings";
static constexpr auto policyLinear =
    "xyz.openbmc_project.Logging.Settings.Policy.Linear";
static constexpr auto policyDefault =
    "xyz.openbmc_project.Logging.Settings.Policy.Circular";

using DBusInterface = std::string;
using DBusService = std::string;
using DBusPath = std::string;
using DBusInterfaceList = std::vector<DBusInterface>;
using DBusSubTree =
    std::map<DBusPath, std::map<DBusService, DBusInterfaceList>>;

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

Manager::~Manager()
{
    // Clean up D-Bus match callback. We use release() instead of reset()
    // because during test teardown, the mock D-Bus framework may already be
    // invalid, causing the match_t destructor to segfault when it tries to
    // unregister from D-Bus. Since the D-Bus daemon automatically cleans up
    // all subscriptions when the connection closes, we can safely release
    // ownership without explicit cleanup. The leaked object (typically <1KB)
    // is acceptable for global static objects that live for process lifetime.
    selPolicyChangedCallback.release();

    if constexpr (REDUNDANT_BMC)
    {
        if (errDirInotifyFD != -1)
        {
            if (errDirWatcherWD != -1)
            {
                inotify_rm_watch(errDirInotifyFD, errDirWatcherWD);
            }
            close(errDirInotifyFD);
        }
    }
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
    auto priorId = entryId;
    _commit(transactionId, std::move(errMsg), level);
    // Return 0 if entry was rejected (entryId unchanged); otherwise return new
    // entry ID
    return (entryId > priorId) ? entryId : 0;
}

uint32_t Manager::commitWithLvl(uint64_t transactionId, std::string errMsg,
                                uint32_t errLvl)
{
    auto priorId = entryId;
    _commit(transactionId, std::move(errMsg),
            static_cast<Entry::Level>(errLvl));
    // Return 0 if entry was rejected (entryId unchanged); otherwise return new
    // entry ID
    return (entryId > priorId) ? entryId : 0;
}

void Manager::_commit(uint64_t transactionId [[maybe_unused]],
                      std::string&& errMsg, Entry::Level errLvl)
{
    std::map<std::string, std::string> additionalData{};

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
                std::string metadata(data, length);
                if (auto pos = metadata.find('='); pos != std::string::npos)
                {
                    auto key = metadata.substr(0, pos);
                    auto value = metadata.substr(pos + 1);
                    additionalData.emplace(std::move(key), std::move(value));
                }
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
    // createEntry returns empty path if entry creation was rejected (e.g.,
    // linear SEL at capacity). Callers should check commit() return value: 0
    // means rejected, non-zero means entry was created with that ID.
    createEntry(errMsg, errLvl, additionalData);
}

std::string Manager::getSelPolicy()
{
    DBusSubTree subtree;

    try
    {
        auto method = this->busLog.new_method_call(mapperBusName, mapperObjPath,
                                                   mapperIntf, "GetSubTree");
        method.append(std::string{"/"}, 0,
                      std::vector<std::string>{policyInterface});

        auto reply = this->busLog.call(method);
        reply.read(subtree);
    }
    catch (const std::exception& e)
    {
        lg2::error("Failed to get SEL policy subtree from D-Bus: {ERROR}. "
                   "Continuing with Circular policy",
                   "ERROR", e);
        return policyDefault;
    }

    if (subtree.empty())
    {
        lg2::info("SEL policy interface not found on D-Bus. Continuing with "
                  "Circular policy");
        return policyDefault;
    }

    const auto& object = *subtree.begin();
    const auto& policyPath = object.first;
    const auto& policyService = object.second.begin()->first;

    std::variant<std::string> property;
    try
    {
        auto method = this->busLog.new_method_call(
            policyService.c_str(), policyPath.c_str(), dbusProperty, "Get");
        method.append(policyInterface, "SelPolicy");

        auto reply = this->busLog.call(method);
        reply.read(property);
    }
    catch (const std::exception& e)
    {
        lg2::error(
            "Failed to read SEL policy from D-Bus: {ERROR}. Continuing with "
            "Circular policy",
            "ERROR", e);
        return policyDefault;
    }

    return std::get<std::string>(property);
}

void Manager::refreshSelPolicy()
{
    try
    {
        cachedSelPolicy = getSelPolicy();
    }
    catch (const std::exception& e)
    {
        lg2::error("Failed to refresh SEL policy: {ERROR}", "ERROR", e);
    }
}

void Manager::setupSelPolicyMatch()
{
    using namespace sdbusplus::bus::match::rules;
    selPolicyChangedCallback = std::make_unique<sdbusplus::bus::match_t>(
        busLog,
        propertiesChanged("/xyz/openbmc_project/logging/settings",
                          "xyz.openbmc_project.Logging.Settings"),
        std::bind_front(&Manager::onSelPolicyChanged, this));
}

Manager::Manager(sdbusplus::bus_t& bus, const char* objPath) :
    details::ServerObject<details::ManagerIface>(bus, objPath), busLog(bus),
    entryId(0), fwVersion(readFWVersion()),
    event(sdeventplus::Event::get_default())
{
    if constexpr (REDUNDANT_BMC)
    {
        bmcPosMgr = std::make_unique<BMCPosMgr>();
    }

    try
    {
        cachedSelPolicy = getSelPolicy();
    }
    catch (const std::exception& e)
    {
        lg2::debug("Failed to query initial SEL policy: {ERROR}, "
                   "defaulting to Circular",
                   "ERROR", e);
        cachedSelPolicy = policyDefault;
    }

    try
    {
        setupSelPolicyMatch();
    }
    catch (const std::exception& e)
    {
        lg2::debug("Failed to register SEL policy PropertiesChanged "
                   "match: {ERROR}",
                   "ERROR", e);
    }
}

void Manager::onSelPolicyChanged(sdbusplus::message_t& msg)
{
    using Interface = std::string;
    using Property = std::string;
    using PropertyValue = std::variant<std::string, bool>;
    using Properties = std::map<Property, PropertyValue>;

    Interface interface;
    Properties properties;
    std::vector<Property> invalidated;

    msg.read(interface, properties, invalidated);

    if (interface != policyInterface)
    {
        return;
    }

    const auto policyIt = properties.find("SelPolicy");
    if (policyIt != properties.end())
    {
        if (const auto* value = std::get_if<std::string>(&policyIt->second))
        {
            cachedSelPolicy = *value;
            return;
        }
    }

    if (std::ranges::find(invalidated, "SelPolicy") != invalidated.end())
    {
        refreshSelPolicy();
    }
}

auto Manager::createEntry(std::string errMsg, Entry::Level errLvl,
                          std::map<std::string, std::string> additionalData,
                          const FFDCEntries& ffdc) -> sdbusplus::object_path
{
    if (!Extensions::disableDefaultLogCaps())
    {
        if (cachedSelPolicy == policyLinear)
        {
            if (errLvl < Entry::sevLowerLimit)
            {
                if (realErrors.size() >= ERROR_CAP)
                {
                    lg2::info(
                        "Linear SEL: error capacity reached ({ERROR_CAP}), "
                        "rejecting new event",
                        "ERROR_CAP", ERROR_CAP);
                    return {};
                }
            }
            else
            {
                if (infoErrors.size() >= ERROR_INFO_CAP)
                {
                    lg2::info(
                        "Linear SEL: info capacity reached ({ERROR_CAP}), "
                        "rejecting new event",
                        "ERROR_CAP", ERROR_INFO_CAP);
                    return {};
                }
            }
        }
        else
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
    }

    if constexpr (REDUNDANT_BMC)
    {
        if (!bmcPosMgr->isPositionValid())
        {
            // In case position is now available check again.
            bmcPosMgr->readBMCPosition();

            if (bmcPosMgr->isPositionValid())
            {
                // Find last ID used of this new position.
                entryId = 0;
                for (auto id : std::views::keys(entries))
                {
                    if (bmcPosMgr->idContainsCurrentPosition(id))
                    {
                        entryId = std::max(entryId, id);
                    }
                }
            }
        }

        // Fold the position into the ID
        entryId = bmcPosMgr->processEntryId(entryId);
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
    auto additionalDataVec = util::additional_data::combine(additionalData);
    processMetadata(errMsg, additionalDataVec, objects);

    auto e = std::make_unique<Entry>(
        busLog, objPath, entryId,
        ms, // Milliseconds since 1970
        errLvl, std::move(errMsg), std::move(additionalData),
        std::move(objects), fwVersion, getEntrySerializePath(entryId), *this);

    serialize(*e);
    serializeJSON(*e);

    if (isQuiesceOnErrorEnabled() && (errLvl < Entry::sevLowerLimit) &&
        isCalloutPresent(*e))
    {
        quiesceOnError(entryId);
    }

    // Add entry before calling the extensions so that they have access to it
    entries.insert(std::make_pair(entryId, std::move(e)));

    doExtensionLogCreate(*entries.find(entryId)->second, ffdc);

    // Note: No need to close the file descriptors in the FFDC.

    return objPath;
}

auto Manager::createFromEvent(
    sdbusplus::exception::generated_event_base&& event)
    -> sdbusplus::object_path
{
    auto [msg, level, data] = lg2::details::extractEvent(std::move(event));
    return this->createEntry(msg, level, std::move(data));
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
        return false;
    }

    return std::get<bool>(property);
}

bool Manager::isCalloutPresent(const Entry& entry)
{
    for (const auto& c : std::views::keys(entry.additionalData()))
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
    using Host = sdbusplus::server::xyz::openbmc_project::state::Host;

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

    auto blockPath =
        std::string(OBJ_LOGGING) + "/block" + std::to_string(entryId);
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

size_t Manager::eraseAll()
{
    std::vector<uint32_t> logIDWithHwIsolation;
    for (auto& func : Extensions::getLogIDWithHwIsolationFunctions())
    {
        try
        {
            func(logIDWithHwIsolation);
        }
        catch (const std::exception& e)
        {
            lg2::error("An extension's LogIDWithHwIsolation function threw an "
                       "exception: {ERROR}",
                       "ERROR", e);
        }
    }
    size_t entriesSize = entries.size();
    auto iter = entries.begin();
    if (logIDWithHwIsolation.empty())
    {
        while (iter != entries.end())
        {
            auto e = iter->first;
            ++iter;
            erase(e);
        }
        entryId = 0;
    }
    else
    {
        while (iter != entries.end())
        {
            auto e = iter->first;
            ++iter;
            try
            {
                if (!std::ranges::contains(logIDWithHwIsolation, e))
                {
                    erase(e);
                }
                else
                {
                    entriesSize--;
                }
            }
            catch (const sdbusplus::xyz::openbmc_project::Common::Error::
                       Unavailable& e)
            {
                entriesSize--;
            }
        }
        if (!entries.empty())
        {
            entryId = std::ranges::max_element(entries, [](const auto& a,
                                                           const auto& b) {
                          return a.first < b.first;
                      })->first;
        }
        else
        {
            entryId = 0;
        }
    }
    return entriesSize;
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
                    throw sdbusplus::xyz::openbmc_project::Common::Error::
                        Unavailable();
                }
            }
            catch (const sdbusplus::xyz::openbmc_project::Common::Error::
                       Unavailable& e)
            {
                throw;
            }
            catch (const std::exception& e)
            {
                lg2::error("An extension's deleteProhibited function threw an "
                           "exception: {ERROR}",
                           "ERROR", e);
            }
        }

        // Delete the persistent representation of this error.
        fs::path errorPath(paths::error());
        errorPath /= std::to_string(entryId);
        fs::remove(errorPath);

        fs::path jsonPath(paths::error_json());
        jsonPath /= std::to_string(entryId) + ".json";
        fs::remove(jsonPath);

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

    fs::path dir(paths::error());
    fs::path jsondir(paths::error_json());

    if (!fs::exists(dir) && !fs::exists(jsondir))
    {
        return;
    }

    // The directory location might have JSON or Cereal serialized logs (or
    // both).  Use a map to deduplicate to just the individual event IDs and
    // then deserialize them.
    std::map<uint32_t, std::filesystem::path> files{};
    // Prioritize for JSON first.
    for (auto& file : fs::directory_iterator(jsondir))
    {
        auto id = file.path().filename().string();
        if (!id.ends_with(".json"))
        {
            continue;
        }

        uint32_t idNum = std::stoul(id.substr(0, id.size() - 5));
        files.try_emplace(idNum, file.path());
    }
    // Look for Cereal.
    for (auto& file : fs::directory_iterator(dir))
    {
        auto id = file.path().filename().string();
        uint32_t idNum = std::stoul(id);

        files.try_emplace(idNum, file.path());
    }

    for (const auto& [idNum, filePath] : files)
    {
        auto e = std::make_unique<Entry>(
            busLog, std::string(OBJ_ENTRY) + '/' + std::to_string(idNum), idNum,
            *this);

        if (filePath.extension() == ".json")
        {
            deserializeJSON(filePath, *e);
            e->path(filePath, true);
        }
        else
        {
            deserialize(filePath, *e);

            // If we got here, either we didn't record the file in JSON
            // previously, such as due to an upgrade, or it is corrupted.
            // Rewrite the JSON now and ensure the path is adjusted.
            auto jsonPath = serializeJSON(*e);
            e->path(jsonPath, true);
        }

        // Sanity check for proper deserialization.
        if (!sanity(static_cast<uint32_t>(idNum), e->id()))
        {
            lg2::error(
                "Unable to find or parse error entry {ID_NUM}/?{ENTRY_ID}: {PATH}",
                "ID_NUM", idNum, "ENTRY_ID", e->id(), "PATH", filePath);
            continue;
        }

        // Add the event to the appropriate queue.
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

    if constexpr (!REDUNDANT_BMC)
    {
        if (!entries.empty())
        {
            entryId = entries.rbegin()->first;
        }
    }
    else
    {
        // Find the largest ID just from this BMC's entries.
        entryId = 0;
        for (auto id : std::views::keys(entries))
        {
            if (bmcPosMgr->idContainsCurrentPosition(id))
            {
                entryId = std::max(entryId, id);
            }
        }
        lg2::debug("Last entry ID for this BMC is {ID}", "ID", entryId);
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

auto Manager::create(const std::string& message, Entry::Level severity,
                     const std::map<std::string, std::string>& additionalData,
                     const FFDCEntries& ffdc) -> sdbusplus::object_path
{
    return createEntry(message, severity, additionalData, ffdc);
}

void Manager::setupErrorFileWatch()
{
    auto errDir = paths::error();

    // In the redundant BMC sync flow, files are written to a temporary path
    // first and moved into place only after the write completes. Using
    // IN_MOVED_TO ensures we react only when the finalized file appears in the
    // target directory.
    //
    // IN_DELETE handles synced file removals so the corresponding in-memory
    // event log entry is also removed.
    uint32_t mask = IN_MOVED_TO | IN_DELETE;

    if (!util::setupInotifyWatch(errDir, mask, errDirInotifyFD,
                                 errDirWatcherWD))
    {
        abort();
    }

    errorFileWatchEventSource = std::make_unique<sdeventplus::source::IO>(
        event, errDirInotifyFD, EPOLLIN,
        std::bind_front(&Manager::errorFileChanged, this));
}

void Manager::errorFileChanged(sdeventplus::source::IO&, int, uint32_t revents)
{
    if (!(revents & EPOLLIN))
    {
        return;
    }

    // As per inotify(7), sizeof(struct inotify_event) + NAME_MAX + 1 is
    // sufficient for one worst-case event. Keep a larger buffer so one read
    // can drain multiple queued events. this size allows up to 240
    // worst-case events in a single read.
    std::array<uint8_t, 272 * 240> buf{};
    const auto bytesRead = read(errDirInotifyFD, buf.data(), buf.size());
    if (bytesRead < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return;
        }

        lg2::error("read(inotify errors) failed errno {ERRNO}", "ERRNO", errno);
        return;
    }

    const auto totalBytes = static_cast<size_t>(bytesRead);
    size_t offset = 0;

    while (offset < totalBytes)
    {
        auto* ev = reinterpret_cast<inotify_event*>(&buf[offset]);

        if (ev->len)
        {
            try
            {
                const auto idNum =
                    static_cast<uint32_t>(std::stoul(ev->name, nullptr, 10));

                if (ev->mask & IN_MOVED_TO)
                {
                    if (entries.contains(idNum))
                    {
                        if (!refreshFromDisk(idNum))
                        {
                            lg2::error("Failed to refresh entry {ID} from disk",
                                       "ID", idNum);
                        }
                    }
                    else
                    {
                        if (!restoreFromDisk(idNum))
                        {
                            lg2::error("Failed to restore entry {ID} from disk",
                                       "ID", idNum);
                        }
                    }
                }
                else if (ev->mask & IN_DELETE)
                {
                    if (entries.contains(idNum))
                    {
                        erase(idNum);
                    }
                }
            }
            catch (const std::exception& e)
            {
                lg2::error(
                    "Could not parse error entry ID from filename {NAME}",
                    "NAME", ev->name);
            }
        }

        offset += offsetof(inotify_event, name) + ev->len;
    }
}

bool Manager::restoreFromDisk(uint32_t id)
{
    const fs::path path = paths::error() / std::to_string(id);

    std::error_code ec;
    if (!fs::is_regular_file(path, ec))
    {
        return false;
    }

    const std::string objPath =
        std::string(OBJ_ENTRY) + "/" + std::to_string(id);

    auto entry = std::make_unique<Entry>(busLog, objPath, id, *this);

    if (!deserialize(path, *entry))
    {
        lg2::error("Failed to deserialize entry {ID} from {PATH}", "ID", id,
                   "PATH", path);
        return false;
    }

    if (entry->id() != id)
    {
        lg2::error(
            "Sanity check failed while restoring entry EXPECTED_ID={EXPECTED_ID} "
            "RESTORED_ID={RESTORED_ID}",
            "EXPECTED_ID", id, "RESTORED_ID", entry->id());
        return false;
    }

    entry->path(path, true);

    auto [it, inserted] = entries.emplace(id, std::move(entry));

    if (it->second->severity() >= Entry::sevLowerLimit)
    {
        infoErrors.push_back(id);
    }
    else
    {
        realErrors.push_back(id);
    }

    it->second->emit_object_added();

    for (auto& func : Extensions::getExtensionLogAssociationFunctions())
    {
        try
        {
            func(id, objPath);
        }
        catch (const std::exception& e)
        {
            lg2::error(
                "An extension's PEL entry link function threw an exception: {ERROR}",
                "ERROR", e);
        }
    }

    if (bmcPosMgr->idContainsCurrentPosition(id))
    {
        entryId = std::max(entryId, id);
    }

    return true;
}

bool Manager::refreshFromDisk(uint32_t id)
{
    auto it = entries.find(id);
    if (it == entries.end() || !it->second)
    {
        lg2::error("Unknown refreshed entry ID {ID}", "ID", id);
        return false;
    }

    const fs::path path = paths::error() / std::to_string(id);

    std::error_code ec;
    if (!fs::is_regular_file(path, ec))
    {
        return false;
    }

    // Max uint32_t value
    const uint32_t tempId = 0xFFFFFFFF;
    auto tempEntry = std::make_unique<Entry>(
        busLog, std::string(OBJ_ENTRY) + "/" + std::to_string(tempId), tempId,
        *this);

    // Read the persisted entry data from disk into the temporary entry
    if (!deserialize(path, *tempEntry))
    {
        lg2::error("Failed to deserialize entry {ID} from {PATH}", "ID", id,
                   "PATH", path);
        return false;
    }

    // Assign properties from the deserialized temp entry using assignment
    // operator, will only update properties that differ
    auto& existingEntry = it->second;
    *existingEntry = *tempEntry;

    existingEntry->path(path, true);

    return true;
}

} // namespace internal
} // namespace logging
} // namespace phosphor
