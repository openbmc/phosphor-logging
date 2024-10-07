#pragma once

#include <sdbusplus/async.hpp>
#include <sdbusplus/exception.hpp>

namespace lg2::details
{

/* Non-template versions of the commit functions */

auto commit(sdbusplus::exception::generated_event_base&& t)
    -> sdbusplus::message::object_path;

auto commit(sdbusplus::async::context& ctx,
            sdbusplus::exception::generated_event_base&& t)
    -> sdbusplus::async::task<sdbusplus::message::object_path>;

} // namespace lg2::details
