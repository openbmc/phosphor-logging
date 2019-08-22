#include "peltool.hpp"

#include <CLI/CLI.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <phosphor-logging/log.hpp>
#include <sstream>
#include <string>

using namespace phosphor::logging;
namespace fs = std::filesystem;
using namespace openpower::pels;

static void exit_with_error(const std::string& help, const char* err)
{
    std::cerr << "ERROR: " << err << std::endl << help << std::endl;
    exit(-1);
}

// namespace openpower
int main(int argc, char** argv)
{
    std::string pelRawFile = "";
    CLI::App app{"OpenBMC PEL Tool"};
    bool flag_list;
    app.add_flag("--list,-l", flag_list, "list all PELs in Repo");

    app.add_option(" -r --rawpel", pelRawFile, "file for reading raw pel from");
    CLI11_PARSE(app, argc, argv);

    PELParser pp("");
    if (flag_list)
    {
        Repository rep__("/var/lib/phosphor-logging/extensions/pels");
        std::map<Repository::LogID, std::filesystem::path> tmpPELMap;
        tmpPELMap = rep__.getPELMap();
        std::map<Repository::LogID, std::filesystem::path>::iterator it;
        printf(
            "| Entry Id      Commit Time      SubSystem                        "
            "Committed by |\n");
        printf("| Platform Id   State            Event Severity                "
               "    "
               "  Ascii Str |\n");
        for (it = tmpPELMap.begin(); it != tmpPELMap.end(); ++it)
        {
            if (rep__.hasPEL(it->first))
            {
                PEL myPel(*(rep__.getPELData(it->first)), it->first.obmcID.id);
                printf("| 0x%08X %02X/%02X/%02X%02X  %02X:%02X:%02X %-36.36s "
                       "%8.8s |\n",
                       it->first.obmcID.id, myPel.commitTime().month,
                       myPel.commitTime().day, myPel.commitTime().yearMSB,
                       myPel.commitTime().yearLSB, myPel.commitTime().hour,
                       myPel.commitTime().minutes, myPel.commitTime().seconds,
                       pp.getFailingSubSystem(myPel.userHeader()->subsystem()),
                       "todo");
                printf("| 0x%08X %-19s %-36.36s %8.8s |\n",
                       myPel.privateHeader()->plid(), "todo",
                       pp.getSeverity(myPel.userHeader()->severity()), "todo");
            }
        }
    }
    if (!pelRawFile.empty())
    {
        if (fs::exists(pelRawFile))
        {
            std::ifstream file(pelRawFile, std::ios::in | std::ios::binary);

            auto data =
                std::vector<uint8_t>(std::istreambuf_iterator<char>(file),
                                     std::istreambuf_iterator<char>());
            if (file.fail())
            {
                log<level::ERR>("Filesystem error reading a raw PEL",
                                entry("PELFILE=%s", pelRawFile.c_str()),
                                entry("OBMCLOGID=%d", 0x0000));
                // TODO, Decide what to do here. Maybe nothing.
            }
            Stream stream(data);
            PrivateHeader ph(stream);
            UserHeader uh(stream);
            pp.openpower::pels::PELParser::printUH(
                uh.subsystem(), uh.severity(), uh.eventType(),
                uh.actionFlags());
        }
    }
    if (!flag_list && pelRawFile.empty())
    {
        exit_with_error(app.help("", CLI::AppFormatMode::All),
                        "No valid id or raw pel file path not specified.");
    }
    return 0;
}
