#pragma once

#include "plugin/plugin.hpp"
#include "plugin/plugin_descriptor.hpp"
#include "plugin/plugin_request.hpp"

#include <nlohmann/json.hpp>

#include <memory>

namespace phosphor::logging
{

/**
 * @brief Base interface for plugin factories.
 *
 * Plugin factories create runtime plugin instances from
 * plugin descriptors.
 *
 * Concrete plugin implementations may provide factory
 * implementations that validate descriptor contents and
 * construct the corresponding runtime plugin object.
 */
class PluginFactory
{
  public:
    PluginFactory() = default;
    virtual ~PluginFactory() = default;

    PluginFactory(const PluginFactory&) = delete;
    PluginFactory(PluginFactory&&) = delete;
    PluginFactory& operator=(const PluginFactory&) = delete;
    PluginFactory& operator=(PluginFactory&&) = delete;

    /**
     * @brief Create a runtime plugin instance.
     *
     * Construct a plugin from the supplied context and
     * descriptor.
     *
     * @param[in] context Plugin creation context.
     * @param[in] descriptor Plugin descriptor.
     *
     * @return Newly created plugin instance.
     */
    virtual PluginPtr create(const PluginContext& context,
                             const plugin::Descriptor& descriptor) const = 0;

    /**
     * @brief Create a plugin descriptor.
     *
     * Converts plugin request information into a
     * runtime descriptor.
     *
     * @param[in] request Plugin request information.
     *
     * @return Plugin descriptor.
     */
    virtual plugin::DescriptorPtr createDescriptor(
        const plugin::Request& request) const = 0;

    /**
     * @brief Build plugin transport payload.
     *
     * Converts plugin-specific metadata into the payload stored
     * under the plugin interface within the _EXTENSIONS
     * additional-data field.
     *
     * The default implementation returns the input metadata
     * unchanged.
     *
     * @param[in] metadata Plugin metadata.
     *
     * @return Extension payload.
     */
    virtual nlohmann::json buildExtensionPayload(
        const nlohmann::json& metadata) const
    {
        return metadata;
    }

    /**
     * @brief Restore plugin from serialized state.
     *
     * Plugins supporting persistence may override
     * this method.
     *
     * @param[in] context Plugin runtime context.
     * @param[in] data Serialized plugin state.
     *
     * @return Restored plugin instance.
     */
    virtual PluginPtr deserialize(const PluginContext&,
                                  const nlohmann::json&) const
    {
        return nullptr;
    }
};

/** Unique ownership of a plugin factory. */
using PluginFactoryPtr = std::unique_ptr<PluginFactory>;

} // namespace phosphor::logging
