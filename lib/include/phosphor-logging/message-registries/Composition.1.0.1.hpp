#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace Composition_1_0_1
{

struct [[deprecated ("Use Composition_1_1_2")]]
ResourceBlockChanged :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourceBlockChanged(Args... args) :
        RedfishMessage(
            "Composition.1.0.1.ResourceBlockChanged",
            "Indicates that a resource block has changed.  This is not used whenever there is another event message for that specific change, such as only the state has changed.",
            "This message shall be used to indicate that one or more properties of a resource block have changed.  This message shall not be used for cases where a more specific message could be used in its place.",
            "The resource block '%1' has changed on the service.",
            "OK",
            1,
            { "string" },
            { "The `Id` of the resource block that has changed." },
            { "The value of this argument shall be a string containing the value of the `Id` property of the resource block that has changed." },
            "Refresh your cached version of the resource block to get the updated information from the service.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Composition_1_1_2")]]
ResourceBlockStateChanged :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourceBlockStateChanged(Args... args) :
        RedfishMessage(
            "Composition.1.0.1.ResourceBlockStateChanged",
            "Indicates that the state of a resource block has changed, specifically the value of the `State` property within `Status` has changed.",
            "This message shall be used to indicate that the `State` property within the `Status` object of a resource block has changed.",
            "The state of the resource block '%1' has changed.",
            "OK",
            1,
            { "string" },
            { "The `Id` of the resource block that has changed." },
            { "The value of this argument shall be a string containing the value of the `Id` property of the resource block that has changed." },
            "Refresh your cached version of the resource block to get the updated information from the service.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Composition_1_1_2")]]
ResourceBlockCompositionStateChanged :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourceBlockCompositionStateChanged(Args... args) :
        RedfishMessage(
            "Composition.1.0.1.ResourceBlockCompositionStateChanged",
            "Indicates that the composition state of a resource block has changed, specifically the value of the `CompositionState` property within `CompositionStatus` has changed.",
            "This message shall be used to indicate that the `CompositionState` property within the `CompositionStatus` object of a resource block has changed.",
            "The composition status of the resource block '%1' has changed.",
            "OK",
            1,
            { "string" },
            { "The `Id` of the resource block that has changed." },
            { "The value of this argument shall be a string containing the value of the `Id` property of the resource block that has changed." },
            "Refresh your cached version of the resource block to get the updated information from the service.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Composition_1_1_2")]]
ResourceZoneMembershipChanged :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourceZoneMembershipChanged(Args... args) :
        RedfishMessage(
            "Composition.1.0.1.ResourceZoneMembershipChanged",
            "Indicates that the membership of a resource zone has changed due to resource blocks being added or removed from the resource zone.",
            "This message shall be used to indicate that entries found in the `ResourceBlocks` array within the `Links` object for a resource zone have been added or removed.",
            "The membership of resource zone '%1' has been changed.",
            "OK",
            1,
            { "string" },
            { "The `Id` of the resource zone that has changed." },
            { "The value of this argument shall be a string containing the value of the `Id` property of the resource zone that has changed." },
            "Refresh your cached version of the resource zone to get the updated information from the service.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Composition_1_1_2")]]
ResourceBlockInUse :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourceBlockInUse(Args... args) :
        RedfishMessage(
            "Composition.1.0.1.ResourceBlockInUse",
            "Indicates that the composition request contains a resource block that is unable to participate in more compositions.",
            "This message shall be used to indicate that the composition request contains a resource block that is unable to participate in more compositions due to restrictions specified in its `CompositionStatus` property.  For example, the resource block has reached its composition limit specified by the `MaxCompositions` property.",
            "The resource block with Id '%1' cannot be part of any new compositions.",
            "Warning",
            1,
            { "string" },
            { "The `Id` of the resource block that cannot be part of any new compositions." },
            { "The value of this argument shall be a string containing the value of the `Id` property of the resource block that cannot be part of any new compositions." },
            "Remove the resource block from the request and resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace Composition_1_0_1
}; // namespace registries
}; // namespace redfish
