#include <boost/asio/generic/raw_protocol.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "audit_manager.hpp"

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
        // Generate audit SEL for POWER operation.
    }

    boost::asio::generic::raw_protocol::endpoint rep(
        reinterpret_cast<struct sockaddr*>(sockaddrStruct.data()),
        sizeof(sockaddrStruct));
    if (rep.protocol().family() == AF_INET)
    {
        boost::asio::ip::tcp::endpoint tcpEp;
        auto rawIpAddr = ntohl(
            reinterpret_cast<const sockaddr_in*>(rep.data())->sin_addr.s_addr);
        auto port =
            ntohs(reinterpret_cast<const sockaddr_in*>(rep.data())->sin_port);
        auto ipAddr = boost::asio::ip::make_address_v4(rawIpAddr);

        _auditd->LogEvent(msg.str() + _config->getMessage(requestId),
                          ipAddr.to_string() + "/" + std::to_string(port),
                          std::string("get_hostname()"),
                          std::string("no_tty()"));
    }
    else if (rep.protocol().family() == AF_INET6)
    {
        boost::asio::ip::tcp::endpoint tcpEp;
        boost::asio::ip::address_v6::bytes_type bytes;

        auto port =
            ntohs(reinterpret_cast<const sockaddr_in6*>(rep.data())->sin6_port);
        std::memcpy(bytes.data(),
                    reinterpret_cast<const sockaddr_in6*>(rep.data())
                        ->sin6_addr.s6_addr,
                    16);
        auto scopeId =
            reinterpret_cast<const sockaddr_in6*>(rep.data())->sin6_scope_id;

        auto ip6Addr = boost::asio::ip::make_address_v6(bytes, scopeId);
        _auditd->LogEvent(msg.str() + _config->getMessage(requestId),
                          ip6Addr.to_string() + "/" + std::to_string(scopeId) +
                              " port:" + std::to_string(port),
                          std::string("get_hostname()"),
                          std::string("no_tty()"));
    }
    else
        // send notify message to the linux audit as example
        _auditd->LogEvent(
            msg.str() + _config->getMessage(requestId), std::string("clientIp"),
            std::string("get_hostname()"), std::string("no_tty()"), rc);
}

} // namespace audit
} // namespace logging
} // namespace phosphor
