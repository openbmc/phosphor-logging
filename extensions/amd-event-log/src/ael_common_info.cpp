#include "ael_common_info.hpp"

namespace phosphor::logging::extensions::ael
{

AdditionalDataMap parseAdditionalData(
    const std::vector<std::string>& additionalData)
{
    AdditionalDataMap result;

    for (const auto& entry : additionalData)
    {
        auto pos = entry.find('=');
        if (pos == std::string::npos)
        {
            continue;
        }

        result.emplace(entry.substr(0, pos), entry.substr(pos + 1));
    }

    return result;
}

} // namespace phosphor::logging::extensions::ael
