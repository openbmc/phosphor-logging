#include "plugin/cper_processed_plugin.hpp"

#include <stdexcept>

namespace phosphor::logging::plugin::cperprocessed
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
    return cperprocessed::interface;
}

PluginPtr Factory::create(const PluginContext& context,
                          const plugin::Descriptor& descriptor) const
{
    if (descriptor.interface() != cperprocessed::interface)
    {
        throw std::invalid_argument(
            "CPERProcessed factory received non-CPERProcessed descriptor");
    }

    const auto& cperDescriptor = dynamic_cast<const Descriptor&>(descriptor);

    return std::make_unique<Plugin>(context, cperDescriptor);
}

void registerPlugin(PluginRegistry& registry)
{
    registry.registerPlugin(cperprocessed::interface,
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
} // namespace phosphor::logging::plugin::cperprocessed
