#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace Fabric_1_0_2
{

struct MaxFrameSizeExceeded :
    public RedfishMessage<std::string, std::string, double>
{
    template <typename... Args>
    MaxFrameSizeExceeded(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.MaxFrameSizeExceeded",
            "Indicates that the maximum transmission unit (MTU) for the link was exceeded.",
            "This message shall indicate that packets were dropped due to the maximum transmission unit (MTU) frame size being exceeded.  This message shall not be emitted more than once in any 24-hour period of time.",
            "MTU size on switch '%1' port '%2' is set to %3.  One or more packets with a larger size were dropped.",
            "Warning",
            3,
            { "string", "string", "number" },
            { "The `Id` of the switch.", "The `Id` of the port.", "The MTU size." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource in which one or more packets were dropped.", "This argument shall contain the value of the `Id` property of the `Port` resource in which one or more oversize packets were received.", "This argument shall contain the MTU size." },
            "Ensure that path MTU discovery is enabled and functioning correctly.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct UpstreamLinkEstablished :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    UpstreamLinkEstablished(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.UpstreamLinkEstablished",
            "Indicates that a switch's upstream connection is established.",
            "This message shall indicate that a switch's upstream connection is established.",
            "Switch '%1' upstream link is established on port '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource.", "This argument shall contain the value of the `Id` property of the `Port` resource that represents the upstream port." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct UpstreamLinkDropped :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    UpstreamLinkDropped(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.UpstreamLinkDropped",
            "Indicates that a switch's upstream connection has gone down.",
            "This message shall indicate that a switch's upstream connection has gone down.",
            "Switch '%1' upstream link has gone down on port '%2'.",
            "Warning",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource.", "This argument shall contain the value of the `Id` property of the `Port` resource that represents the upstream port." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct DegradedUpstreamLinkEstablished :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    DegradedUpstreamLinkEstablished(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.DegradedUpstreamLinkEstablished",
            "Indicates that a switch's upstream connection is established but is in a degraded state.",
            "This message shall indicate that a switch's upstream connection is established but that the link state is degraded.",
            "Switch '%1' upstream link is established on port '%2', but is running in a degraded state.",
            "Warning",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource.", "This argument shall contain the value of the `Id` property of the `Port` resource that represents the upstream port." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct UpstreamLinkFlapDetected :
    public RedfishMessage<std::string, std::string, double, double>
{
    template <typename... Args>
    UpstreamLinkFlapDetected(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.UpstreamLinkFlapDetected",
            "Indicates that a switch's upstream connection is highly unstable.",
            "This message shall indicate that a switch's upstream connection was repeatedly established and dropped.  This message shall not be repeated more than once in any 24-hour period.",
            "Switch '%1' upstream link on port '%2' was established and dropped %3 times in the last %4 minutes.",
            "Warning",
            4,
            { "string", "string", "number", "number" },
            { "The `Id` of the switch.", "The `Id` of the port.", "The number of times the link has flapped.", "The number of minutes over which the link has flapped." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource.", "This argument shall contain the value of the `Id` property of the `Port` resource that represents the upstream port that is flapping.", "This argument shall contain the count of uplink establishment/disconnection cycles.", "This argument shall contain the number of minutes over which link flapping activity was detected." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct DownstreamLinkEstablished :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    DownstreamLinkEstablished(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.DownstreamLinkEstablished",
            "Indicates that a switch's downstream connection is established.",
            "This message shall indicate that a switch's downstream connection is established.",
            "Switch '%1' downstream link is established on port '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource.", "This argument shall contain the value of the `Id` property of the `Port` resource that represents the downstream port." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct DownstreamLinkDropped :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    DownstreamLinkDropped(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.DownstreamLinkDropped",
            "Indicates that a switch's downstream connection has gone down.",
            "This message shall indicate that a switch's downstream connection has gone down.",
            "Switch '%1' downstream link has gone down on port '%2'.",
            "Warning",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource.", "This argument shall contain the value of the `Id` property of the `Port` resource that represents the downstream port." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct DegradedDownstreamLinkEstablished :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    DegradedDownstreamLinkEstablished(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.DegradedDownstreamLinkEstablished",
            "Indicates that a switch's downstream connection is established but is in a degraded state.",
            "This message shall indicate that a switch's downstream connection is established but that the link state is degraded.",
            "Switch '%1' downstream link is established on port '%2', but is running in a degraded state.",
            "Warning",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource.", "This argument shall contain the value of the `Id` property of the `Port` resource that represents the downstream port." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct DownstreamLinkFlapDetected :
    public RedfishMessage<std::string, std::string, double, double>
{
    template <typename... Args>
    DownstreamLinkFlapDetected(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.DownstreamLinkFlapDetected",
            "Indicates that a switch's downstream connection is highly unstable.",
            "This message shall indicate that a switch's downstream connection was repeatedly established and dropped.  This message shall not be repeated more than once in any 24-hour period.",
            "Switch '%1' downstream link on port '%2' was established and dropped %3 times in the last %4 minutes.",
            "Warning",
            4,
            { "string", "string", "number", "number" },
            { "The `Id` of the switch.", "The `Id` of the port.", "The number of times the link has flapped.", "The number of minutes over which the link has flapped." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource.", "This argument shall contain the value of the `Id` property of the `Port` resource that represents the downstream port that is flapping.", "This argument shall contain the count of uplink establishment/disconnection cycles.", "This argument shall contain the number of minutes over which link flapping activity was detected." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct InterswitchLinkEstablished :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    InterswitchLinkEstablished(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.InterswitchLinkEstablished",
            "Indicates that a switch's interswitch connection is established.",
            "This message shall indicate that a switch's interswitch connection is established.",
            "Switch '%1' interswitch link is established on port '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource.", "This argument shall contain the value of the `Id` property of the `Port` resource that represents the interswitch port." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct InterswitchLinkDropped :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    InterswitchLinkDropped(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.InterswitchLinkDropped",
            "Indicates that a switch's interswitch connection has gone down.",
            "This message shall indicate that a switch's interswitch connection has gone down.",
            "Switch '%1' interswitch link has gone down on port '%2'.",
            "Warning",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource.", "This argument shall contain the value of the `Id` property of the `Port` resource that represents the interswitch port." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct DegradedInterswitchLinkEstablished :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    DegradedInterswitchLinkEstablished(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.DegradedInterswitchLinkEstablished",
            "Indicates that a switch's interswitch connection is established but is in a degraded state.",
            "This message shall indicate that a switch's interswitch connection is established but that the link state is degraded.",
            "Switch '%1' interswitch link is established on port '%2', but is running in a degraded state.",
            "Warning",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource.", "This argument shall contain the value of the `Id` property of the `Port` resource that represents the interswitch port." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct InterswitchLinkFlapDetected :
    public RedfishMessage<std::string, std::string, double, double>
{
    template <typename... Args>
    InterswitchLinkFlapDetected(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.InterswitchLinkFlapDetected",
            "Indicates that a switch's interswitch connection is highly unstable.",
            "This message shall indicate that a switch's interswitch connection was repeatedly established and dropped.  This message shall not be repeated more than once in any 24-hour period.",
            "Switch '%1' interswitch link on port '%2' was established and dropped %3 times in the last %4 minutes.",
            "Warning",
            4,
            { "string", "string", "number", "number" },
            { "The `Id` of the switch.", "The `Id` of the port.", "The number of times the link has flapped.", "The number of minutes over which the link has flapped." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource.", "This argument shall contain the value of the `Id` property of the `Port` resource that represents the interswitch port that is flapping.", "This argument shall contain the count of uplink establishment/disconnection cycles.", "This argument shall contain the number of minutes over which link flapping activity was detected." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PortManuallyDisabled :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    PortManuallyDisabled(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.PortManuallyDisabled",
            "Indicates that a switch's port was manually disabled.",
            "This message shall indicate that a switch's port was manually disabled, such as by a network administrator.",
            "Switch '%1' port '%2' was manually disabled.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource with the disabled port.", "This argument shall contain the value of the `Id` property of the `Port` resource that was disabled." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PortManuallyEnabled :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    PortManuallyEnabled(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.PortManuallyEnabled",
            "Indicates that a switch's port was manually enabled.",
            "This message shall indicate that a switch's port was manually enabled, such as by a network administrator.",
            "Switch '%1' port '%2' was manually enabled.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource with the enabled port.", "This argument shall contain the value of the `Id` property of the `Port` resource that was enabled." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PortAutomaticallyDisabled :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    PortAutomaticallyDisabled(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.PortAutomaticallyDisabled",
            "Indicates that a switch's port was automatically disabled.",
            "This message shall indicate that a switch's port was automatically disabled, such as by the service.",
            "Switch '%1' port '%2' was automatically disabled.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource with the disabled port.", "This argument shall contain the value of the `Id` property of the `Port` resource that was disabled." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PortAutomaticallyEnabled :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    PortAutomaticallyEnabled(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.PortAutomaticallyEnabled",
            "Indicates that a switch's port was automatically enabled.",
            "This message shall indicate that a switch's port was automatically enabled, such as by the service.",
            "Switch '%1' port '%2' was automatically enabled.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource with the enabled port.", "This argument shall contain the value of the `Id` property of the `Port` resource that was enabled." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PortFailed :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    PortFailed(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.PortFailed",
            "Indicates that a switch's port has become inoperative.",
            "This message shall indicate that a port has become inoperative.  This message shall not be issued when a port is manually disabled.",
            "Switch '%1' port '%2' has failed and is inoperative.",
            "Critical",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource with the failed port.", "This argument shall contain the value of the `Id` property of the `Port` resource that has failed." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PortDegraded :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    PortDegraded(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.PortDegraded",
            "Indicates that a switch's port is in a degraded state.",
            "This message shall indicate that a port has entered a degraded state.",
            "Switch '%1' port '%2' is in a degraded state.",
            "Warning",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource with the degraded port.", "This argument shall contain the value of the `Id` property of the `Port` resource that has entered a degraded state." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct PortOK :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    PortOK(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.PortOK",
            "Indicates that a switch's port has returned to a functional state.",
            "This message shall indicate that a port has returned to a functional state.",
            "Switch '%1' port '%2' has returned to a functional state.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource with the functional port.", "This argument shall contain the value of the `Id` property of the `Port` resource that has returned to a functional state." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct SwitchFailed :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    SwitchFailed(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.SwitchFailed",
            "Indicates that a switch has become inoperative.",
            "This message shall indicate that a switch has become inoperative.",
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

struct SwitchDegraded :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    SwitchDegraded(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.SwitchDegraded",
            "Indicates that a switch is in a degraded state.",
            "This message shall indicate that a switch has entered a degraded state.",
            "Switch '%1' is in a degraded state.",
            "Warning",
            1,
            { "string" },
            { "The `Id` of the switch." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource that has entered a degraded state." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct SwitchOK :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    SwitchOK(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.SwitchOK",
            "Indicates that a switch has returned to a functional state.",
            "This message shall indicate that a switch has returned to a functional state.",
            "Switch '%1' has returned to a functional state.",
            "OK",
            1,
            { "string" },
            { "The `Id` of the switch." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource that has returned to a functional state." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct CableRemoved :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    CableRemoved(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.CableRemoved",
            "Indicates that a cable was removed from a switch's port.",
            "This message shall indicate that a cable was removed from a port on a switch.",
            "A cable was removed from switch '%1' port '%2'.",
            "Warning",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource with the port whose cable was removed.", "This argument shall contain the value of the `Id` property of the `Port` resource whose cable was removed." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct CableInserted :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    CableInserted(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.CableInserted",
            "Indicates that a cable was inserted into a switch's port.",
            "This message shall indicate that a cable was inserted into a port on a switch.",
            "A cable was inserted into switch '%1' port '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource with the port whose cable was inserted.", "This argument shall contain the value of the `Id` property of the `Port` resource whose cable was inserted." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct CableFailed :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    CableFailed(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.CableFailed",
            "Indicates that a cable has failed.",
            "This message shall indicate that a cable was detected as failed.  This message shall not be emitted more than once in any 24-hour period of time.",
            "The cable in switch '%1' port '%2' has failed.",
            "Warning",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource with the failed cable.", "This argument shall contain the value of the `Id` property of the `Port` resource with the failed cable." },
            "Contact the network administrator for problem resolution.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct CableOK :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    CableOK(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.CableOK",
            "Indicates that a cable has returned to working condition.",
            "This message shall indicate that a cable was restored to working condition.",
            "The cable in switch '%1' port '%2' has returned to working condition.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the switch.", "The `Id` of the port." },
            { "This argument shall contain the value of the `Id` property of the `Switch` resource with the cable that returned to a functional state.", "This argument shall contain the value of the `Id` property of the `Port` resource with the cable that returned to a functional state." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ConnectionCreated :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ConnectionCreated(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.ConnectionCreated",
            "Indicates that a connection was created.",
            "This message shall indicate that a resource of type `Connection` was created.",
            "Connection '%1' was created in fabric '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the connection.", "The `Id` of the fabric." },
            { "This argument shall contain the value of the `Id` property of the `Connection` resource that was created.", "This argument shall contain the value of the `Id` property of the `Fabric` resource in which the connection was created." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ConnectionRemoved :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ConnectionRemoved(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.ConnectionRemoved",
            "Indicates that a connection was removed.",
            "This message shall indicate that a resource of type `Connection` was removed.",
            "Connection '%1' was removed from fabric '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the connection.", "The `Id` of the fabric." },
            { "This argument shall contain the value of the `Id` property of the `Connection` resource that was removed.", "This argument shall contain the value of the `Id` property of the `Fabric` resource in which the connection was removed." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ConnectionModified :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ConnectionModified(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.ConnectionModified",
            "Indicates that a connection was modified.",
            "This message shall indicate that a resource of type `Connection` was modified.",
            "Connection '%1' in fabric '%2' was modified.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the connection.", "The `Id` of the fabric." },
            { "This argument shall contain the value of the `Id` property of the `Connection` resource that was modified.", "This argument shall contain the value of the `Id` property of the `Fabric` resource in which the connection was modified." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ZoneCreated :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ZoneCreated(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.ZoneCreated",
            "Indicates that a zone was created.",
            "This message shall indicate that a resource of type `Zone` was created.",
            "Zone '%1' was created in fabric '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the zone.", "The `Id` of the fabric." },
            { "This argument shall contain the value of the `Id` property of the `Zone` resource that was created.", "This argument shall contain the value of the `Id` property of the `Fabric` resource in which the zone was created." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ZoneRemoved :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ZoneRemoved(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.ZoneRemoved",
            "Indicates that a zone was removed.",
            "This message shall indicate that a resource of type `Zone` was removed.",
            "Zone '%1' was removed from fabric '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the zone.", "The `Id` of the fabric." },
            { "This argument shall contain the value of the `Id` property of the `Zone` resource that was removed.", "This argument shall contain the value of the `Id` property of the `Fabric` resource in which the zone was removed." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct ZoneModified :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ZoneModified(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.ZoneModified",
            "Indicates that a zone was modified.",
            "This message shall indicate that a resource of type `Zone` was modified.",
            "Zone '%1' in fabric '%2' was modified.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the zone.", "The `Id` of the fabric." },
            { "This argument shall contain the value of the `Id` property of the `Zone` resource that was modified.", "This argument shall contain the value of the `Id` property of the `Fabric` resource in which the zone was modified." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct EndpointCreated :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    EndpointCreated(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.EndpointCreated",
            "Indicates that an endpoint was created or discovered.",
            "This message shall indicate that a resource of type `Endpoint` was created or discovered.",
            "Endpoint '%1' was created in fabric '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the endpoint.", "The `Id` of the fabric." },
            { "This argument shall contain the value of the `Id` property of the `Endpoint` resource that was created.", "This argument shall contain the value of the `Id` property of the `Fabric` resource in which the endpoint was created." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct EndpointRemoved :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    EndpointRemoved(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.EndpointRemoved",
            "Indicates that an endpoint was removed.",
            "This message shall indicate that a resource of type `Endpoint` was removed.",
            "Endpoint '%1' was removed from fabric '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the endpoint.", "The `Id` of the fabric." },
            { "This argument shall contain the value of the `Id` property of the `Endpoint` resource that was removed.", "This argument shall contain the value of the `Id` property of the `Fabric` resource in which the endpoint was removed." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct EndpointModified :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    EndpointModified(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.EndpointModified",
            "Indicates that an endpoint was modified.",
            "This message shall indicate that a resource of type `Endpoint` was modified.",
            "Endpoint '%1' in fabric '%2' was modified.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the endpoint.", "The `Id` of the fabric." },
            { "This argument shall contain the value of the `Id` property of the `Endpoint` resource that was modified.", "This argument shall contain the value of the `Id` property of the `Fabric` resource in which the endpoint was modified." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct AddressPoolCreated :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    AddressPoolCreated(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.AddressPoolCreated",
            "Indicates that an address pool was created.",
            "This message shall indicate that a resource of type `AddressPool` was created.",
            "Address pool '%1' was created in fabric '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the address pool.", "The `Id` of the fabric." },
            { "This argument shall contain the value of the `Id` property of the `AddressPool` resource that was created.", "This argument shall contain the value of the `Id` property of the `Fabric` resource in which the address pool was created." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct AddressPoolRemoved :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    AddressPoolRemoved(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.AddressPoolRemoved",
            "Indicates that an address pool was removed.",
            "This message shall indicate that a resource of type `AddressPool` was removed.",
            "Address pool '%1' was removed from fabric '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the address pool.", "The `Id` of the fabric." },
            { "This argument shall contain the value of the `Id` property of the `AddressPool` resource that was removed.", "This argument shall contain the value of the `Id` property of the `Fabric` resource in which the address pool was removed." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct AddressPoolModified :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    AddressPoolModified(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.AddressPoolModified",
            "Indicates that an address pool was modified.",
            "This message shall indicate that a resource of type `AddressPool` was modified.",
            "Address pool '%1' in fabric '%2' was modified.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the address pool.", "The `Id` of the fabric." },
            { "This argument shall contain the value of the `Id` property of the `AddressPool` resource that was modified.", "This argument shall contain the value of the `Id` property of the `Fabric` resource in which the address pool was modified." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct MediaControllerAdded :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    MediaControllerAdded(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.MediaControllerAdded",
            "Indicates that a media controller was added.",
            "This message shall indicate that a resource of type `MediaController` was added.",
            "Media controller '%1' was added to chassis '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the media controller.", "The `Id` of the chassis." },
            { "This argument shall contain the value of the `Id` property of the `MediaController` resource that was added.", "This argument shall contain the value of the `Id` property of the `Chassis` resource in which the media controller was added." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct MediaControllerRemoved :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    MediaControllerRemoved(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.MediaControllerRemoved",
            "Indicates that a media controller was removed.",
            "This message shall indicate that a resource of type `MediaController` was removed.",
            "Media controller '%1' was removed from chassis '%2'.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the media controller.", "The `Id` of the chassis." },
            { "This argument shall contain the value of the `Id` property of the `MediaController` resource that was removed.", "This argument shall contain the value of the `Id` property of the `Chassis` resource in which the media controller was removed." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct MediaControllerModified :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    MediaControllerModified(Args... args) :
        RedfishMessage(
            "Fabric.1.0.2.MediaControllerModified",
            "Indicates that a media controller was modified.",
            "This message shall indicate that a resource of type `MediaController` was modified.",
            "Media controller '%1' in chassis '%2' was modified.",
            "OK",
            2,
            { "string", "string" },
            { "The `Id` of the media controller.", "The `Id` of the chassis." },
            { "This argument shall contain the value of the `Id` property of the `MediaController` resource that was modified.", "This argument shall contain the value of the `Id` property of the `Chassis` resource in which the media controller was modified." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace Fabric_1_0_2
}; // namespace registries
}; // namespace redfish
