#include "event_extensions/builtins.hpp"

#include "event_extensions/cper/processed.hpp"

namespace phosphor::logging::event_extensions
{

void registerBuiltins(Registry& registry)
{
    cper::processed::registerExtension(registry);
}

} // namespace phosphor::logging::event_extensions
