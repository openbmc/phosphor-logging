#include <fstream>
#include <iostream>
#include <stdio.h>
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
 * @return rc
 */
auto commit(sd_bus_message *msg, void *user_data, sd_bus_error *error)
{
    // TODO Change /tmp path to a permanent location on flash
    constexpr const char *path = "/tmp/elog";
    constexpr const char *msg_id_str = "_PID";
    const char* meta_var;
    int rc = -1;
    char *msg_id = NULL;
    std::string filename;
    std::vector<const char*> meta_list;
    sd_journal *j;

    // Read message id
    rc = sd_bus_message_read(msg, "s", &msg_id);
    if (rc < 0)
    {
        logging::log<logging::level::ERR>("Failed to read msg id",
                           logging::entry("DESCRIPTION=%s", strerror(-rc)));
        return sd_bus_reply_method_return(msg, "i", rc);
    }

    // Create log file
    filename.append(path);
    // TODO Create error logs in their own separate dir once permanent location
    // on flash is determined. Ex: ../msg_id/1
    filename.append(msg_id);
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
        return sd_bus_reply_method_return(msg, "i", rc);
    }
    while ((rc = sd_bus_message_read_basic(msg, 's', &meta_var)) > 0)
    {
        meta_list.push_back(meta_var);
    }
    sd_bus_message_exit_container(msg);

    rc = sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
    if (rc < 0)
    {
        logging::log<logging::level::ERR>("Failed to open journal",
                           logging::entry("DESCRIPTION=%s", strerror(-rc)));
        return sd_bus_reply_method_return(msg, "i", rc);
    }

    // Read the journal from the end to get the most recent entry first.
    // The result from the sd_journal_get_data() is of the form VARIABLE=value.
    SD_JOURNAL_FOREACH_BACKWARDS(j)
    {
        const char *d = NULL;
        size_t l = 0;

        // Search for the msg id
        rc = sd_journal_get_data(j, msg_id_str, (const void **)&d, &l);
        if (rc < 0)
        {
            // Instance not found, continue to next journal entry
            continue;
        }
        std::string result(d);
        if (result.find(msg_id) == std::string::npos)
        {
            // Match not found, continue to next journal entry
            continue;
        }

        // Match found, write to file
        // TODO This is a draft format based on the redfish event logs written
        // in json, the final openbmc format is to be determined
        efile << "\t{" << std::endl;
        efile << "\t\"@" << d << "\"," << std::endl;

        // Include the journal message
        rc = sd_journal_get_data(j, "MESSAGE", (const void **)&d, &l);
        if (rc < 0)
        {
            continue;
        }
        efile << "\t\"@" << d << "\"," << std::endl;

        // Search for the metadata variables in the current journal entry
        // and stop once all the metadata fields have been found
        // TODO Implement once this function reads the metadata fields from
        // the header file
    }
    sd_journal_close(j);

    efile << "}" << std::endl;
    efile.close();
    return sd_bus_reply_method_return(msg, "i", rc);
}

constexpr sdbusplus::vtable::vtable_t log_vtable[] =
{
    sdbusplus::vtable::start(),
    sdbusplus::vtable::method("Commit", "sas", "i", commit),
    sdbusplus::vtable::end()
};

int main(int argc, char *argv[])
{
    constexpr const auto *dbus_log_obj = "/xyz/openbmc_project/log";
    constexpr const auto *dbus_log_name = "xyz.openbmc_project.log";
    int rc = -1;
    sd_bus *bus = nullptr;
    sd_bus_slot *slot = nullptr;

    rc = sd_bus_open_system(&bus);
    if (rc < 0)
    {
        logging::log<logging::level::ERR>("Failed to open system bus",
                           logging::entry("DESCRIPTION=%s", strerror(-rc)));
        goto cleanup;
    }

    rc = sd_bus_add_object_manager(bus, nullptr, dbus_log_obj);
    if (rc < 0)
    {
        logging::log<logging::level::ERR>("Failed to add object mgr",
                           logging::entry("DESCRIPTION=%s", strerror(-rc)));
        goto cleanup;
    }

    rc = sd_bus_add_object_vtable(bus,
                                  &slot,
                                  dbus_log_obj,
                                  dbus_log_name,
                                  log_vtable,
                                  nullptr);
    if (rc < 0)
    {
        logging::log<logging::level::ERR>("Failed to add vtable",
                           logging::entry("DESCRIPTION=%s", strerror(-rc)));
        goto cleanup;
    }

    rc = sd_bus_request_name(bus, dbus_log_name, 0);
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
    slot = sd_bus_slot_unref(slot);
    sd_bus_unref(bus);

    return rc;
}

