#pragma once

#include <sdbusplus/async.hpp>
#include <sdbusplus/exception.hpp>

#include <optional>

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
auto commit(sdbusplus::exception::generated_event_base&& e,
            std::optional<int> overrideLevel = std::nullopt)
    -> sdbusplus::message::object_path;

/** Resolve an existing event/error.
 *
 *  @param logPath - The object path of the event to resolve.
 *  @return None.
 *
 *  Note: Similar to elog(), this will use the default dbus connection to
 *  perform the operation.
 */
void resolve(const sdbusplus::message::object_path& logPath);

/** Commit a generated event/error (using async context).
 *
 *  @param ctx - The async context to use.
 *  @param e - The event to commit.
 *  @return The object path of the resulting event.
 */
auto commit(sdbusplus::async::context& ctx,
            sdbusplus::exception::generated_event_base&& e,
            std::optional<int> overrideLevel = std::nullopt)
    -> sdbusplus::async::task<sdbusplus::message::object_path>;

/** Resolve an existing event/error (using async context).
 *
 *  @param ctx - The async context to use.
 *  @param logPath - The object path of the event to resolve.
 *  @return None
 */
auto resolve(sdbusplus::async::context& ctx,
             const sdbusplus::message::object_path& logPath)
    -> sdbusplus::async::task<>;

} // namespace lg2
