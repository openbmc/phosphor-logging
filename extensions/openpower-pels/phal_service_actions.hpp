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
    sdbusplus::xyz::openbmc_project::HardwareIsolation::server::Entry::Type;

/**
 * @brief Helper function to get EntrySeverity based on
 *        the given GardType
 *
 * @param[in] gardType openpower gard type
 *
 * @return EntrySeverity on success
 * Empty optional on failure
 *
 * @note This function will return EntrySeverity::Warning
 * if the given GardType is not found in conversion switch block
 */

EntrySeverity getEntrySeverityType(const std::string& gardType);

using GuardType =
    sdbusplus::xyz::openbmc_project::HardwareIsolation::server::Entry::Type;

/**
 * @brief Helper function to create service actions in the PEL
 *
 * @param[in] jsonCallouts - The array of JSON callouts, or an empty object.
 * @param[in] path - The BMC error log object path
 * @param[in] dataIface - The DataInterface object
 */
void createServiceActions(const nlohmann::json& jsonCallouts,
                          const std::string& path,
                          const DataInterfaceBase& dataIface);

/**
 * @brief Helper function to create guard records.
 *
 * User need to fill the JSON callouts array with below keywords/data
 * "Entitypath": entity path of the hardware from the PHAL device tree.
 * "Guardtype": The hardware isolation severity which is defined in
 * 		 xyz.openbmc_project.HardwareIsolation.Entry
 * "Guarded": boolean, true to create gurad records.
 *
 * @param[in] jsonCallouts - The array of JSON callouts, or an empty object.
 * @param[in] path - The BMC error log object path
 * @param[in] dataIface - The DataInterface object
 */
void createGuardRecords(const nlohmann::json& jsonCallouts,
                        const std::string& path,
                        const DataInterfaceBase& dataIface);
} // namespace phal
} // namespace pels
} // namespace openpower
