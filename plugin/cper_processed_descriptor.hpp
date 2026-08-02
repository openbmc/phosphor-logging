#pragma once

#include "plugin/plugin_descriptor.hpp"

#include <xyz/openbmc_project/CPER/Types/common.hpp>

#include <map>
#include <string>
#include <string_view>
#include <utility>

namespace phosphor::logging::plugin::cper
{

using ContentType =
    sdbusplus::common::xyz::openbmc_project::cper::Types::ContentType;

using OemMetadata = std::map<std::string, std::string>;

inline constexpr std::string_view interface =
    "xyz.openbmc_project.Logging.Extension.CPERProcessed";

/**
 * @brief CPERProcessed interface properties.
 *
 * Represents the schema-defined properties of the
 * xyz.openbmc_project.Logging.Extension.CPERProcessed
 * interface.
 *
 * This structure should remain aligned with the D-Bus
 * interface YAML definition and serves as the canonical
 * in-memory representation of CPERProcessed metadata within
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
 * @brief CPERProcessed plugin creation descriptor.
 *
 * Carries schema-backed CPERProcessed metadata extracted
 * from a logging event and provides the input used to
 * construct a runtime CPERProcessed plugin instance.
 */
class ProcessedDescriptor : public phosphor::logging::plugin::Descriptor
{
  public:
    ProcessedDescriptor() = delete;
    ProcessedDescriptor(const ProcessedDescriptor&) = delete;
    ProcessedDescriptor(ProcessedDescriptor&&) = default;
    ProcessedDescriptor& operator=(const ProcessedDescriptor&) = delete;
    ProcessedDescriptor& operator=(ProcessedDescriptor&&) = default;
    ~ProcessedDescriptor() override = default;

    /**
     * @brief Construct a CPERProcessed descriptor.
     *
     * @param[in] properties
     *     CPERProcessed interface properties.
     */
    explicit ProcessedDescriptor(Properties properties) :
        properties_(std::move(properties))
    {}

    /**
     * @brief Return plugin interface identifier.
     *
     * @return Interface name.
     */
    std::string_view interface() const override
    {
        return cper::interface;
    }

    /**
     * @brief Return CPERProcessed properties.
     *
     * Consumers should prefer this accessor over individual
     * field-specific accessors to keep descriptor usage
     * aligned with the underlying schema definition.
     *
     * @return CPERProcessed properties.
     */
    const Properties& properties() const
    {
        return properties_;
    }

  private:
    /** Schema-backed CPERProcessed properties. */
    Properties properties_;
};

} // namespace phosphor::logging::plugin::cper
