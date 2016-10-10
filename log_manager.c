#include <stdio.h>
#include <systemd/sd-bus.h>

/*
 * @fn commit()
 * @brief Create an error/event log
 */
static int commit(sd_bus_message *msg, void *user_data, sd_bus_error *error)
{
    int rc = 0;

    return rc;
}

static const sd_bus_vtable log_vtable[] =
{
    SD_BUS_VTABLE_START(0),
    SD_BUS_METHOD("commit", "", "i", &commit, SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_VTABLE_END
};

int main(int argc, char *argv[])
{
    static const char *dbus_log_obj = "/xyz/openbmc_project/log";
    static const char *dbus_log_name = "xyz.openbmc_project.Log";
    int rc = -1;
    sd_bus *bus = NULL;
    sd_bus_slot *slot = NULL;

    rc = sd_bus_open_system(&bus);
    if (rc < 0)
    {
        fprintf(stderr, "Failed to open system bus: %s\n", strerror(-rc));
        return rc;
    }

    rc = sd_bus_add_object_manager(bus, NULL, dbus_log_obj);
    if (rc < 0)
    {
        fprintf(stderr, "Failed to add object mgr: %s\n", strerror(-rc));
        sd_bus_unref(bus);
        return rc;
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
        sd_bus_slot_unref(slot);
        sd_bus_unref(bus);
        return rc;
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

    sd_bus_slot_unref(slot);
    sd_bus_unref(bus);

    return rc;
}

