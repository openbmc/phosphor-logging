#pragma once

#include <vector>
#include <string>
#include <tuple>
#include <algorithm>
#include <cstring>
#include <phosphor-logging/elog-errors.hpp>
#include "elog_entry.hpp"
#include "callouts-gen.hpp"

namespace phosphor
{
namespace logging
{
namespace metadata
{

using Metadata = std::string;

namespace associations
{

using Type = void(const std::string&,
                  const std::vector<std::string>&,
                  AssociationList& list);

/** @brief Pull out metadata name and value from the string
 *         <metadata name>=<metadata value>
 *  @param [in] data - metadata key=value entries
 *  @param [out] metadata - map of metadata name:value
 */
inline void parse(const std::vector<std::string>& data,
                  std::map<std::string, std::string>& metadata)
{
    constexpr auto separator = '=';
    for(const auto& entry: data)
    {
        auto pos = entry.find(separator);
        if(std::string::npos != pos)
        {
            auto key = entry.substr(0, entry.find(separator));
            auto value = entry.substr(entry.find(separator) + 1);
            metadata.emplace(std::move(key), std::move(value));
        }
    }
};

/** @brief Build error associations specific to metadata. Specialize this
 *         template for handling a specific type of metadata.
 *  @tparam M - type of metadata
 *  @param [in] match - metadata to be handled
 *  @param [in] data - metadata key=value entries
 *  @param [out] list - list of error association objects
 */
template <typename M>
void build(const std::string& match,
           const std::vector<std::string>& data,
           AssociationList& list) = delete;

// Example template specialization - we don't want to do anything
// for this metadata.
using namespace example::xyz::openbmc_project::Example::Elog;
template <>
inline void build<TestErrorTwo::DEV_ID>(const std::string& match,
                                        const std::vector<std::string>& data,
                                        AssociationList& list)
{
}

template <>
inline void build<example::xyz::openbmc_project::
                  Example::Device::Callout::CALLOUT_DEVICE_PATH_TEST>(
    const std::string& match,
    const std::vector<std::string>& data,
    AssociationList& list)
{
    constexpr auto ROOT = "/xyz/openbmc_project/inventory";
    std::map<std::string, std::string> metadata;
    parse(data, metadata);
    auto iter = metadata.find(match);
    if(metadata.end() != iter)
    {
        auto comp = [](const auto& first, const auto& second)
        {
            return (strcmp(std::get<0>(first), second) < 0);
        };
        auto callout = std::lower_bound(callouts.begin(),
                                        callouts.end(),
                                        (iter->second).c_str(),
                                        comp);
        if((callouts.end() != callout) &&
           !strcmp((iter->second).c_str(), std::get<0>(*callout)))
        {
            list.push_back(std::make_tuple("callout",
                                           "fault",
                                           std::string(ROOT) +
                                           std::get<1>(*callout)));
        }
    }
}

#if defined PROCESS_META

template <>
void build<xyz::openbmc_project::Common::
           Callout::Device::CALLOUT_DEVICE_PATH>(
    std::string&& match,
    const std::vector<std::string>& data,
    AssociationList& list);

#endif // PROCESS_META


} // namespace associations
} // namespace metadata
} // namespace logging
} // namespace phosphor
