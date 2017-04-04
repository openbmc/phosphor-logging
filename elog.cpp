#include "config.h"
#include <phosphor-logging/elog.hpp>

namespace phosphor
{
namespace logging
{
namespace details
{
void commit(const char* name)
{
    using phosphor::logging::log;
    constexpr auto MAPPER_BUSNAME = "xyz.openbmc_project.ObjectMapper";
    constexpr auto MAPPER_PATH = "/xyz/openbmc_project/object_mapper";
    constexpr auto MAPPER_INTERFACE = "xyz.openbmc_project.ObjectMapper";

    constexpr auto IFACE_INTERNAL("xyz.openbmc_project.Logging.Internal.Manager");

    // Transaction id is located at the end of the string separated by a period.

    auto b = sdbusplus::bus::new_default();
    auto mapper = b.new_method_call(
            MAPPER_BUSNAME,
            MAPPER_PATH,
            MAPPER_INTERFACE,
            "GetObject");
    mapper.append(OBJ_INTERNAL, std::vector<std::string>({IFACE_INTERNAL}));

    auto mapperResponseMsg = b.call(mapper);
    if (mapperResponseMsg.is_method_error())
    {
        log<level::ERR>("Error in mapper call");
        return;
    }

    std::map<std::string, std::vector<std::string>> mapperResponse;
    mapperResponseMsg.read(mapperResponse);
    if (mapperResponse.empty())
    {
        log<level::ERR>("Error reading mapper response");
        return;
    }

    const auto& host = mapperResponse.cbegin()->first;
    auto m = b.new_method_call(
            host.c_str(),
            OBJ_INTERNAL,
            IFACE_INTERNAL,
            "Commit");
    uint64_t id = sdbusplus::server::transaction::get_id();
    m.append(id, name);
    b.call_noreply(m);
}
} // namespace details

void commit(const char* name)
{
    log<level::ERR>("method is deprecated, use commit() with exception type");
    phosphor::logging::details::commit(name);
}

} // namespace logging
} // namespace phosphor

