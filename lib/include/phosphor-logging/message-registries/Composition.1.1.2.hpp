#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace Composition_1_1_2
{

struct ResourceBlockChanged :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourceBlockChanged(Args... args) :
        RedfishMessage(
            "Composition.1.1.2.ResourceBlockChanged",
            "Indicates that a resource block has changed.  This is not used whenever there is another event message for that specific change, such as when only the state has changed.",
            "This message shall indicate that one or more properties of a `ResourceBlock` resource have changed.  This message shall not be used for cases where a more specific message could be used in its place.",
            "Resource block '%1' has changed on the service.",
            "OK",
            1,
            { "string" },
            { "The `Id` of the resource block that has changed." },
            { "This argument shall contain the value of the `Id` property of the `ResourceBlock` resource that has changed." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ResourceBlockStateChanged :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourceBlockStateChanged(Args... args) :
        RedfishMessage(
            "Composition.1.1.2.ResourceBlockStateChanged",
            "Indicates that the state of a resource block has changed, specifically the value of the `State` property within `Status`.",
            "This message shall indicate that the `State` property within the `Status` object of a `ResourceBlock` resource has changed.",
            "The state of resource block '%1' has changed.",
            "OK",
            1,
            { "string" },
            { "The `Id` of the resource block that has changed." },
            { "This argument shall contain the value of the `Id` property of the `ResourceBlock` resource that has changed." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ResourceBlockCompositionStateChanged :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourceBlockCompositionStateChanged(Args... args) :
        RedfishMessage(
            "Composition.1.1.2.ResourceBlockCompositionStateChanged",
            "Indicates that the composition state of a resource block has changed, specifically the value of the `CompositionState` property within `CompositionStatus`.",
            "This message shall indicate that the `CompositionState` property within the `CompositionStatus` object of a `ResourceBlock` resource has changed.",
            "The composition status of the resource block '%1' has changed.",
            "OK",
            1,
            { "string" },
            { "The `Id` of the resource block that has changed." },
            { "This argument shall contain the value of the `Id` property of the `ResourceBlock` resource that has changed." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ResourceZoneMembershipChanged :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourceZoneMembershipChanged(Args... args) :
        RedfishMessage(
            "Composition.1.1.2.ResourceZoneMembershipChanged",
            "Indicates that the membership of a resource zone has changed due to resource blocks being added or removed from the resource zone.",
            "This message shall indicate that entries found in the `ResourceBlocks` array within the `Links` object for a resource zone have been added or removed.",
            "The membership of resource zone '%1' has been changed.",
            "OK",
            1,
            { "string" },
            { "The `Id` of the resource zone that has changed." },
            { "This argument shall contain the value of the `Id` property of the `Zone` resource that represents the resource zone that has changed." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ResourceBlockInUse :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourceBlockInUse(Args... args) :
        RedfishMessage(
            "Composition.1.1.2.ResourceBlockInUse",
            "Indicates that the composition request contains a resource block that is unable to participate in more compositions.",
            "This message shall indicate that the composition request contains a `ResourceBlock` resource that is unable to participate in more compositions due to restrictions specified in its `CompositionStatus` property.  For example, the resource block has reached its composition limit specified by the `MaxCompositions` property.",
            "Resource block '%1' cannot be part of any new compositions.",
            "Warning",
            1,
            { "string" },
            { "The `Id` of the resource block that cannot be part of any new compositions." },
            { "This argument shall contain the value of the `Id` property of the `ResourceBlock` resource that cannot be part of any new compositions." },
            "Remove the resource block from the request and resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct EmptyManifest :
    public RedfishMessage<>
{
    template <typename... Args>
    EmptyManifest(Args... args) :
        RedfishMessage(
            "Composition.1.1.2.EmptyManifest",
            "Indicates that the manifest contains no stanzas or that a stanza in the manifest contains no request.",
            "This message shall indicate that the manifest contains no stanzas or that a stanza in the manifest contains no request.",
            "The provided manifest is empty or a stanza in the manifest contains no request.",
            "Warning",
            0,
            {},
            {},
            {},
            "Provide a request content for the manifest and resubmit.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct UnableToProcessStanzaRequest :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    UnableToProcessStanzaRequest(Args... args) :
        RedfishMessage(
            "Composition.1.1.2.UnableToProcessStanzaRequest",
            "Indicates that the manifest provided for the `Compose` action contains a stanza with `Content` that could not be processed.",
            "This message shall indicate that the manifest provided for the `Compose` action contains a stanza with `Content` that could not be processed.",
            "The provided manifest for the Compose action of type %1 contains a stanza with Id of value '%2' with a Content parameter that could not be processed.",
            "Critical",
            2,
            { "string", "string" },
            { "The `RequestType` of the `Compose` action.", "The `Id` of the stanza." },
            { "This argument shall contain the value of the `RequestType` parameter of the `Compose` action.", "This argument shall contain the value of the `Id` parameter of the stanza." },
            "Add the Content parameter to the stanza or remove the stanza, and resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ResourceBlockInvalid :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourceBlockInvalid(Args... args) :
        RedfishMessage(
            "Composition.1.1.2.ResourceBlockInvalid",
            "Indicates that the `Id` of a referenced resource block is no longer valid.",
            "This message shall indicate that the `Id` of a referenced `ResourceBlock` resource is no longer valid.",
            "Resource block '%1' is not valid.",
            "Critical",
            1,
            { "string" },
            { "The `Id` of the resource block." },
            { "This argument shall contain the value of the `Id` property of the `ResourceBlock` resource that is no longer valid." },
            "Remove the resource block and resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ResourceBlockNotFound :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourceBlockNotFound(Args... args) :
        RedfishMessage(
            "Composition.1.1.2.ResourceBlockNotFound",
            "Indicates that the referenced resource block was not found.",
            "This message shall indicate that the referenced `ResourceBlock` resource was not found.",
            "Resource block '%1' was not found.",
            "Critical",
            1,
            { "string" },
            { "The URI of the resource block." },
            { "This argument shall contain the value of the URI of the `ResourceBlock` resource that was not found." },
            "Remove the resource block and resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct NoResourceMatch :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    NoResourceMatch(Args... args) :
        RedfishMessage(
            "Composition.1.1.2.NoResourceMatch",
            "Indicates that the service could not find a matching resource based on the given parameters.",
            "This message shall indicate that the service could not find a matching resource based on the given parameters.",
            "The requested resources of type '%1' are not available for allocation.",
            "Critical",
            1,
            { "string" },
            { "The resource type requested." },
            { "This argument shall contain the schema name of the requested resource." },
            "Change parameters associated with the resource, such as quantity or performance, and resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct IncompatibleZone :
    public RedfishMessage<>
{
    template <typename... Args>
    IncompatibleZone(Args... args) :
        RedfishMessage(
            "Composition.1.1.2.IncompatibleZone",
            "Indicates that not all referenced resource blocks are in the same resource zone.",
            "This message shall indicate that not all referenced `ResourceBlock` resources are in the same resource zone.",
            "The requested resource blocks span multiple resource zones.",
            "Critical",
            0,
            {},
            {},
            {},
            "Request resource blocks from the same resource zone.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct SpecifiedResourceAlreadyReserved :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    SpecifiedResourceAlreadyReserved(Args... args) :
        RedfishMessage(
            "Composition.1.1.2.SpecifiedResourceAlreadyReserved",
            "Indicates that a resource block is already reserved in response to a specific composition request.",
            "This message shall indicate that a `ResourceBlock` resource is already reserved in response to a specific composition request.",
            "Resource block '%1' is already reserved under reservation '%2'.",
            "Critical",
            2,
            { "string", "string" },
            { "The `Id` of the resource block.", "The `Id` of the reservation." },
            { "This argument shall contain the value of the `Id` property of the `ResourceBlock` resource that is already reserved.", "This argument shall contain the value of the `Id` property of the `CompositionReservation` resource containing the requested resource." },
            "Delete the reservation containing the resource block or select a different resource block and resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ConstrainedResourceAlreadyReserved :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ConstrainedResourceAlreadyReserved(Args... args) :
        RedfishMessage(
            "Composition.1.1.2.ConstrainedResourceAlreadyReserved",
            "Indicates that the requested resources are already reserved in response to a constrained composition request.",
            "This message shall indicate that the requested resources are already reserved in response to a constrained composition request.",
            "The requested resources are reserved under reservation '%1'.",
            "Critical",
            1,
            { "string" },
            { "The `Id` of the reservation." },
            { "This argument shall contain the value of the `Id` property of the `CompositionReservation` resource containing the requested resources." },
            "Delete the reservation containing the resources and resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace Composition_1_1_2
}; // namespace registries
}; // namespace redfish
