#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace AccountSecurity_1_0_0
{

struct InsufficientPrivilege :
    public RedfishMessage<std::string, std::string, std::string, std::string>
{
    template <typename... Args>
    InsufficientPrivilege(Args... args) :
        RedfishMessage(
            "AccountSecurity.1.0.0.InsufficientPrivilege",
            "Indicates that a user with valid credentials attempted an operation without the necessary privileges.",
            "This message shall indicate that a user with valid credentials attempted an operation without the necessary privileges.  Services shall not include this message in responses to clients.",
            "'%1' attempted an operation over '%2' with the privleges '%3', but requires the privileges '%4'.",
            "Critical",
            4,
            { "string", "string", "string", "string" },
            { "The IP address of the user that attempted the operation.", "The protocol or interface over which the operation was attempted.  Examples include 'Redfish', 'Web UI'. and 'SSH'.", "The privileges of the user that attempted the operation.", "The required privileges for the requested operation." },
            { "This argument shall contain the IP address of the user that attempted the operation.", "This argument shall contain the protocol or interface over which the operation was attempted.", "This argument shall contain the privileges of the user that attempted the operation.", "This argument shall contain the required privileges for the requested operation." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct InvalidCredentials :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    InvalidCredentials(Args... args) :
        RedfishMessage(
            "AccountSecurity.1.0.0.InvalidCredentials",
            "Indicates that a user provided invalid credentials with a request.",
            "This message shall indicate that a user provided invalid credentials with a request, such as those found in the `Authorization` and `X-Auth-Token` HTTP request headers.  Services shall not include this message in responses to clients.",
            "'%1' provided invalid credentials over '%2'.",
            "Critical",
            2,
            { "string", "string" },
            { "The IP address of the user that attempted the operation.", "The protocol or interface over which the operation was attempted.  Examples include 'Redfish', 'Web UI'. and 'SSH'." },
            { "This argument shall contain the IP address of the user that attempted the operation.", "This argument shall contain the protocol or interface over which the operation was attempted." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct SuccessfulLogin :
    public RedfishMessage<std::string, std::string, std::string>
{
    template <typename... Args>
    SuccessfulLogin(Args... args) :
        RedfishMessage(
            "AccountSecurity.1.0.0.SuccessfulLogin",
            "Indicates that a user provided valid credentials and made a successful login.",
            "This message shall indicate that a user provided valid credentials and made a successful login.  Services shall not include this message in responses to clients.",
            "Successful login of user '%1' from '%2' over '%3'.",
            "OK",
            3,
            { "string", "string", "string" },
            { "The username of the account that made a successful login.", "The IP address of the user that made a successful login.", "The protocol or interface over which the login was made.  Examples include 'Redfish', 'Web UI'. and 'SSH'." },
            { "This argument shall contain the username of the account that made a successful login.", "This argument shall contain the IP address of the user that made a successful login.", "This argument shall contain the protocol or interface over which the login was made." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct AccountCreated :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    AccountCreated(Args... args) :
        RedfishMessage(
            "AccountSecurity.1.0.0.AccountCreated",
            "Indicates that an account was created.",
            "This message shall indicate that an account was created.  Services shall not include this message in responses to clients.",
            "Account '%1' was created.",
            "OK",
            1,
            { "string" },
            { "The username of the account that was created." },
            { "This argument shall contain the username of the account that was created." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct AccountRemoved :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    AccountRemoved(Args... args) :
        RedfishMessage(
            "AccountSecurity.1.0.0.AccountRemoved",
            "Indicates that an account was removed.",
            "This message shall indicate that an account was removed.  Services shall not include this message in responses to clients.",
            "Account '%1' was removed.",
            "OK",
            1,
            { "string" },
            { "The username of the account that was removed." },
            { "This argument shall contain the username of the account that was removed." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct AccountLocked :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    AccountLocked(Args... args) :
        RedfishMessage(
            "AccountSecurity.1.0.0.AccountLocked",
            "Indicates that an account was locked due to failed authorization attempts.",
            "This message shall indicate that an account was locked due to failed authorization attempts.  Services shall not include this message in responses to clients.",
            "Account '%1' was locked due to excessive failed authorization attempts.",
            "Critical",
            1,
            { "string" },
            { "The username of the account that was locked." },
            { "This argument shall contain the username of the account that was locked." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct AccountLockoutExpired :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    AccountLockoutExpired(Args... args) :
        RedfishMessage(
            "AccountSecurity.1.0.0.AccountLockoutExpired",
            "Indicates that the lockout timer on an account expired and the account is now unlocked.",
            "This message shall indicate that the lockout timer on an account expired and the account is now unlocked.  Services shall not include this message in responses to clients.",
            "Account '%1' was unlocked following the expiration of a lockout timer.",
            "OK",
            1,
            { "string" },
            { "The username of the account that was unlocked." },
            { "This argument shall contain the username of the account that was unlocked." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct AccountUnlocked :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    AccountUnlocked(Args... args) :
        RedfishMessage(
            "AccountSecurity.1.0.0.AccountUnlocked",
            "Indicates that an account was manually unlocked.",
            "This message shall indicate that an account was manually unlocked.  Services shall not include this message in responses to clients.",
            "Account '%1' was unlocked by an administrator.",
            "OK",
            1,
            { "string" },
            { "The username of the account that was unlocked." },
            { "This argument shall contain the username of the account that was unlocked." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct AccountEnabled :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    AccountEnabled(Args... args) :
        RedfishMessage(
            "AccountSecurity.1.0.0.AccountEnabled",
            "Indicates that an account was enabled.",
            "This message shall indicate that an account was enabled.  Services shall not include this message in responses to clients.",
            "Account '%1' was enabled.",
            "OK",
            1,
            { "string" },
            { "The username of the account that was enabled." },
            { "This argument shall contain the username of the account that was enabled." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct AccountDisabled :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    AccountDisabled(Args... args) :
        RedfishMessage(
            "AccountSecurity.1.0.0.AccountDisabled",
            "Indicates that an account was disabled.",
            "This message shall indicate that an account was disabled.  Services shall not include this message in responses to clients.",
            "Account '%1' was disabled.",
            "Warning",
            1,
            { "string" },
            { "The username of the account that was disabled." },
            { "This argument shall contain the username of the account that was disabled." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PasswordModified :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PasswordModified(Args... args) :
        RedfishMessage(
            "AccountSecurity.1.0.0.PasswordModified",
            "Indicates that the password of an account was changed.",
            "This message shall indicate that the password of an account was changed.  Services shall not include this message in responses to clients.",
            "The password for account '%1' was changed.",
            "OK",
            1,
            { "string" },
            { "The username of the account with the modified password." },
            { "This argument shall contain the username of the account with the modified password." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ManagerAccountRoleChanged :
    public RedfishMessage<std::string, std::string, std::string>
{
    template <typename... Args>
    ManagerAccountRoleChanged(Args... args) :
        RedfishMessage(
            "AccountSecurity.1.0.0.ManagerAccountRoleChanged",
            "Indicates that the role for an account was changed.",
            "This message shall indicate that the role for an account was changed.  Services shall not include this message in responses to clients.",
            "Account '%1' has changed from role '%2' to '%3'.",
            "OK",
            3,
            { "string", "string", "string" },
            { "The username of the account that was modified.", "The former role for the account.", "The new role for the account" },
            { "This argument shall contain the username of the account that was modified.", "This argument shall contain the former Redfish role for the account.", "This argument shall contain the new Redfish role for the account." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct RolePrivilegeRemoved :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    RolePrivilegeRemoved(Args... args) :
        RedfishMessage(
            "AccountSecurity.1.0.0.RolePrivilegeRemoved",
            "Indicates that a privilege was removed from a role.",
            "This message shall indicate that a privilege was removed from a role.  Services shall not include this message in responses to clients.",
            "Privilege '%1' was removed from role '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The privilege that was removed from the role.", "The role that was modified." },
            { "This argument shall contain the privilege that was removed from the role.", "This argument shall contain the Redfish role that was modified." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct RolePrivilegeAdded :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    RolePrivilegeAdded(Args... args) :
        RedfishMessage(
            "AccountSecurity.1.0.0.RolePrivilegeAdded",
            "Indicates that a privilege was added to a role.",
            "This message shall indicate that a privilege was added to a role.  Services shall not include this message in responses to clients.",
            "Privilege '%1' was added to role '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The privilege that was added to the role.", "The role that was modified." },
            { "This argument shall contain the privilege that was added too the role.", "This argument shall contain the Redfish role that was modified." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct UserCertificateAdded :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    UserCertificateAdded(Args... args) :
        RedfishMessage(
            "AccountSecurity.1.0.0.UserCertificateAdded",
            "Indicates that a user certificate was added to an account.",
            "This message shall indicate that a user certificate was added to an account.  Services shall not include this message in responses to clients.",
            "A user certificate was added to account '%1'.",
            "OK",
            1,
            { "string" },
            { "The username of the account with the certificate that was addedd." },
            { "This argument shall contain the username of the account with the certificate that was added." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct UserCertificateRemoved :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    UserCertificateRemoved(Args... args) :
        RedfishMessage(
            "AccountSecurity.1.0.0.UserCertificateRemoved",
            "Indicates that a user certificate was removed from an account.",
            "This message shall indicate that a user certificate was removed from an account.  Services shall not include this message in responses to clients.",
            "A user certificate was removed from account '%1'.",
            "OK",
            1,
            { "string" },
            { "The username of the account with the certificate that was removed." },
            { "This argument shall contain the username of the account with the certificate that was removed." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct UserCertificateReplaced :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    UserCertificateReplaced(Args... args) :
        RedfishMessage(
            "AccountSecurity.1.0.0.UserCertificateReplaced",
            "Indicates that a user certificate was replaced on an account.",
            "This message shall indicate that a user certificate was replaced on an account.  Services shall not include this message in responses to clients.",
            "A user certificate was replaced on account '%1'.",
            "OK",
            1,
            { "string" },
            { "The username of the account with the certificate that was replaced." },
            { "This argument shall contain the username of the account with the certificate that was replaced." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct UserKeyAdded :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    UserKeyAdded(Args... args) :
        RedfishMessage(
            "AccountSecurity.1.0.0.UserKeyAdded",
            "Indicates that a user key was added to an account.",
            "This message shall indicate that a user key was added to an account.  Services shall not include this message in responses to clients.",
            "A user key was added to account '%1'.",
            "OK",
            1,
            { "string" },
            { "The username of the account with the key that was added." },
            { "This argument shall contain the username of the account with the key that was added." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct UserKeyRemoved :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    UserKeyRemoved(Args... args) :
        RedfishMessage(
            "AccountSecurity.1.0.0.UserKeyRemoved",
            "Indicates that a user key was removed from an account.",
            "This message shall indicate that a user key was removed from an account.  Services shall not include this message in responses to clients.",
            "A user key was removed from account '%1'.",
            "OK",
            1,
            { "string" },
            { "The username of the account with the key that was removed." },
            { "This argument shall contain the username of the account with the key that was removed." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace AccountSecurity_1_0_0
}; // namespace registries
}; // namespace redfish
