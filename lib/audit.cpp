#include <linux/mctp.h>
#include <linux/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/uio.h>

#include <phosphor-logging/audit/audit.hpp>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/message.hpp>

#include <algorithm>
#include <bitset>
#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <mutex>
#include <source_location>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace phosphor
{
namespace logging
{
namespace audit
{
namespace details
{

auto _prepareMsg(const char* funcName)
{
    constexpr auto IFACE_INTERNAL("xyz.openbmc_project.Logging.Audit.Manager");
    constexpr auto BUSNAME("xyz.openbmc_project.Logging.Audit");
    constexpr auto OBJ_INTERNAL("/xyz/openbmc_project/logging/audit/manager");

    auto bus = sdbusplus::bus::new_default();

    auto method =
        bus.new_method_call(BUSNAME, OBJ_INTERNAL, IFACE_INTERNAL, funcName);
    return method;
}

int32_t audit_event(uint8_t type, uint32_t requestId, int32_t rc,
                      std::string user, std::vector<uint8_t> sockAddrStruct,
                      std::vector<uint8_t> data)
{
    int32_t ret = 0;
    auto msg = _prepareMsg("Notify");

    msg.append(type, requestId, rc, user, sockAddrStruct, data);
    auto bus = sdbusplus::bus::new_default();

    bus.call_noreply(msg);
    return ret;
}

} // namespace details

/*
  https://gerrit.openbmc.org/plugins/gitiles/openbmc/docs/+/9f5e2d83a3b9fd852422f735c55c41892de4c590/designs/phosphor-audit.md

  struct blob_audit
  {
  uint8_t type;
  int32_t rc;
  uint32_t request_id;
  char user;
  sockaddr_* addr; // in/in6/mctp
  struct iovec *data;
  }
*/
int32_t audit_event(uint8_t type, uint32_t requestId, int32_t rc,
                      std::string user, struct sockaddr* sockAddrStruct,
                      size_t sockAddrStructSize, struct iovec* iovecStruct)
{
    std::vector<uint8_t> sockAddrVec;
    std::vector<uint8_t> dataVec;

    if (sockAddrStruct != nullptr)
    {
        // validate passed struct size
        switch (sockAddrStruct->sa_family)
        {
            case AF_INET:
                sockAddrVec.reserve(sizeof(struct sockaddr_in) + 1);
                if (sizeof(struct sockaddr_in) == sockAddrStructSize)
                    sockAddrVec.assign((uint8_t *)sockAddrStruct,
                                       (uint8_t *)sockAddrStruct + sockAddrStructSize);
                break;

            case AF_INET6:
                sockAddrVec.reserve(sizeof(struct sockaddr_in6) + 1);
                if (sizeof(struct sockaddr_in6) == sockAddrStructSize)
                    sockAddrVec.assign((uint8_t *)sockAddrStruct,
                                       (uint8_t *)sockAddrStruct + sockAddrStructSize);
                break;

            case AF_MCTP:
                sockAddrVec.reserve(sizeof(struct sockaddr_mctp) + 1);
                if (sizeof(struct sockaddr_mctp) == sockAddrStructSize)
                    sockAddrVec.assign((uint8_t *)sockAddrStruct,
                                       (uint8_t *)sockAddrStruct + sockAddrStructSize);

                break;
        }
    }

    if (iovecStruct != nullptr)
    {
        dataVec.assign((uint8_t*)iovecStruct->iov_base,
                       (uint8_t*)iovecStruct->iov_base + iovecStruct->iov_len);
    }

    return phosphor::logging::audit::details::audit_event(
        type, requestId, rc, user, sockAddrVec, dataVec);
}

} // namespace audit
} // namespace logging
} // namespace phosphor
