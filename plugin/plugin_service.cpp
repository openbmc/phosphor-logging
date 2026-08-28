#include "plugin_service.hpp"

namespace phosphor::logging
{

PluginList PluginService::create(const PluginContext& context,
                                 const plugin::RequestList& requests)
{
    return manager.create(context, requests);
}

nlohmann::json PluginService::buildExtensionPayload(
    std::string_view interface, const nlohmann::json& metadata)
{
    return manager.buildExtensionPayload(interface, metadata);
}

PluginList PluginService::deserialize(const PluginContext& context,
                                      const nlohmann::json& data) const
{
    return manager.deserialize(context, data);
}

} // namespace phosphor::logging
