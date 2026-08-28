#pragma once

#include "plugin/plugin.hpp"
#include "plugin/plugin_manager.hpp"
#include "plugin/plugin_request.hpp"

#include <nlohmann/json.hpp>

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

    /**
     * @brief Build an extension payload.
     *
     * Resolves the specified plugin interface and invokes the
     * corresponding plugin implementation to construct an
     * extension payload suitable for storage within the
     * _EXTENSIONS transport object.
     *
     * This API allows runtime metadata providers to remain
     * independent of plugin-specific schemas while enabling
     * plugin implementations to define the payload format
     * associated with their extension interfaces.
     *
     * @param[in] interface Extension interface name.
     * @param[in] metadata Runtime metadata.
     *
     * @return Extension payload.
     */
    nlohmann::json buildExtensionPayload(std::string_view interface,
                                         const nlohmann::json& metadata);

    /**
     * @brief Restore runtime plugins.
     *
     * Delegates plugin restoration to PluginManager.
     *
     * @param[in] context Plugin runtime context.
     * @param[in] data Serialized plugin state.
     *
     * @return Restored plugin instances.
     */
    PluginList deserialize(const PluginContext& context,
                           const nlohmann::json& data) const;

  private:
    /** Plugin framework manager. */
    PluginManager manager;
};

} // namespace phosphor::logging
