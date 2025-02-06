#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace StorageDevice_1_3_0
{

struct [[deprecated ("Use StorageDevice_1_4_0")]]
ControllerOK :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ControllerOK(Args... args) :
        RedfishMessage(
            "StorageDevice.1.3.0.ControllerOK",
            "The storage controller health has changed to OK.",
            "This message shall indicate that the storage controller health has changed to OK.",
            "The health of the storage controller located in '%1' has changed to OK.",
            "OK",
            1,
            { "string" },
            { "The location of the storage controller." },
            { "This argument shall contain the storage controller location." },
            "None.",
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
            "StorageDevice.1.3.0.ControllerFailure",
            "A storage controller failure was detected.",
            "This message shall indicate that a storage controller failure condition was detected due to a hardware, communication, or firmware failure.",
            "A failure condition for the storage controller located in '%1' was detected.",
            "Critical",
            1,
            { "string" },
            { "The location of the storage controller." },
            { "This argument shall contain the storage controller location." },
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
            "StorageDevice.1.3.0.DriveOK",
            "The health of a drive has changed to OK.",
            "This message shall indicate a drive health has changed to OK.",
            "The health of the drive located in '%1' has changed to OK.",
            "OK",
            1,
            { "string" },
            { "The location of the drive." },
            { "This argument shall contain the drive location." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
DrivePredictedMediaLifeLow :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    DrivePredictedMediaLifeLow(Args... args) :
        RedfishMessage(
            "StorageDevice.1.3.0.DrivePredictedMediaLifeLow",
            "The drive media predicted life left is low.",
            "This message shall indicate a drive media predicted life left level is below a user or vendor-defined threshold.",
            "The drive located at `%1` has a predicted media life left of %2 percent and is below the specified threhsold of %3 percent.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The location of the drive.", "The percent of predicted media life left.", "The alert threshold." },
            { "This argument shall contain the drive location.", "This argument shall contain the value of PredictedMediaLifeLeft for this drive.", "This argument shall contain the value of the threshold that triggered this message." },
            "Replace the drive before the predicted media life left reaches 0%.",
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
            "StorageDevice.1.3.0.DrivePredictiveFailure",
            "A predictive drive failure condition was detected.",
            "This message shall indicate a drive predictive failure condition was detected.",
            "A predictive failure condition for the drive located in '%1' was detected.",
            "Warning",
            1,
            { "string" },
            { "The location of the drive." },
            { "This argument shall contain the drive location." },
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
            "StorageDevice.1.3.0.DrivePredictiveFailureCleared",
            "A previously detected predictive failure condition on a drive was cleared.",
            "This message shall indicate a predictive failure condition on a drive was cleared, but any other failure conditions remain.",
            "A predictive failure condition for the drive located in '%1' was cleared.",
            "OK",
            1,
            { "string" },
            { "The location of the drive." },
            { "This argument shall contain the drive location." },
            "None.",
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
            "StorageDevice.1.3.0.DriveFailure",
            "A drive failure condition was detected.",
            "This message shall indicate a drive failure condition was detected due to a drive not installed correctly or failed.",
            "A failure condition for the drive located in '%1' was detected.",
            "Critical",
            1,
            { "string" },
            { "The location of the drive." },
            { "This argument shall contain the drive location." },
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
            "StorageDevice.1.3.0.DriveFailureCleared",
            "A previously detected failure condition on a drive was cleared.",
            "This message shall indicate a failure condition on a drive was cleared, but any predictive failure or other conditions remain.",
            "A failure condition for the drive located in '%1' was cleared.",
            "OK",
            1,
            { "string" },
            { "The location of the drive." },
            { "This argument shall contain the drive location." },
            "None.",
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
            "StorageDevice.1.3.0.DriveInserted",
            "A drive was inserted.",
            "This message shall indicate a drive was inserted.",
            "The drive located in '%1' was inserted.",
            "OK",
            1,
            { "string" },
            { "The location of the drive." },
            { "This argument shall contain the drive location." },
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
            "StorageDevice.1.3.0.DriveRemoved",
            "A drive was removed.",
            "This message shall indicate a drive was removed.",
            "The drive located in '%1' was removed.",
            "Critical",
            1,
            { "string" },
            { "The location of the drive." },
            { "This argument shall contain the drive location." },
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
            "StorageDevice.1.3.0.VolumeOK",
            "A volume health has changed to OK.",
            "This message shall indicate a volume health has changed to OK.",
            "The health of volume '%1' that is attached to the storage controller located in '%2' has changed to OK.",
            "OK",
            2,
            { "string", "string" },
            { "The identifier of the volume.", "The location of the storage controller." },
            { "This argument shall contain an identifier for the volume.  The identifier may contain `Id`, `Name`, `Identifiers`, `LogicalUnitNumber`, or other data used to uniquely identify the volume.", "This argument shall contain the storage controller location." },
            "None.",
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
            "StorageDevice.1.3.0.VolumeDegraded",
            "The storage controller has detected a degraded volume condition.",
            "This message shall indicate a volume degraded condition was detected due to one or more drives not installed correctly, missing, or failed.",
            "The volume '%1' attached to the storage controller located in '%2' is degraded.",
            "Warning",
            2,
            { "string", "string" },
            { "The identifier of the volume.", "The location of the storage controller." },
            { "This argument shall contain an identifier for the volume.  The identifier may contain `Id`, `Name`, `Identifiers`, `LogicalUnitNumber`, or other data used to uniquely identify the volume.", "This argument shall contain the storage controller location." },
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
            "StorageDevice.1.3.0.VolumeFailure",
            "The storage controller has detected a failed volume condition.",
            "This message shall indicate a volume failure condition was detected due to one or more drives not installed correctly, missing, or failed.",
            "The volume '%1' attached to the storage controller located in '%2' has failed.",
            "Critical",
            2,
            { "string", "string" },
            { "The identifier of the volume.", "The location of the storage controller." },
            { "This argument shall contain an identifier for the volume.  The identifier may contain `Id`, `Name`, `Identifiers`, `LogicalUnitNumber`, or other data used to uniquely identify the volume.", "This argument shall contain the storage controller location." },
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
            "StorageDevice.1.3.0.WriteCacheProtected",
            "A storage controller write cache state is in protected mode.",
            "This message shall indicate a controller write cache state is in protected mode.",
            "The write cache state on the storage controller located in '%1' is in protected mode.",
            "OK",
            1,
            { "string" },
            { "The location of the storage controller." },
            { "This argument shall contain the storage controller location." },
            "None.",
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
            "StorageDevice.1.3.0.WriteCacheTemporarilyDegraded",
            "The write cache state is temporarily degraded.",
            "This message shall indicate write cache state temporarily degraded due to battery charging or data recovery rebuild operation pending.",
            "The write cache state on the storage controller located in '%1' is temporarily degraded.",
            "Warning",
            1,
            { "string" },
            { "The location of the storage controller." },
            { "This argument shall contain the storage controller location." },
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
            "StorageDevice.1.3.0.WriteCacheDegraded",
            "The write cache state is degraded.",
            "This message shall indicate the write cache state degraded due to a missing battery or hardware failure.",
            "The write cache state on the storage controller located in '%1' is degraded.",
            "Critical",
            1,
            { "string" },
            { "The location of the storage controller." },
            { "This argument shall contain the storage controller location." },
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
            "StorageDevice.1.3.0.WriteCacheDataLoss",
            "The write cache is reporting loss of data in posted-writes memory.",
            "This message shall indicate the write cache is reporting loss of data in posted-writes cache memory due to loss of battery power when the server lost power or a hardware error.",
            "The write cache on the storage controller located in '%1' has data loss.",
            "Critical",
            1,
            { "string" },
            { "The location of the storage controller." },
            { "This argument shall contain the storage controller location." },
            "Check the controller resource properties to determine the cause of the write cache data loss.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
BatteryOK :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    BatteryOK(Args... args) :
        RedfishMessage(
            "StorageDevice.1.3.0.BatteryOK",
            "The health of a battery has changed to OK.",
            "This message shall indicate a battery health has changed to OK.",
            "The health of the battery located in '%1' has changed to OK.",
            "OK",
            1,
            { "string" },
            { "The location of the battery." },
            { "This argument shall contain the battery location." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
BatteryCharging :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    BatteryCharging(Args... args) :
        RedfishMessage(
            "StorageDevice.1.3.0.BatteryCharging",
            "A battery charging condition was detected.",
            "This message shall indicate a battery is below the required capacity and is charging.",
            "A charging condition for the battery located in '%1' was detected.",
            "Warning",
            1,
            { "string" },
            { "The location of the battery." },
            { "This argument shall contain the battery location." },
            "None.  There may be reduced performance and features while the battery is charging.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
BatteryMissing :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    BatteryMissing(Args... args) :
        RedfishMessage(
            "StorageDevice.1.3.0.BatteryMissing",
            "A battery missing condition was detected.",
            "This message shall indicate a battery that is required by the configuration is missing.",
            "The battery located in '%1' is missing.",
            "Critical",
            1,
            { "string" },
            { "The location of the battery." },
            { "This argument shall contain the battery location." },
            "Attach the battery.  Ensure all cables are properly and securely connected.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
BatteryFailure :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    BatteryFailure(Args... args) :
        RedfishMessage(
            "StorageDevice.1.3.0.BatteryFailure",
            "A battery failure condition was detected.",
            "This message shall indicate a battery failure condition was detected.",
            "A failure condition for the battery located in '%1' was detected.",
            "Critical",
            1,
            { "string" },
            { "The location of the battery." },
            { "This argument shall contain the battery location." },
            "Ensure all cables are properly and securely connected.  Replace the failed battery.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
ControllerDegraded :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ControllerDegraded(Args... args) :
        RedfishMessage(
            "StorageDevice.1.3.0.ControllerDegraded",
            "A storage controller degraded condition was detected.",
            "This message shall indicate that a storage controller degraded condition was detected.",
            "A degraded condition for the storage controller located in '%1' was detected due to reason '%2'.",
            "Warning",
            2,
            { "string", "string" },
            { "The location of the storage controller.", "The reason for the degraded controller." },
            { "This argument shall contain the storage controller location.", "This argument shall contain the reason for the degraded controller." },
            "Reseat the storage controller in the PCI slot.  Update the controller to the latest firmware version.  If the issue persists, replace the controller.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
ControllerPreviousError :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ControllerPreviousError(Args... args) :
        RedfishMessage(
            "StorageDevice.1.3.0.ControllerPreviousError",
            "A storage controller error was detected prior to reboot.",
            "This message shall indicate that an unreported storage controller error condition was detected prior to reboot.",
            "A previous error condition for the storage controller located in '%1' was detected due to '%2'.",
            "Warning",
            2,
            { "string", "string" },
            { "The location of the storage controller.", "The reason for error." },
            { "This argument shall contain the storage controller location.", "This argument shall contain the reason for failure." },
            "Update the controller to the latest firmware version.  If the issue persists, replace the controller.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
ControllerPasswordAccepted :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ControllerPasswordAccepted(Args... args) :
        RedfishMessage(
            "StorageDevice.1.3.0.ControllerPasswordAccepted",
            "The storage controller password was entered.",
            "This message shall indicate that the storage controller password was accepted.  Normally this is used by storage controllers that require a controller password during boot to unlock encrypted volumes or drives.",
            "A password was entered for the storage controller located in '%1'.",
            "OK",
            1,
            { "string" },
            { "The location of the storage controller." },
            { "This argument shall contain the storage controller location." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
ControllerPasswordRequired :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ControllerPasswordRequired(Args... args) :
        RedfishMessage(
            "StorageDevice.1.3.0.ControllerPasswordRequired",
            "The storage controller requires a password.",
            "This message shall indicate that the storage controller requires a password.  Normally this is used by storage controllers that require a controller password during boot to unlock encrypted volumes or drives.",
            "The storage controller located in '%1' requires a password.",
            "Critical",
            1,
            { "string" },
            { "The location of the storage controller." },
            { "This argument shall contain the storage controller location." },
            "Enter the controller password.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
ControllerPortOK :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ControllerPortOK(Args... args) :
        RedfishMessage(
            "StorageDevice.1.3.0.ControllerPortOK",
            "The health of a controller port has changed to OK.",
            "This message shall indicate a controller port health has changed to OK.",
            "The health of the controller port located in '%1' has changed to OK.",
            "OK",
            1,
            { "string" },
            { "The location of the controller port." },
            { "This argument shall contain the controller port location." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
ControllerPortDegraded :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ControllerPortDegraded(Args... args) :
        RedfishMessage(
            "StorageDevice.1.3.0.ControllerPortDegraded",
            "A controller port degraded condition was detected.",
            "This message shall indicate a controller port degraded condition was detected.",
            "A degraded condition for the controller port located in '%1' was detected due to reason '%2'.",
            "Warning",
            2,
            { "string", "string" },
            { "The location of the controller port.", "The reason for the degraded controller port." },
            { "This argument shall contain the controller port location.", "This argument shall contain the reason for the degraded controller port." },
            "Ensure all cables are properly and securely connected.  Replace faulty cables.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
ControllerPortFailure :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ControllerPortFailure(Args... args) :
        RedfishMessage(
            "StorageDevice.1.3.0.ControllerPortFailure",
            "A controller port failure condition was detected.",
            "This message shall indicate a controller port failure condition was detected.",
            "A failure condition for the controller port located in '%1' was detected.",
            "Critical",
            1,
            { "string" },
            { "The location of the controller port." },
            { "This argument shall contain the controller port location." },
            "Ensure all cables are properly and securely connected.  Replace faulty cables.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
DriveMissing :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    DriveMissing(Args... args) :
        RedfishMessage(
            "StorageDevice.1.3.0.DriveMissing",
            "A drive missing condition was detected.",
            "This message shall indicate a drive that is required by the configuration is missing.  This message shall exist any time a volume is degraded or failed due to a missing drive.",
            "A missing condition for the drive located in '%1' was detected.",
            "Critical",
            1,
            { "string" },
            { "The location of the drive." },
            { "This argument shall contain the drive location." },
            "Ensure all cables are properly and securely connected.  Ensure all drives are fully seated.  Replace the defective cables, drive, or both.  Delete the volume if it is no longer needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
DriveMissingCleared :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    DriveMissingCleared(Args... args) :
        RedfishMessage(
            "StorageDevice.1.3.0.DriveMissingCleared",
            "A previous drive missing condition was cleared.",
            "This message shall indicate a previous drive missing condition was cleared.",
            "A missing condition for the drive located in '%1' was cleared.",
            "OK",
            1,
            { "string" },
            { "The location of the drive." },
            { "This argument shall contain the drive location." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
DriveOffline :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    DriveOffline(Args... args) :
        RedfishMessage(
            "StorageDevice.1.3.0.DriveOffline",
            "A drive offline condition was detected.",
            "This message shall indicate a drive offline condition was detected.  This may occur when a drive is unconfigured, encrypted by a foreign controller, offline during a background operation, or unsupported.",
            "An offline condition for the drive located in '%1' was detected.",
            "Critical",
            1,
            { "string" },
            { "The location of the drive." },
            { "This argument shall contain the drive location." },
            "If the drive is unconfigured or needs an import, configure the drive.  If the drive operation is in progress, wait for the operation to complete.  If the drive is not supported, replace it.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
DriveOfflineCleared :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    DriveOfflineCleared(Args... args) :
        RedfishMessage(
            "StorageDevice.1.3.0.DriveOfflineCleared",
            "A drive offline condition was cleared.",
            "This message shall indicate a drive offline condition was cleared.",
            "An offline condition for the drive located in '%1' was cleared.",
            "OK",
            1,
            { "string" },
            { "The location of the drive." },
            { "This argument shall contain the drive location." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
VolumeOffline :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    VolumeOffline(Args... args) :
        RedfishMessage(
            "StorageDevice.1.3.0.VolumeOffline",
            "The storage controller has detected an offline volume condition.",
            "This message shall indicate a volume offline condition was detected.  This may occur if the volume configuration is not supported or needs to be imported.",
            "The volume '%1' attached to the storage controller located in '%2' is offline.",
            "Critical",
            2,
            { "string", "string" },
            { "The identifier of the volume.", "The location of the storage controller." },
            { "This argument shall contain an identifier for the volume.  The identifier may contain `Id`, `Name`, `Identifiers`, `LogicalUnitNumber`, or other data used to uniquely identify the volume.", "This argument shall contain the storage controller location." },
            "Use storage software to enable, repair, or import the volume.  You may also delete or move volume back to the original controller.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
VolumeOffine :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    VolumeOffine(Args... args) :
        RedfishMessage(
            "StorageDevice.1.3.0.VolumeOffine",
            "The storage controller has detected an offline volume condition.",
            "This message shall indicate a volume offline condition was detected.  This may occur if the volume configuration is not supported or needs to be imported.",
            "The volume '%1' attached to the storage controller located in '%2' is offline.",
            "Critical",
            2,
            { "string", "string" },
            { "The identifier of the volume.", "The location of the storage controller." },
            { "This argument shall contain an identifier for the volume.  The identifier may contain `Id`, `Name`, `Identifiers`, `LogicalUnitNumber`, or other data used to uniquely identify the volume.", "This argument shall contain the storage controller location." },
            "Use storage software to enable, repair, or import the volume.  You may also delete or move volume back to the original controller.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use StorageDevice_1_4_0")]]
VolumeOfflineCleared :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    VolumeOfflineCleared(Args... args) :
        RedfishMessage(
            "StorageDevice.1.3.0.VolumeOfflineCleared",
            "The storage controller has detected an online volume condition.",
            "This message shall indicate a volume online condition was detected.",
            "The volume '%1' attached to the storage controller located in '%2' is online.",
            "OK",
            2,
            { "string", "string" },
            { "The identifier of the volume.", "The location of the storage controller." },
            { "This argument shall contain an identifier for the volume.  The identifier may contain `Id`, `Name`, `Identifiers`, `LogicalUnitNumber`, or other data used to uniquely identify the volume.", "This argument shall contain the storage controller location." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace StorageDevice_1_3_0
}; // namespace registries
}; // namespace redfish
