#pragma once

#include <sdbusplus/async.hpp>
#include <sdbusplus/exception.hpp>
#include <sdbusplus/message/native_types.hpp>

#include <optional>

// Older sdbusplus releases only define sdbusplus::message::object_path. Newer
// releases define sdbusplus::object_path as an alias for the same type, so
// this redeclaration is valid with both.
namespace sdbusplus
{
using object_path = message::details::string_path_wrapper;
}

namespace lg2
{
/** Commit a generated event/error.
 *
 *  @param e - The event to commit.
 *  @return The object path of the resulting event. The path is empty if the
 *          event has been filtered out or if the logging service does not
 *          report the path of new events (older phosphor-dbus-interfaces
 *          releases).
 *
 *  Note: Similar to elog(), this will use the default dbus connection to
 *  perform the operation.
 */
auto commit(sdbusplus::exception::generated_event_base&& e,
            std::optional<int> overrideLevel = std::nullopt)
    -> sdbusplus::object_path;

/** Resolve an existing event/error.
 *
 *  @param logPath - The object path of the event to resolve.
 *  @return None.
 *
 *  Note: Similar to elog(), this will use the default dbus connection to
 *  perform the operation.
 */
void resolve(const sdbusplus::object_path& logPath);

/** Commit a generated event/error (using async context).
 *
 *  @param ctx - The async context to use.
 *  @param e - The event to commit.
 *  @return The object path of the resulting event. The path is empty if the
 *          event has been filtered out or if the logging service does not
 *          report the path of new events (older phosphor-dbus-interfaces
 *          releases).
 */
auto commit(sdbusplus::async::context& ctx,
            sdbusplus::exception::generated_event_base&& e,
            std::optional<int> overrideLevel = std::nullopt)
    -> sdbusplus::async::task<sdbusplus::object_path>;

/** Resolve an existing event/error (using async context).
 *
 *  @param ctx - The async context to use.
 *  @param logPath - The object path of the event to resolve.
 *  @return None
 */
auto resolve(sdbusplus::async::context& ctx,
             const sdbusplus::object_path& logPath) -> sdbusplus::async::task<>;

} // namespace lg2
