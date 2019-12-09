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

template <typename T>
std::string genPELJSON(T itr)
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
        std::ifstream stream(fileName, std::ios::in | std::ios::binary);
        std::vector<uint8_t> data((std::istreambuf_iterator<char>(stream)),
                                  std::istreambuf_iterator<char>());
        stream.close();
        PEL pel{data};
        if (pel.valid())
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
            val = pv::creatorIDs.count(creatorID) ? pv::creatorIDs.at(creatorID)
                                                  : "Unknown Creator ID";
            listStr += "\t\t\"CreatorID\": \"" + val + "\",\n";
            // subsytem
            std::string subsystem = pv::getValue(pel.userHeader().subsystem(),
                                                 pel_values::subsystemValues);
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
        try
        {
            std::ifstream stream((*it).path(), std::ios::in | std::ios::binary);
            std::vector<uint8_t> data((std::istreambuf_iterator<char>(stream)),
                                      std::istreambuf_iterator<char>());
            stream.close();
            PEL pel{data};
            if (pel.valid())
            {

                std::bitset<16> actionFlags{pel.userHeader().actionFlags()};
                if (hidden || !actionFlags.test(hiddenFlagBit))
                {
                    PELs.emplace(pel.id(),
                                 pel.privateHeader().commitTimestamp());
                }
            }
        }
        catch (std::exception& e)
        {
            log<level::ERR>("Hit exception while reading PEL File",
                            entry("FILENAME=%s", (*it).path().c_str()),
                            entry("ERROR=%s", e.what()));
        }
    }
    std::string val;
    auto buildJSON = [&listStr](const auto& i) { listStr += genPELJSON(i); };
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

static void exitWithError(const std::string& help, const char* err)
{
    std::cerr << "ERROR: " << err << std::endl << help << std::endl;
    exit(-1);
}

int main(int argc, char** argv)
{
    CLI::App app{"OpenBMC PEL Tool"};
    std::string fileName;
    bool listPEL;
    bool listPELDescOrd;
    bool listPELShowHidden;
    listPELDescOrd = false;
    listPELShowHidden = false;
    app.add_option("-f,--file", fileName, "Raw PEL file");
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
