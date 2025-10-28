// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "registry.hpp"

#include "pel_types.hpp"
#include "pel_values.hpp"

#include <phosphor-logging/lg2.hpp>

#include <algorithm>
#include <fstream>

namespace openpower
{
namespace pels
{
namespace message
{

namespace pv = pel_values;
namespace fs = std::filesystem;

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
        lg2::error("Invalid subsystem name used in message registry: {SUBSYS}",
                   "SUBSYS", subsystemName);

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
        lg2::error("Invalid severity name used in message registry: {SEV}",
                   "SEV", severityName);

        throw std::runtime_error("Invalid severity used in message registry");
    }

    return std::get<pv::fieldValuePos>(*s);
}

std::vector<RegistrySeverity> getSeverities(const nlohmann::json& severity)
{
    std::vector<RegistrySeverity> severities;

    // The plain string value, like "unrecoverable"
    if (severity.is_string())
    {
        RegistrySeverity s;
        s.severity = getSeverity(severity.get<std::string>());
        severities.push_back(std::move(s));
    }
    else
    {
        // An array, with an element like:
        // {
        //    "SevValue": "unrecoverable",
        //    "System", "systemA"
        // }
        for (const auto& sev : severity)
        {
            RegistrySeverity s;
            s.severity = getSeverity(sev["SevValue"].get<std::string>());

            if (sev.contains("System"))
            {
                s.system = sev["System"].get<std::string>();
            }

            severities.push_back(std::move(s));
        }
    }

    return severities;
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
            lg2::error(
                "Invalid action flag name used in message registry: {FLAG}",
                "FLAG", flag);

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
        lg2::error("Invalid event type used in message registry: {TYPE}",
                   "TYPE", eventTypeName);

        throw std::runtime_error("Invalid event type used in message registry");
    }
    return std::get<pv::fieldValuePos>(*t);
}

uint8_t getEventScope(const std::string& eventScopeName)
{
    auto s = pv::findByName(eventScopeName, pv::eventScopeValues);
    if (s == pv::eventScopeValues.end())
    {
        lg2::error("Invalid event scope used in registry: {SCOPE}", "SCOPE",
                   eventScopeName);

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
        lg2::error(
            "Invalid reason code {RC} in message registry, error name = {ERROR}",
            "RC", rc, "ERROR", name);

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
        lg2::error(
            "Invalid SRC Type {TYPE} in message registry, error name = {ERROR}",
            "TYPE", srcType, "ERROR", name);

        throw std::runtime_error("Invalid SRC Type in message registry");
    }

    return type;
}

bool getSRCDeconfigFlag(const nlohmann::json& src)
{
    return src["DeconfigFlag"].get<bool>();
}

bool getSRCCheckstopFlag(const nlohmann::json& src)
{
    return src["CheckstopFlag"].get<bool>();
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
            lg2::error(
                "Invalid SRC word number {NUM} in message registry, error name = {ERROR}",
                "NUM", num, "ERROR", name);

            throw std::runtime_error("Invalid SRC word in message registry");
        }

        auto attributes = word.value();

        // Use an empty string for the description if it does not exist.
        auto itr = attributes.find("Description");
        std::string desc = (attributes.end() != itr) ? *itr : "";

        std::tuple<std::string, std::string> adPropSourceDesc(
            attributes["AdditionalDataPropSource"], desc);
        hexwordFields[wordNum] = std::move(adPropSourceDesc);
    }

    if (!hexwordFields.empty())
    {
        return hexwordFields;
    }

    return std::nullopt;
}
std::optional<std::vector<SRC::WordNum>> getSRCSymptomIDFields(
    const nlohmann::json& src, const std::string& name)
{
    std::vector<SRC::WordNum> symptomIDFields;

    // Looks like:
    // "SymptomIDFields": ["SRCWord3", "SRCWord6"],

    for (const std::string field : src["SymptomIDFields"])
    {
        // Just need the last digit off the end, e.g. SRCWord6.
        // The schema enforces the format of these.
        auto srcWordNum = field.substr(field.size() - 1);
        size_t num = std::strtoul(srcWordNum.c_str(), nullptr, 10);
        if (num == 0)
        {
            lg2::error(
                "Invalid symptom ID field {FIELD} in message registry, error name = {ERROR}",
                "FIELD", field, "ERROR", name);

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
    if (pelEntry.contains("ComponentID"))
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
            lg2::error(
                "Missing component ID field in message registry, error name = {ERROR}",
                "ERROR", name);

            throw std::runtime_error(
                "Missing component ID field in message registry");
        }
    }

    return id;
}

