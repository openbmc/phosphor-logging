#include "extensions.hpp"

#include <phosphor-logging/lg2.hpp>

namespace phosphor::logging::extensions::ael
{

using namespace phosphor::logging;

/**
 * @brief AMD runtime metadata provider.
 *
 * Contributes AMD-specific runtime metadata during
 * log creation.
 *
 * @param[in,out] metadata
 *     Aggregated runtime metadata object.
 *
 * @param[in] message
 *     Log message associated with the entry.
 *
 * @param[in] level
 *     Severity level of the entry.
 *
 * @param[in] additionalData
 *     Additional data supplied during log creation.
 */
static void amdRuntimeMetadataProvider(
    nlohmann::json& metadata, const std::string& /*message*/,
    Entry::Level /*level*/,
    const std::map<std::string, std::string>& /*additionalData*/)
{
    try
    {
        /*
         * Create AMD runtime metadata section.
         *
         * Future enhancements may populate:
         *   metadata["AMD"]["AFID"]
         *   metadata["AMD"]["FRU"]
         *   metadata["AMD"]["Platform"]
         */
        auto& amd = metadata["AMD"];
        (void)amd;
    }
    catch (const std::exception& e)
    {
        lg2::error("AMD runtime metadata provider failed: {ERROR}", "ERROR",
                   e.what());
    }
    catch (...)
    {
        lg2::error(
            "AMD runtime metadata provider failed with unknown exception");
    }
}

/**
 * Register AMD runtime metadata provider.
 */
REGISTER_RUNTIME_METADATA_PROVIDER(amdRuntimeMetadataProvider);

} // namespace phosphor::logging::extensions::ael
