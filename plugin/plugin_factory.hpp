#pragma once

#include "plugin/plugin.hpp"
#include "plugin/plugin_descriptor.hpp"
#include "plugin/plugin_request.hpp"

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
};

/** Unique ownership of a plugin factory. */
using PluginFactoryPtr = std::unique_ptr<PluginFactory>;

} // namespace phosphor::logging
