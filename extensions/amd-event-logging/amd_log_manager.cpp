#include "amd_log_manager.hpp"

#include <phosphor-logging/lg2.hpp>

#include <map>
#include <variant>

namespace amd::logging::ael
{

/**
 * @brief DBus namespace for logging entries
 */
static constexpr auto loggingPath = "/xyz/openbmc_project/logging";

/**
 * @brief Constructor
 */
AmdLogManager::AmdLogManager(sdbusplus::bus::bus& bus) : bus(bus) {}

/**
 * @brief Setup DBus signal listener
 *
 * Listens for InterfacesAdded signals under logging namespace.
 * Triggered when a new log entry is created.
 */
void AmdLogManager::setupMatch()
{
    using namespace sdbusplus::bus::match::rules;

    match = std::make_unique<sdbusplus::bus::match_t>(
        bus,
        type::signal() + member("InterfacesAdded") +
            path_namespace(loggingPath),
        [this](sdbusplus::message_t& msg) { this->handleLogAdded(msg); });
    lg2::info("AEL DBus match registered for logging namespace");
}

/**
 * @brief Handle new log entry event
 *
 * This function is triggered whenever a new log entry appears.
 * Currently prints basic information for review/debug.
 */
void AmdLogManager::handleLogAdded(sdbusplus::message_t& msg)
{
    try
    {
        sdbusplus::message::object_path objectPath;

        std::map<std::string,
                 std::map<std::string, std::variant<std::string, uint64_t>>>
            interfaces;

        msg.read(objectPath, interfaces);

        // Only process logging entries
        if (interfaces.count("xyz.openbmc_project.Logging.Entry") == 0)
        {
            return;
        }

        lg2::info("AEL: New log entry detected", "PATH", objectPath.str);

        for (const auto& [iface, props] : interfaces)
        {
            lg2::debug("AEL: Interface", "IFACE", iface);

            for (const auto& [key, val] : props)
            {
                lg2::debug("AEL: Property", "KEY", key);
            }
        }
    }
    catch (const std::exception& e)
    {
        lg2::error("AEL: Failed to parse DBus signal", "ERROR", e.what());
    }
}

} // namespace amd::logging::ael
