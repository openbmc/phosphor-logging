/**
 * @brief The phosphor audit manager processing changes specific
 * methods/properties on a D-bus and call methods on interfaces
 *
 * This file is part of the phosphor-logging audit project.
 *
 */

#include <sstream>
#include <iostream>
#include <cstdint>

#include "audit_manager.hpp"

namespace phosphor {
namespace logging {
namespace audit {

void Manager::notify(std::uint64_t type, std::uint64_t requestId,
                     std::uint64_t rc, std::string user) {

  std::stringstream msg;

  msg << "Item: "
      << " type [ " << type << " ]\t"
      << " requestId [ " << requestId << " ]\t"
      << " return code [ " << rc << " ]\t"
      << " user [ " << user << " ]\t";

  // send notify message to the linux audit as example
  if (_linuxAudit->IsInitSuccess()) {
    _linuxAudit->LogEvent(msg.str());
  }

  std::cout << msg.str() << std::endl;
}

} // namespace audit
} // namespace logging
} // namespace phosphor
