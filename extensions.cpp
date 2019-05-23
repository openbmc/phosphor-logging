#include "extensions.hpp"

namespace phosphor
{
namespace logging
{

StartupFunctions Extensions::startupFunctions{};
CreateFunctions Extensions::createFunctions{};
DeleteFunctions Extensions::deleteFunctions{};
Extensions::DefaultErrorCaps Extensions::defaultErrorCaps =
    Extensions::DefaultErrorCaps::enable;

} // namespace logging
} // namespace phosphor
