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
 * @brief Returns the path to the message registry JSON file
 */
std::filesystem::path getMessageRegistryPath();
} // namespace pels
} // namespace openpower
