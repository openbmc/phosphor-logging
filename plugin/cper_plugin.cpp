#include "plugin/cper_plugin.hpp"

#include <fcntl.h>

#include <nlohmann/json.hpp>

#include <stdexcept>
#include <system_error>
#include <utility>

namespace phosphor::logging::plugin::cper
{

PluginPtr Factory::create(const PluginContext& context,
                          const plugin::Descriptor& descriptor) const
{
    if (descriptor.interface() != cper::interface)
    {
        throw std::invalid_argument("CPER plugin received non-CPER descriptor");
    }

    const auto& cperDescriptor = static_cast<const Descriptor&>(descriptor);

    auto artifact = persistArtifact(context.objectPath, cperDescriptor.cperFd(),
                                    artifactRoot);

    return std::make_unique<Plugin>(context, cperDescriptor,
                                    std::move(artifact));
}

plugin::DescriptorPtr Factory::createDescriptor(const plugin::Info& info) const
{
    auto notificationType = info.data.value(notificationTypeKey, std::string{});
    auto sectionType = info.data.value(sectionTypeKey, std::string{});
    auto cperFd = info.data.value(cperFdKey, -1);
    auto oem = info.data.value(oemKey, nlohmann::json::object());

    return std::make_unique<Descriptor>(
        DiagnosticDataType::CPER, std::move(notificationType),
        std::move(sectionType), cperFd, std::move(oem));
}

Plugin::Plugin(const PluginContext& context, const Descriptor& descriptor,
               std::filesystem::path artifactPath) :
    CperIface(context.bus, context.objectPath.c_str()),
    artifactPath_(std::move(artifactPath))
{
    diagnosticDataType(descriptor.diagnosticDataType(), true);
    notificationType(descriptor.notificationType(), true);
    sectionType(descriptor.sectionType(), true);
    oem(descriptor.oem(), true);
}

std::string_view Plugin::interface() const
{
    return cper::interface;
}

sdbusplus::message::unix_fd Plugin::getCPERBinary()
{
    const auto fd = ::open(artifactPath_.c_str(), O_RDONLY);

    if (fd < 0)
    {
        throw std::system_error(errno, std::generic_category(),
                                "Failed to open CPER artifact");
    }

    return sdbusplus::message::unix_fd(fd);
}

void registerPlugin(PluginRegistry& registry)
{
    registry.registerPlugin(cper::interface, std::make_unique<Factory>());
}

} // namespace phosphor::logging::plugin::cper
