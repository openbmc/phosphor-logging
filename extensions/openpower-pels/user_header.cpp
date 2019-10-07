#include "user_header.hpp"

#include "pel_types.hpp"

#include <fifo_map/src/fifo_map.hpp>
#include <iostream>
#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;
using namespace nlohmann;
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

/**
 * @brief Validates the section contents
 *
 * Updates _valid (in Section) with the results.
 */
void UserHeader::validate()
{
    bool failed = false;
    if (header().id != static_cast<uint16_t>(SectionID::userHeader))
    {
        log<level::ERR>("Invalid user header section ID",
                        entry("ID=0x%X", header().id));
        failed = true;
    }

    if (header().version != userHeaderVersion)
    {
        log<level::ERR>("Invalid user header version",
                        entry("VERSION=0x%X", header().version));
        failed = true;
    }

    _valid = (failed) ? false : true;
}

/**
 * @brief Returns the section header in json format.
 *
 * @return fifoMap
 */
fifoMap UserHeader::toJson()
{
    auto sevCode = pel_values::findByValue(this->_eventSeverity,
                                           pel_values::severityValues);
    const char* severity = std::get<pel_values::registryNamePos>(*sevCode);
    // TODO FOR ALL FIELDS.
    fifoMap uh = {{"Section Version", 1},
                  {"Sub-section type", 0},
                  {"Log Committed by", "occc"},
                  {"Subsystem", "Miscellaneous"},
                  {"Event Scope", "Single Platform"},
                  {"Event Severity", severity},
                  {"Event Type", "Miscellaneous, Informational Only"},
                  {"Return Code", "0x00002A01"}

    };

    return uh;
}

} // namespace pels
} // namespace openpower
