/**
 * @brief The phosphor audit manager processing changes specific
 * methods/properties on a D-bus and call methods on interfaces
 *
 * This file is part of the phosphor-logging audit project.
 *
 */

#include "audit_manager.hpp"

#include "audit_common.hpp"

#include <cstdint>
#include <iostream>
#include <sstream>

namespace phosphor
{
namespace logging
{
namespace audit
{

void Manager::notify(std::string ip, uint64_t type, uint64_t requestId,
                     uint64_t rc, std::string user, std::vector<uint8_t> data)
{
    std::stringstream msg;

    msg << "Item: "
        << " type [ " << type << " ]\t"
        << " requestId [ " << requestId << " ]\t"
        << " return code [ " << rc << " ]\t"
        << " user [ " << user << " ]\t";

    // send notify message to the linux audit as example
    if (_linuxAudit->IsInitSuccess())
    {
        _linuxAudit->LogEvent(msg.str(), ip);
    }

    std::cout << msg.str() << std::endl;
}

} // namespace audit
} // namespace logging
} // namespace phosphor
