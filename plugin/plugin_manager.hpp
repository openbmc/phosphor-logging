#pragma once

#include "plugin/plugin.hpp"
#include "plugin/plugin_descriptor.hpp"
#include "plugin/plugin_registry.hpp"

#include <memory>

namespace phosphor::logging
{
/**
 * @brief Manager for runtime plugin instances.
 *
 * PluginManager serves as the central coordination point
 * for the logging plugin framework.
 *
 * The manager is responsible for operations involving
 * runtime plugin instances. It uses PluginRegistry to
 * locate the factory associated with a descriptor type and
 * delegates plugin construction to the corresponding
 * PluginFactory.
 *
 * Centralizing plugin-related operations behind a single
 * interface isolates callers from registry and factory
 * implementation details.
 */
class PluginManager
{
  public:
    explicit PluginManager(PluginRegistry& registry);

    PluginManager(const PluginManager&) = delete;
    PluginManager(PluginManager&&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    PluginManager& operator=(PluginManager&&) = delete;
    ~PluginManager() = default;

    /**
     * Create a plugin instance.
     *
     * @param[in] context Plugin creation context.
     * @param[in] descriptor Plugin creation descriptor.
     *
     * @return Plugin instance or nullptr if no plugin is
     *         registered for the descriptor type.
     */
    std::unique_ptr<Plugin> create(const PluginContext& context,
                                   const plugin::Descriptor& descriptor) const;

  private:
    /** Plugin registry. */
    PluginRegistry& registry;
};

} // namespace phosphor::logging
