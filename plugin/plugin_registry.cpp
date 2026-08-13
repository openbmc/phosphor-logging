#include "plugin/plugin_registry.hpp"

#include <utility>

namespace phosphor::logging
{

void PluginRegistry::registerPlugin(std::string_view interface,
                                    PluginFactoryPtr factory)
{
    registry.emplace(interface, std::move(factory));
}

const PluginFactory* PluginRegistry::lookup(std::string_view interface) const
{
    auto it = registry.find(std::string(interface));

    if (it == registry.end())
    {
        return nullptr;
    }

    return it->second.get();
}

plugin::DescriptorPtr PluginRegistry::createDescriptor(
    const plugin::Info& info) const
{
    auto factory = lookup(info.interface);

    if (factory == nullptr)
    {
        return nullptr;
    }

    return factory->createDescriptor(info);
}

nlohmann::json PluginRegistry::buildPayload(
    std::string_view interface, const nlohmann::json& metadata) const
{
    const auto* factory = lookup(interface);

    if (factory == nullptr)
    {
        return metadata;
    }

    return factory->buildPayload(metadata);
}

} // namespace phosphor::logging
