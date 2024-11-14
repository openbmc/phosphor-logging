/**
 * @brief phosphor audit API implementation
 *
 * This file is part of the phosphor-logging audit project.
 *
 */

#include <algorithm>
#include <bitset>
#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <mutex>
#include <source_location>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <sdbusplus/message.hpp>
#include <sdbusplus/bus.hpp>

#include <phosphor-logging/audit/audit.hpp>
#include <xyz/openbmc_project/Logging/Audit/Manager/server.hpp>


namespace phosphor {
namespace logging {
namespace audit {
namespace details {

auto _prepareMsg(const char *funcName) {
  constexpr auto IFACE_INTERNAL("xyz.openbmc_project.Logging.Audit.Manager");

  constexpr auto BUSNAME("xyz.openbmc_project.Logging.Audit");

  constexpr auto OBJ_INTERNAL("/xyz/openbmc_project/logging/audit/manager");

  auto bus = sdbusplus::bus::new_default();

  auto method =
      bus.new_method_call(BUSNAME, OBJ_INTERNAL, IFACE_INTERNAL, funcName);
  return method;
}

uint32_t notify_event(std::uint64_t type, std::uint64_t requestId,
                      std::uint64_t rc, std::string user) {
  uint32_t ret = 0;
  auto msg = _prepareMsg("Notify");
  uint64_t id = sdbusplus::server::transaction::get_id();
  msg.append(type, requestId, rc, user);
  auto bus = sdbusplus::bus::new_default();

  bus.call_noreply(msg);
  return ret;
}

} // namespace details

uint32_t notify_event(std::uint64_t type, std::uint64_t requestId,
                      std::uint64_t rc, std::string user) {
  return phosphor::logging::audit::details::notify_event(type, requestId, rc,
                                                         user);
}

} // namespace audit
} // namespace logging
} // namespace phosphor
