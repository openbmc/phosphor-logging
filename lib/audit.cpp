#include <phosphor-logging/audit/audit.hpp>

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

template <typename T>
T getPropertySync(sdbusplus::bus::bus& bus, const std::string& service,
                  const std::string& path, const std::string& interface,
                  const std::string& propertyName)
{
    auto method = bus.new_method_call(service.c_str(), path.c_str(),
                                      "org.freedesktop.DBus.Properties", "Get");
    method.append(interface, propertyName);
    auto reply = bus.call(method);
    std::variant<T> value;
    reply.read(value);
    return std::get<T>(value);
}

/*
  https://gerrit.openbmc.org/plugins/gitiles/openbmc/docs/+/9f5e2d83a3b9fd852422f735c55c41892de4c590/designs/phosphor-audit.md

  struct blob_audit
  {
  uint8_t type;
  int32_t rc;
  uint32_t request_id;
  uint16_t user;
  sockaddr_* addr; // in/in6/mctp
  struct iovec *data;
  }
*/
int32_t auditEvent(uint8_t type, uint32_t requestId, int32_t rc,
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
                    sockAddrVec.assign((uint8_t*)sockAddrStruct,
                                       (uint8_t*)sockAddrStruct +
                                           sockAddrStructSize);
                break;

            case AF_INET6:
                sockAddrVec.reserve(sizeof(struct sockaddr_in6) + 1);
                if (sizeof(struct sockaddr_in6) == sockAddrStructSize)
                    sockAddrVec.assign((uint8_t*)sockAddrStruct,
                                       (uint8_t*)sockAddrStruct +
                                           sockAddrStructSize);
                break;

            case AF_MCTP:
                sockAddrVec.reserve(sizeof(struct sockaddr_mctp) + 1);
                if (sizeof(struct sockaddr_mctp) == sockAddrStructSize)
                    sockAddrVec.assign((uint8_t*)sockAddrStruct,
                                       (uint8_t*)sockAddrStruct +
                                           sockAddrStructSize);

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

template <typename KeyType, typename AllowListEntry>
void generalizedAuditEvent(
    sdbusplus::bus::bus& bus, std::optional<bool>& enabledFlag,
    std::optional<std::vector<AllowListEntry>>& allowList,
    const std::string& dbusService, const std::string& dbusPath,
    const std::string& dbusInterface, const std::string& enabledProperty,
    const std::string& allowListProperty, const KeyType& key,
    std::function<bool(const AllowListEntry&, const KeyType&)> matcher,
    std::function<void(uint32_t auditId)> auditCallback)
{
    // 1. Fetch Enabled property if not cached.
    if (!enabledFlag.has_value())
    {
        enabledFlag = getPropertySync<bool>(bus, dbusService, dbusPath,
                                            dbusInterface, enabledProperty);
    }

    if (!*enabledFlag)
    {
        return;
    }

    // 2. Fetch AllowList property if not cached.
    if (!allowList.has_value())
    {
        allowList = getPropertySync<std::vector<AllowListEntry>>(
            bus, dbusService, dbusPath, dbusInterface, allowListProperty);
    }

    // 3. Search for matching entry and call the audit callback.
    for (const auto& entry : *allowList)
    {
        if (matcher(entry, key))
        {
            constexpr size_t auditIdPos =
                std::tuple_size<AllowListEntry>::value - 1;
            uint32_t auditId = std::get<auditIdPos>(entry);
            auditCallback(auditId);
            break;
        }
    }
}

void hostIpmiAuditEvent(
    int32_t rc, uint8_t netFn, uint8_t cmd,
    std::optional<bool>& ipmiAuditEnabled,
    std::optional<std::vector<std::tuple<uint8_t, uint8_t, uint32_t>>>&
        ipmiAllowList)
{
    // TODO: change to getBus/get_bus
    auto bus = sdbusplus::bus::new_default();

    auto matcher = [](const std::tuple<uint8_t, uint8_t, uint32_t>& entry,
                      const std::tuple<uint8_t, uint8_t>& key) -> bool {
        return (std::get<0>(entry) == std::get<0>(key)) && // cmd
               (std::get<1>(entry) == std::get<1>(key));   // netFn
    };

    auto auditor = [&](uint32_t auditId) {
        phosphor::logging::audit::auditEvent(phosphor::logging::audit::HOSTIPMI,
                                             auditId, rc, std::string("root"));
    };

    generalizedAuditEvent<std::tuple<uint8_t, uint8_t>,
                          std::tuple<uint8_t, uint8_t, uint32_t>>(
        bus, ipmiAuditEnabled, ipmiAllowList,
        "xyz.openbmc_project.Logging.Audit",
        "/xyz/openbmc_project/logging/audit/manager",
        "xyz.openbmc_project.Logging.Audit.AuditIPMI", "Enabled", "AllowList",
        std::make_tuple(cmd, netFn), matcher, auditor);
}

void bmcwebAuditEvent(
    uint32_t pathId, int32_t retval,
    boost::asio::ip::tcp::endpoint& ep,
    std::string& username,
    std::optional<bool>& bmcwebAuditEnabled,
    std::optional<std::vector<std::tuple<uint32_t, uint32_t>>>& bmcwebAllowList)
{
    // TODO: change to getBus/get_bus
    auto bus = sdbusplus::bus::new_default();

    auto matcher = [](const std::tuple<uint32_t, uint32_t>& entry,
                      uint32_t key) -> bool {
        return std::get<0>(entry) == key;
    };

    auto auditor = [&](uint32_t auditId) {
        phosphor::logging::audit::auditEvent(
            phosphor::logging::audit::BMCWEB, auditId, retval, username,
            ep.data(), ep.size());
    };

    generalizedAuditEvent<uint32_t, std::tuple<uint32_t, uint32_t>>(bus, bmcwebAuditEnabled, bmcwebAllowList,
                          "xyz.openbmc_project.Logging.Audit",
                          "/xyz/openbmc_project/logging/audit/manager",
                          "xyz.openbmc_project.Logging.Audit.AuditBMCWEB",
                          "Enabled", "AllowList", pathId, matcher, auditor);
}

} // namespace audit
} // namespace logging
} // namespace phosphor
