#include "event_extensions/builtins.hpp"

#include "event_extensions/cper/processed/extension.hpp"
#include "event_extensions/cper/raw/extension.hpp"
#include "event_extensions/query/search_hint/extension.hpp"

namespace phosphor::logging::event_extensions
{

void registerBuiltins(Registry& registry)
{
    cper::processed::registerExtension(registry);
    cper::raw::registerExtension(registry);
    query::search_hint::registerExtension(registry);
}

} // namespace phosphor::logging::event_extensions
