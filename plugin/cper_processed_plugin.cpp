#include "plugin/cper_processed_plugin.hpp"

#include <stdexcept>

namespace phosphor::logging::plugin::cper::processed
{

Plugin::Plugin(const PluginContext& context, const Descriptor& descriptor) :
    Interface(context.bus, context.objectPath.c_str())
{
    const auto& properties = descriptor.properties();

    diagnosticDataType(properties.diagnosticDataType, true);
    notificationType(properties.notificationType, true);
    sectionType(properties.sectionType, true);
    oem(properties.oem, true);
}

std::string_view Plugin::interface() const
{
    return cper::processed::interface;
}

nlohmann::json Plugin::serialize() const
{
    return {
        {diagnosticDataTypeKey, diagnosticDataType()},
        {notificationTypeKey, notificationType()},
        {sectionTypeKey, sectionType()},
        {oemKey, oem()},
    };
}

PluginPtr Factory::create(const PluginContext& context,
                          const plugin::Descriptor& descriptor) const
{
    if (descriptor.interface() != cper::processed::interface)
    {
        throw std::invalid_argument(
            "CPER Processed factory received non-CPER Processed descriptor");
    }

    const auto& cperDescriptor = dynamic_cast<const Descriptor&>(descriptor);

    return std::make_unique<Plugin>(context, cperDescriptor);
}

void registerPlugin(PluginRegistry& registry)
{
    registry.registerPlugin(cper::processed::interface,
                            std::make_unique<Factory>());
}

plugin::DescriptorPtr Factory::createDescriptor(
    const plugin::Request& request) const
{
    Properties properties{};

    properties.diagnosticDataType = ContentType::CPER;
    properties.notificationType =
        request.data.value(notificationTypeKey, std::string{});
    properties.sectionType = request.data.value(sectionTypeKey, std::string{});
    properties.oem = request.data.value(oemKey, OemMetadata{});

    return std::make_unique<Descriptor>(std::move(properties));
}

nlohmann::json Factory::buildExtensionPayload(
    const nlohmann::json& metadata) const
{
    auto oem = nlohmann::json::object();

    for (const auto& [ns, value] : metadata.items())
    {
        oem[ns] = value.dump();
    }

    return {
        {oemKey, std::move(oem)},
    };
}

PluginPtr Factory::deserialize(const PluginContext& context,
                               const nlohmann::json& data) const
{
    Properties properties{};

    properties.diagnosticDataType =
        data.at(diagnosticDataTypeKey).get<ContentType>();
    properties.notificationType =
        data.at(notificationTypeKey).get<std::string>();
    properties.sectionType = data.at(sectionTypeKey).get<std::string>();
    properties.oem = data.at(oemKey).get<OemMetadata>();
    Descriptor descriptor{std::move(properties)};

    return create(context, descriptor);
}

} // namespace phosphor::logging::plugin::cper::processed
