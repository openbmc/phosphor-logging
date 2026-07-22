#include "amd_afid_utils.hpp"

#include "amd_afid_db_gen.hpp"

namespace phosphor::logging::extensions::ael
{

std::optional<AFIDInfo> lookupAFID(
    const std::string& message, const std::vector<std::string>& additionalData)
{
    if (message.empty())
    {
        return std::nullopt;
    }

    auto result = ::lookupAFID(message, additionalData);

    return AFIDInfo{
        result.afid,
        std::vector<std::string>(result.origins.begin(), result.origins.end())};
}

} // namespace phosphor::logging::extensions::ael
