#include "plugin/plugin_registry.hpp"

#include <stdexcept>
#include <utility>

namespace phosphor::logging
{

void PluginRegistry::registerPlugin(std::string_view interface,
                                    PluginFactoryPtr factory)
{
    auto [it, inserted] = registry.emplace(interface, std::move(factory));

    if (!inserted)
    {
        throw std::invalid_argument("Plugin interface already registered");
    }
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
    const plugin::Request& request) const
{
    auto factory = lookup(request.interface);

    if (factory == nullptr)
    {
        return nullptr;
    }

    return factory->createDescriptor(request);
}

nlohmann::json PluginRegistry::buildExtensionPayload(
    std::string_view interface, const nlohmann::json& metadata) const
{
    const auto* factory = lookup(interface);

    if (factory == nullptr)
    {
        return metadata;
    }

    return factory->buildExtensionPayload(metadata);
}

} // namespace phosphor::logging
