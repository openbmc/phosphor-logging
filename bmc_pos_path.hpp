#pragma once
#include <filesystem>

namespace phosphor::logging
{

/** @brief Returns the BMC position file path
 *
 *  In production: returns /run/openbmc/bmc_position
 *  In tests: returns a temporary test directory path with initialized file
 *
 *  @return The path to the BMC position file
 */
std::filesystem::path getBMCPositionFilePath();

} // namespace phosphor::logging
