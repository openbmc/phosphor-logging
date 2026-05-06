#pragma once

#include <sdbusplus/bus.hpp>
#include <sdbusplus/bus/match.hpp>

#include <memory>

namespace amd::logging::ael
{

/**
 * @brief AMD Event Logging (AEL) Manager
 *
 * Minimal daemon that observes phosphor-logging events via DBus.
 *
 * Future responsibilities:
 *  - Apply filter mapping (BMC log → AMD error type)
 *  - Load AMD Error Registry
 *  - Perform metadata correlation via DBus services
 *  - Publish AMD Event objects
 */
class AmdLogManager
{
  public:
    /**
     * @brief Construct AEL manager
     *
     * @param bus System DBus reference
     */
    explicit AmdLogManager(sdbusplus::bus::bus& bus);

    /**
     * @brief Register DBus match for new log entries
     */
    void setupMatch();

  private:
    /// DBus handle
    sdbusplus::bus::bus& bus;

    /// Signal match object
    std::unique_ptr<sdbusplus::bus::match_t> match;

    /**
     * @brief Handle new log entry signal
     *
     * @param msg InterfacesAdded DBus message
     */
    void handleLogAdded(sdbusplus::message_t& msg);
};

} // namespace amd::logging::ael
