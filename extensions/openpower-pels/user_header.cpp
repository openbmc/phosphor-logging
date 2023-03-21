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

#include "json_utils.hpp"
#include "pel_types.hpp"
#include "pel_values.hpp"
#include "severity.hpp"

#include <fmt/format.h>

#include <phosphor-logging/log.hpp>

#include <iostream>

namespace openpower
{
namespace pels
{

namespace pv = openpower::pels::pel_values;
using namespace phosphor::logging;

void UserHeader::unflatten(Stream& stream)
{
    stream >> _header >> _eventSubsystem >> _eventScope >> _eventSeverity >>
        _eventType >> _reserved4Byte1 >> _problemDomain >> _problemVector >>
        _actionFlags >> _states;
}

void UserHeader::flatten(Stream& stream) const
{
    stream << _header << _eventSubsystem << _eventScope << _eventSeverity
           << _eventType << _reserved4Byte1 << _problemDomain << _problemVector
           << _actionFlags << _states;
}

UserHeader::UserHeader(const message::Entry& entry,
                       phosphor::logging::Entry::Level severity,
                       const AdditionalData& additionalData,
                       const DataInterfaceBase& dataIface)
{
    _header.id = static_cast<uint16_t>(SectionID::userHeader);
    _header.size = UserHeader::flattenedSize();
    _header.version = userHeaderVersion;
    _header.subType = 0;
    _header.componentID = static_cast<uint16_t>(ComponentID::phosphorLogging);

    std::optional<uint8_t> subsys;

    // Check for additional data - PEL_SUBSYSTEM
    auto ss = additionalData.getValue("PEL_SUBSYSTEM");
    if (ss)
    {
        auto eventSubsystem = std::stoul(*ss, NULL, 16);
        std::string subsystemString = pv::getValue(eventSubsystem,
                                                   pel_values::subsystemValues);
        if (subsystemString == "invalid")
        {
            log<level::WARNING>(
                fmt::format(
                    "UH: Invalid SubSystem value in PEL_SUBSYSTEM: {:#X}",
                    eventSubsystem)
                    .c_str());
        }
        else
        {
            subsys = eventSubsystem;
        }
    }
    else
    {
        subsys = entry.subsystem;
    }

    if (subsys)
    {
        _eventSubsystem = *subsys;
    }
    else
    {
        // Gotta use something, how about 'others'.
        log<level::WARNING>(
            "No PEL subystem value supplied for error, using 'others'");
        _eventSubsystem = 0x70;
    }

    _eventScope = entry.eventScope.value_or(
        static_cast<uint8_t>(EventScope::entirePlatform));

    {
        bool mfgSevStatus = false;
        bool mfgActionFlagStatus = false;

        // Get the mfg severity & action flags
        if (entry.mfgSeverity || entry.mfgActionFlags)
        {
            std::optional<uint8_t> sev = std::nullopt;
            uint16_t val = 0;

            if (entry.mfgSeverity)
            {
                // Find the mf severity possibly dependent on the system type.
                sev = getSeverity(entry.mfgSeverity.value(), dataIface);
            }

            if (entry.mfgActionFlags)
            {
                // Find the mfg action flags
                val = entry.mfgActionFlags.value();
            }

            if (sev || val)
            {
                bool mfgProp = dataIface.getQuiesceOnError();
                if (mfgProp)
                {
                    if (sev)
                    {
                        _eventSeverity = *sev;
                        mfgSevStatus = true;
                    }

                    if (val)
                    {
                        _actionFlags = val;
                        mfgActionFlagStatus = true;
                    }
                }
            }
        }

        if (!mfgSevStatus)
        {
            // Get the severity from the registry if it's there, otherwise get
            // it from the OpenBMC event log severity value.
            if (!entry.severity)
            {
                _eventSeverity = convertOBMCSeverityToPEL(severity);
            }
            else
            {
                // Find the severity possibly dependent on the system type.
                auto sev = getSeverity(entry.severity.value(), dataIface);
                if (sev)
                {
                    _eventSeverity = *sev;
                }
                else
                {
                    // Either someone  screwed up the message registry
                    // or getSystemNames failed.
                    log<level::ERR>(
                        "Failed finding the severity in the message registry",
                        phosphor::logging::entry("ERROR=%s",
                                                 entry.name.c_str()));

                    // Have to choose something, just use informational.
                    _eventSeverity = 0;
                }
            }
        }

        // Convert Critical error (0x50) to Critical Error-System Termination
        // (0x51), if the AdditionalData is set to SYSTEM_TERM
        auto sevLevel = additionalData.getValue("SEVERITY_DETAIL");
        if ((_eventSeverity & 0xF0) == 0x50)
        {
            if (sevLevel.value_or("") == "SYSTEM_TERM")
            {
                // Change to Critical Error, System Termination
                _eventSeverity = 0x51;
            }
        }

        if (entry.eventType)
        {
            _eventType = *entry.eventType;
        }
        else
        {
            // There are different default event types for info errors
            // vs non info ones.
            auto sevType = static_cast<SeverityType>(_eventSeverity & 0xF0);
            _eventType =
                (sevType == SeverityType::nonError)
                    ? static_cast<uint8_t>(EventType::miscInformational)
                    : static_cast<uint8_t>(EventType::notApplicable);
        }

        _reserved4Byte1 = 0;

        // No uses for problem domain or vector
        _problemDomain = 0;
        _problemVector = 0;

        // These will be set in pel_rules::check() if they're still
        // at the default value.
        if (!mfgActionFlagStatus)
        {
            _actionFlags = entry.actionFlags.value_or(actionFlagsDefault);
        }

        _states = 0;

        _valid = true;
    }
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
        log<level::ERR>(
            fmt::format("Cannot unflatten user header: {}", e.what()).c_str());
        _valid = false;
    }
}

void UserHeader::validate()
{
    bool failed = false;
    if (header().id != static_cast<uint16_t>(SectionID::userHeader))
    {
        log<level::ERR>(
            fmt::format("Invalid user header section ID: {0:#x}", header().id)
                .c_str());
        failed = true;
    }

    if (header().version != userHeaderVersion)
    {
        log<level::ERR>(
            fmt::format("Invalid user header version: {0:#x}", header().version)
                .c_str());
        failed = true;
    }

    _valid = (failed) ? false : true;
}

std::optional<std::string> UserHeader::getJSON(uint8_t creatorID) const
{
    std::string severity;
    std::string subsystem;
    std::string eventScope;
    std::string eventType;
    std::vector<std::string> actionFlags;
    severity = pv::getValue(_eventSeverity, pel_values::severityValues);
    subsystem = pv::getValue(_eventSubsystem, pel_values::subsystemValues);
    eventScope = pv::getValue(_eventScope, pel_values::eventScopeValues);
    eventType = pv::getValue(_eventType, pel_values::eventTypeValues);
    actionFlags = pv::getValuesBitwise(_actionFlags,
                                       pel_values::actionFlagsValues);

    std::string hostState{"Invalid"};
    std::string hmcState{"Invalid"};
    auto iter = pv::transmissionStates.find(
        static_cast<TransmissionState>(hostTransmissionState()));
    if (iter != pv::transmissionStates.end())
    {
        hostState = iter->second;
    }
    auto iter1 = pv::transmissionStates.find(
        static_cast<TransmissionState>(hmcTransmissionState()));
    if (iter1 != pv::transmissionStates.end())
    {
        hmcState = iter1->second;
    }

    std::string uh;
    jsonInsert(uh, pv::sectionVer, getNumberString("%d", userHeaderVersion), 1);
    jsonInsert(uh, pv::subSection, getNumberString("%d", _header.subType), 1);
    jsonInsert(uh, "Log Committed by",
               getComponentName(_header.componentID, creatorID), 1);
    jsonInsert(uh, "Subsystem", subsystem, 1);
    jsonInsert(uh, "Event Scope", eventScope, 1);
    jsonInsert(uh, "Event Severity", severity, 1);
    jsonInsert(uh, "Event Type", eventType, 1);
    jsonInsertArray(uh, "Action Flags", actionFlags, 1);
    jsonInsert(uh, "Host Transmission", hostState, 1);
    jsonInsert(uh, "HMC Transmission", hmcState, 1);
    uh.erase(uh.size() - 2);
    return uh;
}

std::optional<uint8_t> UserHeader::getSeverity(
    const std::vector<message::RegistrySeverity>& severities,
    const DataInterfaceBase& dataIface) const
{
    const uint8_t* s = nullptr;
    std::vector<std::string> systemNames;

    // getSystemNames makes D-Bus calls, so only call it if we
    // know we'll need it because there is a system name in the sev list
    if (std::any_of(severities.begin(), severities.end(),
                    [](const auto& sev) { return !sev.system.empty(); }))
    {
        try
        {
            systemNames = dataIface.getSystemNames();
        }
        catch (const std::exception& e)
        {
            log<level::ERR>("Failed trying to look up system names on D-Bus",
                            entry("ERROR=%s", e.what()));
            return std::nullopt;
        }
    }

    // Find the severity to use for this system type, or use the default
    // entry (where no system type is specified).
    for (const auto& sev : severities)
    {
        if (std::find(systemNames.begin(), systemNames.end(), sev.system) !=
            systemNames.end())
        {
            s = &sev.severity;
            break;
        }
        else if (sev.system.empty())
        {
            s = &sev.severity;
        }
    }

    if (s)
    {
        return *s;
    }

    return std::nullopt;
}

} // namespace pels
} // namespace openpower
