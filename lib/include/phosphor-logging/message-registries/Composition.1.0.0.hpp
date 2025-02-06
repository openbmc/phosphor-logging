#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace Composition_1_0_0
{

struct [[deprecated ("Use Composition_1_1_2")]]
ResourceBlockChanged :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourceBlockChanged(Args... args) :
        RedfishMessage(
            "Composition.1.0.0.ResourceBlockChanged",
            "Indicates that a Resource Block has changed.  This is not used whenever there is another event message for that specific change, such as only the state has changed.",
            "This message shall be used to indicate that one or more properties of a Resource Block have changed.  This message shall not be used for cases where a more specific message could be used in its place.",
            "The Resource Block '%1' has changed on the service.",
            "OK",
            1,
            { "string" },
            { "The Id of the Resource Block that has changed." },
            { "The value of this argument shall be a string containing the value of the Id property of the Resource Block that has changed." },
            "Refresh your cached version of the Resource Block to get the updated information from the service.",
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
            "Composition.1.0.0.ResourceBlockStateChanged",
            "Indicates that the state of a Resource Block has changed, specifically the value of the Status.State property has changed.",
            "This message shall be used to indicate that the State property within the Status object of a Resource Block has changed.",
            "The state of the Resource Block '%1' has changed.",
            "OK",
            1,
            { "string" },
            { "The Id of the Resource Block that has changed." },
            { "The value of this argument shall be a string containing the value of the Id property of the Resource Block that has changed." },
            "Refresh your cached version of the Resource Block to get the updated information from the service.",
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
            "Composition.1.0.0.ResourceBlockCompositionStateChanged",
            "Indicates that the composition state of a Resource Block has changed, specifically the value of the CompositionStatus.CompositionState property has changed.",
            "This message shall be used to indicate that the CompositionState property within the CompositionStatus object of a Resource Block has changed.",
            "The composition status of the Resource Block '%1' has changed.",
            "OK",
            1,
            { "string" },
            { "The Id of the Resource Block that has changed." },
            { "The value of this argument shall be a string containing the value of the Id property of the Resource Block that has changed." },
            "Refresh your cached version of the Resource Block to get the updated information from the service.",
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
            "Composition.1.0.0.ResourceZoneMembershipChanged",
            "Indicates that the membership of a Resource Zone has changed due to Resource Blocks being added or removed from the Resource Zone.",
            "This message shall be used to indicate that entries found in the ResourceBlocks array within the Links object for a Resource Zone have been added or removed.",
            "The membership of Resource Zone '%1' has been changed.",
            "OK",
            1,
            { "string" },
            { "The Id of the Resource Zone that has changed." },
            { "The value of this argument shall be a string containing the value of the Id property of the Resource Zone that has changed." },
            "Refresh your cached version of the Resource Zone to get the updated information from the service.",
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
            "Composition.1.0.0.ResourceBlockInUse",
            "Indicates that the composition request contains a Resource Block that is unable to participate in more compositions.",
            "This message shall be used to indicate that the composition request contains a Resource Block that is unable to participate in more compositions due to restrictions specified in its CompositionStatus property.  For example, the Resource Block has reached its composition limit specified by the MaxCompositions property.",
            "The Resource Block with Id '%1' cannot be part of any new compositions.",
            "Warning",
            1,
            { "string" },
            { "The Id of the Resource Block that cannot be part of any new compositions." },
            { "The value of this argument shall be a string containing the value of the Id property of the Resource Block that cannot be part of any new compositions." },
            "Remove the Resource Block from the request and resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace Composition_1_0_0
}; // namespace registries
}; // namespace redfish
