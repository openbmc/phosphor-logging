#pragma once

#include <vector>
#include <string>
#include <phosphor-logging/elog-errors.hpp>
#include "elog_entry.hpp"

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

} // namespace associations
} // namespace metadata
} // namespace logging
} // namespace phosphor
