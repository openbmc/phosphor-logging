#pragma once

#include "config.h"
#include <sdbusplus/bus.hpp>

namespace phosphor
{
namespace rsyslog_utils
{

/** @brief Restart rsyslog's systemd unit
 */
void restart()
{
    auto bus = sdbusplus::bus::new_default();
    auto method = bus.new_method_call(
                      SYSTEMD_BUSNAME,
                      SYSTEMD_PATH,
                      SYSTEMD_INTERFACE,
                      "RestartUnit");
    method.append("rsyslog.service", "replace");
    bus.call_noreply(method);
}

} // namespace rsyslog_utils
} // namespace phosphor
