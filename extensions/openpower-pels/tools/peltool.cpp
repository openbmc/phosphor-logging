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
#include "config.h"

#include "../bcd_time.hpp"
#include "../pel.hpp"
#include "../pel_types.hpp"
#include "../pel_values.hpp"

#include <CLI/CLI.hpp>
#include <bitset>
#include <iostream>
#include <phosphor-logging/log.hpp>
#include <regex>
#include <string>
#include <xyz/openbmc_project/Common/File/error.hpp>

namespace fs = std::filesystem;
using namespace phosphor::logging;
using namespace openpower::pels;
namespace file_error = sdbusplus::xyz::openbmc_project::Common::File::Error;
namespace message = openpower::pels::message;
namespace pv = openpower::pels::pel_values;

std::string ltrim(const std::string& s)
{
    return std::regex_replace(s, std::regex("^\\s+"), std::string(""));
}

std::string rtrim(const std::string& s)
{
    return std::regex_replace(s, std::regex("\\s+$"), std::string(""));
}

std::string trim(const std::string& s)
{
    return ltrim(rtrim(s));
}

/**
 * @brief helper function to get PEL commit timestamp from file name
 * @retrun BCDTime - PEL commit timestamp
 * @param[in] std::string - file name
 */
BCDTime fileNameToTimestamp(const std::string& fileName)
{
    std::string token = fileName.substr(0, fileName.find("_"));
    int i = 0;
    BCDTime tmp;
    if (token.length() >= 14)
    {
        try
        {
            tmp.yearMSB = std::stoi(token.substr(i, 2), 0, 16);
        }
        catch (std::exception& err)
        {
            std::cout << "Conversion failure: " << err.what() << std::endl;
        }
        i += 2;
        try
        {
            tmp.yearLSB = std::stoi(token.substr(i, 2), 0, 16);
        }
        catch (std::exception& err)
        {
            std::cout << "Conversion failure: " << err.what() << std::endl;
        }
        i += 2;
        try
        {
            tmp.month = std::stoi(token.substr(i, 2), 0, 16);
        }
        catch (std::exception& err)
        {
            std::cout << "Conversion failure: " << err.what() << std::endl;
        }
        i += 2;
        try
        {
            tmp.day = std::stoi(token.substr(i, 2), 0, 16);
        }
        catch (std::exception& err)
        {
            std::cout << "Conversion failure: " << err.what() << std::endl;
        }
        i += 2;
        try
        {
            tmp.hour = std::stoi(token.substr(i, 2), 0, 16);
        }
        catch (std::exception& err)
        {
            std::cout << "Conversion failure: " << err.what() << std::endl;
        }
        i += 2;
        try
        {
            tmp.minutes = std::stoi(token.substr(i, 2), 0, 16);
        }
        catch (std::exception& err)
        {
            std::cout << "Conversion failure: " << err.what() << std::endl;
        }
        i += 2;
        try
        {
            tmp.seconds = std::stoi(token.substr(i, 2), 0, 16);
        }
        catch (std::exception& err)
        {
            std::cout << "Conversion failure: " << err.what() << std::endl;
        }
        i += 2;
        try
        {
            tmp.hundredths = std::stoi(token.substr(i, 2), 0, 16);
        }
        catch (std::exception& err)
        {
            std::cout << "Conversion failure: " << err.what() << std::endl;
        }
    }
    return tmp;
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
        num = std::stoi(fileName.substr(fileName.find("_") + 1), 0, 16);
    }
    catch (std::exception& err)
    {
        std::cout << "Conversion failure: " << err.what() << std::endl;
    }
    return num;
}

/**
 * @brief helper function to check string suffix
 * @retrun bool - true with suffix matches
 * @param[in] std::string - string to check for suffix
 * @param[in] std::string - suffix string
 */
