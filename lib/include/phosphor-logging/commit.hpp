#pragma once

#include <phosphor-logging/lg2/commit.hpp>
#include <sdbusplus/async.hpp>
#include <sdbusplus/exception.hpp>

namespace lg2
{
/** Commit a generated event/error.
 *
 *  @param t - The event to commit.
 *  @return The object path of the resulting event.
 *
 *  Note: Similar to elog(), this will use the default dbus connection to
 *  perform the operation.
 */
template <typename T>
    requires std::is_base_of_v<sdbusplus::exception::generated_event_base, T>
auto commit(T&& t) -> sdbusplus::message::object_path
{
    return details::commit(std::forward<T>(t));
}

/** Commit a generated event/error (using async context).
 *
 *  @param ctx - The async context to use.
 *  @param t - The event to commit.
 *  @return The object path of the resulting event.
 */
template <typename T>
    requires std::is_base_of_v<sdbusplus::exception::generated_event_base, T>
auto commit(sdbusplus::async::context& ctx,
            T&& t) -> sdbusplus::async::task<sdbusplus::message::object_path>
{
    return details::commit(ctx, std::forward<T>(t));
}

} // namespace lg2
