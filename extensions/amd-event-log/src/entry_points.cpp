#include "ael_info.hpp"
#include "extensions.hpp"

#include <phosphor-logging/lg2.hpp>

#include <ranges>
#include <string>

namespace phosphor::logging::extensions::ael
{

using namespace phosphor::logging;

static void amdOemProvider(OemType& oem, const std::string& msg,
                           uint32_t /*id*/, Entry::Level level,
                           const AdditionalDataVec& additionalData,
                           const AssociationList& associations)
{
    try
    {
        const AELInfo info =
            AELInfoProvider(msg, level, additionalData, associations).get();

        if (info.afid == 0 && info.fruList.empty())
        {
            lg2::info("amdOemProvider: no AEL data found");
            return;
        }

        auto& amd = oem["AMD"];

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
        lg2::error("amdOemProvider exception: {ERROR}", "ERROR", e.what());
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
