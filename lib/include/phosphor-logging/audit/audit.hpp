#pragma once

#include <boost/asio/generic/raw_protocol.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <linux/mctp.h>
#include <linux/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/uio.h>

#include <sdbusplus/bus.hpp>
#include <sdbusplus/message.hpp>

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace phosphor
{
namespace logging
{
namespace audit
{

// TODO: make enums
static constexpr auto HOSTIPMI = 1;
static constexpr auto NETIPMI = 2;
static constexpr auto BMCWEB = 3;

// TODO: make enums
// requestId operations
static constexpr uint32_t POWEROP = 1;
static constexpr uint32_t SETUSERPASSWD = 2;

int32_t auditEvent(uint8_t type, uint32_t requestId, int32_t rc,
                   std::string user, struct sockaddr* sockAddrStruct = nullptr,
                   size_t sockAddrStructSize = 0u,
                   struct iovec* iovecStruct = nullptr);

template <typename KeyType, typename AllowListEntry>
void generalizedAuditEvent(
    sdbusplus::bus::bus& bus, std::optional<bool>& enabledFlag,
    std::optional<std::vector<AllowListEntry>>& allowList,
    const std::string& dbusService, const std::string& dbusPath,
    const std::string& dbusInterface, const std::string& enabledProperty,
    const std::string& allowListProperty, const KeyType& key,
    std::function<bool(const AllowListEntry&, const KeyType&)> matcher,
    std::function<void(uint32_t auditId)> auditCallback);

void hostIpmiAuditEvent(
    int32_t rc, uint8_t netFn, uint8_t cmd,
    std::optional<bool>& ipmiAuditEnabled,
    std::optional<std::vector<std::tuple<uint8_t, uint8_t, uint32_t>>>&
        ipmiAllowList);

void bmcwebAuditEvent(
    uint32_t pathId, int32_t retval,
    boost::asio::ip::tcp::endpoint& ep,
    std::string& username,
    std::optional<bool>& bmcwebAuditEnabled,
    std::optional<std::vector<std::tuple<uint32_t, uint32_t>>>&
        bmcwebAllowList);

} // namespace audit
} // namespace logging
} // namespace phosphor
