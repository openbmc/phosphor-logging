#include "redfish/amd_redfish_utils.hpp"

#include "redfish/amd_redfish_constants.hpp"

#include <sstream>

namespace phosphor::logging::extensions::ael::redfish
{

std::string getAFIDDescription(uint64_t afid)
{
    for (const auto& range : constants::afidRanges)
    {
        if (afid >= range.start && afid <= range.end)
        {
            return std::string(range.description);
        }
    }

    return "Unknown Error";
}

std::vector<std::string> splitFRUs(const std::string& value)
{
    std::vector<std::string> frus;
    std::istringstream ss(value);
    std::string item;

    while (std::getline(ss, item, ','))
    {
        const auto begin = item.find_first_not_of(" \t");
        if (begin == std::string::npos)
        {
            continue;
        }

        const auto end = item.find_last_not_of(" \t");
        frus.emplace_back(item.substr(begin, end - begin + 1));
    }

    return frus;
}

} // namespace phosphor::logging::extensions::ael::redfish
