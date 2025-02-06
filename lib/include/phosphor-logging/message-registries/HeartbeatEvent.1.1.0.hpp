#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace HeartbeatEvent_1_1_0
{

struct RedfishServiceFunctional :
    public RedfishMessage<>
{
    template <typename... Args>
    RedfishServiceFunctional(Args... args) :
        RedfishMessage(
            "HeartbeatEvent.1.1.0.RedfishServiceFunctional",
            "An event sent periodically upon request to indicate that the Redfish service is functional.",
            "This message shall indicate that the Redfish service is functional.  This message shall only be sent if specifically requested by an event destination during the creation of a subscription using the `HeartbeatIntervalMinutes` property.",
            "Redfish service is functional.",
            "OK",
            0,
            {},
            {},
            {},
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct RedfishServiceStarted :
    public RedfishMessage<>
{
    template <typename... Args>
    RedfishServiceStarted(Args... args) :
        RedfishMessage(
            "HeartbeatEvent.1.1.0.RedfishServiceStarted",
            "Indicates that the Redfish service started and is ready to accept requests.",
            "This message shall indicate that the Redfish service started and is fully functional.",
            "Redfish service started.",
            "OK",
            0,
            {},
            {},
            {},
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct RedfishServiceShuttingDown :
    public RedfishMessage<>
{
    template <typename... Args>
    RedfishServiceShuttingDown(Args... args) :
        RedfishMessage(
            "HeartbeatEvent.1.1.0.RedfishServiceShuttingDown",
            "Indicates that the Redfish service is shutting down and will no longer accept requests.",
            "This message shall indicate that the Redfish service is shutting down and is no longer able to service requests.",
            "Redfish service is shutting down.",
            "OK",
            0,
            {},
            {},
            {},
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace HeartbeatEvent_1_1_0
}; // namespace registries
}; // namespace redfish