/**
 * @brief Says if the JSON is the format that contains AdditionalData keys
 *        as in index into them.
 *
 * @param[in] json - The highest level callout JSON
 *
 * @return bool - If it is the AdditionalData format or not
 */
bool calloutUsesAdditionalData(const nlohmann::json& json)
{
    return (json.contains("ADName") &&
            json.contains("CalloutsWithTheirADValues"));
}

/**
 * @brief Finds the callouts to use when there is no AdditionalData,
 *        but the system type may be used as a key.
 *
 * A sample calloutList array looks like the following.  The System and Systems
 * key are optional.
 *
 * System key - Value of the key will be the system name as a string. The
 * callouts for a specific system can define under this key.
 *
 * Systems key - Value of the key will be an array of system names in the form
 * of string. The callouts common to the systems mentioned in the array can
 * define under this key.
 *
 * If both System and Systems not present it means that entry applies to every
 * configuration that doesn't have another entry with a matching System and
 * Systems key.
 *
 *    {
 *        "System": "system1",
 *        "CalloutList":
 *        [
 *            {
 *                "Priority": "high",
 *                "LocCode": "P1-C1"
 *            },
 *            {
 *                "Priority": "low",
 *                "LocCode": "P1"
 *            }
 *        ]
 *    },
 *    {
 *        "Systems": ["system1", 'system2"],
 *        "CalloutList":
 *        [
 *            {
 *                "Priority": "high",
 *                "LocCode": "P0-C1"
 *            },
 *            {
 *                "Priority": "low",
 *                "LocCode": "P0"
 *            }
 *        ]
 *    }
 *
 * @param[in] json - The callout JSON
 * @param[in] systemNames - List of compatible system type names
 * @param[out] calloutLists - The JSON array which will hold the calloutlist to
 * use specific to the system.
 *
 * @return - Throws runtime exception if json is not an array or if calloutLists
 *           is empty.
 */
static void findCalloutList(const nlohmann::json& json,
                            const std::vector<std::string>& systemNames,
                            nlohmann::json& calloutLists)
{
    if (!json.is_array())
    {
        throw std::runtime_error{"findCalloutList was not passed a JSON array"};
    }

    // Flag to indicate whether system specific callouts found or not
    bool foundCallouts = false;

    for (const auto& callouts : json)
    {
        if (callouts.contains("System"))
        {
            if (std::ranges::find(systemNames,
                                  callouts["System"].get<std::string>()) !=
                systemNames.end())
            {
                calloutLists.insert(calloutLists.end(),
                                    callouts["CalloutList"].begin(),
                                    callouts["CalloutList"].end());
                foundCallouts = true;
            }
            continue;
        }

        if (callouts.contains("Systems"))
        {
            std::vector<std::string> systems =
                callouts["Systems"].get<std::vector<std::string>>();
            auto inSystemNames = [systemNames](const auto& system) {
                return (std::ranges::find(systemNames, system) !=
                        systemNames.end());
            };
            if (std::ranges::any_of(systems, inSystemNames))
            {
                calloutLists.insert(calloutLists.end(),
                                    callouts["CalloutList"].begin(),
                                    callouts["CalloutList"].end());
                foundCallouts = true;
            }
            continue;
        }

        // Any entry if neither System/Systems key matches with system name
        if (!foundCallouts)
        {
            calloutLists.insert(calloutLists.end(),
                                callouts["CalloutList"].begin(),
                                callouts["CalloutList"].end());
        }
    }
    if (calloutLists.empty())
    {
        std::string types;
        std::for_each(systemNames.begin(), systemNames.end(),
                      [&types](const auto& t) { types += t + '|'; });
        lg2::warning(
            "No matching system name entry or default system name entry "
            " for PEL callout list, names = {TYPES}",
            "TYPES", types);

        throw std::runtime_error{
            "Could not find a CalloutList JSON for this error and system name"};
    }
}

