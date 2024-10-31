#include "lg2_commit.hpp"

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
        // Special cases for the "_SOURCE" fields, which contain debug
        // information about the origin of the event.
        if (item.key() == "_SOURCE")
        {
            for (const auto& source_item : item.value().items())
            {
                if (source_item.key() == "PID")
                {
                    result.emplace("_PID", source_item.value().dump());
                    continue;
                }
                if (source_item.key() == "FILE")
                {
                    result.emplace("_CODE_FILE", source_item.value());
                    continue;
                }
                if (source_item.key() == "FUNCTION")
                {
                    result.emplace("_CODE_FUNC", source_item.value());
                    continue;
                }
                if (source_item.key() == "LINE")
                {
                    result.emplace("_CODE_LINE", source_item.value().dump());
                    continue;
                }
            }
            continue;
        }

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

auto extractEvent(sdbusplus::exception::generated_event_base&& t)
    -> std::tuple<std::string, Entry::Level, std::vector<std::string>>
{
    auto data = data_from_json(t);
    std::vector<std::string> additional_data = {};

    for (auto& [key, data] : data)
    {
        additional_data.emplace_back(key + "=" + data);
    }

    return {t.name(), severity_from_syslog(t.severity()),
            std::move(additional_data)};
}

} // namespace lg2::details
