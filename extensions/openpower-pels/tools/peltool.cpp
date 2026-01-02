// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "config.h"

#include "config_main.h"

#include "../bcd_time.hpp"
#include "../json_utils.hpp"
#include "../paths.hpp"
#include "../pel.hpp"
#include "../pel_types.hpp"
#include "../pel_values.hpp"

#include <Python.h>

#include <CLI/CLI.hpp>
#include <phosphor-logging/log.hpp>

#include <bitset>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

namespace fs = std::filesystem;
using namespace phosphor::logging;
using namespace openpower::pels;
namespace message = openpower::pels::message;
namespace pv = openpower::pels::pel_values;

const uint8_t critSysTermSeverity = 0x51;

using PELFunc = std::function<void(const PEL&, bool hexDump)>;
message::Registry registry(getPELReadOnlyDataPath() / message::registryFileName,
                           false);

std::string pelLogDir()
{
    return std::string(phosphor::logging::paths::extension()) + "/pels/logs";
}

/**
 * @brief helper function to get PEL commit timestamp from file name
 * @retrun uint64_t - PEL commit timestamp
 * @param[in] std::string - file name
 */
uint64_t fileNameToTimestamp(const std::string& fileName)
{
    std::string token = fileName.substr(0, fileName.find("_"));
    uint64_t bcdTime = 0;
    if (token.length() >= 14)
    {
        int i = 0;

        try
        {
            auto tmp = std::stoul(token.substr(i, 2), nullptr, 16);
            bcdTime |= (static_cast<uint64_t>(tmp) << 56);
        }
        catch (const std::exception& err)
        {
            std::cout << "Conversion failure: " << err.what() << std::endl;
        }
        i += 2;
        try
        {
            auto tmp = std::stoul(token.substr(i, 2), nullptr, 16);
            bcdTime |= (static_cast<uint64_t>(tmp) << 48);
        }
        catch (const std::exception& err)
        {
            std::cout << "Conversion failure: " << err.what() << std::endl;
        }
        i += 2;
        try
        {
            auto tmp = std::stoul(token.substr(i, 2), nullptr, 16);
            bcdTime |= (static_cast<uint64_t>(tmp) << 40);
        }
        catch (const std::exception& err)
        {
            std::cout << "Conversion failure: " << err.what() << std::endl;
        }
        i += 2;
        try
        {
            auto tmp = std::stoul(token.substr(i, 2), nullptr, 16);
            bcdTime |= (static_cast<uint64_t>(tmp) << 32);
        }
        catch (const std::exception& err)
        {
            std::cout << "Conversion failure: " << err.what() << std::endl;
        }
        i += 2;
        try
        {
            auto tmp = std::stoul(token.substr(i, 2), nullptr, 16);
            bcdTime |= (tmp << 24);
        }
        catch (const std::exception& err)
        {
            std::cout << "Conversion failure: " << err.what() << std::endl;
        }
        i += 2;
        try
        {
            auto tmp = std::stoul(token.substr(i, 2), nullptr, 16);
            bcdTime |= (tmp << 16);
        }
        catch (const std::exception& err)
        {
            std::cout << "Conversion failure: " << err.what() << std::endl;
        }
        i += 2;
        try
        {
            auto tmp = std::stoul(token.substr(i, 2), nullptr, 16);
            bcdTime |= (tmp << 8);
        }
        catch (const std::exception& err)
        {
            std::cout << "Conversion failure: " << err.what() << std::endl;
        }
        i += 2;
        try
        {
            auto tmp = std::stoul(token.substr(i, 2), nullptr, 16);
            bcdTime |= tmp;
        }
        catch (const std::exception& err)
        {
            std::cout << "Conversion failure: " << err.what() << std::endl;
        }
    }
    return bcdTime;
}

/**
 * @brief helper function to get PEL id from file name
 * @retrun uint32_t - PEL id
 * @param[in] std::string - file name
 */
