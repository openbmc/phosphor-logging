#include "data_interface.hpp"
#include "elog_entry.hpp"
#include "extensions.hpp"
#include "manager.hpp"

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;

std::unique_ptr<Manager> manager;

DISABLE_LOG_ENTRY_CAPS();

void pelStartup(internal::Manager& logManager)
{
    std::unique_ptr<DataInterfaceBase> dataIface =
        std::make_unique<DataInterface>(logManager.getBus());

    manager = std::make_unique<Manager>(logManager, std::move(dataIface));
}

REGISTER_EXTENSION_FUNCTION(pelStartup);

void pelCreate(const std::string& message, uint32_t id, uint64_t timestamp,
               Entry::Level severity, const AdditionalDataArg& additionalData,
               const AssociationEndpointsArg& assocs)
{
    manager->create(message, id, timestamp, severity, additionalData, assocs);
}

REGISTER_EXTENSION_FUNCTION(pelCreate);

void pelDelete(uint32_t id)
{
    return manager->erase(id);
}

REGISTER_EXTENSION_FUNCTION(pelDelete);

void pelDeleteProhibited(uint32_t id, bool& prohibited)
{
    prohibited = manager->isDeleteProhibited(id);
}

REGISTER_EXTENSION_FUNCTION(pelDeleteProhibited);

} // namespace pels
} // namespace openpower
