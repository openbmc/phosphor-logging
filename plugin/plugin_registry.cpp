#include "plugin/plugin_registry.hpp"

#include <utility>

namespace phosphor::logging
{

void PluginRegistry::registerPlugin(plugin::Type type, PluginFactoryPtr factory)
{
    registry.emplace(type, std::move(factory));
}

const PluginFactory* PluginRegistry::lookup(plugin::Type type) const
{
    auto it = registry.find(type);

    if (it == registry.end())
    {
        return nullptr;
    }

    return it->second.get();
}

plugin::DescriptorPtr PluginRegistry::createDescriptor(
    const plugin::Info& info) const
{
    return createDescriptor(plugin::Type::cper, info);
}

plugin::DescriptorPtr PluginRegistry::createDescriptor(
    plugin::Type type, const plugin::Info& info) const
{
    auto factory = lookup(type);

    if (factory == nullptr)
    {
        return nullptr;
    }

    return factory->createDescriptor(info);
}

} // namespace phosphor::logging
