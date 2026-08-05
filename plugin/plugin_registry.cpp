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

} // namespace phosphor::logging
