#pragma once

#include "plugin/cper_artifact.hpp"
#include "plugin/cper_descriptor.hpp"
#include "plugin/plugin.hpp"
#include "plugin/plugin_factory.hpp"
#include "plugin/plugin_registry.hpp"

#include <sdbusplus/message/native_types.hpp>
#include <xyz/openbmc_project/Logging/Diagnostic/CPER/server.hpp>

#include <filesystem>

namespace phosphor::logging::plugin::cper
{

using CperIface =
    sdbusplus::server::xyz::openbmc_project::logging::diagnostic::CPER;

/**
 * @brief CPER plugin implementation.
 *
 * Augments a log entry with the
 * xyz.openbmc_project.Logging.Diagnostic.CPER interface.
 *
 * The plugin consumes a CPER descriptor and exposes
 * CPER-specific metadata through the generated D-Bus
 * interface.
 */
class Plugin : public phosphor::logging::Plugin, public CperIface
{
  public:
    Plugin() = delete;
    Plugin(const Plugin&) = delete;
    Plugin(Plugin&&) = delete;
    Plugin& operator=(const Plugin&) = delete;
    Plugin& operator=(Plugin&&) = delete;
    ~Plugin() override = default;

    /**
     * @brief Construct a CPER plugin.
     *
     * @param[in] context Plugin creation context.
     * @param[in] descriptor CPER descriptor.
     * @param[in] artifactPath Persisted CPER artifact path.
     */
    Plugin(const PluginContext& context, const Descriptor& descriptor,
           std::filesystem::path artifactPath);
    /**
     * @brief Return plugin interface name.
     *
     * @return Plugin D-Bus interface name.
     */
    std::string_view interface() const override;

    /**
     * @brief Return the CPER binary associated with the log.
     *
     * @return Read-only CPER file descriptor.
     */
    sdbusplus::message::unix_fd getCPERBinary() override;

  private:
    /**
     * @brief Path to the persisted CPER artifact.
     */
    std::filesystem::path artifactPath_;
};

/**
 * @brief Factory for CPER runtime plugins.
 */
class Factory : public phosphor::logging::PluginFactory
{
  public:
    /**
     * @brief Construct a CPER plugin factory.
     *
     * @param[in] artifactRoot Root directory used for
     *            persisted CPER artifacts.
     */
    explicit Factory(
        std::filesystem::path artifactRoot = defaultArtifactDirectory) :
        artifactRoot(std::move(artifactRoot))
    {}

    ~Factory() override = default;

    /**
     * @brief Create a runtime CPER plugin.
     *
     * @param[in] context Plugin creation context.
     * @param[in] descriptor CPER descriptor.
     *
     * @return Runtime CPER plugin instance.
     */
    PluginPtr create(const PluginContext& context,
                     const plugin::Descriptor& descriptor) const override;

    /**
     * @brief Create a CPER descriptor from plugin request data.
     *
     * CPER descriptors require a producer supplied file
     * descriptor and therefore cannot currently be derived
     * from PluginInfo metadata alone.
     *
     * @param[in] info Plugin request information.
     *
     * @return nullptr.
     */
    plugin::DescriptorPtr createDescriptor(
        const plugin::Info& info) const override;

  private:
    /**
     * @brief Root directory used for persisted CPER artifacts.
     */
    std::filesystem::path artifactRoot;
};

/**
 * @brief Register the CPER plugin implementation.
 *
 * @param[in,out] registry Plugin registry.
 */
void registerPlugin(PluginRegistry& registry);

} // namespace phosphor::logging::plugin::cper
