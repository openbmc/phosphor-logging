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
#include <phosphor-logging/elog-errors-HostEvent.hpp>
#include "config.h"
#include "elog_entry.hpp"
#include <phosphor-logging/log.hpp>
#include "log_manager.hpp"

namespace phosphor
{
namespace logging
{

void Manager::commit(uint64_t transactionId, std::string errMsg)
{
    constexpr const auto transactionIdVar = "TRANSACTION_ID";
    constexpr const auto transactionIdVarSize = strlen(transactionIdVar);

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
    const auto& metalistHostEvent = g_errMetaMapHostEvent[errMsg];
    std::vector<std::string> additionalData;

    // TODO Remove once host event error header file is auto-generated.
    // Also make metalist a const variable.
    // Tracking with issue openbmc/phosphor-logging#4
    for (auto& metaVarStrHostEvent : metalistHostEvent)
    {
        metalist.insert(metaVarStrHostEvent);
    }

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

        // The metadata field result will be TRANSACTION_ID=1234. Skip the
        // TRANSACTION_ID piece and (=) sign to get the id number to compare.
        if (strcmp((data + transactionIdVarSize + 1),
                    transactionIdStr.c_str()) != 0)
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
    level reqLevel = level::INFO; // Default to INFO
    auto levelmap = g_errLevelMap.find(errMsg);
    if (levelmap != g_errLevelMap.end())
    {
        reqLevel = levelmap->second;
    }
    entries.insert(std::make_pair(entryId, std::make_unique<Entry>(
            busLog,
            objPath,
            entryId,
            ms, // Milliseconds since 1970
            (Entry::Level)reqLevel,
            std::move(errMsg),
            std::move(additionalData))));
    return;
}

} // namespace logging
} // namepsace phosphor
