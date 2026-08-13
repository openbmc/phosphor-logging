#pragma once

#include "plugin/plugin.hpp"
#include "plugin/plugin_descriptor.hpp"
#include "plugin/plugin_registry.hpp"
#include "plugin/plugin_request.hpp"

#include <nlohmann/json.hpp>

namespace phosphor::logging
{

/**
 * @brief Manager for runtime plugin instances.
 *
 * PluginManager acts as the coordination layer between
 * plugin requests, plugin descriptors, factories, and
 * runtime plugin instances.
 *
 * The manager is responsible for operations involving
 * runtime plugin instances. It uses PluginRegistry to
 * construct plugin descriptors, locate the factory
 * associated with a descriptor type, and delegate plugin
 * construction to the corresponding PluginFactory.
 *
 * Centralizing plugin-related operations behind a single
 * interface isolates callers from registry and factory
 * implementation details.
 */
class PluginManager
{
  public:
    /**
     * @brief Construct a plugin manager.
     */
    PluginManager();
    explicit PluginManager(PluginRegistry&& registry);

    PluginManager(const PluginManager&) = delete;
    PluginManager(PluginManager&&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    PluginManager& operator=(PluginManager&&) = delete;
    ~PluginManager() = default;

    /**
     * @brief Create runtime plugins.
     *
     * Creates runtime plugin instances from the supplied
     * plugin requests.
     *
     * @param[in] context Plugin creation context.
     * @param[in] requests Plugin requests.
     *
     * @return Runtime plugin instances.
     */
    PluginList create(const PluginContext& context,
                      const plugin::RequestList& requests) const;

    /**
     * @brief Build an extension payload.
     *
     * Resolves the specified plugin interface and delegates
     * payload construction to the corresponding registered
     * plugin implementation.
     *
     * @param[in] interface Extension interface name.
     * @param[in] metadata Runtime metadata.
     *
     * @return Extension payload.
     */
    nlohmann::json buildExtensionPayload(std::string_view interface,
                                         const nlohmann::json& metadata) const;

  private:
    /**
     * @brief Create a plugin instance.
     *
     * @param[in] context Plugin creation context.
     * @param[in] descriptor Plugin creation descriptor.
     *
     * @return Plugin instance or nullptr if no plugin is
     *         registered for the descriptor type.
     */
    PluginPtr create(const PluginContext& context,
                     const plugin::Descriptor& descriptor) const;

    /**
     * @brief Register builtin plugin implementations.
     */
    void registerBuiltins();

    /** Plugin registry. */
    PluginRegistry registry;
};

} // namespace phosphor::logging
