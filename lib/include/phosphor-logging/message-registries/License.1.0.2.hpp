#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace License_1_0_2
{

struct [[deprecated ("Use License_1_0_3")]]
LicenseInstalled :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    LicenseInstalled(Args... args) :
        RedfishMessage(
            "License.1.0.2.LicenseInstalled",
            "Indicates that a license has been installed.",
            "This message shall be used to indicate that a license has been installed.  Existing licenses may have been updated or otherwise affected by the installation.",
            "The license '%1' has been installed.",
            "OK",
            1,
            { "string" },
            { "The name of the license." },
            { "This argument shall contain the human readable identifier of the license." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use License_1_0_3")]]
DaysBeforeExpiration :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    DaysBeforeExpiration(Args... args) :
        RedfishMessage(
            "License.1.0.2.DaysBeforeExpiration",
            "Indicates the number of days remaining on a license before expiration.",
            "This message shall be used to indicate the number of days remaining on a license before expiration.",
            "The license '%1' will expire in %2 days.",
            "OK",
            2,
            { "string", "number" },
            { "The name of the license.", "The number of days before the license expires." },
            { "This argument shall contain the human readable identifier of the license.", "This argument shall contain the number of days before the license expires." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use License_1_0_3")]]
GracePeriod :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    GracePeriod(Args... args) :
        RedfishMessage(
            "License.1.0.2.GracePeriod",
            "Indicates that a license has expired and entered its grace period.",
            "This message shall be used to indicate that a license has expired and entered its grace period.",
            "The license '%1' has expired, %2 day grace period before licensed functionality is disabled.",
            "Warning",
            2,
            { "string", "number" },
            { "The name of the license.", "The number day of grace period." },
            { "This argument shall contain the human readable identifier of the license.", "This argument shall contain the number day of grace period for the licensed functionality." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use License_1_0_3")]]
Expired :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    Expired(Args... args) :
        RedfishMessage(
            "License.1.0.2.Expired",
            "Indicates that a license has expired and its functionality has been disabled.",
            "This message shall be used to indicate that a license has expired and its functionality has been disabled.",
            "The license '%1' has expired.",
            "Warning",
            1,
            { "string" },
            { "The name of the license." },
            { "This argument shall contain the human readable identifier of the license." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use License_1_0_3")]]
InstallFailed :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    InstallFailed(Args... args) :
        RedfishMessage(
            "License.1.0.2.InstallFailed",
            "Indicates that the service failed to install the license.",
            "This message shall be used to indicate that the service failed to install the license.",
            "Failed to install the license.  Reason: %1.",
            "Critical",
            1,
            { "string" },
            { "The reason for the license installation failure." },
            { "This argument shall contain the reason for the license installation failure." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use License_1_0_3")]]
InvalidLicense :
    public RedfishMessage<>
{
    template <typename... Args>
    InvalidLicense(Args... args) :
        RedfishMessage(
            "License.1.0.2.InvalidLicense",
            "Indicates that the license was not recognized, is corrupted, or is invalid.",
            "This message shall be used to indicate that the content of the license was not recognized, is corrupted, or is invalid.",
            "The content of the license was not recognized, is corrupted, or is invalid.",
            "Critical",
            0,
            {},
            {},
            {},
            "Verify the license content is correct and resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use License_1_0_3")]]
NotApplicableToTarget :
    public RedfishMessage<>
{
    template <typename... Args>
    NotApplicableToTarget(Args... args) :
        RedfishMessage(
            "License.1.0.2.NotApplicableToTarget",
            "Indicates that the license is not applicable to the target.",
            "This message shall be used to indicate that the license is not applicable to the target.",
            "The license is not applicable to the target.",
            "Critical",
            0,
            {},
            {},
            {},
            "Check the license compatibility or applicability to the specified target.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use License_1_0_3")]]
TargetsRequired :
    public RedfishMessage<>
{
    template <typename... Args>
    TargetsRequired(Args... args) :
        RedfishMessage(
            "License.1.0.2.TargetsRequired",
            "Indicates that one or more targets need to be specified with the license.",
            "This message shall be used to indicate that one or more targets need to be specified with the license.",
            "The license requires targets to be specified.",
            "Critical",
            0,
            {},
            {},
            {},
            "Add `AuthorizedDevices` to `Links` and resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace License_1_0_2
}; // namespace registries
}; // namespace redfish
