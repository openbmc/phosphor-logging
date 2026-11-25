#pragma once

#include <sdbusplus/bus.hpp>
#include <sdbusplus/server/interface.hpp>

namespace lg2
{

/** Serves org.freedesktop.LogControl1 at /org/freedesktop/LogControl1.
 *
 *  Implements the freedesktop standard interface for runtime log control,
 *  allowing `systemctl service-log-level <unit> debug` to adjust this
 *  process' lg2 journal output without a restart.
 *
 *  The owning unit must declare `BusName=` for systemctl to locate the
 *  service.  Writes are restricted to privileged callers by sd-bus.
 *
 *  Construct one instance per process, on a bus that is attached to the
 *  process' event loop, and keep it alive for the lifetime of the daemon.
 */
class log_control
{
  public:
    log_control() = delete;
    log_control(const log_control&) = delete;
    log_control& operator=(const log_control&) = delete;
    log_control(log_control&&) = delete;
    log_control& operator=(log_control&&) = delete;
    ~log_control() = default;

    explicit log_control(sdbusplus::bus_t& bus);

  private:
    sdbusplus::server::interface_t iface;
};

} // namespace lg2
