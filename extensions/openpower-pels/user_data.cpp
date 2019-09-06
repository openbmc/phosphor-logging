#include "user_data.hpp"

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;

Stream& operator>>(Stream& s, vector<uint8_t> v)
{
    for (int i = 0; i < v.size(); ++i)
    {
        s >> v[i];
    }
    return s;
}
void UserData::unflatten(Stream& stream)
{
    stream >> _header >> _data;
}

void UserData::flatten(Stream& stream)
{
    stream << _header << _data;
}

UserData::UserData(Stream& pel)
{
    try
    {
        unflatten(pel);
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
