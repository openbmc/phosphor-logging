#include "audit_manager.hpp"

#include <sdbusplus/bus.hpp>
#include <sdbusplus/server/manager.hpp>
#include <sdeventplus/event.hpp>

int main(int /*argc*/, char* /*argv*/[])
{
    static constexpr auto busName = "xyz.openbmc_project.Logging.Audit";
    static constexpr auto objPath = "/xyz/openbmc_project/logging/audit";
    static constexpr auto objMgr = "/xyz/openbmc_project/logging/audit/manager";
    // static constexpr auto intf = "xyz.openbmc_project.Logging.Audit";

    auto bus = sdbusplus::bus::new_default();
    auto event = sdeventplus::Event::get_default();
    bus.attach_event(event.get(), SD_EVENT_PRIORITY_NORMAL);

    sdbusplus::server::manager::manager objManager(bus, objPath);

    phosphor::audit::Manager mgr(bus, objMgr);

    bus.request_name(busName);

    return event.loop();
}
