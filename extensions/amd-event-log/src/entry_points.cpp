#include "ael_info.hpp"
#include "extensions.hpp"

#include <phosphor-logging/lg2.hpp>

namespace phosphor::logging::extensions::ael
{

using namespace phosphor::logging;

/**
 * @brief AMD runtime metadata provider.
 *
 * Resolves AFID metadata for an event and adds it to the
 * aggregated runtime metadata. This provider is responsible
 * only for AFID lookup integration and does not perform any
 * Redfish payload projection.
 *
 * @param[in,out] metadata Aggregated runtime metadata.
 * @param[in] message Event message identifier.
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
        auto info = resolveAFID(message, additionalData);
        if (!info)
        {
            lg2::debug("amdRuntimeMetadataProvider: "
                       "no AFID metadata found");
            return;
        }

        nlohmann::json amdMetadata;
        amdMetadata["AFID"] = info->afid;
        if (!info->description.empty())
        {
            amdMetadata["Description"] = info->description;
        }
        if (!info->origins.empty())
        {
            amdMetadata["OriginOfCondition"] = info->origins;
        }
        if (!info->redfishMappings.empty())
        {
            amdMetadata["RedfishMappings"] = info->redfishMappings;
        }
        metadata["AMD"] = std::move(amdMetadata);
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
