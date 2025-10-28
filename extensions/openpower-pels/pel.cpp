// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "config.h"

#include "pel.hpp"

#include "bcd_time.hpp"
#include "extended_user_data.hpp"
#include "extended_user_header.hpp"
#include "failing_mtms.hpp"
#include "fru_identity.hpp"
#include "json_utils.hpp"
#include "log_id.hpp"
#include "pel_rules.hpp"
#include "pel_values.hpp"
#include "section_factory.hpp"
#include "src.hpp"
#include "stream.hpp"
#include "user_data_formats.hpp"

#ifdef PEL_ENABLE_PHAL
#include "phal_service_actions.hpp"
#include "sbe_ffdc_handler.hpp"

#include <libguard/guard_interface.hpp>
#include <libguard/include/guard_record.hpp>

namespace libguard = openpower::guard;
#endif

#include <sys/stat.h>
#include <unistd.h>

#include <phosphor-logging/lg2.hpp>

#include <format>
#include <iostream>
#include <ranges>

namespace openpower
{
namespace pels
{
namespace pv = openpower::pels::pel_values;

constexpr auto unknownValue = "Unknown";
constexpr auto AdDIMMInfoFetchError = "DIMMs Info Fetch Error";

PEL::PEL(const message::Entry& regEntry, uint32_t obmcLogID, uint64_t timestamp,
         phosphor::logging::Entry::Level severity,
         const AdditionalData& additionalData, const PelFFDC& ffdcFilesIn,
         const DataInterfaceBase& dataIface, const JournalBase& journal)
{
    // No changes in input, for non SBE error related requests
    PelFFDC ffdcFiles = ffdcFilesIn;

#ifdef PEL_ENABLE_PHAL
    // Add sbe ffdc processed data into ffdcfiles.
    namespace sbe = openpower::pels::sbe;
    auto processReq =
        std::any_of(ffdcFiles.begin(), ffdcFiles.end(), [](const auto& file) {
            return file.format == UserDataFormat::custom &&
                   file.subType == sbe::sbeFFDCSubType;
        });
    // sbeFFDC can't be destroyed until the end of the PEL constructor
    // because it needs to keep around the FFDC Files to be used below.
    std::unique_ptr<sbe::SbeFFDC> sbeFFDCPtr;
    if (processReq)
    {
        sbeFFDCPtr =
            std::make_unique<sbe::SbeFFDC>(additionalData, ffdcFilesIn);
        const auto& sbeFFDCFiles = sbeFFDCPtr->getSbeFFDC();
        ffdcFiles.insert(ffdcFiles.end(), sbeFFDCFiles.begin(),
                         sbeFFDCFiles.end());

        // update pel priority for spare clock failures
        if (auto customSeverity = sbeFFDCPtr->getSeverity())
        {
            severity = customSeverity.value();
        }
    }
#endif

    DebugData debugData;
    nlohmann::json callouts;

    _ph = std::make_unique<PrivateHeader>(regEntry.componentID, obmcLogID,
                                          timestamp);
    _uh = std::make_unique<UserHeader>(regEntry, severity, additionalData,
                                       dataIface);

    // Extract any callouts embedded in an FFDC file.
    if (!ffdcFiles.empty())
    {
        try
        {
            callouts = getCalloutJSON(ffdcFiles);
        }
        catch (const std::exception& e)
        {
            debugData.emplace("FFDC file JSON callouts error",
                              std::vector<std::string>{e.what()});
        }
    }

    auto src =
        std::make_unique<SRC>(regEntry, additionalData, callouts, dataIface);

    nlohmann::json adSysInfoData(nlohmann::json::value_t::object);
    addAdDetailsForDIMMsCallout(src, dataIface, adSysInfoData, debugData);

    if (!src->getDebugData().empty())
    {
        // Something didn't go as planned
        debugData.emplace("SRC", src->getDebugData());
    }

    auto euh = std::make_unique<ExtendedUserHeader>(dataIface, regEntry, *src);

    _optionalSections.push_back(std::move(src));
    _optionalSections.push_back(std::move(euh));

    auto mtms = std::make_unique<FailingMTMS>(dataIface);
    _optionalSections.push_back(std::move(mtms));

    auto ud = util::makeSysInfoUserDataSection(additionalData, dataIface, true,
                                               adSysInfoData);
    addUserDataSection(std::move(ud));

    //  Check for pel severity of type - 0x51 = critical error, system
    //  termination and update terminate bit in SRC for pels
    updateTerminateBitInSRCSection();

    // Create a UserData section from AdditionalData.
    if (!additionalData.empty())
    {
        ud = util::makeADUserDataSection(additionalData);
        addUserDataSection(std::move(ud));
    }

    // Add any FFDC files into UserData sections
    for (const auto& file : ffdcFiles)
    {
        ud = util::makeFFDCuserDataSection(regEntry.componentID, file);
        if (!ud)
        {
            // Add this error into the debug data UserData section
            std::ostringstream msg;
            msg << "Could not make PEL FFDC UserData section from file"
                << std::hex << regEntry.componentID << " " << file.subType
                << " " << file.version;
            if (debugData.count("FFDC File"))
            {
                debugData.at("FFDC File").push_back(msg.str());
            }
            else
            {
                debugData.emplace("FFDC File",
                                  std::vector<std::string>{msg.str()});
            }

            continue;
        }

        addUserDataSection(std::move(ud));
    }

#ifdef PEL_ENABLE_PHAL
    auto path = std::string(OBJ_ENTRY) + '/' + std::to_string(obmcLogID);
    openpower::pels::phal::createServiceActions(callouts, dataIface, plid());
#endif

    // Store in the PEL any important debug data created while
    // building the PEL sections.
    if (!debugData.empty())
    {
        nlohmann::json data;
        data["PEL Internal Debug Data"] = debugData;
        ud = util::makeJSONUserDataSection(data);

        addUserDataSection(std::move(ud));

        // Also put in the journal for debug
        for (const auto& [name, msgs] : debugData)
        {
            for (const auto& message : msgs)
            {
                lg2::info("{NAME}: {MSG}", "NAME", name, "MSG", message);
            }
        }
    }

    addJournalSections(regEntry, journal);

    _ph->setSectionCount(2 + _optionalSections.size());

    checkRulesAndFix();
}

PEL::PEL(std::vector<uint8_t>& data) : PEL(data, 0) {}

PEL::PEL(std::vector<uint8_t>& data, uint32_t obmcLogID)
{
    populateFromRawData(data, obmcLogID);
}

void PEL::populateFromRawData(std::vector<uint8_t>& data, uint32_t obmcLogID)
{
    Stream pelData{data};
    _ph = std::make_unique<PrivateHeader>(pelData);
    if (obmcLogID != 0)
    {
        _ph->setOBMCLogID(obmcLogID);
    }

    _uh = std::make_unique<UserHeader>(pelData);

    // Use the section factory to create the rest of the objects
    for (size_t i = 2; i < _ph->sectionCount(); i++)
    {
        auto section = section_factory::create(pelData);
        _optionalSections.push_back(std::move(section));
    }
}

bool PEL::valid() const
{
    bool valid = _ph->valid();

    if (valid)
    {
        valid = _uh->valid();
    }

    if (valid)
    {
        if (!std::all_of(_optionalSections.begin(), _optionalSections.end(),
                         [](const auto& section) { return section->valid(); }))
        {
            valid = false;
        }
    }

    return valid;
}

void PEL::setCommitTime()
{
    auto now = std::chrono::system_clock::now();
    _ph->setCommitTimestamp(getBCDTime(now));
}

void PEL::assignID()
{
    _ph->setID(generatePELID());
}

void PEL::flatten(std::vector<uint8_t>& pelBuffer) const
{
    Stream pelData{pelBuffer};

    if (!valid())
    {
        lg2::warning("Unflattening an invalid PEL");
    }

    _ph->flatten(pelData);
    _uh->flatten(pelData);

    for (auto& section : _optionalSections)
    {
        section->flatten(pelData);
    }
}

std::vector<uint8_t> PEL::data() const
{
    std::vector<uint8_t> pelData;
    flatten(pelData);
    return pelData;
}

size_t PEL::size() const
{
    size_t size = 0;

    if (_ph)
    {
        size += _ph->header().size;
    }

    if (_uh)
    {
        size += _uh->header().size;
    }

    for (const auto& section : _optionalSections)
    {
        size += section->header().size;
    }

    return size;
}

std::optional<SRC*> PEL::primarySRC() const
{
    auto src = std::find_if(
        _optionalSections.begin(), _optionalSections.end(), [](auto& section) {
            return section->header().id ==
                   static_cast<uint16_t>(SectionID::primarySRC);
        });
    if (src != _optionalSections.end())
    {
        return static_cast<SRC*>(src->get());
    }

    return std::nullopt;
}

void PEL::checkRulesAndFix()
{
    // Only fix if the action flags are at their default value which
    // means they weren't specified in the registry.  Otherwise
    // assume the user knows what they are doing.
    if (_uh->actionFlags() == actionFlagsDefault)
    {
        auto [actionFlags, eventType] =
            pel_rules::check(0, _uh->eventType(), _uh->severity());

        _uh->setActionFlags(actionFlags);
        _uh->setEventType(eventType);
    }
}

void PEL::printSectionInJSON(
    const Section& section, std::string& buf,
    std::map<uint16_t, size_t>& pluralSections, message::Registry& registry,
    const std::vector<std::string>& plugins, uint8_t creatorID) const
{
    char tmpB[5];
    uint8_t id[] = {static_cast<uint8_t>(section.header().id >> 8),
                    static_cast<uint8_t>(section.header().id)};
    sprintf(tmpB, "%c%c", id[0], id[1]);
    std::string sectionID(tmpB);
    std::string sectionName = pv::sectionTitles.count(sectionID)
                                  ? pv::sectionTitles.at(sectionID)
                                  : "Unknown Section";

    // Add a count if there are multiple of this type of section
    auto count = pluralSections.find(section.header().id);
    if (count != pluralSections.end())
    {
        sectionName += " " + std::to_string(count->second);
        count->second++;
    }

    if (section.valid())
    {
        std::optional<std::string> json;
        if (sectionID == "PS" || sectionID == "SS")
        {
            json = section.getJSON(registry, plugins, creatorID);
        }
        else if ((sectionID == "UD") || (sectionID == "ED"))
        {
            json = section.getJSON(creatorID, plugins);
        }
        else
        {
            json = section.getJSON(creatorID);
        }

        buf += "\"" + sectionName + "\": {\n";

        if (json)
        {
            buf += *json + "\n},\n";
        }
        else
        {
            jsonInsert(buf, pv::sectionVer,
                       getNumberString("%d", section.header().version), 1);
            jsonInsert(buf, pv::subSection,
                       getNumberString("%d", section.header().subType), 1);
            jsonInsert(buf, pv::createdBy,
                       getNumberString("0x%X", section.header().componentID),
                       1);

            std::vector<uint8_t> data;
            Stream s{data};
            section.flatten(s);
            std::string dstr =
                dumpHex(std::data(data) + SectionHeader::flattenedSize(),
                        data.size() - SectionHeader::flattenedSize(), 2)
                    .get();
            std::string jsonIndent(indentLevel, 0x20);
            buf += jsonIndent + "\"Data\": [\n";
            buf += dstr;
            buf += jsonIndent + "]\n";
            buf += "},\n";
        }
    }
    else
    {
        buf += "\n\"Invalid Section\": [\n    \"invalid\"\n],\n";
    }
}

std::map<uint16_t, size_t> PEL::getPluralSections() const
{
    std::map<uint16_t, size_t> sectionCounts;

    for (const auto& section : optionalSections())
    {
        if (sectionCounts.find(section->header().id) == sectionCounts.end())
        {
            sectionCounts[section->header().id] = 1;
        }
        else
        {
            sectionCounts[section->header().id]++;
        }
    }

    std::map<uint16_t, size_t> sections;
    for (const auto& [id, count] : sectionCounts)
    {
        if (count > 1)
        {
            // Start with 0 here and printSectionInJSON()
            // will increment it as it goes.
            sections.emplace(id, 0);
        }
    }

    return sections;
}

void PEL::toJSON(message::Registry& registry,
                 const std::vector<std::string>& plugins) const
{
    auto sections = getPluralSections();

    std::string buf = "{\n";
    printSectionInJSON(*(_ph.get()), buf, sections, registry, plugins,
                       _ph->creatorID());
    printSectionInJSON(*(_uh.get()), buf, sections, registry, plugins,
                       _ph->creatorID());
    for (auto& section : this->optionalSections())
    {
        printSectionInJSON(*(section.get()), buf, sections, registry, plugins,
                           _ph->creatorID());
    }
    buf += "}";
    std::size_t found = buf.rfind(",");
    if (found != std::string::npos)
        buf.replace(found, 1, "");
    std::cout << buf << std::endl;
}

bool PEL::addUserDataSection(std::unique_ptr<UserData> userData)
{
    if (size() + userData->header().size > _maxPELSize)
    {
        if (userData->shrink(_maxPELSize - size()))
        {
            _optionalSections.push_back(std::move(userData));
        }
        else
        {
            lg2::warning("Could not shrink UserData section. Dropping. "
                         "Section size = {SSIZE}, Component ID = {COMP_ID}, "
                         "Subtype = {SUBTYPE}, Version = {VERSION}",
                         "SSIZE", userData->header().size, "COMP_ID",
                         userData->header().componentID, "SUBTYPE",
                         userData->header().subType, "VERSION",
                         userData->header().version);
            return false;
        }
    }
    else
    {
        _optionalSections.push_back(std::move(userData));
    }
    return true;
}

nlohmann::json PEL::getCalloutJSON(const PelFFDC& ffdcFiles)
{
    nlohmann::json callouts;

    for (const auto& file : ffdcFiles)
    {
        if ((file.format == UserDataFormat::json) &&
            (file.subType == jsonCalloutSubtype))
        {
            auto data = util::readFD(file.fd);
            if (data.empty())
            {
                throw std::runtime_error{
                    "Could not get data from JSON callout file descriptor"};
            }

            std::string jsonString{data.begin(), data.begin() + data.size()};

            callouts = nlohmann::json::parse(jsonString);
            break;
        }
    }

    return callouts;
}

bool PEL::isHwCalloutPresent() const
{
    auto pSRC = primarySRC();
    if (!pSRC)
    {
        return false;
    }

    bool calloutPresent = false;
    if ((*pSRC)->callouts())
    {
        for (auto& i : (*pSRC)->callouts()->callouts())
        {
            if (((*i).fruIdentity()))
            {
                auto& fruId = (*i).fruIdentity();
                if ((*fruId).failingComponentType() ==
                    src::FRUIdentity::hardwareFRU)
                {
                    calloutPresent = true;
                    break;
                }
            }
        }
    }

    return calloutPresent;
}

void PEL::updateSysInfoInExtendedUserDataSection(
    const DataInterfaceBase& dataIface)
{
    const AdditionalData additionalData;

    // Check for PEL from Hostboot
    if (_ph->creatorID() == static_cast<uint8_t>(CreatorID::hostboot))
    {
        // Get the ED section from PEL
        auto op = std::find_if(
            _optionalSections.begin(), _optionalSections.end(),
            [](auto& section) {
                return section->header().id ==
                       static_cast<uint16_t>(SectionID::extUserData);
            });

        // Check for ED section found and its not the last section of PEL
        if (op != _optionalSections.end())
        {
            // Get the extended user data class mapped to found section
            auto extUserData = static_cast<ExtendedUserData*>(op->get());

            // Check for the creator ID is for OpenBMC
            if (extUserData->creatorID() ==
                static_cast<uint8_t>(CreatorID::openBMC))
            {
                // Update subtype and component id
                auto subType = static_cast<uint8_t>(UserDataFormat::json);
                auto componentId =
                    static_cast<uint16_t>(ComponentID::phosphorLogging);

                // Update system data to ED section
                auto ud = util::makeSysInfoUserDataSection(additionalData,
                                                           dataIface, false);
                extUserData->updateDataSection(subType, componentId,
                                               ud->data());
            }
        }
    }
}

bool PEL::getDeconfigFlag() const
{
    auto creator = static_cast<CreatorID>(_ph->creatorID());

    if ((creator == CreatorID::openBMC) || (creator == CreatorID::hostboot))
    {
        auto src = primarySRC();
        return (*src)->getErrorStatusFlag(SRC::ErrorStatusFlags::deconfigured);
    }
    return false;
}

bool PEL::getGuardFlag() const
{
    auto creator = static_cast<CreatorID>(_ph->creatorID());

    if ((creator == CreatorID::openBMC) || (creator == CreatorID::hostboot))
    {
        auto src = primarySRC();
        return (*src)->getErrorStatusFlag(SRC::ErrorStatusFlags::guarded);
    }
    return false;
}

void PEL::updateTerminateBitInSRCSection()
{
    //  Check for pel severity of type - 0x51 = critical error, system
    //  termination
    if (_uh->severity() == 0x51)
    {
        // Get the primary SRC section
        auto pSRC = primarySRC();
        if (pSRC)
        {
            (*pSRC)->setTerminateBit();
        }
    }
}

void PEL::addJournalSections(const message::Entry& regEntry,
                             const JournalBase& journal)
{
    if (!regEntry.journalCapture)
    {
        return;
    }

    // Write all unwritten journal data to disk.
    journal.sync();

    const auto& jc = regEntry.journalCapture.value();
    std::vector<std::vector<std::string>> allMessages;

    if (std::holds_alternative<size_t>(jc))
    {
        // Get the previous numLines journal entries
        const auto& numLines = std::get<size_t>(jc);
        try
        {
            auto messages = journal.getMessages("", numLines);
            if (!messages.empty())
            {
                allMessages.push_back(std::move(messages));
            }
        }
        catch (const std::exception& e)
        {
            lg2::error("Failed during journal collection: {ERROR}", "ERROR", e);
        }
    }
    else if (std::holds_alternative<message::AppCaptureList>(jc))
    {
        // Get journal entries based on the syslog id field.
        const auto& sections = std::get<message::AppCaptureList>(jc);
        for (const auto& [syslogID, numLines] : sections)
        {
            try
            {
                auto messages = journal.getMessages(syslogID, numLines);
                if (!messages.empty())
                {
                    allMessages.push_back(std::move(messages));
                }
            }
            catch (const std::exception& e)
            {
                lg2::error("Failed during journal collection: {ERROR}", "ERROR",
                           e);
            }
        }
    }

    // Create the UserData sections
    for (const auto& messages : allMessages)
    {
        auto buffer = util::flattenLines(messages);

        // If the buffer is way too big, it can overflow the uint16_t
        // PEL section size field that is checked below so do a cursory
        // check here.
        if (buffer.size() > _maxPELSize)
        {
            lg2::warning(
                "Journal UserData section does not fit in PEL, dropping. "
                "PEL size = {PEL_SIZE}, data size = {DATA_SIZE}",
                "PEL_SIZE", size(), "DATA_SIZE", buffer.size());
            continue;
        }

        // Sections must be 4 byte aligned.
        while (buffer.size() % 4 != 0)
        {
            buffer.push_back(0);
        }

        auto ud = std::make_unique<UserData>(
            static_cast<uint16_t>(ComponentID::phosphorLogging),
            static_cast<uint8_t>(UserDataFormat::text),
            static_cast<uint8_t>(UserDataFormatVersion::text), buffer);

        if (size() + ud->header().size <= _maxPELSize)
        {
            _optionalSections.push_back(std::move(ud));
        }
        else
        {
            // Don't attempt to shrink here since we'd be dropping the
            // most recent journal entries which would be confusing.
            lg2::warning(
                "Journal UserData section does not fit in PEL, dropping. "
                "PEL size = {PEL_SIZE}, data size = {DATA_SIZE}",
                "PEL_SIZE", size(), "DATA_SIZE", buffer.size());
            ud.reset();
            continue;
        }
    }
}

void PEL::addAdDetailsForDIMMsCallout(
    const std::unique_ptr<SRC>& src, const DataInterfaceBase& dataIface,
    nlohmann::json& adSysInfoData, DebugData& debugData)
{
    if (!src->callouts())
    {
        // No callouts
        return;
    }

    auto isDIMMCallout = [&dataIface](const auto& callout) {
        auto locCode{callout->locationCode()};
        if (locCode.empty())
        {
            // Not a hardware callout. No action required
            return false;
        }
        else
        {
            return const_cast<DataInterfaceBase&>(dataIface).isDIMM(locCode);
        }
    };
    auto addAdDIMMDetails = [&dataIface, &adSysInfoData,
                             &debugData](const auto& callout) {
        auto dimmLocCode{callout->locationCode()};

        auto diPropVal = dataIface.getDIProperty(dimmLocCode);
        if (!diPropVal.has_value())
        {
            std::string errMsg{
                std::format("Failed reading DI property from "
                            "VINI Interface for the LocationCode:[{}]",
                            dimmLocCode)};
            debugData[AdDIMMInfoFetchError].emplace_back(errMsg);
        }
        else
        {
            util::addDIMMInfo(dimmLocCode, diPropVal.value(), adSysInfoData);
        }
    };

    auto DIMMsCallouts = src->callouts()->callouts() |
                         std::views::filter(isDIMMCallout);

    std::ranges::for_each(DIMMsCallouts, addAdDIMMDetails);
}

namespace util
{

std::unique_ptr<UserData> makeJSONUserDataSection(const nlohmann::json& json)
{
    std::string jsonString;
    try
    {
        jsonString = json.dump();
    }
    catch (const std::exception& e)
    {
        lg2::error("json.dump() failed with: {ERROR}", "ERROR", e);
        jsonString = "Invalid JSON passed to makeJSONUserDataSection!";
    }

    std::vector<uint8_t> jsonData(jsonString.begin(), jsonString.end());

    // Pad to a 4 byte boundary
    while ((jsonData.size() % 4) != 0)
    {
        jsonData.push_back(0);
    }

    return std::make_unique<UserData>(
        static_cast<uint16_t>(ComponentID::phosphorLogging),
        static_cast<uint8_t>(UserDataFormat::json),
        static_cast<uint8_t>(UserDataFormatVersion::json), jsonData);
}

std::unique_ptr<UserData> makeADUserDataSection(const AdditionalData& ad)
{
    assert(!ad.empty());
    nlohmann::json json;

    // Remove the 'ESEL' entry, as it contains a full PEL in the value.
    if (ad.getValue("ESEL"))
    {
        auto newAD = ad;
        newAD.remove("ESEL");
        json = newAD.toJSON();
    }
    else
    {
        json = ad.toJSON();
    }

    return makeJSONUserDataSection(json);
}

void addProcessNameToJSON(nlohmann::json& json,
                          const std::optional<std::string>& pid,
                          const DataInterfaceBase& dataIface)
{
    std::string name{unknownValue};

    try
    {
        if (pid)
        {
            auto n = dataIface.getProcessName(*pid);
            if (n)
            {
                name = *n;
            }
        }
    }
    catch (const std::exception& e)
    {}

    if (pid)
    {
        json["Process Name"] = std::move(name);
    }
}

void addBMCFWVersionIDToJSON(nlohmann::json& json,
                             const DataInterfaceBase& dataIface)
{
    auto id = dataIface.getBMCFWVersionID();
    if (id.empty())
    {
        id = unknownValue;
    }

    json["FW Version ID"] = std::move(id);
}

std::string lastSegment(char separator, std::string data)
{
    auto pos = data.find_last_of(separator);
    if (pos != std::string::npos)
    {
        data = data.substr(pos + 1);
    }

    return data;
}

void addIMKeyword(nlohmann::json& json, const DataInterfaceBase& dataIface)
{
    auto keyword = dataIface.getSystemIMKeyword();

    std::string value{};

    std::for_each(keyword.begin(), keyword.end(), [&](const auto& byte) {
        value += std::format("{:02X}", byte);
    });

    json["System IM"] = value;
}

void addStatesToJSON(nlohmann::json& json, const DataInterfaceBase& dataIface)
{
    json["BMCState"] = lastSegment('.', dataIface.getBMCState());
    json["ChassisState"] = lastSegment('.', dataIface.getChassisState());
    json["HostState"] = lastSegment('.', dataIface.getHostState());
    json["BootState"] = lastSegment('.', dataIface.getBootState());
}

void addBMCUptime(nlohmann::json& json, const DataInterfaceBase& dataIface)
{
    auto seconds = dataIface.getUptimeInSeconds();
    if (seconds)
    {
        json["BMCUptime"] = dataIface.getBMCUptime(*seconds);
    }
    else
    {
        json["BMCUptime"] = "";
    }
    json["BMCLoad"] = dataIface.getBMCLoadAvg();
}

std::unique_ptr<UserData> makeSysInfoUserDataSection(
    const AdditionalData& ad, const DataInterfaceBase& dataIface,
    bool addUptime, const nlohmann::json& adSysInfoData)
{
    nlohmann::json json;

    addProcessNameToJSON(json, ad.getValue("_PID"), dataIface);
    addBMCFWVersionIDToJSON(json, dataIface);
    addIMKeyword(json, dataIface);
    addStatesToJSON(json, dataIface);

    if (addUptime)
    {
        addBMCUptime(json, dataIface);
    }

    if (!adSysInfoData.empty())
    {
        json.update(adSysInfoData);
    }

    return makeJSONUserDataSection(json);
}

std::vector<uint8_t> readFD(int fd)
{
    std::vector<uint8_t> data;

    // Get the size
    struct stat s;
    int r = fstat(fd, &s);
    if (r != 0)
    {
        auto e = errno;
        lg2::error("Could not get FFDC file size from FD, errno = {ERRNO}",
                   "ERRNO", e);
        return data;
    }

    if (0 == s.st_size)
    {
        lg2::error("FFDC file is empty");
        return data;
    }

    data.resize(s.st_size);

    // Make sure its at the beginning, as maybe another
    // extension already used it.
    r = lseek(fd, 0, SEEK_SET);
    if (r == -1)
    {
        auto e = errno;
        lg2::error("Could not seek to beginning of FFDC file, errno = {ERRNO}",
                   "ERRNO", e);
        return data;
    }

    r = read(fd, data.data(), s.st_size);
    if (r == -1)
    {
        auto e = errno;
        lg2::error("Could not read FFDC file, errno = {ERRNO}", "ERRNO", e);
    }
    else if (r != s.st_size)
    {
        lg2::warning("Could not read full FFDC file. "
                     "File size = {FSIZE}, Size read = {SIZE_READ}",
                     "FSIZE", s.st_size, "SIZE_READ", r);
    }

    return data;
}

std::unique_ptr<UserData> makeFFDCuserDataSection(uint16_t componentID,
                                                  const PelFFDCfile& file)
{
    auto data = readFD(file.fd);

    if (data.empty())
    {
        return std::unique_ptr<UserData>();
    }

    // The data needs 4 Byte alignment, and save amount padded for the
    // CBOR case.
    uint32_t pad = 0;
    while (data.size() % 4)
    {
        data.push_back(0);
        pad++;
    }

    // For JSON, CBOR, and Text use our component ID, subType, and version,
    // otherwise use the supplied ones.
    uint16_t compID = static_cast<uint16_t>(ComponentID::phosphorLogging);
    uint8_t subType{};
    uint8_t version{};

    switch (file.format)
    {
        case UserDataFormat::json:
            subType = static_cast<uint8_t>(UserDataFormat::json);
            version = static_cast<uint8_t>(UserDataFormatVersion::json);
            break;
        case UserDataFormat::cbor:
            subType = static_cast<uint8_t>(UserDataFormat::cbor);
            version = static_cast<uint8_t>(UserDataFormatVersion::cbor);

            // The CBOR parser will fail on the extra pad bytes since they
            // aren't CBOR.  Add the amount we padded to the end and other
            // code will remove it all before parsing.
            {
                data.resize(data.size() + 4);
                Stream stream{data};
                stream.offset(data.size() - 4);
                stream << pad;
            }

            break;
        case UserDataFormat::text:
            subType = static_cast<uint8_t>(UserDataFormat::text);
            version = static_cast<uint8_t>(UserDataFormatVersion::text);
            break;
        case UserDataFormat::custom:
        default:
            // Use the passed in values
            compID = componentID;
            subType = file.subType;
            version = file.version;
            break;
    }

    return std::make_unique<UserData>(compID, subType, version, data);
}

std::vector<uint8_t> flattenLines(const std::vector<std::string>& lines)
{
    std::vector<uint8_t> out;

    for (const auto& line : lines)
    {
        out.insert(out.end(), line.begin(), line.end());

        if (out.back() != '\n')
        {
            out.push_back('\n');
        }
    }

    return out;
}

void addDIMMInfo(const std::string& locationCode,
                 const std::vector<std::uint8_t>& diPropVal,
                 nlohmann::json& adSysInfoData)
{
    nlohmann::json dimmInfoObj;
    dimmInfoObj["Location Code"] = locationCode;
    std::ranges::transform(
        diPropVal, std::back_inserter(dimmInfoObj["DRAM Manufacturer ID"]),
        [](const auto& diPropEachByte) {
            return std::format("{:#04x}", diPropEachByte);
        });
    adSysInfoData["DIMMs Additional Info"] += dimmInfoObj;
}

} // namespace util

} // namespace pels
} // namespace openpower
