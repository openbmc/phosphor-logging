#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace openpower::pels::user_data
{

/**
 * @brief  Returns the UserData contents as a formatted JSON string.
 *
 * @param[in] componentID - The comp ID from the UserData section header
 * @param[in] subType - The subtype from the UserData section header
 * @param[in] version - The version from the UserData section header
 * @param[in] data - The section data
 * @param[in] creatorID - Creator Subsystem ID from Private Header
 * @param[in] plugins - Vector of strings of plugins found in filesystem
 * @return std::optional<std::string> - The JSON string if it could be created,
 *                                      else std::nullopt.
 */
std::optional<std::string> getJSON(uint16_t componentID, uint8_t subType,
                                   uint8_t version,
                                   const std::vector<uint8_t>& data,
                                   uint8_t creatorID,
                                   const std::vector<std::string>& plugins);

} // namespace openpower::pels::user_data
