#include <sdbusplus/bus.hpp>
#include <sdbusplus/server/manager.hpp>
#include "config.h"
#include "log_manager.hpp"

int main(int argc, char *argv[])
{
    auto bus = sdbusplus::bus::new_default();

    // Add sdbusplus ObjectManager.
    sdbusplus::server::manager::manager objManager(bus, OBJ_INTERNAL);

    phosphor::logging::Manager manager(bus, OBJ_INTERNAL);

    bus.request_name(BUSNAME_LOGGING);

    while(true)
    {
        bus.process_discard();
        bus.wait();
    }

    return 0;
}
