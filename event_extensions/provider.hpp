#pragma once

#include "extension.hpp"
#include "request.hpp"

#include <nlohmann/json.hpp>

#include <memory>

namespace phosphor::logging::event_extensions
{

/**
 * @brief Creates runtime event extensions.
 *
 * Providers interpret extension-specific request data
 * and construct the corresponding runtime extension.
 */
class Provider
{
  public:
    virtual ~Provider() = default;

    /**
     * @brief Create an extension instance.
     *
     * @param[in] context Runtime creation context.
     * @param[in] request Extension request.
     *
     * @return Created extension instance.
     */
    virtual ExtensionPtr create(const Context& context,
                                const Request& request) const = 0;

    /**
     * @brief Restore an extension from serialized data.
     *
     * @param[in] context Runtime creation context.
     * @param[in] data Serialized extension state.
     *
     * @return Restored extension instance.
     */
    virtual ExtensionPtr restore(const Context& context,
                                 const nlohmann::json& data) const = 0;
};

using ProviderPtr = std::unique_ptr<Provider>;

} // namespace phosphor::logging::event_extensions
