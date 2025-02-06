#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace NetworkDevice_1_0_3
{

struct [[deprecated ("Use NetworkDevice_1_1_0")]]
CableInserted :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    CableInserted(Args... args) :
        RedfishMessage(
            "NetworkDevice.1.0.3.CableInserted",
            "Indicates that a network cable was inserted.",
            "This message shall indicate that a network cable was inserted.  This message shall not be sent for a backplane connection.",
            "A network cable was inserted into network adapter '%1' port '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The name or identifier of the network adapter.", "The name or identifier of the network port." },
            { "This argument shall contain contain a string that identifies or describes the location or physical context of the network adapter into which a cable was plugged.", "This argument shall contain contain a string that identifies or describes the location or physical context of the network port into which a cable was plugged." },
            "None.",
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
            "NetworkDevice.1.0.3.CableRemoved",
            "Indicates that a network cable was removed.",
            "This message shall indicate that a network cable was removed.  This message shall not be sent for a backplane connection.",
            "A cable was removed from network adapter '%1' port '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The name or identifier of the network adapter.", "The name or identifier of the network port." },
            { "This argument shall contain contain a string that identifies or describes the location or physical context of the network adapter from which a cable was unplugged.", "This argument shall contain contain a string that identifies or describes the location or physical context of the network port from which a cable was unplugged." },
            "None.",
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
            "NetworkDevice.1.0.3.ConnectionDropped",
            "Indicates that a network connection was dropped.",
            "This message shall indicate that the link for a network connection is no longer intact.",
            "The connection is no longer active for network adapter '%1' port '%2' function '%3'.",
            "OK",
            3,
            { "string", "string", "string" },
            { "The name or identifier of network adapter.", "The name or identifier of network port.", "The name or identifier of network function." },
            { "This argument shall contain contain a string that identifies or describes the location or physical context of the network adapter from which a connection was dropped.", "This argument shall contain contain a string that identifies or describes the location or physical context of the network port from which a connection was dropped.", "This argument shall contain contain a string that identifies or describes the location or physical context of the network device function from which a connection was dropped." },
            "None.",
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
            "NetworkDevice.1.0.3.ConnectionEstablished",
            "Indicates that a network connection was established.",
            "This message shall indicate that the link for a network connection is newly established.",
            "A network connection was established for network adapter '%1' port '%2' function '%3'.",
            "OK",
            3,
            { "string", "string", "string" },
            { "The name or identifier of network adapter.", "The name or identifier of network port.", "The name or identifier of network function." },
            { "This argument shall contain contain a string that identifies or describes the location or physical context of the network adapter to which a connection was established.", "This argument shall contain contain a string that identifies or describes the location or physical context of the network port to which a connection was established.", "This argument shall contain contain a string that identifies or describes the location or physical context of the network device function to which a connection was established." },
            "None.",
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
            "NetworkDevice.1.0.3.DegradedConnectionEstablished",
            "Indicates that a network connection was established, but at an unexpectedly low link speed.",
            "This message shall indicate that a network connection is newly established but that the link speed is lower than provisioned for the channel.",
            "A degraded network connection was established for network adapter '%1' port '%2' function '%3'.",
            "Warning",
            3,
            { "string", "string", "string" },
            { "The name or identifier of network adapter.", "The name or identifier of network port.", "The name or identifier of network function." },
            { "This argument shall contain contain a string that identifies or describes the location or physical context of the network adapter to which a connection was established.", "This argument shall contain contain a string that identifies or describes the location or physical context of the network port to which a connection was established.", "This argument shall contain contain a string that identifies or describes the location or physical context of the network device function to which a connection was established." },
            "None.",
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
            "NetworkDevice.1.0.3.LinkFlapDetected",
            "Indicates that a network connection is highly unstable.",
            "This message shall indicate that a network connection was repeatedly established and dropped.  This message shall not be repeated more than once in any 24-hour period.",
            "The network connection for network adapter '%1' port '%2' function '%3' was established and dropped '%4' times in the last '%5' minutes.",
            "Warning",
            5,
            { "string", "string", "string", "number", "number" },
            { "The name or identifier of network adapter.", "The name or identifier of network port.", "The name or identifier of network function.", "The number of times the link has flapped.", "The number of minutes over which the link has flapped." },
            { "This argument shall contain contain a string that identifies or describes the location or physical context of the network adapter to which a connection was established.", "This argument shall contain contain a string that identifies or describes the location or physical context of the network port to which a connection was established.", "This argument shall contain contain a string that identifies or describes the location or physical context of the network device function to which a connection was established.", "This argument shall contain be a number representing the count of link establishment/disconnection cycles.", "This argument shall contain be a number representing the number of minutes over which link flapping activity was detected." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace NetworkDevice_1_0_3
}; // namespace registries
}; // namespace redfish
