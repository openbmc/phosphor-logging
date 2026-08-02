#pragma once

#include <nlohmann/json.hpp>

#include <string>

namespace phosphor::logging::plugin
{

/**
 * @brief Plugin request information.
 *
 * Describes a plugin requested by an event along with
 * plugin-specific metadata derived from event extensions
 * or other framework plugin request providers.
 */
struct Request
{
    /**
     * Plugin interface identifier.
     * Example: xyz.openbmc_project.Logging.Extension.CPER.Processed
     */
    std::string interface;

    /**
     * Plugin-specific metadata.
     * The content is plugin-defined and may contain neste JSON objects.
     */
    nlohmann::json data;
};

using RequestList = std::vector<Request>;

} // namespace phosphor::logging::plugin
