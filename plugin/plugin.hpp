#pragma once

#include <sdbusplus/bus.hpp>

#include <memory>
#include <string>
#include <string_view>

namespace phosphor::logging
{

/**
 * @brief Context provided during plugin construction.
 *
 * Provides runtime resources required during plugin creation
 * and association with a log entry.
 */
struct PluginContext
{
    /** D-Bus connection. */
    sdbusplus::bus_t& bus;

    /** Object path of the associated log entry. */
    const std::string& objectPath;
};

/**
 * @brief Base class for log entry plugins.
 *
 * Plugins extend log entries with plugin-specific metadata,
 * behavior, and D-Bus interfaces.
 *
 * Concrete plugin implementations derive from this interface and
 * are created from plugin descriptors during log processing.
 */
class Plugin
{
  public:
    Plugin() = default;
    virtual ~Plugin() = default;

    Plugin(const Plugin&) = delete;
    Plugin& operator=(const Plugin&) = delete;
    Plugin(Plugin&&) = default;
    Plugin& operator=(Plugin&&) = delete;

    /**
     * @brief Get the plugin interface identifier.
     *
     * This is the canonical plugin identity used by the
     * framework.
     *
     * @return Plugin interface name.
     */
    virtual std::string_view interface() const = 0;
};

/** Unique ownership of a runtime plugin instance. */
using PluginPtr = std::unique_ptr<Plugin>;

} // namespace phosphor::logging
