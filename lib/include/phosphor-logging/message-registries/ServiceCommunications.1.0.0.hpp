#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace ServiceCommunications_1_0_0
{

struct NameResolutionFailure :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    NameResolutionFailure(Args... args) :
        RedfishMessage(
            "ServiceCommunications.1.0.0.NameResolutionFailure",
            "Indicates that DNS or other name resolution failed on the indicated name.",
            "This message shall indicate that DNS or other name resolution failed on the indicated name.",
            "There was an error resolving the name '%1'.",
            "Critical",
            1,
            { "string" },
            { "The network name that could not be resolved." },
            { "This argument shall contain the network name of the remote service that could not be resolved." },
            "Check your name server settings or the name provided.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ConnectionRefused :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    ConnectionRefused(Args... args) :
        RedfishMessage(
            "ServiceCommunications.1.0.0.ConnectionRefused",
            "Indicates that a connection to the remote service was refused.",
            "This message shall indicate that a connection to the remote service was refused.",
            "The connection to '%1:%2' was refused.",
            "Critical",
            2,
            { "string", "number" },
            { "The network name of the service that refused the connection.", "The port number that was refused." },
            { "This argument shall contain the network name of the remote service that refused the connection.", "This argument shall contain the port number that was refused." },
            "Check the settings on the remote service or the port settings.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ConnectionTimeout :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    ConnectionTimeout(Args... args) :
        RedfishMessage(
            "ServiceCommunications.1.0.0.ConnectionTimeout",
            "Indicates that a connection to the remote service timed out.",
            "This message shall indicate that a connection to the remote service timed out.",
            "The connection to '%1:%2' timed out.",
            "Critical",
            2,
            { "string", "number" },
            { "The network name of the service that timed out.", "The port number that timed out." },
            { "This argument shall contain the network name of the remote service that timed out.", "This argument shall contain the port number that timed out." },
            "Check the settings on the remote service or the port settings.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct HTTPFailure :
    public RedfishMessage<double, std::string>
{
    template <typename... Args>
    HTTPFailure(Args... args) :
        RedfishMessage(
            "ServiceCommunications.1.0.0.HTTPFailure",
            "Indicates that an HTTP failure code was returned.",
            "This message shall indicate that an HTTP failure code was returned.",
            "Received HTTP status code %1 from '%2'.",
            "Critical",
            2,
            { "number", "string" },
            { "The HTTP status code that was returned.", "The network name of the service that returned the status code." },
            { "This argument shall contain the HTTP status code indicating an error that was returned.  This argument should contain a value in the `4XX` or `5XX` range.", "This argument shall contain the network name of the remote service that returned the status code." },
            "Check the payload settings and the remote service for errors.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace ServiceCommunications_1_0_0
}; // namespace registries
}; // namespace redfish
