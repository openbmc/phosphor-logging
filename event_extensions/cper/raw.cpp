#include "config.h"

#include "event_extensions/cper/raw.hpp"

#include <phosphor-logging/lg2.hpp>

#include <stdexcept>

namespace phosphor::logging::event_extensions::cper::raw
{

Extension::Extension(sdbusplus::bus_t& bus, const std::string& objectPath,
                     utils::ArtifactRef artifact) :
    Interface(bus, objectPath.c_str()), artifact_(std::move(artifact))
{}

sdbusplus::message::unix_fd Extension::getFileHandle()
{
    auto fd = utils::openArtifact(artifact_);

    if (fd < 0)
    {
        throw std::runtime_error("Failed to open CPER Raw artifact");
    }

    return sdbusplus::message::unix_fd(fd);
}

void Extension::onDelete()
{
    utils::removeArtifact(artifact_);
}

nlohmann::json Extension::serialize() const
{
    return {
        {artifactPathKey, artifact_.path},
    };
}

ExtensionPtr Provider::create(const Context& context,
                              const Request& request) const
{
    const auto payload = request.data.value(dataKey, std::vector<uint8_t>{});

    if (payload.empty())
    {
        return nullptr;
    }

    if (payload.size() > MAX_CPER_RAW_BINARY_SIZE)
    {
        lg2::warning("Ignoring oversized CPER Raw payload. "
                     "SIZE={SIZE} LIMIT={LIMIT}",
                     "SIZE", payload.size(), "LIMIT", MAX_CPER_RAW_BINARY_SIZE);

        return nullptr;
    }

    try
    {
        auto artifact = utils::persistArtifact(payload, cperRawArtifactSuffix);

        return std::make_unique<Extension>(context.bus, context.objectPath,
                                           std::move(artifact));
    }
    catch (const std::exception& e)
    {
        lg2::error("Failed to persist CPER Raw artifact: {ERROR}", "ERROR",
                   e.what());

        return nullptr;
    }
}

ExtensionPtr Provider::restore(const Context& context,
                               const nlohmann::json& data) const
{
    return std::make_unique<Extension>(
        context.bus, context.objectPath,
        utils::ArtifactRef{
            .path = data.at(artifactPathKey).get<std::filesystem::path>(),
        });
}

void registerProvider(Registry& registry)
{
    registry.registerProvider(interface, std::make_unique<Provider>());
}

} // namespace phosphor::logging::event_extensions::cper::raw
