#include <fstream>
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <sdbusplus/vtable.hpp>
#include <systemd/sd-bus.h>
#include <systemd/sd-journal.h>
#include "elog-lookup.cpp"
#include "config.h"
#include "elog_entry.hpp"
#include "log.hpp"
#include "log_manager.hpp"

namespace phosphor
{
namespace logging
{

void Manager::commit(uint64_t transactionId, std::string errMsg)
{
    // TODO Change /tmp path to a permanent location on flash
    constexpr const auto path = "/tmp/elog";
    constexpr const auto transactionIdVar = "TRANSACTION_ID";

    std::string filename{};
    filename.append(path);
    // TODO Create error logs in their own separate dir once permanent location
    // on flash is determined. Ex: ../transactionId/1
    std::ofstream efile;
    efile.open(filename);
    efile << "{" << std::endl;

    sd_journal *j = nullptr;
    int rc = sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
    if (rc < 0)
    {
        logging::log<logging::level::ERR>("Failed to open journal",
                           logging::entry("DESCRIPTION=%s", strerror(-rc)));
        return;
    }

    std::string transactionIdStr = std::to_string(transactionId);
    std::vector<std::string> metalist = g_errMetaMap[errMsg];

    Entry::Properties properties;

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
        // Match found, write to file
        // TODO This is a draft format based on the redfish event logs written
        // in json, the final openbmc format is to be determined
        efile << "\t{" << std::endl;
        efile << "\t\"@" << data << "\"," << std::endl;

        // Include the journal message
        rc = sd_journal_get_data(j, "MESSAGE", (const void **)&data, &length);
        if (rc < 0)
        {
            continue;
        }

        properties.msg = std::move(data);
        efile << "\t\"@" << data << "\"," << std::endl;

        // Search for the metadata variables in the current journal entry
        for (auto metaVarStr : metalist)
        {
            rc = sd_journal_get_data(j, metaVarStr.c_str(),
                                    (const void **)&data, &length);
            if (rc < 0)
            {
                // Not found, continue to next metadata variable
                logging::log<logging::level::INFO>("Failed to find metadata",
                        logging::entry("META_FIELD=%s", metaVarStr.c_str()));
                continue;
            }

            // Metatdata variable found, write to file
            properties.additionalData.push_back(std::string(data));
            efile << "\t\"@" << data << "\"," << std::endl;
        }
        efile << "\t}" << std::endl;

        // TODO Break only once all metadata fields have been found. Implement
        // once this function reads the metadata fields from the header file.
        break;
    }
    sd_journal_close(j);

    // Create error Entry dbus object
    properties.id = ++entryId;
    properties.severity = (Entry::Level)g_errLevelMap[errMsg];
    auto objPath =  std::string(OBJ_ENTRY) + '/' +
        std::to_string(properties.id);
    entries.insert(std::make_pair(properties.id, std::make_unique<Entry>
        (busLog, objPath, properties)));

    efile << "}" << std::endl;
    efile.close();
    return;
}

} // namespace logging
} // namepsace phosphor
