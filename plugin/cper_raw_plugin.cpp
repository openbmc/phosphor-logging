#include "config.h"

#include "plugin/cper_raw_plugin.hpp"

#include <phosphor-logging/lg2.hpp>

#include <stdexcept>
#include <utility>

namespace phosphor::logging::plugin::cper::raw
{

Plugin::Plugin(const PluginContext& context, const Descriptor& descriptor) :
    Interface(context.bus, context.objectPath.c_str()),
    artifact_(descriptor.properties().artifact)
{}

std::string_view Plugin::interface() const
{
    return cper::raw::interface;
}

sdbusplus::message::unix_fd Plugin::getFileHandle()
{
    auto fd = utils::openArtifact(artifact_);
    if (fd < 0)
    {
        throw std::runtime_error("Failed to open GetFileHandle artifact");
    }

    return sdbusplus::message::unix_fd(fd);
}

PluginPtr Factory::create(const PluginContext& context,
                          const plugin::Descriptor& descriptor) const
{
    if (descriptor.interface() != cper::raw::interface)
    {
        throw std::invalid_argument(
            "CPERRaw factory received non-CPER Raw descriptor");
    }
    const auto& rawDescriptor = dynamic_cast<const Descriptor&>(descriptor);

    return std::make_unique<Plugin>(context, rawDescriptor);
}

void registerPlugin(PluginRegistry& registry)
{
    registry.registerPlugin(interface, std::make_unique<Factory>());
}

plugin::DescriptorPtr Factory::createDescriptor(
    const plugin::Request& request) const
{
    const auto payload = request.data.value(dataKey, std::vector<uint8_t>{});
    if (payload.empty())
    {
        return nullptr;
    }
    if (payload.size() > MAX_CPER_RAW_BINARY_SIZE)
    {
        lg2::warning("Ignoring oversized CPERRaw payload. "
                     "SIZE={SIZE} LIMIT={LIMIT}",
                     "SIZE", payload.size(), "LIMIT", MAX_CPER_RAW_BINARY_SIZE);

        return nullptr;
    }

    auto artifact = utils::persistArtifact(payload, cperRawArtifactSuffix);
    if (!artifact)
    {
        lg2::error("Failed to persist CPERRaw artifact");

        return nullptr;
    }

    Properties properties{
        .artifact = std::move(artifact),
    };

    return std::make_unique<Descriptor>(std::move(properties));
}

} // namespace phosphor::logging::plugin::cper::raw
