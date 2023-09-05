#pragma once

#include "data_interface.hpp"

#include <nlohmann/json.hpp>
#include <xyz/openbmc_project/HardwareIsolation/Entry/server.hpp>

namespace openpower
{
namespace pels
{
namespace phal
{

using EntrySeverity =
    sdbusplus::server::xyz::openbmc_project::hardware_isolation::Entry::Type;

/**
 * @brief Helper function to create service actions in the PEL
 *
 * @param[in] jsonCallouts - The array of JSON callouts, or an empty object.
 * @param[in] path - The BMC error log object path
 * @param[in] dataIface - The DataInterface object
 * @param[in] plid -  the PLID
 */
void createServiceActions(const nlohmann::json& jsonCallouts,
                          const std::string& path,
                          const DataInterfaceBase& dataIface,
                          const uint32_t plid);
} // namespace phal
} // namespace pels
} // namespace openpower
