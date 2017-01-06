#include <iostream>
#include "config.h"
#include "log_manager.hpp"

int main(int argc, char *argv[])
{
    auto busManager = sdbusplus::bus::new_default();
    auto busEntry = sdbusplus::bus::new_default();

    phosphor::logging::Manager manager(busManager, OBJ_INTERNAL);

    // Add sdbusplus ObjectManager.
    sdbusplus::server::manager::manager(busManager, OBJ_INTERNAL);
    sdbusplus::server::manager::manager(busEntry, OBJ_ENTRY);

    busManager.request_name(BUSNAME_INTERNAL);
    busEntry.request_name(BUSNAME_ENTRY);

    // TODO Create error log dbus object on demand, when the Commit interface
    // creates an error log it'd call this entry interface to create an object.

    while(true)
    {
        busManager.process_discard();
        busManager.wait();
    }

    return 0;
}