uint32_t fileNameToPELId(const std::string& fileName)
{
    uint32_t num = 0;
    try
    {
        num = std::stoul(fileName.substr(fileName.find("_") + 1), nullptr, 16);
    }
    catch (const std::exception& err)
    {
        std::cout << "Conversion failure: " << err.what() << std::endl;
    }
    return num;
}

/**
 * @brief Check if the string ends with the PEL ID string passed in
 * @param[in] str - string to check for PEL ID
 * @param[in] pelID - PEL id string
 *
 * @return bool - true with suffix matches
 */
bool endsWithPelID(const std::string& str, const std::string& pelID)
{
    constexpr size_t pelIDSize = 8;

    if (pelID.size() != pelIDSize)
    {
        return false;
    }

    size_t slen = str.size(), elen = pelID.size();
    if (slen < elen)
        return false;
    while (elen)
    {
        if (str[--slen] != pelID[--elen])
            return false;
    }
    return true;
}

/**
 * @brief get data form raw PEL file.
 * @param[in] std::string Name of file with raw PEL
 * @return std::vector<uint8_t> char vector read from raw PEL file.
 */
std::vector<uint8_t> getFileData(const std::string& name)
{
    std::ifstream file(name, std::ifstream::in);
    if (file.good())
    {
        std::vector<uint8_t> data{std::istreambuf_iterator<char>(file),
                                  std::istreambuf_iterator<char>()};
        return data;
    }
    else
    {
        return {};
    }
}

/**
 * @brief Initialize Python interpreter and gather all UD parser modules under
 *        the paths found in Python sys.path and the current user directory.
 *        This is to prevent calling a non-existant module which causes Python
 *        to print an import error message and breaking JSON output.
 *
 * @return std::vector<std::string> Vector of plugins found in filesystem
 */
std::vector<std::string> getPlugins()
{
    Py_Initialize();
    std::vector<std::string> plugins;
    std::vector<std::string> siteDirs;
    std::array<std::string, 2> parserDirs = {"udparsers", "srcparsers"};
    PyObject* pName = PyUnicode_FromString("sys");
    PyObject* pModule = PyImport_Import(pName);
    Py_XDECREF(pName);
    PyObject* pDict = PyModule_GetDict(pModule);
    Py_XDECREF(pModule);
    PyObject* pResult = PyDict_GetItemString(pDict, "path");
    PyObject* pValue = PyUnicode_FromString(".");
    PyList_Append(pResult, pValue);
    Py_XDECREF(pValue);
    auto list_size = PyList_Size(pResult);
    for (auto i = 0; i < list_size; i++)
    {
        PyObject* item = PyList_GetItem(pResult, i);
        PyObject* pBytes = PyUnicode_AsEncodedString(item, "utf-8", "~E~");
        const char* output = PyBytes_AS_STRING(pBytes);
        Py_XDECREF(pBytes);
        std::string tmpStr(output);
        siteDirs.push_back(tmpStr);
    }
    for (const auto& dir : siteDirs)
    {
        for (const auto& parserDir : parserDirs)
        {
            if (fs::exists(dir + "/" + parserDir))
            {
                for (const auto& entry :
                     fs::directory_iterator(dir + "/" + parserDir))
                {
                    if (entry.is_directory() and
                        fs::exists(entry.path().string() + "/" +
                                   entry.path().stem().string() + ".py"))
                    {
                        plugins.push_back(entry.path().stem());
                    }
                }
            }
        }
    }
    return plugins;
}

/**
 * @brief Creates JSON string of a PEL entry if fullPEL is false or prints to
 *        stdout the full PEL in JSON if fullPEL is true
 * @param[in] itr - std::map iterator of <uint32_t, BCDTime>
 * @param[in] hidden - Boolean to include hidden PELs
 * @param[in] includeInfo - Boolean to include informational PELs
 * @param[in] critSysTerm - Boolean to include critical error and system
 * termination PELs
 * @param[in] fullPEL - Boolean to print full JSON representation of PEL
 * @param[in] foundPEL - Boolean to check if any PEL is present
 * @param[in] scrubRegex - SRC regex object
 * @param[in] plugins - Vector of strings of plugins found in filesystem
 * @param[in] hexDump - Boolean to print hexdump of PEL instead of JSON
 * @return std::string - JSON string of PEL entry (empty if fullPEL is true)
 */
