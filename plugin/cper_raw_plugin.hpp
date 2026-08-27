#pragma once

#include "plugin/cper_raw_descriptor.hpp"
#include "plugin/plugin.hpp"
#include "plugin/plugin_factory.hpp"
#include "plugin/plugin_registry.hpp"
#include "plugin/plugin_request.hpp"

#include <xyz/openbmc_project/Logging/CPERRaw/server.hpp>

namespace phosphor::logging::plugin::cperraw
{

inline constexpr auto dataKey = "Data";

/** CPERRaw runtime D-Bus interface. */
using RawIface = sdbusplus::server::xyz::openbmc_project::logging::CPERRaw;

/**
 * @brief CPERRaw plugin implementation.
 *
 * Augments a log entry with the
 * xyz.openbmc_project.Logging.CPERRaw interface.
 *
 * The plugin exposes persisted raw CPER payloads
 * associated with a logging entry.
 */
class Plugin : public phosphor::logging::Plugin, public RawIface
{
  public:
    Plugin() = delete;
    Plugin(const Plugin&) = delete;
    Plugin(Plugin&&) = delete;
    Plugin& operator=(const Plugin&) = delete;
    Plugin& operator=(Plugin&&) = delete;
    ~Plugin() override = default;

    /**
     * @brief Construct a CPERRaw plugin.
     *
     * @param[in] context Plugin creation context.
     * @param[in] descriptor CPERRaw descriptor.
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
    sdbusplus::message::unix_fd getCPERRaw() override;

    /**
     * @brief Cleanup plugin-owned resources.
     */
    void onDelete() override;

  private:
    /** Persisted CPER artifact reference. */
    utils::ArtifactRef artifact_;
};

/**
 * @brief Factory for CPERRaw plugins.
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
     * @brief Create a CPERRaw plugin instance.
     *
     * @param[in] context Plugin creation context.
     * @param[in] descriptor Plugin descriptor.
     *
     * @return Newly created CPERRaw plugin.
     */
    PluginPtr create(const PluginContext& context,
                     const plugin::Descriptor& descriptor) const override;

    /**
     * @brief Create a CPERRaw descriptor.
     *
     * Converts request metadata into a runtime
     * CPERRaw descriptor.
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
};

/**
 * @brief Register the CPERRaw plugin implementation.
 *
 * @param[in,out] registry Plugin registry.
 */
void registerPlugin(PluginRegistry& registry);

} // namespace phosphor::logging::plugin::cperraw
