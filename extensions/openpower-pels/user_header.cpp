#include "user_header.hpp"

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;

Stream& operator>>(Stream& s, UserHeader& uh)
{
    s >> uh._header >> uh._eventSubsystem >> uh._eventScope >>
        uh._eventSeverity >> uh._eventType >> uh._reserved4Byte1 >>
        uh._problemDomain >> uh._problemVector >> uh._actionFlags >>
        uh._reserved4Byte2;
    return s;
}

Stream& operator<<(Stream& s, UserHeader& uh)
{
    s << uh._header << uh._eventSubsystem << uh._eventScope << uh._eventSeverity
      << uh._eventType << uh._reserved4Byte1 << uh._problemDomain
      << uh._problemVector << uh._actionFlags << uh._reserved4Byte2;
    return s;
}

UserHeader::UserHeader(Stream& pel)
{
    try
    {
        pel >> *this;
        validate();
    }
    catch (std::exception& e)
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
        return;
    }

    if (header().version != userHeaderVersion)
    {
        log<level::ERR>("Invalid user header version",
                        entry("VERSION=0x%X", header().version));
        failed = true;
        return;
    }

    _valid = (failed) ? false : true;
}

} // namespace pels
} // namespace openpower
