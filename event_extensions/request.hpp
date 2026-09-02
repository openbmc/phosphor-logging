#pragma once

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace phosphor::logging::event_extensions
{

/**
 * @brief Event extension request.
 *
 * Represents an event-requested extension and any
 * extension-specific metadata associated with it.
 */
struct Request
{
    /**
     * @brief D-Bus interface implemented by the extension.
     *
     * Example:
     * xyz.openbmc_project.Logging.Extension.CPER.Raw
     */
    std::string interface;

    /**
     * @brief Extension-specific metadata.
     *
     * The schema and contents are defined by the extension
     * implementation.
     */
    nlohmann::json data;
};

/** Collection of event extension requests. */
using RequestList = std::vector<Request>;

} // namespace phosphor::logging::event_extensions
