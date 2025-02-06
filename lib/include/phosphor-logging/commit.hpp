#pragma once

#include <sdbusplus/async.hpp>
#include <sdbusplus/exception.hpp>
#include <xyz/openbmc_project/Logging/Entry/common.hpp>

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
            sdbusplus::exception::generated_event_base&& e)
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

using AdditionalData_t = std::map<std::string, std::string>;

/**
 * Commit a generated event/error with specified name, severity, and additional
 * data.
 *
 * @param name - The name of the event to commit.
 * @param severity - The severity level of the event.
 * @param data - Additional data associated with the event, represented as a map
 * of strings.
 * @return The object path of the resulting event.
 *
 * Note: This function will use the default dbus connection to perform the
 * operation.
 */
auto commit(
    std::string name,
    sdbusplus::common::xyz::openbmc_project::logging::Entry::Level severity,
    AdditionalData_t& data) -> sdbusplus::message::object_path;

} // namespace lg2