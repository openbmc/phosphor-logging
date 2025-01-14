#pragma once

#include <linux/mctp.h>
#include <linux/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/uio.h>

#include <string>

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
//requestId operations
static constexpr uint32_t POWEROP = 1;
static constexpr uint32_t SETUSERPASSWD = 2;

int32_t audit_event(uint8_t type, uint32_t requestId, int32_t rc,
                    std::string user, struct sockaddr* sockAddrStruct = nullptr,
                    size_t sockAddrStructSize = 0u,
                    struct iovec* iovecStruct = nullptr);

} // namespace audit
} // namespace logging
} // namespace phosphor
