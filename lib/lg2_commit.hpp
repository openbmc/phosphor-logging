#pragma once

#include "event_extensions/request.hpp"

#include <sdbusplus/exception.hpp>
#include <xyz/openbmc_project/Logging/Entry/client.hpp>

#include <map>
#include <string>

namespace lg2::details
{

using Entry = sdbusplus::client::xyz::openbmc_project::logging::Entry<>;

/**
 * Information extracted from a generated event.
 */
struct EventInfo
{
    /** Event message identifier. */
    std::string message;

    /** Event level. */
    Entry::Level level;

    /** Event metadata. */
    std::map<std::string, std::string> additionalData;

    /** Optional event extension requests. */
    phosphor::logging::event_extensions::RequestList extensions;
};

/** Extract information from a generated event.
 *
 * @param[in] event - The event to extract data from.
 *
 * @return Extracted event information.
 */
auto extractEvent(sdbusplus::exception::generated_event_base&& event)
    -> EventInfo;

bool filterEvent(const std::string&);
bool filterError(const std::string&);

} // namespace lg2::details
