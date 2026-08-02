#pragma once

#include "plugin/plugin_descriptor.hpp"

#include <xyz/openbmc_project/Logging/CPER/Types/common.hpp>

#include <map>
#include <string>
#include <string_view>
#include <utility>

namespace phosphor::logging::plugin::cper::processed
{

using ContentType =
    sdbusplus::common::xyz::openbmc_project::logging::cper::Types::ContentType;

using OemMetadata = std::map<std::string, std::string>;

inline constexpr std::string_view interface =
    "xyz.openbmc_project.Logging.Extension.CPER.Processed";

/**
 * @brief CPER Processed interface properties.
 *
 * Represents the schema-defined properties of the
 * xyz.openbmc_project.Logging.Extension.CPER.Processed
 * interface.
 *
 * This structure should remain aligned with the D-Bus
 * interface YAML definition and serves as the canonical
 * in-memory representation of CPER Processed metadata within
 * the plugin framework.
 */
struct Properties
{
    /** Record or section level CPER content. */
    ContentType diagnosticDataType;

    /** CPER Notification Type GUID. */
    std::string notificationType;

    /** CPER Section Type GUID. */
    std::string sectionType;

    /** OEM-specific CPER metadata. */
    OemMetadata oem;
};

/**
 * @brief CPER Processed plugin creation descriptor.
 *
 * Carries schema-backed CPER Processed metadata extracted
 * from a logging event and provides the input used to
 * construct a runtime CPER Processed plugin instance.
 */
class Descriptor : public phosphor::logging::plugin::Descriptor
{
  public:
    Descriptor() = delete;
    Descriptor(const Descriptor&) = delete;
    Descriptor(Descriptor&&) = default;
    Descriptor& operator=(const Descriptor&) = delete;
    Descriptor& operator=(Descriptor&&) = default;
    ~Descriptor() override = default;

    /**
     * @brief Construct a CPER Processed descriptor.
     *
     * @param[in] properties
     *     CPER Processed interface properties.
     */
    explicit Descriptor(Properties properties) :
        properties_(std::move(properties))
    {}

    /**
     * @brief Return plugin interface identifier.
     *
     * @return Interface name.
     */
    std::string_view interface() const override
    {
        return cper::processed::interface;
    }

    /**
     * @brief Return CPER Processed properties.
     *
     * Consumers should prefer this accessor over individual
     * field-specific accessors to keep descriptor usage
     * aligned with the underlying schema definition.
     *
     * @return CPER Processed properties.
     */
    const Properties& properties() const
    {
        return properties_;
    }

  private:
    /** Schema-backed CPER Processed properties. */
    Properties properties_;
};

} // namespace phosphor::logging::plugin::cper::processed
