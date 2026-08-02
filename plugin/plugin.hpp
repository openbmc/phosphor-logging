#pragma once

#include <sdbusplus/bus.hpp>

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace phosphor::logging
{

/**
 * @brief Context provided during plugin construction.
 *
 * Provides runtime resources required by plugin implementations
 * when they are instantiated for a log entry.
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
     * Example:
     *
     * xyz.openbmc_project.Logging.Diagnostic.CPER
     *
     * @return Plugin interface name.
     */
    virtual std::string_view interface() const = 0;
};

/** Unique ownership of a runtime plugin instance. */
using PluginPtr = std::unique_ptr<Plugin>;

/**
 * @brief Collection of runtime plugins associated with a log entry.
 */
using PluginList = std::vector<PluginPtr>;

} // namespace phosphor::logging
