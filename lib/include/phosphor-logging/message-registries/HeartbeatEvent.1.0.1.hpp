#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace HeartbeatEvent_1_0_1
{

struct [[deprecated ("Use HeartbeatEvent_1_1_0")]]
RedfishServiceFunctional :
    public RedfishMessage<>
{
    template <typename... Args>
    RedfishServiceFunctional(Args... args) :
        RedfishMessage(
            "HeartbeatEvent.1.0.1.RedfishServiceFunctional",
            "An event sent periodically upon request to indicates that the Redfish service is functional.",
            "This message shall indicate that the Redfish service is functional.  This message shall only be sent if specifically requested by an event destination during the creation of a subscription using the `SendHeartbeatMinutes` property.",
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

}; // namespace HeartbeatEvent_1_0_1
}; // namespace registries
}; // namespace redfish
