#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace TaskEvent_1_0_1
{

struct [[deprecated ("Use TaskEvent_1_0_4")]]
TaskStarted :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    TaskStarted(Args... args) :
        RedfishMessage(
            "TaskEvent.1.0.1.TaskStarted",
            "The task with id %1 has started.",
            "The task with id %1 has started.",
            "The task with id %1 has started.",
            "OK",
            1,
            { "string" },
            {},
            {},
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
            "TaskEvent.1.0.1.TaskCompletedOK",
            "The task with id %1 has completed.",
            "The task with id %1 has completed.",
            "The task with id %1 has completed.",
            "OK",
            1,
            { "string" },
            {},
            {},
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
            "TaskEvent.1.0.1.TaskCompletedWarning",
            "The task with id %1 has completed with warnings.",
            "The task with id %1 has completed with warnings.",
            "The task with id %1 has completed with warnings.",
            "Warning",
            1,
            { "string" },
            {},
            {},
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
            "TaskEvent.1.0.1.TaskAborted",
            "The task with id %1 has been aborted.",
            "The task with id %1 has been aborted.",
            "The task with id %1 has been aborted.",
            "Critical",
            1,
            { "string" },
            {},
            {},
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
            "TaskEvent.1.0.1.TaskCancelled",
            "The task with id %1 has been cancelled.",
            "The task with id %1 has been cancelled.",
            "The task with id %1 has been cancelled.",
            "Warning",
            1,
            { "string" },
            {},
            {},
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
            "TaskEvent.1.0.1.TaskRemoved",
            "The task with id %1 has been removed.",
            "The task with id %1 has been removed.",
            "The task with id %1 has been removed.",
            "Warning",
            1,
            { "string" },
            {},
            {},
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
            "TaskEvent.1.0.1.TaskPaused",
            "The task with id %1 has been paused.",
            "The task with id %1 has been paused.",
            "The task with id %1 has been paused.",
            "Warning",
            1,
            { "string" },
            {},
            {},
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
            "TaskEvent.1.0.1.TaskResumed",
            "The task with id %1 has been resumed.",
            "The task with id %1 has been resumed.",
            "The task with id %1 has been resumed.",
            "OK",
            1,
            { "string" },
            {},
            {},
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
            "TaskEvent.1.0.1.TaskProgressChanged",
            "The task with id %1 has changed to progress %2 percent complete.",
            "The task with id %1 has changed to progress %2 percent complete.",
            "The task with id %1 has changed to progress %2 percent complete.",
            "OK",
            2,
            { "string", "number" },
            {},
            {},
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace TaskEvent_1_0_1
}; // namespace registries
}; // namespace redfish
