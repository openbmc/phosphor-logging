#include "registry.hpp"

#include "pel_types.hpp"
#include "pel_values.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
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

} // namespace helper

std::optional<Entry> Registry::lookup(const std::string& name)
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
        file.open(_registryFile);
    }

    try
    {
        registry = nlohmann::json::parse(file);
    }
    catch (std::exception& e)
    {
        log<level::ERR>("Error parsing message registry JSON",
                        entry("JSON_ERROR=%s", e.what()));
        return {};
    }

    // Find an entry with this name in the PEL array.
    auto e = std::find_if(registry["PELs"].begin(), registry["PELs"].end(),
                          [&name](const auto& j) { return name == j["Name"]; });

    if (e != registry["PELs"].end())
    {
        // Fill in the Entry structure from the JSON.  Most, but not all, fields
        // are optional.

        try
        {
            Entry entry;
            entry.name = (*e)["Name"];
            entry.subsystem = helper::getSubsystem((*e)["Subsystem"]);
            entry.actionFlags = helper::getActionFlags((*e)["ActionFlags"]);

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

            // TODO: SRC fields

            return entry;
        }
        catch (std::exception& e)
        {
            log<level::ERR>("Found invalid message registry field",
                            entry("ERROR=%s", e.what()));
        }
    }

    return {};
}

} // namespace message
} // namespace pels
} // namespace openpower
