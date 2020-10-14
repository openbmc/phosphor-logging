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

#include "device_callouts.hpp"
#include "json_utils.hpp"
#include "paths.hpp"
#include "pel_values.hpp"
#ifdef PELTOOL
#include <Python.h>

#include <fifo_map.hpp>
#include <nlohmann/json.hpp>
#include <sstream>
#endif
#include <fmt/format.h>

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{
namespace pv = openpower::pels::pel_values;
namespace rg = openpower::pels::message;
using namespace phosphor::logging;
using namespace std::string_literals;

constexpr size_t ccinSize = 4;

#ifdef PELTOOL
// Use fifo_map as nlohmann::json's map. We are just ignoring the 'less'
// compare.  With this map the keys are kept in FIFO order.
template <class K, class V, class dummy_compare, class A>
using fifoMap = nlohmann::fifo_map<K, V, nlohmann::fifo_map_compare<K>, A>;
using fifoJSON = nlohmann::basic_json<fifoMap>;

void pyDecRef(PyObject* pyObj)
{
    Py_XDECREF(pyObj);
}

/**
 * @brief Returns a JSON string to append to SRC section.
 *
 * The returning string will contain a JSON object, but without
 * the outer {}.  If the input JSON isn't a JSON object (dict), then
 * one will be created with the input added to a 'SRC Details' key.
 *
 * @param[in] json - The JSON to convert to a string
 *
 * @return std::string - The JSON string
 */
std::string prettyJSON(const fifoJSON& json)
{
    fifoJSON output;
    if (!json.is_object())
    {
        output["SRC Details"] = json;
    }
    else
    {
        for (const auto& [key, value] : json.items())
        {
            output[key] = value;
        }
    }

    // Let nlohmann do the pretty printing.
    std::stringstream stream;
    stream << std::setw(4) << output;

    auto jsonString = stream.str();

    // Now it looks like:
    // {
    //     "Key": "Value",
    //     ...
    // }

    // Replace the { and the following newline, and the } and its
    // preceeding newline.
    jsonString.erase(0, 2);

    auto pos = jsonString.find_last_of('}');
    jsonString.erase(pos - 1);

    return jsonString;
}

/**
 * @brief Call Python modules to parse the data into a JSON string
 *
 * The module to call is based on the Creator Subsystem ID under the namespace
 * "srcparsers". For example: "srcparsers.xsrc.xsrc" where "x" is the Creator
 * Subsystem ID in ASCII lowercase.
 *
 * All modules must provide the following:
 * Function: parseSRCToJson
 * Argument list:
 *    1. (str) ASCII string (Hex Word 1)
 *    2. (str) Hex Word 2
 *    3. (str) Hex Word 3
 *    4. (str) Hex Word 4
 *    5. (str) Hex Word 5
 *    6. (str) Hex Word 6
 *    7. (str) Hex Word 7
 *    8. (str) Hex Word 8
 *    9. (str) Hex Word 9
 *-Return data:
 *    1. (str) JSON string
 *
 * @param[in] hexwords - Vector of strings of Hexwords 1-9
 * @param[in] creatorID - The creatorID from the Private Header section
 * @return std::optional<std::string> - The JSON string if it could be created,
 *                                      else std::nullopt
 */
std::optional<std::string> getPythonJSON(std::vector<std::string>& hexwords,
                                         uint8_t creatorID)
{
    PyObject *pName, *pModule, *pDict, *pFunc, *pArgs, *pResult, *pBytes,
        *eType, *eValue, *eTraceback;
    std::string pErrStr;
    std::string module = getNumberString("%c", tolower(creatorID)) + "src";
    pName = PyUnicode_FromString(
        std::string("srcparsers." + module + "." + module).c_str());
    std::unique_ptr<PyObject, decltype(&pyDecRef)> modNamePtr(pName, &pyDecRef);
    pModule = PyImport_Import(pName);
    std::unique_ptr<PyObject, decltype(&pyDecRef)> modPtr(pModule, &pyDecRef);
    if (pModule == NULL)
    {
        pErrStr = "No error string found";
        PyErr_Fetch(&eType, &eValue, &eTraceback);
        if (eValue)
        {
            PyObject* pStr = PyObject_Str(eValue);
            if (pStr)
            {
                pErrStr = PyUnicode_AsUTF8(pStr);
            }
            Py_XDECREF(pStr);
        }
    }
    else
    {
        pDict = PyModule_GetDict(pModule);
        pFunc = PyDict_GetItemString(pDict, "parseSRCToJson");
        if (PyCallable_Check(pFunc))
        {
            pArgs = PyTuple_New(9);
            std::unique_ptr<PyObject, decltype(&pyDecRef)> argPtr(pArgs,
                                                                  &pyDecRef);
            for (size_t i = 0; i < 9; i++)
            {
                if (i < hexwords.size())
                {
                    auto arg = hexwords[i];
                    PyTuple_SetItem(pArgs, i,
                                    Py_BuildValue("s#", arg.c_str(), 8));
                }
                else
                {
                    PyTuple_SetItem(pArgs, i, Py_BuildValue("s", "00000000"));
                }
            }
            pResult = PyObject_CallObject(pFunc, pArgs);
            std::unique_ptr<PyObject, decltype(&pyDecRef)> resPtr(pResult,
                                                                  &pyDecRef);
            if (pResult)
            {
                pBytes = PyUnicode_AsEncodedString(pResult, "utf-8", "~E~");
                std::unique_ptr<PyObject, decltype(&pyDecRef)> pyBytePtr(
                    pBytes, &pyDecRef);
                const char* output = PyBytes_AS_STRING(pBytes);
                try
                {
                    fifoJSON json = nlohmann::json::parse(output);
                    return prettyJSON(json);
                }
                catch (std::exception& e)
                {
                    log<level::ERR>("Bad JSON from parser",
                                    entry("ERROR=%s", e.what()),
                                    entry("SRC=%s", hexwords.front().c_str()),
                                    entry("PARSER_MODULE=%s", module.c_str()));
                    return std::nullopt;
                }
            }
            else
            {
                pErrStr = "No error string found";
                PyErr_Fetch(&eType, &eValue, &eTraceback);
                if (eValue)
                {
                    PyObject* pStr = PyObject_Str(eValue);
                    if (pStr)
                    {
                        pErrStr = PyUnicode_AsUTF8(pStr);
                    }
                    Py_XDECREF(pStr);
                }
            }
        }
    }
    if (!pErrStr.empty())
    {
        log<level::ERR>("Python exception thrown by parser",
                        entry("ERROR=%s", pErrStr.c_str()),
                        entry("SRC=%s", hexwords.front().c_str()),
                        entry("PARSER_MODULE=%s", module.c_str()));
    }
    Py_XDECREF(eType);
    Py_XDECREF(eValue);
    Py_XDECREF(eTraceback);
    return std::nullopt;
}
#endif

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

SRC::SRC(const message::Entry& regEntry, const AdditionalData& additionalData,
         const nlohmann::json& jsonCallouts, const DataInterfaceBase& dataIface)
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

    // Hex Word 2 Nibbles:
    //   MIGVEPFF
    //   M: Partition dump status = 0
    //   I: System boot state = TODO
    //   G: Partition Boot type = 0
    //   V: BMC dump status = TODO
    //   E: Platform boot mode = 0 (side = temporary, speed = fast)
    //   P: Platform dump status = TODO
    //  FF: SRC format, set below

    setBMCFormat();
    setBMCPosition();
    setMotherboardCCIN(dataIface);

    // Fill in the last 4 words from the AdditionalData property contents.
    setUserDefinedHexWords(regEntry, additionalData);

    _asciiString = std::make_unique<src::AsciiString>(regEntry);

    addCallouts(regEntry, additionalData, jsonCallouts, dataIface);

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
            std::string msg =
                "SRC user data word out of range: " + std::to_string(wordNum);
            addDebugData(msg);
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
            std::string msg =
                "Source for user data SRC word not found: " + adName;
            addDebugData(msg);
        }
    }
}

