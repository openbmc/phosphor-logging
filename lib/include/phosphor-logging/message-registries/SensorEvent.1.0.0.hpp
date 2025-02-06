#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace SensorEvent_1_0_0
{

struct [[deprecated ("Use SensorEvent_1_0_1")]]
ReadingAboveUpperCautionThreshold :
    public RedfishMessage<std::string, double, std::string, double>
{
    template <typename... Args>
    ReadingAboveUpperCautionThreshold(Args... args) :
        RedfishMessage(
            "SensorEvent.1.0.0.ReadingAboveUpperCautionThreshold",
            "Indicates that a sensor reading is above the upper caution threshold.",
            "This message shall be used to indicate that the value of the `Reading` property in a `Sensor` resource is above the value of the `Reading` property within the `UpperCaution` or `UpperCautionUser` properties.",
            "Sensor '%1' reading of %2 (%3) is above the %4 upper caution threshold.",
            "Warning",
            4,
            { "string", "number", "string", "number" },
            { "The name or identifier of the sensor.", "The reading of the sensor.", "The reading units of measure.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the `Sensor` resource.", "This argument shall contain a number that equals the value of the `Reading` property of the `Sensor` resource.", "This argument shall contain a string that equals the value of the `ReadingUnits` property of the `Sensor` resource.  Unitless readings should use `count`.", "This argument shall contain a number that equals the value of the `Reading` property within the `UpperCaution` or `UpperCautionUser` properties of the `Sensor` resource." },
            "Check the condition of the resources listed in RelatedItem.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use SensorEvent_1_0_1")]]
ReadingAboveUpperCriticalThreshold :
    public RedfishMessage<std::string, double, std::string, double>
{
    template <typename... Args>
    ReadingAboveUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "SensorEvent.1.0.0.ReadingAboveUpperCriticalThreshold",
            "Indicates that a sensor reading is above the upper critical threshold.",
            "This message shall be used to indicate that the value of the `Reading` property in a `Sensor` resource is above the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties.",
            "Sensor '%1' reading of %2 (%3) is above the %4 upper critical threshold.",
            "Critical",
            4,
            { "string", "number", "string", "number" },
            { "The name or identifier of the sensor.", "The reading of the sensor.", "The reading units of measure.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the `Sensor` resource.", "This argument shall contain a number that equals the value of the `Reading` property of the `Sensor` resource.", "This argument shall contain a string that equals the value of the `ReadingUnits` property of the `Sensor` resource.  Unitless readings should use `count`.", "This argument shall contain a number that equals the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resources listed in RelatedItem.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use SensorEvent_1_0_1")]]
ReadingAboveUpperFatalThreshold :
    public RedfishMessage<std::string, double, std::string, double>
{
    template <typename... Args>
    ReadingAboveUpperFatalThreshold(Args... args) :
        RedfishMessage(
            "SensorEvent.1.0.0.ReadingAboveUpperFatalThreshold",
            "Indicates that a sensor reading is above the upper fatal threshold.",
            "This message shall be used to indicate that the value of the `Reading` property in a `Sensor` resource is above the value of the `Reading` property within the `UpperFatal` property.",
            "Sensor '%1' reading of %2 (%3) is above the %4 upper fatal threshold.",
            "Critical",
            4,
            { "string", "number", "string", "number" },
            { "The name or identifier of the sensor.", "The reading of the sensor.", "The reading units of measure.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the `Sensor` resource.", "This argument shall contain a number that equals the value of the `Reading` property of the `Sensor` resource.", "This argument shall contain a string that equals the value of the `ReadingUnits` property of the `Sensor` resource.  Unitless readings should use `count`.", "This argument shall contain a number that equals the value of the `Reading` property within the `UpperFatal` property of the `Sensor` resource." },
            "Check the condition of the resources listed in RelatedItem.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use SensorEvent_1_0_1")]]
