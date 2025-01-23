#include <CLI/CLI.hpp>
#include <phosphor-logging/commit.hpp>
#include <xyz/openbmc_project/Logging/Entry/client.hpp>

#include <iostream>
#include <string>

using Proxy = sdbusplus::client::xyz::openbmc_project::logging::Entry<>;

int main(int argc, char** argv)
{
    CLI::App app{"log-resolve"};

    std::string id = "0";
    app.add_option("id", id, "Log Entry index or full path");

    CLI11_PARSE(app, argc, argv);

    try
    {
        static constexpr auto nsPath = Proxy::namespace_path::value;
        static constexpr auto nsEntry = Proxy::namespace_path::entry;
        auto svcPath = std::string(nsPath) + "/" + std::string(nsEntry);
        if (id.contains(svcPath))
        {
            lg2::resolve(id);
        }
        else
        {
            auto path = svcPath + "/" + id;
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