/**
 * @brief Creates a RegistryCallout based on the input JSON.
 *
 * The JSON looks like:
 *     {
 *          "Priority": "high",
 *          "LocCode": "E1"
 *          ...
 *     }
 *
 * Schema validation enforces what keys are present.
 *
 * @param[in] json - The JSON dictionary entry for a callout
 *
 * @return RegistryCallout - A filled in RegistryCallout
 */
RegistryCallout makeRegistryCallout(const nlohmann::json& json)
{
    RegistryCallout callout;

    callout.priority = "high";
    callout.useInventoryLocCode = false;

    if (json.contains("Priority"))
    {
        callout.priority = json["Priority"].get<std::string>();
    }

    if (json.contains("LocCode"))
    {
        callout.locCode = json["LocCode"].get<std::string>();
    }

    if (json.contains("Procedure"))
    {
        callout.procedure = json["Procedure"].get<std::string>();
    }
    else if (json.contains("SymbolicFRU"))
    {
        callout.symbolicFRU = json["SymbolicFRU"].get<std::string>();
    }
    else if (json.contains("SymbolicFRUTrusted"))
    {
        callout.symbolicFRUTrusted =
            json["SymbolicFRUTrusted"].get<std::string>();
    }

    if (json.contains("UseInventoryLocCode"))
    {
        callout.useInventoryLocCode = json["UseInventoryLocCode"].get<bool>();
    }

    return callout;
}

/**
 * @brief Returns the callouts to use when an AdditionalData key is
 *        required to find the correct entries.
 *
 *       The System property is used to find which CalloutList to use.
 *       If System is missing, then that CalloutList is valid for
 *       everything.
 *
 * The JSON looks like:
 *    {
 *        "System": "system1",
 *        "CalloutList":
 *        [
 *            {
 *                "Priority": "high",
 *                "LocCode": "P1-C1"
 *            },
 *            {
 *                "Priority": "low",
 *                "LocCode": "P1"
 *            }
 *        ]
 *    },
 *    {
 *        "Systems": ["system1", 'system2"],
 *        "CalloutList":
 *        [
 *            {
 *                "Priority": "high",
 *                "LocCode": "P0-C1"
 *            },
 *            {
 *                "Priority": "low",
 *                "LocCode": "P0"
 *            }
 *        ]
 *    }
 *
 * @param[in] json - The callout JSON
 * @param[in] systemNames - List of compatible system type names
 *
 * @return std::vector<RegistryCallout> - The callouts to use
 */
std::vector<RegistryCallout> getCalloutsWithoutAD(
    const nlohmann::json& json, const std::vector<std::string>& systemNames)
{
    std::vector<RegistryCallout> calloutEntries;

    nlohmann::json calloutLists = nlohmann::json::array();

    // Find the CalloutList to use based on the system type
    findCalloutList(json, systemNames, calloutLists);

    // We finally found the callouts, make the objects.
    for (const auto& callout : calloutLists)
    {
        calloutEntries.push_back(makeRegistryCallout(callout));
    }

    return calloutEntries;
}

