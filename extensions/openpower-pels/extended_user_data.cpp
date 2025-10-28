// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2020 IBM Corporation

#include "extended_user_data.hpp"

#include "pel_types.hpp"
#ifdef PELTOOL
#include "user_data_json.hpp"
#endif
#include <phosphor-logging/lg2.hpp>

#include <format>

namespace openpower::pels
{

void ExtendedUserData::unflatten(Stream& stream)
{
    stream >> _header;

    if (_header.size <= SectionHeader::flattenedSize() + 4)
    {
        throw std::out_of_range(
            std::format("ExtendedUserData::unflatten: SectionHeader::size {} "
                        "too small",
                        _header.size));
    }

    size_t dataLength = _header.size - 4 - SectionHeader::flattenedSize();
    _data.resize(dataLength);

    stream >> _creatorID >> _reserved1B >> _reserved2B >> _data;
}

void ExtendedUserData::flatten(Stream& stream) const
{
    stream << _header << _creatorID << _reserved1B << _reserved2B << _data;
}

ExtendedUserData::ExtendedUserData(Stream& pel)
{
    try
    {
        unflatten(pel);
        validate();
    }
    catch (const std::exception& e)
    {
        lg2::error("Cannot unflatten ExtendedUserData section: {EXCEPTION}",
                   "EXCEPTION", e);
        _valid = false;
    }
}

ExtendedUserData::ExtendedUserData(uint16_t componentID, uint8_t subType,
                                   uint8_t version, uint8_t creatorID,
                                   const std::vector<uint8_t>& data)
{
    _header.id = static_cast<uint16_t>(SectionID::extUserData);
    _header.version = version;
    _header.subType = subType;
    _header.componentID = componentID;

    _creatorID = creatorID;
    _reserved1B = 0;
    _reserved2B = 0;
    _data = data;
    _header.size = flattenedSize();
    _valid = true;
}

void ExtendedUserData::validate()
{
    if (header().id != static_cast<uint16_t>(SectionID::extUserData))
    {
        lg2::error("Invalid ExtendedUserData section ID: {HEADER_ID}",
                   "HEADER_ID", lg2::hex, header().id);
        _valid = false;
    }
    else
    {
        _valid = true;
    }
}

std::optional<std::string> ExtendedUserData::getJSON(
    uint8_t /*creatorID*/,
    const std::vector<std::string>& plugins [[maybe_unused]]) const
{
    // Use the creator ID value from the section.
#ifdef PELTOOL
    return user_data::getJSON(_header.componentID, _header.subType,
                              _header.version, _data, _creatorID, plugins);
#endif
    return std::nullopt;
}

bool ExtendedUserData::shrink(size_t newSize)
{
    // minimum size is 8B header + 4B of fields + 4B of data
    if ((newSize < flattenedSize()) && (newSize >= (Section::headerSize() + 8)))
    {
        auto dataSize = newSize - Section::headerSize() - 4;

        // Ensure it's 4B aligned
        _data.resize((dataSize / 4) * 4);
        _header.size = flattenedSize();
        return true;
    }

    return false;
}

} // namespace openpower::pels
