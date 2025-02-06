#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace StorageDevice_1_0_1
{

struct [[deprecated ("Use StorageDevice_1_4_0")]]
ControllerOK :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ControllerOK(Args... args) :
        RedfishMessage(
            "StorageDevice.1.0.1.ControllerOK",
            "The storage controller health has changed to OK.",
            "This message shall be used to indicate that the storage controller health has changed to OK.",
            "The health of the storage controller located in '%1' has changed to OK.",
            "OK",
            1,
            { "string" },
            { "The location of the storage controller." },
            { "The value of this argument shall be a string containing the storage controller location." },
            "No action required.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
ControllerFailure :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ControllerFailure(Args... args) :
        RedfishMessage(
            "StorageDevice.1.0.1.ControllerFailure",
            "A storage controller failure has been detected.",
            "This message shall be used to indicate that a storage controller failure condition has been detected due to a hardware, communication, or firmware failure.",
            "A failure condition for the storage controller located in '%1' has been detected.",
            "Critical",
            1,
            { "string" },
            { "The location of the storage controller." },
            { "The value of this argument shall be a string containing the storage controller location." },
            "Reseat the storage controller in the PCI slot.  Update the controller to the latest firmware version.  If the issue persists, replace the controller.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
DriveOK :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    DriveOK(Args... args) :
        RedfishMessage(
            "StorageDevice.1.0.1.DriveOK",
            "The health of a drive has changed to OK.",
            "This message shall be used to indicate a drive health has changed to OK.",
            "The health of the drive located in '%1' has changed to OK.",
            "OK",
            1,
            { "string" },
            { "The location of the drive." },
            { "The value of this argument shall be a string containing the drive location." },
            "No action needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
DrivePredictiveFailure :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    DrivePredictiveFailure(Args... args) :
        RedfishMessage(
            "StorageDevice.1.0.1.DrivePredictiveFailure",
            "A predictive drive failure condition has been detected.",
            "This message shall be used to indicate a drive predictive failure condition has been detected.",
            "A predictive failure condition for the drive located in '%1' has been detected.",
            "Warning",
            1,
            { "string" },
            { "The location of the drive." },
            { "The value of this argument shall be a string containing the drive location." },
            "If this drive is not part of a fault-tolerant volume, first back up all data, then replace the drive and restore all data afterward.  If this drive is part of a fault-tolerant volume, replace this drive as soon as possible as long as the volume health is OK.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
DrivePredictiveFailureCleared :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    DrivePredictiveFailureCleared(Args... args) :
        RedfishMessage(
            "StorageDevice.1.0.1.DrivePredictiveFailureCleared",
            "A previously detected predictive failure condition on a drive has been cleared.",
            "This message shall be used to indicate a predictive failure condition on a drive has been cleared, but any other failure conditions remain.",
            "A predictive failure condition for the drive located in '%1' has been cleared.",
            "OK",
            1,
            { "string" },
            { "The location of the drive." },
            { "The value of this argument shall be a string containing the drive location." },
            "No action needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
DriveFailure :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    DriveFailure(Args... args) :
        RedfishMessage(
            "StorageDevice.1.0.1.DriveFailure",
            "A drive failure condition has been detected.",
            "This message shall be used to indicate a drive failure condition detected due to a drive not installed correctly or has failed.",
            "A failure condition for the drive located in '%1' has been detected.",
            "Critical",
            1,
            { "string" },
            { "The location of the drive." },
            { "The value of this argument shall be a string containing the drive location." },
            "Ensure all cables are properly and securely connected.  Ensure all drives are fully seated.  Replace the defective cables, drive, or both.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
DriveFailureCleared :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    DriveFailureCleared(Args... args) :
        RedfishMessage(
            "StorageDevice.1.0.1.DriveFailureCleared",
            "A previously detected failure condition on a drive has been cleared.",
            "This message shall be used to indicate a failure condition on a drive has been cleared, but any predictive failure or other conditions remain.",
            "A failure condition for the drive located in '%1' has been cleared.",
            "OK",
            1,
            { "string" },
            { "The location of the drive." },
            { "The value of this argument shall be a string containing the drive location." },
            "No action needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
DriveInserted :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    DriveInserted(Args... args) :
        RedfishMessage(
            "StorageDevice.1.0.1.DriveInserted",
            "A drive has been inserted.",
            "This message shall be used to indicate a drive has been inserted.",
            "The drive located in '%1' was inserted.",
            "OK",
            1,
            { "string" },
            { "The location of the drive." },
            { "The value of this argument shall be a string containing the drive location." },
            "If the drive is not properly displayed, attempt to refresh the cached data.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
DriveRemoved :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    DriveRemoved(Args... args) :
        RedfishMessage(
            "StorageDevice.1.0.1.DriveRemoved",
            "A drive has been removed.",
            "This message shall be used to indicate a drive has been removed.",
            "The drive located in '%1' was removed.",
            "Critical",
            1,
            { "string" },
            { "The location of the drive." },
            { "The value of this argument shall be a string containing the drive location." },
            "If the drive is still displayed, attempt to refresh the cached data.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
VolumeOK :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    VolumeOK(Args... args) :
        RedfishMessage(
            "StorageDevice.1.0.1.VolumeOK",
            "A volume health has changed to OK.",
            "This message shall be used to indicate a volume health has changed to OK.",
            "The health of volume '%1' that is attached to the storage controller located in '%2' has changed to OK.",
            "OK",
            2,
            { "string", "string" },
            { "The identifier of the volume.", "The location of the storage controller." },
            { "The value of this argument shall be a string containing an identifier for the volume.  The identifier may contain `Id`, `Name`, `Identifiers`, `LogicalUnitNumber`, or other data used to uniquely identify the volume.", "The value of this argument shall be a string containing the storage controller location." },
            "No action needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
VolumeDegraded :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    VolumeDegraded(Args... args) :
        RedfishMessage(
            "StorageDevice.1.0.1.VolumeDegraded",
            "The storage controller has detected a degraded volume condition.",
            "This message shall be used to indicate a volume degraded condition was detected due to one or more drives not installed correctly, missing, or has failed.",
            "The volume '%1' attached to the storage controller located in '%2' is degraded.",
            "Warning",
            2,
            { "string", "string" },
            { "The identifier of the volume.", "The location of the storage controller." },
            { "The value of this argument shall be a string containing an identifier for the volume.  The identifier may contain `Id`, `Name`, `Identifiers`, `LogicalUnitNumber`, or other data used to uniquely identify the volume.", "The value of this argument shall be a string containing the storage controller location." },
            "Ensure all cables are properly and securely connected.  Replace failed drives.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
VolumeFailure :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    VolumeFailure(Args... args) :
        RedfishMessage(
            "StorageDevice.1.0.1.VolumeFailure",
            "The storage controller has detected a failed volume condition.",
            "This message shall be used to indicate a volume failure condition was detected due to one or more drives not installed correctly, missing, or has failed.",
            "The volume '%1' attached to the storage controller located in '%2' has failed.",
            "Critical",
            2,
            { "string", "string" },
            { "The identifier of the volume.", "The location of the storage controller." },
            { "The value of this argument shall be a string containing an identifier for the volume.  The identifier may contain `Id`, `Name`, `Identifiers`, `LogicalUnitNumber`, or other data used to uniquely identify the volume.", "The value of this argument shall be a string containing the storage controller location." },
            "Ensure all cables are properly and securely connected.  Ensure all drives are fully seated and operational.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
WriteCacheProtected :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    WriteCacheProtected(Args... args) :
        RedfishMessage(
            "StorageDevice.1.0.1.WriteCacheProtected",
            "A storage controller write cache state is in protected mode.",
            "This message shall be used to indicate a controller write cache state is in protected mode.",
            "The write cache state on the storage controller located in '%1' is in protected mode.",
            "OK",
            1,
            { "string" },
            { "The location of the storage controller." },
            { "The value of this argument shall be a string containing the storage controller location." },
            "No action needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
WriteCacheTemporarilyDegraded :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    WriteCacheTemporarilyDegraded(Args... args) :
        RedfishMessage(
            "StorageDevice.1.0.1.WriteCacheTemporarilyDegraded",
            "The write cache state is temporarily degraded.",
            "This message shall be used to indicate write cache state temporarily degraded due to battery charging or data recovery rebuild operation pending.",
            "The write cache state on the storage controller located in '%1' is temporarily degraded.",
            "Warning",
            1,
            { "string" },
            { "The location of the storage controller." },
            { "The value of this argument shall be a string containing the storage controller location." },
            "Check the controller to determine the cause of write cache temporarily degraded state, such as a battery is charging or a data recovery rebuild operation is pending.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
WriteCacheDegraded :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    WriteCacheDegraded(Args... args) :
        RedfishMessage(
            "StorageDevice.1.0.1.WriteCacheDegraded",
            "The write cache state is degraded.",
            "This message shall be used to indicate the write cache state degraded due to a missing battery or hardware failure.",
            "The write cache state on the storage controller located in '%1' is degraded.",
            "Critical",
            1,
            { "string" },
            { "The location of the storage controller." },
            { "The value of this argument shall be a string containing the storage controller location." },
            "Check the controller to determine the cause of write cache degraded state, such as a missing battery or hardware failure.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
WriteCacheDataLoss :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    WriteCacheDataLoss(Args... args) :
        RedfishMessage(
            "StorageDevice.1.0.1.WriteCacheDataLoss",
            "The write cache is reporting loss of data in posted-writes memory.",
            "This message shall be used to indicate the write cache is reporting loss of data in posted-writes cache memory due to loss of battery power when the server lost power or a hardware error.",
            "The write cache on the storage controller located in '%1' has data loss.",
            "Critical",
            1,
            { "string" },
            { "The location of the storage controller." },
            { "The value of this argument shall be a string containing the storage controller location." },
            "Check the controller resource properties to determine cause of the write cache data loss.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace StorageDevice_1_0_1
}; // namespace registries
}; // namespace redfish
