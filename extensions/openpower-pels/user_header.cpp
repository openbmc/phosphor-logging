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

/**
 * @brief Validates the section contents
 *
 * Updates _valid (in Section) with the results.
 */
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
 * @brief Returns the section header in json format.
 *
 * @return  char *
 */
const char* UserHeader::toJson()
{
    auto tmp = pel_values::findByValue(this->_eventSeverity,
                                       pel_values::severityValues);
    const char* severity = std::get<pel_values::registryNamePos>(*tmp);
    // TODO FOR ALL OTHER FIELDS
    auto uh = R"(
    {
        "Section Version": "",
        "Sub-section type": "",
        "Log Committed by": "",
        "Subsystem": "",
        "Event Scope": "",
        "Event Severity":""
    }
    )"_json;
    // any Hex dumps would be added here too
    uh["Event Severity"] = severity;
    std::stringstream ss;
    ss << "TODO" << uh.dump();
    const char* c = &*ss.str().begin();
    return c;
}

} // namespace pels
} // namespace openpower