ReadingBelowLowerCautionThreshold :
    public RedfishMessage<std::string, double, std::string, double>
{
    template <typename... Args>
    ReadingBelowLowerCautionThreshold(Args... args) :
        RedfishMessage(
            "SensorEvent.1.0.0.ReadingBelowLowerCautionThreshold",
            "Indicates that a sensor reading is below the lower caution threshold.",
            "This message shall be used to indicate that the value of the `Reading` property in a `Sensor` resource is below the value of the `Reading` property within the `LowerCaution` or `LowerCautionUser` properties.",
            "Sensor '%1' reading of %2 (%3) is below the %4 lower caution threshold.",
            "Warning",
            4,
            { "string", "number", "string", "number" },
            { "The name or identifier of the sensor.", "The reading of the sensor.", "The reading units of measure.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the `Sensor` resource.", "This argument shall contain a number that equals the value of the `Reading` property of the `Sensor` resource.", "This argument shall contain a string that equals the value of the `ReadingUnits` property of the `Sensor` resource.  Unitless readings should use `count`.", "This argument shall contain a number that equals the value of the `Reading` property within the `LowerCaution` or `LowerCautionUser` properties of the `Sensor` resource." },
            "Check the condition of the resources listed in RelatedItem.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use SensorEvent_1_0_1")]]
ReadingBelowLowerCriticalThreshold :
    public RedfishMessage<std::string, double, std::string, double>
{
    template <typename... Args>
    ReadingBelowLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "SensorEvent.1.0.0.ReadingBelowLowerCriticalThreshold",
            "Indicates that a sensor reading is below the lower critical threshold.",
            "This message shall be used to indicate that the value of the `Reading` property in a `Sensor` resource is below the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties.",
            "Sensor '%1' reading of %2 (%3) is below the %4 lower critical threshold.",
            "Critical",
            4,
            { "string", "number", "string", "number" },
            { "The name or identifier of the sensor.", "The reading of the sensor.", "The reading units of measure.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the `Sensor` resource.", "This argument shall contain a number that equals the value of the `Reading` property of the `Sensor` resource.", "This argument shall contain a string that equals the value of the `ReadingUnits` property of the `Sensor` resource.  Unitless readings should use `count`.", "This argument shall contain a number that equals the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resources listed in RelatedItem.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use SensorEvent_1_0_1")]]
ReadingBelowLowerFatalThreshold :
    public RedfishMessage<std::string, double, std::string, double>
{
    template <typename... Args>
    ReadingBelowLowerFatalThreshold(Args... args) :
        RedfishMessage(
            "SensorEvent.1.0.0.ReadingBelowLowerFatalThreshold",
            "Indicates that a sensor reading is below the lower fatal threshold.",
            "This message shall be used to indicate that the value of the `Reading` property in a `Sensor` resource is below the value of the `Reading` property within the `LowerFatal` property.",
            "Sensor '%1' reading of %2 (%3) is below the %4 lower fatal threshold.",
            "Critical",
            4,
            { "string", "number", "string", "number" },
            { "The name or identifier of the sensor.", "The reading of the sensor.", "The reading units of measure.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the `Sensor` resource.", "This argument shall contain a number that equals the value of the `Reading` property of the `Sensor` resource.", "This argument shall contain a string that equals the value of the `ReadingUnits` property of the `Sensor` resource.  Unitless readings should use `count`.", "This argument shall contain a number that equals the value of the `Reading` property within the `LowerFatal` property of the `Sensor` resource." },
            "Check the condition of the resources listed in RelatedItem.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use SensorEvent_1_0_1")]]
