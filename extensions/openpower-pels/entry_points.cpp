#include "elog_entry.hpp"
#include "extensions.hpp"
#include "manager.hpp"
#include "sdbusplus/bus.hpp"

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;

DISABLE_LOG_ENTRY_CAPS();

void pelStartup(sdbusplus::bus::bus& bus)
{
    Manager::createManager(bus);
}

REGISTER_EXTENSION_FUNCTION(pelStartup);

DeleteList pelCreate(const Entry& entry, const std::string& json)
{
    return Manager::get()->create(entry, json);
}

REGISTER_EXTENSION_FUNCTION(pelCreate);

void pelDelete(std::uint32_t id)
{
    Manager::get()->erase(id);
}

REGISTER_EXTENSION_FUNCTION(pelDelete);

} // namespace pels
} // namespace openpower
