#include "user_header.hpp"

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;
using json = nlohmann::json;

void UserHeader::unflatten(Stream& stream)
{
    stream >> _header >> _eventSubsystem >> _eventScope >> _eventSeverity >>
        _eventType >> _reserved4Byte1 >> _problemDomain >> _problemVector >>
        _actionFlags >> _reserved4Byte2;
}

void UserHeader::flatten(Stream& stream)
{
    stream << _header << _eventSubsystem << _eventScope << _eventSeverity
           << _eventType << _reserved4Byte1 << _problemDomain << _problemVector
           << _actionFlags << _reserved4Byte2;
}

UserHeader::UserHeader(Stream& pel)
{
    try
    {
        unflatten(pel);
        validate();
    }
    catch (const std::exception& e)
    {
        log<level::ERR>("Cannot unflatten user header",
                        entry("ERROR=%s", e.what()));
        _valid = false;
    }
}

void UserHeader::validate()
{
    bool failed = false;
    if (header().id != userHeaderSectionID)
    {
        log<level::ERR>("Invalid user header section ID",
                        entry("ID=0x%X", header().id));
        failed = true;
        return;
    }

    if (header().version != userHeaderVersion)
    {
        log<level::ERR>("Invalid user header version",
                        entry("VERSION=0x%X", header().version));
        failed = true;
        return;
    }

    _valid = (failed) ? false : true;
}

/**
 * @brief Returns the section header in json format with values in hex.
 *
 * @return  char * - json string with values in hex
 */
const char* UserHeader::toJson()
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

} // namespace pels
} // namespace openpower
