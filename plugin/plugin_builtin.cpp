#include "plugin/plugin_builtin.hpp"

#include "plugin/cper_processed_plugin.hpp"
#include "plugin/cper_raw_plugin.hpp"

namespace phosphor::logging
{

void registerBuiltins(PluginRegistry& registry)
{
    plugin::cperprocessed::registerPlugin(registry);
    plugin::cperraw::registerPlugin(registry);
}

} // namespace phosphor::logging
