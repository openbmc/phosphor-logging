// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "data_interface.hpp"
#include "elog_entry.hpp"
#include "event_logger.hpp"
#include "extensions.hpp"
#include "journal.hpp"
#include "manager.hpp"
#include "pldm_interface.hpp"

#include <phosphor-logging/lg2.hpp>

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;

std::unique_ptr<Manager> manager;

DISABLE_LOG_ENTRY_CAPS()

void pelStartup(internal::Manager& logManager)
{
    EventLogger::LogFunction logger =
        std::bind(std::mem_fn(&internal::Manager::create), &logManager,
                  std::placeholders::_1, std::placeholders::_2,
                  std::placeholders::_3, phosphor::logging::FFDCEntries{});

    std::unique_ptr<DataInterfaceBase> dataIface =
        std::make_unique<DataInterface>(logManager.getBus());

    std::unique_ptr<JournalBase> journal = std::make_unique<Journal>();

#ifndef DONT_SEND_PELS_TO_HOST
    std::unique_ptr<HostInterface> hostIface = std::make_unique<PLDMInterface>(
        logManager.getBus().get_event(), *(dataIface.get()));

    manager = std::make_unique<Manager>(logManager, std::move(dataIface),
                                        std::move(logger), std::move(journal),
                                        std::move(hostIface));
#else
    manager = std::make_unique<Manager>(logManager, std::move(dataIface),
                                        std::move(logger), std::move(journal));
#endif
}

REGISTER_EXTENSION_FUNCTION(pelStartup)

void pelCreate(const std::string& message, uint32_t id, uint64_t timestamp,
               Entry::Level severity, const AdditionalDataArg& additionalData,
               const AssociationEndpointsArg& assocs, const FFDCArg& ffdc)
{
    manager->create(message, id, timestamp, severity, additionalData, assocs,
                    ffdc);
}

REGISTER_EXTENSION_FUNCTION(pelCreate)

void pelDelete(uint32_t id)
{
    return manager->erase(id);
}

REGISTER_EXTENSION_FUNCTION(pelDelete)

void pelDeleteProhibited(uint32_t id, bool& prohibited)
{
    prohibited = manager->isDeleteProhibited(id);
}

REGISTER_EXTENSION_FUNCTION(pelDeleteProhibited)

void getLogIDWithHwIsolation(std::vector<uint32_t>& logIDs)
{
    manager->getLogIDWithHwIsolation(logIDs);
}

REGISTER_EXTENSION_FUNCTION(getLogIDWithHwIsolation)

} // namespace pels
} // namespace openpower
