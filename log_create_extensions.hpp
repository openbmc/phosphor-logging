#pragma once

#include <nlohmann/json_fwd.hpp>
#include <sdbusplus/exception.hpp>

#include <string>

/**
 * @brief Extend an event with data for a registered extension interface.
 *
 * The interface name is looked up against a registry of known logging
 * extension interfaces.  The JSON payload is deserialized into the
 * properties type of that interface and applied to the event via its
 * `.extend(...)` mechanism.
 *
 * @param[in,out] event - The event to extend.
 * @param[in] interface - The interface name to look up, e.g.
 *                        "xyz.openbmc_project.Logging.Extension.CPER.Processed".
 * @param[in] json - JSON object matching the interface's properties.
 *
 * @throws std::invalid_argument if the interface is not registered.
 * @throws An exception from the interface's JSON deserialization (e.g. a
 *         nlohmann::json exception or sdbusplus::exception::InvalidEnumString)
 *         if the payload does not match the interface's properties.
 */
void extendLogEvent(sdbusplus::exception::generated_event_base& event,
                    const std::string& interface, const nlohmann::json& json);
