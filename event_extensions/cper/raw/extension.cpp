#include "config.h"

#include "event_extensions/cper/raw/extension.hpp"

#include "paths.hpp"

#include <unistd.h>

#include <phosphor-logging/lg2.hpp>

#include <functional>
#include <stdexcept>

namespace phosphor::logging::event_extensions::cper::raw
{

Extension::Extension(sdbusplus::bus_t& bus, const std::string& objectPath,
                     utils::ArtifactRef artifact) :
    Interface(bus, objectPath.c_str()), artifact(std::move(artifact))
{}

sdbusplus::message::unix_fd Extension::getFileHandle()
{
    auto fd = utils::openArtifact(artifact);

    if (fd < 0)
    {
        throw std::runtime_error("Failed to open CPER Raw artifact");
    }

    auto event = sdeventplus::Event::get_default();
    fdCloseEventSource = std::make_unique<sdeventplus::source::Defer>(
        event, std::bind_front(&Extension::closeFD, this, fd));

    return fd;
}

void Extension::closeFD(int fd, sdeventplus::source::EventBase&)
{
    ::close(fd);
    fdCloseEventSource.reset();
}

void Extension::onDelete()
{
    utils::removeArtifact(artifact);
}

nlohmann::json Extension::serialize() const
{
    return {
        {artifactPathKey,
         artifact ? nlohmann::json(*artifact) : nlohmann::json()},
    };
}

ExtensionPtr create(const Context& context, const Request& request)
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
        auto artifact = utils::persistArtifact(payload, cperRawArtifactSuffix,
                                               paths::artifact());

        return std::make_unique<Extension>(context.bus, context.objectPath,
                                           std::move(artifact));
    }
    catch (const std::exception& e)
    {
        lg2::error("Failed to persist CPER Raw artifact: "
                   "{ERROR}",
                   "ERROR", e);

        return nullptr;
    }
}

ExtensionPtr restore(const Context& context, const nlohmann::json& data)
{
    auto artifact = data.at(artifactPathKey).get<std::filesystem::path>();

    return std::make_unique<Extension>(context.bus, context.objectPath,
                                       utils::ArtifactRef{std::move(artifact)});
}

void registerExtension(Registry& registry)
{
    registry.registerExtension(interface, {
                                              .createCallback = create,
                                              .restoreCallback = restore,
                                          });
}

} // namespace phosphor::logging::event_extensions::cper::raw
