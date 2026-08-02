#include "plugin/cper_descriptor.hpp"

#include <utility>

namespace phosphor::logging::plugin::cper
{

Descriptor::Descriptor(DiagnosticDataType diagnosticDataType,
                       std::string notificationType, std::string sectionType,
                       int cperFd, nlohmann::json oem) :
    diagnosticDataTypeValue(diagnosticDataType),
    notificationTypeGuid(std::move(notificationType)),
    sectionTypeGuid(std::move(sectionType)), cperDataFd(cperFd),
    oemMetadata(std::move(oem))
{}

DiagnosticDataType Descriptor::diagnosticDataType() const
{
    return diagnosticDataTypeValue;
}

const std::string& Descriptor::notificationType() const
{
    return notificationTypeGuid;
}

const std::string& Descriptor::sectionType() const
{
    return sectionTypeGuid;
}

int Descriptor::cperFd() const
{
    return cperDataFd;
}

const nlohmann::json& Descriptor::oem() const
{
    return oemMetadata;
}

} // namespace phosphor::logging::plugin::cper
