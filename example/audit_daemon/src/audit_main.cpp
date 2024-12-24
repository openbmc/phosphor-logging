/**
 * @brief The phosphor audit daemon main entry point
 *
 * This file is part of the phosphor-logging audit project.
 *
 */

#include "audit_config.hpp"
#include "audit_manager.hpp"
#include "linux_audit.hpp"

#include <sdbusplus/bus.hpp>
#include <sdbusplus/server/manager.hpp>
#include <sdeventplus/event.hpp>

#include <algorithm>
#include <any>
#include <exception>
#include <filesystem>
#include <format>
#include <forward_list>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

// busctl tree xyz.openbmc_project.Logging
//   busctl introspect xyz.openbmc_project.Logging
//    /xyz/openbmc_project/logging/internal/manager

int main(int argc, char* argv[])
{
    using namespace phosphor::logging;

    auto config = std::make_shared<audit::Config::>(conf_file_path);

    config->ReadConfigurationFile();

    static constexpr auto busName = "xyz.openbmc_project.Logging.Audit";
    static constexpr auto objPath = "/xyz/openbmc_project/logging/audit";
    static constexpr auto objMgr = "/xyz/openbmc_project/logging/audit/manager";

    config->ShowConfigs();

    auto bus = sdbusplus::bus::new_default();
    auto event = sdeventplus::Event::get_default();

    bus.attach_event(event.get(), SD_EVENT_PRIORITY_NORMAL);
    sdbusplus::server::manager::manager objManager(bus, objPath);
    audit::Manager mgr(bus, objMgr);
    bus.request_name(busName);

    return event.loop();
}