ReadingBelowUpperFatalThreshold :
    public RedfishMessage<std::string, double, std::string, double>
{
    template <typename... Args>
    ReadingBelowUpperFatalThreshold(Args... args) :
        RedfishMessage(
            "SensorEvent.1.0.0.ReadingBelowUpperFatalThreshold",
            "Indicates that a sensor reading is no longer above the upper fatal threshold, but is still outside of normal operating range.",
            "This message shall be used to indicate that the value of the `Reading` in a `Sensor` resource, previously above the threshold, is now below the value of the `Reading` property within the `UpperFatal` property.  However, the value of the `Reading` property is still above at least one `Reading` property of another upper threshold and is not within the normal operating range.",
            "Sensor '%1' reading of %2 (%3) is now below the %4 upper fatal threshold but remains outside of normal range.",
            "Critical",
            4,
            { "string", "number", "string", "number" },
            { "The name or identifier of the sensor.", "The reading of the sensor.", "The reading units of measure.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the `Sensor` resource.", "This argument shall contain a number that equals the value of the `Reading` property of the `Sensor` resource.", "This argument shall contain a string that equals the value of the `ReadingUnits` property of the `Sensor` resource.  Unitless readings should use `count`.", "This argument shall contain a number that equals the value of the `Reading` property within the `UpperFatal` property of the `Sensor` resource." },
            "Check the condition of the resources listed in RelatedItem.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use SensorEvent_1_0_1")]]
ReadingBelowUpperCriticalThreshold :
    public RedfishMessage<std::string, double, std::string, double>
{
    template <typename... Args>
    ReadingBelowUpperCriticalThreshold(Args... args) :
        RedfishMessage(
            "SensorEvent.1.0.0.ReadingBelowUpperCriticalThreshold",
            "Indicates that a sensor reading is no longer above the upper critical threshold, but is still outside of normal operating range.",
            "This message shall be used to indicate that the value of the `Reading` in a `Sensor` resource, previously above the threshold, is now below the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties.  However, the value of the `Reading` property is still above at least one `Reading` property of another upper threshold and is not within the normal operating range.",
            "Sensor '%1' reading of %2 (%3) is now below the %4 upper critical threshold but remains outside of normal range.",
            "Warning",
            4,
            { "string", "number", "string", "number" },
            { "The name or identifier of the sensor.", "The reading of the sensor.", "The reading units of measure.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the `Sensor` resource.", "This argument shall contain a number that equals the value of the `Reading` property of the `Sensor` resource.", "This argument shall contain a string that equals the value of the `ReadingUnits` property of the `Sensor` resource.  Unitless readings should use `count`.", "This argument shall contain a number that equals the value of the `Reading` property within the `UpperCritical` or `UpperCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resources listed in RelatedItem.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use SensorEvent_1_0_1")]]
ReadingAboveLowerFatalThreshold :
    public RedfishMessage<std::string, double, std::string, double>
{
    template <typename... Args>
    ReadingAboveLowerFatalThreshold(Args... args) :
        RedfishMessage(
            "SensorEvent.1.0.0.ReadingAboveLowerFatalThreshold",
            "Indicates that a sensor reading is no longer below the lower fatal threshold, but is still outside of normal operating range.",
            "This message shall be used to indicate that the value of the `Reading` in a `Sensor` resource, previously below the threshold, is now above the value of the `Reading` property within the `LowerFatal` property.  However, the value of the `Reading` property is still below at least one `Reading` property of another lower threshold and is not within the normal operating range.",
            "Sensor '%1' reading of %2 (%3) is now above the %4 lower fatal threshold but remains outside of normal range.",
            "Critical",
            4,
            { "string", "number", "string", "number" },
            { "The name or identifier of the sensor.", "The reading of the sensor.", "The reading units of measure.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the `Sensor` resource.", "This argument shall contain a number that equals the value of the `Reading` property of the `Sensor` resource.", "This argument shall contain a string that equals the value of the `ReadingUnits` property of the `Sensor` resource.  Unitless readings should use `count`.", "This argument shall contain a number that equals the value of the `Reading` property within the `LowerFatal` property of the `Sensor` resource." },
            "Check the condition of the resources listed in RelatedItem.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use SensorEvent_1_0_1")]]
