#include "config.h"
#include "log_manager.hpp"
#include <sdbusplus/bus.hpp>
#include <cstdlib>
#include <iostream>
#include <exception>

int main(int argc, char *argv[])
{
    try {
        auto manager = phosphor::logging::Manager(
                sdbusplus::bus::new_system(),
                BUSNAME_INTERNAL,
                OBJ_INTERNAL);

        auto bus = sdbusplus::bus::new_default();

        // Add sdbusplus ObjectManager.
        sdbusplus::server::manager::manager(bus, OBJ_ENTRY);

        bus.request_name(BUSNAME_ENTRY);

        // TODO Create error log dbus object on demand, when the Commit interface
        // creates an error log it'd call this entry interface to create an object.

        manager.run();

        exit(EXIT_SUCCESS);
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    exit(EXIT_FAILURE);
}