/**
 * @brief Returns the callouts to use when an AdditionalData key is
 *        required to find the correct entries.
 *
 * The JSON looks like:
 *    {
 *        "ADName": "PROC_NUM",
 *        "CalloutsWithTheirADValues":
 *        [
 *            {
 *                "ADValue": "0",
 *                "Callouts":
 *                [
 *                    {
 *                        "CalloutList":
 *                        [
 *                            {
 *                                "Priority": "high",
 *                                "LocCode": "P1-C5"
 *                            }
 *                        ]
 *                    }
 *                ]
 *            }
 *        ]
 *     }
 *
 * Note that the "Callouts" entry above is the same as the top level
 * entry used when there is no AdditionalData key.
 *
 * @param[in] json - The callout JSON
 * @param[in] systemNames - List of compatible system type names
 * @param[in] additionalData - The AdditionalData property
 *
 * @return std::vector<RegistryCallout> - The callouts to use
 */
std::vector<RegistryCallout> getCalloutsUsingAD(
    const nlohmann::json& json, const std::vector<std::string>& systemNames,
    const AdditionalData& additionalData)
{
    // This indicates which AD field we'll be using
    auto keyName = json["ADName"].get<std::string>();

    // Get the actual value from the AD data
    auto adValue = additionalData.getValue(keyName);

    if (!adValue)
    {
        // The AdditionalData did not contain the necessary key
        lg2::warning("The PEL message registry callouts JSON "
                     "said to use an AdditionalData key that isn't in the "
                     "AdditionalData event log property, key = {KEY}",
                     "KEY", keyName);
        throw std::runtime_error{
            "Missing AdditionalData entry for this callout"};
    }

    const auto& callouts = json["CalloutsWithTheirADValues"];

    // find the entry with that AD value
    auto it = std::find_if(
        callouts.begin(), callouts.end(), [adValue](const nlohmann::json& j) {
            return *adValue == j["ADValue"].get<std::string>();
        });

    if (it == callouts.end())
    {
        // This can happen if not all possible values were in the
        // message registry and that's fine.  There may be a
        // "CalloutsWhenNoADMatch" section that contains callouts
        // to use in this case.
        if (json.contains("CalloutsWhenNoADMatch"))
        {
            return getCalloutsWithoutAD(json["CalloutsWhenNoADMatch"],
                                        systemNames);
        }
        return std::vector<RegistryCallout>{};
    }

    // Proceed to find the callouts possibly based on system type.
    return getCalloutsWithoutAD((*it)["Callouts"], systemNames);
}

/**
 * @brief Returns the journal capture information
 *
 *  The JSON looks like:
 *    "JournalCapture": {
 *        "NumLines": 30
 *    }
 *
 *    "JournalCapture":
 *    {
 *        "Sections": [
 *            {
 *                "SyslogID": "phosphor-log-manager",
 *                "NumLines": 20
 *            }
 *        ]
 *    }
 *
 * @param json - The journal capture JSON
 * @return JournalCapture - The filled in variant
 */
