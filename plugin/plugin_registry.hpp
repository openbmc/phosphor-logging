#pragma once

#include "plugin/plugin_factory.hpp"
#include "plugin/plugin_type.hpp"

#include <unordered_map>

namespace phosphor::logging
{

/**
 * @brief Registry of supported plugin types.
 *
 * Maintains the mapping between plugin types and their
 * associated plugin factories.
 */
class PluginRegistry
{
  public:
    PluginRegistry() = default;
    PluginRegistry(const PluginRegistry&) = delete;
    PluginRegistry(PluginRegistry&&) = delete;
    PluginRegistry& operator=(const PluginRegistry&) = delete;
    PluginRegistry& operator=(PluginRegistry&&) = delete;
    ~PluginRegistry() = default;

    /**
     * @brief Register a plugin factory.
     *
     * @param[in] type Plugin type.
     * @param[in] factory Plugin factory.
     */
    void registerPlugin(plugin::Type type, PluginFactoryPtr factory);

    /**
     * @brief Lookup a plugin factory.
     *
     * @param[in] type Plugin type.
     *
     * @return Registered plugin factory or nullptr if the
     *         plugin type is not registered.
     */
    const PluginFactory* lookup(plugin::Type type) const;

    /**
     * @brief Create a plugin descriptor.
     *
     * Resolves the plugin type specified by the plugin
     * request and invokes the corresponding registered
     * plugin factory to generate a runtime descriptor.
     *
     * @param[in] info Plugin request information.
     *
     * @return Generated plugin descriptor or nullptr
     *         if the plugin type is not supported.
     */
    plugin::DescriptorPtr createDescriptor(const plugin::Info& info) const;

    /**
     * @brief Create a plugin descriptor.
     *
     * @param[in] type Plugin type.
     * @param[in] info Plugin request information.
     *
     * @return Plugin descriptor.
     */
    plugin::DescriptorPtr createDescriptor(plugin::Type type,
                                           const plugin::Info& info) const;

  private:
    /** Registered plugin factories. */
    std::unordered_map<plugin::Type, PluginFactoryPtr> registry;
};

} // namespace phosphor::logging
