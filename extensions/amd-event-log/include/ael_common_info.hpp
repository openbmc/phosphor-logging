#pragma once

#include <optional>
#include <string>

namespace phosphor::logging::extensions::ael
{

/**
 * @brief Common metadata shared across AEL extensions.
 *
 * Common metadata is derived by the AEL infrastructure
 * and may be consumed by multiple AEL extensions and
 * projection layers.
 */
struct CommonInfo
{
    /**
     * @brief Rack identifier associated with the event.
     */
    std::optional<std::string> rackId;
};

/**
 * @brief Generate common AEL metadata.
 *
 * The initial implementation provides the framework for
 * future common metadata enrichment.
 *
 * @return Common metadata.
 */
CommonInfo getCommonInfo();

} // namespace phosphor::logging::extensions::ael
