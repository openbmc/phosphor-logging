#include "severity.hpp"

#include "pel_types.hpp"

namespace openpower
{
namespace pels
{

using LogSeverity = phosphor::logging::Entry::Level;

uint8_t convertOBMCSeverityToPEL(LogSeverity severity)
{
    uint8_t pelSeverity = static_cast<uint8_t>(SeverityType::unrecoverable);
    switch (severity)
    {
        case (LogSeverity::Notice):
        case (LogSeverity::Informational):
        case (LogSeverity::Debug):
            pelSeverity = static_cast<uint8_t>(SeverityType::nonError);
            break;

        case (LogSeverity::Warning):
            pelSeverity = static_cast<uint8_t>(SeverityType::predictive);
            break;

        case (LogSeverity::Critical):
            pelSeverity = static_cast<uint8_t>(SeverityType::critical);
            break;

        case (LogSeverity::Emergency):
        case (LogSeverity::Alert):
        case (LogSeverity::Error):
            pelSeverity = static_cast<uint8_t>(SeverityType::unrecoverable);
            break;
    }

    return pelSeverity;
}
} // namespace pels
} // namespace openpower
