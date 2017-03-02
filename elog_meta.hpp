#pragma once

#include <vector>
#include <string>
#include <iostream>
#include "elog-gen.hpp"
#include "elog_entry.hpp"

namespace phosphor
{
namespace logging
{
namespace metadata
{

using Metadata = std::string;
using HandlerType = void(const std::vector<std::string>&,
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
 *  @param [in] data - metadata key=value entries
 *  @param [out] list - list of error association objects
 */
template <typename M>
void handler(const std::vector<std::string>& data,
             AssociationList& list) = delete;

// Example template specialization - we don't want to do anything
// for this metadata.
using namespace example::xyz::openbmc_project::Example;
template <>
inline void handler<TestErrorTwo::DEV_ID>(const std::vector<std::string>& data,
                                          AssociationList& list)
{
}

} // namespace metadata
} // namespace logging
} // namespace phosphor
