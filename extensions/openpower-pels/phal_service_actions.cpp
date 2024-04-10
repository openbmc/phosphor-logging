#include "phal_service_actions.hpp"

#include <attributes_info.H>
#include <libphal.H>

#include <phosphor-logging/lg2.hpp>

#include <format>

namespace openpower
{
namespace pels
{
namespace phal
{

/**
 * @brief Helper function to get EntrySeverity based on
 *        the given GardType
 *
 * @param[in] guardType openpower gard type
 *
 * @return EntrySeverity on success
 *         Empty optional on failure
 */
std::optional<EntrySeverity> getEntrySeverityType(const std::string& guardType)
{
    if ((guardType == "GARD_Unrecoverable") || (guardType == "GARD_Fatal"))
    {
        return EntrySeverity::Critical;
    }
    else if (guardType == "GARD_User_Manual")
    {
        return EntrySeverity::Manual;
    }
    else if (guardType == "GARD_Predictive")
    {
        return EntrySeverity::Warning;
    }
    else
    {
        lg2::error(
            "GUARD: Unsupported GuardType [{GUARD_TYPE}] was given to get the "
            "hardware isolation entry severity type",
            "GUARD_TYPE", guardType);
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
 * @param[in] path - The BMC error log object path
 * @param[in] dataIface - The DataInterface object
 */
void createGuardRecords(const nlohmann::json& jsonCallouts,
                        const std::string& path,
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
            // Check Callout data conatains Guarded requests.
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
            for (uint32_t a = 0; a < sizeof(ATTR_PHYS_BIN_PATH_Type); a++)
            {
                ss << " 0x" << std::hex << static_cast<int>(entityPath[a]);
            }
            std::string s = ss.str();
            lg2::info("GUARD: ({GUARD_TARGET})", "GUARD_TARGET", s);

            // Get Guard type
            auto severity = EntrySeverity::Warning;
            if (!_callout.contains("GuardType"))
            {
                lg2::error("GUARD: doesn't have Severity, setting to warning");
            }
            else
            {
                auto guardType = _callout.at("GuardType").get<std::string>();
                // convert GuardType to severity type.
                auto sType = getEntrySeverityType(guardType);
                if (sType.has_value())
                {
                    severity = sType.value();
                }
            }
            // Create guard record
            auto type = sdbusplus::xyz::openbmc_project::HardwareIsolation::
                server::convertForMessage(severity);
            dataIface.createGuardRecord(entityPath, type, path);
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
void createDeconfigRecords(const nlohmann::json& jsonCallouts,
                           const uint32_t plid)
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
            // Check Callout data conatains Guarded requests.
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
            if (!pdbg_targets_init(NULL))
            {
                lg2::error(
                    "pdbg_targets_init failed, skipping deconfig record update");
                return;
            }
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
                          const std::string& path,
                          const DataInterfaceBase& dataIface,
                          const uint32_t plid)
{
    // Create Guard records.
    createGuardRecords(jsonCallouts, path, dataIface);
    // Create Deconfigure records.
    createDeconfigRecords(jsonCallouts, plid);
}

} // namespace phal
} // namespace pels
} // namespace openpower
