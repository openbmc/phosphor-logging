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
#include "user_header.hpp"

#include "pel_types.hpp"
#include "pel_values.hpp"
#include "severity.hpp"

#include <iostream>
#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;

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

UserHeader::UserHeader(const message::Entry& entry,
                       phosphor::logging::Entry::Level severity)
{
    _header.id = static_cast<uint16_t>(SectionID::userHeader);
    _header.size = UserHeader::flattenedSize();
    _header.version = userHeaderVersion;
    _header.subType = 0;
    _header.componentID = static_cast<uint16_t>(ComponentID::phosphorLogging);

    _eventSubsystem = entry.subsystem;

    _eventScope = entry.eventScope.value_or(
        static_cast<uint8_t>(EventScope::entirePlatform));

    // Get the severity from the registry if it's there, otherwise get it
    // from the OpenBMC event log severity value.
    _eventSeverity =
        entry.severity.value_or(convertOBMCSeverityToPEL(severity));

    // TODO: ibm-dev/dev/#1144 Handle manufacturing sev & action flags

    _eventType = entry.eventType.value_or(
        static_cast<uint8_t>(EventType::notApplicable));

    _reserved4Byte1 = 0;

    // No uses for problem domain or vector
    _problemDomain = 0;
    _problemVector = 0;

    _actionFlags = entry.actionFlags.value_or(0);

    _reserved4Byte2 = 0;

    _valid = true;
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

std::string UserHeader::getValue(const uint8_t field,
                                 const PELValues values) const
{

    auto tmp = pel_values::findByValue(field, values);
    if (tmp != values.end())
    {
        return std::get<pel_values::registryNamePos>(*tmp);
    }
    else
    {
        return "invalid";
    }
}
std::optional<std::string> UserHeader::getJSON() const
{
    std::string severity;
    std::string subsystem;
    std::string eventScope;
    std::string eventType;
    severity = getValue(_eventSeverity, pel_values::severityValues);
    subsystem = getValue(_eventSubsystem, pel_values::subsystemValues);
    eventScope = getValue(_eventScope, pel_values::eventScopeValues);
    eventType = getValue(_eventType, pel_values::eventTypeValues);
    char tmpUhVal[8];
    sprintf(tmpUhVal, "%d", userHeaderVersion);
    std::string uhVerStr(tmpUhVal);
    sprintf(tmpUhVal, "0x%X", _header.componentID);
    std::string uhCbStr(tmpUhVal);
    sprintf(tmpUhVal, "%d", _header.subType);
    std::string uhStStr(tmpUhVal);

    std::string uh = "{\"Section Version\": \"" + uhVerStr +
                     "\"}, \n {\"Sub-section type\": \"" + uhStStr +
                     "\"}, \n "
                     "{\"Log Committed by\": \"" +
                     uhCbStr + "\"}, \n {\"Subsystem\": \"" + subsystem +
                     "\"},\n "
                     "{\"Event Scope\": \"" +
                     eventScope + "\"}, \n {\"Event Severity\":\"" + severity +
                     "\"},\n {\"Event Type\": \"" + eventType + "\"}";
    return uh;
}
} // namespace pels
} // namespace openpower
