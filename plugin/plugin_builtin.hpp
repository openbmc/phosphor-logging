#pragma once

namespace phosphor::logging
{

class PluginRegistry;

/**
 * @brief Register built-in plugin implementations.
 *
 * Populates the supplied plugin registry with plugin
 * implementations compiled into phosphor-logging.
 *
 * This function serves as the framework composition point
 * for builtin plugins. New builtin plugin implementations
 * should be integrated through this interface.
 *
 * @param[in,out] registry Plugin registry.
 */
void registerBuiltins(PluginRegistry& registry);

} // namespace phosphor::logging
