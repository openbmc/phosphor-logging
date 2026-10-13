#include "event_extensions/builtins.hpp"

// Keep these checks in sync with cper_interfaces in meson.build.
#if __has_include(<xyz/openbmc_project/Logging/CPER/Raw/common.hpp>) &&        \
    __has_include(<xyz/openbmc_project/Logging/CPER/Types/common.hpp>) &&      \
    __has_include(                                                             \
        <xyz/openbmc_project/Logging/Extension/CPER/Processed/common.hpp>)
#define HAVE_CPER_EXTENSIONS 1
#include "event_extensions/cper/processed/extension.hpp"
#include "event_extensions/cper/raw/extension.hpp"
#else
#define HAVE_CPER_EXTENSIONS 0
#endif

namespace phosphor::logging::event_extensions
{

void registerBuiltins([[maybe_unused]] Registry& registry)
{
#if HAVE_CPER_EXTENSIONS
    cper::processed::registerExtension(registry);
    cper::raw::registerExtension(registry);
#endif
}

} // namespace phosphor::logging::event_extensions
