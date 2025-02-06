#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace Platform_1_1_0
{

struct [[deprecated ("Use Platform_1_2_0")]]
UnhandledExceptionDetectedAfterReset :
    public RedfishMessage<>
{
    template <typename... Args>
    UnhandledExceptionDetectedAfterReset(Args... args) :
        RedfishMessage(
            "Platform.1.1.0.UnhandledExceptionDetectedAfterReset",
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
            "Platform.1.1.0.OperatingSystemCrash",
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
            "Platform.1.1.0.PlatformError",
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
            "Platform.1.1.0.PlatformErrorAtLocation",
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

struct [[deprecated ("Use Platform_1_2_0")]]
NVRAMClearAsserted :
    public RedfishMessage<>
{
    template <typename... Args>
    NVRAMClearAsserted(Args... args) :
        RedfishMessage(
            "Platform.1.1.0.NVRAMClearAsserted",
            "Indicates that the 'NVRAM Clear' jumper or switch is currently asserted.",
            "This message shall indicate that the 'NVRAM Clear' jumper or switch is currently asserted.",
            "'NVRAM Clear' is currently on.",
            "Warning",
            0,
            {},
            {},
            {},
            "Turn off the 'NVRAM Clear' jumper or switch.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Platform_1_2_0")]]
SecurityBypassAsserted :
    public RedfishMessage<>
{
    template <typename... Args>
    SecurityBypassAsserted(Args... args) :
        RedfishMessage(
            "Platform.1.1.0.SecurityBypassAsserted",
            "Indicates that the 'Security Bypass' jumper or switch is currently asserted.",
            "This message shall indicate that the 'Security Bypass' or 'Password Bypass' jumper or switch is currently asserted.",
            "'Security Bypass' is currently on.",
            "Warning",
            0,
            {},
            {},
            {},
            "Turn off the 'Security Bypass' jumper or switch.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace Platform_1_1_0
}; // namespace registries
}; // namespace redfish
