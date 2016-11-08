#include <fstream>
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <sdbusplus/vtable.hpp>
#include <systemd/sd-bus.h>
#include <systemd/sd-journal.h>
#include "log.hpp"

using namespace phosphor;

/*
 * @fn commit()
 * @brief Create an error/event log based on specified id and metadata variable
 *        names that includes the journal message and the metadata values.
 */
auto commit(sd_bus_message *msg, void *userdata, sd_bus_error *error)
{
    // TODO Change /tmp path to a permanent location on flash
    constexpr const auto path = "/tmp/elog";
    constexpr const auto msgIdStr = "_PID";

    // Read PID
    int rc = -1;
    char *msgId = nullptr;
    rc = sd_bus_message_read(msg, "s", &msgId);
    if (rc < 0)
    {
        logging::log<logging::level::ERR>("Failed to read msg id",
                           logging::entry("DESCRIPTION=%s", strerror(-rc)));
        return sd_bus_reply_method_return(msg, "i", rc);
    }

    // Create log file
    std::string filename{};
    filename.append(path);
    // TODO Create error logs in their own separate dir once permanent location
    // on flash is determined. Ex: ../msgId/1
    filename.append(msgId);
    std::ofstream efile;
    efile.open(filename);
    efile << "{" << std::endl;

    // Read metadata variables passed as array of strings and store in vector
    // TODO Read required metadata fields from header file instead
    rc = sd_bus_message_enter_container(msg, SD_BUS_TYPE_ARRAY, "s");
    if (rc < 0)
    {
        logging::log<logging::level::ERR>("Failed to read metadata vars",
                           logging::entry("DESCRIPTION=%s", strerror(-rc)));
        return sd_bus_reply_method_return(msg, nullptr);
    }
    const char* metaVar = nullptr;
    std::vector<const char*> metaList;
    while ((rc = sd_bus_message_read_basic(msg, 's', &metaVar)) > 0)
    {
        metaList.push_back(metaVar);
    }
    sd_bus_message_exit_container(msg);

    sd_journal *j = nullptr;
    rc = sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
    if (rc < 0)
    {
        logging::log<logging::level::ERR>("Failed to open journal",
                           logging::entry("DESCRIPTION=%s", strerror(-rc)));
        return sd_bus_reply_method_return(msg, nullptr);
    }

    // Read the journal from the end to get the most recent entry first.
    // The result from the sd_journal_get_data() is of the form VARIABLE=value.
    SD_JOURNAL_FOREACH_BACKWARDS(j)
    {
        const char *data = nullptr;
        size_t length = 0;

        // Search for the msg id
        rc = sd_journal_get_data(j, msgIdStr, (const void **)&data, &length);
        if (rc < 0)
        {
            // Instance not found, continue to next journal entry
            continue;
        }
        std::string result(data);
        if (result.find(msgId) == std::string::npos)
        {
            // Match not found, continue to next journal entry
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
        efile << "\t\"@" << data << "\"," << std::endl;

        // Search for the metadata variables in the current journal entry
        for (auto i : metaList)
        {
            rc = sd_journal_get_data(j, i, (const void **)&data, &length);
            if (rc < 0)
            {
                // Not found, continue to next metadata variable
                logging::log<logging::level::INFO>("Failed to find metadata",
                                    logging::entry("META_FIELD=%s", i));
                continue;
            }

            // Metatdata variable found, write to file
            efile << "\t\"@" << data << "\"," << std::endl;
        }
        efile << "\t}" << std::endl;

        // TODO Break only once all metadata fields have been found. Implement
        // once this function reads the metadata fields from the header file.
        break;
    }
    sd_journal_close(j);

    efile << "}" << std::endl;
    efile.close();
    return sd_bus_reply_method_return(msg, nullptr);
}

constexpr sdbusplus::vtable::vtable_t log_vtable[] =
{
    sdbusplus::vtable::start(),
    sdbusplus::vtable::method("Commit", "sas", "", commit),
    sdbusplus::vtable::end()
};

int main(int argc, char *argv[])
{
    constexpr const auto dbusLogObj = "/xyz/openbmc_project/Logging";
    constexpr const auto dbusLogName = "xyz.openbmc_project.Logging";
    int rc = -1;
    sd_bus *bus = nullptr;

    rc = sd_bus_open_system(&bus);
    if (rc < 0)
    {
        logging::log<logging::level::ERR>("Failed to open system bus",
                           logging::entry("DESCRIPTION=%s", strerror(-rc)));
        goto cleanup;
    }

    rc = sd_bus_add_object_manager(bus, nullptr, dbusLogObj);
    if (rc < 0)
    {
        logging::log<logging::level::ERR>("Failed to add object mgr",
                           logging::entry("DESCRIPTION=%s", strerror(-rc)));
        goto cleanup;
    }

    rc = sd_bus_add_object_vtable(bus,
                                  nullptr,
                                  dbusLogObj,
                                  dbusLogName,
                                  log_vtable,
                                  nullptr);
    if (rc < 0)
    {
        logging::log<logging::level::ERR>("Failed to add vtable",
                           logging::entry("DESCRIPTION=%s", strerror(-rc)));
        goto cleanup;
    }

    rc = sd_bus_request_name(bus, dbusLogName, 0);
    if (rc < 0)
    {
        logging::log<logging::level::ERR>("Failed to acquire service name",
                           logging::entry("DESCRIPTION=%s", strerror(-rc)));
    }
    else
    {
        for(;;)
        {
            rc = sd_bus_process(bus, nullptr);
            if (rc < 0)
            {
                logging::log<logging::level::ERR>("Failed to connect to bus",
                           logging::entry("DESCRIPTION=%s", strerror(-rc)));
                break;
            }
            if (rc > 0)
            {
                continue;
            }

            rc = sd_bus_wait(bus, (uint64_t) - 1);
            if (rc < 0)
            {
                logging::log<logging::level::ERR>("Failed to wait on bus",
                           logging::entry("DESCRIPTION=%s", strerror(-rc)));
                break;
            }
        }
    }

cleanup:
    sd_bus_unref(bus);

    return rc;
}

