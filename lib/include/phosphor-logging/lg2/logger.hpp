#pragma once

#include <phosphor-logging/lg2/level.hpp>

#include <cstddef>
#include <source_location>

namespace lg2::details
{

/** Low-level function that actually performs the logging.
 *
 *  This is a variadic argument function (std::va_list) where the variadic
 *  arguments are a set of 3: { header, flags, value }.
 *
 *  The argument set is finalized with a 'nullptr' valued header.
 *
 *  @param[in] level - The logging level to use.
 *  @param[in] source_location - The original source location of the upper-level
 *                               log call.
 *  @param[in] char* - The primary message to log.
 */
void do_log(level, const std::source_location&, const char*, ...);

} // namespace lg2::details
