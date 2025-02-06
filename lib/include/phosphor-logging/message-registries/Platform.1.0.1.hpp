#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace Platform_1_0_1
{

struct [[deprecated ("Use Platform_1_2_0")]]
UnhandledExceptionDetectedAfterReset :
    public RedfishMessage<>
{
    template <typename... Args>
    UnhandledExceptionDetectedAfterReset(Args... args) :
        RedfishMessage(
            "Platform.1.0.1.UnhandledExceptionDetectedAfterReset",
            "Indicates that an unhandled exception caused the platform to reset.",
            "This message shall indicate that an unhandled exception caused a compute platform to reset.  The exception was detected after the reset occurred.",
            "An unhandled exception caused a platform reset.",
            "Critical",
            0,
            {},
            {},
            {},
            "Check additional diagnostic data if available.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Platform_1_2_0")]]
OperatingSystemCrash :
    public RedfishMessage<>
{
    template <typename... Args>
    OperatingSystemCrash(Args... args) :
        RedfishMessage(
            "Platform.1.0.1.OperatingSystemCrash",
            "Indicates the operating system was halted due to a catastrophic error.",
            "This message shall indicate that an unhandled exception caused a compute platform to crash or otherwise halt.",
            "An operating system crash occurred.",
            "Critical",
            0,
            {},
            {},
            {},
            "Check additional diagnostic data if available.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Platform_1_2_0")]]
PlatformError :
    public RedfishMessage<>
{
    template <typename... Args>
    PlatformError(Args... args) :
        RedfishMessage(
            "Platform.1.0.1.PlatformError",
            "Indicates that a platform error occurred.",
            "This message shall indicate that a platform error occurred.",
            "A platform error occurred.",
            "Warning",
            0,
            {},
            {},
            {},
            "Check additional diagnostic data if available.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Platform_1_2_0")]]
PlatformErrorAtLocation :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PlatformErrorAtLocation(Args... args) :
        RedfishMessage(
            "Platform.1.0.1.PlatformErrorAtLocation",
            "Indicates that a platform error occurred and device or other location information is available.",
            "This message shall indicate that a platform error occurred with specific device identification information available.",
            "A platform error occurred at location '%1'.",
            "Warning",
            1,
            { "string" },
            { "The device description or location where the error occurred." },
            { "This argument shall contain a description of the device or location where the error occurred." },
            "Check additional diagnostic data if available.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace Platform_1_0_1
}; // namespace registries
}; // namespace redfish
