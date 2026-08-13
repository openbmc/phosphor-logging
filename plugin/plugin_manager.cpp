#include "plugin/plugin_manager.hpp"

#include "plugin/plugin_builtin.hpp"

#include <phosphor-logging/lg2.hpp>

namespace phosphor::logging
{

PluginManager::PluginManager()
{
    registerBuiltins();
}

PluginManager::PluginManager(PluginRegistry&& registry) :
    registry(std::move(registry))
{
    registerBuiltins();
}

void PluginManager::registerBuiltins()
{
    ::phosphor::logging::registerBuiltins(registry);
}

PluginList PluginManager::create(const PluginContext& context,
                                 const plugin::RequestList& requests) const
{
    PluginList plugins;

    for (const auto& request : requests)
    {
        auto descriptor = registry.createDescriptor(request);

        if (descriptor == nullptr)
        {
            lg2::warning("Unknown plugin interface '{INTERFACE}'", "INTERFACE",
                         request.interface);
            continue;
        }

        auto plugin = create(context, *descriptor);

        if (plugin != nullptr)
        {
            plugins.emplace_back(std::move(plugin));
        }
    }

    return plugins;
}

PluginPtr PluginManager::create(const PluginContext& context,
                                const plugin::Descriptor& descriptor) const
{
    auto factory = registry.lookup(descriptor.interface());

    if (factory == nullptr)
    {
        return nullptr;
    }

    return factory->create(context, descriptor);
}

nlohmann::json PluginManager::buildExtensionPayload(
    std::string_view interface, const nlohmann::json& metadata) const
{
    return registry.buildExtensionPayload(interface, metadata);
}

} // namespace phosphor::logging