ReadingAboveLowerCriticalThreshold :
    public RedfishMessage<std::string, double, std::string, double>
{
    template <typename... Args>
    ReadingAboveLowerCriticalThreshold(Args... args) :
        RedfishMessage(
            "SensorEvent.1.0.0.ReadingAboveLowerCriticalThreshold",
            "Indicates that a sensor reading is no longer below the lower critical threshold, but is still outside of normal operating range.",
            "This message shall be used to indicate that the value of the `Reading` in a `Sensor` resource, previously below the threshold, is now above the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties.  However, the value of the `Reading` property is still below at least one `Reading` property of another lower threshold and is not within the normal operating range.",
            "Sensor '%1' reading of %2 (%3) is now above the %4 lower critical threshold but remains outside of normal range.",
            "Warning",
            4,
            { "string", "number", "string", "number" },
            { "The name or identifier of the sensor.", "The reading of the sensor.", "The reading units of measure.", "The threshold value." },
            { "This argument shall contain a string that identifies or describes the `Sensor` resource.", "This argument shall contain a number that equals the value of the `Reading` property of the `Sensor` resource.", "This argument shall contain a string that equals the value of the `ReadingUnits` property of the `Sensor` resource.  Unitless readings should use `count`.", "This argument shall contain a number that equals the value of the `Reading` property within the `LowerCritical` or `LowerCriticalUser` properties of the `Sensor` resource." },
            "Check the condition of the resources listed in RelatedItem.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use SensorEvent_1_0_1")]]
ReadingWarning :
    public RedfishMessage<std::string, double, std::string>
{
    template <typename... Args>
    ReadingWarning(Args... args) :
        RedfishMessage(
            "SensorEvent.1.0.0.ReadingWarning",
            "Indicates that a sensor reading exceeds an internal warning level.",
            "This message shall be used to indicate that the value of the `Reading` property in a `Sensor` resource exceeds the value of an internal warning level.  This message shall be used for `Sensor` resources that do not contain readable caution threshold values, such as boolean trip points or similiar hardware mechanisms.  This message shall not be used if the `Reading` property within a corresponding caution threshold is available.",
            "Sensor '%1' reading of %2 (%3) exceeds the warning level.",
            "Warning",
            3,
            { "string", "number", "string" },
            { "The name or identifier of the sensor.", "The reading of the sensor.", "The reading units of measure." },
            { "This argument shall contain a string that identifies or describes the `Sensor` resource.", "This argument shall contain a number that equals the value of the `Reading` property of the `Sensor` resource.", "This argument shall contain a string that equals the value of the `ReadingUnits` property of the `Sensor` resource.  Unitless readings should use `count`." },
            "Check the condition of the resources listed in RelatedItem.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use SensorEvent_1_0_1")]]
ReadingCritical :
    public RedfishMessage<std::string, double, std::string>
{
    template <typename... Args>
    ReadingCritical(Args... args) :
        RedfishMessage(
            "SensorEvent.1.0.0.ReadingCritical",
            "Indicates that a sensor reading exceeds an internal critical level.",
            "This message shall be used to indicate that the value of the `Reading` property in a `Sensor` resource exceeds the value of an internal critical level.  This message shall be used for `Sensor` resources that do not contain readable critical threshold values, such as boolean trip points or similiar hardware mechanisms.  This message shall not be used if the `Reading` property within a corresponding critical threshold is available.",
            "Sensor '%1' reading of %2 (%3) exceeds the critical level.",
            "Critical",
            3,
            { "string", "number", "string" },
            { "The name or identifier of the sensor.", "The reading of the sensor.", "The reading units of measure." },
            { "This argument shall contain a string that identifies or describes the `Sensor` resource.", "This argument shall contain a number that equals the value of the `Reading` property of the `Sensor` resource.", "This argument shall contain a string that equals the value of the `ReadingUnits` property of the `Sensor` resource.  Unitless readings should use `count`." },
            "Check the condition of the resources listed in RelatedItem.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use SensorEvent_1_0_1")]]
