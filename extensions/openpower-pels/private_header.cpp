// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "private_header.hpp"

#include "json_utils.hpp"
#include "log_id.hpp"
#include "pel_types.hpp"
#include "pel_values.hpp"

#include <phosphor-logging/lg2.hpp>

namespace openpower
{
namespace pels
{

namespace pv = openpower::pels::pel_values;

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
        lg2::error("Cannot unflatten private header: {EXCEPTION}", "EXCEPTION",
                   e);
        _valid = false;
    }
}
std::optional<std::string> PrivateHeader::getJSON(uint8_t creatorID) const
{
    char tmpPhVal[50];
    sprintf(tmpPhVal, "%02X/%02X/%02X%02X %02X:%02X:%02X",
            _createTimestamp.month, _createTimestamp.day,
            _createTimestamp.yearMSB, _createTimestamp.yearLSB,
            _createTimestamp.hour, _createTimestamp.minutes,
            _createTimestamp.seconds);
    std::string phCreateTStr(tmpPhVal);
    sprintf(tmpPhVal, "%02X/%02X/%02X%02X %02X:%02X:%02X",
            _commitTimestamp.month, _commitTimestamp.day,
            _commitTimestamp.yearMSB, _commitTimestamp.yearLSB,
            _commitTimestamp.hour, _commitTimestamp.minutes,
            _commitTimestamp.seconds);
    std::string phCommitTStr(tmpPhVal);
    std::string creator = getNumberString("%c", _creatorID);
    creator = pv::creatorIDs.count(creator) ? pv::creatorIDs.at(creator)
                                            : "Unknown CreatorID";
    std::string phCreatorVersionStr =
        std::string(reinterpret_cast<const char*>(_creatorVersion.version));

    std::string ph;
    jsonInsert(ph, pv::sectionVer, getNumberString("%d", privateHeaderVersion),
               1);
    jsonInsert(ph, pv::subSection, getNumberString("%d", _header.subType), 1);
    jsonInsert(ph, pv::createdBy,
               getComponentName(_header.componentID, creatorID), 1);
    jsonInsert(ph, "Created at", phCreateTStr, 1);
    jsonInsert(ph, "Committed at", phCommitTStr, 1);
    jsonInsert(ph, "Creator Subsystem", creator, 1);
    jsonInsert(ph, "CSSVER", phCreatorVersionStr, 1);
    jsonInsert(ph, "Platform Log Id", getNumberString("0x%X", _plid), 1);
    jsonInsert(ph, "Entry Id", getNumberString("0x%X", _id), 1);
    jsonInsert(ph, "BMC Event Log Id", std::to_string(_obmcLogID), 1);
    ph.erase(ph.size() - 2);

    return ph;
}
void PrivateHeader::validate()
{
    bool failed = false;

    if (header().id != static_cast<uint16_t>(SectionID::privateHeader))
    {
        lg2::error("Invalid private header section ID: {HEADER_ID}",
                   "HEADER_ID", lg2::hex, header().id);
        failed = true;
    }

    if (header().version != privateHeaderVersion)
    {
        lg2::error("Invalid private header version: {HEADER_VERSION}",
                   "HEADER_VERSION", lg2::hex, header().version);
        failed = true;
    }

    if (_sectionCount < minSectionCount)
    {
        lg2::error("Invalid section count in private header: {SECTION_COUNT}",
                   "SECTION_COUNT", lg2::hex, _sectionCount);
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
