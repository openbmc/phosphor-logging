#include "processor.hpp"

#include <CLI/CLI.hpp>
#include <sdbusplus/async.hpp>
#include <sdbusplus/server/manager.hpp>

#include <filesystem>
#include <string>

using phosphor::logging::cper::Decoder;
using phosphor::logging::cper::OemRegistry;
using phosphor::logging::cper::Processor;

int main(int argc, char** argv)
{
    CLI::App app{"phosphor-cper-processor"};

    std::string pluginDir{OemRegistry::defaultDir};
    app.add_option("-p,--plugin-dir", pluginDir,
                   "Directory to load OEM decoder plugins from")
        ->capture_default_str()
        ->check(CLI::ExistingDirectory);

    CLI11_PARSE(app, argc, argv);

    sdbusplus::async::context ctx;
    sdbusplus::server::manager_t manager{ctx, Processor::instance_path};

    Decoder decoder{};
    decoder.start(std::filesystem::path{pluginDir});

    Processor processor{ctx, Processor::instance_path, decoder};

    ctx.spawn([](sdbusplus::async::context& ctx) -> sdbusplus::async::task<> {
        ctx.request_name(Processor::default_service);
        co_return;
    }(ctx));

    ctx.run();

    return 0;
}