template <typename T>
std::string genPELJSON(
    T itr, bool hidden, bool includeInfo, bool critSysTerm, bool fullPEL,
    bool& foundPEL, const std::optional<std::regex>& scrubRegex,
    const std::vector<std::string>& plugins, bool hexDump, bool archive)
{
    std::string val;
    std::string listStr;
    char name[51];
    sprintf(name, "/%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X_%.8X",
            static_cast<uint8_t>((itr.second >> 56) & 0xFF),
            static_cast<uint8_t>((itr.second >> 48) & 0xFF),
            static_cast<uint8_t>((itr.second >> 40) & 0xFF),
            static_cast<uint8_t>((itr.second >> 32) & 0xFF),
            static_cast<uint8_t>((itr.second >> 24) & 0xFF),
            static_cast<uint8_t>((itr.second >> 16) & 0xFF),
            static_cast<uint8_t>((itr.second >> 8) & 0xFF),
            static_cast<uint8_t>(itr.second & 0xFF), itr.first);

    auto fileName = (archive ? pelLogDir() + "/archive" : pelLogDir()) + name;
    try
    {
        std::vector<uint8_t> data = getFileData(fileName);
        if (data.empty())
        {
            log<level::ERR>("Empty PEL file",
                            entry("FILENAME=%s", fileName.c_str()));
            return listStr;
        }
        PEL pel{data};
        if (!pel.valid())
        {
            return listStr;
        }
        if (!includeInfo && pel.userHeader().severity() == 0)
        {
            return listStr;
        }
        if (critSysTerm && pel.userHeader().severity() != critSysTermSeverity)
        {
            return listStr;
        }
        std::bitset<16> actionFlags{pel.userHeader().actionFlags()};
        if (!hidden && actionFlags.test(hiddenFlagBit))
        {
            return listStr;
        }
        if (pel.primarySRC() && scrubRegex)
        {
            val = pel.primarySRC().value()->asciiString();
            if (std::regex_search(trimEnd(val), scrubRegex.value(),
                                  std::regex_constants::match_not_null))
            {
                return listStr;
            }
        }
        if (hexDump)
        {
            std::cout
                << dumpHex(std::data(pel.data()), pel.size(), 0, false).get()
                << std::endl;
        }
        else if (fullPEL)
        {
            if (!foundPEL)
            {
                std::cout << "[\n";
                foundPEL = true;
            }
            else
            {
                std::cout << ",\n\n";
            }
            pel.toJSON(registry, plugins);
        }
        else
        {
            // id
            listStr += "    \"" +
                       getNumberString("0x%X", pel.privateHeader().id()) +
                       "\": {\n";
            // ASCII
            if (pel.primarySRC())
            {
                val = pel.primarySRC().value()->asciiString();
                jsonInsert(listStr, "SRC", trimEnd(val), 2);

                // Registry message
                auto regVal = pel.primarySRC().value()->getErrorDetails(
                    registry, DetailLevel::message, true);
                if (regVal)
                {
                    val = regVal.value();
                    jsonInsert(listStr, "Message", val, 2);
                }
            }
            else
            {
                jsonInsert(listStr, "SRC", "No SRC", 2);
            }

            // platformid
            jsonInsert(listStr, "PLID",
                       getNumberString("0x%X", pel.privateHeader().plid()), 2);

            // creatorid
            std::string creatorID =
                getNumberString("%c", pel.privateHeader().creatorID());
            val = pv::creatorIDs.count(creatorID) ? pv::creatorIDs.at(creatorID)
                                                  : "Unknown Creator ID";
            jsonInsert(listStr, "CreatorID", val, 2);

            // subsystem
            std::string subsystem = pv::getValue(pel.userHeader().subsystem(),
                                                 pel_values::subsystemValues);
            jsonInsert(listStr, "Subsystem", subsystem, 2);

            // commit time
            char tmpValStr[50];
            sprintf(tmpValStr, "%02X/%02X/%02X%02X %02X:%02X:%02X",
                    pel.privateHeader().commitTimestamp().month,
                    pel.privateHeader().commitTimestamp().day,
                    pel.privateHeader().commitTimestamp().yearMSB,
                    pel.privateHeader().commitTimestamp().yearLSB,
                    pel.privateHeader().commitTimestamp().hour,
                    pel.privateHeader().commitTimestamp().minutes,
                    pel.privateHeader().commitTimestamp().seconds);
            jsonInsert(listStr, "Commit Time", tmpValStr, 2);

            // severity
            std::string severity = pv::getValue(pel.userHeader().severity(),
                                                pel_values::severityValues);
            jsonInsert(listStr, "Sev", severity, 2);

            // compID
            jsonInsert(
                listStr, "CompID",
                getComponentName(pel.privateHeader().header().componentID,
                                 pel.privateHeader().creatorID()),
                2);

            auto found = listStr.rfind(",");
            if (found != std::string::npos)
            {
                listStr.replace(found, 1, "");
                listStr += "    },\n";
            }
            foundPEL = true;
        }
    }
    catch (const std::exception& e)
    {
        log<level::ERR>("Hit exception while reading PEL File",
                        entry("FILENAME=%s", fileName.c_str()),
                        entry("ERROR=%s", e.what()));
    }
    return listStr;
}

