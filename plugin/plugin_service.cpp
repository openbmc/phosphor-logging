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

} // namespace phosphor::logging
