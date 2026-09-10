#pragma once

#include "amd_afid_utils.hpp"

#include <map>
#include <optional>
#include <string>

namespace phosphor::logging::extensions::ael
{

/**
 * @brief Resolve AFID metadata for an event.
 *
 * Uses the generated AFID lookup database to resolve
 * event-specific metadata based on the event message and
 * associated AdditionalData fields.
 *
 * @param[in] message Event message identifier.
 * @param[in] additionalData Event AdditionalData key/value pairs.
 *
 * @return AFID metadata when a matching lookup entry is
 *         found, otherwise std::nullopt.
 */
std::optional<AFIDInfo> resolveAFID(
    const std::string& message,
    const std::map<std::string, std::string>& additionalData);

} // namespace phosphor::logging::extensions::ael
