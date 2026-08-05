#include "plugin_service.hpp"

namespace phosphor::logging
{

PluginList PluginService::create(const PluginContext& context,
                                 const plugin::RequestList& requests)
{
    return manager.create(context, requests);
}

} // namespace phosphor::logging
