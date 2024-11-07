#pragma once

#include <sdbusplus/async.hpp>
#include <sdbusplus/exception.hpp>

namespace lg2
{
/** Commit a generated event/error.
 *
 *  @param e - The event to commit.
 *  @return The object path of the resulting event.
 *
 *  Note: Similar to elog(), this will use the default dbus connection to
 *  perform the operation.
 */
auto commit(sdbusplus::exception::generated_event_base&& e)
    -> sdbusplus::message::object_path;

/** Commit a generated event/error (using async context).
 *
 *  @param ctx - The async context to use.
 *  @param e - The event to commit.
 *  @return The object path of the resulting event.
 */
auto commit(sdbusplus::async::context& ctx,
            sdbusplus::exception::generated_event_base&& e)
    -> sdbusplus::async::task<sdbusplus::message::object_path>;
} // namespace lg2
