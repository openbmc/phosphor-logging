#pragma once

#include "plugin/plugin_factory.hpp"

#include <string>
#include <string_view>
#include <unordered_map>

namespace phosphor::logging
{

/**
 * @brief Registry of supported plugins.
 *
 * Maintains the mapping between plugin interface names and
 * their associated plugin factories.
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
     * @param[in] interface Plugin interface name.
     * @param[in] factory Plugin factory.
     */
    void registerPlugin(std::string_view interface, PluginFactoryPtr factory);

    /**
     * @brief Lookup a plugin factory.
     *
     * @param[in] interface Plugin interface name.
     *
     * @return Registered plugin factory or nullptr if the
     *         plugin interface is not registered.
     */
    const PluginFactory* lookup(std::string_view interface) const;

    /**
     * @brief Create a plugin descriptor.
     *
     * Resolves the plugin interface specified by the plugin
     * request and invokes the corresponding registered
     * factory to generate a descriptor.
     *
     * @param[in] info Plugin request information.
     *
     * @return Generated plugin descriptor or nullptr if the
     *         plugin interface is not registered.
     */
    plugin::DescriptorPtr createDescriptor(const plugin::Info& info) const;

    /**
     * @brief Build extension payload for a plugin.
     *
     * Resolves the specified plugin interface and invokes the
     * registered plugin factory to construct the payload stored
     * in _EXTENSIONS.
     *
     * @param[in] interface Plugin interface name.
     * @param[in] metadata Plugin metadata.
     *
     * @return Plugin payload.
     */
    nlohmann::json buildPayload(std::string_view interface,
                                const nlohmann::json& metadata) const;

  private:
    /** Registered plugin factories. */
    std::unordered_map<std::string, PluginFactoryPtr> registry;
};

} // namespace phosphor::logging
