#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace ResourceEvent_1_0_1
{

struct [[deprecated ("Use ResourceEvent_1_4_2")]]
ResourceCreated :
    public RedfishMessage<>
{
    template <typename... Args>
    ResourceCreated(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.0.1.ResourceCreated",
            "Indicates that all conditions of a successful creation operation have been met.",
            "Indicates that all conditions of a successful creation operation have been met.",
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
            "ResourceEvent.1.0.1.ResourceRemoved",
            "Indicates that all conditions of a successful remove operation have been met.",
            "Indicates that all conditions of a successful remove operation have been met.",
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
            "ResourceEvent.1.0.1.ResourceErrorsDetected",
            "The resource property %1 has detected errors of type %2. Examples would be drive IO errors, or network link errors.",
            "The resource property %1 has detected errors of type %2. Examples would be drive IO errors, or network link errors.",
            "The resource property %1 has detected errors of type %2.",
            "Warning",
            2,
            { "string", "string" },
            {},
            {},
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
            "ResourceEvent.1.0.1.ResourceErrorsCorrected",
            "The resource property %1 has corrected errors of type %2. Examples would be drive IO errors, or network link errors.",
            "The resource property %1 has corrected errors of type %2. Examples would be drive IO errors, or network link errors.",
            "The resource property %1 has corrected errors of type %2.",
            "OK",
            2,
            { "string", "string" },
            {},
            {},
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
            "ResourceEvent.1.0.1.ResourceErrorThresholdExceeded",
            "The resource property %1 has exceeded error threshold of value %2. Examples would be drive IO errors, or network link errors.",
            "The resource property %1 has exceeded error threshold of value %2. Examples would be drive IO errors, or network link errors.",
            "The resource property %1 has exceeded error threshold of value %2.",
            "Critical",
            2,
            { "string", "number" },
            {},
            {},
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
            "ResourceEvent.1.0.1.ResourceErrorThresholdCleared",
            "The resource property %1 has cleared the error threshold of value %2. Examples would be drive IO errors, or network link errors.",
            "The resource property %1 has cleared the error threshold of value %2. Examples would be drive IO errors, or network link errors.",
            "The resource property %1 has cleared the error threshold of value %2.",
            "OK",
            2,
            { "string", "number" },
            {},
            {},
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
            "ResourceEvent.1.0.1.ResourceWarningThresholdExceeded",
            "The resource property %1 has cleared its warning threshold of value %2. Examples would be drive IO errors, or network link errors. Suggested resolution may be provided as OEM data.",
            "The resource property %1 has cleared its warning threshold of value %2. Examples would be drive IO errors, or network link errors. Suggested resolution may be provided as OEM data.",
            "The resource property %1 has cleared its warning threshold of value %2.",
            "Warning",
            2,
            { "string", "number" },
            {},
            {},
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
            "ResourceEvent.1.0.1.ResourceWarningThresholdCleared",
            "The resource property %1 has cleared the warning threshold of value %2. Examples would be drive IO errors, or network link errors. Suggested resolution may be provided as OEM data.",
            "The resource property %1 has cleared the warning threshold of value %2. Examples would be drive IO errors, or network link errors. Suggested resolution may be provided as OEM data.",
            "The resource property %1 has cleared the warning threshold of value %2.",
            "OK",
            2,
            { "string", "number" },
            {},
            {},
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
            "ResourceEvent.1.0.1.ResourceStatusChangedOK",
            "The state of resource %1 has changed to state type %2. The state types shall be used from Resource.State.",
            "The state of resource %1 has changed to state type %2. The state types shall be used from Resource.State.",
            "The state of resource %1 has changed to state type %2.",
            "OK",
            2,
            { "string", "string" },
            {},
            {},
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
            "ResourceEvent.1.0.1.ResourceStatusChangedWarning",
            "The state of resource %1 has changed to state type %2. The state types shall be used from Resource.State.",
            "The state of resource %1 has changed to state type %2. The state types shall be used from Resource.State.",
            "The state of resource %1 has changed to state type %2.",
            "Warning",
            2,
            { "string", "string" },
            {},
            {},
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
            "ResourceEvent.1.0.1.ResourceStatusChangedCritical",
            "The state of resource %1 has changed to state type %2. The state types shall be used from Resource.State.",
            "The state of resource %1 has changed to state type %2. The state types shall be used from Resource.State.",
            "The state of resource %1 has changed to state type %2.",
            "Critical",
            2,
            { "string", "string" },
            {},
            {},
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
            "ResourceEvent.1.0.1.URIForResourceChanged",
            "The URI for the model resource has changed. Examples for this would be physical component replacement or redistribution.",
            "The URI for the model resource has changed. Examples for this would be physical component replacement or redistribution.",
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
            "ResourceEvent.1.0.1.ResourceChanged",
            "One or more resource properties have changed. This is not used whenever there is another event message for that specific change, such as only the state has changed.",
            "One or more resource properties have changed. This is not used whenever there is another event message for that specific change, such as only the state has changed.",
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
            "ResourceEvent.1.0.1.ResourceVersionIncompatible",
            "An incompatible version of software %1 has been detected. Examples may be after a component or system level software update.",
            "An incompatible version of software %1 has been detected. Examples may be after a component or system level software update.",
            "An incompatible version of software %1 has been detected.",
            "Warning",
            1,
            { "string" },
            {},
            {},
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
            "ResourceEvent.1.0.1.ResourceSelfTestFailed",
            "A self-test has failed. Suggested resolution may be provided as OEM data.",
            "A self-test has failed. Suggested resolution may be provided as OEM data.",
            "A self-test has failed. The following message was returned %1.",
            "Critical",
            1,
            { "string" },
            {},
            {},
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
            "ResourceEvent.1.0.1.ResourceSelfTestCompleted",
            "A self-test has completed.",
            "A self-test has completed.",
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
LicenseExpired :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    LicenseExpired(Args... args) :
        RedfishMessage(
            "ResourceEvent.1.0.1.LicenseExpired",
            "A license for %1 has expired.",
            "A license for %1 has expired.",
            "A license for %1 has expired. The following message was returned %2.",
            "Warning",
            2,
            { "string", "string" },
            {},
            {},
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
            "ResourceEvent.1.0.1.LicenseChanged",
            "A license for %1 has changed.",
            "A license for %1 has changed.",
            "A license for %1 has changed. The following message was returned %2.",
            "Warning",
            2,
            { "string", "string" },
            {},
            {},
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
            "ResourceEvent.1.0.1.LicenseAdded",
            "A license for %1 has been added.",
            "A license for %1 has been added.",
            "A license for %1 has been added. The following message was returned %2.",
            "OK",
            2,
            { "string", "string" },
            {},
            {},
            "See vendor specific instructions for specific actions.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace ResourceEvent_1_0_1
}; // namespace registries
}; // namespace redfish
