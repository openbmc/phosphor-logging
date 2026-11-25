#include "lg2_logger.hpp"

#include <errno.h>
#include <systemd/sd-bus.h>

#include <phosphor-logging/lg2/log_control.hpp>
#include <sdbusplus/vtable.hpp>

#include <algorithm>
#include <array>
#include <iterator>
#include <string_view>
#include <utility>

namespace lg2
{
namespace
{

constexpr auto objectPath = "/org/freedesktop/LogControl1";
constexpr auto interfaceName = "org.freedesktop.LogControl1";

// Indexed by syslog priority, which is the value of lg2::level.
constexpr std::array<std::string_view, 8> levelNames = {
    "emerg", "alert", "crit", "err", "warning", "notice", "info", "debug"};
static_assert(levelNames.size() == std::to_underlying(level::debug) + 1);

int property_get(sd_bus*, const char*, const char*, const char* property,
                 sd_bus_message* reply, void*, sd_bus_error* error)
{
    std::string_view name{property};

    if (name == "LogLevel")
    {
        auto l = static_cast<size_t>(details::log_threshold().load());
        return sd_bus_message_append(reply, "s", levelNames[l].data());
    }

    if (name == "LogTarget")
    {
        return sd_bus_message_append(reply, "s", "journal");
    }

    if (name == "SyslogIdentifier")
    {
        return sd_bus_message_append(reply, "s", program_invocation_short_name);
    }

    return sd_bus_error_setf(error, SD_BUS_ERROR_UNKNOWN_PROPERTY,
                             "Unknown property '%s'", property);
}

int property_set(sd_bus*, const char*, const char*, const char* property,
                 sd_bus_message* value, void*, sd_bus_error* error)
{
    const char* raw = nullptr;
    if (int r = sd_bus_message_read(value, "s", &raw); r < 0)
    {
        return r;
    }
    std::string_view requested{raw};
    std::string_view name{property};

    if (name == "LogLevel")
    {
        auto it = std::ranges::find(levelNames, requested);
        if (it == levelNames.end())
        {
            return sd_bus_error_setf(error, SD_BUS_ERROR_INVALID_ARGS,
                                     "Invalid LogLevel '%s'", raw);
        }

        details::log_threshold().store(
            static_cast<level>(std::distance(levelNames.begin(), it)));
        return 0;
    }

    if (name == "LogTarget")
    {
        // lg2 always logs to the journal; other targets are not supported.
        if (requested == "journal")
        {
            return 0;
        }

        return sd_bus_error_setf(error, SD_BUS_ERROR_NOT_SUPPORTED,
                                 "Unsupported LogTarget '%s'", raw);
    }

    return sd_bus_error_setf(error, SD_BUS_ERROR_UNKNOWN_PROPERTY,
                             "Unknown property '%s'", property);
}

// Properties are privileged by default, so only root may write them.
constexpr sdbusplus::vtable_t vtable[] = {
    sdbusplus::vtable::start(),
    sdbusplus::vtable::property("LogLevel", "s", property_get, property_set),
    sdbusplus::vtable::property("LogTarget", "s", property_get, property_set),
    sdbusplus::vtable::property("SyslogIdentifier", "s", property_get,
                                sdbusplus::vtable::property_::const_),
    sdbusplus::vtable::end(),
};

} // namespace

log_control::log_control(sdbusplus::bus_t& bus) :
    iface(bus, objectPath, interfaceName, vtable, this)
{}

} // namespace lg2
