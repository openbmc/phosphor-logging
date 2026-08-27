#pragma once

#include <nlohmann/json.hpp>

namespace phosphor::logging::extensions::ael::redfish
{

/*
 * TODO:
 * Extend projection support as additional AMD metadata
 * domains become available. Initial implementation focuses
 * on AEL metadata and establishes the foundation for CPER
 * and future platform-specific extensions.
 */

/**
 * @brief Generate Redfish JSON from AMD runtime metadata.
 *
 * @param[in] amdMetadata AMD runtime metadata namespace.
 *
 * @return Redfish OEM JSON.
 */
nlohmann::json project(const nlohmann::json& amdMetadata);

} // namespace phosphor::logging::extensions::ael::redfish
