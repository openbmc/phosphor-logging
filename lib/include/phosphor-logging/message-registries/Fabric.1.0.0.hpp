#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace Fabric_1_0_0
{

struct [[deprecated ("Use Fabric_1_0_2")]]
MaxFrameSizeExceeded :
    public RedfishMessage<std::string, std::string, double>
{
    template <typename... Args>
    MaxFrameSizeExceeded(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.MaxFrameSizeExceeded",
            "Indicates that the maximum transmission unit (MTU) for the link has been exceeded.",
            "This message shall be used to indicate that packets have been dropped due to the maximum transmission unit (MTU) frame size being exceeded.  This message shall not be emitted more than once in any 24-hour period of time.",
            "MTU size on switch '%1' port '%2' is set to %3.  One or more packets with a larger size have been dropped.",
            "Warning",
            3,
            { "string", "string", "number" },
            { "The `Id` of the switch.", "The `Id` of the port.", "The MTU size." },
            { "This argument shall contain the value of the `Id` property of the switch in which one or more packets have been dropped.", "This argument shall contain the value of the `Id` property of the port in which one or more oversize packets were received.", "This argument shall contain the MTU size." },
            "Ensure that path MTU discovery is enabled and functioning correctly.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
UpstreamLinkEstablished :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    UpstreamLinkEstablished(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.UpstreamLinkEstablished",
            "Indicates that a switch's upstream connection is established.",
            "This message shall be used to indicate that a switch's upstream connection is established.",
            "Switch '%1' upstream link is established on port '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the switch.", "This argument shall contain the value of the `Id` property of the upstream port." },
            "No action needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
UpstreamLinkDropped :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    UpstreamLinkDropped(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.UpstreamLinkDropped",
            "Indicates that a switch's upstream connection has gone down.",
            "This message shall be used to indicate that a switch's upstream connection has gone down.",
            "Switch '%1' upstream link has gone down on port '%2'.",
            "Warning",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the switch.", "This argument shall contain the value of the `Id` property of the upstream port." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
DegradedUpstreamLinkEstablished :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    DegradedUpstreamLinkEstablished(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.DegradedUpstreamLinkEstablished",
            "Indicates that a switch's upstream connection is established, but in a degraded state.",
            "This message shall be used to indicate that a switch's upstream connection is established, but that the link state is degraded.",
            "Switch '%1' upstream link is established on port '%2', but is running in a degraded state.",
            "Warning",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the switch.", "This argument shall contain the value of the `Id` property of the upstream port." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
UpstreamLinkFlapDetected :
    public RedfishMessage<std::string, std::string, double, double>
{
    template <typename... Args>
    UpstreamLinkFlapDetected(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.UpstreamLinkFlapDetected",
            "Indicates that a switch's upstream connection is highly unstable.",
            "This message shall be used to indicate that a switch's upstream connection has repeatedly been established and dropped.  This message shall not be repeated more than once in any 24-hour period.",
            "Switch '%1' upstream link on port '%2' has been established and dropped %3 times in the last %4 minutes.",
            "Warning",
            4,
            { "string", "string", "number", "number" },
            { "The `Id` of the switch.", "The `Id` of the port.", "The number of times the link has flapped.", "The number of minutes over which the link has flapped." },
            { "This argument shall contain the value of the `Id` property of the switch.", "This argument shall contain the value of the `Id` property of the upstream port that is flapping.", "This argument shall contain the count of uplink establishment/disconnection cycles.", "This argument shall contain the number of minutes over which link flapping activity has been detected." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
DownstreamLinkEstablished :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    DownstreamLinkEstablished(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.DownstreamLinkEstablished",
            "Indicates that a switch's downstream connection is established.",
            "This message shall be used to indicate that a switch's downstream connection is established.",
            "Switch '%1' downstream link is established on port '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the switch.", "This argument shall contain the value of the `Id` property of the downstream port." },
            "No action needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
DownstreamLinkDropped :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    DownstreamLinkDropped(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.DownstreamLinkDropped",
            "Indicates that a switch's downstream connection has gone down.",
            "This message shall be used to indicate that a switch's downstream connection has gone down.",
            "Switch '%1' downstream link has gone down on port '%2'.",
            "Warning",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the switch.", "This argument shall contain the value of the `Id` property of the downstream port." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
DegradedDownstreamLinkEstablished :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    DegradedDownstreamLinkEstablished(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.DegradedDownstreamLinkEstablished",
            "Indicates that a switch's downstream connection is established, but in a degraded state.",
            "This message shall be used to indicate that a switch's downstream connection is established, but that the link state is degraded.",
            "Switch '%1' downstream link is established on port '%2', but is running in a degraded state.",
            "Warning",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the switch.", "This argument shall contain the value of the `Id` property of the downstream port." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
DownstreamLinkFlapDetected :
    public RedfishMessage<std::string, std::string, double, double>
{
    template <typename... Args>
    DownstreamLinkFlapDetected(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.DownstreamLinkFlapDetected",
            "Indicates that a switch's downstream connection is highly unstable.",
            "This message shall be used to indicate that a switch's downstream connection has repeatedly been established and dropped.  This message shall not be repeated more than once in any 24-hour period.",
            "Switch '%1' downstream link on port '%2' has been established and dropped %3 times in the last %4 minutes.",
            "Warning",
            4,
            { "string", "string", "number", "number" },
            { "The `Id` of the switch.", "The `Id` of the port.", "The number of times the link has flapped.", "The number of minutes over which the link has flapped." },
            { "This argument shall contain the value of the `Id` property of the switch.", "This argument shall contain the value of the `Id` property of the downstream port that is flapping.", "This argument shall contain the count of uplink establishment/disconnection cycles.", "This argument shall contain the number of minutes over which link flapping activity has been detected." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
InterswitchLinkEstablished :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    InterswitchLinkEstablished(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.InterswitchLinkEstablished",
            "Indicates that a switch's interswitch connection is established.",
            "This message shall be used to indicate that a switch's interswitch connection is established.",
            "Switch '%1' interswitch link is established on port '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the switch.", "This argument shall contain the value of the `Id` property of the interswitch port." },
            "No action needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
InterswitchLinkDropped :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    InterswitchLinkDropped(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.InterswitchLinkDropped",
            "Indicates that a switch's interswitch connection has gone down.",
            "This message shall be used to indicate that a switch's interswitch connection has gone down.",
            "Switch '%1' interswitch link has gone down on port '%2'.",
            "Warning",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the switch.", "This argument shall contain the value of the `Id` property of the interswitch port." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
DegradedInterswitchLinkEstablished :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    DegradedInterswitchLinkEstablished(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.DegradedInterswitchLinkEstablished",
            "Indicates that a switch's interswitch connection is established, but in a degraded state.",
            "This message shall be used to indicate that a switch's interswitch connection is established, but that the link state is degraded.",
            "Switch '%1' interswitch link is established on port '%2', but is running in a degraded state.",
            "Warning",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the switch.", "This argument shall contain the value of the `Id` property of the interswitch port." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
InterswitchLinkFlapDetected :
    public RedfishMessage<std::string, std::string, double, double>
{
    template <typename... Args>
    InterswitchLinkFlapDetected(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.InterswitchLinkFlapDetected",
            "Indicates that a switch's interswitch connection is highly unstable.",
            "This message shall be used to indicate that a switch's interswitch connection has repeatedly been established and dropped.  This message shall not be repeated more than once in any 24-hour period.",
            "Switch '%1' interswitch link on port '%2' has been established and dropped %3 times in the last %4 minutes.",
            "Warning",
            4,
            { "string", "string", "number", "number" },
            { "The `Id` of the switch.", "The `Id` of the port.", "The number of times the link has flapped.", "The number of minutes over which the link has flapped." },
            { "This argument shall contain the value of the `Id` property of the switch.", "This argument shall contain the value of the `Id` property of the interswitch port that is flapping.", "This argument shall contain the count of uplink establishment/disconnection cycles.", "This argument shall contain the number of minutes over which link flapping activity has been detected." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
PortManuallyDisabled :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    PortManuallyDisabled(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.PortManuallyDisabled",
            "Indicates that a switch's port has been manually disabled.",
            "This message shall be used to indicate that a switch's port has been manually disabled, such as by a network administrator.",
            "Switch '%1' port '%2' has been manually disabled.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the switch with the disabled port.", "This argument shall contain the value of the `Id` property of the port that has been disabled." },
            "No action needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
PortManuallyEnabled :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    PortManuallyEnabled(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.PortManuallyEnabled",
            "Indicates that a switch's port has been manually enabled.",
            "This message shall be used to indicate that a switch's port has been manually enabled, such as by a network administrator.",
            "Switch '%1' port '%2' has been manually enabled.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the switch with the enabled port.", "This argument shall contain the value of the `Id` property of the port that has been enabled." },
            "No action needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
PortAutomaticallyDisabled :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    PortAutomaticallyDisabled(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.PortAutomaticallyDisabled",
            "Indicates that a switch's port has been automatically disabled.",
            "This message shall be used to indicate that a switch's port has been automatically disabled, such as by the service.",
            "Switch '%1' port '%2' has been automatically disabled.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the switch with the disabled port.", "This argument shall contain the value of the `Id` property of the port that has been disabled." },
            "No action needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
PortAutomaticallyEnabled :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    PortAutomaticallyEnabled(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.PortAutomaticallyEnabled",
            "Indicates that a switch's port has been automatically enabled.",
            "This message shall be used to indicate that a switch's port has been automatically enabled, such as by the service.",
            "Switch '%1' port '%2' has been automatically enabled.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the switch with the enabled port.", "This argument shall contain the value of the `Id` property of the port that has been enabled." },
            "No action needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
PortFailed :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    PortFailed(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.PortFailed",
            "Indicates that a switch's port has become inoperative.",
            "This message shall be used to indicate that a port has become inoperative.  This message shall not be issued when a port is manually disabled.",
            "Switch '%1' port '%2' has failed and is inoperative.",
            "Critical",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the switch with the failed port.", "This argument shall contain the value of the `Id` property of the port that has failed." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
PortDegraded :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    PortDegraded(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.PortDegraded",
            "Indicates that a switch's port is in a degraded state.",
            "This message shall be used to indicate that a port has entered a degraded state.",
            "Switch '%1' port '%2' is in a degraded state.",
            "Warning",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the switch with the degraded port.", "This argument shall contain the value of the `Id` property of the port that has entered a degraded state." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
PortOK :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    PortOK(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.PortOK",
            "Indicates that a switch's port has returned to a functional state.",
            "This message shall be used to indicate that a port has returned to a functional state.",
            "Switch '%1' port '%2' has returned to a functional state.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the switch with the functional port.", "This argument shall contain the value of the `Id` property of the port that has returned to a functional state." },
            "No action is needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
SwitchFailed :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    SwitchFailed(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.SwitchFailed",
            "Indicates that a switch has become inoperative.",
            "This message shall be used to indicate that a switch has become inoperative.",
            "Switch '%1' has failed and is inoperative.",
            "Critical",
            1,
            { "string" },
            { "The `Id` of the switch." },
            { "This argument shall contain the value of the `Id` property of the switch that has failed." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
SwitchDegraded :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    SwitchDegraded(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.SwitchDegraded",
            "Indicates that a switch is in a degraded state.",
            "This message shall be used to indicate that a switch has entered a degraded state.",
            "Switch '%1' is in a degraded state.",
            "Warning",
            1,
            { "string" },
            { "The `Id` of the switch." },
            { "This argument shall contain the value of the `Id` property of the switch that has entered a degraded state." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
SwitchOK :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    SwitchOK(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.SwitchOK",
            "Indicates that a switch has returned to a functional state.",
            "This message shall be used to indicate that a switch has returned to a functional state.",
            "Switch '%1' has returned to a functional state.",
            "OK",
            1,
            { "string" },
            { "The `Id` of the switch." },
            { "This argument shall contain the value of the `Id` property of the switch that has returned to a functional state." },
            "No action is needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
CableRemoved :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    CableRemoved(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.CableRemoved",
            "Indicates that a cable has been removed from a switch's port.",
            "This message shall be used to indicate that a cable has been removed from a port on a switch.",
            "A cable has been removed from switch '%1' port '%2'.",
            "Warning",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the switch with the port whose cable was removed.", "This argument shall contain the value of the `Id` property of the port whose cable was removed." },
            "No action is needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
CableInserted :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    CableInserted(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.CableInserted",
            "Indicates that a cable has been inserted into a switch's port.",
            "This message shall be used to indicate that a cable has been inserted into a port on a switch.",
            "A cable has been inserted into switch '%1' port '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the switch with the port whose cable was inserted.", "This argument shall contain the value of the `Id` property of the port whose cable was inserted." },
            "No action is needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
CableFailed :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    CableFailed(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.CableFailed",
            "Indicates that a cable has failed.",
            "This message shall be used to indicate that a cable has been detected as failed.  This message shall not be emitted more than once in any 24-hour period of time.",
            "The cable in switch '%1' port '%2' has failed.",
            "Warning",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the switch with the failed cable.", "This argument shall contain the value of the `Id` property of the port with the failed cable." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
CableOK  :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    CableOK (Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.CableOK ",
            "Indicates that a cable has returned to working condition.",
            "This message shall be used to indicate that a cable has been restored to working condition.",
            "The cable in switch '%1' port '%2' has returned to working condition.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the switch with the cable that returned to a functional state.", "This argument shall contain the value of the `Id` property of the port with the cable that returned to a functional state." },
            "No action is needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
ConnectionCreated :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ConnectionCreated(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.ConnectionCreated",
            "Indicates that a connection has been created.",
            "This message shall be used to indicate that a resource of type `Connection` has been created.",
            "Connection '%1' has been created in fabric '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the connection.", "The `Id` of the fabric." },
            { "This argument shall contain the value of the `Id` property of the connection that was created.", "This argument shall contain the value of the `Id` property of the fabric in which the connection was created." },
            "No action is needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
ConnectionRemoved :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ConnectionRemoved(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.ConnectionRemoved",
            "Indicates that a connection has been removed.",
            "This message shall be used to indicate that a resource of type `Connection` has been removed.",
            "Connection '%1' has been removed from fabric '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the connection.", "The `Id` of the fabric." },
            { "This argument shall contain the value of the `Id` property of the connection that was removed.", "This argument shall contain the value of the `Id` property of the fabric in which the connection was removed." },
            "No action is needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
ConnectionModified :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ConnectionModified(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.ConnectionModified",
            "Indicates that a connection has been modified.",
            "This message shall be used to indicate that a resource of type `Connection` has been modified.",
            "Connection '%1' in fabric '%2' has been modified.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the connection.", "The `Id` of the fabric." },
            { "This argument shall contain the value of the `Id` property of the connection that was modified.", "This argument shall contain the value of the `Id` property of the fabric in which the connection was modified." },
            "No action is needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
ZoneCreated :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ZoneCreated(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.ZoneCreated",
            "Indicates that a zone has been created.",
            "This message shall be used to indicate that a resource of type `Zone` has been created.",
            "Zone '%1' has been created in fabric '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the zone.", "The `Id` of the fabric." },
            { "This argument shall contain the value of the `Id` property of the zone that was created.", "This argument shall contain the value of the `Id` property of the fabric in which the zone was created." },
            "No action is needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
ZoneRemoved :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ZoneRemoved(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.ZoneRemoved",
            "Indicates that a zone has been removed.",
            "This message shall be used to indicate that a resource of type `Zone` has been removed.",
            "Zone '%1' has been removed from fabric '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the zone.", "The `Id` of the fabric." },
            { "This argument shall contain the value of the `Id` property of the zone that was removed.", "This argument shall contain the value of the `Id` property of the fabric in which the zone was removed." },
            "No action is needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
ZoneModified :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ZoneModified(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.ZoneModified",
            "Indicates that a zone has been modified.",
            "This message shall be used to indicate that a resource of type `Zone` has been modified.",
            "Zone '%1' in fabric '%2' has been modified.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the zone.", "The `Id` of the fabric." },
            { "This argument shall contain the value of the `Id` property of the zone that was modified.", "This argument shall contain the value of the `Id` property of the fabric in which the zone was modified." },
            "No action is needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
EndpointCreated :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    EndpointCreated(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.EndpointCreated",
            "Indicates that an endpoint has been created or discovered.",
            "This message shall be used to indicate that a resource of type `Endpoint` has been created or discovered.",
            "Endpoint '%1' has been created in fabric '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the endpoint.", "The `Id` of the fabric." },
            { "This argument shall contain the value of the `Id` property of the endpoint that was created.", "This argument shall contain the value of the `Id` property of the fabric in which the endpoint was created." },
            "No action is needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
EndpointRemoved :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    EndpointRemoved(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.EndpointRemoved",
            "Indicates that an endpoint has been removed.",
            "This message shall be used to indicate that a resource of type `Endpoint` has been removed.",
            "Endpoint '%1' has been removed from fabric '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the endpoint.", "The `Id` of the fabric." },
            { "This argument shall contain the value of the `Id` property of the endpoint that was removed.", "This argument shall contain the value of the `Id` property of the fabric in which the endpoint was removed." },
            "No action is needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
EndpointModified :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    EndpointModified(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.EndpointModified",
            "Indicates that an endpoint has been modified.",
            "This message shall be used to indicate that a resource of type `Endpoint` has been modified.",
            "Endpoint '%1' in fabric '%2' has been modified.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the endpoint.", "The `Id` of the fabric." },
            { "This argument shall contain the value of the `Id` property of the endpoint that was modified.", "This argument shall contain the value of the `Id` property of the fabric in which the endpoint was modified." },
            "No action is needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
AddressPoolCreated :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    AddressPoolCreated(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.AddressPoolCreated",
            "Indicates that an address pool has been created.",
            "This message shall be used to indicate that a resource of type `AddressPool` has been created.",
            "Address pool '%1' has been created in fabric '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the address pool.", "The `Id` of the fabric." },
            { "This argument shall contain the value of the `Id` property of the address pool that was created.", "This argument shall contain the value of the `Id` property of the fabric in which the address pool was created." },
            "No action is needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
AddressPoolRemoved :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    AddressPoolRemoved(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.AddressPoolRemoved",
            "Indicates that an address pool has been removed.",
            "This message shall be used to indicate that a resource of type `AddressPool` has been removed.",
            "Address pool '%1' has been removed from fabric '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the address pool.", "The `Id` of the fabric." },
            { "This argument shall contain the value of the `Id` property of the address pool that was removed.", "This argument shall contain the value of the `Id` property of the fabric in which the address pool was removed." },
            "No action is needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
AddressPoolModified :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    AddressPoolModified(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.AddressPoolModified",
            "Indicates that an address pool has been modified.",
            "This message shall be used to indicate that a resource of type `AddressPool` has been modified.",
            "Address pool '%1' in fabric '%2' has been modified.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the address pool.", "The `Id` of the fabric." },
            { "This argument shall contain the value of the `Id` property of the address pool that was modified.", "This argument shall contain the value of the `Id` property of the fabric in which the address pool was modified." },
            "No action is needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
MediaControllerAdded :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    MediaControllerAdded(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.MediaControllerAdded",
            "Indicates that a media controller has been added.",
            "This message shall be used to indicate that a resource of type `MediaController` has been added.",
            "Media controller '%1' has been added to chassis '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the media controller.", "The `Id` of the chassis." },
            { "This argument shall contain the value of the `Id` property of the media controller that was added.", "This argument shall contain the value of the `Id` property of the chassis in which the media controller was added." },
            "No action is needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
MediaControllerRemoved :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    MediaControllerRemoved(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.MediaControllerRemoved",
            "Indicates that a media controller has been removed.",
            "This message shall be used to indicate that a resource of type `MediaController` has been removed.",
            "Media controller '%1' has been removed from chassis '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the media controller.", "The `Id` of the chassis." },
            { "This argument shall contain the value of the `Id` property of the media controller that was removed.", "This argument shall contain the value of the `Id` property of the chassis in which the media controller was removed." },
            "No action is needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Fabric_1_0_2")]]
MediaControllerModified :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    MediaControllerModified(Args... args) :
        RedfishMessage(
            "Fabric.1.0.0.MediaControllerModified",
            "Indicates that a media controller has been modified.",
            "This message shall be used to indicate that a resource of type `MediaController` has been modified.",
            "Media controller '%1' in chassis '%2' has been modified.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the media controller.", "The `Id` of the chassis." },
            { "This argument shall contain the value of the `Id` property of the media controller that was modified.", "This argument shall contain the value of the `Id` property of the chassis in which the media controller was modified." },
            "No action is needed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace Fabric_1_0_0
}; // namespace registries
}; // namespace redfish