bool ends_with(const std::string& str, const std::string& end)
{
    size_t slen = str.size(), elen = end.size();
    if (slen < elen)
        return false;
    while (elen)
    {
        if (str[--slen] != end[--elen])
            return false;
    }
    return true;
}

/**
 * @brief get data form raw PEL file.
 * @param[in] std::string Name of file with raw PEL
 * @return std::vector<uint8_t> char vector read from raw PEL file.
 */
std::vector<uint8_t> getFileData(std::string name)
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
        printf("Can't open raw PEL file");
        return {};
    }
}

template <typename T>
std::string genPELJSON(T itr, bool hidden)
{
    std::size_t found;
    std::string val;
    char tmpValStr[50];
    std::string listStr;
    char name[50];
    sprintf(name, "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X_%.8X", itr.second.yearMSB,
            itr.second.yearLSB, itr.second.month, itr.second.day,
            itr.second.hour, itr.second.minutes, itr.second.seconds,
            itr.second.hundredths, itr.first);
    std::string fileName(name);
    fileName = EXTENSION_PERSIST_DIR "/pels/logs/" + fileName;
    try
    {
        std::vector<uint8_t> data = getFileData(fileName);

        if (!data.empty())
        {

            PEL pel{data};
            std::bitset<16> actionFlags{pel.userHeader().actionFlags()};
            if (pel.valid() && (hidden || !actionFlags.test(hiddenFlagBit)))
            {
                // id
                sprintf(tmpValStr, "0x%X", pel.privateHeader().id());
                val = std::string(tmpValStr);
                listStr += "\t\"" + val + "\": {\n";
                // ASCII
                val = pel.primarySRC() ? pel.primarySRC().value()->asciiString()
                                       : "No SRC";
                listStr += "\t\t\"SRC\": \"" + trim(val) + "\",\n";
                // platformid
                sprintf(tmpValStr, "0x%X", pel.privateHeader().plid());
                val = std::string(tmpValStr);
                listStr += "\t\t\"PLID\": \"" + val + "\",\n";
                // creatorid
                sprintf(tmpValStr, "%c", pel.privateHeader().creatorID());
                std::string creatorID(tmpValStr);
                val = pv::creatorIDs.count(creatorID)
                          ? pv::creatorIDs.at(creatorID)
                          : "Unknown Creator ID";
                listStr += "\t\t\"CreatorID\": \"" + val + "\",\n";
                // subsytem
                std::string subsystem = pv::getValue(
                    pel.userHeader().subsystem(), pel_values::subsystemValues);
                listStr += "\t\t\"Subsystem\": \"" + subsystem + "\",\n";
                // commit time
                sprintf(tmpValStr, "%02X/%02X/%02X%02X  %02X:%02X:%02X",
                        pel.privateHeader().commitTimestamp().month,
                        pel.privateHeader().commitTimestamp().day,
                        pel.privateHeader().commitTimestamp().yearMSB,
                        pel.privateHeader().commitTimestamp().yearLSB,
                        pel.privateHeader().commitTimestamp().hour,
                        pel.privateHeader().commitTimestamp().minutes,
                        pel.privateHeader().commitTimestamp().seconds);
                val = std::string(tmpValStr);
                listStr += "\t\t\"Commit Time\": \"" + val + "\",\n";
                // severity
                std::string severity = pv::getValue(pel.userHeader().severity(),
                                                    pel_values::severityValues);
                listStr += "\t\t\"Sev\": \"" + severity + "\",\n ";
                // compID
                sprintf(tmpValStr, "0x%X",
                        pel.privateHeader().header().componentID);
                val = std::string(tmpValStr);
                listStr += "\t\t\"CompID\": \"" + val + "\",\n ";

                found = listStr.rfind(",");
                if (found != std::string::npos)
                {
                    listStr.replace(found, 1, "");
                    listStr += "\t}, \n";
                }
            }
        }
        else
        {
            log<level::ERR>("Empty PEL file",
                            entry("FILENAME=%s", fileName.c_str()),
                            entry("ERROR=%s", "Empty PEL file"));
        }
    }
    catch (std::exception& e)
    {
        log<level::ERR>("Hit exception while reading PEL File",
                        entry("FILENAME=%s", fileName.c_str()),
                        entry("ERROR=%s", e.what()));
    }
    return listStr;
}
/**
 * @brief Print a list of PELs
 */
