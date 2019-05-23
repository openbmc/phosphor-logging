#include "elog_entry.hpp"
#include "extensions.hpp"
#include "sdbusplus/bus.hpp"

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;

DISABLE_LOG_ENTRY_CAPS();

void pelStartup(internal::Manager& logManager)
{
}

REGISTER_EXTENSION_FUNCTION(pelStartup);

void pelCreate(const Entry& entry)
{
}

REGISTER_EXTENSION_FUNCTION(pelCreate);

void pelDelete(std::uint32_t id)
{
}

REGISTER_EXTENSION_FUNCTION(pelDelete);

bool pelDeleteProhibited(std::uint32_t id)
{
    return false;
}

REGISTER_EXTENSION_FUNCTION(pelDeleteProhibited);

} // namespace pels
} // namespace openpower
