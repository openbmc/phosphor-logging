#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace TaskEvent_1_0_4
{

struct TaskStarted :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    TaskStarted(Args... args) :
        RedfishMessage(
            "TaskEvent.1.0.4.TaskStarted",
            "A task has started.",
            "This message shall indicate that a task has started as the `TaskState` property transitions to `Starting` or `Running`.",
            "The task with Id '%1' has started.",
            "OK",
            1,
            { "string" },
            { "The `Id` of the task." },
            { "This argument shall contain the value of the `Id` property of the `Task` resource that has started." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TaskCompletedOK :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    TaskCompletedOK(Args... args) :
        RedfishMessage(
            "TaskEvent.1.0.4.TaskCompletedOK",
            "A task has completed.",
            "This message shall indicate that a task has completed and the `TaskState` property transitioned to `Completed`.",
            "The task with Id '%1' has completed.",
            "OK",
            1,
            { "string" },
            { "The `Id` of the task." },
            { "This argument shall contain the value of the `Id` property of the `Task` resource that has completed." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TaskCompletedWarning :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    TaskCompletedWarning(Args... args) :
        RedfishMessage(
            "TaskEvent.1.0.4.TaskCompletedWarning",
            "A task has completed with warnings.",
            "This message shall indicate that a task has completed with warnings and the `TaskState` property transitioned to `Completed`.",
            "The task with Id '%1' has completed with warnings.",
            "Warning",
            1,
            { "string" },
            { "The `Id` of the task." },
            { "This argument shall contain the value of the `Id` property of the `Task` resource that has completed with warnings." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TaskAborted :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    TaskAborted(Args... args) :
        RedfishMessage(
            "TaskEvent.1.0.4.TaskAborted",
            "A task has completed with errors.",
            "This message shall indicate that a task has completed with errors and the `TaskState` property transitioned to `Exception`.",
            "The task with Id '%1' has completed with errors.",
            "Critical",
            1,
            { "string" },
            { "The `Id` of the task." },
            { "This argument shall contain the value of the `Id` property of the `Task` resource that has completed with errors." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TaskCancelled :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    TaskCancelled(Args... args) :
        RedfishMessage(
            "TaskEvent.1.0.4.TaskCancelled",
            "A task was cancelled.",
            "This message shall indicate that work on a task has halted prior to completion due to an explicit request.  The `TaskState` property transitioned to `Cancelled`.",
            "Work on the task with Id '%1' was halted prior to completion due to an explicit request.",
            "Warning",
            1,
            { "string" },
            { "The `Id` of the task." },
            { "This argument shall contain the value of the `Id` property of the `Task` resource that was halted." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TaskRemoved :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    TaskRemoved(Args... args) :
        RedfishMessage(
            "TaskEvent.1.0.4.TaskRemoved",
            "A task was removed.",
            "This message shall indicate that a task was removed.",
            "The task with Id '%1' was removed.",
            "Warning",
            1,
            { "string" },
            { "The `Id` of the task." },
            { "This argument shall contain the value of the `Id` property of the `Task` resource that was removed." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TaskPaused :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    TaskPaused(Args... args) :
        RedfishMessage(
            "TaskEvent.1.0.4.TaskPaused",
            "A task was paused.",
            "This message shall indicate that a task was paused and the `TaskState` property transitioned to `Suspended` or `Interrupted`.",
            "The task with Id '%1' was paused.",
            "Warning",
            1,
            { "string" },
            { "The `Id` of the task." },
            { "This argument shall contain the value of the `Id` property of the `Task` resource that was paused." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TaskResumed :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    TaskResumed(Args... args) :
        RedfishMessage(
            "TaskEvent.1.0.4.TaskResumed",
            "A task was resumed.",
            "This message shall indicate that a task has resumed and the `TaskState` property transitioned to `Running` from `Suspended` or `Interrupted`.",
            "The task with Id '%1' was resumed.",
            "OK",
            1,
            { "string" },
            { "The `Id` of the task." },
            { "This argument shall contain the value of the `Id` property of the `Task` resource that was resumed." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct TaskProgressChanged :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    TaskProgressChanged(Args... args) :
        RedfishMessage(
            "TaskEvent.1.0.4.TaskProgressChanged",
            "A task has changed progress.",
            "This message shall indicate that a task has changed progress.",
            "The task with Id '%1' has changed to progress %2 percent complete.",
            "OK",
            2,
            { "string", "number" },
            { "The `Id` of the task.", "The percent completion of the task." },
            { "This argument shall contain the value of the `Id` property of the `Task` resource that has changed progress.", "This argument shall contain the percent completion of the task that has changed progress." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace TaskEvent_1_0_4
}; // namespace registries
}; // namespace redfish
