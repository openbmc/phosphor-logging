#include <sys/syslog.h>

#include <nlohmann/json.hpp>
#include <phosphor-logging/lg2/commit.hpp>
#include <sdbusplus/async.hpp>
#include <sdbusplus/exception.hpp>
#include <xyz/openbmc_project/Logging/Create/client.hpp>
#include <xyz/openbmc_project/Logging/Entry/client.hpp>

namespace lg2::details
{

using Create = sdbusplus::client::xyz::openbmc_project::logging::Create<>;
using Entry = sdbusplus::client::xyz::openbmc_project::logging::Entry<>;

/* Convert syslog severity to Entry::Level */
static auto severity_from_syslog(int s) -> Entry::Level
{
    switch (s)
    {
        case LOG_DEBUG:
            return Entry::Level::Debug;

        case LOG_INFO:
            return Entry::Level::Informational;

        case LOG_NOTICE:
            return Entry::Level::Notice;

        case LOG_WARNING:
            return Entry::Level::Warning;

        case LOG_ERR:
            return Entry::Level::Error;

        case LOG_CRIT:
            return Entry::Level::Critical;

        case LOG_ALERT:
            return Entry::Level::Alert;

        case LOG_EMERG:
            return Entry::Level::Emergency;
    }
    return Entry::Level::Emergency;
}

using AdditionalData_t = std::map<std::string, std::string>;

/* Create AdditionalData from the sdbusplus event json. */
static auto data_from_json(sdbusplus::exception::generated_event_base& t)
    -> AdditionalData_t
{
    AdditionalData_t result{};

    auto j = t.to_json()[t.name()];
    for (const auto& item : j.items())
    {
        result.emplace(item.key(), item.value().dump());
    }

    return result;
}

auto commit(sdbusplus::exception::generated_event_base&& t)
    -> sdbusplus::message::object_path
{
    auto b = sdbusplus::bus::new_default();
    auto m = b.new_method_call(Create::default_service, Create::instance_path,
                               Create::interface, "Create");

    m.append(t.name(), severity_from_syslog(t.severity()), data_from_json(t));

    auto reply = b.call(m);

    return reply.unpack<sdbusplus::message::object_path>();
}

auto commit(sdbusplus::async::context& ctx,
            sdbusplus::exception::generated_event_base&& t)
    -> sdbusplus::async::task<sdbusplus::message::object_path>
{
    co_return co_await Create(ctx)
        .service(Create::default_service)
        .path(Create::instance_path)
        .create(t.name(), severity_from_syslog(t.severity()),
                data_from_json(t));
}

} // namespace lg2::details
