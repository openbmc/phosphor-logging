#include "severity.hpp"

namespace openpower
{
namespace pels
{

using LogSeverity = phosphor::logging::Entry::Level;

/**
 * @brief The major types of severity values, based on the
 *        the left nibble of the severity value.
 */
enum class PELSeverityType : uint8_t
{
    nonError = 0x00,
    recovered = 0x10,
    predictive = 0x20,
    unrecoverable = 0x40,
    critical = 0x50,
    diagnostic = 0x60,
    symptom = 0x70
};

uint8_t convertOBMCSeverityToPEL(LogSeverity severity)
{
    uint8_t pelSeverity = static_cast<uint8_t>(PELSeverityType::unrecoverable);
    switch (severity)
    {
        case (LogSeverity::Notice):
        case (LogSeverity::Informational):
        case (LogSeverity::Debug):
            pelSeverity = static_cast<uint8_t>(PELSeverityType::nonError);
            break;

        case (LogSeverity::Warning):
            pelSeverity = static_cast<uint8_t>(PELSeverityType::predictive);
            break;

        case (LogSeverity::Critical):
            pelSeverity = static_cast<uint8_t>(PELSeverityType::critical);
            break;

        case (LogSeverity::Emergency):
        case (LogSeverity::Alert):
        case (LogSeverity::Error):
            pelSeverity = static_cast<uint8_t>(PELSeverityType::unrecoverable);
            break;
    }

    return pelSeverity;
}
} // namespace pels
} // namespace openpower
