#include "plugin/cper_processed_plugin.hpp"

#include <stdexcept>

namespace phosphor::logging::plugin::cper
{

ProcessedPlugin::ProcessedPlugin(const PluginContext& context,
                                 const ProcessedDescriptor& descriptor) :
    ProcessedIface(context.bus, context.objectPath.c_str())
{
    const auto& properties = descriptor.properties();

    diagnosticDataType(properties.diagnosticDataType, true);
    notificationType(properties.notificationType, true);
    sectionType(properties.sectionType, true);
    oem(properties.oem, true);
}

std::string_view ProcessedPlugin::interface() const
{
    return cper::interface;
}

PluginPtr ProcessedFactory::create(const PluginContext& context,
                                   const plugin::Descriptor& descriptor) const
{
    if (descriptor.interface() != cper::interface)
    {
        throw std::invalid_argument(
            "CPERProcessed factory received non-CPERProcessed descriptor");
    }

    const auto& cperDescriptor =
        dynamic_cast<const ProcessedDescriptor&>(descriptor);

    return std::make_unique<ProcessedPlugin>(context, cperDescriptor);
}

void registerProcessedPlugin(PluginRegistry& registry)
{
    registry.registerPlugin(cper::interface,
                            std::make_unique<ProcessedFactory>());
}

plugin::DescriptorPtr ProcessedFactory::createDescriptor(
    const plugin::Request& request) const
{
    Properties properties{};

    properties.diagnosticDataType = ContentType::CPER;
    properties.notificationType =
        request.data.value(notificationTypeKey, std::string{});
    properties.sectionType = request.data.value(sectionTypeKey, std::string{});
    properties.oem = request.data.value(oemKey, OemMetadata{});

    return std::make_unique<ProcessedDescriptor>(std::move(properties));
}

nlohmann::json ProcessedFactory::buildExtensionPayload(
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

} // namespace phosphor::logging::plugin::cper
