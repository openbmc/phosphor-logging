#include "plugin/plugin_builtin.hpp"

#include "plugin/cper_processed_plugin.hpp"
#include "plugin/cper_raw_plugin.hpp"

namespace phosphor::logging
{

void registerBuiltins(PluginRegistry& registry)
{
    plugin::cper::processed::registerPlugin(registry);
    plugin::cper::raw::registerPlugin(registry);
}

} // namespace phosphor::logging
