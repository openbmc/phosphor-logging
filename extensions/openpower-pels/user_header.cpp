#include "user_header.hpp"

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
    if (header().id != userHeaderSectionID)
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
