#include "ael_info.hpp"
#include "extensions.hpp"

#include <phosphor-logging/lg2.hpp>

#include <ranges>
#include <string>

namespace phosphor::logging::extensions::ael
{

using namespace phosphor::logging;

/**
 * @brief AMD runtime metadata provider.
 *
 * Generates AEL metadata and contributes it to the
 * aggregated runtime metadata object.
 *
 * @param[in,out] metadata
 *     Aggregated runtime metadata.
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
        const AELInfo info =
            AELInfoProvider(message, level, additionalData).get();

        if (info.afid == 0 && info.fruList.empty())
        {
            lg2::debug("amdRuntimeMetadataProvider: no AEL data found");
            return;
        }

        auto& amd = metadata["AMD"];

        amd[std::string(fields::Version)] = info.version;

        if (info.afid != 0)
        {
            amd[std::string(fields::AFID)] = std::to_string(info.afid);
        }

        if (!info.fruList.empty())
        {
            const auto joined = info.fruList | std::views::join_with(',');

            const std::string frus(joined.begin(), joined.end());

            amd[std::string(fields::FruList)] = frus;

            lg2::debug("FruList=[{FRUS}]", "FRUS", frus);
        }
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
