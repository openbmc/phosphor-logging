#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace Update_1_1_1
{

struct TargetDetermined :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    TargetDetermined(Args... args) :
        RedfishMessage(
            "Update.1.1.1.TargetDetermined",
            "Indicates that a target resource or device for an image was determined for update.",
            "This message shall indicate that a target resource or device for an image was determined for update.",
            "The target device '%1' will be updated with image '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The URI or name of the target that will be updated.", "The identifier for the image." },
            { "This argument shall contain the URI of the resource or a unique name of the device that will be updated.", "This argument shall contain the identifier for the image." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct AllTargetsDetermined :
    public RedfishMessage<>
{
    template <typename... Args>
    AllTargetsDetermined(Args... args) :
        RedfishMessage(
            "Update.1.1.1.AllTargetsDetermined",
            "Indicates that all target resources or devices for an update operation were determined by the service.",
            "This message shall indicate that all target resources or devices for an update operation were determined by the service.",
            "All the target devices to be updated were determined.",
            "OK",
            0,
            {},
            {},
            {},
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct NoTargetsDetermined :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    NoTargetsDetermined(Args... args) :
        RedfishMessage(
            "Update.1.1.1.NoTargetsDetermined",
            "Indicates that no target resource or device for an image was determined for update.",
            "This message shall indicate that no target resource or device for an image was determined for update.",
            "No target device will be updated with image '%1'.",
            "OK",
            1,
            { "string" },
            { "The identifier for the image." },
            { "This argument shall contain the identifier for the image." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct UpdateInProgress :
    public RedfishMessage<>
{
    template <typename... Args>
    UpdateInProgress(Args... args) :
        RedfishMessage(
            "Update.1.1.1.UpdateInProgress",
            "Indicates that an update is in progress.",
            "This message shall indicate that an update is in progress.",
            "An update is in progress.",
            "OK",
            0,
            {},
            {},
            {},
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TransferringToComponent :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    TransferringToComponent(Args... args) :
        RedfishMessage(
            "Update.1.1.1.TransferringToComponent",
            "Indicates that the service is transferring an image to a component.",
            "This message shall indicate that the service is transferring an image to a component.",
            "Image '%1' is being transferred to '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The identifier for the image.", "The URI or name of the target receiving the image." },
            { "This argument shall contain the identifier for the image.", "This argument shall contain the URI of the resource or a unique name of the component receiving the image." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct VerifyingAtComponent :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    VerifyingAtComponent(Args... args) :
        RedfishMessage(
            "Update.1.1.1.VerifyingAtComponent",
            "Indicates that a component is verifying an image.",
            "This message shall indicate that a component is verifying an image.",
            "Image '%1' is being verified at '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The identifier for the image.", "The URI or name of the target verifying the image." },
            { "This argument shall contain the identifier for the image.", "This argument shall contain the URI of the resource or a unique name of the component verifying the image." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct InstallingOnComponent :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    InstallingOnComponent(Args... args) :
        RedfishMessage(
            "Update.1.1.1.InstallingOnComponent",
            "Indicates that a component is installing an image.",
            "This message shall indicate that a component is installing an image.",
            "Image '%1' is being installed on '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The identifier for the image.", "The URI or name of the target installing the image." },
            { "This argument shall contain the identifier for the image.", "This argument shall contain the URI of the resource or a unique name of the component installing the image." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ApplyingOnComponent :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ApplyingOnComponent(Args... args) :
        RedfishMessage(
            "Update.1.1.1.ApplyingOnComponent",
            "Indicates that a component is applying an image.",
            "This message shall indicate that a component is applying an image.",
            "Image '%1' is being applied on '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The identifier for the image.", "The URI or name of the target applying the image." },
            { "This argument shall contain the identifier for the image.", "This argument shall contain the URI of the resource or a unique name of the component applying the image." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TransferFailed :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    TransferFailed(Args... args) :
        RedfishMessage(
            "Update.1.1.1.TransferFailed",
            "Indicates that the service failed to transfer an image to a component.",
            "This message shall indicate that the service failed to transfer an image to a component.",
            "Transfer of image '%1' to '%2' failed.",
            "Critical",
            2,
            { "string", "string" },
            { "The identifier for the image, or the filename of the image if an identifier cannot be determined.", "The URI or name of the target receiving the image." },
            { "This argument shall contain the identifier for the image.  If the identifier cannot be determined, this argument shall contain the filename of the image.", "This argument shall contain the URI of the resource or a unique name of the component receiving the image." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct VerificationFailed :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    VerificationFailed(Args... args) :
        RedfishMessage(
            "Update.1.1.1.VerificationFailed",
            "Indicates that the service or component failed to verify an image.",
            "This message shall indicate that the service or component failed to verify an image.  Some examples of image verification failure include: invalid signature, invalid checksum, or unknown image format.",
            "Verification of image '%1' at '%2' failed.",
            "Critical",
            2,
            { "string", "string" },
            { "The identifier for the image, or the filename of the image if an identifier cannot be determined.", "The URI or name of the target verifying the image." },
            { "This argument shall contain the identifier for the image.  If the identifier cannot be determined, this argument shall contain the filename of the image.", "This argument shall contain the URI of the resource or a unique name of the component verifying the image." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ApplyFailed :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ApplyFailed(Args... args) :
        RedfishMessage(
            "Update.1.1.1.ApplyFailed",
            "Indicates that the component failed to apply an image.",
            "This message shall indicate that the component failed to apply an image.",
            "Installation of image '%1' to '%2' failed.",
            "Critical",
            2,
            { "string", "string" },
            { "The identifier for the image.", "The URI or name of the target applying the image." },
            { "This argument shall contain the identifier for the image.", "This argument shall contain the URI of the resource or a unique name of the component applying the image." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ActivateFailed :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ActivateFailed(Args... args) :
        RedfishMessage(
            "Update.1.1.1.ActivateFailed",
            "Indicates that the component failed to activate the image.",
            "This message shall indicate that the component failed to activate the image.",
            "Activation of image '%1' on '%2' failed.",
            "Critical",
            2,
            { "string", "string" },
            { "The identifier for the image.", "The URI or name of the target activating the image." },
            { "This argument shall contain the identifier for the image.", "This argument shall contain the URI of the resource or a unique name of the component activating the image." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct AwaitToUpdate :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    AwaitToUpdate(Args... args) :
        RedfishMessage(
            "Update.1.1.1.AwaitToUpdate",
            "Indicates that the resource or device is waiting for an action to proceed with installing an image.",
            "This message shall indicate that the resource or device is waiting for an action to proceed with installing an image.",
            "Awaiting for an action to proceed with installing image '%1' on '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The identifier for the image.", "The URI or name of the target awating for an action to install the image." },
            { "This argument shall contain the identifier for the image.", "This argument shall contain the URI of the resource or a unique name of the component awaiting for an action to install the image." },
            "Perform the requested action to advance the update operation.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct AwaitToActivate :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    AwaitToActivate(Args... args) :
        RedfishMessage(
            "Update.1.1.1.AwaitToActivate",
            "Indicates that the resource or device is waiting for an action to proceed with activating an image.",
            "This message shall indicate that the resource or device is waiting for an action to proceed with activating an image.",
            "Awaiting for an action to proceed with activating image '%1' on '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The identifier for the image.", "The URI or name of the target awating for an action to activate the image." },
            { "This argument shall contain the identifier for the image.", "This argument shall contain the URI of the resource or a unique name of the component awaiting for an action to activate the image." },
            "Perform the requested action to advance the update operation.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct UpdateSuccessful :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    UpdateSuccessful(Args... args) :
        RedfishMessage(
            "Update.1.1.1.UpdateSuccessful",
            "Indicates that a resource or device was updated.",
            "This message shall indicate that a resource or device was updated.",
            "Device '%1' successfully updated with image '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The URI or name of the device that was updated.", "The identifier for the image." },
            { "This argument shall contain the URI of the resource or a unique name of the device that was updated.", "This argument shall contain the identifier for the image." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct OperationTransitionedToJob :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    OperationTransitionedToJob(Args... args) :
        RedfishMessage(
            "Update.1.1.1.OperationTransitionedToJob",
            "Indicates that the update operation transitioned to a job for managing the progress of the operation.",
            "This message shall indicate that the update operation transitioned to a job for managing the progress of the operation.",
            "The update operation has transitioned to the job at URI '%1'.",
            "OK",
            1,
            { "string" },
            { "The job URI." },
            { "This argument shall contain the URI of the `Job` resource that is managing the progress of the update operation." },
            "Follow the referenced job and monitor the job for further updates.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace Update_1_1_1
}; // namespace registries
}; // namespace redfish
