#include "peltool.hpp"

#include "peltoolutils.hpp"

#include <CLI/CLI.hpp>
#include <cstring>
#include <fifo_map/src/fifo_map.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <phosphor-logging/log.hpp>
#include <sstream>
#include <string>
#include <vector>
#include <xyz/openbmc_project/Common/File/error.hpp>

using namespace nlohmann;
using json = nlohmann::json;
using namespace phosphor::logging;
namespace fs = std::filesystem;
using namespace openpower::pels;
using namespace std;
// A workaround to give to use fifo_map as map, we are just ignoring the 'less'
// compare
template <class K, class V, class dummy_compare, class A>
using workaround_fifo_map = fifo_map<K, V, fifo_map_compare<K>, A>;
using fifoMap = basic_json<workaround_fifo_map>;

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
    // std::unique_ptr<PEL> _pel;
    //_pel = std::make_unique<PEL>(realPELData);
    PELTool pt(realPELData);
    CLI::App app{"OpenBMC PEL Tool"};
    bool flagList;
    bool flagHexDump;
    bool rawPel;
    app.add_flag("--list", flagList, "list all PELs in Repo");
    app.add_flag("--hexdump", flagHexDump, "Hex dump for entire PEL");
    app.add_flag("--rp", rawPel, "Print raw PEL");
    CLI11_PARSE(app, argc, argv);

    if (flagHexDump)
    {
        // hexDump(std::data(pt._pel.data()), pt._pel.data().size());
    }
    if (rawPel)
    {
        std::ifstream file("testpel", std::ifstream::in);
        std::vector<uint8_t> data{std::istreambuf_iterator<char>(file),
                                  std::istreambuf_iterator<char>()};
        file.close();
        PEL pel{data};
        for (auto& section : pel.optionalSections())
        {
            vector<uint8_t> _data;
            Stream s{_data};
            section->flatten(s);
            std::cout << "=====================";
            // TODO: working on hexdump function that returns a formated string
            // for the dump.
            // std::cout<<hexdump<std::vector<uint8_t>>(&_data, _data.size(),
            // "Hexdump" );
            hexDump(std::data(_data), _data.size());
        }
    }
    // put switch or if else elseif
    if (!flagList && !flagHexDump && !rawPel)
    {
        exitWithError(app.help("", CLI::AppFormatMode::All),
                      "No valid id or raw pel file path not specified.");
    }
    return 0;
}
