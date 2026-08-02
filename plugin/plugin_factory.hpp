#pragma once

#include "plugin/plugin.hpp"
#include "plugin/plugin_descriptor.hpp"

#include <memory>

namespace phosphor::logging
{

/**
 * @class PluginFactory
 *
 * @brief Base interface for runtime plugin factories.
 *
 * Plugin factories are responsible for constructing
 * runtime plugin instances from plugin descriptors.
 *
 * Individual plugin implementations may provide
 * plugin-specific factory implementations that derive
 * from this interface.
 *
 * Registration and integration with the plugin
 * management infrastructure will be introduced by
 * subsequent changes.
 */
class PluginFactory
{
  public:
    PluginFactory() = default;
    PluginFactory(const PluginFactory&) = delete;
    PluginFactory(PluginFactory&&) = delete;
    PluginFactory& operator=(const PluginFactory&) = delete;
    PluginFactory& operator=(PluginFactory&&) = delete;
    virtual ~PluginFactory() = default;

    /**
     * @brief Create a runtime plugin instance.
     *
     * Constructs a plugin implementation from the
     * supplied creation context and descriptor.
     *
     * @param[in] context Plugin creation context.
     * @param[in] descriptor Plugin creation descriptor.
     *
     * @return Runtime plugin instance.
     */
    virtual std::unique_ptr<Plugin> create(
        const PluginContext& context,
        const plugin::Descriptor& descriptor) const = 0;
};

} // namespace phosphor::logging
