#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace ResourceEvent_1_3_0
{

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
ResourceCreated :
    public RedfishMessage<>
{
    template <typename... Args>
    ResourceCreated(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.ResourceCreated",
            "Indicates that all conditions of a successful creation operation have been met.",
            "This message shall be used to indicates that all conditions of a successful creation operation have been met.",
            "The resource has been created successfully.",
            "OK",
            0,
            {},
            {},
            {},
            "None",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
ResourceRemoved :
    public RedfishMessage<>
{
    template <typename... Args>
    ResourceRemoved(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.ResourceRemoved",
            "Indicates that all conditions of a successful remove operation have been met.",
            "This message shall be used to indicates that all conditions of a successful remove operation have been met.",
            "The resource has been removed successfully.",
            "OK",
            0,
            {},
            {},
            {},
            "None",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
ResourceErrorsDetected :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ResourceErrorsDetected(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.ResourceErrorsDetected",
            "Indicates that a specified resource property has detected errors.  Examples would be drive I/O errors, or network link errors.",
            "This message shall be used to indicate that a specified resource property has detected errors.",
            "The resource property %1 has detected errors of type '%2'.",
            "Warning",
            2,
            { "string", "string" },
            { "The name of the property.", "The type of error encountered." },
            { "This argument shall contain the name of the relevant property from a Redfish resource.", "This argument shall contain a description of the type of error encountered." },
            "Resolution dependent upon error type.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
ResourceErrorsCorrected :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ResourceErrorsCorrected(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.ResourceErrorsCorrected",
            "Indicates that a specified resource property has corrected errors.  Examples would be drive I/O errors, or network link errors.",
            "This message shall be used to indicate that a specified resource property has corrected errors.",
            "The resource property %1 has corrected errors of type '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The name of the property.", "The type of error encountered." },
            { "This argument shall contain the name of the relevant property from a Redfish resource.", "This argument shall contain a description of the type of error encountered." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
ResourceErrorThresholdExceeded :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    ResourceErrorThresholdExceeded(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.ResourceErrorThresholdExceeded",
            "Indicates that a specified resource property has exceeded its error threshold.  Examples would be drive I/O errors, or network link errors.",
            "This message shall be used to indicate that a specified resource property has exceeded its error threshold.",
            "The resource property %1 has exceeded error threshold of value %2.",
            "Critical",
            2,
            { "string", "number" },
            { "The name of the property.", "The value of the threshold." },
            { "This argument shall contain the name of the relevant property from a Redfish resource.", "This argument shall contain the value of the relevant error threshold." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
ResourceErrorThresholdCleared :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    ResourceErrorThresholdCleared(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.ResourceErrorThresholdCleared",
            "Indicates that a specified resource property has cleared its error threshold.  Examples would be drive I/O errors, or network link errors.",
            "This message shall be used to indicate that a specified resource property has cleared its error threshold.",
            "The resource property %1 has cleared the error threshold of value %2.",
            "OK",
            2,
            { "string", "number" },
            { "The name of the property.", "The value of the threshold." },
            { "This argument shall contain the name of the relevant property from a Redfish resource.", "This argument shall contain the value of the relevant error threshold." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
ResourceWarningThresholdExceeded :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    ResourceWarningThresholdExceeded(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.ResourceWarningThresholdExceeded",
            "Indicates that a specified resource property has exceeded its warning threshold.  Examples would be drive I/O errors, or network link errors.  Suggested resolution may be provided as OEM data.",
            "This message shall be used to indicate that a specified resource property has exceeded its warning threshold.",
            "The resource property %1 has exceeded its warning threshold of value %2.",
            "Warning",
            2,
            { "string", "number" },
            { "The name of the property.", "The value of the threshold." },
            { "This argument shall contain the name of the relevant property from a Redfish resource.", "This argument shall contain the value of the relevant error threshold." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
ResourceWarningThresholdCleared :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    ResourceWarningThresholdCleared(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.ResourceWarningThresholdCleared",
            "Indicates that a specified resource property has cleared its warning threshold.  Examples would be drive I/O errors, or network link errors.  Suggested resolution may be provided as OEM data.",
            "This message shall be used to indicate that a specified resource property has cleared its warning threshold.",
            "The resource property %1 has cleared the warning threshold of value %2.",
            "OK",
            2,
            { "string", "number" },
            { "The name of the property.", "The value of the threshold." },
            { "This argument shall contain the name of the relevant property from a Redfish resource.", "This argument shall contain the value of the relevant error threshold." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
ResourceStatusChangedOK :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ResourceStatusChangedOK(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.ResourceStatusChangedOK",
            "Indicates that the health of a resource has changed to OK.",
            "This message shall be used to indicate that the health of a resource has changed to OK.",
            "The health of resource '%1' has changed to %2.",
            "OK",
            2,
            { "string", "string" },
            { "The relevant resource.", "The state of the resource." },
            { "This argument shall contain the name of the relevant Redfish resource.", "This argument shall contain the value of the `Health` property for the relevant Redfish resource.  The values shall be used from the definition of the `Health` enumeration in the `Resource` schema." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
ResourceStatusChangedWarning :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ResourceStatusChangedWarning(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.ResourceStatusChangedWarning",
            "Indicates that the health of a resource has changed to Warning.",
            "This message shall be used to indicate that the health of a resource has changed to Warning.",
            "The health of resource `%1` has changed to %2.",
            "Warning",
            2,
            { "string", "string" },
            { "The relevant resource.", "The state of the resource." },
            { "This argument shall contain the name of the relevant Redfish resource.", "This argument shall contain the value of the `Health` property for the relevant Redfish resource.  The values shall be used from the definition of the `Health` enumeration in the `Resource` schema." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
ResourceStatusChangedCritical :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ResourceStatusChangedCritical(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.ResourceStatusChangedCritical",
            "Indicates that the health of a resource has changed to Critical.",
            "This message shall be used to indicate that the health of a resource has changed to Critical.",
            "The health of resource `%1` has changed to %2.",
            "Critical",
            2,
            { "string", "string" },
            { "The relevant resource.", "The state of the resource." },
            { "This argument shall contain the name of the relevant Redfish resource.", "This argument shall contain the value of the `Health` property for the relevant Redfish resource.  The values shall be used from the definition of the `Health` enumeration in the `Resource` schema." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
ResourceStateChanged :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ResourceStateChanged(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.ResourceStateChanged",
            "Indicates that the state of a resource has changed.",
            "This message shall be used to indicate that the value of the State property of a resource has changed.",
            "The state of resource `%1` has changed to %2.",
            "OK",
            2,
            { "string", "string" },
            { "The relevant resource.", "The state of the resource." },
            { "This argument shall contain the name of the relevant Redfish resource.", "This argument shall contain the value of the `State` property for the relevant Redfish resource.  The values shall be used from the definition of the `State` enumeration in the `Resource` schema." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
ResourcePoweredOn :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourcePoweredOn(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.ResourcePoweredOn",
            "Indicates that the power state of a resource has changed to powered on.",
            "This message shall be used to indicate that the value of the PowerState property of a resource has changed to `On`.",
            "The resource `%1` has powered on.",
            "OK",
            1,
            { "string" },
            { "The relevant resource." },
            { "This argument shall contain the name of the relevant Redfish resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
ResourcePoweringOn :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourcePoweringOn(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.ResourcePoweringOn",
            "Indicates that the power state of a resource has changed to powering on.",
            "This message shall be used to indicate that the value of the PowerState property of a resource has changed to `PoweringOn`.  This message should not be used if the PowerState transition to `On` will occur immediately.",
            "The resource `%1` is powering on.",
            "OK",
            1,
            { "string" },
            { "The relevant resource." },
            { "This argument shall contain the name of the relevant Redfish resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
ResourcePoweredOff :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourcePoweredOff(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.ResourcePoweredOff",
            "Indicates that the power state of a resource has changed to powered off.",
            "This message shall be used to indicate that the value of the PowerState property of a resource has changed to `Off`.",
            "The resource `%1` has powered off.",
            "OK",
            1,
            { "string" },
            { "The relevant resource." },
            { "This argument shall contain the name of the relevant Redfish resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
ResourcePoweringOff :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourcePoweringOff(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.ResourcePoweringOff",
            "Indicates that the power state of a resource has changed to powering off.",
            "This message shall be used to indicate that the value of the PowerState property of a resource has changed to `PoweringOff`.  This message should not be used if the PowerState transition to `Off` will occur immediately.",
            "The resource `%1` is powering off.",
            "OK",
            1,
            { "string" },
            { "The relevant resource." },
            { "This argument shall contain the name of the relevant Redfish resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
ResourcePaused :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourcePaused(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.ResourcePaused",
            "Indicates that the power state of a resource has changed to paused.",
            "This message shall be used to indicate that the value of the PowerState property of a resource has changed to `Paused`.",
            "The resource `%1` has been paused.",
            "OK",
            1,
            { "string" },
            { "The relevant resource." },
            { "This argument shall contain the name of the relevant Redfish resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
URIForResourceChanged :
    public RedfishMessage<>
{
    template <typename... Args>
    URIForResourceChanged(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.URIForResourceChanged",
            "Indicates that the URI for a resource has changed.  Examples for this would be physical component replacement or redistribution.",
            "This message shall be used to indicate that the URI for a resource has changed.",
            "The URI for the resource has changed.",
            "OK",
            0,
            {},
            {},
            {},
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
ResourceChanged :
    public RedfishMessage<>
{
    template <typename... Args>
    ResourceChanged(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.ResourceChanged",
            "Indicates that one or more resource properties have changed.  This is not used whenever there is another event message for that specific change, such as only the state has changed.",
            "This message shall be used to indicate that one or more resource properties have changed.",
            "One or more resource properties have changed.",
            "OK",
            0,
            {},
            {},
            {},
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
ResourceVersionIncompatible :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourceVersionIncompatible(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.ResourceVersionIncompatible",
            "Indicates that an incompatible version of software has been detected.  Examples may be after a component or system level software update.",
            "This message shall be used to indicate that an incompatible version of software has been detected.",
            "An incompatible version of software '%1' has been detected.",
            "Warning",
            1,
            { "string" },
            { "The name of the software component." },
            { "This argument shall contain the name of the relevant software component or package.  This might include both name and version information." },
            "Compare the version of the resource with the compatible version of the software.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
ResourceSelfTestFailed :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourceSelfTestFailed(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.ResourceSelfTestFailed",
            "Indicates that a self-test has failed.  Suggested resolution may be provided as OEM data.",
            "This message shall be used to indicate that a self-test has failed.",
            "A self-test has failed.  The following message was returned: '%1'.",
            "Critical",
            1,
            { "string" },
            { "The self-test error message." },
            { "This argument shall contain the error message received as a result from the self-test." },
            "See vendor specific instructions for specific actions.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
ResourceSelfTestCompleted :
    public RedfishMessage<>
{
    template <typename... Args>
    ResourceSelfTestCompleted(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.ResourceSelfTestCompleted",
            "Indicates that a self-test has completed.",
            "This message shall be used to indicate that a self-test has completed.",
            "A self-test has completed.",
            "OK",
            0,
            {},
            {},
            {},
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
TestMessage :
    public RedfishMessage<>
{
    template <typename... Args>
    TestMessage(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.TestMessage",
            "A test message used to validate event delivery mechanisms.",
            "This message shall be used to indicate a test of a event delivery mechanism, such as the creation of a LogEntry resource or an event transmission.",
            "Test message.",
            "OK",
            0,
            {},
            {},
            {},
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
AggregationSourceDiscovered :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    AggregationSourceDiscovered(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.AggregationSourceDiscovered",
            "Indicates that a new aggregation source has been discovered.",
            "This message shall be used to indicate an aggregation source has been discovered in the network or host.  The `OriginOfCondition` property shall contain a link to the matching `ConnectionMethod` resource if known.",
            "A aggregation source of connection method `%1` located at `%2` has been discovered.",
            "OK",
            2,
            { "string", "string" },
            { "The connection method of the discovered aggregation source.", "The location of the discovered aggregation source, such as an IP address or service entry point." },
            { "This argument shall contain a value from the `ConnectionMethodType` enumeration defined in the `ConnectionMethod` schema that describes connection method of the discovered aggregation source.", "This argument shall contain the location of the discovered aggregation source, such as an IP address or service entry point." },
            "The aggregation source is available to the service and can be identified using the identified connection method.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
LicenseExpired :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    LicenseExpired(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.LicenseExpired",
            "Indicates that a license has expired.",
            "This message shall be used to indicate that a license has expired.",
            "A license for '%1' has expired.  The following message was returned: '%2'.",
            "Warning",
            2,
            { "string", "string" },
            { "The relevant resource.", "The message returned from the license validation process." },
            { "This argument shall contain the name of the relevant resource or service affected by the software license.", "This argument shall contain the message returned from the license validation process or software." },
            "See vendor specific instructions for specific actions.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
LicenseChanged :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    LicenseChanged(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.LicenseChanged",
            "Indicates that a license has changed.",
            "This message shall be used to indicate that a license has changed.",
            "A license for '%1' has changed.  The following message was returned: '%2'.",
            "Warning",
            2,
            { "string", "string" },
            { "The relevant resource.", "The message returned from the license validation process." },
            { "This argument shall contain the name of the relevant resource or service affected by the software license.", "This argument shall contain the message returned from the license validation process or software." },
            "See vendor specific instructions for specific actions.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
LicenseAdded :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    LicenseAdded(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.3.0.LicenseAdded",
            "Indicates that a license has been added.",
            "This message shall be used to indicate that a license has been added.",
            "A license for '%1' has been added.  The following message was returned: '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The relevant resource.", "The message returned from the license validation process." },
            { "This argument shall contain the name of the relevant resource or service affected by the software license.", "This argument shall contain the message returned from the license validation process or software." },
            "See vendor specific instructions for specific actions.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace ResourceEvent_1_3_0
}; // namespace registries
}; // namespace redfish
