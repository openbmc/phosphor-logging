#pragma once

#include "plugin/cper_raw_descriptor.hpp"
#include "plugin/plugin.hpp"
#include "plugin/plugin_factory.hpp"
#include "plugin/plugin_registry.hpp"
#include "plugin/plugin_request.hpp"

#include <xyz/openbmc_project/Logging/CPER/Raw/server.hpp>

namespace phosphor::logging::plugin::cper::raw
{

inline constexpr auto dataKey = "Data";
inline constexpr auto cperRawArtifactSuffix = "cperraw";
inline constexpr auto artifactPathKey = "ArtifactPath";

/** CPER Raw runtime D-Bus interface. */
using Interface = sdbusplus::server::xyz::openbmc_project::logging::cper::Raw;

/**
 * @brief CPER Raw plugin implementation.
 *
 * Augments a log entry with the
 * xyz.openbmc_project.Logging.CPER Raw interface.
 *
 * The plugin exposes persisted raw CPER payloads
 * associated with a logging entry.
 */
class Plugin : public phosphor::logging::Plugin, public Interface
{
  public:
    Plugin() = delete;
    Plugin(const Plugin&) = delete;
    Plugin(Plugin&&) = delete;
    Plugin& operator=(const Plugin&) = delete;
    Plugin& operator=(Plugin&&) = delete;
    ~Plugin() override = default;

    /**
     * @brief Construct a CPER Raw plugin.
     *
     * @param[in] context Plugin creation context.
     * @param[in] descriptor CPER Raw descriptor.
     */
    Plugin(const PluginContext& context, const Descriptor& descriptor);

    /**
     * @brief Return plugin interface identifier.
     *
     * @return Plugin interface name.
     */
    std::string_view interface() const override;

    /**
     * @brief Retrieve the raw CPER artifact.
     *
     * Opens the persisted CPER artifact and returns a
     * read-only file descriptor.
     *
     * @return File descriptor referencing the raw CPER
     *         payload.
     */
    sdbusplus::message::unix_fd getFileHandle() override;

    /**
     * @brief Cleanup plugin-owned resources.
     */
    void onDelete() override;

    /**
     * @brief Serialize CPER Raw plugin state.
     *
     * Persists the artifact reference associated with the
     * log entry.
     *
     * @return Serialized plugin state.
     */
    nlohmann::json serialize() const override;

  private:
    /** Persisted CPER artifact reference. */
    utils::ArtifactRef artifact_;
};

/**
 * @brief Factory for CPER Raw plugins.
 */
class Factory : public phosphor::logging::PluginFactory
{
  public:
    Factory() = default;
    Factory(const Factory&) = delete;
    Factory(Factory&&) = delete;
    Factory& operator=(const Factory&) = delete;
    Factory& operator=(Factory&&) = delete;
    ~Factory() override = default;

    /**
     * @brief Create a CPER Raw plugin instance.
     *
     * @param[in] context Plugin creation context.
     * @param[in] descriptor Plugin descriptor.
     *
     * @return Newly created CPER Raw plugin.
     */
    PluginPtr create(const PluginContext& context,
                     const plugin::Descriptor& descriptor) const override;

    /**
     * @brief Create a CPER Raw descriptor.
     *
     * Converts request metadata into a runtime
     * CPER Raw descriptor.
     *
     * Raw payloads are validated, persisted using the
     * artifact utility, and represented through an
     * ArtifactRef stored in the resulting descriptor.
     *
     * @param[in] request Plugin request information.
     *
     * @return CPERRaw descriptor.
     */
    plugin::DescriptorPtr createDescriptor(
        const plugin::Request& request) const override;

    /**
     * @brief Restore a CPER Raw plugin instance.
     *
     * Reconstructs a CPER Raw descriptor from persisted
     * plugin state and recreates the runtime plugin.
     *
     * @param[in] context Plugin runtime context.
     * @param[in] data Serialized plugin state.
     *
     * @return Restored plugin instance.
     */
    PluginPtr deserialize(const PluginContext& context,
                          const nlohmann::json& data) const override;
};

/**
 * @brief Register the CPER Raw plugin implementation.
 *
 * @param[in,out] registry Plugin registry.
 */
void registerPlugin(PluginRegistry& registry);

} // namespace phosphor::logging::plugin::cper::raw
