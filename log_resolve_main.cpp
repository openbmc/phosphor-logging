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
    app.add_option("id", id, "Log Entry index");

    CLI11_PARSE(app, argc, argv);

    try
    {
        static constexpr auto nsPath = Proxy::namespace_path::value;
        static constexpr auto nsEntry = Proxy::namespace_path::entry;

        auto path = std::string(nsPath) + "/" + std::string(nsEntry) + "/" +
                    std::to_string(id);
        lg2::resolve(path);
    }
    catch (std::exception& e)
    {
        std::cerr << "Unable to resolve: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
