#include "phal_service_actions.hpp"

#ifdef PEL_ENABLE_PHAL_NEXT
#include <guard/guard_facade.H>
#include <guard/libguard/guard_exception.H>
#include <guard/libguard/include/guard_record.H>
#include <ipl_facade.H>
#include <target_utils.H>
#include <targeting/common/entitypath.H>

using GardType = hostfw::guard::GARD_ErrorType;
#else
#include <attributes_info.H>
#include <libphal.H>

#include <libguard/guard_interface.hpp>
#include <libguard/include/guard_record.hpp>

using GardType = openpower::guard::GardType;
#endif

#include <phosphor-logging/lg2.hpp>

#include <format>

namespace openpower
{
namespace pels
{
namespace phal
{

/**
 * @brief Helper function to get guard type based on
 *        the given GardType string
 *
 * @param[in] guardTypeStr guard type enum value as a string
 *
 * @return GardType on success
 *         Empty optional on failure
 */
std::optional<GardType> getGardType(const std::string& guardTypeStr)
{
#ifdef PEL_ENABLE_PHAL_NEXT
    const std::unordered_map<std::string, GardType> gardTypeMap = {
        {"GARD_Fatal", hostfw::guard::GARD_Fatal},
        {"GARD_User_Manual", hostfw::guard::GARD_User_Manual},
        {"GARD_Predictive", hostfw::guard::GARD_Predictive},
        {"GARD_Spare", hostfw::guard::GARD_Spare},
        {"GARD_Unrecoverable", hostfw::guard::GARD_Unrecoverable},
        {"GARD_Power", hostfw::guard::GARD_Power},
        {"GARD_PHYP", hostfw::guard::GARD_PHYP},
        {"GARD_Reconfig", hostfw::guard::GARD_Reconfig},
        {"GARD_Sticky_deconfig", hostfw::guard::GARD_Sticky_deconfig},
    };
#else
    const std::unordered_map<std::string, GardType> gardTypeMap = {
        {"GARD_Fatal", GardType::GARD_Fatal},
        {"GARD_User_Manual", GardType::GARD_User_Manual},
        {"GARD_Predictive", GardType::GARD_Predictive},
        {"GARD_Spare", GardType::GARD_Spare},
        {"GARD_Unrecoverable", GardType::GARD_Unrecoverable},
    };
#endif

    auto it = gardTypeMap.find(guardTypeStr);
    if (it != gardTypeMap.end())
    {
        return it->second;
    }
    lg2::error("Invalid GardType ({GUARDTYPE})", "GUARDTYPE", guardTypeStr);
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

#ifdef PEL_ENABLE_PHAL_NEXT
            const auto ep = TARGETING::EntityPath::fromBinary(
                std::span<const uint8_t>{entityPath.data(), entityPath.size()});
            if (ep.getSize() == 0)
            {
                lg2::error(
                    "GUARD: Invalid EntityPath binary data, ep.getSize() == 0");
                continue;
            }
#endif

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

#ifdef PEL_ENABLE_PHAL_NEXT
            GardType eGuardType = getGardType(guardTypeStr)
                                      .value_or(hostfw::guard::GARD_Predictive);
#else
            GardType eGuardType =
                getGardType(guardTypeStr).value_or(GardType::GARD_Predictive);
#endif
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
    if (jsonCallouts.empty())
    {
        return;
    }

    if (!jsonCallouts.is_array())
    {
        lg2::error("Deconfig: Callout JSON isn't an array");
        return;
    }

#ifdef PEL_ENABLE_PHAL_NEXT
    try
    {
        TARGETING::utils::targetingInit();
    }
    catch (const std::exception& e)
    {
        lg2::error("Deconfig: targetingInit failed: {EXCEPTION}", "EXCEPTION",
                   e);
        return;
    }
#endif

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

#ifdef PEL_ENABLE_PHAL_NEXT
            const auto ep = TARGETING::EntityPath::fromBinary(
                std::span<const uint8_t>{entityPath.data(), entityPath.size()});
            if (ep.getSize() == 0)
            {
                lg2::error(
                    "Deconfig: Invalid EntityPath binary data, ep.getSize() == 0");
                continue;
            }

            if (!hostfw::ipl::deconfigureTarget(entityPath, plid))
            {
                lg2::error(
                    "Deconfig: hostfw::ipl::deconfigureTarget failed for target");
            }
#else
            using namespace openpower::phal::pdbg;
            // convert to libphal required format.
            ATTR_PHYS_BIN_PATH_Type physBinPath;
            std::copy(entityPath.begin(), entityPath.end(), physBinPath);
            // libphal api to deconfigure the target
            openpower::phal::pdbg::deconfigureTgt(physBinPath, plid);
#endif
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
