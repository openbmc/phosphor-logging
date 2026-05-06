#include "amd_log_manager.hpp"

#include <phosphor-logging/lg2.hpp>
#include <sdbusplus/asio/connection.hpp>
#include <sdbusplus/bus.hpp>
#include <sdeventplus/event.hpp>

int main()
{
    lg2::info("Starting AMD Event Logging Manager (AEL)");

    // Create default event loop
    auto event = sdeventplus::Event::get_default();

    // Create DBus connection bound to event loop
    auto bus = sdbusplus::bus::new_default();

    // Attach DBus to sd-event loop
    bus.attach_event(event.get(), SD_EVENT_PRIORITY_NORMAL);

    // Create AEL manager
    amd::logging::ael::AmdLogManager manager(bus);

    manager.setupMatch();

    lg2::info("AEL: Listening for log events");

    // Run event loop (blocks here)
    event.loop();

    return 0;
}
