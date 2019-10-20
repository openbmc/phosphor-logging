#include "../hexdump.hpp"
#include "../pel.hpp"

#include <CLI/CLI.hpp>
#include <iostream>
#include <string>

using namespace phosphor::logging;
using namespace openpower::pels;

static void exitWithError(const std::string& help, const char* err)
{
    std::cerr << "ERROR: " << err << std::endl << help << std::endl;
    exit(-1);
}

int main(int argc, char** argv)
{
    CLI::App app{"OpenBMC PEL Tool"};
    std::string fileName;
    app.add_option("-f,--file", fileName, "Raw PEL File");
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
    else
    {
        exitWithError(app.help("", CLI::AppFormatMode::All),
                      "Raw PEL file path not specified.");
    }
    return 0;
}
