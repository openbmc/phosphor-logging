#pragma once

#include "plugin/plugin_descriptor.hpp"
#include "plugin/plugin_type.hpp"

#include <nlohmann/json.hpp>
#include <xyz/openbmc_project/Logging/Diagnostic/CPER/common.hpp>

#include <map>
#include <string>

namespace phosphor::logging::plugin::cper
{

using DiagnosticDataType = sdbusplus::common::xyz::openbmc_project::logging::
    diagnostic::CPER::DiagnosticDataType;

/**
 * CPER plugin creation descriptor.
 *
 * Carries producer-supplied CPER metadata into the
 * plugin framework.
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
     * Construct a CPER descriptor.
     *
     * @param[in] diagnosticDataType CPER diagnostic data type.
     * @param[in] notificationType CPER notification type GUID.
     * @param[in] sectionType CPER section type GUID.
     * @param[in] cperFd File descriptor containing CPER data.
     * @param[in] oem OEM-specific metadata.
     */
    Descriptor(DiagnosticDataType diagnosticDataType,
               std::string notificationType, std::string sectionType,
               int cperFd,
               nlohmann::json oemMetadata = nlohmann::json::object());

    /**
     * Return the plugin type.
     *
     * @return Plugin type.
     */
    Type type() const override
    {
        return Type::cper;
    }

    /**
     * Return the diagnostic data type.
     *
     * @return Diagnostic data type.
     */
    DiagnosticDataType diagnosticDataType() const;

    /**
     * Return the notification type GUID.
     *
     * @return Notification type GUID.
     */
    const std::string& notificationType() const;

    /**
     * Return the section type GUID.
     *
     * @return Section type GUID.
     */
    const std::string& sectionType() const;

    /**
     * Return the CPER data file descriptor.
     *
     * @return CPER file descriptor.
     */
    int cperFd() const;

    /**
     * Return OEM metadata.
     *
     * @return OEM metadata.
     */
    const nlohmann::json& oem() const;

  private:
    /** CPER diagnostic data type. */
    DiagnosticDataType diagnosticDataTypeValue;

    /** CPER notification type GUID. */
    std::string notificationTypeGuid;

    /** CPER section type GUID. */
    std::string sectionTypeGuid;

    /** CPER binary file descriptor. */
    int cperDataFd;

    /**
     * @brief OEM metadata.
     *
     * Contains implementation-defined OEM information
     * associated with the CPER record.
     */
    nlohmann::json oemMetadata;
};

} // namespace phosphor::logging::plugin::cper
