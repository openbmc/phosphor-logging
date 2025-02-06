#pragma once

#include "commit.hpp"

#include <nlohmann/json.hpp>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

// Template class to represent a Redfish message with variable number of
// arguments
template <typename... ArgsType> struct RedfishMessage
{
    // Enum to represent message severity levels
    enum class MessageSeverity
    {
        Critical,
        Ok,
        Warning,
    };

    MessageSeverity stringToMessageSeverity(const std::string& severity) const
    {
        if (severity == "Critical")
        {
            return MessageSeverity::Critical;
        }
        else if (severity == "OK")
        {
            return MessageSeverity::Ok;
        }
        else if (severity == "Warning")
        {
            return MessageSeverity::Warning;
        }
        else
        {
            throw std::invalid_argument(
                "Invalid message severity: " + severity);
        }
    }

    nlohmann::json to_json() const
    {
        nlohmann::json j;
        j["MessageID"] = MessageID;
        j["Description"] = Description;
        j["LongDescription"] = LongDescription;
        j["Message"] = Message;
        j["NumberOfArgs"] = NumberOfArgs;
        j["Resolution"] = Resolution;
        return j;
    }

    // Constructor to initialize the Redfish message with various parameters
    RedfishMessage(const char* messageID, const char* description,
                   const char* longDescription, std::string message,
                   const char* messageSeverity, int numberOfArgs,
                   std::vector<const char*> paramTypes,
                   std::vector<const char*> argDescriptions,
                   std::vector<const char*> argLongDescriptions,
                   const char* resolution, std::tuple<ArgsType...> args) :
        MessageID(messageID), Description(description),
        LongDescription(longDescription), Message(std::move(message)),
        MessageSeverity(stringToMessageSeverity(messageSeverity)),
        NumberOfArgs(numberOfArgs), ParamTypes(std::move(paramTypes)),
        ArgDescriptions(std::move(argDescriptions)),
        ArgLongDescriptions(std::move(argLongDescriptions)),
        Resolution(resolution), Args(std::move(args))
    {}

    static const char* MsgID()
    {
        return "BaseClass";
    }
    const char* MessageID;
    const char* Description;         // Short description of the message
    const char* LongDescription;     // Long description of the message
    std::string Message;             // The message template with placeholders
    MessageSeverity MessageSeverity; // Severity level of the message
    int NumberOfArgs;                // Number of arguments in the message
    std::vector<const char*> ParamTypes;      // Types of the arguments
    std::vector<const char*> ArgDescriptions; // Descriptions of the arguments
    std::vector<const char*>
        ArgLongDescriptions;      // Long descriptions of the arguments
    const char* Resolution;       // Resolution instructions for the message
    std::tuple<ArgsType...> Args; // Tuple containing the arguments

    // Function to get the message with arguments substituted
    std::string getMessageWithArgs() const
    {
        std::ostringstream
            oss;             // Output string stream to build the final message
        formatMessage(oss, Message,
                      Args); // Call the helper function to format the message
        return oss.str();    // Return the formatted message as a string
    }

  private:
    // Helper function to format the message (base case for recursion)
    template <std::size_t I = 0, typename... Args>
    typename std::enable_if<I == sizeof...(Args), void>::type
        formatMessage(std::ostringstream& oss, const std::string& message,
                      const std::tuple<Args...>& args) const
    {
        static_cast<void>(args); // Prevent warning about unused parameter
        oss << message;          // Append the remaining part of the message
    }

    // Helper function to format the message (recursive case)
    template <std::size_t I = 0, typename... Args>
        typename std::enable_if < I<sizeof...(Args), void>::type
        formatMessage(std::ostringstream& oss, const std::string& message,
                      const std::tuple<Args...>& args) const
    {
        // Find the position of the current placeholder (e.g., %1)
        size_t pos = message.find("%" + std::to_string(I + 1));
        if (pos != std::string::npos)
        {
            // Start position of the placeholder
            size_t start = pos;
            // End position of the placeholder (length of "%1", "%2", etc. is 2)
            size_t end = pos + 2;

            // Append the part of the message before the placeholder
            oss << message.substr(0, start);
            // Insert the argument corresponding to the placeholder
            insertArgument(oss, std::get<I>(args));

            // Recursively format the remaining part of the message
            formatMessage<I + 1>(oss, message.substr(end), args);
        }
        else
        {
            // If no more placeholders, append the remaining part of the message
            oss << message;
        }
    }

    // Helper function to insert an argument into the output stream
    template <typename T>
    void insertArgument(std::ostringstream& oss, T arg) const
    {
        if constexpr (std::is_same_v<T, std::string>)
        {
            oss << arg; // Insert string argument
        }
        else if constexpr (std::is_floating_point_v<T> || std::is_integral_v<T>)
        {
            oss << arg; // Insert floating-point or integral argument
        }
        else
        {
            static_assert(std::is_same_v<T, std::string> ||
                              std::is_floating_point_v<T> ||
                              std::is_integral_v<T>,
                          "Unsupported argument type in RedfishMessage");
        }
    }

  public:
    using Level =
        sdbusplus::common::xyz::openbmc_project::logging::Entry::Level;
    Level LogLevel() const
    {
        switch (MessageSeverity)
        {
            case MessageSeverity::Critical:
                return Level::Critical;
            case MessageSeverity::Ok:
                return Level::Informational;
            case MessageSeverity::Warning:
                return Level::Warning;
            default:
                throw std::invalid_argument("Invalid MessageSeverity value");
        }
    }
};

namespace lg2
{

template <typename... ArgsType>
auto commit(const RedfishMessage<ArgsType...>& t,
            const AdditionalData_t& data) -> sdbusplus::message::object_path
{
    AdditionalData_t messageData(data);
    messageData["MessageID"] = t.MessageID;
    return lg2::commit(t.getMessageWithArgs(), t.LogLevel(), messageData);
}

template <typename... ArgsType>
auto commit(const RedfishMessage<ArgsType...>& t)
    -> sdbusplus::message::object_path
{
    return commit(t, AdditionalData_t{});
}

} // namespace lg2
