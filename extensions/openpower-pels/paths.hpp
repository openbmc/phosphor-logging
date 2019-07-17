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

} // namespace pels
} // namespace openpower