/**
 * @brief Print a list of PELs or a JSON array of PELs
 * @param[in] order - Boolean to print in reverse orser
 * @param[in] hidden - Boolean to include hidden PELs
 * @param[in] includeInfo - Boolean to include informational PELs
 * @param[in] critSysTerm - Boolean to include critical error and system
 * termination PELs
 * @param[in] fullPEL - Boolean to print full PEL into a JSON array
 * @param[in] scrubRegex - SRC regex object
 * @param[in] hexDump - Boolean to print hexdump of PEL instead of JSON
 */
void printPELs(bool order, bool hidden, bool includeInfo, bool critSysTerm,
               bool fullPEL, const std::optional<std::regex>& scrubRegex,
               bool hexDump, bool archive = false)
{
    std::string listStr;
    std::vector<std::pair<uint32_t, uint64_t>> PELs;
    std::vector<std::string> plugins;
    listStr = "{\n";
    for (auto it = (archive ? fs::directory_iterator(pelLogDir() + "/archive")
                            : fs::directory_iterator(pelLogDir()));
         it != fs::directory_iterator(); ++it)
    {
        if (!fs::is_regular_file((*it).path()))
        {
            continue;
        }
        else
        {
            PELs.emplace_back(fileNameToPELId((*it).path().filename()),
                              fileNameToTimestamp((*it).path().filename()));
        }
    }

    // Sort the pairs based on second time parameter
    std::sort(PELs.begin(), PELs.end(),
              [](const auto& left, const auto& right) {
                  return left.second < right.second;
              });

    bool foundPEL = false;

    if (fullPEL && !hexDump)
    {
        plugins = getPlugins();
    }
    auto buildJSON = [&listStr, &hidden, &includeInfo, &critSysTerm, &fullPEL,
                      &foundPEL, &scrubRegex, &plugins, &hexDump,
                      &archive](const auto& i) {
        listStr += genPELJSON(i, hidden, includeInfo, critSysTerm, fullPEL,
                              foundPEL, scrubRegex, plugins, hexDump, archive);
    };
    if (order)
    {
        std::for_each(PELs.rbegin(), PELs.rend(), buildJSON);
    }
    else
    {
        std::for_each(PELs.begin(), PELs.end(), buildJSON);
    }
    if (hexDump)
    {
        return;
    }
    if (foundPEL)
    {
        if (fullPEL)
        {
            std::cout << "]" << std::endl;
        }
        else
        {
            std::size_t found;
            found = listStr.rfind(",");
            if (found != std::string::npos)
            {
                listStr.replace(found, 1, "");
                listStr += "}\n";
                printf("%s", listStr.c_str());
            }
        }
    }
    else
    {
        std::string emptyJSON = fullPEL ? "[]" : "{}";
        std::cout << emptyJSON << std::endl;
    }
}

