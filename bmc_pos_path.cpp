#include "bmc_pos_path.hpp"

namespace phosphor::logging
{

std::filesystem::path getBMCPositionFilePath()
{
    return "/run/openbmc/bmc_position";
}

} // namespace phosphor::logging
