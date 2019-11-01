#include "user_header.hpp"

#include "pel_types.hpp"
#include "severity.hpp"

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;

void UserHeader::unflatten(Stream& stream)
{
    stream >> _header >> _eventSubsystem >> _eventScope >> _eventSeverity >>
        _eventType >> _reserved4Byte1 >> _problemDomain >> _problemVector >>
        _actionFlags >> _reserved4Byte2;
}

void UserHeader::flatten(Stream& stream)
{
    stream << _header << _eventSubsystem << _eventScope << _eventSeverity
           << _eventType << _reserved4Byte1 << _problemDomain << _problemVector
           << _actionFlags << _reserved4Byte2;
}

UserHeader::UserHeader(const message::Entry& entry,
                       phosphor::logging::Entry::Level severity)
{
    _header.id = static_cast<uint16_t>(SectionID::userHeader);
    _header.size = UserHeader::flattenedSize();
    _header.version = userHeaderVersion;
    _header.subType = 0;
    _header.componentID = static_cast<uint16_t>(ComponentID::phosphorLogging);

    _eventSubsystem = entry.subsystem;

    _eventScope = entry.eventScope.value_or(
        static_cast<uint8_t>(EventScope::entirePlatform));

    // Get the severity from the registry if it's there, otherwise get it
    // from the OpenBMC event log severity value.
    _eventSeverity =
        entry.severity.value_or(convertOBMCSeverityToPEL(severity));

    // TODO: ibm-dev/dev/#1144 Handle manufacturing sev & action flags

    if (entry.eventType)
    {
        _eventType = *entry.eventType;
    }
    else
    {
        // There are different default event types for info errors
        // vs non info ones.
        auto sevType = static_cast<SeverityType>(_eventSeverity & 0xF0);

        _eventType = (sevType == SeverityType::nonError)
                         ? static_cast<uint8_t>(EventType::miscInformational)
                         : static_cast<uint8_t>(EventType::notApplicable);
    }

    _reserved4Byte1 = 0;

    // No uses for problem domain or vector
    _problemDomain = 0;
    _problemVector = 0;

    // These will be cleaned up later in pel_rules::check()
    _actionFlags = entry.actionFlags.value_or(0);

    _reserved4Byte2 = 0;

    _valid = true;
}

UserHeader::UserHeader(Stream& pel)
{
    try
    {
        unflatten(pel);
        validate();
    }
    catch (const std::exception& e)
    {
        log<level::ERR>("Cannot unflatten user header",
                        entry("ERROR=%s", e.what()));
        _valid = false;
    }
}

void UserHeader::validate()
{
    bool failed = false;
    if (header().id != static_cast<uint16_t>(SectionID::userHeader))
    {
        log<level::ERR>("Invalid user header section ID",
                        entry("ID=0x%X", header().id));
        failed = true;
    }

    if (header().version != userHeaderVersion)
    {
        log<level::ERR>("Invalid user header version",
                        entry("VERSION=0x%X", header().version));
        failed = true;
    }

    _valid = (failed) ? false : true;
}

} // namespace pels
} // namespace openpower