JournalCapture getJournalCapture(const nlohmann::json& json)
{
    JournalCapture capt;

    // Primary key is either NumLines or Sections.
    if (json.contains("NumLines"))
    {
        capt = json.at("NumLines").get<size_t>();
    }
    else if (json.contains("Sections"))
    {
        AppCaptureList captures;
        for (const auto& capture : json.at("Sections"))
        {
            AppCapture ac;
            ac.syslogID = capture.at("SyslogID").get<std::string>();
            ac.numLines = capture.at("NumLines").get<size_t>();
            captures.push_back(std::move(ac));
        }

        capt = captures;
    }
    else
    {
        lg2::error("JournalCapture section not the right format");
        throw std::runtime_error{"JournalCapture section not the right format"};
    }

    return capt;
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
        [&name, &type](const nlohmann::json& j) {
            return ((name == j.at("Name").get<std::string>() &&
                     type == LookupType::name) ||
                    (name == j.at("SRC").at("ReasonCode").get<std::string>() &&
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

            if (e->contains("Subsystem"))
            {
                entry.subsystem = helper::getSubsystem((*e)["Subsystem"]);
            }

            if (e->contains("ActionFlags"))
            {
                entry.actionFlags = helper::getActionFlags((*e)["ActionFlags"]);
            }

            if (e->contains("MfgActionFlags"))
            {
                entry.mfgActionFlags =
                    helper::getActionFlags((*e)["MfgActionFlags"]);
            }

            if (e->contains("Severity"))
            {
                entry.severity = helper::getSeverities((*e)["Severity"]);
            }

            if (e->contains("MfgSeverity"))
            {
                entry.mfgSeverity = helper::getSeverities((*e)["MfgSeverity"]);
            }

            if (e->contains("EventType"))
            {
                entry.eventType = helper::getEventType((*e)["EventType"]);
            }

            if (e->contains("EventScope"))
            {
                entry.eventScope = helper::getEventScope((*e)["EventScope"]);
            }

            auto& src = (*e)["SRC"];
            entry.src.reasonCode = helper::getSRCReasonCode(src, name);

            if (src.contains("Type"))
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

            if (src.contains("Words6To9"))
            {
                entry.src.hexwordADFields =
                    helper::getSRCHexwordFields(src, name);
            }

            if (src.contains("SymptomIDFields"))
            {
                entry.src.symptomID = helper::getSRCSymptomIDFields(src, name);
            }

            if (src.contains("DeconfigFlag"))
            {
                entry.src.deconfigFlag = helper::getSRCDeconfigFlag(src);
            }

            if (src.contains("CheckstopFlag"))
            {
                entry.src.checkstopFlag = helper::getSRCCheckstopFlag(src);
            }

            auto& doc = (*e)["Documentation"];
            entry.doc.message = doc["Message"];
            entry.doc.description = doc["Description"];
            if (doc.contains("MessageArgSources"))
            {
                entry.doc.messageArgSources = doc["MessageArgSources"];
            }

            // If there are callouts defined, save the JSON for later
            if (_loadCallouts)
            {
                if (e->contains("Callouts"))
                {
                    entry.callouts = (*e)["Callouts"];
                }
                else if (e->contains("CalloutsUsingAD"))
                {
                    entry.callouts = (*e)["CalloutsUsingAD"];
                }
            }

            if (e->contains("JournalCapture"))
            {
                entry.journalCapture =
                    helper::getJournalCapture((*e)["JournalCapture"]);
            }

            return entry;
        }
        catch (const std::exception& ex)
        {
            lg2::error("Found invalid message registry field. Error: {ERROR}",
                       "ERROR", ex);
        }
    }

    return std::nullopt;
}

std::optional<nlohmann::json> Registry::readRegistry(
    const std::filesystem::path& registryFile)
{
    // Look in /etc first in case someone put a test file there
    fs::path debugFile{fs::path{debugFilePath} / registryFileName};
    nlohmann::json registry;
    std::ifstream file;

    if (fs::exists(debugFile))
    {
        lg2::info("Using debug PEL message registry");
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
    catch (const std::exception& e)
    {
        lg2::error("Error parsing message registry JSON. Error: {ERROR}",
                   "ERROR", e);
        return std::nullopt;
    }
    return registry;
}

std::vector<RegistryCallout> Registry::getCallouts(
    const nlohmann::json& calloutJSON,
    const std::vector<std::string>& systemNames,
    const AdditionalData& additionalData)
{
    // The JSON may either use an AdditionalData key
    // as an index, or not.
    if (helper::calloutUsesAdditionalData(calloutJSON))
    {
        return helper::getCalloutsUsingAD(calloutJSON, systemNames,
                                          additionalData);
    }

    return helper::getCalloutsWithoutAD(calloutJSON, systemNames);
}

} // namespace message
} // namespace pels
} // namespace openpower
