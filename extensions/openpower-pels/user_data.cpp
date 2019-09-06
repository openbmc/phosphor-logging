#include "user_data.hpp"

#include "pel_types.hpp"

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;

void UserData::unflatten(Stream& stream)
{
    stream >> _header;

    if (_header.size <= SectionHeader::flattenedSize())
    {
        throw std::out_of_range(
            "UserData::unflatten: SectionHeader::size field too small");
    }

    size_t dataLength = _header.size - SectionHeader::flattenedSize();
    _data.resize(dataLength);

    stream >> _data;
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

UserData::UserData(uint16_t componentID, uint8_t subType, uint8_t version,
                   const std::vector<uint8_t>& data)
{
    _header.id = static_cast<uint16_t>(SectionID::userData);
    _header.size = Section::flattenedSize() + data.size();
    _header.version = version;
    _header.subType = subType;
    _header.componentID = componentID;

    _data = data;

    _valid = true;
}

void UserData::validate()
{
    if (header().id != static_cast<uint16_t>(SectionID::userData))
    {
        log<level::ERR>("Invalid user data section ID",
                        entry("ID=0x%X", header().id));
        _valid = false;
    }
    else
    {
        _valid = true;
    }
}

} // namespace pels
} // namespace openpower
