#include "event_extensions/cper/processed.hpp"

namespace phosphor::logging::event_extensions::cper::processed
{

Extension::Extension(sdbusplus::bus_t& bus, const std::string& objectPath,
                     Properties properties) : Interface(bus, objectPath.c_str())
{
    diagnosticDataType(properties.diagnosticDataType, true);
    notificationType(std::move(properties.notificationType), true);
    sectionType(std::move(properties.sectionType), true);
    oem(std::move(properties.oem), true);
}

nlohmann::json Extension::serialize() const
{
    return {
        {diagnosticDataTypeKey, diagnosticDataType()},
        {notificationTypeKey, notificationType()},
        {sectionTypeKey, sectionType()},
        {oemKey, oem()},
    };
}

ExtensionPtr Provider::create(const Context& context,
                              const Request& request) const
{
    Properties properties{};

    properties.diagnosticDataType = ContentType::CPER;
    properties.notificationType =
        request.data.value(notificationTypeKey, std::string{});
    properties.sectionType = request.data.value(sectionTypeKey, std::string{});
    properties.oem = request.data.value(oemKey, OemMetadata{});

    return std::make_unique<Extension>(context.bus, context.objectPath,
                                       std::move(properties));
}

ExtensionPtr Provider::restore(const Context& context,
                               const nlohmann::json& data) const
{
    Properties properties{};

    properties.diagnosticDataType =
        data.at(diagnosticDataTypeKey).get<ContentType>();
    properties.notificationType =
        data.at(notificationTypeKey).get<std::string>();
    properties.sectionType = data.at(sectionTypeKey).get<std::string>();
    properties.oem = data.at(oemKey).get<OemMetadata>();

    return std::make_unique<Extension>(context.bus, context.objectPath,
                                       std::move(properties));
}

void registerProvider(Registry& registry)
{
    registry.registerProvider(interface, std::make_unique<Provider>());
}

} // namespace phosphor::logging::event_extensions::cper::processed
