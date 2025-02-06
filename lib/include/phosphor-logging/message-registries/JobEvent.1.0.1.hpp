#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace JobEvent_1_0_1
{

struct JobStarted :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    JobStarted(Args... args) :
        RedfishMessage(
            "JobEvent.1.0.1.JobStarted",
            "A job has started.",
            "This message shall indicate that a job has started as the `JobState` property transitions to `Starting` or `Running`.",
            "The job with Id '%1' has started.",
            "OK",
            1,
            { "string" },
            { "The `Id` of the job that started." },
            { "This argument shall contain the value of the `Id` property of the `Job` resource that has started." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct JobCompletedOK :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    JobCompletedOK(Args... args) :
        RedfishMessage(
            "JobEvent.1.0.1.JobCompletedOK",
            "A job has completed.",
            "This message shall indicate that a job has completed and the `JobState` property transitioned to `Completed`.",
            "The job with Id '%1' has completed.",
            "OK",
            1,
            { "string" },
            { "The `Id` of the job." },
            { "This argument shall contain the value of the `Id` property of the `Job` resource that has completed." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct JobCompletedException :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    JobCompletedException(Args... args) :
        RedfishMessage(
            "JobEvent.1.0.1.JobCompletedException",
            "A job has completed with warnings or errors.",
            "This message shall indicate that a job has completed and the `JobState` property transitioned to `Exception`.",
            "The job with Id '%1' has completed with warnings or errors.",
            "Warning",
            1,
            { "string" },
            { "The `Id` of the job." },
            { "This argument shall contain the value of the `Id` property of the `Job` resource that has completed with warnings or errors." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct JobCancelled :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    JobCancelled(Args... args) :
        RedfishMessage(
            "JobEvent.1.0.1.JobCancelled",
            "A job was cancelled.",
            "This message shall indicate that a job was cancelled and the `JobState` property transitioned to `Cancelled`.",
            "The job with Id '%1' was cancelled.",
            "Warning",
            1,
            { "string" },
            { "The `Id` of the job." },
            { "This argument shall contain the value of the `Id` property of the `Job` resource that was cancelled." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct JobRemoved :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    JobRemoved(Args... args) :
        RedfishMessage(
            "JobEvent.1.0.1.JobRemoved",
            "A job was removed.",
            "This message shall indicate that a job was removed.",
            "The job with Id '%1' was removed.",
            "Warning",
            1,
            { "string" },
            { "The `Id` of the job." },
            { "This argument shall contain the value of the `Id` property of the `Job` resource that was removed." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct JobSuspended :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    JobSuspended(Args... args) :
        RedfishMessage(
            "JobEvent.1.0.1.JobSuspended",
            "A job was suspended.",
            "This message shall indicate that a job was suspended and the `JobState` property transitioned to `Suspended`, `Interrupted`, or `UserIntervention`.",
            "The job with Id '%1' was suspended.",
            "Warning",
            1,
            { "string" },
            { "The `Id` of the job." },
            { "This argument shall contain the value of the `Id` property of the `Job` resource that was suspended." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct JobResumed :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    JobResumed(Args... args) :
        RedfishMessage(
            "JobEvent.1.0.1.JobResumed",
            "A job has resumed.",
            "This message shall indicate that a job has resumed and the `JobState` property transitioned to `Running` from `Suspended`, `Interrupted`, or `UserIntervention`.",
            "The job with Id '%1' has resumed.",
            "OK",
            1,
            { "string" },
            { "The `Id` of the job." },
            { "This argument shall contain the value of the `Id` property of the `Job` resource that has resumed." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct JobProgressChanged :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    JobProgressChanged(Args... args) :
        RedfishMessage(
            "JobEvent.1.0.1.JobProgressChanged",
            "A job has changed progress.",
            "This message shall indicate that a job has changed progress.",
            "The job with Id '%1' has changed to progress %2 percent complete.",
            "OK",
            2,
            { "string", "number" },
            { "The `Id` of the job.", "The percent completion of the job." },
            { "This argument shall contain the value of the `Id` property of the `Job` resource that has changed progress.", "This argument shall contain the percent completion of the job that has changed progress and shall be a value from 0 to 100." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace JobEvent_1_0_1
}; // namespace registries
}; // namespace redfish
