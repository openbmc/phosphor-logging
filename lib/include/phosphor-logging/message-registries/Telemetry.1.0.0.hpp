#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace Telemetry_1_0_0
{

struct TriggerNumericAboveUpperWarning :
    public RedfishMessage<std::string, double, double, std::string>
{
    template <typename... Args>
    TriggerNumericAboveUpperWarning(Args... args) :
        RedfishMessage(
            "Telemetry.1.0.0.TriggerNumericAboveUpperWarning",
            "Indicates that a numeric metric reading is above the upper warning trigger threshold.",
            "This message shall indicate that a numeric value of a monitored metric is above the `Reading` property within the `UpperWarning` property in a `Triggers` resource`.",
            "Metric '%1' value of %2 is above the %3 upper warning threshold of trigger '%4'",
            "Warning",
            4,
            { "string", "number", "number", "string" },
            { "The metric name.", "The value of the metric.", "The threshold value.", "The telemetry trigger name." },
            { "This argument shall contain a string that identifies or describes the metric.", "This argument shall contain a number that equals the value of the metric reading.", "This argument shall contain a number that equals the value of the `Reading` property within the `UpperWarning` property of the `Triggers` resource.", "This argument shall contain a string that identifies or describes the `Triggers` resource." },
            "Check the condition of the metric that reported the trigger.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TriggerNumericAboveUpperCritical :
    public RedfishMessage<std::string, double, double, std::string>
{
    template <typename... Args>
    TriggerNumericAboveUpperCritical(Args... args) :
        RedfishMessage(
            "Telemetry.1.0.0.TriggerNumericAboveUpperCritical",
            "Indicates that a numeric metric reading is above the upper critical trigger threshold.",
            "This message shall indicate that a numeric value of a monitored metric is above the `Reading` property within the `UpperCritical` property in a `Triggers` resource`.",
            "Metric '%1' value of %2 is above the %3 upper critical threshold of trigger '%4'",
            "Critical",
            4,
            { "string", "number", "number", "string" },
            { "The metric name.", "The value of the metric.", "The threshold value.", "The telemetry trigger name." },
            { "This argument shall contain a string that identifies or describes the metric.", "This argument shall contain a number that equals the value of the metric reading.", "This argument shall contain a number that equals the value of the `Reading` property within the `UpperCritical` property of the `Triggers` resource.", "This argument shall contain a string that identifies or describes the `Triggers` resource." },
            "Check the condition of the metric that reported the trigger.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TriggerNumericBelowLowerWarning :
    public RedfishMessage<std::string, double, double, std::string>
{
    template <typename... Args>
    TriggerNumericBelowLowerWarning(Args... args) :
        RedfishMessage(
            "Telemetry.1.0.0.TriggerNumericBelowLowerWarning",
            "Indicates that a numeric metric reading is below the lower warning trigger threshold.",
            "This message shall indicate that a numeric value of a monitored metric is below the `Reading` property within the `LowerWarning` property in a `Triggers` resource`.",
            "Metric '%1' value of %2 is below the %3 lower warning threshold of trigger '%4'",
            "Warning",
            4,
            { "string", "number", "number", "string" },
            { "The metric name.", "The value of the metric.", "The threshold value.", "The telemetry trigger name." },
            { "This argument shall contain a string that identifies or describes the metric.", "This argument shall contain a number that equals the value of the metric reading.", "This argument shall contain a number that equals the value of the `Reading` property within the `LowerWarning` property of the `Triggers` resource.", "This argument shall contain a string that identifies or describes the `Triggers` resource." },
            "Check the condition of the metric that reported the trigger.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TriggerNumericBelowLowerCritical :
    public RedfishMessage<std::string, double, double, std::string>
{
    template <typename... Args>
    TriggerNumericBelowLowerCritical(Args... args) :
        RedfishMessage(
            "Telemetry.1.0.0.TriggerNumericBelowLowerCritical",
            "Indicates that a numeric metric reading is below the lower critical trigger threshold.",
            "This message shall indicate that a numeric value of a monitored metric is below the `Reading` property within the `LowerCritical` property in a `Triggers` resource`.",
            "Metric '%1' value of %2 is below the %3 lower critical threshold of trigger '%4'",
            "Critical",
            4,
            { "string", "number", "number", "string" },
            { "The metric name.", "The value of the metric.", "The threshold value.", "The telemetry trigger name." },
            { "This argument shall contain a string that identifies or describes the metric.", "This argument shall contain a number that equals the value of the metric reading.", "This argument shall contain a number that equals the value of the `Reading` property within the `LowerCritical` property of the `Triggers` resource.", "This argument shall contain a string that identifies or describes the `Triggers` resource." },
            "Check the condition of the metric that reported the trigger.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TriggerNumericAboveLowerCritical :
    public RedfishMessage<std::string, double, double, std::string>
{
    template <typename... Args>
    TriggerNumericAboveLowerCritical(Args... args) :
        RedfishMessage(
            "Telemetry.1.0.0.TriggerNumericAboveLowerCritical",
            "Indicates that a numeric metric reading is no longer below the lower critical trigger threshold, but is still outside of normal operating range.",
            "This message shall indicate that a numeric value of a monitored metric, previously below the threshold, is now above the `Reading` property within the `LowerCritical` property in a `Triggers` resource`.  However, the value of the metric is still below the `Reading` property within the `LowerWarning` property and is not within the normal operating range.",
            "Metric '%1' value of %2 is now above the %3 lower critical threshold of trigger '%4' but remains outside of normal range",
            "Warning",
            4,
            { "string", "number", "number", "string" },
            { "The metric name.", "The value of the metric.", "The threshold value.", "The telemetry trigger name." },
            { "This argument shall contain a string that identifies or describes the metric.", "This argument shall contain a number that equals the value of the metric reading.", "This argument shall contain a number that equals the value of the `Reading` property within the `LowerCritical` property of the `Triggers` resource.", "This argument shall contain a string that identifies or describes the `Triggers` resource." },
            "Check the condition of the metric that reported the trigger.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TriggerNumericBelowUpperCritical :
    public RedfishMessage<std::string, double, double, std::string>
{
    template <typename... Args>
    TriggerNumericBelowUpperCritical(Args... args) :
        RedfishMessage(
            "Telemetry.1.0.0.TriggerNumericBelowUpperCritical",
            "Indicates that a numeric metric reading is no longer above the upper critical trigger threshold, but is still outside of normal operating range.",
            "This message shall indicate that a numeric value of a monitored metric, previously above the threshold, is now below the `Reading` property within the `UpperCritical` property in a `Triggers` resource`.  However, the value of the metric is still above the `Reading` property within the `UpperWarning` property and is not within the normal operating range.",
            "Metric '%1' value of %2 is now below the %3 upper critical threshold of trigger '%4' but remains outside of normal range",
            "Warning",
            4,
            { "string", "number", "number", "string" },
            { "The metric name.", "The value of the metric.", "The threshold value.", "The telemetry trigger name." },
            { "This argument shall contain a string that identifies or describes the metric.", "This argument shall contain a number that equals the value of the metric reading.", "This argument shall contain a number that equals the value of the `Reading` property within the `UpperCritical` property of the `Triggers` resource.", "This argument shall contain a string that identifies or describes the `Triggers` resource." },
            "Check the condition of the metric that reported the trigger.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TriggerNumericReadingNormal :
    public RedfishMessage<std::string, double, std::string>
{
    template <typename... Args>
    TriggerNumericReadingNormal(Args... args) :
        RedfishMessage(
            "Telemetry.1.0.0.TriggerNumericReadingNormal",
            "Indicates that a numeric metric reading is now within normal operating range.",
            "This message shall indicate that a numeric value of a monitored metric in a `Triggers` resource has returned to its normal operating range.",
            "Metric '%1' value of %2 is within normal operating range of trigger '%3'",
            "OK",
            3,
            { "string", "number", "string" },
            { "The metric name.", "The value of the metric.", "The telemetry trigger name." },
            { "This argument shall contain a string that identifies or describes the metric.", "This argument shall contain a number that equals the value of the metric reading.", "This argument shall contain a string that identifies or describes the `Triggers` resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TriggerDiscreteConditionMet :
    public RedfishMessage<std::string, std::string, std::string>
{
    template <typename... Args>
    TriggerDiscreteConditionMet(Args... args) :
        RedfishMessage(
            "Telemetry.1.0.0.TriggerDiscreteConditionMet",
            "Indicates that a discrete trigger condition is met.",
            "This message shall indicate that the `Value` of the `DiscreteTriggers` property in a `Triggers` resource is equal to the monitored discrete metric value.  The severity of the event shall be the same as `Severity` property in the related `Triggers` resource.",
            "Metric '%1' has the value '%2', which meets the discrete condition of trigger '%3'",
            "OK",
            3,
            { "string", "string", "string" },
            { "The metric name.", "The discrete value of the metric.", "The telemetry trigger name." },
            { "This argument shall contain a string that identifies or describes the metric.", "This argument shall contain a number that equals the discrete value of the metric reading.", "This argument shall contain a string that identifies or describes the `Triggers` resource." },
            "Check the condition of the metric that reported the trigger.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace Telemetry_1_0_0
}; // namespace registries
}; // namespace redfish
