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

} // namespace pels
} // namespace openpower
