#include "private_header.hpp"

#include "pel_types.hpp"

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;

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
 * @return  char *
 */
const char* PrivateHeader::toJson()
{
    std::stringstream ss;
    ss << "TODO";
    // ss << this->id;
    // ss << "0x" << std::uppercase << std::setfill('0') << std::setw(4)
    // << std::hex << id;/* << "0x" << std::uppercase << std::setfill('0')
    /*<< std::setw(4) << std::hex << size << "0x" << std::uppercase
    << std::setfill('0') << std::setw(4) << std::hex << version << "0x"
    << std::uppercase << std::setfill('0') << std::setw(4) << std::hex
    << subType << "0x" << std::uppercase << std::setfill('0')
    << std::setw(4) << std::hex << componentID;*/
    const char* c = &*ss.str().begin();
    return c;
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
