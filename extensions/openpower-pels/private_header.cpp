#include "private_header.hpp"

#include "pel_types.hpp"

#include <fifo_map/src/fifo_map.hpp>
#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;
using namespace nlohmann;
using json = nlohmann::json;

PrivateHeader::PrivateHeader(Stream& pel)
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

/**
 * @brief Returns the section header in json format.
 *
 * @return fifoMap
 */
fifoMap PrivateHeader::toJson()
{
    fifoMap ph = {{"Section Version", 1},
                  {"Sub-section type", 0},
                  {"Created by", "occc"},
                  {"Created at", "09/11/2019 10:58:25"},
                  {"Committed at", "09/11/2019 10:58:26"},
                  {"Creator Subsystem", "FipS Error Logger"},
                  {"CSSVER"},
                  {"Platform Log Id", "0x500DEA2C"},
                  {"Entry Id", "0x500DEA2C"},
                  {"Total Log Size", 5212}};

    return ph;
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

void PrivateHeader::flatten(Stream& stream)
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

Stream& operator<<(Stream& s, CreatorVersion& cv)
{
    for (size_t i = 0; i < sizeof(CreatorVersion); i++)
    {
        s << cv.version[i];
    }
    return s;
}

} // namespace pels
} // namespace openpower
