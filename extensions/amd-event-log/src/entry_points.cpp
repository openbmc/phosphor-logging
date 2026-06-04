#include "extensions.hpp"

#include <phosphor-logging/lg2.hpp>

namespace phosphor::logging::extensions::ael
{

using namespace phosphor::logging;

/**
 * @brief AMD OEM provider (minimal infra)
 */
static void amdOemProvider(OemType& oem, const std::string& /*msg*/,
                           uint32_t /*id*/, Entry::Level /*level*/,
                           const AdditionalDataVec& /*additionalData*/,
                           const AssociationList& /*associations*/)
{
    try
    {
        // Ensure vendor section exists
        auto& amd = oem["AMD"];
        (void)amd;
    }
    catch (...)
    {
        lg2::error("amdOemProvider: exception while creating OEM section");
    }
}

/**
 * @brief Register OEM providers
 */
static void registerOemProviders()
{
    Extensions::getOemProviderFunctions().emplace_back(amdOemProvider);
}

/**
 * @brief Main initialization entry
 */
void initialize()
{
    registerOemProviders();
    lg2::info("AMD AEL extension initialized");
}

/**
 * @brief Extension startup hook
 */
void startupHandler(internal::Manager& /*manager*/)
{
    initialize();
}

/**
 * Register startup hook
 */
REGISTER_EXTENSION_FUNCTION(startupHandler);

} // namespace phosphor::logging::extensions::ael
