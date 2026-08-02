#include "plugin/plugin_builtin.hpp"

#include "plugin/cper_processed_plugin.hpp"

namespace phosphor::logging
{

void registerBuiltins(PluginRegistry& registry)
{
    plugin::cper::registerProcessedPlugin(registry);
}

} // namespace phosphor::logging
