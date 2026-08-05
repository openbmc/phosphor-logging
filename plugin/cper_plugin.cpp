#include "plugin/cper_plugin.hpp"

#include "plugin/cper_artifact.hpp"

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
    if (descriptor.type() != plugin::Type::cper)
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
    auto notificationType = info.data.value("NotificationType", std::string{});

    auto sectionType = info.data.value("SectionType", std::string{});

    auto cperFd = info.data.value(cper::cperFdKey, -1);

    auto oem = info.data.value("Oem", nlohmann::json::object());

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

plugin::Type Plugin::type() const
{
    return plugin::Type::cper;
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
    registry.registerPlugin(plugin::Type::cper, std::make_unique<Factory>());
}

} // namespace phosphor::logging::plugin::cper
