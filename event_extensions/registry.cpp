#include "event_extensions/registry.hpp"

#include <phosphor-logging/lg2.hpp>

namespace phosphor::logging::event_extensions
{

void Registry::registerExtension(std::string_view interface,
                                 Registration registration)
{
    auto [it,
          inserted] = registrations.emplace(interface, std::move(registration));

    if (!inserted)
    {
        lg2::warning("Duplicate event extension registration: "
                     "{INTERFACE}",
                     "INTERFACE", interface);
    }
}

const Registration* Registry::find(std::string_view interface) const
{
    auto it = registrations.find(std::string(interface));

    if (it == registrations.end())
    {
        return nullptr;
    }

    return &it->second;
}

} // namespace phosphor::logging::event_extensions
