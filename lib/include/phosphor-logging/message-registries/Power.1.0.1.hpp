#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace Power_1_0_1
{

struct VoltageAboveUpperCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    VoltageAboveUpperCautionThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.VoltageAboveUpperCautionThreshold",
            "Indicates that a voltage reading is above the upper caution threshold.",
            "This message shall indicate that a voltage reading violates an upper caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCaution` or `UpperCautionUser` properties.",
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

struct VoltageAboveUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    VoltageAboveUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.VoltageAboveUpperCriticalThreshold",
            "Indicates that a voltage reading is above the upper critical threshold.",
            "This message shall indicate that a voltage reading violates an upper critical threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties.",
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

struct VoltageAboveUpperFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    VoltageAboveUpperFatalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.VoltageAboveUpperFatalThreshold",
            "Indicates that a voltage reading is above the upper fatal threshold.",
            "This message shall indicate that a voltage reading violates an upper fatal threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperFatal` property.",
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

struct VoltageBelowLowerCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    VoltageBelowLowerCautionThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.VoltageBelowLowerCautionThreshold",
            "Indicates that a voltage reading is below the lower caution threshold.",
            "This message shall indicate that a voltage reading violates a lower caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCaution` or `LowerCautionUser` properties.",
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

struct VoltageBelowLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    VoltageBelowLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.VoltageBelowLowerCriticalThreshold",
            "Indicates that a voltage reading is below the lower critical threshold.",
            "This message shall indicate that a voltage reading violates a lower critical threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties.",
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

struct VoltageBelowLowerFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    VoltageBelowLowerFatalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.VoltageBelowLowerFatalThreshold",
            "Indicates that a voltage reading is below the lower fatal threshold.",
            "This message shall indicate that a voltage reading violates a lower fatal threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerFatal` property.",
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

struct VoltageBelowUpperFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    VoltageBelowUpperFatalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.VoltageBelowUpperFatalThreshold",
            "Indicates that a voltage reading is no longer above the upper fatal threshold but is still outside of normal operating range.",
            "This message shall indicate that a voltage reading, previously above the fatal threshold, no longer violates the upper fatal threshold.  However, the voltage violates at least one upper threshold and is not within the normal operating range.",
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

struct VoltageBelowUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    VoltageBelowUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.VoltageBelowUpperCriticalThreshold",
            "Indicates that a voltage reading is no longer above the upper critical threshold but is still outside of normal operating range.",
            "This message shall indicate that a voltage reading, previously above the critical threshold, no longer violates the upper critical threshold.  However, the voltage violates at least one upper threshold and is not within the normal operating range.",
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

struct VoltageAboveLowerFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    VoltageAboveLowerFatalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.VoltageAboveLowerFatalThreshold",
            "Indicates that a voltage reading is no longer below the lower fatal threshold but is still outside of normal operating range.",
            "This message shall indicate that a voltage reading, previously below the fatal threshold, no longer violates the lower fatal threshold.  However, the voltage violates at least one lower threshold and is not within the normal operating range.",
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

struct VoltageAboveLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    VoltageAboveLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.VoltageAboveLowerCriticalThreshold",
            "Indicates that a voltage reading is no longer below the lower critical threshold but is still outside of normal operating range.",
            "This message shall indicate that a voltage reading, previously below the lower critical threshold, no longer violates the lower critical threshold.  However, the voltage violates at least one lower threshold and is not within the normal operating range.",
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

struct VoltageWarning :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    VoltageWarning(Args... args) :
        RedfishMessage(
            "Power.1.0.1.VoltageWarning",
            "Indicates that a voltage reading exceeds an internal warning level.",
            "This message shall indicate that a voltage reading violates an internal warning level.  This message shall be used for voltage properties or `Sensor` resources that implement boolean trip points for a caution limit, such as from hardware mechanisms with no threshold value.  The voltage properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
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

struct VoltageCritical :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    VoltageCritical(Args... args) :
        RedfishMessage(
            "Power.1.0.1.VoltageCritical",
            "Indicates that a voltage reading exceeds an internal critical level.",
            "This message shall indicate that a voltage reading violates an internal critical level.  This message shall be used for voltage properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The voltage properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
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

struct VoltageNoLongerCritical :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    VoltageNoLongerCritical(Args... args) :
        RedfishMessage(
            "Power.1.0.1.VoltageNoLongerCritical",
            "Indicates that a voltage reading no longer exceeds an internal critical level but still exceeds an internal warning level.",
            "This message shall indicate that a voltage reading no longer violates an internal critical level but still violates an internal warning level.  This message shall be used for voltage properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The voltage properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Voltage '%1' reading of %2 volts no longer exceeds the critical level.",
            "Warning",
            2,
            { "string", "number" },
            { "The name or identifier of the voltage reading.", "The voltage in volts." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the voltage reading.", "This argument shall contain the voltage in volts." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct VoltageNormal :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    VoltageNormal(Args... args) :
        RedfishMessage(
            "Power.1.0.1.VoltageNormal",
            "Indicates that a voltage reading is now within normal operating range.",
            "This message shall indicate that a voltage reading has returned to its normal operating range.",
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

struct CurrentAboveUpperCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    CurrentAboveUpperCautionThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.CurrentAboveUpperCautionThreshold",
            "Indicates that a current reading is above the upper caution threshold.",
            "This message shall indicate that a current reading violates an upper caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCaution` or `UpperCautionUser` properties.",
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

struct CurrentAboveUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    CurrentAboveUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.CurrentAboveUpperCriticalThreshold",
            "Indicates that a current reading is above the upper critical threshold.",
            "This message shall indicate that a current reading violates an upper critical threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties.",
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

struct CurrentAboveUpperFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    CurrentAboveUpperFatalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.CurrentAboveUpperFatalThreshold",
            "Indicates that a current reading is above the upper fatal threshold.",
            "This message shall indicate that a current reading violates an upper fatal threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperFatal` property.",
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

struct CurrentBelowLowerCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    CurrentBelowLowerCautionThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.CurrentBelowLowerCautionThreshold",
            "Indicates that a current reading is below the lower caution threshold.",
            "This message shall indicate that a current reading violates a lower caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCaution` or `LowerCautionUser` properties.",
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

struct CurrentBelowLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    CurrentBelowLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.CurrentBelowLowerCriticalThreshold",
            "Indicates that a current reading is below the lower critical threshold.",
            "This message shall indicate that a current reading violates a lower critical threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties.",
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

struct CurrentBelowLowerFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    CurrentBelowLowerFatalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.CurrentBelowLowerFatalThreshold",
            "Indicates that a current reading is below the lower fatal threshold.",
            "This message shall indicate that a current reading violates a lower fatal threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerFatal` property.",
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

struct CurrentBelowUpperFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    CurrentBelowUpperFatalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.CurrentBelowUpperFatalThreshold",
            "Indicates that a current reading is no longer above the upper fatal threshold but is still outside of normal operating range.",
            "This message shall indicate that a current reading, previously above the fatal threshold, no longer violates the upper fatal threshold.  However, the current violates at least one upper threshold and is not within the normal operating range.",
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

struct CurrentBelowUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    CurrentBelowUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.CurrentBelowUpperCriticalThreshold",
            "Indicates that a current reading is no longer above the upper critical threshold but is still outside of normal operating range.",
            "This message shall indicate that a current reading, previously above the critical threshold, no longer violates the upper critical threshold.  However, the current violates at least one upper threshold and is not within the normal operating range.",
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

struct CurrentAboveLowerFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    CurrentAboveLowerFatalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.CurrentAboveLowerFatalThreshold",
            "Indicates that a current reading is no longer below the lower fatal threshold but is still outside of normal operating range.",
            "This message shall indicate that a current reading, previously below the fatal threshold, no longer violates the lower fatal threshold.  However, the current violates at least one lower threshold and is not within the normal operating range.",
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

struct CurrentAboveLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    CurrentAboveLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.CurrentAboveLowerCriticalThreshold",
            "Indicates that a current reading is no longer below the lower critical threshold but is still outside of normal operating range.",
            "This message shall indicate that a current reading, previously below the lower critical threshold, no longer violates the lower critical threshold.  However, the current violates at least one lower threshold and is not within the normal operating range.",
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

struct CurrentWarning :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    CurrentWarning(Args... args) :
        RedfishMessage(
            "Power.1.0.1.CurrentWarning",
            "Indicates that a current reading exceeds an internal warning level.",
            "This message shall indicate that a current reading violates an internal warning level.  This message shall be used for current-related properties or `Sensor` resources that implement boolean trip points for a caution limit, such as from hardware mechanisms with no threshold value.  The current-related properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
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

struct CurrentCritical :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    CurrentCritical(Args... args) :
        RedfishMessage(
            "Power.1.0.1.CurrentCritical",
            "Indicates that a current reading exceeds an internal critical level.",
            "This message shall indicate that a current reading violates an internal critical level.  This message shall be used for current-related properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The current-related properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
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

struct CurrentNoLongerCritical :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    CurrentNoLongerCritical(Args... args) :
        RedfishMessage(
            "Power.1.0.1.CurrentNoLongerCritical",
            "Indicates that a current reading no longer exceeds an internal critical level but still exceeds an internal warning level.",
            "This message shall indicate that a current reading no longer violates an internal critical level but still exceeds an internal warning level.  This message shall be used for current-related properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The current-related properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Current '%1' reading of %2 amperes no longer exceeds the critical level.",
            "Warning",
            2,
            { "string", "number" },
            { "The name or identifier of the current reading.", "The current in amperes." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the current reading.", "This argument shall contain the current in amperes." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct CurrentNormal :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    CurrentNormal(Args... args) :
        RedfishMessage(
            "Power.1.0.1.CurrentNormal",
            "Indicates that a current reading is now within normal operating range.",
            "This message shall indicate that a current reading has returned to its normal operating range.",
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

struct PowerAboveUpperCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PowerAboveUpperCautionThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.PowerAboveUpperCautionThreshold",
            "Indicates that a power reading is above the upper caution threshold.",
            "This message shall indicate that a power reading violates an upper caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCaution` or `UpperCautionUser` properties.",
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

struct PowerAboveUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PowerAboveUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.PowerAboveUpperCriticalThreshold",
            "Indicates that a power reading is above the upper critical threshold.",
            "This message shall indicate that a power reading violates an upper critical threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties.",
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

struct PowerAboveUpperFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PowerAboveUpperFatalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.PowerAboveUpperFatalThreshold",
            "Indicates that a power reading is above the upper fatal threshold.",
            "This message shall indicate that a power reading violates an upper fatal threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperFatal` property.",
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

struct PowerBelowLowerCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PowerBelowLowerCautionThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.PowerBelowLowerCautionThreshold",
            "Indicates that a power reading is below the lower caution threshold.",
            "This message shall indicate that a power reading violates a lower caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCaution` or `LowerCautionUser` properties.",
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

struct PowerBelowLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PowerBelowLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.PowerBelowLowerCriticalThreshold",
            "Indicates that a power reading is below the lower critical threshold.",
            "This message shall indicate that a power reading violates a lower critical threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties.",
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

struct PowerBelowLowerFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PowerBelowLowerFatalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.PowerBelowLowerFatalThreshold",
            "Indicates that a power reading is below the lower fatal threshold.",
            "This message shall indicate that a power reading violates a lower fatal threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerFatal` property.",
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

struct PowerBelowUpperFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PowerBelowUpperFatalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.PowerBelowUpperFatalThreshold",
            "Indicates that a power reading is no longer above the upper fatal threshold but is still outside of normal operating range.",
            "This message shall indicate that a power reading, previously above the fatal threshold, no longer violates the upper fatal threshold.  However, the power violates at least one upper threshold and is not within the normal operating range.",
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

struct PowerBelowUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PowerBelowUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.PowerBelowUpperCriticalThreshold",
            "Indicates that a power reading is no longer above the upper critical threshold but is still outside of normal operating range.",
            "This message shall indicate that a power reading, previously above the critical threshold, no longer violates the upper critical threshold.  However, the power violates at least one upper threshold and is not within the normal operating range.",
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

struct PowerAboveLowerFatalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PowerAboveLowerFatalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.PowerAboveLowerFatalThreshold",
            "Indicates that a power reading is no longer below the lower fatal threshold but is still outside of normal operating range.",
            "This message shall indicate that a power reading, previously below the fatal threshold, no longer violates the lower fatal threshold.  However, the power violates at least one lower threshold and is not within the normal operating range.",
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

struct PowerAboveLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    PowerAboveLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.PowerAboveLowerCriticalThreshold",
            "Indicates that a power reading is no longer below the lower critical threshold but is still outside of normal operating range.",
            "This message shall indicate that a power reading, previously below the lower critical threshold, no longer violates the lower critical threshold.  However, the power violates at least one lower threshold and is not within the normal operating range.",
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

struct PowerWarning :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    PowerWarning(Args... args) :
        RedfishMessage(
            "Power.1.0.1.PowerWarning",
            "Indicates that a power reading exceeds an internal warning level.",
            "This message shall indicate that a power reading violates an internal warning level.  This message shall be used for power properties or `Sensor` resources that implement boolean trip points for a warning limit, such as from hardware mechanisms with no threshold value.  The power properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
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

struct PowerCritical :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    PowerCritical(Args... args) :
        RedfishMessage(
            "Power.1.0.1.PowerCritical",
            "Indicates that a power reading exceeds an internal critical level.",
            "This message shall indicate that a power reading violates an internal critical level.  This message shall be used for power properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The power properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
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

struct PowerNoLongerCritical :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    PowerNoLongerCritical(Args... args) :
        RedfishMessage(
            "Power.1.0.1.PowerNoLongerCritical",
            "Indicates that a power reading no longer exceeds an internal critical level but still exceeds an internal warning level.",
            "This message shall indicate that a power reading no longer violates an internal critical level but still exceeds an internal warning level.  This message shall be used for power properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The power properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Power '%1' reading of %2 watts no longer exceeds the critical level.",
            "Warning",
            2,
            { "string", "number" },
            { "The name or identifier of the power reading.", "The power in watts." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the power reading.", "This argument shall contain the power in watts." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PowerNormal :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    PowerNormal(Args... args) :
        RedfishMessage(
            "Power.1.0.1.PowerNormal",
            "Indicates that a power reading is now within normal operating range.",
            "This message shall indicate that a power reading has returned to its normal operating range.",
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

struct FrequencyAboveUpperCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FrequencyAboveUpperCautionThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.FrequencyAboveUpperCautionThreshold",
            "Indicates that a frequency reading is above the upper caution threshold.",
            "This message shall indicate that a frequency reading violates an upper caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCaution` or `UpperCautionUser` properties.",
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

struct FrequencyAboveUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FrequencyAboveUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.FrequencyAboveUpperCriticalThreshold",
            "Indicates that a frequency reading is above the upper critical threshold.",
            "This message shall indicate that a frequency reading violates an upper caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is above the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties.",
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

struct FrequencyBelowLowerCautionThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FrequencyBelowLowerCautionThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.FrequencyBelowLowerCautionThreshold",
            "Indicates that a frequency reading is below the lower caution threshold.",
            "This message shall indicate that a frequency reading violates an lower caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCaution` or `LowerCautionUser` properties.",
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

struct FrequencyBelowLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FrequencyBelowLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.FrequencyBelowLowerCriticalThreshold",
            "Indicates that a frequency reading is below the lower critical threshold.",
            "This message shall indicate that a frequency reading violates a lower caution threshold.  If the reading originates from a `Sensor` resource, the message shall indicate that the value of the `Reading` property is below the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties.",
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

struct FrequencyBelowUpperCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FrequencyBelowUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.FrequencyBelowUpperCriticalThreshold",
            "Indicates that a frequency reading is no longer above the upper critical threshold but is still outside of normal operating range.",
            "This message shall indicate that a frequency reading, previously above the critical threshold, no longer violates the upper critical threshold.  However, the frequency violates at least one upper threshold and is not within the normal operating range.",
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

struct FrequencyAboveLowerCriticalThreshold :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    FrequencyAboveLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "Power.1.0.1.FrequencyAboveLowerCriticalThreshold",
            "Indicates that a frequency reading is no longer below the lower critical threshold but is still outside of normal operating range.",
            "This message shall indicate that a frequency reading, previously below the lower critical threshold, no longer violates the lower critical threshold.  However, the frequency violates at least one lower threshold and is not within the normal operating range.",
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

struct FrequencyWarning :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    FrequencyWarning(Args... args) :
        RedfishMessage(
            "Power.1.0.1.FrequencyWarning",
            "Indicates that a frequency reading exceeds an internal warning level.",
            "This message shall indicate that a frequency reading violates an internal warning level.  This message shall be used for frequency properties or `Sensor` resources that implement boolean trip points for a warning limit, such as from hardware mechanisms with no threshold value.  The frequency properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
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

struct FrequencyCritical :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    FrequencyCritical(Args... args) :
        RedfishMessage(
            "Power.1.0.1.FrequencyCritical",
            "Indicates that a frequency reading exceeds an internal critical level.",
            "This message shall indicate that a frequency reading violates an internal critical level.  This message shall be used for frequency properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The frequency properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
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

struct FrequencyNoLongerCritical :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    FrequencyNoLongerCritical(Args... args) :
        RedfishMessage(
            "Power.1.0.1.FrequencyNoLongerCritical",
            "Indicates that a frequency reading no longer exceeds an internal critical level but still exceeds an internal warning level.",
            "This message shall indicate that a frequency reading no longer violates an internal critical level but still exceeds an internal warning level.  This message shall be used for frequency properties or `Sensor` resources that implement boolean trip points for a critical limit, such as from hardware mechanisms with no threshold value.  The frequency properties or `Sensor` resources may contain readable thresholds that coexist with this unspecified trip point.",
            "Frequency '%1' reading of %2 hertz no longer exceeds the critical level.",
            "Warning",
            2,
            { "string", "number" },
            { "The name or identifier of the frequency reading.", "The frequency in hertz." },
            { "This argument shall contain a string that identifies or describes the location or physical context of the frequency reading.", "This argument shall contain the frequency in hertz." },
            "Check the condition of the resource listed in OriginOfCondition.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct FrequencyNormal :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    FrequencyNormal(Args... args) :
        RedfishMessage(
            "Power.1.0.1.FrequencyNormal",
            "Indicates that a frequency reading is now within normal operating range.",
            "This message shall indicate that a frequency reading has returned to its normal operating range.",
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

struct CircuitPoweredOn :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    CircuitPoweredOn(Args... args) :
        RedfishMessage(
            "Power.1.0.1.CircuitPoweredOn",
            "Indicates that a circuit was powered on.",
            "This message shall indicate that the value of the `PowerState` property of a `Circuit` resource changed to `On`.",
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

struct CircuitPoweredOff :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    CircuitPoweredOff(Args... args) :
        RedfishMessage(
            "Power.1.0.1.CircuitPoweredOff",
            "Indicates that a circuit was powered off.",
            "This message shall indicate that the value of the `PowerState` property of a `Circuit` resource changed to `Off`.",
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

struct OutletPoweredOn :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    OutletPoweredOn(Args... args) :
        RedfishMessage(
            "Power.1.0.1.OutletPoweredOn",
            "Indicates that an outlet was powered on.",
            "This message shall indicate that the value of the `PowerState` property of an `Outlet` resource changed to `On`.",
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

struct OutletPoweredOff :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    OutletPoweredOff(Args... args) :
        RedfishMessage(
            "Power.1.0.1.OutletPoweredOff",
            "Indicates that an outlet was powered off.",
            "This message shall indicate that the value of the `PowerState` property of an `Outlet` resource changed to `Off`.",
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

struct BreakerTripped :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    BreakerTripped(Args... args) :
        RedfishMessage(
            "Power.1.0.1.BreakerTripped",
            "Indicates that a circuit breaker tripped.",
            "This message shall indicate that a circuit breaker tripped due to an over-current condition or an electrical fault.",
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

struct BreakerReset :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    BreakerReset(Args... args) :
        RedfishMessage(
            "Power.1.0.1.BreakerReset",
            "Indicates that a circuit breaker reset.",
            "This message shall indicate that a circuit breaker reset.  The power state may depend on any policy applicable to the circuit or resource.",
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

struct BreakerFault :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    BreakerFault(Args... args) :
        RedfishMessage(
            "Power.1.0.1.BreakerFault",
            "Indicates that a circuit breaker has an internal fault.",
            "This message shall indicate that a fault was detected within a circuit breaker.  The power state of the circuit may not be known.",
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

struct PowerSupplyFailed :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PowerSupplyFailed(Args... args) :
        RedfishMessage(
            "Power.1.0.1.PowerSupplyFailed",
            "Indicates that a power supply has failed.",
            "This message shall indicate that a power supply has failed.",
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

struct PowerSupplyPredictiveFailure :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PowerSupplyPredictiveFailure(Args... args) :
        RedfishMessage(
            "Power.1.0.1.PowerSupplyPredictiveFailure",
            "Indicates that the power supply predicted a future failure condition.",
            "This message shall indicate a power supply predicted a future failure condition, but the power supply remains functional.",
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

struct PowerSupplyWarning :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PowerSupplyWarning(Args... args) :
        RedfishMessage(
            "Power.1.0.1.PowerSupplyWarning",
            "Indicates that a power supply has a warning condition.",
            "This message shall indicate that a power supply has a warning condition but continues to supply output power.",
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

struct PowerSupplyRestored :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PowerSupplyRestored(Args... args) :
        RedfishMessage(
            "Power.1.0.1.PowerSupplyRestored",
            "Indicates that a power supply was repaired or restored to normal operation.",
            "This message shall indicate that a power supply was repaired, replaced, or otherwise returned to normal operation.",
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

struct LossOfInputPower :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    LossOfInputPower(Args... args) :
        RedfishMessage(
            "Power.1.0.1.LossOfInputPower",
            "Indicates a loss of power at an electrical input.",
            "This message shall indicate that a loss of power was detected at an electrical input.",
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

struct LineInputPowerFault :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    LineInputPowerFault(Args... args) :
        RedfishMessage(
            "Power.1.0.1.LineInputPowerFault",
            "Indicates a fault on an electrical power input.",
            "This message shall indicate that a power fault was detected at an electrical input.  The equipment may not be able to produce output power.",
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

struct LineInputPowerRestored :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    LineInputPowerRestored(Args... args) :
        RedfishMessage(
            "Power.1.0.1.LineInputPowerRestored",
            "Indicates that an electrical power input was restored to normal operation.",
            "This message shall indicate that an electrical power input was repaired, reconnected, or otherwise returned to normal operation.",
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

struct PowerSupplyRemoved :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PowerSupplyRemoved(Args... args) :
        RedfishMessage(
            "Power.1.0.1.PowerSupplyRemoved",
            "Indicates that a power supply was removed.",
            "This message shall indicate that a power supply was removed.",
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

struct PowerSupplyInserted :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PowerSupplyInserted(Args... args) :
        RedfishMessage(
            "Power.1.0.1.PowerSupplyInserted",
            "Indicates that a power supply was inserted or installed.",
            "This message shall indicate that a power supply was inserted or installed.",
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

struct PowerSupplyGroupCritical :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PowerSupplyGroupCritical(Args... args) :
        RedfishMessage(
            "Power.1.0.1.PowerSupplyGroupCritical",
            "Indicates that a power supply group has a critical status.",
            "This message shall indicate that a power supply group is operating in a critical state.",
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

struct PowerSupplyGroupWarning :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PowerSupplyGroupWarning(Args... args) :
        RedfishMessage(
            "Power.1.0.1.PowerSupplyGroupWarning",
            "Indicates that a power supply group has a warning status.",
            "This message shall indicate that a power supply group is operating with a warning status or in a non-redundant state due to a fault or a change in configuration.",
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

struct PowerSupplyGroupNormal :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PowerSupplyGroupNormal(Args... args) :
        RedfishMessage(
            "Power.1.0.1.PowerSupplyGroupNormal",
            "Indicates that a power supply group has returned to normal operations.",
            "This message shall indicate that a power supply group has returned to a normal operating state.",
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

}; // namespace Power_1_0_1
}; // namespace registries
}; // namespace redfish
