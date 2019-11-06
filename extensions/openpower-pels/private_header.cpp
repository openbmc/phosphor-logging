/**
 * Copyright © 2019 IBM Corporation
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
#include "private_header.hpp"

#include "log_id.hpp"
#include "pel_types.hpp"

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;

PrivateHeader::PrivateHeader(uint16_t componentID, uint32_t obmcLogID,
                             uint64_t timestamp)
{
    _header.id = static_cast<uint16_t>(SectionID::privateHeader);
    _header.size = PrivateHeader::flattenedSize();
    _header.version = privateHeaderVersion;
    _header.subType = 0;
    _header.componentID = componentID;

    _createTimestamp = getBCDTime(timestamp);

    auto now = std::chrono::system_clock::now();
    _commitTimestamp = getBCDTime(now);

    _creatorID = static_cast<uint8_t>(CreatorID::openBMC);

    // Add support for reminder and telemetry log types here if
    // ever necessary.
    _logType = 0;

    _reservedByte = 0;

    // the final section count will be updated later
    _sectionCount = 1;

    _obmcLogID = obmcLogID;

    _id = generatePELID();

    _plid = _id;

    // Leave _creatorVersion at 0

    _valid = true;
}

PrivateHeader::PrivateHeader(Stream& pel) :
    _creatorID(0), _logType(0), _reservedByte(0), _sectionCount(0),
    _obmcLogID(0), _plid(0), _id(0)
{
    try
    {
        unflatten(pel);
        validate();
    }
    catch (const std::exception& e)
    {
        log<level::ERR>("Cannot unflatten private header",
                        entry("ERROR=%s", e.what()));
        _valid = false;
    }
}

void PrivateHeader::validate()
{
    bool failed = false;

    if (header().id != static_cast<uint16_t>(SectionID::privateHeader))
    {
        log<level::ERR>("Invalid private header section ID",
                        entry("ID=0x%X", header().id));
        failed = true;
    }

    if (header().version != privateHeaderVersion)
    {
        log<level::ERR>("Invalid private header version",
                        entry("VERSION=0x%X", header().version));
        failed = true;
    }

    if (_sectionCount < minSectionCount)
    {
        log<level::ERR>("Invalid section count in private header",
                        entry("SECTION_COUNT=0x%X", _sectionCount));
        failed = true;
    }

    _valid = (failed) ? false : true;
}

void PrivateHeader::unflatten(Stream& stream)
{
    stream >> _header >> _createTimestamp >> _commitTimestamp >> _creatorID >>
        _logType >> _reservedByte >> _sectionCount >> _obmcLogID >>
        _creatorVersion >> _plid >> _id;
}

void PrivateHeader::flatten(Stream& stream) const
{
    stream << _header << _createTimestamp << _commitTimestamp << _creatorID
           << _logType << _reservedByte << _sectionCount << _obmcLogID
           << _creatorVersion << _plid << _id;
}

Stream& operator>>(Stream& s, CreatorVersion& cv)
{
    for (size_t i = 0; i < sizeof(CreatorVersion); i++)
    {
        s >> cv.version[i];
    }
    return s;
}

Stream& operator<<(Stream& s, const CreatorVersion& cv)
{
    for (size_t i = 0; i < sizeof(CreatorVersion); i++)
    {
        s << cv.version[i];
    }
    return s;
}

} // namespace pels
} // namespace openpower
