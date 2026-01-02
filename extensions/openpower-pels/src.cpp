// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "src.hpp"

#include "device_callouts.hpp"
#include "json_utils.hpp"
#include "pel_values.hpp"
#ifdef PELTOOL
#include <Python.h>

#include <nlohmann/json.hpp>

#include <sstream>
#endif
#include <phosphor-logging/lg2.hpp>

#include <format>

namespace openpower
{
namespace pels
{
namespace pv = openpower::pels::pel_values;
namespace rg = openpower::pels::message;
using namespace std::string_literals;

constexpr size_t ccinSize = 4;

#ifdef PELTOOL
using orderedJSON = nlohmann::ordered_json;

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
std::string prettyJSON(const orderedJSON& json)
{
    orderedJSON output;
    if (!json.is_object())
    {
        output["SRC Details"] = json;
    }
    else
    {
        for (const auto& [key, value] : json.items())
        {
            output["SRC Details"][key] = value;
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
    PyObject *pName, *pModule, *eType, *eValue, *eTraceback;
    std::string pErrStr;
    std::string module = getNumberString("%c", tolower(creatorID)) + "src";
    pName = PyUnicode_FromString(
        std::string("srcparsers." + module + "." + module).c_str());
    std::unique_ptr<PyObject, decltype(&pyDecRef)> modNamePtr(pName, &pyDecRef);
    pModule = PyImport_Import(pName);
    if (pModule == nullptr)
    {
        pErrStr = "No error string found";
        PyErr_Fetch(&eType, &eValue, &eTraceback);
        if (eType)
        {
            Py_XDECREF(eType);
        }
        if (eTraceback)
        {
            Py_XDECREF(eTraceback);
        }
        if (eValue)
        {
            PyObject* pStr = PyObject_Str(eValue);
            Py_XDECREF(eValue);
            if (pStr)
            {
                pErrStr = PyUnicode_AsUTF8(pStr);
                Py_XDECREF(pStr);
            }
        }
    }
    else
    {
        std::unique_ptr<PyObject, decltype(&pyDecRef)> modPtr(
            pModule, &pyDecRef);
        std::string funcToCall = "parseSRCToJson";
        PyObject* pKey = PyUnicode_FromString(funcToCall.c_str());
        std::unique_ptr<PyObject, decltype(&pyDecRef)> keyPtr(pKey, &pyDecRef);
        PyObject* pDict = PyModule_GetDict(pModule);
        Py_INCREF(pDict);
        if (!PyDict_Contains(pDict, pKey))
        {
            Py_DECREF(pDict);
            lg2::error(
                "Python module error.  Function missing: {FUNC}, SRC = {SRC}, module = {MODULE}",
                "FUNC", funcToCall, "SRC", hexwords.front(), "MODULE", module);
            return std::nullopt;
        }
        PyObject* pFunc = PyDict_GetItemString(pDict, funcToCall.c_str());
        Py_DECREF(pDict);
        Py_INCREF(pFunc);
        if (PyCallable_Check(pFunc))
        {
            PyObject* pArgs = PyTuple_New(9);
            std::unique_ptr<PyObject, decltype(&pyDecRef)> argPtr(
                pArgs, &pyDecRef);
            for (size_t i = 0; i < 9; i++)
            {
                std::string arg{"00000000"};
                if (i < hexwords.size())
                {
                    arg = hexwords[i];
                }
                PyTuple_SetItem(pArgs, i, Py_BuildValue("s", arg.c_str()));
            }
            PyObject* pResult = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pFunc);
            if (pResult)
            {
                std::unique_ptr<PyObject, decltype(&pyDecRef)> resPtr(
                    pResult, &pyDecRef);

                if (pResult == Py_None)
                {
                    return std::nullopt;
                }

                PyObject* pBytes =
                    PyUnicode_AsEncodedString(pResult, "utf-8", "~E~");
                std::unique_ptr<PyObject, decltype(&pyDecRef)> pyBytePtr(
                    pBytes, &pyDecRef);
                const char* output = PyBytes_AS_STRING(pBytes);
                try
                {
                    orderedJSON json = orderedJSON::parse(output);
                    if ((json.is_object() && !json.empty()) ||
                        (json.is_array() && json.size() > 0) ||
                        (json.is_string() && json != ""))
                    {
                        return prettyJSON(json);
                    }
                }
                catch (const std::exception& e)
                {
                    lg2::error(
                        "Bad JSON from parser. Error = {ERROR}, SRC = {SRC}, module = {MODULE}",
                        "ERROR", e, "SRC", hexwords.front(), "MODULE", module);
                    return std::nullopt;
                }
            }
            else
            {
                pErrStr = "No error string found";
                PyErr_Fetch(&eType, &eValue, &eTraceback);
                if (eType)
                {
                    Py_XDECREF(eType);
                }
                if (eTraceback)
                {
                    Py_XDECREF(eTraceback);
                }
                if (eValue)
                {
                    PyObject* pStr = PyObject_Str(eValue);
                    Py_XDECREF(eValue);
                    if (pStr)
                    {
                        pErrStr = PyUnicode_AsUTF8(pStr);
                        Py_XDECREF(pStr);
                    }
                }
            }
        }
    }
    if (!pErrStr.empty())
    {
        lg2::debug("Python exception thrown by parser. Error = {ERROR}, "
                   "SRC = {SRC}, module = {MODULE}",
                   "ERROR", pErrStr, "SRC", hexwords.front(), "MODULE", module);
    }
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
        lg2::error("Cannot unflatten SRC, error = {ERROR}", "ERROR", e);
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
    //   V: BMC dump status
    //   E: Platform boot mode = 0 (side = temporary, speed = fast)
    //   P: Platform dump status
    //  FF: SRC format, set below

    setProgressCode(dataIface);
    setBMCFormat();
    setBMCPosition();
    setMotherboardCCIN(dataIface);

    if (regEntry.src.checkstopFlag)
    {
        setErrorStatusFlag(ErrorStatusFlags::hwCheckstop);
    }

    if (regEntry.src.deconfigFlag)
    {
        setErrorStatusFlag(ErrorStatusFlags::deconfigured);
    }

    // Fill in the last 4 words from the AdditionalData property contents.
    setUserDefinedHexWords(regEntry, additionalData);

    _asciiString = std::make_unique<src::AsciiString>(regEntry);

    // Check for additional data - PEL_SUBSYSTEM
    auto ss = additionalData.getValue("PEL_SUBSYSTEM");
    if (ss)
    {
        auto eventSubsystem = std::stoul(*ss, nullptr, 16);
        std::string subsystem =
            pv::getValue(eventSubsystem, pel_values::subsystemValues);
        if (subsystem == "invalid")
        {
            lg2::warning("SRC: Invalid SubSystem value: {VAL}", "VAL", lg2::hex,
                         eventSubsystem);
        }
        else
        {
            _asciiString->setByte(2, eventSubsystem);
        }
    }

    addCallouts(regEntry, additionalData, jsonCallouts, dataIface);

    _size = baseSRCSize;
    _size += _callouts ? _callouts->flattenedSize() : 0;
    _header.size = Section::headerSize() + _size;

    _valid = true;
}

void SRC::setUserDefinedHexWords(const message::Entry& regEntry,
                                 const AdditionalData& ad)
{
    if (!regEntry.src.hexwordADFields)
    {
        return;
    }

    // Save the AdditionalData value corresponding to the first element of
    // adName tuple into _hexData[wordNum].
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

        auto value = ad.getValue(std::get<0>(adName));
        if (value)
        {
            _hexData[getWordIndexFromWordNum(wordNum)] =
                std::strtoul(value.value().c_str(), nullptr, 0);
        }
        else
        {
            std::string msg = "Source for user data SRC word not found: " +
                              std::get<0>(adName);
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
            ccin = std::stoi(ccinString, nullptr, 16);
        }
    }
    catch (const std::exception& e)
    {
        lg2::warning("Could not convert motherboard CCIN {CCIN} to a number",
                     "CCIN", ccinString);
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
        lg2::error("Invalid SRC section ID: {ID}", "ID", lg2::hex, header().id);
        failed = true;
    }

    // Check the version in the SRC, not in the header
    if (_version != srcVersion)
    {
        lg2::error("Invalid SRC version: {VERSION}", "VERSION", lg2::hex,
                   header().version);
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

bool SRC::isHostbootSRC() const
{
    auto as = asciiString();
    if (as.length() >= 2)
    {
        uint8_t errorType = strtoul(as.substr(0, 2).c_str(), nullptr, 16);
        return errorType == static_cast<uint8_t>(SRCType::hostbootError);
    }
    return false;
}

std::optional<std::string> SRC::getErrorDetails(
    message::Registry& registry, DetailLevel type, bool toCache) const
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
                std::map<size_t, std::tuple<std::string, std::string>>
                    adFields = entry->src.hexwordADFields.value();
                for (const auto& hexwordMap : adFields)
                {
                    auto srcValue = getNumberString(
                        "0x%X",
                        _hexData[getWordIndexFromWordNum(hexwordMap.first)]);

                    auto srcKey = std::get<0>(hexwordMap.second);
                    auto srcDesc = std::get<1>(hexwordMap.second);

                    // Only include this hex word in the error details if the
                    // description exists.
                    if (!srcDesc.empty())
                    {
                        std::vector<std::string> valueDescr;
                        valueDescr.push_back(srcValue);
                        valueDescr.push_back(srcDesc);
                        jsonInsertArray(errorOut, srcKey, valueDescr, 2);
                    }
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

std::optional<std::string> SRC::getErrorMessage(
    const message::Entry& regEntry) const
{
    try
    {
        if (regEntry.doc.messageArgSources)
        {
            std::vector<uint32_t> argSourceVals;
            std::string message;
            const auto& argValues = regEntry.doc.messageArgSources.value();
            for (size_t i = 0; i < argValues.size(); ++i)
            {
                argSourceVals.push_back(_hexData[getWordIndexFromWordNum(
                    argValues[i].back() - '0')]);
            }

            auto it = std::begin(regEntry.doc.message);
            auto it_end = std::end(regEntry.doc.message);

            while (it != it_end)
            {
                if (*it == '%')
                {
                    ++it;

                    size_t wordIndex = *it - '0';
                    if (isdigit(*it) && wordIndex >= 1 &&
                        static_cast<uint16_t>(wordIndex) <=
                            argSourceVals.size())
                    {
                        message.append(getNumberString(
                            "0x%08X", argSourceVals[wordIndex - 1]));
                    }
                    else
                    {
                        message.append("%" + std::string(1, *it));
                    }
                }
                else
                {
                    message.push_back(*it);
                }
                ++it;
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
        lg2::error(
            "Cannot get error message from registry entry, error = {ERROR}",
            "ERROR", e);
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

            auto pn = entry->fruIdentity()->getPN();
            if (pn.has_value())
            {
                jsonInsert(printOut, "Part Number", pn.value(), 3);
            }

            auto proc = entry->fruIdentity()->getMaintProc();
            if (proc.has_value())
            {
                jsonInsert(printOut, "Procedure", proc.value(), 3);
                if (pv::procedureDesc.find(proc.value()) !=
                    pv::procedureDesc.end())
                {
                    jsonInsert(printOut, "Description",
                               pv::procedureDesc.at(proc.value()), 3);
                }
            }

            auto ccin = entry->fruIdentity()->getCCIN();
            if (ccin.has_value())
            {
                jsonInsert(printOut, "CCIN", ccin.value(), 3);
            }

            auto sn = entry->fruIdentity()->getSN();
            if (sn.has_value())
            {
                jsonInsert(printOut, "Serial Number", sn.value(), 3);
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
                                        const std::vector<std::string>& plugins
                                        [[maybe_unused]],
                                        uint8_t creatorID) const
{
    std::string ps;
    std::vector<std::string> hexwords;
    jsonInsert(ps, pv::sectionVer, getNumberString("%d", _header.version), 1);
    jsonInsert(ps, pv::subSection, getNumberString("%d", _header.subType), 1);
    jsonInsert(ps, pv::createdBy,
               getComponentName(_header.componentID, creatorID), 1);
    jsonInsert(ps, "SRC Version", getNumberString("0x%02X", _version), 1);
    jsonInsert(ps, "SRC Format", getNumberString("0x%02X", _hexData[0] & 0xFF),
               1);
    jsonInsert(ps, "Virtual Progress SRC",
               pv::boolString.at(_flags & virtualProgressSRC), 1);
    jsonInsert(ps, "I5/OS Service Event Bit",
               pv::boolString.at(_flags & i5OSServiceEventBit), 1);
    jsonInsert(ps, "Hypervisor Dump Initiated",
               pv::boolString.at(_flags & hypDumpInit), 1);

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

        jsonInsert(ps, "Terminate FW Error",
                   pv::boolString.at(
                       _hexData[3] &
                       static_cast<uint32_t>(ErrorStatusFlags::terminateFwErr)),
                   1);
    }

    if (isBMCSRC() || isHostbootSRC())
    {
        jsonInsert(ps, "Deconfigured",
                   pv::boolString.at(
                       _hexData[3] &
                       static_cast<uint32_t>(ErrorStatusFlags::deconfigured)),
                   1);

        jsonInsert(
            ps, "Guarded",
            pv::boolString.at(
                _hexData[3] & static_cast<uint32_t>(ErrorStatusFlags::guarded)),
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
    // If the first registry callout says to use the passed in inventory
    // path to get the location code for a symbolic FRU callout with a
    // trusted location code, then do not add the inventory path as a
    // normal FRU callout.
    bool useInvForSymbolicFRULocCode =
        !registryCallouts.empty() && registryCallouts[0].useInventoryLocCode &&
        !registryCallouts[0].symbolicFRUTrusted.empty();

    if (item && !useInvForSymbolicFRULocCode)
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

void SRC::addLocationCodeOnlyCallout(const std::string& locationCode,
                                     const CalloutPriority priority)
{
    std::string empty;
    std::vector<src::MRU::MRUCallout> mrus;
    auto callout = std::make_unique<src::Callout>(priority, locationCode, empty,
                                                  empty, empty, mrus);
    createCalloutsObject();
    _callouts->addCallout(std::move(callout));
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
        catch (const sdbusplus::exception_t& e)
        {
            std::string msg =
                "No VPD found for " + inventoryPath + ": " + e.what();
            addDebugData(msg);

            // Just create the callout with empty FRU fields
            callout = std::make_unique<src::Callout>(
                CalloutPriority::high, locCode, fn, ccin, sn, mrus);
        }
    }
    catch (const sdbusplus::exception_t& e)
    {
        std::string msg = "Could not get location code for " + inventoryPath +
                          ": " + e.what();
        addDebugData(msg);

        // Don't add a callout in this case, because:
        // 1) With how the inventory is primed, there is no case where
        //    a location code is expected to be missing.  This implies
        //    the caller is passing in something invalid.
        // 2) The addDebugData call above will put the passed in path into
        //    a user data section that can be seen by development for debug.
        // 3) Even if we wanted to do a 'no_vpd_for_fru' sort of maint.
        //    procedure, we don't have a good way to indicate to the user
        //    anything about the intended callout (they won't see user data).
        // 4) Creating a new standalone event log for this problem isn't
        //    possible from inside a PEL section.
    }

    if (callout)
    {
        createCalloutsObject();
        _callouts->addCallout(std::move(callout));
    }
}

std::vector<message::RegistryCallout> SRC::getRegistryCallouts(
    const message::Entry& regEntry, const AdditionalData& additionalData,
    const DataInterfaceBase& dataIface)
{
    std::vector<message::RegistryCallout> registryCallouts;

    if (regEntry.callouts)
    {
        std::vector<std::string> systemNames;

        try
        {
            systemNames = dataIface.getSystemNames();
        }
        catch (const std::exception& e)
        {
            // Compatible interface not available yet
        }

        try
        {
            registryCallouts = message::Registry::getCallouts(
                regEntry.callouts.value(), systemNames, additionalData);
        }
        catch (const std::exception& e)
        {
            addDebugData(std::format(
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
    catch (const std::exception& e)
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
    bool locExpanded = true;

    if (!locCode.empty())
    {
        try
        {
            locCode = dataIface.expandLocationCode(locCode, 0);
        }
        catch (const std::exception& e)
        {
            auto msg = "Unable to expand location code " + locCode + ": " +
                       e.what();
            addDebugData(msg);
            locExpanded = false;
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
        callout = std::make_unique<src::Callout>(priority, regCallout.procedure,
                                                 src::CalloutValueType::raw);
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
        bool trusted = false;

        // Use the location code from the inventory path if there is one.
        if (trustedSymbolicFRUInvPath)
        {
            try
            {
                locCode = dataIface.getLocationCode(*trustedSymbolicFRUInvPath);
                trusted = true;
            }
            catch (const std::exception& e)
            {
                addDebugData(
                    std::format("Could not get location code for {}: {}",
                                *trustedSymbolicFRUInvPath, e.what()));
                locCode.clear();
            }
        }

        // Can only trust the location code if it isn't empty and is expanded.
        if (!locCode.empty() && locExpanded)
        {
            trusted = true;
        }

        // The registry wants it to be trusted, but that requires a valid
        // location code for it to actually be.
        callout = std::make_unique<src::Callout>(
            priority, regCallout.symbolicFRUTrusted, locCode, trusted);
    }
    else
    {
        // A hardware callout

        // If couldn't expand the location code, don't bother
        // looking up the inventory path.
        if (!locExpanded && !locCode.empty())
        {
            addLocationCodeOnlyCallout(locCode, priority);
            return;
        }

        std::vector<std::string> inventoryPaths;

        try
        {
            // Get the inventory item from the unexpanded location code
            inventoryPaths =
                dataIface.getInventoryFromLocCode(regCallout.locCode, 0, false);
        }
        catch (const std::exception& e)
        {
            std::string msg =
                "Unable to get inventory path from location code: " + locCode +
                ": " + e.what();
            addDebugData(msg);
            if (!locCode.empty())
            {
                // Still add a callout with just the location code.
                addLocationCodeOnlyCallout(locCode, priority);
            }
            return;
        }

        // Just use first path returned since they all point to the same FRU.
        addInventoryCallout(inventoryPaths[0], priority, locCode, dataIface);
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
            std::string msg =
                "Invalid CALLOUT_IIC_BUS " + *i2cBus + " or CALLOUT_IIC_ADDR " +
                *i2cAddr + " in AdditionalData property";
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
                auto msg =
                    std::string{
                        "Invalid priority found in dev callout JSON: "} +
                    callout.priority[0];
                addDebugData(msg);
            }
        }

        std::optional<std::string> locCode;

        try
        {
            locCode = dataIface.expandLocationCode(callout.locationCode, 0);
        }
        catch (const std::exception& e)
        {
            auto msg = std::format("Unable to expand location code {}: {}",
                                   callout.locationCode, e.what());
            addDebugData(msg);

            // Add the callout with just the unexpanded location code.
            addLocationCodeOnlyCallout(callout.locationCode, priority);
            continue;
        }

        try
        {
            auto inventoryPaths = dataIface.getInventoryFromLocCode(
                callout.locationCode, 0, false);

            // Just use first path returned since they all
            // point to the same FRU.
            addInventoryCallout(inventoryPaths[0], priority, locCode,
                                dataIface);
        }
        catch (const std::exception& e)
        {
            std::string msg =
                "Unable to get inventory path from location code: " +
                callout.locationCode + ": " + e.what();
            addDebugData(msg);
            // Add the callout with just the location code.
            addLocationCodeOnlyCallout(callout.locationCode, priority);
            continue;
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
            addDebugData(std::format(
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
            addDebugData(std::format("Unable to expand location code {}: {}",
                                     unexpandedLocCode, e.what()));
            // Use the value from the JSON so at least there's something
            locCode = unexpandedLocCode;
        }
    }

    // Create either a procedure, symbolic FRU, or normal FRU callout.
    if (jsonCallout.contains("Procedure"))
    {
        auto procedure = jsonCallout.at("Procedure").get<std::string>();

        // If it's the registry name instead of the raw name, convert.
        if (pv::maintenanceProcedures.find(procedure) !=
            pv::maintenanceProcedures.end())
        {
            procedure = pv::maintenanceProcedures.at(procedure);
        }

        callout = std::make_unique<src::Callout>(
            static_cast<CalloutPriority>(priority), procedure,
            src::CalloutValueType::raw);
    }
    else if (jsonCallout.contains("SymbolicFRU"))
    {
        auto fru = jsonCallout.at("SymbolicFRU").get<std::string>();

        // If it's the registry name instead of the raw name, convert.
        if (pv::symbolicFRUs.find(fru) != pv::symbolicFRUs.end())
        {
            fru = pv::symbolicFRUs.at(fru);
        }

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
                auto inventoryPaths = dataIface.getInventoryFromLocCode(
                    unexpandedLocCode, 0, false);
                // Just use first path returned since they all
                // point to the same FRU.
                inventoryPath = inventoryPaths[0];
            }
            catch (const std::exception& e)
            {
                addDebugData(std::format("Unable to get inventory path from "
                                         "location code: {}: {}",
                                         unexpandedLocCode, e.what()));
                addLocationCodeOnlyCallout(locCode, priority);
                return;
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
            std::format("Invalid priority '{}' found in JSON callout", p)};
    }

    return priority;
}

std::vector<src::MRU::MRUCallout> SRC::getMRUsFromJSON(
    const nlohmann::json& mruJSON)
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
            addDebugData(std::format("Invalid MRU entry in JSON: {}: {}",
                                     mruCallout.dump(), e.what()));
        }
    }

    return mrus;
}

std::vector<uint8_t> SRC::getSrcStruct()
{
    std::vector<uint8_t> data;
    Stream stream{data};

    //------ Ref section 4.3 in PEL doc---
    //------ SRC Structure 40 bytes-------
    // Byte-0 | Byte-1 | Byte-2 | Byte-3 |
    // -----------------------------------
    //   02   |   08   |   00   |   09   | ==> Header
    //   00   |   00   |   00   |   48   | ==> Header
    //   00   |   00   |   00   |   00   | ==> Hex data word-2
    //   00   |   00   |   00   |   00   | ==> Hex data word-3
    //   00   |   00   |   00   |   00   | ==> Hex data word-4
    //   20   |   00   |   00   |   00   | ==> Hex data word-5
    //   00   |   00   |   00   |   00   | ==> Hex data word-6
    //   00   |   00   |   00   |   00   | ==> Hex data word-7
    //   00   |   00   |   00   |   00   | ==> Hex data word-8
    //   00   |   00   |   00   |   00   | ==> Hex data word-9
    // -----------------------------------
    //   ASCII string - 8 bytes          |
    // -----------------------------------
    //   ASCII space NULL - 24 bytes     |
    // -----------------------------------
    //_size = Base SRC struct: 8 byte header + hex data section + ASCII string

    uint8_t flags = (_flags | postOPPanel);

    stream << _version << flags << _reserved1B << _wordCount << _reserved2B
           << _size;

    for (auto& word : _hexData)
    {
        stream << word;
    }

    _asciiString->flatten(stream);

    return data;
}

void SRC::setProgressCode(const DataInterfaceBase& dataIface)
{
    std::vector<uint8_t> progressSRC;

    try
    {
        progressSRC = dataIface.getRawProgressSRC();
    }
    catch (const std::exception& e)
    {
        lg2::error("Error getting progress code: {ERROR}", "ERROR", e);
        return;
    }

    _hexData[2] = getProgressCode(progressSRC);
}

uint32_t SRC::getProgressCode(std::vector<uint8_t>& rawProgressSRC)
{
    uint32_t progressCode = 0;

    // A valid progress SRC is at least 72 bytes
    if (rawProgressSRC.size() < 72)
    {
        return progressCode;
    }

    try
    {
        // The ASCII string field in progress SRCs starts at offset 40.
        // Take the first 8 characters to put in the uint32:
        //   "CC009189" -> 0xCC009189
        Stream stream{rawProgressSRC, 40};
        src::AsciiString aString{stream};
        auto progressCodeString = aString.get().substr(0, 8);

        if (std::all_of(progressCodeString.begin(), progressCodeString.end(),
                        [](char c) {
                            return std::isxdigit(static_cast<unsigned char>(c));
                        }))
        {
            progressCode = std::stoul(progressCodeString, nullptr, 16);
        }
    }
    catch (const std::exception& e)
    {}

    return progressCode;
}

} // namespace pels
} // namespace openpower
