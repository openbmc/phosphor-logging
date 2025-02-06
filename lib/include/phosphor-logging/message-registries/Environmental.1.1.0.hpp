#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace Environmental_1_1_0
{

struct TemperatureAboveUpperCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    TemperatureAboveUpperCautionThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.TemperatureAboveUpperCautionThreshold",
            "Indicates that a temperature reading is above the upper caution threshold.",
            "This message shall indicate that a temperature reading violates an upper caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCaution` or `UpperCautionUser` properties.  Otherwise, the message shall indicate that the reading violates an internal level or threshold.",
            "Temperature '%1' reading of %2 degrees (C) is above the %3 upper caution threshold.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the temperature reading.", "The temperature in degrees Celsius.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the temperature reading.", "This argument shall contain the temperature in degrees Celsius.", "If the reading originated from a `Sensor` resource, this argument shall contain the value of the `Reading` property within the `UpperCaution` or `UpperCautionUser` properties of the `Sensor` resource.  Otherwise, this argument shall contain an internal level or threshold value." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TemperatureAboveUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    TemperatureAboveUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.TemperatureAboveUpperCriticalThreshold",
            "Indicates that a temperature reading is above the upper critical threshold.",
            "This message shall indicate that a temperature reading violates an upper critical threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties.",
            "Temperature '%1' reading of %2 degrees (C) is above the %3 upper critical threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the temperature reading.", "The temperature in degrees Celsius.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the temperature reading.", "This argument shall contain the temperature in degrees Celsius.", "This argument shall contain the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TemperatureAboveUpperFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    TemperatureAboveUpperFatalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.TemperatureAboveUpperFatalThreshold",
            "Indicates that a temperature reading is above the upper fatal threshold.",
            "This message shall indicate that a temperature reading violates an upper fatal threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperFatal` property.",
            "Temperature '%1' reading of %2 degrees (C) is above the %3 upper fatal threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the temperature reading.", "The temperature in degrees Celsius.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the temperature reading.", "This argument shall contain the temperature in degrees Celsius.", "This argument shall contain the value of the `Reading` property within the `UpperFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TemperatureBelowLowerCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    TemperatureBelowLowerCautionThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.TemperatureBelowLowerCautionThreshold",
            "Indicates that a temperature reading is below the lower caution threshold.",
            "This message shall indicate that a temperature reading violates a lower caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCaution` or `LowerCautionUser` properties.",
            "Temperature '%1' reading of %2 degrees (C) is below the %3 lower caution threshold.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the temperature reading.", "The temperature in degrees Celsius.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the temperature reading.", "This argument shall contain the temperature in degrees Celsius.", "This argument shall contain the value of the `Reading` property within the `LowerCaution` or `LowerCautionUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TemperatureBelowLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    TemperatureBelowLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.TemperatureBelowLowerCriticalThreshold",
            "Indicates that a temperature reading is below the lower critical threshold.",
            "This message shall indicate that a temperature reading violates a lower critical threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties.",
            "Temperature '%1' reading of %2 degrees (C) is below the %3 lower critical threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the temperature reading.", "The temperature in degrees Celsius.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the temperature reading.", "This argument shall contain the temperature in degrees Celsius.", "This argument shall contain the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TemperatureBelowLowerFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    TemperatureBelowLowerFatalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.TemperatureBelowLowerFatalThreshold",
            "Indicates that a temperature reading is below the lower fatal threshold.",
            "This message shall indicate that a temperature reading violates a lower fatal threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerFatal` property.",
            "Temperature '%1' reading of %2 degrees (C) is below the %3 lower fatal threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the temperature reading.", "The temperature in degrees Celsius.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the temperature reading.", "This argument shall contain the temperature in degrees Celsius.", "This argument shall contain the value of the `Reading` property within the `LowerFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TemperatureBelowUpperFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    TemperatureBelowUpperFatalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.TemperatureBelowUpperFatalThreshold",
            "Indicates that a temperature reading is no longer above the upper fatal threshold but is still outside of normal operating range.",
            "This message shall indicate that a temperature reading, previously above the upper fatal threshold, no longer violates the upper fatal threshold.  However, the temperature violates at least one upper threshold and is not within the normal operating range.",
            "Temperature '%1' reading of %2 degrees (C) is now below the %3 upper fatal threshold but remains outside of normal range.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the temperature reading.", "The temperature in degrees Celsius.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the temperature reading.", "This argument shall contain the temperature in degrees Celsius.", "This argument shall contain the value of the `Reading` property within the `UpperFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TemperatureBelowUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    TemperatureBelowUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.TemperatureBelowUpperCriticalThreshold",
            "Indicates that a temperature reading is no longer above the upper critical threshold but is still outside of normal operating range.",
            "This message shall indicate that a temperature reading, previously above the upper critical threshold, no longer violates the upper critical threshold.  However, the temperature violates at least one upper threshold and is not within the normal operating range.",
            "Temperature '%1' reading of %2 degrees (C) is now below the %3 upper critical threshold but remains outside of normal range.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the temperature reading.", "The temperature in degrees Celsius.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the temperature reading.", "This argument shall contain the temperature in degrees Celsius.", "This argument shall contain the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TemperatureAboveLowerFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    TemperatureAboveLowerFatalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.TemperatureAboveLowerFatalThreshold",
            "Indicates that a temperature reading is no longer below the lower fatal threshold but is still outside of normal operating range.",
            "This message shall indicate that a temperature reading, previously below the lower fatal threshold, no longer violates the lower fatal threshold.  However, the temperature violates at least one lower threshold and is not within the normal operating range.",
            "Temperature '%1' reading of %2 degrees (C) is now above the %3 lower fatal threshold but remains outside of normal range.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the temperature reading.", "The temperature in degrees Celsius.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the temperature reading.", "This argument shall contain the temperature in degrees Celsius.", "This argument shall contain the value of the `Reading` property within the `LowerFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TemperatureAboveLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    TemperatureAboveLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.TemperatureAboveLowerCriticalThreshold",
            "Indicates that a temperature reading is no longer below the lower critical threshold but is still outside of normal operating range.",
            "This message shall indicate that a temperature reading, previously below the lower critical threshold, no longer violates the lower critical threshold.  However, the temperature violates at least one lower threshold and is not within the normal operating range.",
            "Temperature '%1' reading of %2 degrees (C) is now above the %3 lower critical threshold but remains outside of normal range.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the temperature reading.", "The temperature in degrees Celsius.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the temperature reading.", "This argument shall contain the temperature in degrees Celsius.", "This argument shall contain the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TemperatureWarning :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    TemperatureWarning(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.TemperatureWarning",
            "Indicates that a temperature reading exceeds an internal warning level.",
            "This message shall indicate that a temperature reading violates an internal warning level.  This message shall be used for temperature properties or `Sensor` resources that implement boolean trip points for a caution limit, such as from hardware mechanisms with no threshold value.  The temperature properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Temperature '%1' reading of %2 degrees (C) exceeds the warning level.",
            "Warning",
            2,
            { "string", "number" },
            { "The name or identifier of the temperature reading.", "The temperature in degrees Celsius." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the temperature reading.", "This argument shall contain the temperature in degrees Celsius." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TemperatureCritical :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    TemperatureCritical(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.TemperatureCritical",
            "Indicates that a temperature reading exceeds an internal critical level.",
            "This message shall indicate that a temperature reading violates an internal critical level.  This message shall be used for temperature properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The temperature properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Temperature '%1' reading of %2 degrees (C) exceeds the critical level.",
            "Critical",
            2,
            { "string", "number" },
            { "The name or identifier of the temperature reading.", "The temperature in degrees Celsius." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the temperature reading.", "This argument shall contain the temperature in degrees Celsius." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TemperatureNoLongerCritical :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    TemperatureNoLongerCritical(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.TemperatureNoLongerCritical",
            "Indicates that a temperature reading no longer exceeds an internal critical level but still exceeds an internal warning level.",
            "This message shall indicate that a temperature reading no longer violates an internal critical level but still violates an internal warning level.  This message shall be used for temperature properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The temperature properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Temperature '%1' reading of %2 degrees (C) no longer exceeds the critical level.",
            "Warning",
            2,
            { "string", "number" },
            { "The name or identifier of the temperature reading.", "The temperature in degrees Celsius." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the temperature reading.", "This argument shall contain the temperature in degrees Celsius." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TemperatureNormal :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    TemperatureNormal(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.TemperatureNormal",
            "Indicates that a temperature reading is now within normal operating range.",
            "This message shall indicate that a temperature reading has returned to its normal operating range.",
            "Temperature '%1' reading of %2 degrees (C) is within normal operating range.",
            "OK",
            2,
            { "string", "number" },
            { "The name or identifier of the temperature reading.", "The temperature in degrees Celsius." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the temperature reading.", "This argument shall contain the temperature in degrees Celsius." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct HumidityAboveUpperCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    HumidityAboveUpperCautionThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.HumidityAboveUpperCautionThreshold",
            "Indicates that a humidity reading is above the upper caution threshold.",
            "This message shall indicate that a humidity reading violates an upper caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCaution` or `UpperCautionUser` properties.",
            "Humidity '%1' reading of %2 percent is above the %3 upper caution threshold.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the humidity reading.", "The humidity in percent units.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the humidity reading.", "This argument shall contain the humidity as a percentage.", "This argument shall contain the value of the `Reading` property within the `UpperCaution` or `UpperCautionUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct HumidityAboveUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    HumidityAboveUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.HumidityAboveUpperCriticalThreshold",
            "Indicates that a humidity reading is above the upper critical threshold.",
            "This message shall indicate that a humidity reading violates an upper caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties.",
            "Humidity '%1' reading of %2 percent is above the %3 upper critical threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the humidity reading.", "The humidity in percent units.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the humidity reading.", "This argument shall contain the humidity as a percentage.", "This argument shall contain the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct HumidityBelowLowerCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    HumidityBelowLowerCautionThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.HumidityBelowLowerCautionThreshold",
            "Indicates that a humidity reading is below the lower caution threshold.",
            "This message shall indicate that a humidity reading violates an lower caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCaution` or `LowerCautionUser` properties.",
            "Humidity '%1' reading of %2 percent is below the %3 lower caution threshold.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the humidity reading.", "The humidity in percent units.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the humidity reading.", "This argument shall contain the humidity as a percentage.", "This argument shall contain the value of the `Reading` property within the `LowerCaution` or `LowerCautionUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct HumidityBelowLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    HumidityBelowLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.HumidityBelowLowerCriticalThreshold",
            "Indicates that a humidity reading is below the lower critical threshold.",
            "This message shall indicate that a humidity reading violates a lower critical threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties.",
            "Humidity '%1' reading of %2 percent is below the %3 lower critical threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the humidity reading.", "The humidity in percent units.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the humidity reading.", "This argument shall contain the humidity as a percentage.", "This argument shall contain the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct HumidityBelowUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    HumidityBelowUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.HumidityBelowUpperCriticalThreshold",
            "Indicates that a humidity reading is no longer above the upper critical threshold but is still outside of normal operating range.",
            "This message shall indicate that a humidity reading, previously above the critical threshold, no longer violates the critical threshold.  However, the humidity violates at least one upper threshold and is not within the normal operating range.",
            "Humidity '%1' reading of %2 percent is now below the %3 upper critical threshold but remains outside of normal range.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the humidity reading.", "The percent humidity.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the temperature reading.", "This argument shall contain the humidity as a percentage.", "This argument shall contain the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct HumidityAboveLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    HumidityAboveLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.HumidityAboveLowerCriticalThreshold",
            "Indicates that a humidity reading is no longer below the lower critical threshold but is still outside of normal operating range.",
            "This message shall indicate that a humidity reading, previously below the lower critical threshold, no longer violates the lower critical threshold.  However, the humidity violates at least one lower threshold and is not within the normal operating range.",
            "Humidity '%1' reading of %2 percent is now above the %3 lower critical threshold but remains outside of normal range.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the humidity reading.", "The percent humidity.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the humidity reading.", "This argument shall contain the humidity as a percentage.", "This argument shall contain the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct HumidityNormal :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    HumidityNormal(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.HumidityNormal",
            "Indicates that a humidity reading is now within normal operating range.",
            "This message shall indicate that a humidity reading has returned to its normal operating range.",
            "Humidity '%1' reading of %2 percent is within normal operating range.",
            "OK",
            2,
            { "string", "number" },
            { "The name or identifier of the humidity reading.", "The percent humidity." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the humidity reading.", "This argument shall contain the humidity as a percentage." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FanFailed :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FanFailed(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FanFailed",
            "Indicates that a fan has failed.",
            "This message shall indicate that a fan has failed.",
            "Fan '%1' has failed.",
            "Warning",
            1,
            { "string" },
            { "The name or identifier of the fan." },
            { "This argument shall contain a string that identifies or describes the `Fan` resource." },
            "Check the fan hardware and replace any faulty component.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FanRestored :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FanRestored(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FanRestored",
            "Indicates that a fan was repaired or restored to normal operation.",
            "This message shall indicate that a fan was repaired, replaced, or otherwise returned to normal operation.",
            "Fan '%1' was restored.",
            "OK",
            1,
            { "string" },
            { "The name or identifier of the fan." },
            { "This argument shall contain a string that identifies or describes the `Fan` resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FanRemoved :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FanRemoved(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FanRemoved",
            "Indicates that a fan was removed.",
            "This message shall indicate that a fan was removed.",
            "Fan '%1' was removed.",
            "OK",
            1,
            { "string" },
            { "The name or identifier of the fan." },
            { "This argument shall contain a string that identifies or describes the `Fan` resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FanInserted :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FanInserted(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FanInserted",
            "Indicates that a fan was inserted or installed.",
            "This message shall indicate that a fan was inserted or installed.",
            "Fan '%1' was inserted.",
            "OK",
            1,
            { "string" },
            { "The name or identifier of the fan." },
            { "This argument shall contain a string that identifies or describes the `Fan` resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FanGroupCritical :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FanGroupCritical(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FanGroupCritical",
            "Indicates that a fan group has a critical status.",
            "This message shall indicate that a fan group is operating in a critical state due to a fault or a change in configuration.",
            "Fan group '%1' is in a critical state.",
            "Critical",
            1,
            { "string" },
            { "The name or identifier of the fan group." },
            { "This argument shall contain a string that identifies or describes the group of `Fan` resources." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FanGroupWarning :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FanGroupWarning(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FanGroupWarning",
            "Indicates that a fan group has a warning status.",
            "This message shall indicate that a fan group is operating with a warning status or in a non-redundant state due to a fault or a change in configuration.",
            "Fan group '%1' is in a warning state.",
            "Warning",
            1,
            { "string" },
            { "The name or identifier of the fan group." },
            { "This argument shall contain a string that identifies or describes the group of `Fan` resources." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FanGroupNormal :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FanGroupNormal(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FanGroupNormal",
            "Indicates that a fan group has returned to normal operations.",
            "This message shall indicate that a fan group has returned to a normal operating state.",
            "Fan group '%1' is operating normally.",
            "OK",
            1,
            { "string" },
            { "The name or identifier of the fan group." },
            { "This argument shall contain a string that identifies or describes the group of `Fan` resources." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FlowRateAboveUpperCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FlowRateAboveUpperCautionThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FlowRateAboveUpperCautionThreshold",
            "Indicates that a flow rate reading is above the upper caution threshold.",
            "This message shall indicate that a flow rate reading violates an upper caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCaution` or `UpperCautionUser` properties.  Otherwise, the message shall indicate that the reading violates an internal level or threshold.",
            "Flow rate '%1' reading of %2 L/min is above the %3 upper caution threshold.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the flow rate reading.", "The flow rate in litres per minute.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the flow rate reading.", "This argument shall contain the flow rate in litres per minute.", "If the reading originated from a `Sensor` resource, this argument shall contain the value of the `Reading` property within the `UpperCaution` or `UpperCautionUser` properties of the `Sensor` resource.  Otherwise, this argument shall contain an internal level or threshold value." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FlowRateAboveUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FlowRateAboveUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FlowRateAboveUpperCriticalThreshold",
            "Indicates that a flow rate reading is above the upper critical threshold.",
            "This message shall indicate that a flow rate reading violates an upper critical threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties.",
            "Flow rate '%1' reading of %2 L/min is above the %3 upper critical threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the flow rate reading.", "The flow rate in litres per minute.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the flow rate reading.", "This argument shall contain the flow rate in litres per minute.", "This argument shall contain the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FlowRateAboveUpperFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FlowRateAboveUpperFatalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FlowRateAboveUpperFatalThreshold",
            "Indicates that a flow rate reading is above the upper fatal threshold.",
            "This message shall indicate that a flow rate reading violates an upper fatal threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperFatal` property.",
            "Flow rate '%1' reading of %2 L/min is above the %3 upper fatal threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the flow rate reading.", "The flow rate in litres per minute.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the flow rate reading.", "This argument shall contain the flow rate in litres per minute.", "This argument shall contain the value of the `Reading` property within the `UpperFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FlowRateBelowLowerCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FlowRateBelowLowerCautionThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FlowRateBelowLowerCautionThreshold",
            "Indicates that a flow rate reading is below the lower caution threshold.",
            "This message shall indicate that a flow rate reading violates a lower caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCaution` or `LowerCautionUser` properties.",
            "Flow rate '%1' reading of %2 L/min is below the %3 lower caution threshold.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the flow rate reading.", "The flow rate in litres per minute.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the flow rate reading.", "This argument shall contain the flow rate in litres per minute.", "This argument shall contain the value of the `Reading` property within the `LowerCaution` or `LowerCautionUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FlowRateBelowLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FlowRateBelowLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FlowRateBelowLowerCriticalThreshold",
            "Indicates that a flow rate reading is below the lower critical threshold.",
            "This message shall indicate that a flow rate reading violates a lower critical threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties.",
            "Flow rate '%1' reading of %2 L/min is below the %3 lower critical threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the flow rate reading.", "The flow rate in litres per minute.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the flow rate reading.", "This argument shall contain the flow rate in litres per minute.", "This argument shall contain the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FlowRateBelowLowerFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FlowRateBelowLowerFatalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FlowRateBelowLowerFatalThreshold",
            "Indicates that a flow rate reading is below the lower fatal threshold.",
            "This message shall indicate that a flow rate reading violates a lower fatal threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerFatal` property.",
            "Flow rate '%1' reading of %2 L/min is below the %3 lower fatal threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the flow rate reading.", "The flow rate in litres per minute.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the flow rate reading.", "This argument shall contain the flow rate in litres per minute.", "This argument shall contain the value of the `Reading` property within the `LowerFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FlowRateBelowUpperFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FlowRateBelowUpperFatalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FlowRateBelowUpperFatalThreshold",
            "Indicates that a flow rate reading is no longer above the upper fatal threshold but is still outside of normal operating range.",
            "This message shall indicate that a flow rate reading, previously above the upper fatal threshold, no longer violates the upper fatal threshold.  However, the flow rate violates at least one upper threshold and is not within the normal operating range.",
            "Flow rate '%1' reading of %2 L/min is now below the %3 upper fatal threshold but remains outside of normal range.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the flow rate reading.", "The flow rate in litres per minute.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the flow rate reading.", "This argument shall contain the flow rate in litres per minute.", "This argument shall contain the value of the `Reading` property within the `UpperFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FlowRateBelowUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FlowRateBelowUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FlowRateBelowUpperCriticalThreshold",
            "Indicates that a flow rate reading is no longer above the upper critical threshold but is still outside of normal operating range.",
            "This message shall indicate that a flow rate reading, previously above the upper critical threshold, no longer violates the upper critical threshold.  However, the flow rate violates at least one upper threshold and is not within the normal operating range.",
            "Flow rate '%1' reading of %2 L/min is now below the %3 upper critical threshold but remains outside of normal range.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the flow rate reading.", "The flow rate in litres per minute.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the flow rate reading.", "This argument shall contain the flow rate in litres per minute.", "This argument shall contain the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FlowRateAboveLowerFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FlowRateAboveLowerFatalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FlowRateAboveLowerFatalThreshold",
            "Indicates that a flow rate reading is no longer below the lower fatal threshold but is still outside of normal operating range.",
            "This message shall indicate that a flow rate reading, previously below the lower fatal threshold, no longer violates the lower fatal threshold.  However, the flow rate violates at least one lower threshold and is not within the normal operating range.",
            "Flow rate '%1' reading of %2 L/min is now above the %3 lower fatal threshold but remains outside of normal range.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the flow rate reading.", "The flow rate in litres per minute.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the flow rate reading.", "This argument shall contain the flow rate in litres per minute.", "This argument shall contain the value of the `Reading` property within the `LowerFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FlowRateAboveLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FlowRateAboveLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FlowRateAboveLowerCriticalThreshold",
            "Indicates that a flow rate reading is no longer below the lower critical threshold but is still outside of normal operating range.",
            "This message shall indicate that a flow rate reading, previously below the lower critical threshold, no longer violates the lower critical threshold.  However, the flow rate violates at least one lower threshold and is not within the normal operating range.",
            "Flow rate '%1' reading of %2 L/min is now above the %3 lower critical threshold but remains outside of normal range.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the flow rate reading.", "The flow rate in litres per minute.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the flow rate reading.", "This argument shall contain the flow rate in litres per minute.", "This argument shall contain the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FlowRateWarning :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    FlowRateWarning(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FlowRateWarning",
            "Indicates that a flow rate reading exceeds an internal warning level.",
            "This message shall indicate that a flow rate reading violates an internal warning level.  This message shall be used for flow rate properties or `Sensor` resources that implement boolean trip points for a caution limit, such as from hardware mechanisms with no threshold value.  The flow rate properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Flow rate '%1' reading of %2 L/min exceeds the warning level.",
            "Warning",
            2,
            { "string", "number" },
            { "The name or identifier of the flow rate reading.", "The flow rate in litres per minute." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the flow rate reading.", "This argument shall contain the flow rate in litres per minute." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FlowRateCritical :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    FlowRateCritical(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FlowRateCritical",
            "Indicates that a flow rate reading exceeds an internal critical level.",
            "This message shall indicate that a flow rate reading violates an internal critical level.  This message shall be used for flow rate properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The flow rate properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Flow rate '%1' reading of %2 L/min exceeds the critical level.",
            "Critical",
            2,
            { "string", "number" },
            { "The name or identifier of the flow rate reading.", "The flow rate in litres per minute." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the flow rate reading.", "This argument shall contain the flow rate in litres per minute." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FlowRateFatal :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    FlowRateFatal(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FlowRateFatal",
            "Indicates that a flow rate reading exceeds an internal fatal level or flow rate reading is zero (0).",
            "This message shall indicate that a flow rate reading violates an internal fatal level.  This message shall be used for flow rate properties or `Sensor` resources that implement boolean trip points for a fatal limit, such as from hardware mechanisms with no threshold value.  The flow rate properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Flow rate '%1' reading of %2 L/min exceeds the fatal level.",
            "Critical",
            2,
            { "string", "number" },
            { "The name or identifier of the flow rate reading.", "The flow rate in litres per minute." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the flow rate reading.", "This argument shall contain the flow rate in litres per minute." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FlowRateNoLongerCritical :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    FlowRateNoLongerCritical(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FlowRateNoLongerCritical",
            "Indicates that a flow rate reading no longer exceeds an internal critical level but still exceeds an internal warning level.",
            "This message shall indicate that a flow rate reading no longer violates an internal critical level but still violates an internal warning level.  This message shall be used for flow rate properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The flow rate properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Flow rate '%1' reading of %2 L/min no longer exceeds the critical level.",
            "Warning",
            2,
            { "string", "number" },
            { "The name or identifier of the flow rate reading.", "The flow rate in litres per minute." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the flow rate reading.", "This argument shall contain the flow rate in litres per minute." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FlowRateNoLongerFatal :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    FlowRateNoLongerFatal(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FlowRateNoLongerFatal",
            "Indicates that a flow rate reading is no longer exceeds an internal fatal level but is still exceeds an internal critical level.",
            "This message shall indicate that a flow rate reading no longer violates an internal fatal level but still violates an internal critical level.  This message shall be used for flow rate properties or `Sensor` resources that implement boolean trip points for a fatal limit, such as from hardware mechanisms with no threshold value.  The flow rate properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Flow rate '%1' reading of %2 L/min no longer exceeds the fatal level.",
            "Critical",
            2,
            { "string", "number" },
            { "The name or identifier of the flow rate reading.", "The flow rate in litres per minute." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the flow rate reading.", "This argument shall contain the flow rate in litres per minute." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FlowRateNormal :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    FlowRateNormal(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FlowRateNormal",
            "Indicates that a flow rate reading is now within normal operating range.",
            "This message shall indicate that a flow rate reading has returned to its normal operating range.",
            "Flow rate '%1' reading of %2 L/min is within normal operating range.",
            "OK",
            2,
            { "string", "number" },
            { "The name or identifier of the flow rate reading.", "The flow rate in litres per minute." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the flow rate reading.", "This argument shall contain the flow rate in litres per minute." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PressureAboveUpperCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PressureAboveUpperCautionThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.PressureAboveUpperCautionThreshold",
            "Indicates that a pressure reading is above the upper caution threshold.",
            "This message shall indicate that a pressure reading violates an upper caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCaution` or `UpperCautionUser` properties.  Otherwise, the message shall indicate that the reading violates an internal level or threshold.",
            "Pressure '%1' reading of %2 kPa is above the %3 upper caution threshold.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the pressure reading.", "The pressure in kilopascals.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the pressure reading.", "This argument shall contain the pressure in kilopascals.", "If the reading originated from a `Sensor` resource, this argument shall contain the value of the `Reading` property within the `UpperCaution` or `UpperCautionUser` properties of the `Sensor` resource.  Otherwise, this argument shall contain an internal level or threshold value." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PressureAboveUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PressureAboveUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.PressureAboveUpperCriticalThreshold",
            "Indicates that a pressure reading is above the upper critical threshold.",
            "This message shall indicate that a pressure reading violates an upper critical threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties.",
            "Pressure '%1' reading of %2 kPa is above the %3 upper critical threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the pressure reading.", "The pressure in kilopascals.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the pressure reading.", "This argument shall contain the pressure in kilopascals.", "This argument shall contain the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PressureAboveUpperFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PressureAboveUpperFatalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.PressureAboveUpperFatalThreshold",
            "Indicates that a pressure reading is above the upper fatal threshold.",
            "This message shall indicate that a pressure reading violates an upper fatal threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperFatal` property.",
            "Pressure '%1' reading of %2 kPa is above the %3 upper fatal threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the pressure reading.", "The pressure in kilopascals.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the pressure reading.", "This argument shall contain the pressure in kilopascals.", "This argument shall contain the value of the `Reading` property within the `UpperFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PressureBelowLowerCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PressureBelowLowerCautionThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.PressureBelowLowerCautionThreshold",
            "Indicates that a pressure reading is below the lower caution threshold.",
            "This message shall indicate that a pressure reading violates a lower caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCaution` or `LowerCautionUser` properties.",
            "Pressure '%1' reading of %2 kPa is below the %3 lower caution threshold.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the pressure reading.", "The pressure in kilopascals.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the pressure reading.", "This argument shall contain the pressure in kilopascals.", "This argument shall contain the value of the `Reading` property within the `LowerCaution` or `LowerCautionUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PressureBelowLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PressureBelowLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.PressureBelowLowerCriticalThreshold",
            "Indicates that a pressure reading is below the lower critical threshold.",
            "This message shall indicate that a pressure reading violates a lower critical threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties.",
            "Pressure '%1' reading of %2 kPa is below the %3 lower critical threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the pressure reading.", "The pressure in kilopascals.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the pressure reading.", "This argument shall contain the pressure in kilopascals.", "This argument shall contain the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PressureBelowLowerFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PressureBelowLowerFatalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.PressureBelowLowerFatalThreshold",
            "Indicates that a pressure reading is below the lower fatal threshold.",
            "This message shall indicate that a pressure reading violates a lower fatal threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerFatal` property.",
            "Pressure '%1' reading of %2 kPa is below the %3 lower fatal threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the pressure reading.", "The pressure in kilopascals.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the pressure reading.", "This argument shall contain the pressure in kilopascals.", "This argument shall contain the value of the `Reading` property within the `LowerFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PressureBelowUpperFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PressureBelowUpperFatalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.PressureBelowUpperFatalThreshold",
            "Indicates that a pressure reading is no longer above the upper fatal threshold but is still outside of normal operating range.",
            "This message shall indicate that a pressure reading, previously above the upper fatal threshold, no longer violates the upper fatal threshold.  However, the pressure violates at least one upper threshold and is not within the normal operating range.",
            "Pressure '%1' reading of %2 kPa is now below the %3 upper fatal threshold but remains outside of normal range.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the pressure reading.", "The pressure in kilopascals.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the pressure reading.", "This argument shall contain the pressure in kilopascals.", "This argument shall contain the value of the `Reading` property within the `UpperFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PressureBelowUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PressureBelowUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.PressureBelowUpperCriticalThreshold",
            "Indicates that a pressure reading is no longer above the upper critical threshold but is still outside of normal operating range.",
            "This message shall indicate that a pressure reading, previously above the upper critical threshold, no longer violates the upper critical threshold.  However, the pressure violates at least one upper threshold and is not within the normal operating range.",
            "Pressure '%1' reading of %2 kPa is now below the %3 upper critical threshold but remains outside of normal range.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the pressure reading.", "The pressure in kilopascals.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the pressure reading.", "This argument shall contain the pressure in kilopascals.", "This argument shall contain the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PressureAboveLowerFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PressureAboveLowerFatalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.PressureAboveLowerFatalThreshold",
            "Indicates that a pressure reading is no longer below the lower fatal threshold but is still outside of normal operating range.",
            "This message shall indicate that a pressure reading, previously below the lower fatal threshold, no longer violates the lower fatal threshold.  However, the pressure violates at least one lower threshold and is not within the normal operating range.",
            "Pressure '%1' reading of %2 kPa is now above the %3 lower fatal threshold but remains outside of normal range.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the pressure reading.", "The pressure in kilopascals.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the pressure reading.", "This argument shall contain the pressure in kilopascals.", "This argument shall contain the value of the `Reading` property within the `LowerFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PressureAboveLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PressureAboveLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.PressureAboveLowerCriticalThreshold",
            "Indicates that a pressure reading is no longer below the lower critical threshold but is still outside of normal operating range.",
            "This message shall indicate that a pressure reading, previously below the lower critical threshold, no longer violates the lower critical threshold.  However, the pressure violates at least one lower threshold and is not within the normal operating range.",
            "Pressure '%1' reading of %2 kPa is now above the %3 lower critical threshold but remains outside of normal range.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the pressure reading.", "The pressure in kilopascals.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the pressure reading.", "This argument shall contain the pressure in kilopascals.", "This argument shall contain the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PressureWarning :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    PressureWarning(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.PressureWarning",
            "Indicates that a pressure reading exceeds an internal warning level.",
            "This message shall indicate that a pressure reading violates an internal warning level.  This message shall be used for pressure properties or `Sensor` resources that implement boolean trip points for a caution limit, such as from hardware mechanisms with no threshold value.  The pressure properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Pressure '%1' reading of %2 kPa exceeds the warning level.",
            "Warning",
            2,
            { "string", "number" },
            { "The name or identifier of the pressure reading.", "The pressure in kilopascals." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the pressure reading.", "This argument shall contain the pressure in kilopascals." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PressureCritical :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    PressureCritical(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.PressureCritical",
            "Indicates that a pressure reading exceeds an internal critical level.",
            "This message shall indicate that a pressure reading violates an internal critical level.  This message shall be used for pressure properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The pressure properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Pressure '%1' reading of %2 kPa exceeds the critical level.",
            "Critical",
            2,
            { "string", "number" },
            { "The name or identifier of the pressure reading.", "The pressure in kilopascals." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the pressure reading.", "This argument shall contain the pressure in kilopascals." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PressureNoLongerCritical :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    PressureNoLongerCritical(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.PressureNoLongerCritical",
            "Indicates that a pressure reading no longer violates an internal critical level but still violates an internal warning level.",
            "This message shall indicate that a pressure reading no longer violates an internal critical level but still violates an internal warning level.  This message shall be used for pressure properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The pressure properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Pressure '%1' reading of %2 kPa no longer violates the critical level.",
            "Warning",
            2,
            { "string", "number" },
            { "The name or identifier of the pressure reading.", "The pressure in kilopascals." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the pressure reading.", "This argument shall contain the pressure in kilopascals." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PressureNormal :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    PressureNormal(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.PressureNormal",
            "Indicates that a pressure reading is now within normal operating range.",
            "This message shall indicate that a pressure reading has returned to its normal operating range.",
            "Pressure '%1' reading of %2 kPa is within normal operating range.",
            "OK",
            2,
            { "string", "number" },
            { "The name or identifier of the pressure reading.", "The pressure in kilopascals." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the pressure reading.", "This argument shall contain the pressure in kilopascals." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FluidLevelAboveUpperCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FluidLevelAboveUpperCautionThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FluidLevelAboveUpperCautionThreshold",
            "Indicates that a fluid level reading is above the upper caution threshold.",
            "This message shall indicate that a fluid level reading violates an upper caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCaution` or `UpperCautionUser` properties.  Otherwise, the message shall indicate that the reading violates an internal level or threshold.",
            "Fluid level '%1' reading of %2 percent is above the %3 upper caution threshold.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the fluid level reading.", "The fluid level in percent.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the fluid level reading.", "This argument shall contain the fluid level in percent.", "If the reading originated from a `Sensor` resource, this argument shall contain the value of the `Reading` property within the `UpperCaution` or `UpperCautionUser` properties of the `Sensor` resource.  Otherwise, this argument shall contain an internal level or threshold value." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FluidLevelAboveUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FluidLevelAboveUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FluidLevelAboveUpperCriticalThreshold",
            "Indicates that a fluid level reading is above the upper critical threshold.",
            "This message shall indicate that a fluid level reading violates an upper critical threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties.",
            "Fluid level '%1' reading of %2 percent is above the %3 upper critical threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the fluid level reading.", "The fluid level in percent.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the fluid level reading.", "This argument shall contain the fluid level in percent.", "This argument shall contain the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FluidLevelAboveUpperFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FluidLevelAboveUpperFatalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FluidLevelAboveUpperFatalThreshold",
            "Indicates that a fluid level reading is above the upper fatal threshold.",
            "This message shall indicate that a fluid level reading violates an upper fatal threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperFatal` property.",
            "Fluid level '%1' reading of %2 percent is above the %3 upper fatal threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the fluid level reading.", "The fluid level in percent.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the fluid level reading.", "This argument shall contain the fluid level in percent.", "This argument shall contain the value of the `Reading` property within the `UpperFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FluidLevelBelowLowerCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FluidLevelBelowLowerCautionThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FluidLevelBelowLowerCautionThreshold",
            "Indicates that a fluid level reading is below the lower caution threshold.",
            "This message shall indicate that a fluid level reading violates a lower caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCaution` or `LowerCautionUser` properties.",
            "Fluid level '%1' reading of %2 percent is below the %3 lower caution threshold.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the fluid level reading.", "The fluid level in percent.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the fluid level reading.", "This argument shall contain the fluid level in percent.", "This argument shall contain the value of the `Reading` property within the `LowerCaution` or `LowerCautionUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FluidLevelBelowLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FluidLevelBelowLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FluidLevelBelowLowerCriticalThreshold",
            "Indicates that a fluid level reading is below the lower critical threshold.",
            "This message shall indicate that a fluid level reading violates a lower critical threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties.",
            "Fluid level '%1' reading of %2 percent is below the %3 lower critical threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the fluid level reading.", "The fluid level in percent.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the fluid level reading.", "This argument shall contain the fluid level in percent.", "This argument shall contain the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FluidLevelBelowLowerFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FluidLevelBelowLowerFatalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FluidLevelBelowLowerFatalThreshold",
            "Indicates that a fluid level reading is below the lower fatal threshold.",
            "This message shall indicate that a fluid level reading violates a lower fatal threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerFatal` property.",
            "Fluid level '%1' reading of %2 percent is below the %3 lower fatal threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the fluid level reading.", "The fluid level in percent.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the fluid level reading.", "This argument shall contain the fluid level in percent.", "This argument shall contain the value of the `Reading` property within the `LowerFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FluidLevelBelowUpperFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FluidLevelBelowUpperFatalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FluidLevelBelowUpperFatalThreshold",
            "Indicates that a fluid level reading is no longer above the upper fatal threshold but is still outside of normal operating range.",
            "This message shall indicate that a fluid level reading, previously above the upper fatal threshold, no longer violates the upper fatal threshold.  However, the fluid level violates at least one upper threshold and is not within the normal operating range.",
            "Fluid level '%1' reading of %2 percent is now below the %3 upper fatal threshold but remains outside of normal range.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the fluid level reading.", "The fluid level in percent.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the fluid level reading.", "This argument shall contain the fluid level in percent.", "This argument shall contain the value of the `Reading` property within the `UpperFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FluidLevelBelowUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FluidLevelBelowUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FluidLevelBelowUpperCriticalThreshold",
            "Indicates that a fluid level reading is no longer above the upper critical threshold but is still outside of normal operating range.",
            "This message shall indicate that a fluid level reading, previously above the upper critical threshold, no longer violates the upper critical threshold.  However, the fluid level violates at least one upper threshold and is not within the normal operating range.",
            "Fluid level '%1' reading of %2 percent is now below the %3 upper critical threshold but remains outside of normal range.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the fluid level reading.", "The fluid level in percent.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the fluid level reading.", "This argument shall contain the fluid level in percent.", "This argument shall contain the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FluidLevelAboveLowerFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FluidLevelAboveLowerFatalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FluidLevelAboveLowerFatalThreshold",
            "Indicates that a fluid level reading is no longer below the lower fatal threshold but is still outside of normal operating range.",
            "This message shall indicate that a fluid level reading, previously below the lower fatal threshold, no longer violates the lower fatal threshold.  However, the fluid level violates at least one lower threshold and is not within the normal operating range.",
            "Fluid level '%1' reading of %2 percent is now above the %3 lower fatal threshold but remains outside of normal range.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the fluid level reading.", "The fluid level in percent.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the fluid level reading.", "This argument shall contain the fluid level in percent.", "This argument shall contain the value of the `Reading` property within the `LowerFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FluidLevelAboveLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FluidLevelAboveLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FluidLevelAboveLowerCriticalThreshold",
            "Indicates that a fluid level reading is no longer below the lower critical threshold but is still outside of normal operating range.",
            "This message shall indicate that a fluid level reading, previously below the lower critical threshold, no longer violates the lower critical threshold.  However, the fluid level violates at least one lower threshold and is not within the normal operating range.",
            "Fluid level '%1' reading of %2 percent is now above the %3 lower critical threshold but remains outside of normal range.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the fluid level reading.", "The fluid level in percent.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the fluid level reading.", "This argument shall contain the fluid level in percent.", "This argument shall contain the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FluidLevelWarning :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FluidLevelWarning(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FluidLevelWarning",
            "Indicates that a fluid level reading violates an internal warning level.",
            "This message shall indicate that a fluid level reading violates an internal warning level.  This message shall be used for fluid level properties or `Sensor` resources that implement boolean trip points for a caution limit, such as from hardware mechanisms with no threshold value.  The fluid level properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Fluid level '%1' reading violates the warning level.",
            "Warning",
            1,
            { "string" },
            { "The name or identifier of the fluid level reading." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the fluid level reading." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FluidLevelCritical :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FluidLevelCritical(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FluidLevelCritical",
            "Indicates that a fluid level reading violates an internal critical level.",
            "This message shall indicate that a fluid level reading violates an internal critical level.  This message shall be used for fluid level properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The fluid level properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Fluid level '%1' reading violates the critical level.",
            "Critical",
            1,
            { "string" },
            { "The name or identifier of the fluid level reading." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the fluid level reading." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FluidLevelNoLongerCritical :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FluidLevelNoLongerCritical(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FluidLevelNoLongerCritical",
            "Indicates that a fluid level reading no longer violates an internal critical level but still violates an internal warning level.",
            "This message shall indicate that a fluid level reading no longer violates an internal critical level but still violates an internal warning level.  This message shall be used for fluid level properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The fluid level properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Fluid level '%1' reading no longer violates the critical level.",
            "Warning",
            1,
            { "string" },
            { "The name or identifier of the fluid level reading." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the fluid level reading." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FluidLevelNormal :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FluidLevelNormal(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FluidLevelNormal",
            "Indicates that a fluid level reading is now within normal operating range.",
            "This message shall indicate that a fluid level reading has returned to its normal operating range.",
            "Fluid level '%1' reading is within normal operating range.",
            "OK",
            1,
            { "string" },
            { "The name or identifier of the fluid level reading." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the fluid level reading." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct LeakDetectedCritical :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    LeakDetectedCritical(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.LeakDetectedCritical",
            "Indicates that a leak detector is in a critical state.",
            "This message shall indicate a leak detector is in a critical state.",
            "Leak detector '%1' reports a critical level leak.",
            "Critical",
            1,
            { "string" },
            { "The name or identifier of the leak detector." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the leak detector." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct LeakDetectedWarning :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    LeakDetectedWarning(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.LeakDetectedWarning",
            "Indicates that a leak detector is in a warning state.",
            "This message shall indicate a leak detector is in a warning state.",
            "Leak detector '%1' reports a warning level leak.",
            "Warning",
            1,
            { "string" },
            { "The name or identifier of the leak detector." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the leak detector." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct LeakDetectedNormal :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    LeakDetectedNormal(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.LeakDetectedNormal",
            "Indicates that a leak detector is within normal operating range.",
            "This message shall indicate that a leak detector returned to its normal operating state.",
            "Leak detector '%1' has returned to normal.",
            "OK",
            1,
            { "string" },
            { "The name or identifier of the leak detector." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the leak detector." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PumpRemoved :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PumpRemoved(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.PumpRemoved",
            "Indicates that a pump was removed.",
            "This message shall indicate that a pump was removed.",
            "Pump '%1' was removed.",
            "OK",
            1,
            { "string" },
            { "The name or identifier of the pump." },
            { "This argument shall contain a string that identifies or describes the `Pump` resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PumpInserted :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PumpInserted(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.PumpInserted",
            "Indicates that a pump was inserted or installed.",
            "This message shall indicate that a pump was inserted or installed.",
            "Pump '%1' was inserted.",
            "OK",
            1,
            { "string" },
            { "The name or identifier of the pump." },
            { "This argument shall contain a string that identifies or describes the `Pump` resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PumpFailed :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PumpFailed(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.PumpFailed",
            "Indicates that a pump has failed.",
            "This message shall indicate that a pump has failed.",
            "Pump '%1' has failed.",
            "Warning",
            1,
            { "string" },
            { "The name or identifier of the pump." },
            { "This argument shall contain a string that identifies or describes the `Pump` resource." },
            "Check the pump hardware and replace any faulty component.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PumpRestored :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PumpRestored(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.PumpRestored",
            "Indicates that a pump was repaired or restored to normal operation.",
            "This message shall indicate that a pump was repaired, replaced, or otherwise returned to normal operation.",
            "Pump '%1' was restored.",
            "OK",
            1,
            { "string" },
            { "The name or identifier of the pump." },
            { "This argument shall contain a string that identifies or describes the `Pump` resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FilterRemoved :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FilterRemoved(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FilterRemoved",
            "Indicates that a filter was removed.",
            "This message shall indicate that a filter was removed.",
            "Filter '%1' was removed.",
            "OK",
            1,
            { "string" },
            { "The name or identifier of the filter." },
            { "This argument shall contain a string that identifies or describes the `Filter` resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FilterInserted :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FilterInserted(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FilterInserted",
            "Indicates that a filter was inserted or installed.",
            "This message shall indicate that a filter was inserted or installed.",
            "Filter '%1' was inserted.",
            "OK",
            1,
            { "string" },
            { "The name or identifier of the filter." },
            { "This argument shall contain a string that identifies or describes the `Filter` resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FilterRequiresService :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FilterRequiresService(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FilterRequiresService",
            "Indicates that a filter requires service.",
            "This message shall indicate that a filter requires service.  Examples include a dirty filter or a filter reaching the end of its life.",
            "Filter '%1' requires service.",
            "Warning",
            1,
            { "string" },
            { "The name or identifier of the filter." },
            { "This argument shall contain a string that identifies or describes the `Filter` resource." },
            "Replace the filter or filter media.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FilterRestored :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FilterRestored(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FilterRestored",
            "Indicates that a filter was repaired or restored to normal operation.",
            "This message shall indicate that a filter was repaired, replaced, or otherwise returned to normal operation.",
            "Filter '%1' was restored.",
            "OK",
            1,
            { "string" },
            { "The name or identifier of the filter." },
            { "This argument shall contain a string that identifies or describes the `Filter` resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FluidQualityWarning :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FluidQualityWarning(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FluidQualityWarning",
            "Indicates that a fluid quality reading exceeds an internal warning level.",
            "This message shall indicate that a fluid quality reading violates an internal warning level.  This message shall be used for fluid quality properties or `Sensor` resources that implement boolean trip points for a caution limit, such as from hardware mechanisms with no threshold value.  The fluid quality properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Fluid quality '%1' reading exceeds the warning level.",
            "Warning",
            1,
            { "string" },
            { "The name or identifier of the fluid quality reading." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the fluid quality reading." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FluidQualityCritical :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FluidQualityCritical(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FluidQualityCritical",
            "Indicates that a fluid quality reading exceeds an internal critical level.",
            "This message shall indicate that a fluid quality reading violates an internal critical level.  This message shall be used for fluid quality properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The fluid quality properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Fluid quality '%1' reading exceeds the critical level.",
            "Critical",
            1,
            { "string" },
            { "The name or identifier of the fluid quality reading." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the fluid quality reading." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FluidQualityNoLongerCritical :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FluidQualityNoLongerCritical(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FluidQualityNoLongerCritical",
            "Indicates that a fluid quality reading no longer exceeds an internal critical level but still exceeds an internal warning level.",
            "This message shall indicate that a fluid quality reading no longer violates an internal critical level but still violates an internal warning level.  This message shall be used for fluid quality properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The fluid quality properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Fluid quality '%1' reading no longer exceeds the critical level.",
            "Warning",
            1,
            { "string" },
            { "The name or identifier of the fluid quality reading." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the fluid quality reading." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FluidQualityNormal :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FluidQualityNormal(Args... args) :
        RedfishMessage(
            "Environmental.1.1.0.FluidQualityNormal",
            "Indicates that a fluid quality reading is now within normal operating range.",
            "This message shall indicate that a fluid quality reading has returned to its normal operating range.",
            "Fluid quality '%1' reading is within normal operating range.",
            "OK",
            1,
            { "string" },
            { "The name or identifier of the fluid quality reading." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the fluid quality reading." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace Environmental_1_1_0
}; // namespace registries
}; // namespace redfish
