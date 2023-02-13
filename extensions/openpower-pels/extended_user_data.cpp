/**
 * Copyright © 2020 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "extended_user_data.hpp"

#include "pel_types.hpp"
#ifdef PELTOOL
#include "user_data_json.hpp"
#endif
#include <fmt/format.h>

#include <phosphor-logging/log.hpp>

namespace openpower::pels
{

using namespace phosphor::logging;

void ExtendedUserData::unflatten(Stream& stream)
{
    stream >> _header;

    if (_header.size <= SectionHeader::flattenedSize() + 4)
    {
        throw std::out_of_range(
            fmt::format("ExtendedUserData::unflatten: SectionHeader::size {} "
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
        log<level::ERR>(
            fmt::format("Cannot unflatten ExtendedUserData section: {}",
                        e.what())
                .c_str());
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
        log<level::ERR>(
            fmt::format("Invalid ExtendedUserData section ID: {0:#x}",
                        header().id)
                .c_str());
        _valid = false;
    }
    else
    {
        _valid = true;
    }
}

std::optional<std::string>
    ExtendedUserData::getJSON(uint8_t /*creatorID*/,
                              const std::vector<std::string>& plugins
                              [[maybe_unused]]) const
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
    if ((newSize < flattenedSize()) &&
        (newSize >= (Section::flattenedSize() + 8)))
    {
        auto dataSize = newSize - Section::flattenedSize() - 4;

        // Ensure it's 4B aligned
        _data.resize((dataSize / 4) * 4);
        _header.size = flattenedSize();
        return true;
    }

    return false;
}

} // namespace openpower::pels
