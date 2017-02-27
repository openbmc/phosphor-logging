#pragma once

#include <vector>
#include <string>
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

/** @brief Metadata handler, specialize this template to implement
 *         handling a specific type of metadata.
 *  @tparam M - type of metadata
 *  @param [in] data - metadata key=value entries
 *  @param [out] list - list of error association objects
 */
template <typename M>
void handler(const std::vector<std::string>& data,
             AssociationList& list) = delete;

} // namespace metadata
} // namespace logging
} // namespace phosphor
