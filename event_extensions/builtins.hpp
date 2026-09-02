#pragma once

namespace phosphor::logging::event_extensions
{

class Registry;

/**
 * @brief Register builtin event extension providers.
 *
 * @param[in,out] registry Event extension registry.
 */
void registerBuiltins(Registry& registry);

} // namespace phosphor::logging::event_extensions
