#pragma once

#include "extension.hpp"
#include "registry.hpp"
#include "request.hpp"

#include <nlohmann/json.hpp>

namespace phosphor::logging::event_extensions
{

/**
 * @brief Event extension framework entry point.
 *
 * Manager owns the extension registry and builtin
 * extension registration.
 *
 * LogManager creates a single Manager instance and uses
 * it to create runtime extensions for log entries.
 */
class Manager
{
  public:
    Manager();
    Manager(const Manager&) = delete;
    Manager& operator=(const Manager&) = delete;
    Manager(Manager&&) = delete;
    Manager& operator=(Manager&&) = delete;
    ~Manager() = default;

    /**
     * @brief Create extensions from requests.
     *
     * @param[in] context Runtime creation context.
     * @param[in] requests Requested extensions.
     *
     * @return Created extensions.
     */
    ExtensionList create(const Context& context,
                         const RequestList& requests) const;

    /**
     * @brief Restore extensions from serialized data.
     *
     * @param[in] context Runtime creation context.
     * @param[in] data Serialized extension state.
     *
     * @return Restored extensions.
     */
    ExtensionList restore(const Context& context,
                          const nlohmann::json& data) const;

    /**
     * @brief Build an extension-specific payload from
     *        runtime metadata.
     *
     * @param[in] interface Extension interface name.
     * @param[in] metadata Aggregated runtime metadata.
     *
     * @return Extension payload.
     */
    nlohmann::json buildRuntimeMetadataPayload(
        std::string_view interface, const nlohmann::json& metadata) const;

  private:
    Registry registry;
};

} // namespace phosphor::logging::event_extensions
