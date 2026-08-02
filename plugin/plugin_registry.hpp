#pragma once

#include "plugin/plugin_factory.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace phosphor::logging
{

/**
 * @brief Registry of plugin factories.
 *
 * PluginRegistry maintains the association between a
 * plugin interface identifier and the corresponding
 * PluginFactory implementation.
 *
 * The registry is used by the plugin framework to locate
 * the factory responsible for creating runtime plugin
 * instances from plugin descriptors.
 */
class PluginRegistry
{
  public:
    PluginRegistry() = default;

    PluginRegistry(const PluginRegistry&) = delete;
    PluginRegistry(PluginRegistry&&) = default;
    PluginRegistry& operator=(const PluginRegistry&) = delete;
    PluginRegistry& operator=(PluginRegistry&&) = default;

    ~PluginRegistry() = default;

    /**
     * @brief Register a plugin factory.
     *
     * Registering the same plugin interface more than once
     * is an error.
     *
     * @param[in] interface Plugin interface identifier.
     * @param[in] factory Plugin factory.
     */
    void registerPlugin(std::string_view interface, PluginFactoryPtr factory);

    /**
     * @brief Lookup a plugin factory.
     *
     * @param[in] interface Plugin interface identifier.
     *
     * @return Registered plugin factory or nullptr if no
     *         factory is registered.
     */
    const PluginFactory* lookup(std::string_view interface) const;

    /**
     * @brief Create a plugin descriptor.
     *
     * Resolves the plugin interface specified by the plugin
     * request and invokes the corresponding registered
     * factory to generate a descriptor.
     *
     * @param[in] request Plugin request information.
     *
     * @return Generated plugin descriptor or nullptr if the
     *         plugin interface is not registered.
     */
    plugin::DescriptorPtr createDescriptor(
        const plugin::Request& request) const;

  private:
    /** Registered plugin factories. */
    std::unordered_map<std::string, PluginFactoryPtr> registry;
};

} // namespace phosphor::logging
