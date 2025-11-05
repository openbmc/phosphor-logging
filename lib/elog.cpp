#include "config.h"

#include <phosphor-logging/elog.hpp>
#include <phosphor-logging/lg2.hpp>

#include <stdexcept>

namespace phosphor
{
namespace logging
{
namespace details
{
using namespace sdbusplus::server::xyz::openbmc_project::logging;

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
    auto entryID = reply.unpack<uint32_t>();

    return entryID;
}

uint32_t commit(const char* name, Entry::Level level)
{
    auto msg = _prepareMsg("CommitWithLvl");
    uint64_t id = sdbusplus::server::transaction::get_id();
    msg.append(id, name, static_cast<uint32_t>(level));
    auto bus = sdbusplus::bus::new_default();
    auto reply = bus.call(msg);
    auto entryID = reply.unpack<uint32_t>();

    return entryID;
}
} // namespace details

uint32_t commit(std::string&& name)
{
    lg2::error("method is deprecated, use commit() with exception type");
    return phosphor::logging::details::commit(name.c_str());
}

} // namespace logging
} // namespace phosphor
