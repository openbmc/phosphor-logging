#include "event_extensions/builtins.hpp"

#if __has_include(<xyz/openbmc_project/Logging/CPER/Types/common.hpp>)
#include "event_extensions/cper/processed/extension.hpp"
#include "event_extensions/cper/raw/extension.hpp"
#endif

namespace phosphor::logging::event_extensions
{

void registerBuiltins([[maybe_unused]] Registry& registry)
{
#if __has_include(<xyz/openbmc_project/Logging/CPER/Types/common.hpp>)
    cper::processed::registerExtension(registry);
    cper::raw::registerExtension(registry);
#endif
}

} // namespace phosphor::logging::event_extensions
