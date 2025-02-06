#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace LogService_1_0_0
{

struct [[deprecated ("Use LogService_1_0_1")]]
DiagnosticDataCollected :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    DiagnosticDataCollected(Args... args) :
        RedfishMessage(
            "LogService.1.0.0.DiagnosticDataCollected",
            "Indicates that diagnostic data has been collected due to a client invoking the `CollectDiagnosticData` action.",
            "This message shall be used to indicate that diagnostic data has been collected due to a client invoking the `CollectDiagnosticData` action.",
            "'%1' diagnostic data collected.",
            "OK",
            1,
            { "string" },
            { "The value of the `DiagnosticDataType` parameter." },
            { "The value of this argument shall be a string containing the `DiagnosticDataType` parameter provided by the client when the `CollectDiagnosticData` action was invoked." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace LogService_1_0_0
}; // namespace registries
}; // namespace redfish
