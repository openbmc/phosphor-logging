// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "user_data.hpp"

#include "pel_types.hpp"
#ifdef PELTOOL
#include "user_data_json.hpp"
#endif

#include <phosphor-logging/lg2.hpp>

namespace openpower
{
namespace pels
{

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

void UserData::flatten(Stream& stream) const
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
        lg2::error("Cannot unflatten user data: {EXCEPTION}", "EXCEPTION", e);
        _valid = false;
    }
}

UserData::UserData(uint16_t componentID, uint8_t subType, uint8_t version,
                   const std::vector<uint8_t>& data)
{
    _header.id = static_cast<uint16_t>(SectionID::userData);
    _header.size = Section::headerSize() + data.size();
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
        lg2::error("Invalid UserData section ID: {HEADER_ID}", "HEADER_ID",
                   lg2::hex, header().id);
        _valid = false;
    }
    else
    {
        _valid = true;
    }
}

std::optional<std::string> UserData::getJSON(
    uint8_t creatorID [[maybe_unused]],
    const std::vector<std::string>& plugins [[maybe_unused]]) const
{
#ifdef PELTOOL
    return user_data::getJSON(_header.componentID, _header.subType,
                              _header.version, _data, creatorID, plugins);
#endif
    return std::nullopt;
}

bool UserData::shrink(size_t newSize)
{
    // minimum size is 4 bytes plus the 8B header
    if ((newSize < flattenedSize()) && (newSize >= (Section::headerSize() + 4)))
    {
        auto dataSize = newSize - Section::headerSize();

        // Ensure it's 4B aligned
        _data.resize((dataSize / 4) * 4);
        _header.size = Section::headerSize() + _data.size();
        return true;
    }

    return false;
}

} // namespace pels
} // namespace openpower
