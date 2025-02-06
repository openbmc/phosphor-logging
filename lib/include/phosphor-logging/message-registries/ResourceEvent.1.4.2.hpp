#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace ResourceEvent_1_4_2
{

struct ResourceCreated :
    public RedfishMessage<>
{
    template <typename... Args>
    ResourceCreated(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.ResourceCreated",
            "Indicates that all conditions of a successful creation operation were met.",
            "This message shall indicate that all conditions of a successful creation operation were met.",
            "The resource was created successfully.",
            "OK",
            0,
            {},
            {},
            {},
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ResourceRemoved :
    public RedfishMessage<>
{
    template <typename... Args>
    ResourceRemoved(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.ResourceRemoved",
            "Indicates that all conditions of a successful remove operation were met.",
            "This message shall indicate that all conditions of a successful remove operation were met.",
            "The resource was removed successfully.",
            "OK",
            0,
            {},
            {},
            {},
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ResourceErrorsDetected :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ResourceErrorsDetected(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.ResourceErrorsDetected",
            "Indicates that a specified resource property has detected errors.  Examples of this are drive I/O errors or network link errors.",
            "This message shall indicate that a specified resource property has detected errors.",
            "The resource property %1 has detected errors of type '%2'.",
            "Warning",
            2,
            { "string", "string" },
            { "The name of the property.", "The type of error encountered." },
            { "This argument shall contain the name of the relevant property from a Redfish resource.", "This argument shall contain a description of the type of error encountered." },
            "Resolution is dependent upon error type.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ResourceErrorsCorrected :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ResourceErrorsCorrected(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.ResourceErrorsCorrected",
            "Indicates that a specified resource property has corrected errors.  Examples of this are drive I/O errors or network link errors.",
            "This message shall indicate that a specified resource property has corrected errors.",
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

struct ResourceErrorThresholdExceeded :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    ResourceErrorThresholdExceeded(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.ResourceErrorThresholdExceeded",
            "Indicates that a specified resource property has exceeded its error threshold.  Examples of this are drive I/O errors or network link errors.",
            "This message shall indicate that a specified resource property has exceeded its error threshold.",
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

struct ResourceErrorThresholdCleared :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    ResourceErrorThresholdCleared(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.ResourceErrorThresholdCleared",
            "Indicates that a specified resource property has cleared its error threshold.  Examples of this are drive I/O errors or network link errors.",
            "This message shall indicate that a specified resource property has cleared its error threshold.",
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

struct ResourceWarningThresholdExceeded :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    ResourceWarningThresholdExceeded(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.ResourceWarningThresholdExceeded",
            "Indicates that a specified resource property has exceeded its warning threshold.  Examples of this are drive I/O errors or network link errors.  Suggested resolution may be provided as OEM data.",
            "This message shall indicate that a specified resource property has exceeded its warning threshold.",
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

struct ResourceWarningThresholdCleared :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    ResourceWarningThresholdCleared(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.ResourceWarningThresholdCleared",
            "Indicates that a specified resource property has cleared its warning threshold.  Examples of this are drive I/O errors or network link errors.  Suggested resolution may be provided as OEM data.",
            "This message shall indicate that a specified resource property has cleared its warning threshold.",
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

struct ResourceStatusChangedOK :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ResourceStatusChangedOK(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.ResourceStatusChangedOK",
            "Indicates that the health of a resource has changed to OK.",
            "This message shall indicate that the health of a resource has changed to OK.",
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

struct ResourceStatusChangedWarning :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ResourceStatusChangedWarning(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.ResourceStatusChangedWarning",
            "Indicates that the health of a resource has changed to Warning.",
            "This message shall indicate that the health of a resource has changed to Warning.",
            "The health of resource '%1' has changed to %2.",
            "Warning",
            2,
            { "string", "string" },
            { "The relevant resource.", "The state of the resource." },
            { "This argument shall contain the name of the relevant Redfish resource.", "This argument shall contain the value of the `Health` property for the relevant Redfish resource.  The values shall be used from the definition of the `Health` enumeration in the `Resource` schema." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ResourceStatusChangedCritical :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ResourceStatusChangedCritical(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.ResourceStatusChangedCritical",
            "Indicates that the health of a resource has changed to Critical.",
            "This message shall indicate that the health of a resource has changed to Critical.",
            "The health of resource '%1' has changed to %2.",
            "Critical",
            2,
            { "string", "string" },
            { "The relevant resource.", "The state of the resource." },
            { "This argument shall contain the name of the relevant Redfish resource.", "This argument shall contain the value of the `Health` property for the relevant Redfish resource.  The values shall be used from the definition of the `Health` enumeration in the `Resource` schema." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ResourceStateChanged :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ResourceStateChanged(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.ResourceStateChanged",
            "Indicates that the state of a resource has changed.",
            "This message shall indicate that the value of the State property of a resource has changed.",
            "The state of resource '%1' has changed to %2.",
            "OK",
            2,
            { "string", "string" },
            { "The relevant resource.", "The state of the resource." },
            { "This argument shall contain the name of the relevant Redfish resource.", "This argument shall contain the value of the `State` property for the relevant Redfish resource.  The values shall be used from the definition of the `State` enumeration in the `Resource` schema." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ResourcePoweredOn :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourcePoweredOn(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.ResourcePoweredOn",
            "Indicates that the power state of a resource has changed to powered on.",
            "This message shall indicate that the value of the PowerState property of a resource has changed to `On`.",
            "The resource '%1' has powered on.",
            "OK",
            1,
            { "string" },
            { "The relevant resource." },
            { "This argument shall contain the name of the relevant Redfish resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ResourcePoweringOn :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourcePoweringOn(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.ResourcePoweringOn",
            "Indicates that the power state of a resource has changed to powering on.",
            "This message shall indicate that the value of the PowerState property of a resource has changed to `PoweringOn`.  This message should not be used if the PowerState transition to `On` will occur immediately.",
            "The resource '%1' is powering on.",
            "OK",
            1,
            { "string" },
            { "The relevant resource." },
            { "This argument shall contain the name of the relevant Redfish resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ResourcePoweredOff :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourcePoweredOff(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.ResourcePoweredOff",
            "Indicates that the power state of a resource has changed to powered off.",
            "This message shall indicate that the value of the PowerState property of a resource has changed to `Off`.",
            "The resource '%1' has powered off.",
            "OK",
            1,
            { "string" },
            { "The relevant resource." },
            { "This argument shall contain the name of the relevant Redfish resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ResourcePoweringOff :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourcePoweringOff(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.ResourcePoweringOff",
            "Indicates that the power state of a resource has changed to powering off.",
            "This message shall indicate that the value of the PowerState property of a resource has changed to `PoweringOff`.  This message should not be used if the PowerState transition to `Off` will occur immediately.",
            "The resource '%1' is powering off.",
            "OK",
            1,
            { "string" },
            { "The relevant resource." },
            { "This argument shall contain the name of the relevant Redfish resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ResourcePaused :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourcePaused(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.ResourcePaused",
            "Indicates that the power state of a resource has changed to paused.",
            "This message shall indicate that the value of the PowerState property of a resource has changed to `Paused`.",
            "The resource '%1' was paused.",
            "OK",
            1,
            { "string" },
            { "The relevant resource." },
            { "This argument shall contain the name of the relevant Redfish resource." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct URIForResourceChanged :
    public RedfishMessage<>
{
    template <typename... Args>
    URIForResourceChanged(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.URIForResourceChanged",
            "Indicates that the URI for a resource has changed.  Examples of this are physical component replacement or redistribution.",
            "This message shall indicate that the URI for a resource has changed.",
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

struct ResourceChanged :
    public RedfishMessage<>
{
    template <typename... Args>
    ResourceChanged(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.ResourceChanged",
            "Indicates that one or more resource properties have changed.  This is not used whenever there is another event message for that specific change, such as when only the state has changed.",
            "This message shall indicate that one or more resource properties have changed.",
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

struct PropertyValueModifiedByClient :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    PropertyValueModifiedByClient(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.PropertyValueModifiedByClient",
            "Indicates that the value of a property was modified by a client.",
            "This message shall indicate that the value of a property was modified by a client.  For example, DHCPEnabled property was assigned the value `true` by a client.",
            "The property %1 was assigned the value '%2' due to modification by a client.",
            "OK",
            2,
            { "string", "string" },
            { "The name of the property.", "The value assigned for property." },
            { "This argument shall contain the name of the property.  The value should follow RFC6901-defined JSON pointer notation rules.", "This argument shall contain the value assigned for property." },
            "No resolution is required.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ResourceVersionIncompatible :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourceVersionIncompatible(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.ResourceVersionIncompatible",
            "Indicates that an incompatible version of software was detected.  Examples of this are after a component or system-level software update.",
            "This message shall indicate that an incompatible version of software was detected.",
            "An incompatible version of software '%1' was detected.",
            "Warning",
            1,
            { "string" },
            { "The name of the software component." },
            { "This argument shall contain the name of the relevant software component or package.  This might include both name and version information." },
            "Compare the version of the resource with the compatible version of the software.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ResourceSelfTestFailed :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourceSelfTestFailed(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.ResourceSelfTestFailed",
            "Indicates that a self-test has failed.  Suggested resolution may be provided as OEM data.",
            "This message shall indicate that a self-test has failed.",
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

struct ResourceSelfTestCompleted :
    public RedfishMessage<>
{
    template <typename... Args>
    ResourceSelfTestCompleted(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.ResourceSelfTestCompleted",
            "Indicates that a self-test has completed.",
            "This message shall indicate that a self-test has completed.",
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

struct TestMessage :
    public RedfishMessage<>
{
    template <typename... Args>
    TestMessage(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.TestMessage",
            "A test message used to validate event delivery mechanisms.",
            "This message shall indicate a test of an event delivery mechanism, such as the creation of a LogEntry resource or an event transmission.",
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

struct AggregationSourceDiscovered :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    AggregationSourceDiscovered(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.AggregationSourceDiscovered",
            "Indicates that a new aggregation source was discovered.",
            "This message shall indicate an aggregation source was discovered in the network or host.  The `OriginOfCondition` property shall contain a link to the matching `ConnectionMethod` resource, if known.",
            "An aggregation source of connection method '%1' located at '%2' was discovered.",
            "OK",
            2,
            { "string", "string" },
            { "The connection method of the discovered aggregation source.", "The location of the discovered aggregation source, such as an IP address or service entry point." },
            { "This argument shall contain a value from the `ConnectionMethodType` enumeration defined in the `ConnectionMethod` schema that describes connection method of the discovered aggregation source.", "This argument shall contain the location of the discovered aggregation source, such as an IP address or service entry point." },
            "The aggregation source is available to the service and can be identified using the identified connection method.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("This message was deprecated in favor of `LicenseExpired` in the License Message Registry.")]]
LicenseExpired :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    LicenseExpired(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.LicenseExpired",
            "Indicates that a license has expired.",
            "This message shall indicate that a license has expired.",
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

struct [[deprecated ("This message was deprecated in favor of `ResourceChanged`.")]]
LicenseChanged :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    LicenseChanged(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.LicenseChanged",
            "Indicates that a license has changed.",
            "This message shall indicate that a license has changed.",
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

struct [[deprecated ("This message was deprecated in favor of `LicenseAdded` in the License Message Registry.")]]
LicenseAdded :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    LicenseAdded(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.4.2.LicenseAdded",
            "Indicates that a license was added.",
            "This message shall indicate that a license was added.",
            "A license for '%1' was added.  The following message was returned: '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The relevant resource.", "The message returned from the license validation process." },
            { "This argument shall contain the name of the relevant resource or service affected by the software license.", "This argument shall contain the message returned from the license validation process or software." },
            "See vendor specific instructions for specific actions.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace ResourceEvent_1_4_2
}; // namespace registries
}; // namespace redfish
