#pragma once

#include "event_extensions/extension.hpp"
#include "event_extensions/provider.hpp"
#include "event_extensions/registry.hpp"
#include "event_extensions/utils/artifact.hpp"

#include <nlohmann/json.hpp>
#include <xyz/openbmc_project/Logging/CPER/Raw/server.hpp>

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
 * xyz.openbmc_project.Logging.CPER.Raw interface.
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
     * @brief Constructor.
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
    /** Persisted artifact backing this extension. */
    utils::ArtifactRef artifact_;
};

/**
 * @brief Provider for CPER Raw extensions.
 */
class Provider : public event_extensions::Provider
{
  public:
    ~Provider() override = default;

    ExtensionPtr create(const Context& context,
                        const Request& request) const override;

    ExtensionPtr restore(const Context& context,
                         const nlohmann::json& data) const override;
};

/**
 * @brief Register the CPER Raw provider.
 *
 * @param[in,out] registry Event extension registry.
 */
void registerProvider(Registry& registry);

} // namespace phosphor::logging::event_extensions::cper::raw
