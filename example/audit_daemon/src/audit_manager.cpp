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

void Manager::notify(uint8_t type, uint32_t requestId, int32_t rc,
                     std::string user, std::vector<uint8_t> sockaddrStruct,
                     std::vector<uint8_t> data)
{
    std::stringstream msg;

    msg << "Item:"
        << " type[" << std::to_string(type) << "]"
        << " requestId[" << requestId << "]"
        << " return code[" << rc << "]"
        << " user[" << user << "]"
        << " data size[" << data.size() << "]"
        << " sockaddr size[" << sockaddrStruct.size() << "] ";

    if (requestId == POWEROP)
    {
        // Generate SEL for POWER operation.
    }

    // send notify message to the linux audit as example
    _auditd->LogEvent(msg.str() + _config->getMessage(requestId),
                      std::string("parse_sockaddrstruct()"),
                      std::string("get_hostname()"),
                      std::string("no_tty()"));
}

} // namespace audit
} // namespace logging
} // namespace phosphor
