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
#include "pel.hpp"

#include "bcd_time.hpp"
#include "extended_user_data.hpp"
#include "extended_user_header.hpp"
#include "failing_mtms.hpp"
#include "json_utils.hpp"
#include "log_id.hpp"
#include "pel_rules.hpp"
#include "pel_values.hpp"
#include "section_factory.hpp"
#include "src.hpp"
#include "stream.hpp"
#include "user_data_formats.hpp"

#include <sys/stat.h>
#include <unistd.h>

#include <iostream>
#include <phosphor-logging/log.hpp>

constexpr size_t imkwSize = 4;

namespace openpower
{
namespace pels
{
namespace message = openpower::pels::message;
namespace pv = openpower::pels::pel_values;
using namespace phosphor::logging;

constexpr auto unknownValue = "Unknown";
constexpr uint8_t jsonCalloutSubtype = 0xCA;

PEL::PEL(const message::Entry& regEntry, uint32_t obmcLogID, uint64_t timestamp,
         phosphor::logging::Entry::Level severity,
         const AdditionalData& additionalData, const PelFFDC& ffdcFiles,
         const DataInterfaceBase& dataIface)
{
    std::map<std::string, std::vector<std::string>> debugData;
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

    auto ud = util::makeSysInfoUserDataSection(additionalData, dataIface);
    addUserDataSection(std::move(ud));

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

    // Store in the PEL any important debug data created while
    // building the PEL sections.
    if (!debugData.empty())
    {
        nlohmann::json data;
        data["PEL Internal Debug Data"] = debugData;
        ud = util::makeJSONUserDataSection(data);

        addUserDataSection(std::move(ud));

        // Also put in the journal for debug
        for (const auto& [name, data] : debugData)
        {
            for (const auto& message : data)
            {
                std::string entry = name + ": " + message;
                log<level::INFO>(entry.c_str());
            }
        }
    }

    _ph->setSectionCount(2 + _optionalSections.size());

    checkRulesAndFix();
}

PEL::PEL(std::vector<uint8_t>& data) : PEL(data, 0)
{
}

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
        log<level::WARNING>("Unflattening an invalid PEL");
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

void PEL::printSectionInJSON(const Section& section, std::string& buf,
                             std::map<uint16_t, size_t>& pluralSections,
                             message::Registry& registry,
                             const std::vector<std::string>& plugins,
                             uint8_t creatorID) const
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
            json = section.getJSON();
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
                        data.size() - SectionHeader::flattenedSize(), 2);
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
    printSectionInJSON(*(_ph.get()), buf, sections, registry, plugins);
    printSectionInJSON(*(_uh.get()), buf, sections, registry, plugins);
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
            log<level::WARNING>(
                "Could not shrink UserData section. Dropping",
                entry("SECTION_SIZE=%d\n", userData->header().size),
                entry("COMPONENT_ID=0x%02X", userData->header().componentID),
                entry("SUBTYPE=0x%X", userData->header().subType),
                entry("VERSION=0x%X", userData->header().version));
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

bool PEL::isCalloutPresent() const
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
                if ((*fruId).failingComponentType() != 0)
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
        auto op = std::find_if(_optionalSections.begin(),
                               _optionalSections.end(), [](auto& section) {
                                   return section->header().id ==
                                          static_cast<uint16_t>(
                                              SectionID::extUserData);
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
                auto ud =
                    util::makeSysInfoUserDataSection(additionalData, dataIface);
                extUserData->updateDataSection(subType, componentId,
                                               ud->data());
            }
        }
    }
}

namespace util
{

std::unique_ptr<UserData> makeJSONUserDataSection(const nlohmann::json& json)
{
    auto jsonString = json.dump();
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
    catch (std::exception& e)
    {
    }

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

    json["BMC Version ID"] = std::move(id);
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
    auto systemIMKeyword = dataIface.getSystemIMKeyword();

    if (imkwSize == systemIMKeyword.size())
    {
        char* buffer = new char[sizeof("ABCD1234\n")];
        snprintf(buffer, sizeof("ABCD1234"), "%08X",
                 *(uint32_t*)systemIMKeyword.data());
        json["System IM"] = "0x" + std::string(buffer);
        delete[] buffer;
    }
    else
    {
        log<level::WARNING>("System IM keyword is not expected size");
        json["System IM"] = "unavailable";
    }
}

void addStatesToJSON(nlohmann::json& json, const DataInterfaceBase& dataIface)
{
    json["BMCState"] = lastSegment('.', dataIface.getBMCState());
    json["ChassisState"] = lastSegment('.', dataIface.getChassisState());
    json["HostState"] = lastSegment('.', dataIface.getHostState());
}

std::unique_ptr<UserData>
    makeSysInfoUserDataSection(const AdditionalData& ad,
                               const DataInterfaceBase& dataIface)
{
    nlohmann::json json;

    addProcessNameToJSON(json, ad.getValue("_PID"), dataIface);
    addBMCFWVersionIDToJSON(json, dataIface);
    addIMKeyword(json, dataIface);
    addStatesToJSON(json, dataIface);

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
        log<level::ERR>("Could not get FFDC file size from FD",
                        entry("ERRNO=%d", e));
        return data;
    }

    if (0 == s.st_size)
    {
        log<level::ERR>("FFDC file is empty");
        return data;
    }

    data.resize(s.st_size);

    // Make sure its at the beginning, as maybe another
    // extension already used it.
    r = lseek(fd, 0, SEEK_SET);
    if (r == -1)
    {
        auto e = errno;
        log<level::ERR>("Could not seek to beginning of FFDC file",
                        entry("ERRNO=%d", e));
        return data;
    }

    r = read(fd, data.data(), s.st_size);
    if (r == -1)
    {
        auto e = errno;
        log<level::ERR>("Could not read FFDC file", entry("ERRNO=%d", e));
    }
    else if (r != s.st_size)
    {
        log<level::WARNING>("Could not read full FFDC file",
                            entry("FILE_SIZE=%d", s.st_size),
                            entry("SIZE_READ=%d", r));
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

} // namespace util

} // namespace pels
} // namespace openpower
