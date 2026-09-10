#include "ael_info.hpp"
#include "ael_metadata.hpp"
#include "amd_afid_utils.hpp"
#include "extensions.hpp"
#include "redfish/amd_redfish_projector.hpp"

#include <phosphor-logging/lg2.hpp>

#include <ranges>
#include <string>

namespace phosphor::logging::extensions::ael
{

using namespace phosphor::logging;

/**
 * @brief Create AMD runtime metadata used for Redfish projection.
 *
 * Builds an intermediate AMD metadata object from AFID and
 * static metadata sources. The resulting metadata is used
 * as input to the AMD Redfish projector and is not exposed
 * as the final AMD payload.
 *
 * @param[in] info AFID metadata derived from the event.
 *
 * @return AMD metadata namespace used for Redfish
 *         projection.
 */
static nlohmann::json createAMDMetadata(const AFIDInfo& info)
{
    nlohmann::json amdMetadata;
    if (info.afid != 0)
    {
        amdMetadata[std::string(fields::AFID)] = info.afid;
    }
    if (!info.description.empty())
    {
        amdMetadata[std::string(fields::Description)] = info.description;
    }
    if (!info.redfishMappings.empty())
    {
        const auto joined = info.redfishMappings | std::views::join_with(',');
        const std::string frus(joined.begin(), joined.end());
        amdMetadata[std::string(fields::FruList)] = frus;
    }

    const auto staticInfo = getStaticData();
    if (!staticInfo.version.empty())
    {
        amdMetadata[std::string(fields::Version)] = staticInfo.version;
    }
    if (!staticInfo.rackUnitPosition.empty())
    {
        amdMetadata[std::string(fields::RackUnitPosition)] =
            staticInfo.rackUnitPosition;
    }

    return amdMetadata;
}

/**
 * @brief AMD runtime metadata provider.
 *
 * Generates the AMD OEM payload associated with the
 * current event.
 *
 * A producer may optionally provide a fully rendered
 * Redfish payload using AEL.REDFISH. In this case the
 * supplied payload is used directly and no additional
 * metadata processing is performed.
 *
 * Otherwise AMD metadata is collected and projected
 * into the final Redfish representation.
 *
 * @param[in,out] metadata Aggregated runtime metadata.
 * @param[in] message Event message.
 * @param[in] level Event severity.
 * @param[in] additionalData Event additional data.
 */
static void amdRuntimeMetadataProvider(
    nlohmann::json& metadata, const std::string& message,
    [[maybe_unused]] Entry::Level level,
    const std::map<std::string, std::string>& additionalData)
{
    try
    {
        auto redfishIt = additionalData.find(std::string(fields::Redfish));
        if (redfishIt != additionalData.end())
        {
            metadata["AMD"] = nlohmann::json::parse(redfishIt->second);
            return;
        }

        auto info = resolveAFID(message, additionalData);
        if (!info)
        {
            lg2::debug("amdRuntimeMetadataProvider: "
                       "no AEL data found");
            return;
        }

        const auto amdMetadata = createAMDMetadata(*info);
        metadata["AMD"] = redfish::project(amdMetadata);
    }
    catch (const std::exception& e)
    {
        lg2::error("amdRuntimeMetadataProvider exception: "
                   "{ERROR}",
                   "ERROR", e.what());
    }
}

/**
 * Register AMD runtime metadata provider.
 */
REGISTER_RUNTIME_METADATA_PROVIDER(amdRuntimeMetadataProvider);

} // namespace phosphor::logging::extensions::ael