/**
 * @brief Calls the function passed in on the PEL with the ID
 *        passed in.
 *
 * @param[in] id - The string version of the PEL or BMC Log ID, either with or
 *                 without the 0x prefix.
 * @param[in] func - The std::function<void(const PEL&, bool hexDump)> function
 *                   to run.
 * @param[in] useBMC - if true, search by BMC Log ID, else search by PEL ID
 * @param[in] hexDump - Boolean to print hexdump of PEL instead of JSON
 */
void callFunctionOnPEL(const std::string& id, const PELFunc& func,
                       bool useBMC = false, bool hexDump = false,
                       bool archive = false)
{
    std::string pelID{id};
    if (!useBMC)
    {
        std::transform(pelID.begin(), pelID.end(), pelID.begin(), toupper);

        if (pelID.starts_with("0X"))
        {
            pelID.erase(0, 2);
        }
    }

    bool found = false;

    for (auto it = (archive ? fs::directory_iterator(pelLogDir() + "/archive")
                            : fs::directory_iterator(pelLogDir()));
         it != fs::directory_iterator(); ++it)
    {
        // The PEL ID is part of the filename, so use that to find the PEL if
        // "useBMC" is set to false, otherwise we have to search within the PEL

        if (!fs::is_regular_file((*it).path()))
        {
            continue;
        }

        if ((endsWithPelID((*it).path(), pelID) && !useBMC) || useBMC)
        {
            auto data = getFileData((*it).path());
            if (!data.empty())
            {
                PEL pel{data};
                if (!useBMC ||
                    (useBMC && pel.obmcLogID() == std::stoul(id, nullptr, 0)))
                {
                    found = true;
                    try
                    {
                        func(pel, hexDump);
                        break;
                    }
                    catch (const std::exception& e)
                    {
                        std::cerr << " Internal function threw an exception: "
                                  << e.what() << "\n";
                        exit(1);
                    }
                }
            }
            else
            {
                std::cerr << "Could not read PEL file\n";
                exit(1);
            }
        }
    }

    if (!found)
    {
        std::cerr << "PEL not found\n";
        exit(1);
    }
}

/**
 * @brief Delete a PEL file.
 *
 * @param[in] id - The PEL ID to delete.
 */
void deletePEL(const std::string& id)
{
    std::string pelID{id};

    std::transform(pelID.begin(), pelID.end(), pelID.begin(), toupper);

    if (pelID.starts_with("0X"))
    {
        pelID.erase(0, 2);
    }

    for (auto it = fs::directory_iterator(pelLogDir());
         it != fs::directory_iterator(); ++it)
    {
        if (endsWithPelID((*it).path(), pelID))
        {
            fs::remove((*it).path());
        }
    }
}

/**
 * @brief Delete all PEL files.
 */
void deleteAllPELs()
{
    log<level::INFO>("peltool deleting all event logs");

    for (const auto& entry : fs::directory_iterator(pelLogDir()))
    {
        if (!fs::is_regular_file(entry.path()))
        {
            continue;
        }
        fs::remove(entry.path());
    }
}

/**
 * @brief Display a single PEL
 *
 * @param[in] pel - the PEL to display
 * @param[in] hexDump - Boolean to print hexdump of PEL instead of JSON
 */
void displayPEL(const PEL& pel, bool hexDump)
{
    if (pel.valid())
    {
        if (hexDump)
        {
            std::string dstr =
                dumpHex(std::data(pel.data()), pel.size(), 0, false).get();
            std::cout << dstr << std::endl;
        }
        else
        {
            auto plugins = getPlugins();
            pel.toJSON(registry, plugins);
        }
    }
    else
    {
        std::cerr << "PEL was malformed\n";
        exit(1);
    }
}

