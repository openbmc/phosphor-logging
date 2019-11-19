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
#include "../pel.hpp"
#include "../pel_values.hpp"

#include <CLI/CLI.hpp>
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
 * @brief Print a list of PELs
 */
void printList()
{
    using PEL_t = std::unique_ptr<PEL>;
    std::string listStr;
    std::vector<PEL_t> vec;
    // std::vector<auto> sortedPELs;
    listStr = "{\n";
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

                auto pelPtr = std::make_unique<PEL>(data);
                vec.push_back(std::move(pelPtr));
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
    std::sort(std::begin(vec), std::end(vec),
              [](const PEL_t& a, const PEL_t& b) {
                  return a->privateHeader().id() < b->privateHeader().id();
              });

    std::string val;
    char tmpValStr[50];
    for (std::vector<PEL_t>::iterator it = vec.begin(); it != vec.end(); ++it)
    {

        // id
        sprintf(tmpValStr, "%X", (*it)->privateHeader().id());
        val = std::string(tmpValStr);
        listStr += "\t\t\"" + val + "\": {\n";

        // ASCII
        sprintf(tmpValStr, "%s",
                (*it)->primarySRC().value()->asciiString().c_str());
        val = std::string(tmpValStr);

        listStr += "\t\t\t\"SRC\": \"" + trim(val) + "\",\n";

        // id
        sprintf(tmpValStr, "%X", (*it)->privateHeader().id());
        val = std::string(tmpValStr);
        listStr += "\t\t\t\"PLID\": \"" + val + "\"},\n ";

        // platformid
        sprintf(tmpValStr, "%X", (*it)->privateHeader().plid());
        val = std::string(tmpValStr);
        listStr += "\t\t\t\"PID\": \"" + val + "\",\n";

        // subsytem
        std::string subsystem = (*it)->userHeader().getValue(
            (*it)->userHeader().subsystem(), pel_values::subsystemValues);
        sprintf(tmpValStr, "%s", subsystem.c_str());
        val = std::string(tmpValStr);
        listStr += "\t\t\t\"CompID\": \"" + val + "\",\n";
        // commit time
        sprintf(tmpValStr, " %02X/%02X/%02X%02X  %02X:%02X:%02X",
                (*it)->privateHeader().commitTimestamp().month,
                (*it)->privateHeader().commitTimestamp().day,
                (*it)->privateHeader().commitTimestamp().yearMSB,
                (*it)->privateHeader().commitTimestamp().yearLSB,
                (*it)->privateHeader().commitTimestamp().hour,
                (*it)->privateHeader().commitTimestamp().minutes,
                (*it)->privateHeader().commitTimestamp().seconds);
        val = std::string(tmpValStr);

        listStr += "\t\t\t\"Commit Time\": \"" + val + "\",\n";

        // severity
        std::string severity = (*it)->userHeader().getValue(
            (*it)->userHeader().severity(), pel_values::severityValues);
        sprintf(tmpValStr, "%s", severity.c_str());
        val = std::string(tmpValStr);

        listStr += "\t\t\t\"SEV\": \"" + val + "\",\n ";

        std::size_t found = listStr.rfind(",");
        if (found != std::string::npos)
        {

            listStr.replace(found, 1, "");
        }

        listStr += "\t\t}, \n";
    }

    std::size_t found = listStr.rfind(",");
    if (found != std::string::npos)
    {

        listStr.replace(found, 1, "");
    }
    listStr += "\n}\n";
    printf("%s", listStr.c_str());
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
    app.add_option("-f,--file", fileName, "Raw PEL File");
    app.add_flag("-l", listPEL, "List PELS");
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
        printList();
    }
    else
    {
        exitWithError(app.help("", CLI::AppFormatMode::All),
                      "Raw PEL file path not specified.");
    }
    return 0;
}
