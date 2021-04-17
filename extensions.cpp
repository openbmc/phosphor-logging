#include "extensions.hpp"

// The 'extensions_test' testcase needs to define these itself.
// Skip over the definition to avoid duplicate symbol definitions.
#ifndef TESTCASE_extensions_test

namespace phosphor
{
namespace logging
{

StartupFunctions Extensions::startupFunctions{};
CreateFunctions Extensions::createFunctions{};
DeleteFunctions Extensions::deleteFunctions{};
DeleteProhibitedFunctions Extensions::deleteProhibitedFunctions{};
Extensions::DefaultErrorCaps Extensions::defaultErrorCaps =
    Extensions::DefaultErrorCaps::enable;

} // namespace logging
} // namespace phosphor

#endif