ReadingNoLongerCritical :
    public RedfishMessage<std::string, double, std::string>
{
    template <typename... Args>
    ReadingNoLongerCritical(Args... args) :
        RedfishMessage(
            "SensorEvent.1.0.0.ReadingNoLongerCritical",
            "Indicates that a sensor reading no longer exceeds an internal critical level, but exceeds an internal warning level.",
            "This message shall be used to indicate that the value of the `Reading` property in a `Sensor` resource no longer exceeds the value of an internal critical level, but the value still exceeds an internal warning level.  This message shall be used for `Sensor` resources that do not contain readable critical threshold values, such as boolean trip points or similiar hardware mechanisms.  This message shall not be used if the `Reading` property within a corresponding critical threshold is available.",
            "Sensor '%1' reading of %2 (%3) no longer exceeds the critcal level.",
            "Warning",
            3,
            { "string", "number", "string" },
            { "The name or identifier of the sensor.", "The reading of the sensor.", "The reading units of measure." },
            { "This argument shall contain a string that identifies or describes the `Sensor` resource.", "This argument shall contain a number that equals the value of the `Reading` property of the `Sensor` resource.", "This argument shall contain a string that equals the value of the `ReadingUnits` property of the `Sensor` resource.  Unitless readings should use `count`." },
            "Check the condition of the resources listed in RelatedItem.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use SensorEvent_1_0_1")]]
SensorReadingNormalRange :
    public RedfishMessage<std::string, double, std::string>
{
    template <typename... Args>
    SensorReadingNormalRange(Args... args) :
        RedfishMessage(
            "SensorEvent.1.0.0.SensorReadingNormalRange",
            "Indicates that a sensor reading is now within normal operating range.",
            "This message shall be used to indicate that the value of the `Reading` property within a `Sensor` resource has returned to its normal operating range.",
            "Sensor '%1' reading of %2 (%3) is within normal operating range.",
            "OK",
            3,
            { "string", "number", "string" },
            { "The name or identifier of the sensor.", "The reading of the sensor.", "The reading units of measure." },
            { "This argument shall contain a string that identifies or describes the `Sensor` resource.", "This argument shall contain a number that equals the value of the `Reading` property of the `Sensor` resource.", "This argument shall contain a string that equals the value of the `ReadingUnits` property of the `Sensor` resource.  Unitless readings should use `count`." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use SensorEvent_1_0_1")]]
InvalidSensorReading :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    InvalidSensorReading(Args... args) :
        RedfishMessage(
            "SensorEvent.1.0.0.InvalidSensorReading",
            "Indicates that the service received an invalid reading from a sensor.",
            "This message shall be used to indicate that the service received invalid data while attempting to update the value of the `Reading` property in a `Sensor` resource.",
            "Invalid reading received from sensor '%1'.",
            "Warning",
            1,
            { "string" },
            { "The name or identifier of the sensor." },
            { "This argument shall contain a string that identifies or describes the `Sensor` resource." },
            "Check the sensor hardware or connection.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use SensorEvent_1_0_1")]]
SensorFailure :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    SensorFailure(Args... args) :
        RedfishMessage(
            "SensorEvent.1.0.0.SensorFailure",
            "Indicates that the service cannot communicate with a sensor or has detected a failure.",
            "This message shall be used to indicate that the service is unable to communicate with a sensor or has otherwise detected a fault condition with the sensor.",
            "Sensor '%1' has failed.",
            "Warning",
            1,
            { "string" },
            { "The name or identifier of the sensor." },
            { "This argument shall contain a string that identifies or describes the `Sensor` resource." },
            "Check the sensor hardware or connection.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use SensorEvent_1_0_1")]]
SensorRestored :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    SensorRestored(Args... args) :
        RedfishMessage(
            "SensorEvent.1.0.0.SensorRestored",
            "Indicates that a sensor has been repaired or communications have been restored.  It may also indicate that the service is receiving valid data from a sensor.",
            "This message shall be used to indicate that the service is able to communicate with a sensor that had previously failed, or the sensor has been repaired or replaced.",
            "Sensor '%1' has been restored.",
            "OK",
            1,
            { "string" },
            { "The name or identifier of the sensor." },
            { "This argument shall contain a string that identifies or describes the `Sensor` resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace SensorEvent_1_0_0
}; // namespace registries
}; // namespace redfish
