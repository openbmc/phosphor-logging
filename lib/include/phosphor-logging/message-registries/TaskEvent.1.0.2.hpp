#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace TaskEvent_1_0_2
{

struct [[deprecated ("Use TaskEvent_1_0_4")]]
TaskStarted :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    TaskStarted(Args... args) :
        RedfishMessage(
            "TaskEvent.1.0.2.TaskStarted",
            "A task has started.",
            "This message shall be used to indicate that a task has started.",
            "The task with Id '%1' has started.",
            "OK",
            1,
            { "string" },
            { "The `Id` of the task." },
            { "This argument shall contain the value of the `Id` property of the task resource that has started." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use TaskEvent_1_0_4")]]
TaskCompletedOK :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    TaskCompletedOK(Args... args) :
        RedfishMessage(
            "TaskEvent.1.0.2.TaskCompletedOK",
            "A task has completed.",
            "This message shall be used to indicate that a task has completed.",
            "The task with Id '%1' has completed.",
            "OK",
            1,
            { "string" },
            { "The `Id` of the task." },
            { "This argument shall contain the value of the `Id` property of the task resource that has completed." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use TaskEvent_1_0_4")]]
TaskCompletedWarning :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    TaskCompletedWarning(Args... args) :
        RedfishMessage(
            "TaskEvent.1.0.2.TaskCompletedWarning",
            "A task has completed with warnings.",
            "This message shall be used to indicate that a task has completed with warnings.",
            "The task with Id '%1' has completed with warnings.",
            "Warning",
            1,
            { "string" },
            { "The `Id` of the task." },
            { "This argument shall contain the value of the `Id` property of the task resource that has completed with warnings." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use TaskEvent_1_0_4")]]
TaskAborted :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    TaskAborted(Args... args) :
        RedfishMessage(
            "TaskEvent.1.0.2.TaskAborted",
            "A task has been aborted.",
            "This message shall be used to indicate that a task has been aborted.",
            "The task with Id '%1' has been aborted.",
            "Critical",
            1,
            { "string" },
            { "The `Id` of the task." },
            { "This argument shall contain the value of the `Id` property of the task resource that has been aborted." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use TaskEvent_1_0_4")]]
TaskCancelled :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    TaskCancelled(Args... args) :
        RedfishMessage(
            "TaskEvent.1.0.2.TaskCancelled",
            "A task has been cancelled.",
            "This message shall be used to indicate that a task has been cancelled.",
            "The task with Id '%1' has been cancelled.",
            "Warning",
            1,
            { "string" },
            { "The `Id` of the task." },
            { "This argument shall contain the value of the `Id` property of the task resource that has been cancelled." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use TaskEvent_1_0_4")]]
TaskRemoved :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    TaskRemoved(Args... args) :
        RedfishMessage(
            "TaskEvent.1.0.2.TaskRemoved",
            "A task has been removed.",
            "This message shall be used to indicate that a task has been removed.",
            "The task with Id '%1' has been removed.",
            "Warning",
            1,
            { "string" },
            { "The `Id` of the task." },
            { "This argument shall contain the value of the `Id` property of the task resource that has been removed." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use TaskEvent_1_0_4")]]
TaskPaused :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    TaskPaused(Args... args) :
        RedfishMessage(
            "TaskEvent.1.0.2.TaskPaused",
            "A task has been paused.",
            "This message shall be used to indicate that a task has been paused.",
            "The task with Id '%1' has been paused.",
            "Warning",
            1,
            { "string" },
            { "The `Id` of the task." },
            { "This argument shall contain the value of the `Id` property of the task resource that has been paused." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use TaskEvent_1_0_4")]]
TaskResumed :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    TaskResumed(Args... args) :
        RedfishMessage(
            "TaskEvent.1.0.2.TaskResumed",
            "A task has been resumed.",
            "This message shall be used to indicate that a task has been resumed.",
            "The task with Id '%1' has been resumed.",
            "OK",
            1,
            { "string" },
            { "The `Id` of the task." },
            { "This argument shall contain the value of the `Id` property of the task resource that has been resumed." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use TaskEvent_1_0_4")]]
TaskProgressChanged :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    TaskProgressChanged(Args... args) :
        RedfishMessage(
            "TaskEvent.1.0.2.TaskProgressChanged",
            "A task has changed progress.",
            "This message shall be used to indicate that a task has changed progress.",
            "The task with Id '%1' has changed to progress %2 percent complete.",
            "OK",
            2,
            { "string", "number" },
            { "The `Id` of the task.", "The percent completion of the task." },
            { "This argument shall contain the value of the `Id` property of the task resource that has changed progress.", "This argument shall contain the percent completion of the task that has changed progress." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace TaskEvent_1_0_2
}; // namespace registries
}; // namespace redfish
