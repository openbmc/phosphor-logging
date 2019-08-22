#include "peltool.hpp"

#include <CLI/CLI.hpp>
#include <filesystem>
#include <fstream>
#include <iostream> // std::cout
#include <iterator>
#include <map>
#include <phosphor-logging/log.hpp>
#include <sstream> // std::stringstream
#include <string>  // std::string

using namespace phosphor::logging;
namespace fs = std::filesystem;
using namespace openpower::pels;
#define SECTION_VERSION "Section Version"
//--------------UH------------------------
#define SCOPE "Scope"                   //
#define SEVERITY "Severity"             //
#define EVENT_TYPE "Event Type"         //
#define PROBLEM_DOMAIN "Problem Domain" //
#define PROBLEM_VECTOR "Problem Vector" //
#define ACTION_FLAGS "Action Flags"     //
//----------------------------------------
//-------------PH---------------------------//
#define CREATE_TIME_STAMP "Created at"    //
#define COMMIT_TIME_STAMP "Commited at"   //
#define CREATOR_ID "Creator id"           //
#define LOG_TYPE "Log type"               //
#define SECTION_COUNT "Section count"     //
#define OBMC_LOG_ID "Log id"              //
#define CREATOR_VERSION "Creator version" //
#define ID "id"                           //
#define PLID "plid"                       //
//---------------------------------------

#define SUBSECTION_TYPE "Subsection Type" //
#define CREATED_BY "Created by"           //
#define CREATED_AT "Created at"           //
#define COMMITED_AT "Commited at"
#define CREATOR_SUBSYSTEM "Creator Subsytemm"
#define CSSVER "CSSVER"
#define PLATFORM_LOG_ID "Platform Log Id"
#define ENTRY_ID "Entry ID"
#define LOG_SIZE "Total Log Size"

static void exit_with_error(const std::string& help, const char* err)
{
    std::cerr << "ERROR: " << err << std::endl << help << std::endl;
    exit(-1);
}

/*static std::string parsePel(PEL& _pel)
{
    BCDTime foo;
    std::string _str;
    foo = _pel.privateHeader()->createTimestamp();
    _str = printLine();
    _str += printLineBreak();
    _str += CREATE_TIME_STAMP;
    _str += parseTimeStamp(foo);
    return _str;
}
*/
/*int mainRunner(int argc, char** argv)
{

    //PelTool pt(" ");
    // Read arguments.
    std::string pelid = "";

    CLI::App app{"OpenBMC Pel Tool"};
    app.add_option(" -id, --pelid", pelid, "pelid for pel to read");

    CLI11_PARSE(app, argc, argv);

    // Add a PEL to the repo, and get the data back with getPELData.
    auto data = pelDataFactory(TestPelType::pelSimple);
    auto dataCopy = *data;
    auto pel = std::make_unique<PEL>(*data);
    parsePel(*pel);
    // auto pelID = pel->id();
    std::string wholeTimestamp;
    Stream stream(*data);
    PrivateHeader ph(stream);
    BCDTime foo;
    foo = ph.createTimestamp();
    wholeTimestamp = parseTimeStamp(foo);
    //pt._repo.add(pel);

    //pt.test = "Test";
    // Parse out path argument.
    auto id = pelid;
    if (!id.empty())
    {
        std::cout << "whole time stamp" << wholeTimestamp << '\n';
    }

    if (id.empty())
    {
    }

    if (id.empty())
    {
        exit_with_error(app.help("", CLI::AppFormatMode::All),
                        "id not specified or invalid.");
    }

    return 0;

}
*/
// namespace openpower
int main(int argc, char** argv)
{
    //
    // PelTool pt(" ");
    // Read arguments.
    std::string pelid = "";
    std::string pelRawFile = "";
    CLI::App app{"OpenBMC Pel Tool"};
    app.add_option(" -p, --pelid", pelid, "pelid for pel to read");
    app.add_option(" -r --rawpel", pelRawFile, "file for reading raw pel from");
    CLI11_PARSE(app, argc, argv);

    // Add a PEL to the repo, and get the data back with getPELData.
    auto data = pelDataFactory(TestPelType::pelSimple);
    Stream stream(*data);
    PrivateHeader ph(stream);
    UserHeader uh(stream);
    PelParser pp("");
    auto id = pelid;
    if (!id.empty())
    {
        // eventType
        // actionFlags()
        pp.openpower::pels::PelParser::printUH(
            uh.subsystem(), uh.severity(), uh.eventType(), uh.actionFlags());
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
            Stream stream2(data);
            PrivateHeader ph2(stream);
            UserHeader uh2(stream);
            pp.openpower::pels::PelParser::printUH(
                uh2.subsystem(), uh2.severity(), uh2.eventType(),
                uh2.actionFlags());
        }
    }
    if (id.empty() && pelRawFile.empty())
    {
        exit_with_error(app.help("", CLI::AppFormatMode::All),
                        "No valid id or raw pel file path  not specifiedd.");
    }
    return 0;
}
