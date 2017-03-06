#pragma once

#include <vector>
#include <string>
#include <tuple>
#include <algorithm>
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
using HandlerType = void(std::string&&,
                         const std::vector<std::string>&,
                         AssociationList& list);

/** @brief Pull out metadata name and value from the string
 *         <metadata name>=<metadata value>
 *  @param [in] data - metadata key=value entries
 *  @param [out] metadata - map of metadata name:value
 */
auto parse = [](const std::vector<std::string>& data,
                std::map<std::string, std::string>& metadata)
{
    constexpr auto separator = "=";
    for(const auto& entry: data)
    {
        auto pos = entry.find(separator);
        if(std::string::npos != pos)
        {
            auto key = entry.substr(0, entry.find(separator));
            auto value = entry.substr(entry.find(separator) + 1);
            metadata[std::move(key)] = std::move(value);
        }
    }
};

/** @brief Metadata handler, specialize this template to implement
 *         handling a specific type of metadata.
 *  @tparam M - type of metadata
 *  @param [in] match - metadata to be handled
 *  @param [in] data - metadata key=value entries
 *  @param [out] list - list of error association objects
 */
template <typename M>
void handler(std::string&& match,
             const std::vector<std::string>& data,
             AssociationList& list) = delete;

// Example template specialization - we don't want to do anything
// for this metadata.
using namespace example::xyz::openbmc_project::Example::Elog;
template <>
inline void handler<TestErrorTwo::DEV_ID>(std::string&& match,
                                          const std::vector<std::string>& data,
                                          AssociationList& list)
{
}

template <>
inline void handler<example::xyz::openbmc_project::
                    Example::Device::Callout::CALLOUT_DEVICE_PATH_TEST>(
    std::string&& match,
    const std::vector<std::string>& data,
    AssociationList& list)
{
    using namespace std::string_literals;
    constexpr auto ROOT = "/xyz/openbmc_project/inventory";
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
            list.push_back(std::make_tuple("callout",
                                           "fault",
                                           std::string(ROOT) +
                                           std::get<1>(*callout)));
        }
    }
}

#if defined PROCESS_META

template <>
void handler<xyz::openbmc_project::Common::
             Callout::Device::CALLOUT_DEVICE_PATH>(
    std::string&& match,
    const std::vector<std::string>& data,
    AssociationList& list);

#endif // PROCESS_META


} // namespace metadata
} // namespace logging
} // namespace phosphor
