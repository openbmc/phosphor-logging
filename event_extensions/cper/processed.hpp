#pragma once

#include "event_extensions/extension.hpp"
#include "event_extensions/provider.hpp"
#include "event_extensions/registry.hpp"

#include <nlohmann/json.hpp>
#include <xyz/openbmc_project/Logging/CPER/Types/common.hpp>
#include <xyz/openbmc_project/Logging/Extension/CPER/Processed/server.hpp>

#include <map>
#include <string>
#include <string_view>

namespace phosphor::logging::event_extensions::cper::processed
{

using Interface = sdbusplus::server::xyz::openbmc_project::logging::extension::
    cper::Processed;
using ContentType =
    sdbusplus::common::xyz::openbmc_project::logging::cper::Types::ContentType;
using OemMetadata = std::map<std::string, std::string>;

inline constexpr std::string_view interface =
    "xyz.openbmc_project.Logging.Extension.CPER.Processed";
inline constexpr auto diagnosticDataTypeKey = "DiagnosticDataType";
inline constexpr auto notificationTypeKey = "NotificationType";
inline constexpr auto sectionTypeKey = "SectionType";
inline constexpr auto oemKey = "Oem";

/**
 * @brief CPER Processed interface properties.
 *
 * Represents the schema-defined properties of the
 * xyz.openbmc_project.Logging.Extension.CPER.Processed
 * interface.
 */
struct Properties
{
    ContentType diagnosticDataType;
    std::string notificationType;
    std::string sectionType;
    OemMetadata oem;
};

/**
 * @brief CPER Processed event extension.
 *
 * Provides the
 * xyz.openbmc_project.Logging.Extension.CPER.Processed
 * D-Bus interface on a log entry.
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
     * @brief Construct a CPER Processed extension.
     *
     * @param[in] bus D-Bus connection.
     * @param[in] objectPath Log entry object path.
     * @param[in] properties CPER Processed properties.
     */
    Extension(sdbusplus::bus_t& bus, const std::string& objectPath,
              Properties properties);

    /**
     * @brief Return implemented interface name.
     *
     * @return Interface name.
     */
    std::string_view interface() const override
    {
        return processed::interface;
    }

    /**
     * @brief Serialize extension state.
     *
     * @return Serialized extension state.
     */
    nlohmann::json serialize() const override;
};

/**
 * @brief Provider for CPER Processed extensions.
 */
class Provider : public event_extensions::Provider
{
  public:
    ~Provider() override = default;

    /**
     * @brief Create a CPER Processed extension.
     *
     * @param[in] context Runtime creation context.
     * @param[in] request Extension request.
     *
     * @return Created extension.
     */
    ExtensionPtr create(const Context& context,
                        const Request& request) const override;

    /**
     * @brief Restore a CPER Processed extension.
     *
     * @param[in] context Runtime creation context.
     * @param[in] data Serialized extension state.
     *
     * @return Restored extension.
     */
    ExtensionPtr restore(const Context& context,
                         const nlohmann::json& data) const override;
};

/**
 * @brief Register the CPER Processed provider.
 *
 * @param[in,out] registry Event extension registry.
 */
void registerProvider(Registry& registry);

} // namespace phosphor::logging::event_extensions::cper::processed
