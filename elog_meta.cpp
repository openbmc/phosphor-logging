#include "config.h"
#include "elog_meta.hpp"

namespace phosphor
{
namespace logging
{
namespace metadata
{

#if defined PROCESS_META

template <>
void handler<xyz::openbmc_project::Common::
             Callout::Device::CALLOUT_DEVICE_PATH>(
    std::string&& match,
    const std::vector<std::string>& data,
    AssociationList& list)
{
    using namespace std::string_literals;
    std::map<std::string, std::string> metadata;
    parse(data, metadata);
    auto iter = metadata.find(match);
    if(metadata.end() != iter)
    {
        auto search = [&iter](const auto&entry)
        {
            return std::get<0>(entry) == iter->second;
        };
        auto callout = std::find_if(callouts.begin(), callouts.end(), search);
        if(callouts.end() != callout)
        {
            list.push_back(std::make_tuple(CALLOUT_FWD_ASSOCIATION,
                                           CALLOUT_REV_ASSOCIATION,
                                           std::string(INVENTORY_ROOT) +
                                           std::get<1>(*callout)));
        }
    }
}

#endif

} // namespace metadata
} // namespace logging
} // namespace phosphor
