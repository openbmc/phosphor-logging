#pragma once

#include <fstream>
#include <optional>
#include <string>

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

} // namespace phosphor::logging::util
