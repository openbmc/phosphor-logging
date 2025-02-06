#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace NetworkDevice_1_0_1
{

struct [[deprecated ("Use NetworkDevice_1_1_0")]]
CableInserted :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    CableInserted(Args... args) :
        RedfishMessage(
            "NetworkDevice.1.0.1.CableInserted",
            "Indicates that a network cable has been inserted.",
            "This message shall be used to indicate that a network cable has been inserted.  This message shall not be sent for a backplane connection.",
            "A network cable has been inserted into network adapter '%1' port '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the network adapter.", "The `Id` of the network port." },
            { "The value of this argument shall be a string containing the value of the `Id` property of network adapter into which a cable has been plugged.", "The value of this argument shall be a string containing the value of the `Id` property of network port into which a cable has been plugged." },
            "Refresh your cached version of the network port to get the updated information from the service.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use NetworkDevice_1_1_0")]]
CableRemoved :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    CableRemoved(Args... args) :
        RedfishMessage(
            "NetworkDevice.1.0.1.CableRemoved",
            "Indicates that a network cable has been removed.",
            "This message shall be used to indicate that a network cable has been removed.  This message shall not be sent for a backplane connection.",
            "A cable has been removed from network adapter '%1' port '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the network adapter.", "The `Id` of the network port." },
            { "The value of this argument shall be a string containing the value of the `Id` property of network adapter from which a cable has been unplugged.", "The value of this argument shall be a string containing the value of the `Id` property of network port from which a cable has been unplugged." },
            "Refresh your cached version of the network port to get the updated information from the service.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use NetworkDevice_1_1_0")]]
ConnectionDropped :
    public RedfishMessage<std::string, std::string, std::string>
{
    template <typename... Args>
    ConnectionDropped(Args... args) :
        RedfishMessage(
            "NetworkDevice.1.0.1.ConnectionDropped",
            "Indicates that a network connection has been dropped.",
            "This message shall be used to indicate that the link for a network connection is no longer intact.",
            "The connection is no longer active for network adapter '%1' port '%2' function '%3'.",
            "OK",
            3,
            { "string", "string", "string" },
            { "The `Id` of the network adapter.", "The `Id` of the network port.", "The `Id` of the network function." },
            { "The value of this argument shall be a string containing the value of the `Id` property of network adapter from which a connection has dropped.", "The value of this argument shall be a string containing the value of the `Id` property of network port from which a connection has dropped.", "The value of this argument shall be a string containing the value of the `Id` property of network device function from which a connection has dropped." },
            "Refresh your cached version of the network port to get the updated information from the service.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use NetworkDevice_1_1_0")]]
ConnectionEstablished :
    public RedfishMessage<std::string, std::string, std::string>
{
    template <typename... Args>
    ConnectionEstablished(Args... args) :
        RedfishMessage(
            "NetworkDevice.1.0.1.ConnectionEstablished",
            "Indicates that a network connection has been established.",
            "This message shall be used to indicate that the link for a network connection is newly established.",
            "A network connection has been established for network adapter '%1' port '%2' function '%3'.",
            "OK",
            3,
            { "string", "string", "string" },
            { "The `Id` of the network adapter.", "The `Id` of the network port.", "The `Id` of the network function." },
            { "The value of this argument shall be a string containing the value of the `Id` property of network adapter to which a connection has been established.", "The value of this argument shall be a string containing the value of the `Id` property of network port to which a connection has been established.", "The value of this argument shall be a string containing the value of the `Id` property of network device function to which a connection has been established." },
            "Refresh your cached version of the network port to get the updated information from the service.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use NetworkDevice_1_1_0")]]
DegradedConnectionEstablished :
    public RedfishMessage<std::string, std::string, std::string>
{
    template <typename... Args>
    DegradedConnectionEstablished(Args... args) :
        RedfishMessage(
            "NetworkDevice.1.0.1.DegradedConnectionEstablished",
            "Indicates that a network connection has been established, but at an unexpectedly low link speed.",
            "This message shall be used to indicate that a network connection is newly established, but that the link speed is lower than provisioned for the channel.",
            "A degraded network connection has been established for network adapter '%1' port '%2' function '%3'.",
            "Warning",
            3,
            { "string", "string", "string" },
            { "The `Id` of the network adapter.", "The `Id` of the network port.", "The `Id` of the network function." },
            { "The value of this argument shall be a string containing the value of the `Id` property of network adapter to which a connection has been established.", "The value of this argument shall be a string containing the value of the `Id` property of network port to which a connection has been established.", "The value of this argument shall be a string containing the value of the `Id` property of network device function to which a connection has been established." },
            "Refresh your cached version of the network port to get the updated information from the service.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use NetworkDevice_1_1_0")]]
LinkFlapDetected :
    public RedfishMessage<std::string, std::string, std::string, double, double>
{
    template <typename... Args>
    LinkFlapDetected(Args... args) :
        RedfishMessage(
            "NetworkDevice.1.0.1.LinkFlapDetected",
            "Indicates that a network connection is highly unstable.",
            "This message shall be used to indicate that a network connection has repeatedly been established and dropped.  This message shall not be repeated more than once in any 24 hour period.",
            "The network connection for network adapter '%1' port '%2' function '%3' has been established and dropped '%4' times in the last '%5' minutes.",
            "Warning",
            5,
            { "string", "string", "string", "number", "number" },
            { "The `Id` of the network adapter.", "The `Id` of the network port.", "The `Id` of the network function.", "The number of times the link has flapped.", "The number of minutes over which the link has flapped." },
            { "The value of this argument shall be a string containing the value of the `Id` property of network adapter to which a connection has been established.", "The value of this argument shall be a string containing the value of the `Id` property of network port to which a connection has been established.", "The value of this argument shall be a string containing the value of the `Id` property of network device function to which a connection has been established.", "The value of this argument shall be a number representing the count of link establishment/disconnection cycles.", "The value of this argument shall be a number representing the number of minutes over which link flapping activity has been detected." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace NetworkDevice_1_0_1
}; // namespace registries
}; // namespace redfish
