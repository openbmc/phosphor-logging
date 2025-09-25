#include "config.h"

#include "lg2_commit.hpp"

#include <sys/syslog.h>

#include <nlohmann/json.hpp>
#include <phosphor-logging/commit.hpp>
#include <phosphor-logging/lg2.hpp>
#include <sdbusplus/async.hpp>
#include <sdbusplus/exception.hpp>
#include <xyz/openbmc_project/Logging/Create/client.hpp>
#include <xyz/openbmc_project/Logging/Entry/client.hpp>

namespace lg2
{
namespace details
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

        if (item.value().type() == nlohmann::json::value_t::string)
        {
            result.emplace(item.key(), item.value());
        }
        else
        {
            result.emplace(item.key(), item.value().dump());
        }
    }

    return result;
}

auto extractEvent(sdbusplus::exception::generated_event_base&& t)
    -> std::tuple<std::string, Entry::Level, std::map<std::string, std::string>>
{
    return {t.name(), severity_from_syslog(t.severity()), data_from_json(t)};
}

} // namespace details

auto convertRedfishArgsToString(sdbusplus::exception::generated_event_base& t)
    -> std::string
{
    auto j = t.getRedfishArgs()[t.name()];

    std::string arguments = "";

    for (const auto& item : j.items())
    {
        if (item.value().type() == nlohmann::json::value_t::string)
        {
            arguments += item.value();
            arguments += ",";
        }
        else
        {
            arguments += item.value().dump();
            arguments += ",";
        }
    }

    if (arguments.ends_with(","))
    {
        arguments.pop_back();
    }

    return arguments;
}

auto commit(sdbusplus::exception::generated_event_base&& t)
    -> sdbusplus::message::object_path
{
    // Check event filters first.
    if ((t.severity() == LOG_INFO) && details::filterEvent(t.name()))
    {
        return {};
    }
    else if (details::filterError(t.name()))
    {
        return {};
    }

    if constexpr (LG2_COMMIT_DEPRECATED_RF_FORMAT)
    {
        lg2::error(t.description(), "REDFISH_MESSAGE_ID", t.redfishMessageId(),
                   "REDFISH_MESSAGE_ARGS", convertRedfishArgsToString(t));
    }

    if constexpr (LG2_COMMIT_JOURNAL)
    {
        lg2::error("OPENBMC_MESSAGE_ID={DATA}", "DATA", t.to_json().dump());
    }

    if constexpr (LG2_COMMIT_DBUS)
    {
        using details::Create;

        auto b = sdbusplus::bus::new_default();
        auto m =
            b.new_method_call(Create::default_service, Create::instance_path,
                              Create::interface, "Create");

        m.append(t.name(), details::severity_from_syslog(t.severity()),
                 details::data_from_json(t));

        auto reply = b.call(m);

        return reply.unpack<sdbusplus::message::object_path>();
    }

    return {};
}

void resolve(const sdbusplus::message::object_path& logPath)
{
    if constexpr (LG2_COMMIT_DBUS)
    {
        using details::Entry;

        auto b = sdbusplus::bus::new_default();
        auto m = b.new_method_call(Entry::default_service, logPath.str.c_str(),
                                   "org.freedesktop.DBus.Properties", "Set");
        m.append(Entry::interface, "Resolved", std::variant<bool>(true));
        auto reply = b.call(m);
    }
}

auto commit(sdbusplus::async::context& ctx,
            sdbusplus::exception::generated_event_base&& t)
    -> sdbusplus::async::task<sdbusplus::message::object_path>
{
    using details::Create;

    if constexpr (LG2_COMMIT_DEPRECATED_RF_FORMAT)
    {
        lg2::error(t.description(), "REDFISH_MESSAGE_ID", t.redfishMessageId(),
                   "REDFISH_MESSAGE_ARGS", convertRedfishArgsToString(t));
    }

    if constexpr (LG2_COMMIT_JOURNAL)
    {
        lg2::error("OPENBMC_MESSAGE_ID={DATA}", "DATA", t.to_json().dump());
    }

    if constexpr (LG2_COMMIT_DBUS)
    {
        co_return co_await Create(ctx)
            .service(Create::default_service)
            .path(Create::instance_path)
            .create(t.name(), details::severity_from_syslog(t.severity()),
                    details::data_from_json(t));
    }
    co_return {};
}

auto resolve(sdbusplus::async::context& ctx,
             const sdbusplus::message::object_path& logPath)
    -> sdbusplus::async::task<>
{
    using details::Entry;

    if constexpr (LG2_COMMIT_DBUS)
    {
        std::string path = logPath.str;
        co_await Entry(ctx)
            .service(Entry::default_service)
            .path(path)
            .resolved(true);
    }
    co_return;
}

} // namespace lg2
