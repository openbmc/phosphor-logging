#include "amd_afid_utils.hpp"

#include "amd_afid_db_gen.hpp"

namespace phosphor::logging::extensions::ael
{

std::optional<AFIDInfo> lookupAFID(
    const std::string& message,
    const std::map<std::string, std::string>& additionalData)
{
    if (message.empty())
    {
        return std::nullopt;
    }

    std::vector<std::string> addDataVec;
    addDataVec.reserve(additionalData.size());

    for (const auto& [key, value] : additionalData)
    {
        addDataVec.emplace_back(key + "=" + value);
    }

    auto result = ::lookupAFID(message, addDataVec);
    // TODO Revist this check
    if (result.origins.empty())
    {
        return std::nullopt;
    }

    return AFIDInfo{
        result.afid,
        std::vector<std::string>(result.origins.begin(), result.origins.end())};
}

} // namespace phosphor::logging::extensions::ael
