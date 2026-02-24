#include "phal_service_actions.hpp"

#include <attributes_info.H>
#include <libphal.H>

#include <libguard/guard_interface.hpp>
#include <libguard/include/guard_record.hpp>
#include <phosphor-logging/lg2.hpp>

#include <format>

using GardType = openpower::guard::GardType;

namespace openpower
{
namespace pels
{
namespace phal
{

/**
 * @brief Helper function to get gard type based on
 *        the given GardType string
 *
 * @param[in] guardTypeStr guard type enum value as a string
 *
 * @return GardType on success
 *         Empty optional on failure
 */
std::optional<GardType> getGardType(const std::string& guardTypeStr)
{
    const std::unordered_map<std::string, GardType> gardTypeMap = {
        {"GARD_Fatal", GardType::GARD_Fatal},
        {"GARD_User_Manual", GardType::GARD_User_Manual},
        {"GARD_Predictive", GardType::GARD_Predictive},
        {"GARD_Spare", GardType::GARD_Spare},
        {"GARD_Unrecoverable", GardType::GARD_Unrecoverable},
    };

    auto it = gardTypeMap.find(guardTypeStr);
    if (it != gardTypeMap.end())
    {
        return it->second;
    }
    else
    {
        lg2::error("Invalid GardType ({GUARDTYPE})", "GUARDTYPE", guardTypeStr);
    }
    return std::nullopt;
}

/**
 * @brief Helper function to create guard records.
 *
 * User need to fill the JSON callouts array with below keywords/data
 * "Entitypath": entity path of the hardware from the PHAL device tree.
 * "Guardtype": The hardware isolation severity which is defined in
 *              xyz.openbmc_project.HardwareIsolation.Entry
 * "Guarded": boolean, true to create gurad records.
 *
 * @param[in] jsonCallouts - The array of JSON callouts, or an empty object.
 * @param[in] plid - The PEL ID to be associated with the guard
 * @param[in] dataIface - The DataInterface object
 */
void createGuardRecords(const nlohmann::json& jsonCallouts, uint32_t plid,
                        const DataInterfaceBase& dataIface)
{
    if (jsonCallouts.empty())
    {
        return;
    }

    if (!jsonCallouts.is_array())
    {
        lg2::error("GUARD: Callout JSON isn't an array");
        return;
    }
    for (const auto& _callout : jsonCallouts)
    {
        try
        {
            // Check Callout data contains Guarded requests.
            if (!_callout.contains("Guarded"))
            {
                continue;
            }
            if (!_callout.at("Guarded").get<bool>())
            {
                continue;
            }
            // Get Entity path required for guard D-bus method
            // "CreateWithEntityPath"
            if (!_callout.contains("EntityPath"))
            {
                lg2::error(
                    "GUARD: Callout data, missing EntityPath information");
                continue;
            }
            using EntityPath = std::vector<uint8_t>;

            auto entityPath = _callout.at("EntityPath").get<EntityPath>();

            std::stringstream ss;
            std::ranges::for_each(entityPath, [&ss](const auto& ele) {
                ss << std::format("{:02x} ", ele);
            });

            std::string s = ss.str();
            lg2::info("GUARD: ({GUARD_TARGET})", "GUARD_TARGET", s);

            // Get Guard type
            std::string guardTypeStr = "GARD_Predictive";
            if (!_callout.contains("GuardType"))
            {
                lg2::error(
                    "GUARD: doesn't have Severity, setting to GARD_Predictive");
            }
            else
            {
                guardTypeStr = _callout.at("GuardType").get<std::string>();
            }

            GardType eGuardType =
                getGardType(guardTypeStr).value_or(GardType::GARD_Predictive);
            dataIface.createGuardRecord(entityPath, eGuardType, plid);
        }
        catch (const std::exception& e)
        {
            lg2::info("GUARD: Failed entry creation exception:({EXCEPTION})",
                      "EXCEPTION", e);
        }
    }
}

/**
 * @brief Helper function to create deconfig records.
 *
 * User need to fill the JSON callouts array with below keywords/data
 * "EntityPath": entity path of the hardware from the PHAL device tree.
 * "Deconfigured": boolean, true to create deconfigure records.
 *
 * libphal api is used for creating deconfigure records, which includes
 * update HWAS_STATE attribute to non functional with PLID information.
 *
 * @param[in] jsonCallouts - The array of JSON callouts, or an empty object.
 * @param[in] plid - PLID value
 */
void createDeconfigRecords(const nlohmann::json& jsonCallouts, uint32_t plid)
{
    using namespace openpower::phal::pdbg;

    if (jsonCallouts.empty())
    {
        return;
    }

    if (!jsonCallouts.is_array())
    {
        lg2::error("Deconfig: Callout JSON isn't an array");
        return;
    }
    for (const auto& _callout : jsonCallouts)
    {
        try
        {
            // Check Callout data contains Guarded requests.
            if (!_callout.contains("Deconfigured"))
            {
                continue;
            }

            if (!_callout.at("Deconfigured").get<bool>())
            {
                continue;
            }

            if (!_callout.contains("EntityPath"))
            {
                lg2::error(
                    "Deconfig: Callout data missing EntityPath information");
                continue;
            }
            using EntityPath = std::vector<uint8_t>;
            auto entityPath = _callout.at("EntityPath").get<EntityPath>();
            lg2::info("Deconfig: adding deconfigure record");
            // convert to libphal required format.
            ATTR_PHYS_BIN_PATH_Type physBinPath;
            std::copy(entityPath.begin(), entityPath.end(), physBinPath);
            // libphal api to deconfigure the target
            openpower::phal::pdbg::deconfigureTgt(physBinPath, plid);
        }
        catch (const std::exception& e)
        {
            lg2::info(
                "Deconfig: Failed to create records, exception:({EXCEPTION})",
                "EXCEPTION", e);
        }
    }
}

void createServiceActions(const nlohmann::json& jsonCallouts,
                          const DataInterfaceBase& dataIface, uint32_t plid)
{
    // Create Guard records.
    createGuardRecords(jsonCallouts, plid, dataIface);
    // Create Deconfigure records.
    createDeconfigRecords(jsonCallouts, plid);
}

} // namespace phal
} // namespace pels
} // namespace openpower
