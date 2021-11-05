#include "phal_service_actions.hpp"

#include <attributes_info.H>
#include <fmt/format.h>

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{
namespace phal
{
using namespace phosphor::logging;

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
        log<level::ERR>(
            fmt::format("GUARD: Unsupported GardType [{}] was given ",
                        "to get the hardware isolation entry severity type",
                        guardType.c_str())
                .c_str());
    }
    return std::nullopt;
}

void createServiceActions(const nlohmann::json& jsonCallouts,
                          const std::string& path,
                          const DataInterfaceBase& dataIface)
{
    // Create Guard records.
    createGuardRecords(jsonCallouts, path, dataIface);
}

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
        log<level::ERR>("GUARD: Callout JSON isn't an array");
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
                log<level::ERR>(
                    "GUARD: Callout data, missing EntityPath information");
                continue;
            }
            using EntityPath = std::vector<uint8_t>;

            auto entityPath = _callout.at("EntityPath").get<EntityPath>();

            std::stringstream ss;
            for (uint32_t a = 0; a < sizeof(ATTR_PHYS_BIN_PATH_Type); a++)
            {
                ss << " 0x" << std::hex << int(entityPath[a]);
            }
            std::string s = ss.str();
            log<level::INFO>(fmt::format("GUARD :({})", s).c_str());

            // Get Guard type
            auto severity = EntrySeverity::Warning;
            if (!_callout.contains("GuardType"))
            {
                log<level::ERR>(
                    "GUARD: doesn't have Severity, setting to warning");
            }
            else
            {
                auto guardType = _callout.at("GuardType").get<std::string>();
                // convert GuardType to sevrity type.
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
            log<level::INFO>(
                fmt::format("GUARD: Failed entry creation exception:({})",
                            e.what())
                    .c_str());
        }
    }
}

} // namespace phal
} // namespace pels
} // namespace openpower