void SRC::setMotherboardCCIN(const DataInterfaceBase& dataIface)
{
    uint32_t ccin = 0;
    auto ccinString = dataIface.getMotherboardCCIN();

    try
    {
        if (ccinString.size() == ccinSize)
        {
            ccin = std::stoi(ccinString, 0, 16);
        }
    }
    catch (std::exception& e)
    {
        log<level::WARNING>("Could not convert motherboard CCIN to a number",
                            entry("CCIN=%s", ccinString.c_str()));
        return;
    }

    // Set the first 2 bytes
    _hexData[1] |= ccin << 16;
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

bool SRC::isBMCSRC() const
{
    auto as = asciiString();
    if (as.length() >= 2)
    {
        uint8_t errorType = strtoul(as.substr(0, 2).c_str(), nullptr, 16);
        return (errorType == static_cast<uint8_t>(SRCType::bmcError) ||
                errorType == static_cast<uint8_t>(SRCType::powerError));
    }
    return false;
}

std::optional<std::string> SRC::getErrorDetails(message::Registry& registry,
                                                DetailLevel type,
                                                bool toCache) const
{
    const std::string jsonIndent(indentLevel, 0x20);
    std::string errorOut;
    if (isBMCSRC())
    {
        auto entry = registry.lookup("0x" + asciiString().substr(4, 4),
                                     rg::LookupType::reasonCode, toCache);
        if (entry)
        {
            errorOut.append(jsonIndent + "\"Error Details\": {\n");
            auto errorMsg = getErrorMessage(*entry);
            if (errorMsg)
            {
                if (type == DetailLevel::message)
                {
                    return errorMsg.value();
                }
                else
                {
                    jsonInsert(errorOut, "Message", errorMsg.value(), 2);
                }
            }
            if (entry->src.hexwordADFields)
            {
                std::map<size_t, std::string> adFields =
                    entry->src.hexwordADFields.value();
                for (const auto& hexwordMap : adFields)
                {
                    jsonInsert(errorOut, hexwordMap.second,
                               getNumberString("0x%X",
                                               _hexData[getWordIndexFromWordNum(
                                                   hexwordMap.first)]),
                               2);
                }
            }
            errorOut.erase(errorOut.size() - 2);
            errorOut.append("\n");
            errorOut.append(jsonIndent + "},\n");
            return errorOut;
        }
    }
    return std::nullopt;
}

std::optional<std::string>
    SRC::getErrorMessage(const message::Entry& regEntry) const
{
    try
    {
        if (regEntry.doc.messageArgSources)
        {
            size_t msgLen = regEntry.doc.message.length();
            char msg[msgLen + 1];
            strcpy(msg, regEntry.doc.message.c_str());
            std::vector<uint32_t> argSourceVals;
            std::string message;
            const auto& argValues = regEntry.doc.messageArgSources.value();
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
                        message.append(getNumberString(
                            "0x%X", argSourceVals[wordIndex - 1]));
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
            return message;
        }
        else
        {
            return regEntry.doc.message;
        }
    }
    catch (const std::exception& e)
    {
        log<level::ERR>("Cannot get error message from registry entry",
                        entry("ERROR=%s", e.what()));
    }
    return std::nullopt;
}

std::optional<std::string> SRC::getCallouts() const
{
    if (!_callouts)
    {
        return std::nullopt;
    }
    std::string printOut;
    const std::string jsonIndent(indentLevel, 0x20);
    const auto& callout = _callouts->callouts();
    const auto& compDescrp = pv::failingComponentType;
    printOut.append(jsonIndent + "\"Callout Section\": {\n");
    jsonInsert(printOut, "Callout Count", std::to_string(callout.size()), 2);
    printOut.append(jsonIndent + jsonIndent + "\"Callouts\": [");
    for (auto& entry : callout)
    {
        printOut.append("{\n");
        if (entry->fruIdentity())
        {
            jsonInsert(
                printOut, "FRU Type",
                compDescrp.at(entry->fruIdentity()->failingComponentType()), 3);
            jsonInsert(printOut, "Priority",
                       pv::getValue(entry->priority(),
                                    pel_values::calloutPriorityValues),
                       3);
            if (!entry->locationCode().empty())
            {
                jsonInsert(printOut, "Location Code", entry->locationCode(), 3);
            }
            if (entry->fruIdentity()->getPN().has_value())
            {
                jsonInsert(printOut, "Part Number",
                           entry->fruIdentity()->getPN().value(), 3);
            }
            if (entry->fruIdentity()->getMaintProc().has_value())
            {
                jsonInsert(printOut, "Procedure",
                           entry->fruIdentity()->getMaintProc().value(), 3);
                if (pv::procedureDesc.find(
                        entry->fruIdentity()->getMaintProc().value()) !=
                    pv::procedureDesc.end())
                {
                    jsonInsert(
                        printOut, "Description",
                        pv::procedureDesc.at(
                            entry->fruIdentity()->getMaintProc().value()),
                        3);
                }
            }
            if (entry->fruIdentity()->getCCIN().has_value())
            {
                jsonInsert(printOut, "CCIN",
                           entry->fruIdentity()->getCCIN().value(), 3);
            }
            if (entry->fruIdentity()->getSN().has_value())
            {
                jsonInsert(printOut, "Serial Number",
                           entry->fruIdentity()->getSN().value(), 3);
            }
        }
        if (entry->pceIdentity())
        {
            const auto& pceIdentMtms = entry->pceIdentity()->mtms();
            if (!pceIdentMtms.machineTypeAndModel().empty())
            {
                jsonInsert(printOut, "PCE MTMS",
                           pceIdentMtms.machineTypeAndModel() + "_" +
                               pceIdentMtms.machineSerialNumber(),
                           3);
            }
            if (!entry->pceIdentity()->enclosureName().empty())
            {
                jsonInsert(printOut, "PCE Name",
                           entry->pceIdentity()->enclosureName(), 3);
            }
        }
        if (entry->mru())
        {
            const auto& mruCallouts = entry->mru()->mrus();
            std::string mruId;
            for (auto& element : mruCallouts)
            {
                if (!mruId.empty())
                {
                    mruId.append(", " + getNumberString("%08X", element.id));
                }
                else
                {
                    mruId.append(getNumberString("%08X", element.id));
                }
            }
            jsonInsert(printOut, "MRU Id", mruId, 3);
        }
        printOut.erase(printOut.size() - 2);
        printOut.append("\n" + jsonIndent + jsonIndent + "}, ");
    };
    printOut.erase(printOut.size() - 2);
    printOut.append("]\n" + jsonIndent + "}");
    return printOut;
}

std::optional<std::string> SRC::getJSON(message::Registry& registry,
                                        const std::vector<std::string>& plugins,
                                        uint8_t creatorID) const
{
    std::string ps;
    std::vector<std::string> hexwords;
    jsonInsert(ps, pv::sectionVer, getNumberString("%d", _header.version), 1);
    jsonInsert(ps, pv::subSection, getNumberString("%d", _header.subType), 1);
    jsonInsert(ps, pv::createdBy, getNumberString("0x%X", _header.componentID),
               1);
    jsonInsert(ps, "SRC Version", getNumberString("0x%02X", _version), 1);
    jsonInsert(ps, "SRC Format", getNumberString("0x%02X", _hexData[0] & 0xFF),
               1);
    jsonInsert(ps, "Virtual Progress SRC",
               pv::boolString.at(_flags & virtualProgressSRC), 1);
    jsonInsert(ps, "I5/OS Service Event Bit",
               pv::boolString.at(_flags & i5OSServiceEventBit), 1);
    jsonInsert(ps, "Hypervisor Dump Initiated",
               pv::boolString.at(_flags & hypDumpInit), 1);
    jsonInsert(ps, "Power Control Net Fault",
               pv::boolString.at(isPowerFaultEvent()), 1);

    if (isBMCSRC())
    {
        std::string ccinString;
        uint32_t ccin = _hexData[1] >> 16;

        if (ccin)
        {
            ccinString = getNumberString("%04X", ccin);
        }
        // The PEL spec calls it a backplane, so call it that here.
        jsonInsert(ps, "Backplane CCIN", ccinString, 1);

        jsonInsert(ps, "Deconfigured",
                   pv::boolString.at(
                       _hexData[3] &
                       static_cast<uint32_t>(ErrorStatusFlags::deconfigured)),
                   1);

        jsonInsert(
            ps, "Guarded",
            pv::boolString.at(_hexData[3] &
                              static_cast<uint32_t>(ErrorStatusFlags::guarded)),
            1);
    }

    auto errorDetails = getErrorDetails(registry, DetailLevel::json, true);
    if (errorDetails)
    {
        ps.append(errorDetails.value());
    }
    jsonInsert(ps, "Valid Word Count", getNumberString("0x%02X", _wordCount),
               1);
    std::string refcode = asciiString();
    hexwords.push_back(refcode);
    std::string extRefcode;
    size_t pos = refcode.find(0x20);
    if (pos != std::string::npos)
    {
        size_t nextPos = refcode.find_first_not_of(0x20, pos);
        if (nextPos != std::string::npos)
        {
            extRefcode = trimEnd(refcode.substr(nextPos));
        }
        refcode.erase(pos);
    }
    jsonInsert(ps, "Reference Code", refcode, 1);
    if (!extRefcode.empty())
    {
        jsonInsert(ps, "Extended Reference Code", extRefcode, 1);
    }
    for (size_t i = 2; i <= _wordCount; i++)
    {
        std::string tmpWord =
            getNumberString("%08X", _hexData[getWordIndexFromWordNum(i)]);
        jsonInsert(ps, "Hex Word " + std::to_string(i), tmpWord, 1);
        hexwords.push_back(tmpWord);
    }
    auto calloutJson = getCallouts();
    if (calloutJson)
    {
        ps.append(calloutJson.value());
        ps.append(",\n");
    }
    std::string subsystem = getNumberString("%c", tolower(creatorID));
    bool srcDetailExists = false;
#ifdef PELTOOL
    if (std::find(plugins.begin(), plugins.end(), subsystem + "src") !=
        plugins.end())
    {
        auto pyJson = getPythonJSON(hexwords, creatorID);
        if (pyJson)
        {
            ps.append(pyJson.value());
            srcDetailExists = true;
        }
    }
#endif
    if (!srcDetailExists)
    {
        ps.erase(ps.size() - 2);
    }
    return ps;
}

void SRC::addCallouts(const message::Entry& regEntry,
                      const AdditionalData& additionalData,
                      const nlohmann::json& jsonCallouts,
                      const DataInterfaceBase& dataIface)
{
    auto registryCallouts =
        getRegistryCallouts(regEntry, additionalData, dataIface);

    auto item = additionalData.getValue("CALLOUT_INVENTORY_PATH");
<<<<<<< HEAD

    // If the first registry callout says to use the passed in inventory
    // path to get the location code for a symbolic FRU callout with a
    // trusted location code, then do not add the inventory path as a
    // normal FRU callout.
    bool useInvForSymbolicFRULocCode =
        !registryCallouts.empty() && registryCallouts[0].useInventoryLocCode &&
        !registryCallouts[0].symbolicFRUTrusted.empty();

    if (item && !useInvForSymbolicFRULocCode)
=======
    auto priority = additionalData.getValue("CALLOUT_PRIORITY");

    std::optional<CalloutPriority> calloutPriority;

    // Only  H, M or L priority values.
    if (priority && !(*priority).empty())
    {
        uint8_t p = (*priority)[0];
        if (p == 'H' || p == 'M' || p == 'L')
        {
            calloutPriority = static_cast<CalloutPriority>(p);
        }
    }

    if (item)
>>>>>>> Specify PEL callout priority and sort callouts.
    {
        addInventoryCallout(*item, calloutPriority, std::nullopt, dataIface);
    }

    addDevicePathCallouts(additionalData, dataIface);

    addRegistryCallouts(registryCallouts, dataIface,
                        (useInvForSymbolicFRULocCode) ? item : std::nullopt);

    if (!jsonCallouts.empty())
    {
        addJSONCallouts(jsonCallouts, dataIface);
    }
}

void SRC::addInventoryCallout(const std::string& inventoryPath,
                              const std::optional<CalloutPriority>& priority,
                              const std::optional<std::string>& locationCode,
                              const DataInterfaceBase& dataIface,
                              const std::vector<src::MRU::MRUCallout>& mrus)
{
    std::string locCode;
    std::string fn;
    std::string ccin;
    std::string sn;
    std::unique_ptr<src::Callout> callout;

    try
    {
        // Use the passed in location code if there otherwise look it up
        if (locationCode)
        {
            locCode = *locationCode;
        }
        else
        {
            locCode = dataIface.getLocationCode(inventoryPath);
        }

        try
        {
            dataIface.getHWCalloutFields(inventoryPath, fn, ccin, sn);

            CalloutPriority p =
                priority ? priority.value() : CalloutPriority::high;

            callout =
                std::make_unique<src::Callout>(p, locCode, fn, ccin, sn, mrus);
        }
        catch (const SdBusError& e)
        {
            std::string msg =
                "No VPD found for " + inventoryPath + ": " + e.what();
            addDebugData(msg);

            // Just create the callout with empty FRU fields
            callout = std::make_unique<src::Callout>(
                CalloutPriority::high, locCode, fn, ccin, sn, mrus);
        }
    }
    catch (const SdBusError& e)
    {
        std::string msg = "Could not get location code for " + inventoryPath +
                          ": " + e.what();
        addDebugData(msg);

        callout = std::make_unique<src::Callout>(CalloutPriority::high,
                                                 "no_vpd_for_fru");
    }

    createCalloutsObject();
    _callouts->addCallout(std::move(callout));
}

std::vector<message::RegistryCallout>
    SRC::getRegistryCallouts(const message::Entry& regEntry,
                             const AdditionalData& additionalData,
                             const DataInterfaceBase& dataIface)
{
    std::vector<message::RegistryCallout> registryCallouts;

    if (regEntry.callouts)
    {
        try
        {
            auto systemNames = dataIface.getSystemNames();

            registryCallouts = message::Registry::getCallouts(
                regEntry.callouts.value(), systemNames, additionalData);
        }
        catch (const std::exception& e)
        {
            addDebugData(fmt::format(
                "Error parsing PEL message registry callout JSON: {}",
                e.what()));
        }
    }

    return registryCallouts;
}

void SRC::addRegistryCallouts(
    const std::vector<message::RegistryCallout>& callouts,
    const DataInterfaceBase& dataIface,
    std::optional<std::string> trustedSymbolicFRUInvPath)
{
    try
    {
        for (const auto& callout : callouts)
        {
            addRegistryCallout(callout, dataIface, trustedSymbolicFRUInvPath);

            // Only the first callout gets the inventory path
            if (trustedSymbolicFRUInvPath)
            {
                trustedSymbolicFRUInvPath = std::nullopt;
            }
        }
    }
    catch (std::exception& e)
    {
        std::string msg =
            "Error parsing PEL message registry callout JSON: "s + e.what();
        addDebugData(msg);
    }
}

void SRC::addRegistryCallout(
    const message::RegistryCallout& regCallout,
    const DataInterfaceBase& dataIface,
    const std::optional<std::string>& trustedSymbolicFRUInvPath)
{
    std::unique_ptr<src::Callout> callout;
    auto locCode = regCallout.locCode;

    if (!locCode.empty())
    {
        try
        {
            locCode = dataIface.expandLocationCode(locCode, 0);
        }
        catch (const std::exception& e)
        {
            auto msg =
                "Unable to expand location code " + locCode + ": " + e.what();
            addDebugData(msg);
            return;
        }
    }

    // Via the PEL values table, get the priority enum.
    // The schema will have validated the priority was a valid value.
    auto priorityIt =
        pv::findByName(regCallout.priority, pv::calloutPriorityValues);
    assert(priorityIt != pv::calloutPriorityValues.end());
    auto priority =
        static_cast<CalloutPriority>(std::get<pv::fieldValuePos>(*priorityIt));

    if (!regCallout.procedure.empty())
    {
        // Procedure callout
        callout =
            std::make_unique<src::Callout>(priority, regCallout.procedure);
    }
    else if (!regCallout.symbolicFRU.empty())
    {
        // Symbolic FRU callout
        callout = std::make_unique<src::Callout>(
            priority, regCallout.symbolicFRU, locCode, false);
    }
    else if (!regCallout.symbolicFRUTrusted.empty())
    {
        // Symbolic FRU with trusted location code callout

        // Use the location code from the inventory path if there is one.
        if (trustedSymbolicFRUInvPath)
        {
            try
            {
                locCode = dataIface.getLocationCode(*trustedSymbolicFRUInvPath);
            }
            catch (const std::exception& e)
            {
                addDebugData(
                    fmt::format("Could not get location code for {}: {}",
                                *trustedSymbolicFRUInvPath, e.what()));
                locCode.clear();
            }
        }

        // The registry wants it to be trusted, but that requires a valid
        // location code for it to actually be.
        callout = std::make_unique<src::Callout>(
            priority, regCallout.symbolicFRUTrusted, locCode, !locCode.empty());
    }
    else
    {
        // A hardware callout
        std::string inventoryPath;

        try
        {
            // Get the inventory item from the unexpanded location code
            inventoryPath =
                dataIface.getInventoryFromLocCode(regCallout.locCode, 0, false);
        }
        catch (const std::exception& e)
        {
            std::string msg =
                "Unable to get inventory path from location code: " + locCode +
                ": " + e.what();
            addDebugData(msg);
            return;
        }

        addInventoryCallout(inventoryPath, priority, locCode, dataIface);
    }

    if (callout)
    {
        createCalloutsObject();
        _callouts->addCallout(std::move(callout));
    }
}

void SRC::addDevicePathCallouts(const AdditionalData& additionalData,
                                const DataInterfaceBase& dataIface)
{
    std::vector<device_callouts::Callout> callouts;
    auto i2cBus = additionalData.getValue("CALLOUT_IIC_BUS");
    auto i2cAddr = additionalData.getValue("CALLOUT_IIC_ADDR");
    auto devPath = additionalData.getValue("CALLOUT_DEVICE_PATH");

    // A device callout contains either:
    // * CALLOUT_ERRNO, CALLOUT_DEVICE_PATH
    // * CALLOUT_ERRNO, CALLOUT_IIC_BUS, CALLOUT_IIC_ADDR
    // We don't care about the errno.

    if (devPath)
    {
        try
        {
            callouts = device_callouts::getCallouts(*devPath,
                                                    dataIface.getSystemNames());
        }
        catch (const std::exception& e)
        {
            addDebugData(e.what());
            callouts.clear();
        }
    }
    else if (i2cBus && i2cAddr)
    {
        size_t bus;
        uint8_t address;

        try
        {
            // If /dev/i2c- is prepended, remove it
            if (i2cBus->find("/dev/i2c-") != std::string::npos)
            {
                *i2cBus = i2cBus->substr(9);
            }

            bus = stoul(*i2cBus, nullptr, 0);
            address = stoul(*i2cAddr, nullptr, 0);
        }
        catch (const std::exception& e)
        {
            std::string msg = "Invalid CALLOUT_IIC_BUS " + *i2cBus +
                              " or CALLOUT_IIC_ADDR " + *i2cAddr +
                              " in AdditionalData property";
            addDebugData(msg);
            return;
        }

        try
        {
            callouts = device_callouts::getI2CCallouts(
                bus, address, dataIface.getSystemNames());
        }
        catch (const std::exception& e)
        {
            addDebugData(e.what());
            callouts.clear();
        }
    }

    for (const auto& callout : callouts)
    {
        // The priority shouldn't be invalid, but check just in case.
        CalloutPriority priority = CalloutPriority::high;

        if (!callout.priority.empty())
        {
            auto p = pel_values::findByValue(
                static_cast<uint32_t>(callout.priority[0]),
                pel_values::calloutPriorityValues);

            if (p != pel_values::calloutPriorityValues.end())
            {
                priority = static_cast<CalloutPriority>(callout.priority[0]);
            }
            else
            {
                std::string msg =
                    "Invalid priority found in dev callout JSON: " +
                    callout.priority[0];
                addDebugData(msg);
            }
        }

        try
        {
            auto inventoryPath = dataIface.getInventoryFromLocCode(
                callout.locationCode, 0, false);

            addInventoryCallout(inventoryPath, priority, std::nullopt,
                                dataIface);
        }
        catch (const std::exception& e)
        {
            std::string msg =
                "Unable to get inventory path from location code: " +
                callout.locationCode + ": " + e.what();
            addDebugData(msg);
        }

        // Until the code is there to convert these MRU value strings to
        // the official MRU values in the callout objects, just store
        // the MRU name in the debug UserData section.
        if (!callout.mru.empty())
        {
            std::string msg = "MRU: " + callout.mru;
            addDebugData(msg);
        }

        // getCallouts() may have generated some debug data it stored
        // in a callout object.  Save it as well.
        if (!callout.debug.empty())
        {
            addDebugData(callout.debug);
        }
    }
}

void SRC::addJSONCallouts(const nlohmann::json& jsonCallouts,
                          const DataInterfaceBase& dataIface)
{
    if (jsonCallouts.empty())
    {
        return;
    }

    if (!jsonCallouts.is_array())
    {
        addDebugData("Callout JSON isn't an array");
        return;
    }

    for (const auto& callout : jsonCallouts)
    {
        try
        {
            addJSONCallout(callout, dataIface);
        }
        catch (const std::exception& e)
        {
            addDebugData(fmt::format(
                "Failed extracting callout data from JSON: {}", e.what()));
        }
    }
}

void SRC::addJSONCallout(const nlohmann::json& jsonCallout,
                         const DataInterfaceBase& dataIface)
{
    auto priority = getPriorityFromJSON(jsonCallout);
    std::string locCode;
    std::string unexpandedLocCode;
    std::unique_ptr<src::Callout> callout;

    // Expand the location code if it's there
    if (jsonCallout.contains("LocationCode"))
    {
        unexpandedLocCode = jsonCallout.at("LocationCode").get<std::string>();

        try
        {
            locCode = dataIface.expandLocationCode(unexpandedLocCode, 0);
        }
        catch (const std::exception& e)
        {
            addDebugData(fmt::format("Unable to expand location code {}: {}",
                                     unexpandedLocCode, e.what()));
            // Use the value from the JSON so at least there's something
            locCode = unexpandedLocCode;
        }
    }

    // Create either a procedure, symbolic FRU, or normal FRU callout.
    if (jsonCallout.contains("Procedure"))
    {
        auto procedure = jsonCallout.at("Procedure").get<std::string>();

        callout = std::make_unique<src::Callout>(
            static_cast<CalloutPriority>(priority), procedure,
            src::CalloutValueType::raw);
    }
    else if (jsonCallout.contains("SymbolicFRU"))
    {
        auto fru = jsonCallout.at("SymbolicFRU").get<std::string>();

        bool trusted = false;
        if (jsonCallout.contains("TrustedLocationCode") && !locCode.empty())
        {
            trusted = jsonCallout.at("TrustedLocationCode").get<bool>();
        }

        callout = std::make_unique<src::Callout>(
            static_cast<CalloutPriority>(priority), fru,
            src::CalloutValueType::raw, locCode, trusted);
    }
    else
    {
        // A hardware FRU
        std::string inventoryPath;
        std::vector<src::MRU::MRUCallout> mrus;

        if (jsonCallout.contains("InventoryPath"))
        {
            inventoryPath = jsonCallout.at("InventoryPath").get<std::string>();
        }
        else
        {
            if (unexpandedLocCode.empty())
            {
                throw std::runtime_error{"JSON callout needs either an "
                                         "inventory path or location code"};
            }

            try
            {
                inventoryPath = dataIface.getInventoryFromLocCode(
                    unexpandedLocCode, 0, false);
            }
            catch (const std::exception& e)
            {
                throw std::runtime_error{
                    fmt::format("Unable to get inventory path from "
                                "location code: {}: {}",
                                unexpandedLocCode, e.what())};
            }
        }

        if (jsonCallout.contains("MRUs"))
        {
            mrus = getMRUsFromJSON(jsonCallout.at("MRUs"));
        }

        // If the location code was also passed in, use that here too
        // so addInventoryCallout doesn't have to look it up.
        std::optional<std::string> lc;
        if (!locCode.empty())
        {
            lc = locCode;
        }

        addInventoryCallout(inventoryPath, priority, lc, dataIface, mrus);

        if (jsonCallout.contains("Deconfigured"))
        {
            if (jsonCallout.at("Deconfigured").get<bool>())
            {
                setErrorStatusFlag(ErrorStatusFlags::deconfigured);
            }
        }

        if (jsonCallout.contains("Guarded"))
        {
            if (jsonCallout.at("Guarded").get<bool>())
            {
                setErrorStatusFlag(ErrorStatusFlags::guarded);
            }
        }
    }

    if (callout)
    {
        createCalloutsObject();
        _callouts->addCallout(std::move(callout));
    }
}

CalloutPriority SRC::getPriorityFromJSON(const nlohmann::json& json)
{
    // Looks like:
    // {
    //     "Priority": "H"
    // }
    auto p = json.at("Priority").get<std::string>();
    if (p.empty())
    {
        throw std::runtime_error{"Priority field in callout is empty"};
    }

    auto priority = static_cast<CalloutPriority>(p.front());

    // Validate it
    auto priorityIt = pv::findByValue(static_cast<uint32_t>(priority),
                                      pv::calloutPriorityValues);
    if (priorityIt == pv::calloutPriorityValues.end())
    {
        throw std::runtime_error{
            fmt::format("Invalid priority '{}' found in JSON callout", p)};
    }

    return priority;
}

std::vector<src::MRU::MRUCallout>
    SRC::getMRUsFromJSON(const nlohmann::json& mruJSON)
{
    std::vector<src::MRU::MRUCallout> mrus;

    // Looks like:
    // [
    //     {
    //         "ID": 100,
    //         "Priority": "H"
    //     }
    // ]
    if (!mruJSON.is_array())
    {
        addDebugData("MRU callout JSON is not an array");
        return mrus;
    }

    for (const auto& mruCallout : mruJSON)
    {
        try
        {
            auto priority = getPriorityFromJSON(mruCallout);
            auto id = mruCallout.at("ID").get<uint32_t>();

            src::MRU::MRUCallout mru{static_cast<uint32_t>(priority), id};
            mrus.push_back(std::move(mru));
        }
        catch (const std::exception& e)
        {
            addDebugData(fmt::format("Invalid MRU entry in JSON: {}: {}",
                                     mruCallout.dump(), e.what()));
        }
    }

    return mrus;
}

} // namespace pels
} // namespace openpower
