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
#include "src.hpp"

#include "json_utils.hpp"
#include "paths.hpp"
#include "pel_values.hpp"

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{
namespace pv = openpower::pels::pel_values;
namespace rg = openpower::pels::message;
using namespace phosphor::logging;

void SRC::unflatten(Stream& stream)
{
    stream >> _header >> _version >> _flags >> _reserved1B >> _wordCount >>
        _reserved2B >> _size;

    for (auto& word : _hexData)
    {
        stream >> word;
    }

    _asciiString = std::make_unique<src::AsciiString>(stream);

    if (hasAdditionalSections())
    {
        // The callouts section is currently the only extra subsection type
        _callouts = std::make_unique<src::Callouts>(stream);
    }
}

void SRC::flatten(Stream& stream) const
{
    stream << _header << _version << _flags << _reserved1B << _wordCount
           << _reserved2B << _size;

    for (auto& word : _hexData)
    {
        stream << word;
    }

    _asciiString->flatten(stream);

    if (_callouts)
    {
        _callouts->flatten(stream);
    }
}

SRC::SRC(Stream& pel)
{
    try
    {
        unflatten(pel);
        validate();
    }
    catch (const std::exception& e)
    {
        log<level::ERR>("Cannot unflatten SRC", entry("ERROR=%s", e.what()));
        _valid = false;
    }
}

SRC::SRC(const message::Entry& regEntry, const AdditionalData& additionalData)
{
    _header.id = static_cast<uint16_t>(SectionID::primarySRC);
    _header.version = srcSectionVersion;
    _header.subType = srcSectionSubtype;
    _header.componentID = regEntry.componentID;

    _version = srcVersion;

    _flags = 0;
    if (regEntry.src.powerFault.value_or(false))
    {
        _flags |= powerFaultEvent;
    }

    _reserved1B = 0;

    _wordCount = numSRCHexDataWords + 1;

    _reserved2B = 0;

    // There are multiple fields encoded in the hex data words.
    std::for_each(_hexData.begin(), _hexData.end(),
                  [](auto& word) { word = 0; });
    setBMCFormat();
    setBMCPosition();
    // Partition dump status and partition boot type always 0 for BMC errors.
    //
    // TODO: Fill in other fields that aren't available yet.

    // Fill in the last 4 words from the AdditionalData property contents.
    setUserDefinedHexWords(regEntry, additionalData);

    _asciiString = std::make_unique<src::AsciiString>(regEntry);

    // TODO: add callouts using the Callouts object

    _size = baseSRCSize;
    _size += _callouts ? _callouts->flattenedSize() : 0;
    _header.size = Section::flattenedSize() + _size;

    _valid = true;
}

void SRC::setUserDefinedHexWords(const message::Entry& regEntry,
                                 const AdditionalData& ad)
{
    if (!regEntry.src.hexwordADFields)
    {
        return;
    }

    // Save the AdditionalData value corresponding to the
    // adName key in _hexData[wordNum].
    for (const auto& [wordNum, adName] : *regEntry.src.hexwordADFields)
    {
        // Can only set words 6 - 9
        if (!isUserDefinedWord(wordNum))
        {
            log<level::WARNING>("SRC user data word out of range",
                                entry("WORD_NUM=%d", wordNum),
                                entry("ERROR_NAME=%s", regEntry.name.c_str()));
            continue;
        }

        auto value = ad.getValue(adName);
        if (value)
        {
            _hexData[getWordIndexFromWordNum(wordNum)] =
                std::strtoul(value.value().c_str(), nullptr, 0);
        }
        else
        {
            log<level::WARNING>("Source for user data SRC word not found",
                                entry("ADDITIONALDATA_KEY=%s", adName.c_str()),
                                entry("ERROR_NAME=%s", regEntry.name.c_str()));
        }
    }
}

void SRC::validate()
{
    bool failed = false;

    if ((header().id != static_cast<uint16_t>(SectionID::primarySRC)) &&
        (header().id != static_cast<uint16_t>(SectionID::secondarySRC)))
    {
        log<level::ERR>("Invalid SRC section ID",
                        entry("ID=0x%X", header().id));
        failed = true;
    }

    // Check the version in the SRC, not in the header
    if (_version != srcVersion)
    {
        log<level::ERR>("Invalid SRC version", entry("VERSION=0x%X", _version));
        failed = true;
    }

    _valid = failed ? false : true;
}

std::optional<std::string> SRC::getJSON() const
{
    char tmpPsVal[11];
    const std::string jsonIndent(indentLevel, 0x20);
    sprintf(tmpPsVal, "%d", _header.version);
    std::string psVerStr(tmpPsVal);
    sprintf(tmpPsVal, "0x%X", _header.componentID);
    std::string psCbStr(tmpPsVal);
    sprintf(tmpPsVal, "%d", _header.subType);
    std::string psStStr(tmpPsVal);
    sprintf(tmpPsVal, "0x%02X", _wordCount);
    std::string psWordCount(tmpPsVal);
    sprintf(tmpPsVal, "0x%02X", _hexData[0]);
    std::string psHexWord2(tmpPsVal);
    sprintf(tmpPsVal, "0x%02X", _version);
    std::string psVersion(tmpPsVal);
    std::string tmpString;

    std::string ps;
    jsonInsert(ps, "Section Version", psVerStr, 1);
    jsonInsert(ps, "Sub-section type", psStStr, 1);
    jsonInsert(ps, "Created by", psCbStr, 1);
    jsonInsert(ps, "SRC Version", psVersion, 1);
    jsonInsert(ps, "SRC Format", psHexWord2, 1);

    jsonInsert(ps, "Power Control Net Fault",
               pv::boolString.at(isPowerFaultEvent()), 1);
    if (asciiString().substr(0, 2) == "BD")
    {
        rg::Registry registry(getMessageRegistryPath() / rg::registryFileName);
        auto entry = registry.lookup("0x" + asciiString().substr(4, 4),
                                     rg::lookupType::reasonCode);
        if (entry)
        {
            ps.append(jsonIndent + "\"Error Details\": {\n");
            if (entry->doc.messageArgSources)
            {
                size_t msgLen = entry->doc.message.length();
                char msg[msgLen + 1];
                strcpy(msg, entry->doc.message.c_str());
                const auto& argSources = entry->doc.messageArgSources;
                std::vector<uint32_t> argSourceVals;
                std::string message;
                std::vector<std::string> argValues = argSources.value();
                for (size_t i = 0; i < argValues.size(); ++i)
                {
                    argSourceVals.push_back(_hexData[getWordIndexFromWordNum(
                        argValues[i].back() - '0')]);
                }
                const char* msgPointer = msg;
                while (*msgPointer)
                {
                    if (*msgPointer == '%')
                    {
                        msgPointer++;
                        size_t wordIndex = *msgPointer - '0';
                        if (isdigit(*msgPointer) && wordIndex >= 1 &&
                            static_cast<uint16_t>(wordIndex) <=
                                argSourceVals.size())
                        {
                            sprintf(tmpPsVal, "0x%X",
                                    argSourceVals[wordIndex - 1]);
                            tmpString = tmpPsVal;
                            message.append(tmpString);
                        }
                        else
                        {
                            message.append("%" + std::string(1, *msgPointer));
                        }
                    }
                    else
                    {
                        message.push_back(*msgPointer);
                    }
                    msgPointer++;
                }
                jsonInsert(ps, "Message", message, 2);
            }
            else
            {
                jsonInsert(ps, "Message", entry->doc.message, 2);
            }
            if (entry->src.hexwordADFields)
            {
                std::map<size_t, std::string> adFields =
                    entry->src.hexwordADFields.value();
                for (const auto& hexwordMap : adFields)
                {
                    sprintf(
                        tmpPsVal, "0x%X",
                        _hexData[getWordIndexFromWordNum(hexwordMap.first)]);
                    tmpString = tmpPsVal;
                    jsonInsert(ps, hexwordMap.second, tmpString, 2);
                }
            }
            ps.erase(ps.size() - 2);
            ps.append("\n");
            ps.append(jsonIndent + "},\n");
        }
    }
    jsonInsert(ps, "Valid Word Count", psWordCount, 1);
    jsonInsert(ps, "Reference Code", asciiString().substr(0, 8), 1);
    for (size_t i = 2; i <= _wordCount; i++)
    {
        sprintf(tmpPsVal, "%08X", _hexData[getWordIndexFromWordNum(i)]);
        tmpString = tmpPsVal;
        jsonInsert(ps, "Hex Word " + std::to_string(i), tmpString, 1);
    }
    if (!_callouts)
    {
        ps.erase(ps.size() - 2);
        return ps;
    }
    const auto& callout = _callouts->callouts();
    const auto& compDescrp = pv::failingComponentType;
    ps.append(jsonIndent + "\"Callout Section\": {\n");
    jsonInsert(ps, "Callout Count", std::to_string(callout.size()), 2);
    ps.append(jsonIndent + jsonIndent + "\"Callouts\": [");
    for (auto& entry : callout)
    {
        ps.append("{\n");
        if (entry->fruIdentity())
        {
            jsonInsert(
                ps, "FRU Type",
                compDescrp.at(entry->fruIdentity()->failingComponentType()), 3);
            jsonInsert(ps, "Priority",
                       pv::getValue(entry->priority(),
                                    pel_values::calloutPriorityValues),
                       3);
            if (!entry->locationCode().empty())
            {
                jsonInsert(ps, "Location Code", entry->locationCode(), 3);
            }
            if (entry->fruIdentity()->getPN().has_value())
            {
                jsonInsert(ps, "Part Number",
                           entry->fruIdentity()->getPN().value(), 3);
            }
            if (entry->fruIdentity()->getMaintProc().has_value())
            {
                jsonInsert(ps, "Procedure Number",
                           entry->fruIdentity()->getMaintProc().value(), 3);
                if (pv::procedureDesc.find(
                        entry->fruIdentity()->getMaintProc().value()) !=
                    pv::procedureDesc.end())
                {
                    jsonInsert(
                        ps, "Description",
                        pv::procedureDesc.at(
                            entry->fruIdentity()->getMaintProc().value()),
                        3);
                }
            }
            if (entry->fruIdentity()->getCCIN().has_value())
            {
                jsonInsert(ps, "CCIN", entry->fruIdentity()->getCCIN().value(),
                           3);
            }
            if (entry->fruIdentity()->getSN().has_value())
            {
                jsonInsert(ps, "Serial Number",
                           entry->fruIdentity()->getSN().value(), 3);
            }
        }
        if (entry->pceIdentity())
        {
            const auto& pceIdentMtms = entry->pceIdentity()->mtms();
            if (!pceIdentMtms.machineTypeAndModel().empty())
            {
                jsonInsert(ps, "PCE MTMS",
                           pceIdentMtms.machineTypeAndModel() + "_" +
                               pceIdentMtms.machineSerialNumber(),
                           3);
            }
            if (!entry->pceIdentity()->enclosureName().empty())
            {
                jsonInsert(ps, "PCE Name",
                           entry->pceIdentity()->enclosureName(), 3);
            }
        }
        if (entry->mru())
        {
            const auto& mruCallouts = entry->mru()->mrus();
            std::string mruId;
            for (auto& element : mruCallouts)
            {
                sprintf(tmpPsVal, "%08X", element.id);
                tmpString = tmpPsVal;
                if (!mruId.empty())
                {
                    mruId.append(", " + tmpString);
                }
                else
                {
                    mruId.append(tmpString);
                }
            }
            jsonInsert(ps, "MRU Id", mruId, 3);
        }
        ps.erase(ps.size() - 2);
        ps.append("\n" + jsonIndent + jsonIndent + "}, ");
    };
    ps.erase(ps.size() - 2);
    ps.append("]\n" + jsonIndent + "}");
    return ps;
}

} // namespace pels
} // namespace openpower
