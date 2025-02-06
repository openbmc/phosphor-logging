#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace Power_1_0_0
{

struct [[deprecated ("Use Power_1_0_1")]]
VoltageAboveUpperCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    VoltageAboveUpperCautionThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.VoltageAboveUpperCautionThreshold",
            "Indicates that a voltage reading is above the upper caution threshold.",
            "This message shall be used to indicate that a voltage reading violates an upper caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCaution` or `UpperCautionUser` properties.",
            "Voltage '%1' reading of %2 volts is above the %3 upper caution threshold.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the voltage reading.", "The voltage in volts.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the voltage reading.", "This argument shall contain the voltage in volts.", "This argument shall contain the value of the `Reading` property within the `UpperCaution` or `UpperCautionUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
VoltageAboveUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    VoltageAboveUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.VoltageAboveUpperCriticalThreshold",
            "Indicates that a voltage reading is above the upper critical threshold.",
            "This message shall be used to indicate that a voltage reading violates an upper critical threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties.",
            "Voltage '%1' reading of %2 volts is above the %3 upper critical threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the voltage reading.", "The voltage in volts.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the voltage reading.", "This argument shall contain the voltage in volts.", "This argument shall contain the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
VoltageAboveUpperFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    VoltageAboveUpperFatalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.VoltageAboveUpperFatalThreshold",
            "Indicates that a voltage reading is above the upper fatal threshold.",
            "This message shall be used to indicate that a voltage reading violates an upper fatal threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperFatal` property.",
            "Voltage '%1' reading of %2 volts is above the %3 upper fatal threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the voltage reading.", "The voltage in volts.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the voltage reading.", "This argument shall contain the voltage in volts.", "This argument shall contain the value of the `Reading` property within the `UpperFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
VoltageBelowLowerCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    VoltageBelowLowerCautionThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.VoltageBelowLowerCautionThreshold",
            "Indicates that a voltage reading is below the lower caution threshold.",
            "This message shall be used to indicate that a voltage reading violates a lower caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCaution` or `LowerCautionUser` properties.",
            "Voltage '%1' reading of %2 volts is below the %3 lower caution threshold.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the voltage reading.", "The voltage in volts.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the voltage reading.", "This argument shall contain the voltage in volts.", "This argument shall contain the value of the `Reading` property within the `LowerCaution` or `LowerCautionUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
VoltageBelowLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    VoltageBelowLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.VoltageBelowLowerCriticalThreshold",
            "Indicates that a voltage reading is below the lower critcal threshold.",
            "This message shall be used to indicate that a voltage reading violates a lower critical threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties.",
            "Voltage '%1' reading of %2 volts is below the %3 lower critical threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the voltage reading.", "The voltage in volts.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the voltage reading.", "This argument shall contain the voltage in volts.", "This argument shall contain the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
VoltageBelowLowerFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    VoltageBelowLowerFatalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.VoltageBelowLowerFatalThreshold",
            "Indicates that a voltage reading is below the lower fatal threshold.",
            "This message shall be used to indicate that a voltage reading violates a lower fatal threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerFatal` property.",
            "Voltage '%1' reading of %2 volts is below the %3 lower fatal threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the voltage reading.", "The voltage in volts.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the voltage reading.", "This argument shall contain the voltage in volts.", "This argument shall contain the value of the `Reading` property within the `LowerFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
VoltageBelowUpperFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    VoltageBelowUpperFatalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.VoltageBelowUpperFatalThreshold",
            "Indicates that a voltage reading is no longer above the upper fatal threshold, but is still outside of normal operating range.",
            "This message shall be used to indicate that a voltage reading, previously above the fatal threshold, no longer violates the upper fatal threshold.  However, the voltage violates at least one upper threshold and is not within the normal operating range.",
            "Voltage '%1' reading of %2 volts is now below the %3 upper fatal threshold but remains outside of normal range.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the voltage reading.", "The voltage in volts.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the voltage reading.", "This argument shall contain the voltage in volts.", "This argument shall contain the value of the `Reading` property within the `UpperFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
VoltageBelowUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    VoltageBelowUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.VoltageBelowUpperCriticalThreshold",
            "Indicates that a voltage reading is no longer above the upper critical threshold, but is still outside of normal operating range.",
            "This message shall be used to indicate that a voltage reading, previously above the critical threshold, no longer violates the upper critical threshold.  However, the voltage violates at least one upper threshold and is not within the normal operating range.",
            "Voltage '%1' reading of %2 volts is now below the %3 upper critical threshold but remains outside of normal range.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the voltage reading.", "The voltage in volts.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the voltage reading.", "This argument shall contain the voltage in volts.", "This argument shall contain the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
VoltageAboveLowerFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    VoltageAboveLowerFatalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.VoltageAboveLowerFatalThreshold",
            "Indicates that a voltage reading is no longer below the lower fatal threshold, but is still outside of normal operating range.",
            "This message shall be used to indicate that a voltage reading, previously below the fatal threshold, no longer violates the lower fatal threshold.  However, the voltage violates at least one lower threshold and is not within the normal operating range.",
            "Voltage '%1' reading of %2 volts is now above the %3 lower fatal threshold but remains outside of normal range.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the voltage reading.", "The voltage in volts.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the voltage reading.", "This argument shall contain the voltage in volts.", "This argument shall contain the value of the `Reading` property within the `LowerFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
VoltageAboveLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    VoltageAboveLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.VoltageAboveLowerCriticalThreshold",
            "Indicates that a voltage reading is no longer below the lower critical threshold, but is still outside of normal operating range.",
            "This message shall be used to indicate that a voltage reading, previously below the lower critical threshold, no longer violates the lower critical threshold.  However, the voltage violates at least one lower threshold and is not within the normal operating range.",
            "Voltage '%1' reading of %2 volts is now above the %3 lower critical threshold but remains outside of normal range.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the voltage reading.", "The voltage in volts.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the voltage reading.", "This argument shall contain the voltage in volts.", "This argument shall contain the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
VoltageWarning :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    VoltageWarning(Args... args) :
        RedfishMessage(
            "Power.1.0.0.VoltageWarning",
            "Indicates that a voltage reading exceeds an internal warning level.",
            "This message shall be used to indicate that a voltage reading violates  an internal warning level.  This message shall be used for voltage properties or `Sensor` resources that implement boolean trip points for a caution limit, such as from hardware mechanisms with no threshold value.  The voltage properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Voltage '%1' reading of %2 volts exceeds the warning level.",
            "Warning",
            2,
            { "string", "number" },
            { "The name or identifier of the voltage reading.", "The voltage in volts." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the voltage reading.", "This argument shall contain the voltage in volts." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
VoltageCritical :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    VoltageCritical(Args... args) :
        RedfishMessage(
            "Power.1.0.0.VoltageCritical",
            "Indicates that a voltage reading exceeds an internal critical level.",
            "This message shall be used to indicate that a voltage reading violates an internal critical level.  This message shall be used for voltage properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The voltage properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Voltage '%1' reading of %2 volts exceeds the critical level.",
            "Critical",
            2,
            { "string", "number" },
            { "The name or identifier of the voltage reading.", "The voltage in volts." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the voltage reading.", "This argument shall contain the voltage in volts." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
VoltageNoLongerCritical :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    VoltageNoLongerCritical(Args... args) :
        RedfishMessage(
            "Power.1.0.0.VoltageNoLongerCritical",
            "Indicates that a voltage reading no longer exceeds an internal critical level, but exceeds an internal warning level.",
            "This message shall be used to indicate that a voltage reading no longer violates an internal critical level, but still violates an internal warning level.  This message shall be used for voltage properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The voltage properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Voltage '%1' reading of %2 volts no longer exceeds the critcal level.",
            "Warning",
            2,
            { "string", "number" },
            { "The name or identifier of the voltage reading.", "The voltage in volts." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the voltage reading.", "This argument shall contain the voltage in volts." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
VoltageNormal :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    VoltageNormal(Args... args) :
        RedfishMessage(
            "Power.1.0.0.VoltageNormal",
            "Indicates that a voltage reading is now within normal operating range.",
            "This message shall be used to indicate that a voltage reading has returned to its normal operating range.",
            "Voltage '%1' reading of %2 volts is within normal operating range.",
            "OK",
            2,
            { "string", "number" },
            { "The name or identifier of the voltage reading.", "The voltage in volts." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the voltage reading.", "This argument shall contain the voltage in volts." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
CurrentAboveUpperCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    CurrentAboveUpperCautionThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.CurrentAboveUpperCautionThreshold",
            "Indicates that a current reading is above the upper caution threshold.",
            "This message shall be used to indicate that a current reading violates an upper caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCaution` or `UpperCautionUser` properties.",
            "Current '%1' reading of %2 amperes is above the %3 upper caution threshold.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the current reading.", "The current in amperes.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the current reading.", "This argument shall contain the current in amperes.", "This argument shall contain the value of the `Reading` property within the `UpperCaution` or `UpperCautionUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
CurrentAboveUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    CurrentAboveUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.CurrentAboveUpperCriticalThreshold",
            "Indicates that a current reading is above the upper critical threshold.",
            "This message shall be used to indicate that a current reading violates an upper critical threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties.",
            "Current '%1' reading of %2 amperes is above the %3 upper critical threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the current reading.", "The current in amperes.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the current reading.", "This argument shall contain the current in amperes.", "This argument shall contain the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
CurrentAboveUpperFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    CurrentAboveUpperFatalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.CurrentAboveUpperFatalThreshold",
            "Indicates that a current reading is above the upper fatal threshold.",
            "This message shall be used to indicate that a current reading violates an upper fatal threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperFatal` property.",
            "Current '%1' reading of %2 amperes is above the %3 upper fatal threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the current reading.", "The current in amperes.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the current reading.", "This argument shall contain the current in amperes.", "This argument shall contain the value of the `Reading` property within the `UpperFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
CurrentBelowLowerCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    CurrentBelowLowerCautionThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.CurrentBelowLowerCautionThreshold",
            "Indicates that a current reading is below the lower caution threshold.",
            "This message shall be used to indicate that a current reading violates a lower caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCaution` or `LowerCautionUser` properties.",
            "Current '%1' reading of %2 amperes is below the %3 lower caution threshold.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the current reading.", "The current in amperes.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the current reading.", "This argument shall contain the current in amperes.", "This argument shall contain the value of the `Reading` property within the `LowerCaution` or `LowerCautionUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
CurrentBelowLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    CurrentBelowLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.CurrentBelowLowerCriticalThreshold",
            "Indicates that a current reading is below the lower critcal threshold.",
            "This message shall be used to indicate that a current reading violates a lower critical threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties.",
            "Current '%1' reading of %2 amperes is below the %3 lower critical threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the current reading.", "The current in amperes.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the current reading.", "This argument shall contain the current in amperes.", "This argument shall contain the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
CurrentBelowLowerFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    CurrentBelowLowerFatalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.CurrentBelowLowerFatalThreshold",
            "Indicates that a current reading is below the lower fatal threshold.",
            "This message shall be used to indicate that a current reading violates a lower fatal threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerFatal` property.",
            "Current '%1' reading of %2 amperes is below the %3 lower fatal threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the current reading.", "The current in amperes.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the current reading.", "This argument shall contain the current in amperes.", "This argument shall contain the value of the `Reading` property within the `LowerFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
CurrentBelowUpperFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    CurrentBelowUpperFatalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.CurrentBelowUpperFatalThreshold",
            "Indicates that a current reading is no longer above the upper fatal threshold, but is still outside of normal operating range.",
            "This message shall be used to indicate that a current reading, previously above the fatal threshold, no longer violates the upper fatal threshold.  However, the current violates at least one upper threshold and is not within the normal operating range.",
            "Current '%1' reading of %2 amperes is now below the %3 upper fatal threshold but remains outside of normal range.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the current reading.", "The current in amperes.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the current reading.", "This argument shall contain the current in amperes.", "This argument shall contain the value of the `Reading` property within the `UpperFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
CurrentBelowUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    CurrentBelowUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.CurrentBelowUpperCriticalThreshold",
            "Indicates that a current reading is no longer above the upper critical threshold, but is still outside of normal operating range.",
            "This message shall be used to indicate that a current reading, previously above the critical threshold, no longer violates the upper critical threshold.  However, the current violates at least one upper threshold and is not within the normal operating range.",
            "Current '%1' reading of %2 amperes is now below the %3 upper critical threshold but remains outside of normal range.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the current reading.", "The current in amperes.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the current reading.", "This argument shall contain the current in amperes.", "This argument shall contain the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
CurrentAboveLowerFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    CurrentAboveLowerFatalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.CurrentAboveLowerFatalThreshold",
            "Indicates that a current reading is no longer below the lower fatal threshold, but is still outside of normal operating range.",
            "This message shall be used to indicate that a current reading, previously below the fatal threshold, no longer violates the lower fatal threshold.  However, the current violates at least one lower threshold and is not within the normal operating range.",
            "Current '%1' reading of %2 amperes is now above the %3 lower fatal threshold but remains outside of normal range.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the current reading.", "The current in amperes.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the current reading.", "This argument shall contain the current in amperes.", "This argument shall contain the value of the `Reading` property within the `LowerFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
CurrentAboveLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    CurrentAboveLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.CurrentAboveLowerCriticalThreshold",
            "Indicates that a current reading is no longer below the lower critical threshold, but is still outside of normal operating range.",
            "This message shall be used to indicate that a current reading, previously below the lower critical threshold, no longer violates the lower critical threshold.  However, the current violates at least one lower threshold and is not within the normal operating range.",
            "Current '%1' reading of %2 amperes is now above the %3 lower critical threshold but remains outside of normal range.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the current reading.", "The current in amperes.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the current reading.", "This argument shall contain the current in amperes.", "This argument shall contain the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
CurrentWarning :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    CurrentWarning(Args... args) :
        RedfishMessage(
            "Power.1.0.0.CurrentWarning",
            "Indicates that a current reading exceeds an internal warning level.",
            "This message shall be used to indicate that a current reading violates an internal warning level.  This message shall be used for current-related properties or `Sensor` resources that implement boolean trip points for a caution limit, such as from hardware mechanisms with no threshold value.  The current-related properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Current '%1' reading of %2 amperes exceeds the warning level.",
            "Warning",
            2,
            { "string", "number" },
            { "The name or identifier of the current reading.", "The current in amperes." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the current reading.", "This argument shall contain the current in amperes." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
CurrentCritical :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    CurrentCritical(Args... args) :
        RedfishMessage(
            "Power.1.0.0.CurrentCritical",
            "Indicates that a current reading exceeds an internal critical level.",
            "This message shall be used to indicate that a current reading violates an internal critical level.  This message shall be used for current-related properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The current-related properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Current '%1' reading of %2 amperes exceeds the critical level.",
            "Critical",
            2,
            { "string", "number" },
            { "The name or identifier of the current reading.", "The current in amperes." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the current reading.", "This argument shall contain the current in amperes." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
CurrentNoLongerCritical :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    CurrentNoLongerCritical(Args... args) :
        RedfishMessage(
            "Power.1.0.0.CurrentNoLongerCritical",
            "Indicates that a current reading no longer exceeds an internal critical level, but exceeds an internal warning level.",
            "This message shall be used to indicate that a current reading no longer violates an internal critical level, but still exceeds an internal warning level.  This message shall be used for current-related properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The current-related properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Current '%1' reading of %2 amperes no longer exceeds the critcal level.",
            "Warning",
            2,
            { "string", "number" },
            { "The name or identifier of the current reading.", "The current in amperes." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the current reading.", "This argument shall contain the current in amperes." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
CurrentNormal :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    CurrentNormal(Args... args) :
        RedfishMessage(
            "Power.1.0.0.CurrentNormal",
            "Indicates that a current reading is now within normal operating range.",
            "This message shall be used to indicate that a current reading has returned to its normal operating range.",
            "Current '%1' reading of %2 amperes is within normal operating range.",
            "OK",
            2,
            { "string", "number" },
            { "The name or identifier of the current reading.", "The current in ampere units." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the current reading.", "This argument shall contain the current in ampere units." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
PowerAboveUpperCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PowerAboveUpperCautionThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.PowerAboveUpperCautionThreshold",
            "Indicates that a power reading is above the upper caution threshold.",
            "This message shall be used to indicate that a power reading violates an upper caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCaution` or `UpperCautionUser` properties.",
            "Power '%1' reading of %2 watts is above the %3 upper caution threshold.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the power reading.", "The power in watts.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the power reading.", "This argument shall contain the power in watts.", "This argument shall contain the value of the `Reading` property within the `UpperCaution` or `UpperCautionUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
PowerAboveUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PowerAboveUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.PowerAboveUpperCriticalThreshold",
            "Indicates that a power reading is above the upper critical threshold.",
            "This message shall be used to indicate that a power reading violates an upper critical threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties.",
            "Power '%1' reading of %2 watts is above the %3 upper critical threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the power reading.", "The power in watts.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the power reading.", "This argument shall contain the power in watts.", "This argument shall contain the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
PowerAboveUpperFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PowerAboveUpperFatalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.PowerAboveUpperFatalThreshold",
            "Indicates that a power reading is above the upper fatal threshold.",
            "This message shall be used to indicate that a power reading violates an upper fatal threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperFatal` property.",
            "Power '%1' reading of %2 watts is above the %3 upper fatal threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the power reading.", "The power in watts.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the power reading.", "This argument shall contain the power in watts.", "This argument shall contain the value of the `Reading` property within the `UpperFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
PowerBelowLowerCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PowerBelowLowerCautionThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.PowerBelowLowerCautionThreshold",
            "Indicates that a power reading is below the lower caution threshold.",
            "This message shall be used to indicate that a power reading violates a lower caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCaution` or `LowerCautionUser` properties.",
            "Power '%1' reading of %2 watts is below the %3 lower caution threshold.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the power reading.", "The power in watts.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the power reading.", "This argument shall contain the power in watts.", "This argument shall contain the value of the `Reading` property within the `LowerCaution` or `LowerCautionUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
PowerBelowLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PowerBelowLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.PowerBelowLowerCriticalThreshold",
            "Indicates that a power reading is below the lower critcal threshold.",
            "This message shall be used to indicate that a power reading violates a lower critical threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties.",
            "Power '%1' reading of %2 watts is below the %3 lower critical threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the power reading.", "The power in watts.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the power reading.", "This argument shall contain the power in watts.", "This argument shall contain the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
PowerBelowLowerFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PowerBelowLowerFatalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.PowerBelowLowerFatalThreshold",
            "Indicates that a power reading is below the lower fatal threshold.",
            "This message shall be used to indicate that a power reading violates a lower fatal threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerFatal` property.",
            "Power '%1' reading of %2 watts is below the %3 lower fatal threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the power reading.", "The power in watts.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the power reading.", "This argument shall contain the power in watts.", "This argument shall contain the value of the `Reading` property within the `LowerFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
PowerBelowUpperFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PowerBelowUpperFatalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.PowerBelowUpperFatalThreshold",
            "Indicates that a power reading is no longer above the upper fatal threshold, but is still outside of normal operating range.",
            "This message shall be used to indicate that a power reading, previously above the fatal threshold, no longer violates the upper fatal threshold.  However, the power violates at least one upper threshold and is not within the normal operating range.",
            "Power '%1' reading of %2 watts is now below the %3 upper fatal threshold but remains outside of normal range.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the power reading.", "The power in watts.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the power reading.", "This argument shall contain the power in watts.", "This argument shall contain the value of the `Reading` property within the `UpperFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
PowerBelowUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PowerBelowUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.PowerBelowUpperCriticalThreshold",
            "Indicates that a power reading is no longer above the upper critical threshold, but is still outside of normal operating range.",
            "This message shall be used to indicate that a power reading, previously above the critical threshold, no longer violates the upper critical threshold.  However, the power violates at least one upper threshold and is not within the normal operating range.",
            "Power '%1' reading of %2 watts is now below the %3 upper critical threshold but remains outside of normal range.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the power reading.", "The power in watts.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the power reading.", "This argument shall contain the power in watts.", "This argument shall contain the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
PowerAboveLowerFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PowerAboveLowerFatalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.PowerAboveLowerFatalThreshold",
            "Indicates that a power reading is no longer below the lower fatal threshold, but is still outside of normal operating range.",
            "This message shall be used to indicate that a power reading, previously below the fatal threshold, no longer violates the lower fatal threshold.  However, the power violates at least one lower threshold and is not within the normal operating range.",
            "Power '%1' reading of %2 watts is now above the %3 lower fatal threshold but remains outside of normal range.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the power reading.", "The power in watts.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the power reading.", "This argument shall contain the power in watts.", "This argument shall contain the value of the `Reading` property within the `LowerFatal` property of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
PowerAboveLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PowerAboveLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.PowerAboveLowerCriticalThreshold",
            "Indicates that a power reading is no longer below the lower critical threshold, but is still outside of normal operating range.",
            "This message shall be used to indicate that a power reading, previously below the lower critical threshold, no longer violates the lower critical threshold.  However, the power violates at least one lower threshold and is not within the normal operating range.",
            "Power '%1' reading of %2 watts is now above the %3 lower critical threshold but remains outside of normal range.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the power reading.", "The power in watts.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the power reading.", "This argument shall contain the power in watts.", "This argument shall contain the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
PowerWarning :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    PowerWarning(Args... args) :
        RedfishMessage(
            "Power.1.0.0.PowerWarning",
            "Indicates that a power reading exceeds an internal warning level.",
            "This message shall be used to indicate that a power reading violates an internal warning level.  This message shall be used for power properties or `Sensor` resources that implement boolean trip points for a warning limit, such as from hardware mechanisms with no threshold value.  The power properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Power '%1' reading of %2 watts exceeds the warning level.",
            "Warning",
            2,
            { "string", "number" },
            { "The name or identifier of the power reading.", "The power in watts." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the power reading.", "This argument shall contain the power in watts." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
PowerCritical :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    PowerCritical(Args... args) :
        RedfishMessage(
            "Power.1.0.0.PowerCritical",
            "Indicates that a power reading exceeds an internal critical level.",
            "This message shall be used to indicate that a power reading violates an internal critical level.  This message shall be used for power properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The power properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Power '%1' reading of %2 watts exceeds the critical level.",
            "Critical",
            2,
            { "string", "number" },
            { "The name or identifier of the power reading.", "The power in watts." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the power reading.", "This argument shall contain the power in watts." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
PowerNoLongerCritical :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    PowerNoLongerCritical(Args... args) :
        RedfishMessage(
            "Power.1.0.0.PowerNoLongerCritical",
            "Indicates that a power reading no longer exceeds an internal critical level, but exceeds an internal warning level.",
            "This message shall be used to indicate that a power reading no longer violates an internal critical level, but still exceeds an internal warning level.  This message shall be used for power properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The power properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Power '%1' reading of %2 watts no longer exceeds the critcal level.",
            "Warning",
            2,
            { "string", "number" },
            { "The name or identifier of the power reading.", "The power in watts." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the power reading.", "This argument shall contain the power in watts." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
PowerNormal :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    PowerNormal(Args... args) :
        RedfishMessage(
            "Power.1.0.0.PowerNormal",
            "Indicates that a power reading is now within normal operating range.",
            "This message shall be used to indicate that a power reading has returned to its normal operating range.",
            "Power '%1' reading of %2 watts is within normal operating range.",
            "OK",
            2,
            { "string", "number" },
            { "The name or identifier of the power reading.", "The power in watts." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the power reading.", "This argument shall contain the power in watts." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
FrequencyAboveUpperCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FrequencyAboveUpperCautionThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.FrequencyAboveUpperCautionThreshold",
            "Indicates that a frequency reading is above the upper caution threshold.",
            "This message shall be used to indicate that a frequency reading violates an upper caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCaution` or `UpperCautionUser` properties.",
            "Frequency '%1' reading of %2 hertz is above the %3 upper caution threshold.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the frequency reading.", "The frequency in hertz units.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the frequency reading.", "This argument shall contain the frequency in hertz units.", "This argument shall contain the value of the `Reading` property within the `UpperCaution` or `UpperCautionUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
FrequencyAboveUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FrequencyAboveUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.FrequencyAboveUpperCriticalThreshold",
            "Indicates that a frequency reading is above the upper critical threshold.",
            "This message shall be used to indicate that a frequency reading violates an upper caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties.",
            "Frequency '%1' reading of %2 hertz is above the %3 upper critical threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the frequency reading.", "The frequency in hertz units.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the frequency reading.", "This argument shall contain the frequency in hertz units.", "This argument shall contain the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
FrequencyBelowLowerCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FrequencyBelowLowerCautionThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.FrequencyBelowLowerCautionThreshold",
            "Indicates that a frequency reading is below the lower caution threshold.",
            "This message shall be used to indicate that a frequency reading violates an lower caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCaution` or `LowerCautionUser` properties.",
            "Frequency '%1' reading of %2 hertz is below the %3 lower caution threshold.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the frequency reading.", "The frequency in hertz units.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the frequency reading.", "This argument shall contain the frequency in hertz units.", "This argument shall contain the value of the `Reading` property within the `LowerCaution` or `LowerCautionUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
FrequencyBelowLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FrequencyBelowLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.FrequencyBelowLowerCriticalThreshold",
            "Indicates that a frequency reading is below the lower critical threshold.",
            "This message shall be used to indicate that a frequency reading violates a lower caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties.",
            "Frequency '%1' reading of %2 hertz is below the %3 lower critical threshold.",
            "Critical",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the frequency reading.", "The frequency in hertz.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the frequency reading.", "This argument shall contain the frequency in hertz units.", "This argument shall contain the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
FrequencyBelowUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FrequencyBelowUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.FrequencyBelowUpperCriticalThreshold",
            "Indicates that a frequency reading is no longer above the upper critical threshold, but is still outside of normal operating range.",
            "This message shall be used to indicate that a frequency reading, previously above the critical threshold, no longer violates the upper critical threshold.  However, the frequency violates at least one upper threshold and is not within the normal operating range.",
            "Frequency '%1' reading of %2 hertz is now below the %3 upper critical threshold but remains outside of normal range.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the frequency reading.", "The frequency in hertz.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the frequency reading.", "This argument shall contain the frequency in hertz units.", "This argument shall contain the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
FrequencyAboveLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FrequencyAboveLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.0.FrequencyAboveLowerCriticalThreshold",
            "Indicates that a frequency reading is no longer below the lower critical threshold, but is still outside of normal operating range.",
            "This message shall be used to indicate that a frequency reading, previously below the lower critical threshold, no longer violates the lower critical threshold.  However, the frequency violates at least one lower threshold and is not within the normal operating range.",
            "Frequency '%1' reading of %2 hertz is now above the %3 lower critical threshold but remains outside of normal range.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The name or identifier of the frequency reading.", "The frequency in hertz.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the frequency reading.", "This argument shall contain the frequency in hertz.", "This argument shall contain the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
FrequencyWarning :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    FrequencyWarning(Args... args) :
        RedfishMessage(
            "Power.1.0.0.FrequencyWarning",
            "Indicates that a frequency reading exceeds an internal warning level.",
            "This message shall be used to indicate that a frequency reading violates an internal warning level.  This message shall be used for frequency properties or `Sensor` resources that implement boolean trip points for a warning limit, such as from hardware mechanisms with no threshold value.  The frequency properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Frequency '%1' reading of %2 hertz exceeds the warning level.",
            "Warning",
            2,
            { "string", "number" },
            { "The name or identifier of the frequency reading.", "The frequency in hertz." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the frequency reading.", "This argument shall contain the frequency in hertz." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
FrequencyCritical :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    FrequencyCritical(Args... args) :
        RedfishMessage(
            "Power.1.0.0.FrequencyCritical",
            "Indicates that a frequency reading exceeds an internal critical level.",
            "This message shall be used to indicate that a frequency reading violates an internal critical level.  This message shall be used for frequency properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The frequency properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Frequency '%1' reading of %2 hertz exceeds the critical level.",
            "Critical",
            2,
            { "string", "number" },
            { "The name or identifier of the frequency reading.", "The frequency in hertz." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the frequency reading.", "This argument shall contain the frequency in hertz." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
FrequencyNoLongerCritical :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    FrequencyNoLongerCritical(Args... args) :
        RedfishMessage(
            "Power.1.0.0.FrequencyNoLongerCritical",
            "Indicates that a frequency reading no longer exceeds an internal critical level, but exceeds an internal warning level.",
            "This message shall be used to indicate that a frequency reading no longer violates an internal critical level, but still exceeds an internal warning level.  This message shall be used for frequency properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The frequency properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Frequency '%1' reading of %2 hertz no longer exceeds the critcal level.",
            "Warning",
            2,
            { "string", "number" },
            { "The name or identifier of the frequency reading.", "The frequency in hertz." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the frequency reading.", "This argument shall contain the frequency in hertz." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
FrequencyNormal :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    FrequencyNormal(Args... args) :
        RedfishMessage(
            "Power.1.0.0.FrequencyNormal",
            "Indicates that a frequency reading is now within normal operating range.",
            "This message shall be used to indicate that a frequency reading has returned to its normal operating range.",
            "Frequency '%1' reading of %2 hertz is within normal operating range.",
            "OK",
            2,
            { "string", "number" },
            { "The name or identifier of the frequency reading.", "The hertz frequency." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the frequency reading.", "This argument shall contain the frequency in hertz units." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
CircuitPoweredOn :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    CircuitPoweredOn(Args... args) :
        RedfishMessage(
            "Power.1.0.0.CircuitPoweredOn",
            "Indicates that a circuit was powered on.",
            "This message shall be used to indicate that the value of the `PowerState` property of a `Circuit` resource changed to `On`.",
            "Circuit '%1' powered on.",
            "OK",
            1,
            { "string" },
            { "The name or identifier of the circuit." },
            { "This argument shall contain a string that identifies or describes the `Circuit` resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
CircuitPoweredOff :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    CircuitPoweredOff(Args... args) :
        RedfishMessage(
            "Power.1.0.0.CircuitPoweredOff",
            "Indicates that a circuit was powered off.",
            "This message shall be used to indicate that the value of the `PowerState` property of a `Circuit` resource changed to `Off`.",
            "Circuit '%1' powered off.",
            "OK",
            1,
            { "string" },
            { "The name or identifier of the circuit." },
            { "This argument shall contain a string that identifies or describes the `Circuit` resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
OutletPoweredOn :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    OutletPoweredOn(Args... args) :
        RedfishMessage(
            "Power.1.0.0.OutletPoweredOn",
            "Indicates that an outlet was powered on.",
            "This message shall be used to indicate that the value of the `PowerState` property of an `Outlet` resource changed to `On`.",
            "Outlet '%1' powered on.",
            "OK",
            1,
            { "string" },
            { "The name or identifier of the outlet." },
            { "This argument shall contain a string that identifies or describes the `Outlet` resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
OutletPoweredOff :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    OutletPoweredOff(Args... args) :
        RedfishMessage(
            "Power.1.0.0.OutletPoweredOff",
            "Indicates that an outlet was powered off.",
            "This message shall be used to indicate that the value of the `PowerState` property of a `Outlet` resource changed to `Off`.",
            "Outlet '%1' powered off.",
            "OK",
            1,
            { "string" },
            { "The name or identifier of the outlet." },
            { "This argument shall contain a string that identifies or describes the `Outlet` resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
BreakerTripped :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    BreakerTripped(Args... args) :
        RedfishMessage(
            "Power.1.0.0.BreakerTripped",
            "Indicates that a circuit breaker tripped.",
            "This message shall be used to indicate that a circuit breaker tripped due to an over-current condition or an electrical fault.",
            "Breaker '%1' has tripped.",
            "Critical",
            1,
            { "string" },
            { "The name or identifier of the circuit." },
            { "This argument shall contain a string that identifies or describes the `Circuit` resource or the location of the electrical fault." },
            "Check the circuit and connected devices, and disconnect or replace any faulty devices.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
BreakerReset :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    BreakerReset(Args... args) :
        RedfishMessage(
            "Power.1.0.0.BreakerReset",
            "Indicates that a circuit breaker reset.",
            "This message shall be used to indicate that a circuit breaker reset.  The power state may depend on any policy applicable to the circuit or resource.",
            "Breaker '%1' reset.",
            "OK",
            1,
            { "string" },
            { "The name or identifier of the circuit." },
            { "This argument shall contain a string that identifies or describes the `Circuit` resource or the location of the circuit breaker." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
BreakerFault :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    BreakerFault(Args... args) :
        RedfishMessage(
            "Power.1.0.0.BreakerFault",
            "Indicates that a circuit breaker has an internal fault.",
            "This message shall be used to indicate that a fault was detected within a circuit breaker.  The power state of the circuit may not be known.",
            "Fault detected in breaker '%1'.",
            "Critical",
            1,
            { "string" },
            { "The name or identifier of the circuit." },
            { "This argument shall contain a string that identifies or describes the `Circuit` resource or the location of the circuit breaker." },
            "Check the breaker hardware and replace any faulty components.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
PowerSupplyFailed :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PowerSupplyFailed(Args... args) :
        RedfishMessage(
            "Power.1.0.0.PowerSupplyFailed",
            "Indicates that a power supply has failed.",
            "This message shall be used to indicate that a power supply has failed.",
            "Power supply '%1' has failed.",
            "Warning",
            1,
            { "string" },
            { "The name or identifier of the power supply." },
            { "This argument shall contain a string that identifies or describes the `PowerSupply` resource." },
            "Check the power supply hardware and replace any faulty component.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
PowerSupplyPredictiveFailure :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PowerSupplyPredictiveFailure(Args... args) :
        RedfishMessage(
            "Power.1.0.0.PowerSupplyPredictiveFailure",
            "Indicates that the power supply predicted a future failure condition.",
            "This message shall be used to indicate a power supply predicted a future failure condition, but the power supply remains functional.",
            "Power supply '%1' has a predicted failure condition.",
            "Warning",
            1,
            { "string" },
            { "The name or identifier of the power supply." },
            { "This argument shall contain a string that identifies or describes the `PowerSupply` resource." },
            "Check the power supply hardware and replace any faulty component.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
PowerSupplyWarning :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PowerSupplyWarning(Args... args) :
        RedfishMessage(
            "Power.1.0.0.PowerSupplyWarning",
            "Indicates that a power supply has a warning condition.",
            "This message shall be used to indicate that a power supply has a warning condition, but continues to supply output power.",
            "Power supply '%1' has a warning condition.",
            "Warning",
            1,
            { "string" },
            { "The name or identifier of the power supply." },
            { "This argument shall contain a string that identifies or describes the `PowerSupply` resource." },
            "Check the power supply hardware and replace any faulty component.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
PowerSupplyRestored :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PowerSupplyRestored(Args... args) :
        RedfishMessage(
            "Power.1.0.0.PowerSupplyRestored",
            "Indicates that a power supply was repaired or restored to normal operation.",
            "This message shall be used to indicate that a power supply was repaired, replaced, or otherwise returned to normal operation.",
            "Power supply '%1' was restored.",
            "OK",
            1,
            { "string" },
            { "The name or identifier of the power supply." },
            { "This argument shall contain a string that identifies or describes the `PowerSupply` resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
LossOfInputPower :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    LossOfInputPower(Args... args) :
        RedfishMessage(
            "Power.1.0.0.LossOfInputPower",
            "Indicates a loss of power at an electrical input.",
            "This message shall be used to indicate that a loss of power was detected at an electrical input.",
            "Loss of input power at '%1'.",
            "Warning",
            1,
            { "string" },
            { "The name or identifier of the electrical input or equipment." },
            { "This argument shall contain a string that identifies or describes a `PowerSupply`, `Circuit`, `PowerDistribution`, or `Chassis` resource." },
            "Check the electrical power input and connections.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
LineInputPowerFault :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    LineInputPowerFault(Args... args) :
        RedfishMessage(
            "Power.1.0.0.LineInputPowerFault",
            "Indicates a fault on an electrical power input.",
            "This message shall be used to indicate that a power fault was detected at an electrical input.  The equipment may not be able to produce output power.",
            "Line input power fault at '%1'.",
            "Warning",
            1,
            { "string" },
            { "The name or identifier of the electrical input or equipment." },
            { "This argument shall contain a string that identifies or describes a `PowerSupply`, `Circuit`, `PowerDistribution`, or `Chassis` resource." },
            "Check the electrical power input and connections.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
LineInputPowerRestored :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    LineInputPowerRestored(Args... args) :
        RedfishMessage(
            "Power.1.0.0.LineInputPowerRestored",
            "Indicates that an electrical power input was restored to normal operation.",
            "This message shall be used to indicate that an electrical power input was repaired, reconnected, or otherwise returned to normal operation.",
            "Line input power at '%1' was restored.",
            "OK",
            1,
            { "string" },
            { "The name or identifier of the electrical input or equipment." },
            { "This argument shall contain a string that identifies or describes a `PowerSupply`, `Circuit`, `PowerDistribution`, or `Chassis` resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
PowerSupplyRemoved :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PowerSupplyRemoved(Args... args) :
        RedfishMessage(
            "Power.1.0.0.PowerSupplyRemoved",
            "Indicates that a power supply was removed.",
            "This message shall be used to indicate that a power supply was removed.",
            "Power supply '%1' was removed.",
            "OK",
            1,
            { "string" },
            { "The name or identifier of the power supply." },
            { "This argument shall contain a string that identifies or describes the `PowerSupply` resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
PowerSupplyInserted :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PowerSupplyInserted(Args... args) :
        RedfishMessage(
            "Power.1.0.0.PowerSupplyInserted",
            "Indicates that a power supply was inserted or installed.",
            "This message shall be used to indicate that a power supply was inserted or installed",
            "Power supply '%1' was inserted.",
            "OK",
            1,
            { "string" },
            { "The name or identifier of the power supply." },
            { "This argument shall contain a string that identifies or describes the `PowerSupply` resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
PowerSupplyGroupCritical :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PowerSupplyGroupCritical(Args... args) :
        RedfishMessage(
            "Power.1.0.0.PowerSupplyGroupCritical",
            "Indicates that a power supply group has a critical status.",
            "This message shall be used to indicate that a power supply group is operating in a critical state.",
            "Power supply group '%1' is in a critical state.",
            "Critical",
            1,
            { "string" },
            { "The name or identifier of the power supply group." },
            { "This argument shall contain a string that identifies or describes the group of `PowerSupply` resources." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
PowerSupplyGroupWarning :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PowerSupplyGroupWarning(Args... args) :
        RedfishMessage(
            "Power.1.0.0.PowerSupplyGroupWarning",
            "Indicates that a power supply group has a warning status.",
            "This message shall be used to indicate that a power supply group is operating with a warning status or in a non-redundant state due to a fault or a change in configuration.",
            "Power supply group '%1' is in a warning state.",
            "Warning",
            1,
            { "string" },
            { "The name or identifier of the power supply group." },
            { "This argument shall contain a string that identifies or describes the group of `PowerSupply` resources." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Power_1_0_1")]]
PowerSupplyGroupNormal :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PowerSupplyGroupNormal(Args... args) :
        RedfishMessage(
            "Power.1.0.0.PowerSupplyGroupNormal",
            "Indicates that a power supply group has returned to normal operations.",
            "This message shall be used to indicate that a power supply group has returned to a normal operating state.",
            "Power supply group '%1' is operating normally.",
            "OK",
            1,
            { "string" },
            { "The name or identifier of the power supply group." },
            { "This argument shall contain a string that identifies or describes the group of `PowerSupply` resources." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace Power_1_0_0
}; // namespace registries
}; // namespace redfish