/**
 * @brief Print number of PELs
 * @param[in] hidden - Bool to include hidden logs
 * @param[in] includeInfo - Bool to include informational logs
 * @param[in] critSysTerm - Bool to include CritSysTerm
 * @param[in] scrubRegex - SRC regex object
 */
void printPELCount(bool hidden, bool includeInfo, bool critSysTerm,
                   const std::optional<std::regex>& scrubRegex)
{
    std::size_t count = 0;

    for (auto it = fs::directory_iterator(pelLogDir());
         it != fs::directory_iterator(); ++it)
    {
        if (!fs::is_regular_file((*it).path()))
        {
            continue;
        }
        std::vector<uint8_t> data = getFileData((*it).path());
        if (data.empty())
        {
            continue;
        }
        PEL pel{data};
        if (!pel.valid())
        {
            continue;
        }
        if (!includeInfo && pel.userHeader().severity() == 0)
        {
            continue;
        }
        if (critSysTerm && pel.userHeader().severity() != critSysTermSeverity)
        {
            continue;
        }
        std::bitset<16> actionFlags{pel.userHeader().actionFlags()};
        if (!hidden && actionFlags.test(hiddenFlagBit))
        {
            continue;
        }
        if (pel.primarySRC() && scrubRegex)
        {
            std::string val = pel.primarySRC().value()->asciiString();
            if (std::regex_search(trimEnd(val), scrubRegex.value(),
                                  std::regex_constants::match_not_null))
            {
                continue;
            }
        }
        count++;
    }
    std::cout << "{\n"
              << "    \"Number of PELs found\": "
              << getNumberString("%d", count) << "\n}\n";
}

/**
 * @brief Generate regex pattern object from file contents
 * @param[in] scrubFile - File containing regex pattern
 * @return std::regex - SRC regex object
 */
std::regex genRegex(std::string& scrubFile)
{
    std::string pattern;
    std::ifstream contents(scrubFile);
    if (contents.fail())
    {
        std::cerr << "Can't open \"" << scrubFile << "\"\n";
        exit(1);
    }
    std::string line;
    while (std::getline(contents, line))
    {
        if (!line.empty())
        {
            pattern.append(line + "|");
        }
    }
    try
    {
        std::regex scrubRegex(pattern, std::regex::icase);
        return scrubRegex;
    }
    catch (const std::regex_error& e)
    {
        if (e.code() == std::regex_constants::error_collate)
            std::cerr << "Invalid collating element request\n";
        else if (e.code() == std::regex_constants::error_ctype)
            std::cerr << "Invalid character class\n";
        else if (e.code() == std::regex_constants::error_escape)
            std::cerr << "Invalid escape character or trailing escape\n";
        else if (e.code() == std::regex_constants::error_backref)
            std::cerr << "Invalid back reference\n";
        else if (e.code() == std::regex_constants::error_brack)
            std::cerr << "Mismatched bracket ([ or ])\n";
        else if (e.code() == std::regex_constants::error_paren)
        {
            // to catch return code error_badrepeat when error_paren is retured
            // instead
            size_t pos = pattern.find_first_of("*+?{");
            while (pos != std::string::npos)
            {
                if (pos == 0 || pattern.substr(pos - 1, 1) == "|")
                {
                    std::cerr
                        << "A repetition character (*, ?, +, or {) was not "
                           "preceded by a valid regular expression\n";
                    exit(1);
                }
                pos = pattern.find_first_of("*+?{", pos + 1);
            }
            std::cerr << "Mismatched parentheses (( or ))\n";
        }
        else if (e.code() == std::regex_constants::error_brace)
            std::cerr << "Mismatched brace ({ or })\n";
        else if (e.code() == std::regex_constants::error_badbrace)
            std::cerr << "Invalid range inside a { }\n";
        else if (e.code() == std::regex_constants::error_range)
            std::cerr << "Invalid character range (e.g., [z-a])\n";
        else if (e.code() == std::regex_constants::error_space)
            std::cerr << "Insufficient memory to handle regular expression\n";
        else if (e.code() == std::regex_constants::error_badrepeat)
            std::cerr << "A repetition character (*, ?, +, or {) was not "
                         "preceded by a valid regular expression\n";
        else if (e.code() == std::regex_constants::error_complexity)
            std::cerr << "The requested match is too complex\n";
        else if (e.code() == std::regex_constants::error_stack)
            std::cerr << "Insufficient memory to evaluate a match\n";
        exit(1);
    }
}

