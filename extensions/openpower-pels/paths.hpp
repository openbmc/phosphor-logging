#pragma once
#include <filesystem>

namespace openpower
{
namespace pels
{

/**
 * @brief Returns the path to the PEL ID file
 */
std::filesystem::path getPELIDFile();

/**
 * @brief Returns the path to the PEL repository
 */
std::filesystem::path getPELRepoPath();

/**
 * @brief Returns the path to the read only data directory
 */
std::filesystem::path getPELReadOnlyDataPath();
} // namespace pels
} // namespace openpower
