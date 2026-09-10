#include "ael_info.hpp"

namespace phosphor::logging::extensions::ael
{

std::optional<AFIDInfo> resolveAFID(
    const std::string& message,
    const std::map<std::string, std::string>& additionalData)
{
    auto info = lookupAFID(message, additionalData);

    if (info.afid == 0)
    {
        return std::nullopt;
    }

    return info;
}

} // namespace phosphor::logging::extensions::ael
