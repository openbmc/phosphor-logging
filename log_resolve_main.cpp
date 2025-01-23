#include <CLI/CLI.hpp>
#include <phosphor-logging/commit.hpp>
#include <xyz/openbmc_project/Logging/Entry/client.hpp>

#include <iostream>
#include <string>

using Proxy = sdbusplus::client::xyz::openbmc_project::logging::Entry<>;

int main(int argc, char** argv)
{
    CLI::App app{"log-resolve"};

    size_t id = 0;
    std::string path{};
    auto logIdGroup = app.add_option_group("Log Identifier");
    auto idOpt = logIdGroup->add_option("-i,--id", id, "Log Entry index");
    auto pathOpt =
        logIdGroup->add_option("-p,--path", path, "DBus path of the log entry");
    logIdGroup->require_option(1);

    CLI11_PARSE(app, argc, argv);

    try
    {
        if (*idOpt)
        {
            path = std::string(Proxy::namespace_path::value) + "/" +
                   std::string(Proxy::namespace_path::entry) + "/" +
                   std::to_string(id);
            lg2::resolve(path);
        }
        else if (*pathOpt)
        {
            lg2::resolve(path);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Unable to resolve: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
