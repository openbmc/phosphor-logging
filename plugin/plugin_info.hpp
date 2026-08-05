#pragma once

#include "plugin/plugin_type.hpp"

#include <nlohmann/json.hpp>

#include <string>

namespace phosphor::logging::plugin
{

/**
 * @brief Plugin request information.
 *
 * Describes a plugin requested by an event along with
 * plugin-specific metadata derived from event extensions.
 */
struct Info
{
    /** Registered plugin type identifier. */
    plugin::Type type;

    /**
     * Plugin-specific metadata.
     *
     * The content is plugin-defined and may contain nested
     * JSON objects.
     */
    nlohmann::json data;
};

} // namespace phosphor::logging::plugin