static void exitWithError(const std::string& help, const char* err)
{
    std::cerr << "ERROR: " << err << std::endl << help << std::endl;
    exit(-1);
}

int main(int argc, char** argv)
{
    CLI::App app{"OpenBMC PEL Tool"};
    std::string fileName;
    std::string idPEL;
    std::string bmcId;
    std::string idToDelete;
    std::string scrubFile;
    std::optional<std::regex> scrubRegex;
    bool listPEL = false;
    bool listPELDescOrd = false;
    bool hidden = false;
    bool includeInfo = false;
    bool critSysTerm = false;
    bool deleteAll = false;
    bool showPELCount = false;
    bool fullPEL = false;
    bool hexDump = false;
    bool archive = false;

    app.set_help_flag("--help", "Print this help message and exit");
    app.add_option("--file", fileName, "Display a PEL using its Raw PEL file");
    app.add_option("-i, --id", idPEL, "Display a PEL based on its ID");
    app.add_option("--bmc-id", bmcId,
                   "Display a PEL based on its BMC Event ID");
    app.add_flag("-a", fullPEL, "Display all PELs");
    app.add_flag("-l", listPEL, "List PELs");
    app.add_flag("-n", showPELCount, "Show number of PELs");
    app.add_flag("-r", listPELDescOrd, "Reverse order of output");
    app.add_flag("-h", hidden, "Include hidden PELs");
    app.add_flag("-f,--info", includeInfo, "Include informational PELs");
    app.add_flag("-t, --termination", critSysTerm,
                 "List only critical system terminating PELs");
    app.add_option("-d, --delete", idToDelete, "Delete a PEL based on its ID");
    app.add_flag("-D, --delete-all", deleteAll, "Delete all PELs");
    app.add_option("-s, --scrub", scrubFile,
                   "File containing SRC regular expressions to ignore");
    app.add_flag("-x", hexDump, "Display PEL(s) in hexdump instead of JSON");
    app.add_flag("--archive", archive, "List or display archived PELs");

    CLI11_PARSE(app, argc, argv);

    if (!fileName.empty())
    {
        std::vector<uint8_t> data = getFileData(fileName);
        if (!data.empty())
        {
            PEL pel{data};
            if (hexDump)
            {
                std::string dstr =
                    dumpHex(std::data(pel.data()), pel.size(), 0, false).get();
                std::cout << dstr << std::endl;
            }
            else
            {
                auto plugins = getPlugins();
                pel.toJSON(registry, plugins);
            }
        }
        else
        {
            exitWithError(app.help("", CLI::AppFormatMode::All),
                          "Raw PEL file can't be read.");
        }
    }
    else if (!idPEL.empty())
    {
        callFunctionOnPEL(idPEL, displayPEL, false, hexDump, archive);
    }
    else if (!bmcId.empty())
    {
        callFunctionOnPEL(bmcId, displayPEL, true, hexDump, archive);
    }
    else if (fullPEL || listPEL)
    {
        if (!scrubFile.empty())
        {
            scrubRegex = genRegex(scrubFile);
        }
        printPELs(listPELDescOrd, hidden, includeInfo, critSysTerm, fullPEL,
                  scrubRegex, hexDump, archive);
    }
    else if (showPELCount)
    {
        if (!scrubFile.empty())
        {
            scrubRegex = genRegex(scrubFile);
        }
        printPELCount(hidden, includeInfo, critSysTerm, scrubRegex);
    }
    else if (!idToDelete.empty())
    {
        deletePEL(idToDelete);
    }
    else if (deleteAll)
    {
        deleteAllPELs();
    }
    else
    {
        std::cout << app.help("", CLI::AppFormatMode::All) << std::endl;
    }
    Py_Finalize();
    return 0;
}
