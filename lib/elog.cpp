#include "config.h"

#include <map>
#include <phosphor-logging/elog.hpp>
#include <stdexcept>

namespace phosphor
{
namespace logging
{
namespace details
{
using namespace sdbusplus::xyz::openbmc_project::Logging::server;

auto _prepareMsg(const char* funcName)
{
    using phosphor::logging::log;
    constexpr auto IFACE_INTERNAL(
        "xyz.openbmc_project.Logging.Internal.Manager");

    // Transaction id is located at the end of the string separated by a period.

    auto b = sdbusplus::bus::new_default();

    auto m = b.new_method_call(BUSNAME_LOGGING, OBJ_INTERNAL, IFACE_INTERNAL,
                               funcName);
    return m;
}

uint32_t commit(const char* name)
{
    auto msg = _prepareMsg("Commit");
    uint64_t id = sdbusplus::server::transaction::get_id();
    msg.append(id, name);
    auto bus = sdbusplus::bus::new_default();
    auto reply = bus.call(msg);
    uint32_t entryID;
    reply.read(entryID);
    return entryID;
}

uint32_t commit(const char* name, Entry::Level level)
{
    auto msg = _prepareMsg("CommitWithLvl");
    uint64_t id = sdbusplus::server::transaction::get_id();
    msg.append(id, name, static_cast<uint32_t>(level));
    auto bus = sdbusplus::bus::new_default();
    auto reply = bus.call(msg);
    uint32_t entryID;
    reply.read(entryID);
    return entryID;
}
} // namespace details

uint32_t commit(std::string&& name)
{
    log<level::ERR>("method is deprecated, use commit() with exception type");
    return phosphor::logging::details::commit(name.c_str());
}

std::string getMsgId(const std::string& consumer)
{
    static std::map<std::string, std::string> consumerMaps = {
        {"audit", "AUDIT_MESSSAGE_ID=%s"},
        {"redfish", "REDFISH_MESSSAGE_ID=%s"},
        {"sel", "SEL_MESSSAGE_ID=%s"}};
    if (consumerMaps.contains(consumer))
    {
        return consumerMaps.at(consumer);
    }

    return {};
}

void audit()
{
    try
    {
        constexpr auto aduitBusName = "xyz.openbmc_project.Audit";
        constexpr auto auditObjPath = "/xyz/openbmc_project/Audit";
        constexpr auto auditIntf = "xyz.openbmc_project.Audit.Manager";
        constexpr auto auditMethod = "Commit";

        auto b = sdbusplus::bus::new_default();

        auto msg = b.new_method_call(aduitBusName, auditObjPath, auditIntf,
                                     auditMethod);
        uint64_t id = sdbusplus::server::transaction::get_id();
        msg.append(id);
        auto bus = sdbusplus::bus::new_default();
        auto reply = bus.call(msg);
    }
    catch (const std::exception&)
    {
    }
}

} // namespace logging
} // namespace phosphor
