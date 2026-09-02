#pragma once

#include "event_extensions/extension.hpp"
#include "event_extensions/registry.hpp"
#include "event_extensions/utils/artifact.hpp"

#include <nlohmann/json.hpp>
#include <sdeventplus/event.hpp>
#include <sdeventplus/source/event.hpp>
#include <xyz/openbmc_project/Logging/CPER/Raw/server.hpp>

#include <memory>
#include <string>
#include <string_view>

namespace phosphor::logging::event_extensions::cper::raw
{

using Interface = sdbusplus::server::xyz::openbmc_project::logging::cper::Raw;

inline constexpr std::string_view interface =
    "xyz.openbmc_project.Logging.Extension.CPER.Raw";
inline constexpr auto dataKey = "Data";
inline constexpr auto artifactPathKey = "ArtifactPath";
inline constexpr auto cperRawArtifactSuffix = "cperraw";

/**
 * @brief CPER Raw event extension.
 *
 * Exposes a persisted CPER artifact through the
 * xyz.openbmc_project.Logging.Extension.CPER.Raw
 * interface.
 */
class Extension : public event_extensions::Extension, public Interface
{
  public:
    Extension() = delete;
    Extension(const Extension&) = delete;
    Extension(Extension&&) = delete;
    Extension& operator=(const Extension&) = delete;
    Extension& operator=(Extension&&) = delete;
    ~Extension() override = default;

    /**
     * @brief Construct a CPER Raw extension.
     *
     * @param[in] bus D-Bus connection.
     * @param[in] objectPath Log entry object path.
     * @param[in] artifact Persisted CPER artifact.
     */
    Extension(sdbusplus::bus_t& bus, const std::string& objectPath,
              utils::ArtifactRef artifact);

    /**
     * @brief Return implemented interface name.
     *
     * @return Interface name.
     */
    std::string_view interface() const override
    {
        return raw::interface;
    }

    /**
     * @brief Retrieve raw CPER artifact.
     *
     * @return File descriptor to the artifact.
     */
    sdbusplus::message::unix_fd getFileHandle() override;

    /**
     * @brief Cleanup extension-owned resources.
     */
    void onDelete() override;

    /**
     * @brief Serialize extension state.
     *
     * @return Serialized extension state.
     */
    nlohmann::json serialize() const override;

  private:
    /**
     * @brief Close a transferred file descriptor.
     *
     * Invoked after D-Bus transfer completes.
     *
     * @param[in] fd File descriptor to close.
     * @param[in] source Deferred event source.
     */
    void closeFD(int fd, sdeventplus::source::EventBase& source);

    /** Persisted artifact backing this extension. */
    utils::ArtifactRef artifact;

    /** Deferred cleanup for transferred file descriptors. */
    std::unique_ptr<sdeventplus::source::Defer> fdCloseEventSource;
};

/**
 * @brief Create a CPER Raw extension.
 *
 * @param[in] context Runtime creation context.
 * @param[in] request Extension request.
 *
 * @return Created extension.
 */
ExtensionPtr create(const Context& context, const Request& request);

/**
 * @brief Restore a CPER Raw extension.
 *
 * @param[in] context Runtime creation context.
 * @param[in] data Serialized extension state.
 *
 * @return Restored extension.
 */
ExtensionPtr restore(const Context& context, const nlohmann::json& data);

/**
 * @brief Register the CPER Raw extension.
 *
 * @param[in,out] registry Event extension registry.
 */
void registerExtension(Registry& registry);

} // namespace phosphor::logging::event_extensions::cper::raw
