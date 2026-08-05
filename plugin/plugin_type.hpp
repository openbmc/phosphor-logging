#pragma once

#include <string>

namespace phosphor::logging::plugin
{

/**
 * @brief Plugin extension interface name.
 *
 * The extension interface name is the canonical identifier
 * for a plugin within the framework.
 *
 * Examples:
 *
 * - xyz.openbmc_project.Logging.Diagnostic.CPER
 */
using Interface = std::string;

} // namespace phosphor::logging::plugin
