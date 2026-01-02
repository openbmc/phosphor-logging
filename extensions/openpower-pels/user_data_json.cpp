// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2020 IBM Corporation

#include "user_data_json.hpp"

#include "json_utils.hpp"
#include "pel_types.hpp"
#include "pel_values.hpp"
#include "stream.hpp"
#include "user_data_formats.hpp"

#include <Python.h>

#include <nlohmann/json.hpp>
#include <phosphor-logging/lg2.hpp>

#include <iomanip>
#include <sstream>

namespace openpower::pels::user_data
{
namespace pv = openpower::pels::pel_values;
using orderedJSON = nlohmann::ordered_json;

void pyDecRef(PyObject* pyObj)
{
    Py_XDECREF(pyObj);
}

/**
 * @brief Returns a JSON string for use by PEL::printSectionInJSON().
 *
 * The returning string will contain a JSON object, but without
 * the outer {}.  If the input JSON isn't a JSON object (dict), then
 * one will be created with the input added to a 'Data' key.
 *
 * @param[in] creatorID - The creator ID for the PEL
 *
 * @param[in] json - The JSON to convert to a string
 *
 * @return std::string - The JSON string
 */
std::string prettyJSON(uint16_t componentID, uint8_t subType, uint8_t version,
                       uint8_t creatorID, const orderedJSON& json)
{
    orderedJSON output;
    output[pv::sectionVer] = std::to_string(version);
    output[pv::subSection] = std::to_string(subType);
    output[pv::createdBy] = getComponentName(componentID, creatorID);

    if (!json.is_object())
    {
        output["Data"] = json;
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
    //     "Section Version": ...
    //     ...
    // }

    // Since PEL::printSectionInJSON() will supply the outer { }s,
    // remove the existing ones.

    // Replace the { and the following newline, and the } and its
    // preceeding newline.
    jsonString.erase(0, 2);

    auto pos = jsonString.find_last_of('}');
    jsonString.erase(pos - 1);

    return jsonString;
}

/**
 * @brief Return a JSON string from the passed in CBOR data.
 *
 * @param[in] componentID - The comp ID from the UserData section header
 * @param[in] subType - The subtype from the UserData section header
 * @param[in] version - The version from the UserData section header
 * @param[in] creatorID - The creator ID for the PEL
 * @param[in] data - The CBOR data
 *
 * @return std::string - The JSON string
 */
std::string getCBORJSON(uint16_t componentID, uint8_t subType, uint8_t version,
                        uint8_t creatorID, const std::vector<uint8_t>& data)
{
    // The CBOR parser needs the pad bytes added to 4 byte align
    // removed.  The number of bytes added to the pad is on the
    // very end, so will remove both fields before parsing.

    // If the data vector is too short, an exception will get
    // thrown which will be handled up the call stack.

    auto cborData = data;
    uint32_t pad{};

    Stream stream{cborData};
    stream.offset(cborData.size() - 4);
    stream >> pad;

    if (cborData.size() > (pad + sizeof(pad)))
    {
        cborData.resize(data.size() - sizeof(pad) - pad);
    }

    orderedJSON json = orderedJSON::from_cbor(cborData);

    return prettyJSON(componentID, subType, version, creatorID, json);
}

/**
 * @brief Return a JSON string from the passed in text data.
 *
 * The function breaks up the input text into a vector of strings with
 * newline as separator and converts that into JSON.  It will convert any
 * unprintable characters to periods.
 *
 * @param[in] componentID - The comp ID from the UserData section header
 * @param[in] subType - The subtype from the UserData section header
 * @param[in] version - The version from the UserData section header
 * @param[in] creatorID - The creator ID for the PEL
 * @param[in] data - The CBOR data
 *
 * @return std::string - The JSON string
 */
std::string getTextJSON(uint16_t componentID, uint8_t subType, uint8_t version,
                        uint8_t creatorID, const std::vector<uint8_t>& data)
{
    std::vector<std::string> text;
    size_t startPos = 0;

    // Converts any unprintable characters to periods
    auto validate = [](char& ch) {
        if ((ch < ' ') || (ch > '~'))
        {
            ch = '.';
        }
    };

    // Break up the data into an array of strings with newline as separator
    for (size_t pos = 0; pos < data.size(); ++pos)
    {
        if (data[pos] == '\n')
        {
            std::string line{reinterpret_cast<const char*>(&data[startPos]),
                             pos - startPos};
            std::for_each(line.begin(), line.end(), validate);
            text.push_back(std::move(line));
            startPos = pos + 1;
        }
    }
    if (startPos < data.size())
    {
        std::string line{reinterpret_cast<const char*>(&data[startPos]),
                         data.size() - startPos};
        std::for_each(line.begin(), line.end(), validate);
        text.push_back(std::move(line));
    }

    orderedJSON json = text;
    return prettyJSON(componentID, subType, version, creatorID, json);
}

/**
 * @brief Convert to an appropriate JSON string as the data is one of
 *        the formats that we natively support.
 *
 * @param[in] componentID - The comp ID from the UserData section header
 * @param[in] subType - The subtype from the UserData section header
 * @param[in] version - The version from the UserData section header
 * @param[in] data - The data itself
 *
 * @return std::optional<std::string> - The JSON string if it could be created,
 *                                      else std::nullopt.
 */
std::optional<std::string> getBuiltinFormatJSON(
    uint16_t componentID, uint8_t subType, uint8_t version,
    const std::vector<uint8_t>& data, uint8_t creatorID)
{
    switch (subType)
    {
        case static_cast<uint8_t>(UserDataFormat::json):
        {
            std::string jsonString{data.begin(), data.begin() + data.size()};

            orderedJSON json = orderedJSON::parse(jsonString);

            return prettyJSON(componentID, subType, version, creatorID, json);
        }
        case static_cast<uint8_t>(UserDataFormat::cbor):
        {
            return getCBORJSON(componentID, subType, version, creatorID, data);
        }
        case static_cast<uint8_t>(UserDataFormat::text):
        {
            return getTextJSON(componentID, subType, version, creatorID, data);
        }
        default:
            break;
    }
    return std::nullopt;
}

/**
 * @brief Call Python modules to parse the data into a JSON string
 *
 * The module to call is based on the Creator Subsystem ID and the Component
 * ID under the namespace "udparsers". For example: "udparsers.xyyyy.xyyyy"
 * where "x" is the Creator Subsystem ID and "yyyy" is the Component ID.
 *
 * All modules must provide the following:
 * Function: parseUDToJson
 * Argument list:
 *    1. (int) Sub-section type
 *    2. (int) Section version
 *    3. (memoryview): Data
 *-Return data:
 *    1. (str) JSON string
 *
 * @param[in] componentID - The comp ID from the UserData section header
 * @param[in] subType - The subtype from the UserData section header
 * @param[in] version - The version from the UserData section header
 * @param[in] data - The data itself
 * @param[in] creatorID - The creatorID from the PrivateHeader section
 * @return std::optional<std::string> - The JSON string if it could be created,
 *                                      else std::nullopt
 */
std::optional<std::string> getPythonJSON(
    uint16_t componentID, uint8_t subType, uint8_t version,
    const std::vector<uint8_t>& data, uint8_t creatorID)
{
    PyObject *pName, *pModule, *eType, *eValue, *eTraceback, *pKey;
    std::string pErrStr;
    std::string module = getNumberString("%c", tolower(creatorID)) +
                         getNumberString("%04x", componentID);
    pName = PyUnicode_FromString(
        std::string("udparsers." + module + "." + module).c_str());
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
        std::string funcToCall = "parseUDToJson";
        pKey = PyUnicode_FromString(funcToCall.c_str());
        std::unique_ptr<PyObject, decltype(&pyDecRef)> keyPtr(pKey, &pyDecRef);
        PyObject* pDict = PyModule_GetDict(pModule);
        Py_INCREF(pDict);
        if (!PyDict_Contains(pDict, pKey))
        {
            Py_DECREF(pDict);
            lg2::error("Python module error.  Function missing: {FUNC}, "
                       "module = {MODULE}, subtype = {SUBTYPE}, "
                       "version = {VERSION}, data length = {LEN}",
                       "FUNC", funcToCall, "MODULE", module, "SUBTYPE", subType,
                       "VERSION", version, "LEN", data.size());
            return std::nullopt;
        }
        PyObject* pFunc = PyDict_GetItemString(pDict, funcToCall.c_str());
        Py_DECREF(pDict);
        Py_INCREF(pFunc);
        if (PyCallable_Check(pFunc))
        {
            auto ud = data.data();
            PyObject* pArgs = PyTuple_New(3);
            std::unique_ptr<PyObject, decltype(&pyDecRef)> argPtr(
                pArgs, &pyDecRef);
            PyTuple_SetItem(pArgs, 0,
                            PyLong_FromUnsignedLong((unsigned long)subType));
            PyTuple_SetItem(pArgs, 1,
                            PyLong_FromUnsignedLong((unsigned long)version));
            PyObject* pData = PyMemoryView_FromMemory(
                reinterpret_cast<char*>(const_cast<unsigned char*>(ud)),
                data.size(), PyBUF_READ);
            PyTuple_SetItem(pArgs, 2, pData);
            PyObject* pResult = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pFunc);
            if (pResult)
            {
                std::unique_ptr<PyObject, decltype(&pyDecRef)> resPtr(
                    pResult, &pyDecRef);

                if (pResult == Py_None)
                {
                    // Just return a nullopt so it will hexdump the section
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
                        return prettyJSON(componentID, subType, version,
                                          creatorID, json);
                    }
                }
                catch (const std::exception& e)
                {
                    lg2::error("Bad JSON from parser.  Error = {ERROR}, "
                               "module = {MODULE}, subtype = {SUBTYPE}, "
                               "version = {VERSION}, data length = {LEN}",
                               "ERROR", e, "MODULE", module, "SUBTYPE", subType,
                               "VERSION", version, "LEN", data.size());
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
        lg2::debug("Python exception thrown by parser.  Error = {ERROR}, "
                   "module = {MODULE}, subtype = {SUBTYPE}, "
                   "version = {VERSION}, data length = {LEN}",
                   "ERROR", pErrStr, "MODULE", module, "SUBTYPE", subType,
                   "VERSION", version, "LEN", data.size());
    }
    return std::nullopt;
}

std::optional<std::string> getJSON(
    uint16_t componentID, uint8_t subType, uint8_t version,
    const std::vector<uint8_t>& data, uint8_t creatorID,
    const std::vector<std::string>& plugins)
{
    std::string subsystem = getNumberString("%c", tolower(creatorID));
    std::string component = getNumberString("%04x", componentID);
    try
    {
        if (pv::creatorIDs.at(getNumberString("%c", creatorID)) == "BMC" &&
            componentID == static_cast<uint16_t>(ComponentID::phosphorLogging))
        {
            return getBuiltinFormatJSON(componentID, subType, version, data,
                                        creatorID);
        }
        else if (std::find(plugins.begin(), plugins.end(),
                           subsystem + component) != plugins.end())
        {
            return getPythonJSON(componentID, subType, version, data,
                                 creatorID);
        }
    }
    catch (const std::exception& e)
    {
        lg2::error("Failed parsing UserData.  Error = {ERROR}, "
                   "component ID = {COMP_ID}, subtype = {SUBTYPE}, "
                   "version = {VERSION}, data length = {LEN}",
                   "ERROR", e, "COMP_ID", componentID, "SUBTYPE", subType,
                   "VERSION", version, "LEN", data.size());
    }

    return std::nullopt;
}

} // namespace openpower::pels::user_data
