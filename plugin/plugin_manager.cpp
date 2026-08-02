#include "plugin/plugin_manager.hpp"

namespace phosphor::logging
{

PluginManager::PluginManager(PluginRegistry& registry) : registry(registry) {}

std::unique_ptr<Plugin> PluginManager::create(
    const PluginContext& context, const plugin::Descriptor& descriptor) const
{
    auto factory = registry.lookup(descriptor.interface());

    if (factory == nullptr)
    {
        return nullptr;
    }

    return factory->create(context, descriptor);
}

} // namespace phosphor::logging
