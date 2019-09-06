#include "user_data_header.hpp"

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;

Stream& operator>>(Stream& s, UserDataHeader& ud)
{
    s >> ud._header >> ud._eventSubsystem >> ud._eventScope >>
        ud._eventSeverity >> ud._eventType >> ud._reserved4Byte1 >>
        ud._problemDomain >> ud._problemVector >> ud._actionFlags >>
        ud._reserved4Byte2;
    return s;
}

Stream& operator<<(Stream& s, UserDataHeader& ud)
{
    s << ud._header << ud._eventSubsystem << ud._eventScope << ud._eventSeverity
      << ud._eventType << ud._reserved4Byte1 << ud._problemDomain
      << ud._problemVector << ud._actionFlags << ud._reserved4Byte2;
    return s;
}

UserDataHeader::UserDataHeader(Stream& pel)
{
    try
    {
        pel >> *this;
        validate();
    }
    catch (const std::exception& e)
    {
        log<level::ERR>("Cannot unflatten user data header",
                        entry("ERROR=%s", e.what()));
        _valid = false;
    }
}

void UserDataHeader::validate()
{
    bool failed = false;
    if (header().id != userDataHeaderSectionID)
    {
        log<level::ERR>("Invalid user data header section ID",
                        entry("ID=0x%X", header().id));
        failed = true;
        return;
    }

    if (header().version != userDataHeaderVersion)
    {
        log<level::ERR>("Invalid user data header version",
                        entry("VERSION=0x%X", header().version));
        failed = true;
        return;
    }

    _valid = (failed) ? false : true;
}

} // namespace pels
} // namespace openpower
