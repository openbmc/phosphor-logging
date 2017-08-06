#include <fstream>
#include <iostream>
#include <chrono>
#include <cstdio>
#include <set>
#include <string>
#include <vector>
#include <sdbusplus/vtable.hpp>
#include <systemd/sd-bus.h>
#include <systemd/sd-journal.h>
#include "config.h"
#include "elog_entry.hpp"
#include <phosphor-logging/log.hpp>
#include "log_manager.hpp"
#include "elog_meta.hpp"
#include "elog_serialize.hpp"

using namespace phosphor::logging;
extern const std::map<metadata::Metadata,
                      std::function<metadata::associations::Type>> meta;

namespace phosphor
{
namespace logging
{

void Manager::commit(uint64_t transactionId, std::string errMsg)
{
    constexpr const auto transactionIdVar = "TRANSACTION_ID";
    // Length of 'TRANSACTION_ID' string.
    constexpr const auto transactionIdVarSize = strlen(transactionIdVar);
    // Length of 'TRANSACTION_ID=' string.
    constexpr const auto transactionIdVarOffset = transactionIdVarSize + 1;

    sd_journal *j = nullptr;
    int rc = sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
    if (rc < 0)
    {
        logging::log<logging::level::ERR>("Failed to open journal",
                           logging::entry("DESCRIPTION=%s", strerror(-rc)));
        return;
    }

    std::string transactionIdStr = std::to_string(transactionId);
    std::set<std::string> metalist;
    auto metamap = g_errMetaMap.find(errMsg);
    if (metamap != g_errMetaMap.end())
    {
        metalist.insert(metamap->second.begin(), metamap->second.end());
    }

    std::vector<std::string> additionalData;

    // Read the journal from the end to get the most recent entry first.
    // The result from the sd_journal_get_data() is of the form VARIABLE=value.
    SD_JOURNAL_FOREACH_BACKWARDS(j)
    {
        const char *data = nullptr;
        size_t length = 0;

        // Look for the transaction id metadata variable
        rc = sd_journal_get_data(j, transactionIdVar, (const void **)&data,
                                &length);
        if (rc < 0)
        {
            // This journal entry does not have the TRANSACTION_ID
            // metadata variable.
            continue;
        }

        // journald does not guarantee that sd_journal_get_data() returns NULL
        // terminated strings, so need to specify the size to use to compare,
        // use the returned length instead of anything that relies on NULL
        // terminators like strlen().
        // The data variable is in the form of 'TRANSACTION_ID=1234'. Remove
        // the TRANSACTION_ID characters plus the (=) sign to do the comparison.
        // 'data + transactionIdVarOffset' will be in the form of '1234'.
        // 'length - transactionIdVarOffset' will be the length of '1234'.
        if ((length <= (transactionIdVarOffset)) ||
            (transactionIdStr.compare(0,
                                      transactionIdStr.size(),
                                      data + transactionIdVarOffset,
                                      length - transactionIdVarOffset) != 0))
        {
            // The value of the TRANSACTION_ID metadata is not the requested
            // transaction id number.
            continue;
        }

        // Search for all metadata variables in the current journal entry.
        for (auto i = metalist.cbegin(); i != metalist.cend();)
        {
            rc = sd_journal_get_data(j, (*i).c_str(),
                                    (const void **)&data, &length);
            if (rc < 0)
            {
                // Metadata variable not found, check next metadata variable.
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
            logging::log<logging::level::INFO>("Failed to find metadata",
                    logging::entry("META_FIELD=%s", metaVarStr.c_str()));
        }
    }

    sd_journal_close(j);

    // Create error Entry dbus object
    entryId++;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
    auto objPath =  std::string(OBJ_ENTRY) + '/' +
            std::to_string(entryId);

    AssociationList objects {};
    processMetadata(errMsg, additionalData, objects);

    level reqLevel = level::ERR; // Default to ERR
    auto levelmap = g_errLevelMap.find(errMsg);
    if (levelmap != g_errLevelMap.end())
    {
        reqLevel = levelmap->second;
    }
    auto e = std::make_unique<Entry>(
                 busLog,
                 objPath,
                 entryId,
                 ms, // Milliseconds since 1970
                 static_cast<Entry::Level>(reqLevel),
                 std::move(errMsg),
                 std::move(additionalData),
                 std::move(objects),
                 *this);
    serialize(*e);
    entries.insert(std::make_pair(entryId, std::move(e)));

    //Check Entry Cap value and erase old entries
    checkEntryCapAndEraseEntries();
}

void Manager::processMetadata(const std::string& errorName,
                              const std::vector<std::string>& additionalData,
                              AssociationList& objects) const
{
    // additionalData is a list of "metadata=value"
    constexpr auto separator = '=';
    for(const auto& entry: additionalData)
    {
        auto found = entry.find(separator);
        if(std::string::npos != found)
        {
            auto metadata = entry.substr(0, found);
            auto iter = meta.find(metadata);
            if(meta.end() != iter)
            {
                (iter->second)(metadata, additionalData, objects);
            }
        }
    }
}

void Manager::erase(uint32_t entryId)
{
    auto entry = entries.find(entryId);
    auto id = entry->second->id();
    if(entries.end() != entry)
    {
        // Delete the persistent representation of this error.
        fs::path errorPath(ERRLOG_PERSIST_PATH);
        errorPath /= std::to_string(id);
        fs::remove(errorPath);

        entries.erase(entry);
    }
}

void Manager::restore()
{
    std::vector<uint32_t> errorIds;

    fs::path dir(ERRLOG_PERSIST_PATH);
    if (!fs::exists(dir) || fs::is_empty(dir))
    {
        return;
    }

    for(auto& file: fs::directory_iterator(dir))
    {
        auto id = file.path().filename().c_str();
        auto idNum = std::stol(id);
        auto e = std::make_unique<Entry>(
                     busLog,
                     std::string(OBJ_ENTRY) + '/' + id,
                     idNum,
                     *this);
        if (deserialize(file.path(), *e))
        {
            e->emit_object_added();
            entries.insert(std::make_pair(idNum, std::move(e)));
            errorIds.push_back(idNum);
        }
    }

    entryId = *(std::max_element(errorIds.begin(), errorIds.end()));
}

void Manager::checkEntryCapAndEraseEntries()
{
    constexpr auto ENTRY_CAP_PATH    = "/xyz/openbmc_project/control/host0/entry_cap";
    constexpr auto ENTRY_CAP_INTERFACE = "xyz.openbmc_project.Control.Entry.Cap";
    constexpr auto PROP_INTF = "org.freedesktop.DBus.Properties";
    constexpr auto ENTRY_CAP_PROP = "EntryCap";
    try
    {
        auto service = getService(ENTRY_CAP_PATH, ENTRY_CAP_INTERFACE);
        auto method = busLog.new_method_call(service.c_str(),
                                                ENTRY_CAP_PATH,
                                                PROP_INTF,
                                                "Get");

        method.append(ENTRY_CAP_INTERFACE, ENTRY_CAP_PROP);
        auto reply = busLog.call(method);
        if (reply.is_method_error())
        {
            log<level::ERR>("Error in getting EntryCap property");
            return;
        }
        sdbusplus::message::variant<uint32_t> pcap;
        reply.read(pcap);
        auto entryCap = pcap.get<uint32_t>();

        //If EntryCap value is reached clear the first entry
        if (static_cast<uint32_t>(entries.size()) > entryCap)
        {
            uint32_t count = static_cast<uint32_t>(entries.size()) - entryCap;
            for (uint32_t i = 0; i < count; ++i)
            {
                log<level::INFO>("Entry cap reached. Deleting oldest entry",
                    entry("ENTRY_CAP=%d", entryCap),
                    entry("ENTRY_DELETED=%d", entries.begin()->first));
                erase(entries.begin()->first);
           }
        }
    }
    catch(std::runtime_error& ex)
    {
        log<level::ERR>(ex.what());
    }
}

void Manager::entryCapChanged(sdbusplus::message::message& msg)
{
    log<level::INFO>("Callback entryCapChanged for change in EntryCap");
    checkEntryCapAndEraseEntries();
}

std::string Manager::getService(std::string path, std::string intf)
{
    // Mapper dbus constructs
    auto mapperCall = busLog.new_method_call("xyz.openbmc_project.ObjectMapper",
                                          "/xyz/openbmc_project/object_mapper",
                                          "xyz.openbmc_project.ObjectMapper",
                                          "GetObject");
    mapperCall.append(path);
    mapperCall.append(std::vector<std::string>({intf}));

    auto mapperResponseMsg = busLog.call(mapperCall);
    if (mapperResponseMsg.is_method_error())
    {
        throw std::runtime_error("ERROR in mapper call");
    }

    std::map<std::string, std::vector<std::string>> mapperResponse;
    mapperResponseMsg.read(mapperResponse);
    if (mapperResponse.begin() == mapperResponse.end())
    {
        throw std::runtime_error("ERROR in reading the mapper response");
    }

    return mapperResponse.begin()->first;
}

} // namespace logging
} // namepsace phosphor
