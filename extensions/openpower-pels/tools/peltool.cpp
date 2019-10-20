#include "peltool.hpp"

#include "peltoolutils.hpp"

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

static void exitWithError(const std::string& help, const char* err)
{
    std::cerr << "ERROR: " << err << std::endl << help << std::endl;
    exit(-1);
}

/*PEL PELTool::getPEL()
{
    return _pel;
}
*/

int main(int argc, char** argv)
{
    std::string pelRawFile;
    std::unique_ptr<PEL> _pel;
    //_pel = std::make_unique<PEL>(realPELData);
    PELTool pt(realPELData);
    CLI::App app{"OpenBMC PEL Tool"};
    bool flagList;
    bool flagHexDump;
    app.add_flag("--list", flagList, "list all PELs in Repo");
    app.add_flag("--hexdump", flagHexDump, "Hex dump for entire PEL");
    app.add_option(" -r, --rawpel", pelRawFile,
                   "file for reading raw pel from");
    CLI11_PARSE(app, argc, argv);

    if (flagHexDump)
    {
        hexDump(std::data(pt._pel.data()), pt._pel.data().size());
    }
    if (!flagList && !flagHexDump)
    {
        exitWithError(app.help("", CLI::AppFormatMode::All),
                      "No valid id or raw pel file path not specified.");
    }

    return 0;
}
