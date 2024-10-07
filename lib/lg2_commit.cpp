#include <phosphor-logging/lg2/commit.hpp>
#include <sdbusplus/async.hpp>
#include <sdbusplus/exception.hpp>

namespace lg2::details
{

auto commit([[maybe_unused]] sdbusplus::exception::generated_event_base&& t)
    -> sdbusplus::message::object_path
{
    return {};
}

auto commit([[maybe_unused]] sdbusplus::async::context& ctx,
            [[maybe_unused]] sdbusplus::exception::generated_event_base&& t)
    -> sdbusplus::async::task<sdbusplus::message::object_path>
{
    co_return {};
}

} // namespace lg2::details
