#pragma once

#include "plugin/cper_processed_descriptor.hpp"
#include "plugin/plugin.hpp"
#include "plugin/plugin_factory.hpp"
#include "plugin/plugin_registry.hpp"
#include "plugin/plugin_request.hpp"

#include <nlohmann/json.hpp>
#include <xyz/openbmc_project/Logging/Extension/CPERProcessed/server.hpp>

namespace phosphor::logging::plugin::cper
{

using ProcessedIface =
    sdbusplus::server::xyz::openbmc_project::logging::extension::CPERProcessed;

inline constexpr auto diagnosticDataTypeKey = "DiagnosticDataType";
inline constexpr auto notificationTypeKey = "NotificationType";
inline constexpr auto sectionTypeKey = "SectionType";
inline constexpr auto oemKey = "Oem";
/**
 * @brief CPERProcessed plugin implementation.
 *
 * Augments a log entry with the
 * xyz.openbmc_project.Logging.Extension.CPERProcessed
 * interface.
 *
 * The plugin exposes processed CPER metadata associated
 * with a logging entry and does not manage the underlying
 * CPER artifact.
 */
class ProcessedPlugin : public phosphor::logging::Plugin, public ProcessedIface
{
  public:
    ProcessedPlugin() = delete;
    ProcessedPlugin(const ProcessedPlugin&) = delete;
    ProcessedPlugin(ProcessedPlugin&&) = delete;
    ProcessedPlugin& operator=(const ProcessedPlugin&) = delete;
    ProcessedPlugin& operator=(ProcessedPlugin&&) = delete;
    ~ProcessedPlugin() override = default;

    /**
     * @brief Construct a CPERProcessed plugin.
     *
     * @param[in] context Plugin creation context.
     * @param[in] descriptor CPERProcessed descriptor.
     */
    ProcessedPlugin(const PluginContext& context,
                    const ProcessedDescriptor& descriptor);

    /**
     * @brief Return plugin interface identifier.
     *
     * @return Plugin interface name.
     */
    std::string_view interface() const override;
};

/**
 * @brief Factory for CPERProcessed plugins.
 */
class ProcessedFactory : public phosphor::logging::PluginFactory
{
  public:
    /**
     * @brief Construct a CPERProcessed factory.
     */
    ProcessedFactory() = default;

    ProcessedFactory(const ProcessedFactory&) = delete;
    ProcessedFactory(ProcessedFactory&&) = delete;
    ProcessedFactory& operator=(const ProcessedFactory&) = delete;
    ProcessedFactory& operator=(ProcessedFactory&&) = delete;

    ~ProcessedFactory() override = default;

    /**
     * @brief Create a CPERProcessed plugin instance.
     *
     * @param[in] context Plugin creation context.
     * @param[in] descriptor Plugin descriptor.
     *
     * @return Newly created CPERProcessed plugin.
     */
    PluginPtr create(const PluginContext& context,
                     const plugin::Descriptor& descriptor) const override;

    /**
     * @brief Create a CPERProcessed descriptor.
     *
     * Converts request metadata into a runtime
     * CPERProcessed descriptor.
     *
     * @param[in] request Plugin request information.
     *
     * @return CPERProcessed descriptor.
     */
    plugin::DescriptorPtr createDescriptor(
        const plugin::Request& request) const override;

    /**
     * @brief Build CPERProcessed extension payload.
     *
     * Converts runtime metadata into the payload stored under
     * the CPER entry within the _EXTENSIONS transport object.
     *
     * @param[in] metadata Runtime metadata.
     *
     * @return CPER extension payload.
     */
    nlohmann::json buildExtensionPayload(
        const nlohmann::json& metadata) const override;
};

/**
 * @brief Register the CPERProcessed plugin implementation.
 *
 * @param[in,out] registry Plugin registry.
 */
void registerProcessedPlugin(PluginRegistry& registry);

} // namespace phosphor::logging::plugin::cper
