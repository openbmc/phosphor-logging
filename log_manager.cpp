#include <stdio.h>
#include <sdbusplus/vtable.hpp>
#include <systemd/sd-bus.h>
#include <systemd/sd-journal.h>

/*
 * @fn get_journal_data()
 * @brief Get the most recent metadata value from a specified field name.
 *        The returned data is prefixed with the specified field name and '='.
 * @param[i] msg - dbus message
 * @param[i] user_data - user data
 * @param[i] error - dbus error
 * @return Metadata field and value string
 */
auto get_journal_data(sd_bus_message *msg,
                            void *user_data,
                            sd_bus_error *error)
{
    int rc = -1;
    char *var = NULL;
    char reply[64*1024]; // sd_journal_get_data() truncates data at 64K
    sd_journal *j;

    reply[0] = '\0';

    // Metatdata field name the caller wants to find
    rc = sd_bus_message_read(msg, "s", &var);
    if (rc < 0)
    {
        fprintf(stderr, "Failed to read msg data: %s\n", strerror(-rc));
        return sd_bus_reply_method_return(msg, "i", rc);
    }

    rc = sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
    if (rc < 0)
    {
        fprintf(stderr, "Failed to open journal: %s\n", strerror(-rc));
        return sd_bus_reply_method_return(msg, "i", rc);
    }

    // Find the most recent instance by reading each journal entry starting
    // with the end of the journal
    SD_JOURNAL_FOREACH_BACKWARDS(j)
    {
        const char *d = NULL;
        size_t l = 0;

        rc = sd_journal_get_data(j, var, (const void **)&d, &l);
        if (rc < 0)
        {
            // Instance not found, continue to next journal entry
            continue;
        }
        // Copy match to reply variable since journal fd will be closed
        snprintf(reply, l, "%s", d);
        break;
    }
    sd_journal_close(j);

    // No need to check that reply has been populated, if no match was found
    // then rc is set to -2
    return sd_bus_reply_method_return(msg, "is", rc, reply);
}

/*
 * @fn commit()
 * @brief Create an error/event log based on a message id
 * @param[i] msg - dbus message
 * @param[i] user_data - user data
 * @param[i] error - dbus error
 * @return Commit id
 */
auto commit(sd_bus_message *msg, void *user_data, sd_bus_error *error)
{
    int rc = 0;

    return rc;
}

constexpr sdbusplus::vtable::vtable_t log_vtable[] =
{
    sdbusplus::vtable::start(),
    sdbusplus::vtable::method("Commit", "i", "i", commit),
    sdbusplus::vtable::method("GetJournalData", "s", "is", get_journal_data),
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

