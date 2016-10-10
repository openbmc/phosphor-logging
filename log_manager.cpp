#include <stdio.h>
#include <sdbusplus/vtable.hpp>
#include <systemd/sd-bus.h>

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
                fprintf(stderr, "Failed to process bus: %s\n", strerror(-rc));
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

