#include "config.h"

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

void commit(const char* name)
{
    auto msg = _prepareMsg("Commit");
    uint64_t id = sdbusplus::server::transaction::get_id();
    msg.append(id, name);
    auto bus = sdbusplus::bus::new_default();
    bus.call_noreply(msg);
}

void commit(const char* name, Entry::Level level)
{
    auto msg = _prepareMsg("CommitWithLvl");
    uint64_t id = sdbusplus::server::transaction::get_id();
    msg.append(id, name, static_cast<uint32_t>(level));
    auto bus = sdbusplus::bus::new_default();
    bus.call_noreply(msg);
}
} // namespace details

void commit(std::string&& name)
{
    log<level::ERR>("method is deprecated, use commit() with exception type");
    phosphor::logging::details::commit(name.c_str());
}

} // namespace logging
} // namespace phosphor
