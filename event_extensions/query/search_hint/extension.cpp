#include "event_extensions/query/search_hint/extension.hpp"

#include <phosphor-logging/lg2.hpp>

namespace phosphor::logging::event_extensions::query::search_hint
{

Extension::Extension(sdbusplus::bus_t& bus,
                     const sdbusplus::object_path& objectPath,
                     Properties properties) : Interface(bus, objectPath.str)
{
    service(std::move(properties.service), true);
    hint(std::move(properties.hint), true);
}

nlohmann::json Extension::serialize() const
{
    return {
        {serviceKey, service()},
        {hintKey, hint()},
    };
}

ExtensionPtr create(const Context& context, const Request& request)
{
    Properties properties{};
    try
    {
        properties.service = request.data.value(serviceKey, std::string{});
        properties.hint = request.data.value(hintKey, std::string{});
    }
    catch (const std::exception& e)
    {
        lg2::warning("Invalid SearchHint extension data: {ERROR}", "ERROR", e);
        return nullptr;
    }

    if (properties.service.empty() || properties.hint.empty())
    {
        lg2::warning("SearchHint extension requires Service and Hint");
        return nullptr;
    }

    return std::make_unique<Extension>(context.bus, context.objectPath,
                                       std::move(properties));
}

ExtensionPtr restore(const Context& context, const nlohmann::json& data)
{
    try
    {
        Properties properties{};
        properties.service = data.at(serviceKey).get<std::string>();
        properties.hint = data.at(hintKey).get<std::string>();

        return std::make_unique<Extension>(context.bus, context.objectPath,
                                           std::move(properties));
    }
    catch (const std::exception& e)
    {
        lg2::error("Failed to restore SearchHint extension: {ERROR}", "ERROR",
                   e);

        return nullptr;
    }
}

void registerExtension(Registry& registry)
{
    registry.registerExtension(interface, {
                                              .createCallback = create,
                                              .restoreCallback = restore,
                                          });
}

} // namespace phosphor::logging::event_extensions::query::search_hint
