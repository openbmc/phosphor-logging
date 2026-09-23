#pragma once

#include <sdbusplus/async.hpp>
#include <sdbusplus/message/native_types.hpp>
#include <xyz/openbmc_project/Logging/CPER/Types/common.hpp>
#include <xyz/openbmc_project/Logging/Extension/CPER/Processed/common.hpp>

#include <cstdint>
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
// Single test fixture befriended for access to private functions.
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

    /** CPER severity codes (matches libcper CPER_SEVERITY_TYPES). */
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

    auto processedProps(ContentType type, OemData data, Guid guid)
        -> ProcessedProps;

    // OEM map, type GUID, severity; empty on failure.
    auto parseLibCPER(ContentType type, std::span<const std::uint8_t> raw)
        -> std::tuple<OemData, Guid, Severity>;

    // Map a libcper severity code to Severity.
    static auto severity(uint32_t code) -> Severity;

    auto process(sdbusplus::object_path source, ContentType type,
                 std::vector<std::uint8_t> raw) -> sdbusplus::async::task<>;
    void run();

    sdbusplus::async::context worker;
    std::thread thread;
    bool started = false;
};

} // namespace phosphor::logging::cper
