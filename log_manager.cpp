#include <fstream>
#include <iostream>
#include <chrono>
#include <cstdio>
#include <string>
#include <vector>
#include <sdbusplus/vtable.hpp>
#include <systemd/sd-bus.h>
#include <systemd/sd-journal.h>
#include "elog-lookup.cpp"
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

    sd_journal *j = nullptr;
    int rc = sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
    if (rc < 0)
    {
        logging::log<logging::level::ERR>("Failed to open journal",
                           logging::entry("DESCRIPTION=%s", strerror(-rc)));
        return;
    }

    std::string transactionIdStr = std::to_string(transactionId);
    auto& metalist = g_errMetaMap[errMsg];
    const auto& metalistHostEvent = g_errMetaMapHostEvent[errMsg];
    std::vector<std::string> additionalData;

    // TODO Remove once host event error header file is auto-generated.
    // Also make metalist a const variable.
    // Tracking with issue openbmc/phosphor-logging#4
    for (auto& metaVarStrHostEvent : metalistHostEvent)
    {
        metalist.push_back(metaVarStrHostEvent);
    }

    // Search for each metadata variable in the journal.
    for (auto& metaVarStr : metalist)
    {
        const char *metadata = nullptr;

        // Read the journal from the end to get the most recent entry first.
        // The result from the sd_journal_get_data() is of the form VARIABLE=value.
        SD_JOURNAL_FOREACH_BACKWARDS(j)
        {
            const char *data = nullptr;
            size_t length = 0;
            metadata = nullptr;

            // Search for the metadata variables in the current journal entry
            rc = sd_journal_get_data(j, metaVarStr.c_str(),
                                    (const void **)&metadata, &length);
            if (rc < 0)
            {
                // Metadata value not found, continue to next journal entry.
                continue;
            }

            // Look for the transaction id metadata variable
            rc = sd_journal_get_data(j, transactionIdVar, (const void **)&data,
                                    &length);
            if (rc < 0)
            {
                // This journal entry does not have the transaction id,
                // continue to next entry
                continue;
            }

            std::string result(data);
            if (result.find(transactionIdStr) == std::string::npos)
            {
                // Requested transaction id  not found,
                // continue to next journal entry.
                continue;
            }

            // Metadata matching the transaction id found, save it
            // and break out of the journal search loop
            additionalData.push_back(std::string(metadata));
            break;
        }
        if (!metadata)
        {
            // Metadata value not found in the journal.
            logging::log<logging::level::INFO>("Failed to find metadata",
                    logging::entry("META_FIELD=%s", metaVarStr.c_str()));
            continue;
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
    entries.insert(std::make_pair(entryId, std::make_unique<Entry>(
            busLog,
            objPath,
            entryId,
            ms, // Milliseconds since 1970
            (Entry::Level)g_errLevelMap[errMsg],
            std::move(errMsg),
            std::move(additionalData),
            std::move(objects))));
    return;
}

} // namespace logging
} // namepsace phosphor
