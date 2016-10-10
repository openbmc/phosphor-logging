#include <stdio.h>
#include <sdbusplus/vtable.hpp>
#include <systemd/sd-bus.h>
#include "log.hpp"

/*
 * @fn commit()
 * @brief Create an error/event log based on a message id
 * @param[in] msg - dbus message
 * @param[in] user_data - user data
 * @param[in] error - dbus error
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
    constexpr const auto *dbus_log_obj = "/xyz/openbmc_project/Logging";
    constexpr const auto *dbus_log_name = "xyz.openbmc_project.Logging";
    auto rc = -1;
    sd_bus *bus = nullptr;

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
                                  NULL,
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
    sd_bus_unref(bus);

    return rc;
}

