#pragma once

#include <sdbusplus/async.hpp>
#include <sdbusplus/message/native_types.hpp>
#include <xyz/openbmc_project/Logging/CPER/Types/common.hpp>
#include <xyz/openbmc_project/Logging/Extension/CPER/Processed/common.hpp>

#include <cstdint>
#include <functional>
#include <map>
#include <span>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

namespace phosphor::logging::cper
{
namespace test
{
class DecoderTest;
}; // namespace test

using ContentType =
    sdbusplus::common::xyz::openbmc_project::logging::cper::Types::ContentType;

namespace dbus::cper
{
using Processed = sdbusplus::common::xyz::openbmc_project::logging::extension::
    cper::Processed;
} // namespace dbus::cper

class Decoder
{
  public:
    using ProcessedProps = dbus::cper::Processed::properties_t;
    using OemData = dbus::cper::Processed::oem_t::value_type;
    using Guid = std::string;

    /** CPER severity codes, matches libcper. */
    enum class Severity : uint32_t
    {
        Recoverable = 0,
        Fatal = 1,
        Corrected = 2,
        Informational = 3,
    };

    Decoder() = default;
    ~Decoder();

    void start();
    void stop();

    void queue(sdbusplus::object_path&& source, ContentType type,
               std::vector<std::uint8_t>&& raw);

  private:
    friend class test::DecoderTest;

    auto process(sdbusplus::object_path source, ContentType type,
                 std::vector<std::uint8_t> raw) -> sdbusplus::async::task<>;

    void run();

    auto processedProps(ContentType type, OemData data, Guid guid)
        -> ProcessedProps;

    auto parseLibCPER(ContentType type, std::span<const std::uint8_t> raw)
        -> std::tuple<OemData, Guid, Severity>;

    static auto severity(uint32_t code) -> Severity;

    using Committer = std::function<sdbusplus::async::task<void>(
        sdbusplus::async::context& ctx,
        sdbusplus::exception::generated_event_base&& event)>;

    void setCommitter(Committer handler);

    sdbusplus::async::context worker;
    std::thread thread;
    bool started = false;
    Committer committer = defaultCommitter;

    static auto defaultCommitter(
        sdbusplus::async::context& ctx,
        sdbusplus::exception::generated_event_base&& event)
        -> sdbusplus::async::task<void>;
};

} // namespace phosphor::logging::cper
