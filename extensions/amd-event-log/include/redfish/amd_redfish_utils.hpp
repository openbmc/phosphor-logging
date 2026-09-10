#pragma once

#include <string>
#include <vector>

namespace phosphor::logging::extensions::ael::redfish
{

/**
 * @brief Split a comma-separated FRU list.
 *
 * Converts a comma-separated FRU string into individual
 * Redfish resource paths.
 *
 * @param[in] value Comma-separated FRU list.
 *
 * @return List of FRU paths.
 */
std::vector<std::string> splitFRUs(const std::string& value);

} // namespace phosphor::logging::extensions::ael::redfish
