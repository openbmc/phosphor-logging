#pragma once

#include "plugin/cper_processed_descriptor.hpp"
#include "plugin/plugin.hpp"
#include "plugin/plugin_factory.hpp"
#include "plugin/plugin_registry.hpp"
#include "plugin/plugin_request.hpp"

#include <nlohmann/json.hpp>
#include <xyz/openbmc_project/Logging/Extension/CPER/Processed/server.hpp>

namespace phosphor::logging::plugin::cper::processed
{

using Interface = sdbusplus::server::xyz::openbmc_project::logging::extension::
    cper::Processed;

inline constexpr auto diagnosticDataTypeKey = "DiagnosticDataType";
inline constexpr auto notificationTypeKey = "NotificationType";
inline constexpr auto sectionTypeKey = "SectionType";
inline constexpr auto oemKey = "Oem";
/**
 * @brief CPE RProcessed plugin implementation.
 *
 * Augments a log entry with the
 * xyz.openbmc_project.Logging.Extension.CPER.Processed
 * interface.
 *
 * The plugin exposes processed CPER metadata associated
 * with a logging entry and does not manage the underlying
 * CPER artifact.
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
     * @brief Construct a CPER Processed plugin.
     *
     * @param[in] context Plugin creation context.
     * @param[in] descriptor CPER Processed descriptor.
     */
    Plugin(const PluginContext& context, const Descriptor& descriptor);

    /**
     * @brief Return plugin interface identifier.
     *
     * @return Plugin interface name.
     */
    std::string_view interface() const override;

    /**
     * @brief Serialize plugin state.
     *
     * @return Serialized CPER Processed properties.
     */
    nlohmann::json serialize() const override;
};

/**
 * @brief Factory for CPER Processed plugins.
 */
class Factory : public phosphor::logging::PluginFactory
{
  public:
    /**
     * @brief Construct a CPER Processed factory.
     */
    Factory() = default;

    Factory(const Factory&) = delete;
    Factory(Factory&&) = delete;
    Factory& operator=(const Factory&) = delete;
    Factory& operator=(Factory&&) = delete;

    ~Factory() override = default;

    /**
     * @brief Create a CPER Processed plugin instance.
     *
     * @param[in] context Plugin creation context.
     * @param[in] descriptor Plugin descriptor.
     *
     * @return Newly created CPER Processed plugin.
     */
    PluginPtr create(const PluginContext& context,
                     const plugin::Descriptor& descriptor) const override;

    /**
     * @brief Create a CPER Processed descriptor.
     *
     * Converts request metadata into a runtime
     * CPER Processed descriptor.
     *
     * @param[in] request Plugin request information.
     *
     * @return CPER Processed descriptor.
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

    /**
     * @brief Restore CPER Processed plugin.
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
 * @brief Register the CPER Processed plugin implementation.
 *
 * @param[in,out] registry Plugin registry.
 */
void registerPlugin(PluginRegistry& registry);

} // namespace phosphor::logging::plugin::cper::processed
