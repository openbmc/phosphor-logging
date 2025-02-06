#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace EthernetFabric_1_0_1
{

struct MLAGInterfacesUp :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    MLAGInterfacesUp(Args... args) :
        RedfishMessage(
            "EthernetFabric.1.0.1.MLAGInterfacesUp",
            "Indicates that all multi-chassis link aggregation group (MLAG) interfaces are up.",
            "This message shall indicate all multi-chassis link aggregation group (MLAG) interfaces were established for one MLAG domain ID.",
            "All MLAG interfaces were established for MLAG ID '%1'.",
            "OK",
            1,
            { "string" },
            { "The MLAG domain ID." },
            { "This argument shall contain the MLAG domain ID that was established." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct MLAGInterfaceDown :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    MLAGInterfaceDown(Args... args) :
        RedfishMessage(
            "EthernetFabric.1.0.1.MLAGInterfaceDown",
            "Indicates that the multi-chassis link aggregation group (MLAG) interface is down on a switch.",
            "This message shall indicate that the multi-chassis link aggregation group (MLAG) interface is down on a switch.",
            "The MLAG interface '%1' on switch '%2' is down.",
            "Warning",
            2,
            { "string", "string" },
            { "The MLAG domain ID.", "The `Id` of the switch." },
            { "This argument shall contain the MLAG domain ID that dropped.", "This argument shall contain the value of the `Id` property of the `Switch` resource for which an MLAG interface is down." },
            "Check physical connectivity and that the MLAG system ID matches on switch pairs.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct MLAGInterfaceDegraded :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    MLAGInterfaceDegraded(Args... args) :
        RedfishMessage(
            "EthernetFabric.1.0.1.MLAGInterfaceDegraded",
            "Indicates that multi-chassis link aggregation group (MLAG) interfaces were established, but at an unexpectedly low aggregated link speed.",
            "This message shall indicate that multi-chassis link aggregation group (MLAG) interfaces were established, but the aggregated link speed is lower than provisioned.",
            "MLAG interface '%1' is degraded on switch '%2'.",
            "Warning",
            2,
            { "string", "string" },
            { "The MLAG domain ID.", "The `Id` of the switch." },
            { "This argument shall contain the MLAG domain ID that is degraded.", "This argument shall contain the value of the `Id` property of the `Switch` resource for which an MLAG interface is degraded." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct MLAGPeerUp :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    MLAGPeerUp(Args... args) :
        RedfishMessage(
            "EthernetFabric.1.0.1.MLAGPeerUp",
            "Indicates that the multi-chassis link aggregation group (MLAG) peer is up.",
            "This message shall indicate that a multi-chassis link aggregation group (MLAG) peer is up.",
            "MLAG peer switch '%1' with MLAG ID '%2' is up.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the peer switch.", "The MLAG domain ID." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource that represents the peer switch for which MLAG interfaces are up.", "This argument shall contain the value of the MLAG domain ID that is established." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct MLAGPeerDown :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    MLAGPeerDown(Args... args) :
        RedfishMessage(
            "EthernetFabric.1.0.1.MLAGPeerDown",
            "Indicates that the multi-chassis link aggregation group (MLAG) peer is down.",
            "This message shall indicate that a multi-chassis link aggregation group (MLAG) peer is down.",
            "MLAG peer switch '%1' with MLAG ID '%2' is down.",
            "Warning",
            2,
            { "string", "string" },
            { "The `Id` of the peer switch.", "The MLAG domain ID." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource that represents the peer switch for which MLAG interface dropped.", "This argument shall contain the value of the MLAG domain ID that dropped." },
            "Check physical connectivity and that the port channel ID matches on switch pairs.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct LLDPInterfaceEnabled :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    LLDPInterfaceEnabled(Args... args) :
        RedfishMessage(
            "EthernetFabric.1.0.1.LLDPInterfaceEnabled",
            "Indicates that an interface has enabled Link Layer Discovery Protocol (LLDP).",
            "This message shall indicate that an interface has enabled Link Layer Discovery Protocol (LLDP).",
            "LLDP was enabled on switch '%1' port '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource on which LLDP was enabled.", "This argument shall contain the value of the `Id` property of the `Port` resource on which LLDP was enabled." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct LLDPInterfaceDisabled :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    LLDPInterfaceDisabled(Args... args) :
        RedfishMessage(
            "EthernetFabric.1.0.1.LLDPInterfaceDisabled",
            "Indicates that an interface has disabled Link Layer Discovery Protocol (LLDP).",
            "This message shall indicate that an interface has disabled Link Layer Discovery Protocol (LLDP).",
            "LLDP was disabled on switch '%1' port '%2'.",
            "Warning",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the `Switch` on which LLDP was disabled.", "This argument shall contain the value of the `Id` property of the `Port` on which LLDP was disabled." },
            "Check that LLDP is enabled on device endpoints.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct RoutingFailureThresholdExceeded :
    public RedfishMessage<std::string, double, double>
{
    template <typename... Args>
    RoutingFailureThresholdExceeded(Args... args) :
        RedfishMessage(
            "EthernetFabric.1.0.1.RoutingFailureThresholdExceeded",
            "Indicates that a switch has encountered an unusually large number of routing errors.",
            "This message shall indicate that a switch has exceeded a rate threshold for routing errors.  This message shall not be emitted more than once in any 24-hour period of time.",
            "Switch '%1' has encountered %2 routing errors in the last %3 minutes.",
            "Warning",
            3,
            { "string", "number", "number" },
            { "The `Id` of the switch.", "The number of routing errors encountered.", "The number of minutes over which routing errors were encountered." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource that encountered routing errors.", "This argument shall contain the number of routing errors encountered.", "This argument shall contain the number of minutes over which routing errors were encountered." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace EthernetFabric_1_0_1
}; // namespace registries
}; // namespace redfish
