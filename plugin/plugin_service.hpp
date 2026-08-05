#pragma once

#include "plugin/plugin.hpp"
#include "plugin/plugin_manager.hpp"
#include "plugin/plugin_request.hpp"

namespace phosphor::logging
{

/**
 * @brief Runtime plugin integration service.
 *
 * Provides the logging-specific integration layer
 * between log entry processing and the plugin
 * framework.
 *
 * Plugin requests are resolved into runtime plugin
 * instances through the underlying plugin manager.
 */
class PluginService
{
  public:
    PluginService() = default;

    PluginService(const PluginService&) = delete;
    PluginService(PluginService&&) = delete;
    PluginService& operator=(const PluginService&) = delete;
    PluginService& operator=(PluginService&&) = delete;
    ~PluginService() = default;

    /**
     * @brief Create runtime plugins.
     *
     * Creates runtime plugin instances from the
     * supplied plugin requests.
     *
     * @param[in] context Plugin creation context.
     * @param[in] requests Plugin requests.
     *
     * @return Runtime plugin instances.
     */
    PluginList create(const PluginContext& context,
                      const plugin::RequestList& requests);

  private:
    /** Plugin framework manager. */
    PluginManager manager;
};

} // namespace phosphor::logging
