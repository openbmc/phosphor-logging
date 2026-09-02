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
 * provider registration.
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

  private:
    Registry registry;
};

} // namespace phosphor::logging::event_extensions
