#include <iostream>
#include "config.h"
#include "elog_entry.hpp"

int main(int argc, char *argv[])
{
    auto bus = sdbusplus::bus::new_default();

    // Add sdbusplus ObjectManager.
    sdbusplus::server::manager::manager(bus, OBJ);

    bus.request_name(BUSNAME);

    // TODO Create error log dbus object on demand, when the Commit interface
    // creates an error log it'd call this entry interface to create an object.

    while(true)
    {
        bus.process_discard();
        bus.wait();
    }
    return 0;
}
