#pragma once

#include <fstream>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace phosphor::logging::util
{

/**
 * @brief Return a value found in the /etc/os-release file
 *
 * @param[in] key - The key name, like "VERSION"
 *
 * @return std::optional<std::string> - The value
 */
std::optional<std::string> getOSReleaseValue(const std::string& key);

/**
 * @brief Synchronize unwritten journal messages to disk.
 * @details This is the same implementation as the systemd command
 *          "journalctl --sync".
 */
void journalSync();

namespace additional_data
{
/** @brief Pull out metadata name and value from the string
 *         <metadata name>=<metadata value>
 *  @param [in] data - metadata key=value entries
 *  @return map of metadata name:value
 */
auto parse(const std::vector<std::string>& data)
    -> std::map<std::string, std::string>;
/** @brief Combine the metadata keys and values from the map
 *         into a vector of strings that look like:
 *            "<metadata name>=<metadata value>"
 *  @param [in] data - metadata key:value map
 *  @return vector of "key=value" strings
 */
auto combine(const std::map<std::string, std::string>& data)
    -> std::vector<std::string>;
} // namespace additional_data

} // namespace phosphor::logging::util
