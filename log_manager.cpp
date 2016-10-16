#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <log.hpp>
#include <sdbusplus/vtable.hpp>
#include <systemd/sd-bus.h>
#include <systemd/sd-journal.h>

using namespace phosphor;

/*
 * @fn commit()
 * @brief Create an error/event log based on specified id and metadata variable
 *        names that includes the journal message and the metadata values.
 * @param[i] msg - dbus message
 * @param[i] user_data - user data
 * @param[i] error - dbus error
 * @return rc
 */
auto commit(sd_bus_message *msg, void *user_data, sd_bus_error *error)
{
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
    filename.append(msg_id);
    std::ofstream efile;
    efile.open(filename);
    efile << "{" << std::endl;

    // Read metadata variables passed as array of strings and store in vector
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

    // Read the journal from the end to get the most recent entry first
    SD_JOURNAL_FOREACH_BACKWARDS(j)
    {
        const char *d = NULL;
        size_t l = 0;

        // Search for the id
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
        for (auto i : meta_list)
        {
            rc = sd_journal_get_data(j, i, (const void **)&d, &l);
            if (rc < 0)
            {
                // Not found, continue to next metadata variable
                continue;
            }

            // Metatdata variable found, write to file
            efile << "\t\"@" << d << "\"," << std::endl;
        }
        efile << "\t}" << std::endl;
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
    constexpr const auto *dbus_log_name = "xyz.openbmc_project.Log";
    int rc = -1;
    sd_bus *bus = NULL;
    sd_bus_slot *slot = NULL;

    rc = sd_bus_open_system(&bus);
    if (rc < 0)
    {
        fprintf(stderr, "Failed to open system bus: %s\n", strerror(-rc));
        goto cleanup;
    }

    rc = sd_bus_add_object_manager(bus, NULL, dbus_log_obj);
    if (rc < 0)
    {
        fprintf(stderr, "Failed to add object mgr: %s\n", strerror(-rc));
        goto cleanup;
    }

    rc = sd_bus_add_object_vtable(bus,
                                  &slot,
                                  dbus_log_obj,
                                  dbus_log_name,
                                  log_vtable,
                                  NULL);
    if (rc < 0)
    {
        fprintf(stderr, "Failed to add vtable: %s\n", strerror(-rc));
        goto cleanup;
    }

    rc = sd_bus_request_name(bus, dbus_log_name, 0);
    if (rc < 0)
    {
        fprintf(stderr, "Failed to acquire service name: %s\n", strerror(-rc));
    }
    else
    {
        for(;;)
        {
            rc = sd_bus_process(bus, NULL);
            if (rc < 0)
            {
                fprintf(stderr, "Failed to connect to bus: %s\n", strerror(-rc));
                break;
            }
            if (rc > 0)
            {
                continue;
            }

            rc = sd_bus_wait(bus, (uint64_t) - 1);
            if (rc < 0)
            {
                fprintf(stderr, "Failed to wait on bus: %s\n", strerror(-rc));
                break;
            }
        }
    }

cleanup:
    slot = sd_bus_slot_unref(slot);
    sd_bus_unref(bus);

    return rc;
}

