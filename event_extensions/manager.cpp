#include "event_extensions/manager.hpp"

#include "event_extensions/builtins.hpp"

#include <phosphor-logging/lg2.hpp>

namespace phosphor::logging::event_extensions
{

Manager::Manager()
{
    registerBuiltins(registry);
}

ExtensionList Manager::create(const Context& context,
                              const RequestList& requests) const
{
    ExtensionList extensions;
    for (const auto& request : requests)
    {
        const auto* registration = registry.find(request.interface);
        if (registration == nullptr)
        {
            lg2::warning("No event extension registered for "
                         "{INTERFACE}",
                         "INTERFACE", request.interface);
            continue;
        }

        auto extension = registration->createCallback(context, request);
        if (extension != nullptr)
        {
            extensions.emplace_back(std::move(extension));
        }
    }

    return extensions;
}

ExtensionList Manager::restore(const Context& context,
                               const nlohmann::json& data) const
{
    ExtensionList extensions;
    for (const auto& [interface, payload] : data.items())
    {
        const auto* registration = registry.find(interface);
        if (registration == nullptr)
        {
            continue;
        }

        auto extension = registration->restoreCallback(context, payload);
        if (extension != nullptr)
        {
            extensions.emplace_back(std::move(extension));
        }
    }

    return extensions;
}

} // namespace phosphor::logging::event_extensions
