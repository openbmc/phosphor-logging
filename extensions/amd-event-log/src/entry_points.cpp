#include "ael_common_info.hpp"
#include "ael_info.hpp"
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
 * Builds an intermediate AMD metadata object from AEL and
 * common metadata sources. The resulting metadata is used
 * as input to the AMD Redfish projector and is not exposed
 * as the final AMD payload.
 *
 * Event-specific metadata may override common metadata
 * defaults prior to projection.
 *
 * @param[in] info AEL metadata derived from the event.
 *
 * @return AMD metadata namespace used for Redfish
 *         projection.
 */
static nlohmann::json createAMDMetadata(const AELInfo& info)
{
    nlohmann::json amdMetadata;

    amdMetadata[std::string(fields::Version)] = info.version;

    if (info.afid != 0)
    {
        amdMetadata[std::string(fields::AFID)] = std::to_string(info.afid);
    }

    if (!info.fruList.empty())
    {
        const auto joined = info.fruList | std::views::join_with(',');

        const std::string frus(joined.begin(), joined.end());

        amdMetadata[std::string(fields::FruList)] = frus;
    }

    const CommonInfo commonInfo = getCommonInfo();

    if (commonInfo.rackId)
    {
        amdMetadata[std::string(fields::RackId)] = *commonInfo.rackId;
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
 * Otherwise, AMD metadata is collected and projected
 * into the final Redfish representation.
 *
 * @param[in,out] metadata Aggregated runtime metadata.
 * @param[in] message Event message.
 * @param[in] level Event severity.
 * @param[in] additionalData Event additional data.
 */
static void amdRuntimeMetadataProvider(
    nlohmann::json& metadata, const std::string& message, Entry::Level level,
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

        const AELInfo info =
            AELInfoProvider(message, level, additionalData).get();

        if (info.afid == 0 && info.fruList.empty())
        {
            lg2::debug("amdRuntimeMetadataProvider: no AEL data found");
            return;
        }

        const auto amdMetadata = createAMDMetadata(info);

        metadata["AMD"] = redfish::project(amdMetadata);
    }
    catch (const std::exception& e)
    {
        lg2::error("amdRuntimeMetadataProvider exception: {ERROR}", "ERROR",
                   e.what());
    }
}

/**
 * Register AMD runtime metadata provider.
 */
REGISTER_RUNTIME_METADATA_PROVIDER(amdRuntimeMetadataProvider);

} // namespace phosphor::logging::extensions::ael
