#include "processor.hpp"

#include <sdbusplus/async.hpp>
#include <sdbusplus/server/manager.hpp>

using phosphor::logging::cper::Decoder;
using phosphor::logging::cper::Processor;

int main()
{
    sdbusplus::async::context ctx;
    sdbusplus::server::manager_t manager{ctx, Processor::instance_path};

    Decoder decoder{};
    decoder.start();

    Processor processor{ctx, Processor::instance_path, decoder};

    ctx.spawn([](sdbusplus::async::context& ctx) -> sdbusplus::async::task<> {
        ctx.request_name(Processor::default_service);
        co_return;
    }(ctx));

    ctx.run();

    return 0;
}
