#pragma once

#include "decoder.hpp"

#include <sdbusplus/async.hpp>
#include <xyz/openbmc_project/Logging/CPER/Processor/aserver.hpp>

namespace phosphor::logging::cper
{

namespace dbus
{
template <typename T>
using Interface =
    sdbusplus::aserver::xyz::openbmc_project::logging::cper::Processor<T>;
} // namespace dbus

class Processor : public dbus::Interface<Processor>
{
  public:
    explicit Processor(sdbusplus::async::context& ctx, auto path,
                       Decoder& decoder) :
        dbus::Interface<Processor>(ctx, path, signal_action::emit_object_added),
        decoder(decoder)
    {}

    using ContentType = cper::ContentType;

    void method_call(process_t, sdbusplus::object_path source, ContentType type,
                     sdbusplus::message::unix_fd data);

  private:
    Decoder& decoder;
};

} // namespace phosphor::logging::cper
