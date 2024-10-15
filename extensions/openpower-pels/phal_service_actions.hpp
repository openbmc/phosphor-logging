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

/**
 * @brief Helper function to create service actions in the PEL
 *
 * @param[in] jsonCallouts - The array of JSON callouts, or an empty object.
 * @param[in] dataIface - The DataInterface object
 * @param[in] plid -  the PLID
 */
void createServiceActions(const nlohmann::json& jsonCallouts,
                          const DataInterfaceBase& dataIface,
                          const uint32_t plid);
} // namespace phal
} // namespace pels
} // namespace openpower
