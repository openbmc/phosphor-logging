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

EntrySeverity getEntrySeverityType(const std::string& gardType)
{
    if ((gardType == "GARD_Unrecoverable") || (gardType == "GARD_Fatal"))
        return EntrySeverity::Critical;

    else if (gardType == "GARD_User_Manual")
        return EntrySeverity::Manual;

    else if (gardType == "GARD_Predictive")
        return EntrySeverity::Warning;

    else
    {
        log<level::ERR>(
            fmt::format("GUARD: Unsupported GardType [{}] was given ",
                        "to get the hardware isolation entry severity type",
                        gardType.c_str())
                .c_str());
    }
    return EntrySeverity::Warning;
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
                continue;
            if (!_callout.at("Guarded").get<bool>())
                continue;
            // Get Entity path required for guard D-bus method
            // "CreateWithEntityPath"
            if (!_callout.contains("Entitypath"))
            {
                log<level::ERR>(
                    "GUARD: Callout missing, Entity path information");
                continue;
            }
            using EntityPath = std::vector<uint8_t>;

            auto entityPath = _callout.at("Entitypath").get<EntityPath>();

            std::stringstream ss;
            for (uint32_t a = 0; a < sizeof(ATTR_PHYS_BIN_PATH_Type); a++)
            {
                ss << " 0x" << std::hex << int(entityPath[a]);
            }
            std::string s = ss.str();
            log<level::INFO>(fmt::format("GUARD :({})", s.c_str()).c_str());

            // Get Guard type
            auto guardType = GuardType::Warning;
            if (!_callout.contains("Guardtype"))
            {
                log<level::ERR>(
                    "GUARD: doesn't have Severity, setting to warning");
            }
            // Create guard record
            auto type = sdbusplus::xyz::openbmc_project::HardwareIsolation::
                server::convertForMessage(guardType);
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
