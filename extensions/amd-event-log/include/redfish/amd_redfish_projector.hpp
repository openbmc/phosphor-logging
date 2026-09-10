#pragma once

#include <nlohmann/json.hpp>

namespace phosphor::logging::extensions::ael::redfish
{

/**
 * @brief Project AMD metadata into a Redfish payload.
 *
 * Converts AMD runtime metadata produced by the AEL
 * infrastructure into the final Redfish-compatible AMD
 * payload representation.
 *
 * If a producer supplied Redfish payload already exists,
 * the original payload is returned unchanged.
 *
 * @param[in] amdMetadata Internal AMD metadata.
 *
 * @return Redfish-formatted AMD payload.
 */
nlohmann::json project(const nlohmann::json& amdMetadata);

} // namespace phosphor::logging::extensions::ael::redfish
