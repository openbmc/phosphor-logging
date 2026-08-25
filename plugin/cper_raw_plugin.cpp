#include "plugin/cper_raw_plugin.hpp"

#include <phosphor-logging/lg2.hpp>

#include <stdexcept>
#include <utility>

namespace phosphor::logging::plugin::cperraw
{

namespace
{

constexpr std::size_t maxCPERRawPayloadSize = 16 * 1024;

} // namespace

Plugin::Plugin(const PluginContext& context, const Descriptor& descriptor) :
    RawIface(context.bus, context.objectPath.c_str()),
    artifact_(descriptor.properties().artifact)
{}

std::string_view Plugin::interface() const
{
    return cperraw::interface;
}

sdbusplus::message::unix_fd Plugin::getCPERRaw()
{
    auto fd = utils::openArtifact(artifact_);

    if (fd < 0)
    {
        throw std::runtime_error("Failed to open CPERRaw artifact");
    }

    return sdbusplus::message::unix_fd(fd);
}

PluginPtr Factory::create(const PluginContext& context,
                          const plugin::Descriptor& descriptor) const
{
    if (descriptor.interface() != cperraw::interface)
    {
        throw std::invalid_argument(
            "CPERRaw factory received non-CPERRaw descriptor");
    }

    const auto& rawDescriptor = dynamic_cast<const Descriptor&>(descriptor);

    return std::make_unique<Plugin>(context, rawDescriptor);
}

void registerPlugin(PluginRegistry& registry)
{
    registry.registerPlugin(cperraw::interface, std::make_unique<Factory>());
}

plugin::DescriptorPtr Factory::createDescriptor(
    const plugin::Request& request) const
{
    const auto payload = request.data.value(dataKey, std::vector<uint8_t>{});

    if (payload.empty())
    {
        return nullptr;
    }

    if (payload.size() > maxCPERRawPayloadSize)
    {
        lg2::warning("Ignoring oversized CPERRaw payload. "
                     "SIZE={SIZE} LIMIT={LIMIT}",
                     "SIZE", payload.size(), "LIMIT", maxCPERRawPayloadSize);

        return nullptr;
    }

    auto artifact = utils::persistArtifact(payload, "cperraw");

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

} // namespace phosphor::logging::plugin::cperraw
