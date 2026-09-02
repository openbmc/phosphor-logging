#pragma once

#include "extension.hpp"
#include "request.hpp"

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
 * @brief Registry of event extensions.
 *
 * Maintains the mapping between extension interface names
 * and extension creation callbacks.
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
     * @param[in] createCallback Extension creation callback.
     */
    void registerExtension(std::string_view interface,
                           CreateCallback createCallback);

    /**
     * @brief Lookup an extension creation callback.
     *
     * @param[in] interface Extension interface name.
     *
     * @return Registered callback or nullptr if no
     *         extension is registered.
     */
    const CreateCallback* find(std::string_view interface) const;

  private:
    std::unordered_map<std::string, CreateCallback> registrations;
};

} // namespace phosphor::logging::event_extensions
