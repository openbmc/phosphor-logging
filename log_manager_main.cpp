#include <sdbusplus/bus.hpp>
#include <sdbusplus/server/manager.hpp>
#include <experimental/filesystem>
#include "config.h"
#include "log_manager.hpp"

int main(int argc, char *argv[])
{
    auto bus = sdbusplus::bus::new_default();

    // Add sdbusplus ObjectManager for the 'root' path of the logging manager.
    sdbusplus::server::manager::manager objManager(bus, OBJ_LOGGING);

    phosphor::logging::internal::Manager iMgr(bus, OBJ_INTERNAL);

    phosphor::logging::Manager mgr(bus, OBJ_LOGGING, iMgr);

    // Create a directory to persist errors.
    std::experimental::filesystem::create_directories(ERRLOG_PERSIST_PATH);

    // Recreate error d-bus objects from persisted errors.
    iMgr.restore();

    bus.request_name(BUSNAME_LOGGING);

    while(true)
    {
        bus.process_discard();
        bus.wait();
    }

    return 0;
}
