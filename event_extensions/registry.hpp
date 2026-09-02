#pragma once

#include "provider.hpp"

#include <string>
#include <string_view>
#include <unordered_map>

namespace phosphor::logging::event_extensions
{

/**
 * @brief Registry of event extension providers.
 *
 * Maintains the mapping between extension interface names
 * and the providers responsible for creating them.
 */
class Registry
{
  public:
    /**
     * @brief Register a provider.
     *
     * @param[in] interface Extension interface name.
     * @param[in] provider Provider implementation.
     */
    void registerProvider(std::string_view interface, ProviderPtr provider);

    /**
     * @brief Lookup a provider.
     *
     * @param[in] interface Extension interface name.
     *
     * @return Registered provider or nullptr if no provider
     *         is registered.
     */
    const Provider* lookup(std::string_view interface) const;

  private:
    std::unordered_map<std::string, ProviderPtr> providers;
};

} // namespace phosphor::logging::event_extensions
