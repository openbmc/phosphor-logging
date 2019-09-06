#include "user_data.hpp"

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;

Stream& operator>>(Stream& s, UserData& ud)
{
    s >> ud._header >> ud._data;
    return s;
}

Stream& operator<<(Stream& s, UserData& ud)
{
    s << ud._header << ud._data;
    return s;
}

UserData::UserData(Stream& pel)
{
    try
    {
        pel >> *this;
        validate();
    }
    catch (const std::exception& e)
    {
        log<level::ERR>("Cannot unflatten user data",
                        entry("ERROR=%s", e.what()));
        _valid = false;
    }
}

void UserData::validate()
{
    bool failed = false;
    if (header().id != userDataSectionID)
    {
        log<level::ERR>("Invalid user data section ID",
                        entry("ID=0x%X", header().id));
        failed = true;
        return;
    }

    if (header().version != userDataVersion)
    {
        log<level::ERR>("Invalid user data version",
                        entry("VERSION=0x%X", header().version));
        failed = true;
        return;
    }

    _valid = (failed) ? false : true;
}

} // namespace pels
} // namespace openpower
