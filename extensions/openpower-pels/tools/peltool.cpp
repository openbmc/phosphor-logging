#include "../pel.hpp"
#include "peltoolutils.hpp"

#include <CLI/CLI.hpp>
#include <cstring>
#include <fifo_map/src/fifo_map.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <nlohmann/json.hpp>
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
    CLI::App app{"OpenBMC PEL Tool"};
    bool flagList;
    bool flagHexDump;
    app.add_flag("--list", flagList, "list all PELs in Repo");
    app.add_flag("--hexdump", flagHexDump, "Hex dump for entire PEL");
    std::string filename = "default";
    app.add_option("-f,--file", filename, "Raw PEL File");
    CLI11_PARSE(app, argc, argv);

    if (flagHexDump)
    {
        // hexDump(std::data(pt._pel.data()), pt._pel.data().size());
    }
    if (!filename.empty())
    {
        std::ifstream file(filename, std::ifstream::in);
        std::vector<uint8_t> data{std::istreambuf_iterator<char>(file),
                                  std::istreambuf_iterator<char>()};
        file.close();
        PEL pel{data};
        pel.toJson();
    }
    // put switch or if else elseif
    if (!flagList && !flagHexDump && filename.empty())
    {
        exitWithError(app.help("", CLI::AppFormatMode::All),
                      "No valid id or raw pel file path not specified.");
    }
    return 0;
}
