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

uint32_t notify_event(
    uint64_t type, uint64_t requestId, uint64_t rc, std::string user,
    struct sockaddr* sockAddrStruct = nullptr, size_t sockAddrStructSize = 0u,
    struct iovec* iovecStruct = nullptr);

} // namespace audit
} // namespace logging
} // namespace phosphor
