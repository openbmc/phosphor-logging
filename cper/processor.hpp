#pragma once

#include <sdbusplus/async.hpp>
#include <xyz/openbmc_project/Logging/CPER/Processor/aserver.hpp>
#include <xyz/openbmc_project/Logging/CPER/Types/common.hpp>

namespace phosphor::logging::cper
{

namespace dbus
{
template <typename T>
using Interface =
    sdbusplus::aserver::xyz::openbmc_project::logging::cper::Processor<T>;

using Types = sdbusplus::common::xyz::openbmc_project::logging::cper::Types;
} // namespace dbus

class Processor : public dbus::Interface<Processor>
{
  public:
    explicit Processor(sdbusplus::async::context& ctx, auto path) :
        dbus::Interface<Processor>(ctx, path, signal_action::emit_object_added)
    {}

    using ContentType = dbus::Types::ContentType;

    void method_call(process_t, sdbusplus::object_path source, ContentType type,
                     sdbusplus::message::unix_fd data);
};

} // namespace phosphor::logging::cper
