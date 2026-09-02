#pragma once

#include "extension.hpp"
#include "request.hpp"

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
};

using ProviderPtr = std::unique_ptr<Provider>;

} // namespace phosphor::logging::event_extensions
