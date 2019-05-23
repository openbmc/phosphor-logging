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

void pelCreate(const std::string& message, uint32_t id, uint64_t timestamp,
               Entry::Level severity, const AdditionalDataArg& additionalData,
               const AssociationEndpointsArg& assocs)
{
}

REGISTER_EXTENSION_FUNCTION(pelCreate);

void pelDelete(uint32_t id)
{
}

REGISTER_EXTENSION_FUNCTION(pelDelete);

void pelDeleteProhibited(uint32_t id, bool& prohibited)
{
    prohibited = false;
}

REGISTER_EXTENSION_FUNCTION(pelDeleteProhibited);

} // namespace pels
} // namespace openpower
