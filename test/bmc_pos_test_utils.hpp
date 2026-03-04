#pragma once

namespace phosphor::logging
{

/**
 * @brief Clean up the BMC position test directory
 *
 * Removes the directory created by getBMCPositionFilePath()
 */
void cleanupBMCPosTestDir();

} // namespace phosphor::logging
