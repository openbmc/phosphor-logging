#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace JobEvent_1_0_0
{

struct [[deprecated ("Use JobEvent_1_0_1")]]
JobStarted :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    JobStarted(Args... args) :
        RedfishMessage(
            "JobEvent.1.0.0.JobStarted",
            "A job has started.",
            "This message shall be used to indicate that a job has started as the `JobState` property transitions to `Starting` or `Running`.",
            "The job with Id '%1' has started.",
            "OK",
            1,
            { "string" },
            { "The `Id` of the job that started." },
            { "This argument shall contain the value of the `Id` property of the job resource that has started." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use JobEvent_1_0_1")]]
JobCompletedOK :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    JobCompletedOK(Args... args) :
        RedfishMessage(
            "JobEvent.1.0.0.JobCompletedOK",
            "A job has completed.",
            "This message shall be used to indicate that a job has completed and the `JobState` property transitioned to `Completed`.",
            "The job with Id '%1' has completed.",
            "OK",
            1,
            { "string" },
            { "The `Id` of the job." },
            { "This argument shall contain the value of the `Id` property of the job resource that has completed." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use JobEvent_1_0_1")]]
JobCompletedException :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    JobCompletedException(Args... args) :
        RedfishMessage(
            "JobEvent.1.0.0.JobCompletedException",
            "A job has completed with warnings or errors.",
            "This message shall be used to indicate that a job has completed and the `JobState` property transitioned to `Exception`.",
            "The job with Id '%1' has completed with warnings or errors.",
            "Warning",
            1,
            { "string" },
            { "The `Id` of the job." },
            { "This argument shall contain the value of the `Id` property of the job resource that has completed with warnings or errors." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use JobEvent_1_0_1")]]
JobCancelled :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    JobCancelled(Args... args) :
        RedfishMessage(
            "JobEvent.1.0.0.JobCancelled",
            "A job has been cancelled.",
            "This message shall be used to indicate that a job has been cancelled and the `JobState` property transitioned to `Cancelled`.",
            "The job with Id '%1' has been cancelled.",
            "Warning",
            1,
            { "string" },
            { "The `Id` of the job." },
            { "This argument shall contain the value of the `Id` property of the job resource that has been cancelled." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use JobEvent_1_0_1")]]
JobRemoved :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    JobRemoved(Args... args) :
        RedfishMessage(
            "JobEvent.1.0.0.JobRemoved",
            "A job has been removed.",
            "This message shall be used to indicate that a job has been removed.",
            "The job with Id '%1' has been removed.",
            "Warning",
            1,
            { "string" },
            { "The `Id` of the job." },
            { "This argument shall contain the value of the `Id` property of the job resource that has been removed." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use JobEvent_1_0_1")]]
JobSuspended :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    JobSuspended(Args... args) :
        RedfishMessage(
            "JobEvent.1.0.0.JobSuspended",
            "A job has been susepended.",
            "This message shall be used to indicate that a job has been susepended and the `JobState` property transitioned to `Suspended`, `Interrupted`, or `UserIntervention`.",
            "The job with Id '%1' has been susepended.",
            "Warning",
            1,
            { "string" },
            { "The `Id` of the job." },
            { "This argument shall contain the value of the `Id` property of the job resource that has been susepended." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use JobEvent_1_0_1")]]
JobResumed :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    JobResumed(Args... args) :
        RedfishMessage(
            "JobEvent.1.0.0.JobResumed",
            "A job has resumed.",
            "This message shall be used to indicate that a job has resumed and the `JobState` property transitioned to `Running` from `Suspended`, `Interrupted`, or `UserIntervention`.",
            "The job with Id '%1' has resumed.",
            "OK",
            1,
            { "string" },
            { "The `Id` of the job." },
            { "This argument shall contain the value of the `Id` property of the job resource that has resumed." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use JobEvent_1_0_1")]]
JobProgressChanged :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    JobProgressChanged(Args... args) :
        RedfishMessage(
            "JobEvent.1.0.0.JobProgressChanged",
            "A job has changed progress.",
            "This message shall be used to indicate that a job has changed progress.",
            "The job with Id '%1' has changed to progress %2 percent complete.",
            "OK",
            2,
            { "string", "number" },
            { "The `Id` of the job.", "The percent completion of the job." },
            { "This argument shall contain the value of the `Id` property of the job resource that has changed progress.", "This argument shall contain the percent completion of the job that has changed progress and shall be a value from 0 to 100." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace JobEvent_1_0_0
}; // namespace registries
}; // namespace redfish
