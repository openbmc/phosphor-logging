#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace LogService_1_0_1
{

struct DiagnosticDataCollected :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    DiagnosticDataCollected(Args... args) :
        RedfishMessage(
            "LogService.1.0.1.DiagnosticDataCollected",
            "Indicates that diagnostic data was collected due to a client invoking the `CollectDiagnosticData` action.",
            "This message shall indicate that diagnostic data was collected due to a client invoking the `CollectDiagnosticData` action.",
            "'%1' diagnostic data collected.",
            "OK",
            1,
            { "string" },
            { "The value of the `DiagnosticDataType` parameter." },
            { "This argument shall contain the value of the `DiagnosticDataType` parameter provided by the client when the `CollectDiagnosticData` action was invoked." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace LogService_1_0_1
}; // namespace registries
}; // namespace redfish
