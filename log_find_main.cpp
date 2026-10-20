#include <CLI/CLI.hpp>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/exception.hpp>
#include <sdbusplus/message/native_types.hpp>
#include <xyz/openbmc_project/Common/error.hpp>
#include <xyz/openbmc_project/Logging/Manager/common.hpp>

#include <cstring>
#include <iostream>
#include <string>

using Interface = sdbusplus::common::xyz::openbmc_project::logging::Manager;
using ResourceNotFound =
    sdbusplus::error::xyz::openbmc_project::common::ResourceNotFound;

int main(int argc, char** argv)
{
    CLI::App app{"log-find"};

    std::string service{};
    std::string hint{};
    bool printId = false;
    app.add_option("-s,--service", service,
                   "Service which created the log entry")
        ->required();
    app.add_option("-H,--hint", hint, "Search hint of the log entry")
        ->required();
    app.add_flag("-i,--id", printId,
                 "Print the log entry index instead of the DBus path");

    CLI11_PARSE(app, argc, argv);

    try
    {
        auto bus = sdbusplus::bus::new_default();
        auto method = bus.new_method_call(
            Interface::default_service, Interface::instance_path,
            Interface::interface, Interface::method_names::find_entry);
        method.append(service, hint);

        sdbusplus::object_path path{};
        bus.call(method).read(path);

        if (printId)
        {
            std::cout << path.filename() << std::endl;
        }
        else
        {
            std::cout << path.str << std::endl;
        }
    }
    catch (const sdbusplus::exception_t& e)
    {
        if (std::strcmp(e.name(), ResourceNotFound::errName) == 0)
        {
            std::cerr << "No unresolved log entry found for service '"
                      << service << "' and hint '" << hint << "'" << std::endl;
        }
        else
        {
            std::cerr << "Unable to find: " << e.what() << std::endl;
        }
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Unable to find: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
