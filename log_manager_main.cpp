#include "config.h"

#include "config_main.h"

#include "extensions.hpp"
#include "log_manager.hpp"
#include "paths.hpp"

#include <phosphor-logging/lg2.hpp>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/server/manager.hpp>
#include <sdeventplus/event.hpp>

#include <filesystem>

int main(int argc, char* argv[])
{
    PHOSPHOR_LOG2_USING_WITH_FLAGS;

    if (argc >= 2)
    {
        PERSIST_PATH_ROOT = strdup(argv[1]);
        info("Using temporary {PATH} for logs", "PATH", PERSIST_PATH_ROOT);
    }

    auto bus = sdbusplus::bus::new_default();
    auto event = sdeventplus::Event::get_default();
    bus.attach_event(event.get(), SD_EVENT_PRIORITY_NORMAL);

    // Add sdbusplus ObjectManager for the 'root' path of the logging manager.
    sdbusplus::server::manager_t objManager(bus, OBJ_LOGGING);

    phosphor::logging::internal::Manager iMgr(bus, OBJ_INTERNAL);

    phosphor::logging::Manager mgr(bus, OBJ_LOGGING, iMgr);

    // Create a directory to persist errors.
    std::filesystem::create_directories(phosphor::logging::paths::error());

    // Recreate error d-bus objects from persisted errors.
    iMgr.restore();

    for (auto& startup : phosphor::logging::Extensions::getStartupFunctions())
    {
        try
        {
            startup(iMgr);
        }
        catch (const std::exception& e)
        {
            error("An extension's startup function threw an exception: {ERROR}",
                  "ERROR", e);
        }
    }

    bus.request_name(BUSNAME_LOGGING);

    return event.loop();
}