void printList(bool order, bool hidden)
{
    std::string listStr;
    std::map<uint32_t, BCDTime> PELs;
    std::size_t found;
    listStr = "{\n";
    for (auto it = fs::directory_iterator(EXTENSION_PERSIST_DIR "/pels/logs");
         it != fs::directory_iterator(); ++it)
    {
        if (!fs::is_regular_file((*it).path()))
        {
            continue;
        }
        else
        {
            PELs.emplace(fileNameToPELId((*it).path().filename()),
                         fileNameToTimestamp((*it).path().filename()));
        }
    }
    std::string val;
    auto buildJSON = [&listStr, &hidden](const auto& i) {
        listStr += genPELJSON(i, hidden);
    };
    if (order)
    {
        std::for_each(PELs.rbegin(), PELs.rend(), buildJSON);
    }
    else
    {
        std::for_each(PELs.begin(), PELs.end(), buildJSON);
    }

    found = listStr.rfind(",");
    if (found != std::string::npos)
    {
        listStr.replace(found, 1, "");
        listStr += "\n}\n";
        printf("%s", listStr.c_str());
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
    bool listPEL = false;
    bool listPELDescOrd = false;
    bool listPELShowHidden = false;
    app.add_option("-f,--file", fileName,
                   "Display a PEL using its Raw PEL file");
    app.add_option("-i, --id", idPEL, "Display a PEL based on its ID");
    app.add_flag("-l", listPEL, "List PELS");
    app.add_flag("-r", listPELDescOrd, "Reverse order of output");
    app.add_flag("-s", listPELShowHidden, "Show hidden PELs");
    CLI11_PARSE(app, argc, argv);

    if (!fileName.empty())
    {
        std::vector<uint8_t> data = getFileData(fileName);
        if (!data.empty())
        {
            PEL pel{data};
            pel.toJSON();
        }
        else
        {
            exitWithError(app.help("", CLI::AppFormatMode::All),
                          "Raw PEL file can't be read.");
        }
    }

    else if (!idPEL.empty())
    {
        for (auto it =
                 fs::directory_iterator(EXTENSION_PERSIST_DIR "/pels/logs");
             it != fs::directory_iterator(); ++it)
        {
            if (!fs::is_regular_file((*it).path()))
            {
                continue;
            }
            try
            {
                for (auto& c : idPEL)
                    c = toupper(c);
                size_t found = idPEL.find("0X");
                if (found == 0)
                {
                    idPEL.erase(0, 2);
                }
                if (ends_with((*it).path(), idPEL))
                {
                    std::vector<uint8_t> data = getFileData((*it).path());
                    if (!data.empty())
                    {
                        PEL pel{data};
                        if (pel.valid())
                        {
                            pel.toJSON();
                        }
                        else
                        {
                            log<level::ERR>(
                                "PEL File contains invalid PEL",
                                entry("FILENAME=%s", (*it).path().c_str()),
                                entry("ERROR=%s", "file contains invalid PEL"));
                        }
                    }
                    break;
                }
            }
            catch (std::exception& e)
            {
                log<level::ERR>("Hit exception while reading PEL File",
                                entry("FILENAME=%s", (*it).path().c_str()),
                                entry("ERROR=%s", e.what()));
            }
        }
    }
    else if (listPEL)
    {

        printList(listPELDescOrd, listPELShowHidden);
    }
    else
    {
        exitWithError(app.help("", CLI::AppFormatMode::All),
                      "Raw PEL file path not specified.");
    }
    return 0;
}
