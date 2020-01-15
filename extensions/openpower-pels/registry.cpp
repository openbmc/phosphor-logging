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
#include "registry.hpp"

#include "pel_types.hpp"
#include "pel_values.hpp"

#include <fstream>
#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{
namespace message
{

namespace pv = pel_values;
namespace fs = std::filesystem;
using namespace phosphor::logging;

constexpr auto debugFilePath = "/etc/phosphor-logging/";

namespace helper
{

uint8_t getSubsystem(const std::string& subsystemName)
{
    // Get the actual value to use in the PEL for the string name
    auto ss = pv::findByName(subsystemName, pv::subsystemValues);
    if (ss == pv::subsystemValues.end())
    {
        // Schema validation should be catching this.
        log<level::ERR>("Invalid subsystem name used in message registry",
                        entry("SUBSYSTEM=%s", subsystemName.c_str()));

        throw std::runtime_error("Invalid subsystem used in message registry");
    }

    return std::get<pv::fieldValuePos>(*ss);
}

uint8_t getSeverity(const std::string& severityName)
{
    auto s = pv::findByName(severityName, pv::severityValues);
    if (s == pv::severityValues.end())
    {
        // Schema validation should be catching this.
        log<level::ERR>("Invalid severity name used in message registry",
                        entry("SEVERITY=%s", severityName.c_str()));

        throw std::runtime_error("Invalid severity used in message registry");
    }

    return std::get<pv::fieldValuePos>(*s);
}

uint16_t getActionFlags(const std::vector<std::string>& flags)
{
    uint16_t actionFlags = 0;

    // Make the bitmask based on the array of flag names
    for (const auto& flag : flags)
    {
        auto s = pv::findByName(flag, pv::actionFlagsValues);
        if (s == pv::actionFlagsValues.end())
        {
            // Schema validation should be catching this.
            log<level::ERR>("Invalid action flag name used in message registry",
                            entry("FLAG=%s", flag.c_str()));

            throw std::runtime_error(
                "Invalid action flag used in message registry");
        }

        actionFlags |= std::get<pv::fieldValuePos>(*s);
    }

    return actionFlags;
}

uint8_t getEventType(const std::string& eventTypeName)
{
    auto t = pv::findByName(eventTypeName, pv::eventTypeValues);
    if (t == pv::eventTypeValues.end())
    {
        log<level::ERR>("Invalid event type used in message registry",
                        entry("EVENT_TYPE=%s", eventTypeName.c_str()));

        throw std::runtime_error("Invalid event type used in message registry");
    }
    return std::get<pv::fieldValuePos>(*t);
}

uint8_t getEventScope(const std::string& eventScopeName)
{
    auto s = pv::findByName(eventScopeName, pv::eventScopeValues);
    if (s == pv::eventScopeValues.end())
    {
        log<level::ERR>("Invalid event scope used in registry",
                        entry("EVENT_SCOPE=%s", eventScopeName.c_str()));

        throw std::runtime_error(
            "Invalid event scope used in message registry");
    }
    return std::get<pv::fieldValuePos>(*s);
}

uint16_t getSRCReasonCode(const nlohmann::json& src, const std::string& name)
{
    std::string rc = src["ReasonCode"];
    uint16_t reasonCode = strtoul(rc.c_str(), nullptr, 16);
    if (reasonCode == 0)
    {
        log<phosphor::logging::level::ERR>(
            "Invalid reason code in message registry",
            entry("ERROR_NAME=%s", name.c_str()),
            entry("REASON_CODE=%s", rc.c_str()));

        throw std::runtime_error("Invalid reason code in message registry");
    }
    return reasonCode;
}

uint8_t getSRCType(const nlohmann::json& src, const std::string& name)
{
    // Looks like: "22"
    std::string srcType = src["Type"];
    size_t type = strtoul(srcType.c_str(), nullptr, 16);
    if ((type == 0) || (srcType.size() != 2)) // 1 hex byte
    {
        log<phosphor::logging::level::ERR>(
            "Invalid SRC Type in message registry",
            entry("ERROR_NAME=%s", name.c_str()),
            entry("SRC_TYPE=%s", srcType.c_str()));

        throw std::runtime_error("Invalid SRC Type in message registry");
    }

    return type;
}

std::optional<std::map<SRC::WordNum, SRC::AdditionalDataField>>
    getSRCHexwordFields(const nlohmann::json& src, const std::string& name)
{
    std::map<SRC::WordNum, SRC::AdditionalDataField> hexwordFields;

    // Build the map of which AdditionalData fields to use for which SRC words

    // Like:
    // {
    //   "8":
    //   {
    //     "AdditionalDataPropSource": "TEST"
    //   }
    //
    // }

    for (const auto& word : src["Words6To9"].items())
    {
        std::string num = word.key();
        size_t wordNum = std::strtoul(num.c_str(), nullptr, 10);

        if (wordNum == 0)
        {
            log<phosphor::logging::level::ERR>(
                "Invalid SRC word number in message registry",
                entry("ERROR_NAME=%s", name.c_str()),
                entry("SRC_WORD_NUM=%s", num.c_str()));

            throw std::runtime_error("Invalid SRC word in message registry");
        }

        auto attributes = word.value();
        std::string adPropName = attributes["AdditionalDataPropSource"];
        hexwordFields[wordNum] = std::move(adPropName);
    }

    if (!hexwordFields.empty())
    {
        return hexwordFields;
    }

    return std::nullopt;
}
std::optional<std::vector<SRC::WordNum>>
    getSRCSymptomIDFields(const nlohmann::json& src, const std::string& name)
{
    std::vector<SRC::WordNum> symptomIDFields;

    // Looks like:
    // "SymptomIDFields": ["SRCWord3", "SRCWord6"],

    for (const std::string& field : src["SymptomIDFields"])
    {
        // Just need the last digit off the end, e.g. SRCWord6.
        // The schema enforces the format of these.
        auto srcWordNum = field.substr(field.size() - 1);
        size_t num = std::strtoul(srcWordNum.c_str(), nullptr, 10);
        if (num == 0)
        {
            log<phosphor::logging::level::ERR>(
                "Invalid symptom ID field in message registry",
                entry("ERROR_NAME=%s", name.c_str()),
                entry("FIELD_NAME=%s", srcWordNum.c_str()));

            throw std::runtime_error("Invalid symptom ID in message registry");
        }
        symptomIDFields.push_back(num);
    }
    if (!symptomIDFields.empty())
    {
        return symptomIDFields;
    }

    return std::nullopt;
}

uint16_t getComponentID(uint8_t srcType, uint16_t reasonCode,
                        const nlohmann::json& pelEntry, const std::string& name)
{
    uint16_t id = 0;

    // If the ComponentID field is there, use that.  Otherwise, if it's a
    // 0xBD BMC error SRC, use the reasoncode.
    if (pelEntry.find("ComponentID") != pelEntry.end())
    {
        std::string componentID = pelEntry["ComponentID"];
        id = strtoul(componentID.c_str(), nullptr, 16);
    }
    else
    {
        // On BMC error SRCs (BD), can just get the component ID from
        // the first byte of the reason code.
        if (srcType == static_cast<uint8_t>(SRCType::bmcError))
        {
            id = reasonCode & 0xFF00;
        }
        else
        {
            log<level::ERR>("Missing component ID field in message registry",
                            entry("ERROR_NAME=%s", name.c_str()));

            throw std::runtime_error(
                "Missing component ID field in message registry");
        }
    }

    return id;
}

} // namespace helper

std::optional<Entry> Registry::lookup(const std::string& name, LookupType type,
                                      bool toCache)
{
    std::optional<nlohmann::json> registryTmp;
    auto& registryOpt = (_registry) ? _registry : registryTmp;
    if (!registryOpt)
    {
        registryOpt = readRegistry(_registryFile);
        if (!registryOpt)
        {
            return std::nullopt;
        }
        else if (toCache)
        {
            // Save message registry in memory for peltool
            _registry = std::move(registryTmp);
        }
    }
    auto& reg = (_registry) ? _registry : registryTmp;
    const auto& registry = reg.value();
    // Find an entry with this name in the PEL array.
    auto e = std::find_if(
        registry["PELs"].begin(), registry["PELs"].end(),
        [&name, &type](const auto& j) {
            return ((name == j["Name"] && type == LookupType::name) ||
                    (name == j["SRC"]["ReasonCode"] &&
                     type == LookupType::reasonCode));
        });

    if (e != registry["PELs"].end())
    {
        // Fill in the Entry structure from the JSON.  Most, but not all, fields
        // are optional.

        try
        {
            Entry entry;
            entry.name = (*e)["Name"];
            entry.subsystem = helper::getSubsystem((*e)["Subsystem"]);

            if (e->find("ActionFlags") != e->end())
            {
                entry.actionFlags = helper::getActionFlags((*e)["ActionFlags"]);
            }

            if (e->find("MfgActionFlags") != e->end())
            {
                entry.mfgActionFlags =
                    helper::getActionFlags((*e)["MfgActionFlags"]);
            }

            if (e->find("Severity") != e->end())
            {
                entry.severity = helper::getSeverity((*e)["Severity"]);
            }

            if (e->find("MfgSeverity") != e->end())
            {
                entry.mfgSeverity = helper::getSeverity((*e)["MfgSeverity"]);
            }

            if (e->find("EventType") != e->end())
            {
                entry.eventType = helper::getEventType((*e)["EventType"]);
            }

            if (e->find("EventScope") != e->end())
            {
                entry.eventScope = helper::getEventScope((*e)["EventScope"]);
            }

            auto& src = (*e)["SRC"];
            entry.src.reasonCode = helper::getSRCReasonCode(src, name);

            if (src.find("Type") != src.end())
            {
                entry.src.type = helper::getSRCType(src, name);
            }
            else
            {
                entry.src.type = static_cast<uint8_t>(SRCType::bmcError);
            }

            // Now that we know the SRC type and reason code,
            // we can get the component ID.
            entry.componentID = helper::getComponentID(
                entry.src.type, entry.src.reasonCode, *e, name);

            if (src.find("Words6To9") != src.end())
            {
                entry.src.hexwordADFields =
                    helper::getSRCHexwordFields(src, name);
            }

            if (src.find("SymptomIDFields") != src.end())
            {
                entry.src.symptomID = helper::getSRCSymptomIDFields(src, name);
            }

            if (src.find("PowerFault") != src.end())
            {
                entry.src.powerFault = src["PowerFault"];
            }

            auto& doc = (*e)["Documentation"];
            entry.doc.message = doc["Message"];
            entry.doc.description = doc["Description"];
            if (doc.find("MessageArgSources") != doc.end())
            {
                entry.doc.messageArgSources = doc["MessageArgSources"];
            }

            return entry;
        }
        catch (std::exception& e)
        {
            log<level::ERR>("Found invalid message registry field",
                            entry("ERROR=%s", e.what()));
        }
    }

    return std::nullopt;
}

std::optional<nlohmann::json>
    Registry::readRegistry(const std::filesystem::path& registryFile)
{
    // Look in /etc first in case someone put a test file there
    fs::path debugFile{fs::path{debugFilePath} / registryFileName};
    nlohmann::json registry;
    std::ifstream file;

    if (fs::exists(debugFile))
    {
        log<level::INFO>("Using debug PEL message registry");
        file.open(debugFile);
    }
    else
    {
        file.open(registryFile);
    }

    try
    {
        registry = nlohmann::json::parse(file);
    }
    catch (std::exception& e)
    {
        log<level::ERR>("Error parsing message registry JSON",
                        entry("JSON_ERROR=%s", e.what()));
        return std::nullopt;
    }
    return registry;
}

} // namespace message
} // namespace pels
} // namespace openpower
