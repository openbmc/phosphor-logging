#pragma once

#include "extension.hpp"
#include "request.hpp"

#include <nlohmann/json.hpp>

#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace phosphor::logging::event_extensions
{

/**
 * @brief Callback used to create a runtime extension.
 */
using CreateCallback =
    std::function<ExtensionPtr(const Context&, const Request&)>;

/**
 * @brief Callback used to restore a runtime extension.
 */
using RestoreCallback =
    std::function<ExtensionPtr(const Context&, const nlohmann::json&)>;

/**
 * @brief Extension registration callbacks.
 */
struct Registration
{
    CreateCallback createCallback;
    RestoreCallback restoreCallback;
};

/**
 * @brief Registry of event extensions.
 *
 * Maintains the mapping between extension interface names
 * and extension registration callbacks.
 */
class Registry
{
  public:
    Registry() = default;
    Registry(const Registry&) = delete;
    Registry& operator=(const Registry&) = delete;
    Registry(Registry&&) = delete;
    Registry& operator=(Registry&&) = delete;
    ~Registry() = default;

    /**
     * @brief Register an extension.
     *
     * @param[in] interface Extension interface name.
     * @param[in] registration Extension registration callbacks.
     */
    void registerExtension(std::string_view interface,
                           Registration registration);

    /**
     * @brief Lookup an extension registration.
     *
     * @param[in] interface Extension interface name.
     *
     * @return Registered callbacks or nullptr if no
     *         extension is registered.
     */
    const Registration* find(std::string_view interface) const;

  private:
    std::unordered_map<std::string, Registration> registrations;
};

} // namespace phosphor::logging::event_extensions
