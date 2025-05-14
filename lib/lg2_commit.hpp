#pragma once
#include <sdbusplus/exception.hpp>
#include <xyz/openbmc_project/Logging/Entry/client.hpp>

#include <string>
#include <tuple>
#include <vector>

namespace lg2::details
{

using Entry = sdbusplus::client::xyz::openbmc_project::logging::Entry<>;

/** Extract the message, level, and additional data from agenerated event.
 *
 *  @param[in] The event to extract data from.
 *  @return A tuple containing the message, level, and additional data.
 */
auto extractEvent(sdbusplus::exception::generated_event_base&&)
    -> std::tuple<std::string, Entry::Level,
                  std::map<std::string, std::string>>;

bool filterEvent(const std::string&);
bool filterError(const std::string&);

} // namespace lg2::details
