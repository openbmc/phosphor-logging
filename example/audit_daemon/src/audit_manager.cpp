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

void Manager::notify(uint64_t type, uint64_t requestId, uint64_t rc,
                     std::string user, std::vector<uint8_t> sockaddrStruct,
                     std::vector<uint8_t> data)
{
    std::stringstream msg;

    msg << "Item: "
        << " type [ " << type << " ]\t"
        << " requestId [ " << requestId << " ]\t"
        << " return code [ " << rc << " ]\t"
        << " user [ " << user << " ]\t"
        << " data size [ " << data.size() << " ]\t"
        << " sockaddr size [ " << sockaddrStruct.size() << " ]\t";

    if (requestId == POWERON)
    {
        // do_something
    }

    // send notify message to the linux audit as example
    if (_auditd->IsInitSuccess())
    {
        _auditd->LogEvent(msg.str(), "Hello");
    }

    std::cout << msg.str() << std::endl;
}

} // namespace audit
} // namespace logging
} // namespace phosphor
