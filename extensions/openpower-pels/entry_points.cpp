#include "elog_entry.hpp"
#include "extensions.hpp"
#include "sdbusplus/bus.hpp"

#include <sdeventplus/event.hpp>

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;

DISABLE_LOG_ENTRY_CAPS();

void pelStartup(sdbusplus::bus::bus& bus, sdeventplus::Event& event)
{
}

REGISTER_EXTENSION_FUNCTION(pelStartup);

DeleteList pelCreate(const Entry& entry)
{
    return {};
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
