#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace Environmental_1_0_1
{

struct [[deprecated ("Use Environmental_1_1_0")]]
TemperatureAboveUpperCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    TemperatureAboveUpperCautionThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.TemperatureAboveUpperCautionThreshold",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
TemperatureAboveUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    TemperatureAboveUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.TemperatureAboveUpperCriticalThreshold",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
TemperatureAboveUpperFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    TemperatureAboveUpperFatalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.TemperatureAboveUpperFatalThreshold",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
TemperatureBelowLowerCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    TemperatureBelowLowerCautionThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.TemperatureBelowLowerCautionThreshold",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
TemperatureBelowLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    TemperatureBelowLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.TemperatureBelowLowerCriticalThreshold",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
TemperatureBelowLowerFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    TemperatureBelowLowerFatalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.TemperatureBelowLowerFatalThreshold",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
TemperatureBelowUpperFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    TemperatureBelowUpperFatalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.TemperatureBelowUpperFatalThreshold",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
TemperatureBelowUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    TemperatureBelowUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.TemperatureBelowUpperCriticalThreshold",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
TemperatureAboveLowerFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    TemperatureAboveLowerFatalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.TemperatureAboveLowerFatalThreshold",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
TemperatureAboveLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    TemperatureAboveLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.TemperatureAboveLowerCriticalThreshold",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
TemperatureWarning :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    TemperatureWarning(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.TemperatureWarning",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
TemperatureCritical :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    TemperatureCritical(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.TemperatureCritical",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
TemperatureNoLongerCritical :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    TemperatureNoLongerCritical(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.TemperatureNoLongerCritical",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
TemperatureNormal :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    TemperatureNormal(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.TemperatureNormal",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
HumidityAboveUpperCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    HumidityAboveUpperCautionThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.HumidityAboveUpperCautionThreshold",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
HumidityAboveUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    HumidityAboveUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.HumidityAboveUpperCriticalThreshold",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
HumidityBelowLowerCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    HumidityBelowLowerCautionThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.HumidityBelowLowerCautionThreshold",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
HumidityBelowLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    HumidityBelowLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.HumidityBelowLowerCriticalThreshold",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
HumidityBelowUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    HumidityBelowUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.HumidityBelowUpperCriticalThreshold",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
HumidityAboveLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    HumidityAboveLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.HumidityAboveLowerCriticalThreshold",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
HumidityNormal :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    HumidityNormal(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.HumidityNormal",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
FanFailed :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FanFailed(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.FanFailed",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
FanRestored :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FanRestored(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.FanRestored",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
FanRemoved :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FanRemoved(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.FanRemoved",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
FanInserted :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FanInserted(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.FanInserted",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
FanGroupCritical :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FanGroupCritical(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.FanGroupCritical",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
FanGroupWarning :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FanGroupWarning(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.FanGroupWarning",
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

struct [[deprecated ("Use Environmental_1_1_0")]]
FanGroupNormal :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    FanGroupNormal(Args... args) :
        RedfishMessage(
            "Environmental.1.0.1.FanGroupNormal",
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

}; // namespace Environmental_1_0_1
}; // namespace registries
}; // namespace redfish
