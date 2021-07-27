#pragma once

#include <cstddef>
#include <phosphor-logging/lg2/level.hpp>
#include <source_location>
#include <string_view>

namespace lg2::details
{

/** Low-level function that actually performs the logging.
 *
 *  This is a variadic argument function (std::va_list) where the variadic
 *  arguments are a set of 3: { header, flags, value }.
 *
 *  @param[in] level - The logging level to use.
 *  @param[in] source_location - The original source location of the upper-level
 *                               log call.
 *  @param[in] string_view - The primary message to log.
 *  @param[in] size_t - The number of sets in the variadic arguments.
 */
void do_log(level, const std::source_location&, const std::string_view&, size_t,
            ...);

} // namespace lg2::details
