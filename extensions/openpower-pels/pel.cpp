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
#include "failing_mtms.hpp"
#include "hexdump.hpp"
#include "log_id.hpp"
#include "paths.hpp"
#include "pel_values.hpp"
#include "section_factory.hpp"
#include "src.hpp"
#include "stream.hpp"
#include "user_data_formats.hpp"

#include <filesystem>
#include <iostream>
#include <phosphor-logging/log.hpp>
#include <xyz/openbmc_project/Common/File/error.hpp>

namespace openpower
{
namespace pels
{
namespace fs = std::filesystem;
using namespace phosphor::logging;
namespace file_error = sdbusplus::xyz::openbmc_project::Common::File::Error;

namespace message = openpower::pels::message;
namespace pv = openpower::pels::pel_values;

PEL::PEL(const message::Entry& entry, uint32_t obmcLogID, uint64_t timestamp,
         phosphor::logging::Entry::Level severity,
         const AdditionalData& additionalData,
         const DataInterfaceBase& dataIface)
{
    _ph = std::make_unique<PrivateHeader>(entry.componentID, obmcLogID,
                                          timestamp);
    _uh = std::make_unique<UserHeader>(entry, severity);

    auto src = std::make_unique<SRC>(entry, additionalData);
    _optionalSections.push_back(std::move(src));

    auto mtms = std::make_unique<FailingMTMS>(dataIface);
    _optionalSections.push_back(std::move(mtms));

    if (!additionalData.empty())
    {
        auto ud = util::makeADUserDataSection(additionalData);
        _optionalSections.push_back(std::move(ud));
    }

    _ph->setSectionCount(2 + _optionalSections.size());
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

void PEL::flatten(std::vector<uint8_t>& pelBuffer)
{
    Stream pelData{pelBuffer};

    if (!valid())
    {
        using namespace phosphor::logging;
        log<level::WARNING>("Unflattening an invalid PEL");
    }

    _ph->flatten(pelData);
    _uh->flatten(pelData);

    for (auto& section : _optionalSections)
    {
        section->flatten(pelData);
    }
}

std::vector<uint8_t> PEL::data()
{
    std::vector<uint8_t> pelData;
    flatten(pelData);
    return pelData;
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

namespace util
{

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

    auto jsonString = json.dump();
    std::vector<uint8_t> jsonData(jsonString.begin(), jsonString.end());

    return std::make_unique<UserData>(
        static_cast<uint16_t>(ComponentID::phosphorLogging),
        static_cast<uint8_t>(UserDataFormat::json),
        static_cast<uint8_t>(UserDataFormatVersion::json), jsonData);
}

} // namespace util
void PEL::printSectionInJSON(Section& section, std::string& buf) const
{
    char tmpB[5];
    uint8_t id[] = {static_cast<uint8_t>(section.header().id >> 8),
                    static_cast<uint8_t>(section.header().id)};
    sprintf(tmpB, "%c%c", id[0], id[1]);
    std::string sectionID(tmpB);
    std::string sectionName = pv::sectionTitles.count(sectionID)
                                  ? pv::sectionTitles.at(sectionID)
                                  : "Unknown Section";
    if (section.valid())
    {
        auto json = section.getJSON();
        if (json)
        {
            buf += "\n\"" + sectionName + "\":[\n ";
            buf += *json + "\n],\n";
        }
        else
        {
            buf += "\n\"" + sectionName + "\":[\n ";
            std::vector<uint8_t> data;
            Stream s{data};
            section.flatten(s);
            std::string dstr = dumpHex(std::data(data), data.size());
            buf += dstr + "],\n";
        }
    }
    else
    {
        buf += "\n\"Invalid Section  \":[\n invalid \n],\n";
    }
}

void PEL::toJSON()
{
    std::string buf = "{";
    printSectionInJSON(*(_ph.get()), buf);
    printSectionInJSON(*(_uh.get()), buf);
    for (auto& section : this->optionalSections())
    {
        printSectionInJSON(*(section.get()), buf);
    }
    buf += "}";
    std::size_t found = buf.rfind(",");
    if (found != std::string::npos)
        buf.replace(found, 1, "");
    std::cout << buf << std::endl;
}

void PEL::printList()
{
    std::string listStr = "|---------------------------------------------------"
                          "---------------------------|\n";
    listStr += "| Entry Id      Commit Time      SubSystem                     "
               "   Committed by |\n";
    listStr += "| Platform Id   State            Event Severity                "
               "      Ascii Str |\n";
    listStr += "|--------------------------------------------------------------"
               "----------------|\n";
    for (auto it = fs::directory_iterator(
             "/var/lib/phosphor-logging/extensions/pels/logs");
         it != fs::directory_iterator(); ++it)
    {

        if (!fs::is_regular_file((*it).path()))
        {
            continue;
        }
        try
        {
            std::ifstream stream((*it).path(), std::ios::in | std::ios::binary);
            std::vector<uint8_t> data((std::istreambuf_iterator<char>(stream)),
                                      std::istreambuf_iterator<char>());
            stream.close();
            PEL pel{data};
            if (pel.valid())

            {
                listStr += "|--------------------------------------------------"
                           "------------"
                           "----------------|\n";

                // id
                char tmpValStr[50];
                sprintf(tmpValStr, "|%-10X", pel._ph->id());
                listStr += tmpValStr;
                // commit time
                sprintf(tmpValStr, "     %02X/%02X/%02X%02X  %02X:%02X:%02X",
                        pel._ph->commitTimestamp().month,
                        pel._ph->commitTimestamp().day,
                        pel._ph->commitTimestamp().yearMSB,
                        pel._ph->commitTimestamp().yearLSB,
                        pel._ph->commitTimestamp().hour,
                        pel._ph->commitTimestamp().minutes,
                        pel._ph->commitTimestamp().seconds);
                listStr += tmpValStr;
                std::string subsystem = pel._uh->getValue(
                    pel._uh->subsystem(), pel_values::subsystemValues);
                // subsystem
                sprintf(tmpValStr, "%20s", subsystem.c_str());
                listStr += tmpValStr;
                // committedby
                std::string committedBy = pel._uh->getValue(
                    pel._uh->eventType(), pel_values::eventTypeValues);
                sprintf(tmpValStr, "%25s|", committedBy.c_str());
                listStr += tmpValStr;
                listStr += "\n";
                // platformid
                sprintf(tmpValStr, "|%-15X", pel._ph->plid());
                listStr += tmpValStr;
                // state
                sprintf(tmpValStr, "%-15s", "TODO");
                listStr += tmpValStr;
                // severity
                std::string severity = pel._uh->getValue(
                    pel._uh->severity(), pel_values::severityValues);
                sprintf(tmpValStr, "%-15s", severity.c_str());
                listStr += tmpValStr;
                // ASCII
                sprintf(tmpValStr, "%-15s|",
                        pel.primarySRC().value()->asciiString().c_str());
                listStr += tmpValStr;
                listStr +=
                    "\n|--------------------------------------------------"
                    "------------"
                    "----------------|\n";
            }
            else
            {
                log<level::ERR>(
                    "Found invalid PEL file while restoring.  Removing.",
                    entry("FILENAME=%s", (*it).path().c_str()));
                fs::remove((*it).path());
            }
        }
        catch (std::exception& e)
        {
            log<level::ERR>("Hit exception while restoring PEL File",
                            entry("FILENAME=%s", (*it).path().c_str()),
                            entry("ERROR=%s", e.what()));
        }
    }
    printf("%s", listStr.c_str());
}
} // namespace pels
} // namespace openpower
