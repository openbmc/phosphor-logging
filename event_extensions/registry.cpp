#include "event_extensions/registry.hpp"

#include <phosphor-logging/lg2.hpp>

namespace phosphor::logging::event_extensions
{

void Registry::registerProvider(std::string_view interface,
                                ProviderPtr provider)
{
    auto [it, inserted] = providers.emplace(interface, std::move(provider));
    if (!inserted)
    {
        lg2::warning("Duplicate event extension registration: {INTERFACE}",
                     "INTERFACE", interface);
    }
}

const Provider* Registry::lookup(std::string_view interface) const
{
    auto it = providers.find(std::string(interface));
    if (it == providers.end())
    {
        return nullptr;
    }

    return it->second.get();
}

} // namespace phosphor::logging::event_extensions
