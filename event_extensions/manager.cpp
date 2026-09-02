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
        const auto* provider = registry.lookup(request.interface);
        if (provider == nullptr)
        {
            lg2::warning("No event extension provider registered for "
                         "{INTERFACE}",
                         "INTERFACE", request.interface);
            continue;
        }

        auto extension = provider->create(context, request);
        if (extension != nullptr)
        {
            extensions.emplace_back(std::move(extension));
        }
    }

    return extensions;
}

} // namespace phosphor::logging::event_extensions
