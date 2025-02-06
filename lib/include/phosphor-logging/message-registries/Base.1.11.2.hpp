#pragma once
#include "../message-registries.hpp"

namespace redfish
{
namespace registries
{
namespace Base_1_11_2
{

struct [[deprecated ("Use Base_1_20_0")]]
Success :
    public RedfishMessage<>
{
    template <typename... Args>
    Success(Args... args) :
        RedfishMessage(
            "Base.1.11.2.Success",
            "Indicates that all conditions of a successful operation have been met.",
            "This message shall be used to indicate that all conditions of a successful operation have been met.",
            "The request completed successfully.",
            "OK",
            0,
            {},
            {},
            {},
            "None",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
GeneralError :
    public RedfishMessage<>
{
    template <typename... Args>
    GeneralError(Args... args) :
        RedfishMessage(
            "Base.1.11.2.GeneralError",
            "Indicates that a general error has occurred.  Use in `@Message.ExtendedInfo` is discouraged.  When used in `@Message.ExtendedInfo`, implementations are expected to include a `Resolution` property with this message and provide a service-defined resolution to indicate how to resolve the error.",
            "This message shall be used to indicate that a general error has occurred.  Implementations should not use this message in `@Message.ExtendedInfo`.  If used in `@Message.ExtendedInfo`, implementations should include a `Resolution` property with this message and provide a service-defined resolution to indicate how to resolve the error.",
            "A general error has occurred.  See Resolution for information on how to resolve the error, or @Message.ExtendedInfo if Resolution is not provided.",
            "Critical",
            0,
            {},
            {},
            {},
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
Created :
    public RedfishMessage<>
{
    template <typename... Args>
    Created(Args... args) :
        RedfishMessage(
            "Base.1.11.2.Created",
            "Indicates that all conditions of a successful create operation have been met.",
            "This message shall be used to indicate that all conditions of a successful create operation have been met.",
            "The resource has been created successfully.",
            "OK",
            0,
            {},
            {},
            {},
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
NoOperation :
    public RedfishMessage<>
{
    template <typename... Args>
    NoOperation(Args... args) :
        RedfishMessage(
            "Base.1.11.2.NoOperation",
            "Indicates that the requested operation will not perform any changes on the service.",
            "This message shall be used to indicate that the requested operation will not perform any changes on the service.",
            "The request body submitted contain no data to act upon and no changes to the resource took place.",
            "Warning",
            0,
            {},
            {},
            {},
            "Add properties in the JSON object and resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
PropertyDuplicate :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PropertyDuplicate(Args... args) :
        RedfishMessage(
            "Base.1.11.2.PropertyDuplicate",
            "Indicates that a duplicate property was included in the request body.",
            "This message shall be used to indicate that a duplicate property was included in the request body.",
            "The property %1 was duplicated in the request.",
            "Warning",
            1,
            { "string" },
            { "The name of the duplicate property." },
            { "This argument shall contain the name of the duplicate property." },
            "Remove the duplicate property from the request body and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
PropertyUnknown :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PropertyUnknown(Args... args) :
        RedfishMessage(
            "Base.1.11.2.PropertyUnknown",
            "Indicates that an unknown property was included in the request body.",
            "This message shall be used to indicate that an unknown property was included in the request body.",
            "The property %1 is not in the list of valid properties for the resource.",
            "Warning",
            1,
            { "string" },
            { "The name of the unknown property." },
            { "This argument shall contain the name of the unknown property." },
            "Remove the unknown property from the request body and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
PropertyValueTypeError :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    PropertyValueTypeError(Args... args) :
        RedfishMessage(
            "Base.1.11.2.PropertyValueTypeError",
            "Indicates that a property was given the wrong value type, such as when a number is supplied for a property that requires a string.",
            "This message shall be used to indicate that a property was given the wrong value type.",
            "The value '%1' for the property %2 is of a different type than the property can accept.",
            "Warning",
            2,
            { "string", "string" },
            { "The value provided for the property.", "The name of the property." },
            { "This argument shall contain the value provided for the property.  Numeric values shall be converted to a string, and null values shall be represented by the string `null`.", "This argument shall contain the name of the property." },
            "Correct the value for the property in the request body and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
PropertyValueFormatError :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    PropertyValueFormatError(Args... args) :
        RedfishMessage(
            "Base.1.11.2.PropertyValueFormatError",
            "Indicates that a property was given the correct value type but the value of that property was not supported.",
            "This message shall be used to indicate that a property was given the correct value type but the value of that property was not supported.",
            "The value '%1' for the property %2 is of a different format than the property can accept.",
            "Warning",
            2,
            { "string", "string" },
            { "The value provided for the property.", "The name of the property." },
            { "This argument shall contain the value provided for the property.  Numeric values shall be converted to a string, and null values shall be represented by the string `null`.", "This argument shall contain the name of the property." },
            "Correct the value for the property in the request body and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
PropertyValueNotInList :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    PropertyValueNotInList(Args... args) :
        RedfishMessage(
            "Base.1.11.2.PropertyValueNotInList",
            "Indicates that a property was given the correct value type but the value of that property was not supported.  The value is not in an enumeration.",
            "This message shall be used to indicate that a property was given the correct value type but the value of that property was not supported.",
            "The value '%1' for the property %2 is not in the list of acceptable values.",
            "Warning",
            2,
            { "string", "string" },
            { "The value provided for the property.", "The name of the property." },
            { "This argument shall contain the value provided for the property.", "This argument shall contain the name of the property." },
            "Choose a value from the enumeration list that the implementation can support and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
PropertyValueOutOfRange :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    PropertyValueOutOfRange(Args... args) :
        RedfishMessage(
            "Base.1.11.2.PropertyValueOutOfRange",
            "Indicates that a property was given the correct value type but the value of that property is outside the supported range.",
            "This value shall be used to indicate that a property was given the correct value type but the value of that property is outside the supported range.",
            "The value '%1' for the property %2 is not in the supported range of acceptable values.",
            "Warning",
            2,
            { "string", "string" },
            { "The value provided for the property.", "The name of the property." },
            { "This argument shall contain the value provided for the property.", "This argument shall contain the name of the property." },
            "Correct the value for the property in the request body and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
PropertyValueError :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PropertyValueError(Args... args) :
        RedfishMessage(
            "Base.1.11.2.PropertyValueError",
            "Indicates that a property was given an invalid value.",
            "This message shall be used to indicate that a property was given an invalid value, and the value cannot or should not be included in the message text.",
            "The value provided for the property %1 is not valid.",
            "Warning",
            1,
            { "string" },
            { "The name of the property." },
            { "This argument shall contain the name of the property." },
            "Correct the value for the property in the request body and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
PropertyNotWritable :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PropertyNotWritable(Args... args) :
        RedfishMessage(
            "Base.1.11.2.PropertyNotWritable",
            "Indicates that a property was given a value in the request body, but the property is a readonly property.",
            "This message shall be used to indicate that a property was given a value in the request body, but the property is a readonly property.",
            "The property %1 is a read only property and cannot be assigned a value.",
            "Warning",
            1,
            { "string" },
            { "The name of the property." },
            { "This argument shall contain the name of the property." },
            "Remove the property from the request body and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
PropertyMissing :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PropertyMissing(Args... args) :
        RedfishMessage(
            "Base.1.11.2.PropertyMissing",
            "Indicates that a required property was not supplied as part of the request.",
            "This message shall be used to indicate that a required property was not supplied as part of the request.",
            "The property %1 is a required property and must be included in the request.",
            "Warning",
            1,
            { "string" },
            { "The name of the property." },
            { "This argument shall contain the name of the property." },
            "Ensure that the property is in the request body and has a valid value and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
MalformedJSON :
    public RedfishMessage<>
{
    template <typename... Args>
    MalformedJSON(Args... args) :
        RedfishMessage(
            "Base.1.11.2.MalformedJSON",
            "Indicates that the request body was malformed JSON.",
            "This message shall be used to indicate that the request body was malformed JSON.",
            "The request body submitted was malformed JSON and could not be parsed by the receiving service.",
            "Critical",
            0,
            {},
            {},
            {},
            "Ensure that the request body is valid JSON and resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
InvalidJSON :
    public RedfishMessage<double>
{
    template <typename... Args>
    InvalidJSON(Args... args) :
        RedfishMessage(
            "Base.1.11.2.InvalidJSON",
            "Indicates that the request body contains invalid JSON.",
            "This message shall be used to indicate that the request body contains invalid JSON.",
            "The request body submitted is invalid JSON starting at line %1 and could not be parsed by the receiving service.",
            "Critical",
            1,
            { "number" },
            { "The line number where the invalid JSON starts." },
            { "The value of this argument shall contain the line number where the invalid JSON starts." },
            "Ensure that the request body is valid JSON and resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
EmptyJSON :
    public RedfishMessage<>
{
    template <typename... Args>
    EmptyJSON(Args... args) :
        RedfishMessage(
            "Base.1.11.2.EmptyJSON",
            "Indicates that the request body contained an empty JSON object when one or more properties are expected in the body.",
            "This message shall be used to indicate that the request body contained an empty JSON object when one or more properties are expected in the body.",
            "The request body submitted contained an empty JSON object and the service is unable to process it.",
            "Warning",
            0,
            {},
            {},
            {},
            "Add properties in the JSON object and resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ActionNotSupported :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ActionNotSupported(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ActionNotSupported",
            "Indicates that the action supplied with the POST operation is not supported by the resource.",
            "This message shall be used to indicate that the action supplied with the POST operation is not supported by the resource.",
            "The action %1 is not supported by the resource.",
            "Critical",
            1,
            { "string" },
            { "The name of the action." },
            { "This argument shall contain the name of the action." },
            "The action supplied cannot be resubmitted to the implementation.  Perhaps the action was invalid, the wrong resource was the target or the implementation documentation may be of assistance.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ActionParameterMissing :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ActionParameterMissing(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ActionParameterMissing",
            "Indicates that the action requested was missing a parameter that is required to process the action.",
            "This message shall be used to indicate that the action requested was missing a parameter that is required to process the action.",
            "The action %1 requires the parameter %2 to be present in the request body.",
            "Critical",
            2,
            { "string", "string" },
            { "The name of the action.", "The name of the action parameter." },
            { "This argument shall contain the name of the action.", "This argument shall contain the name of the action parameter." },
            "Supply the action with the required parameter in the request body when the request is resubmitted.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ActionParameterDuplicate :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ActionParameterDuplicate(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ActionParameterDuplicate",
            "Indicates that the action was supplied with a duplicated parameter in the request body.",
            "This message shall be used to indicate that the action was supplied with a duplicated parameter in the request body.",
            "The action %1 was submitted with more than one value for the parameter %2.",
            "Warning",
            2,
            { "string", "string" },
            { "The name of the action.", "The name of the action parameter." },
            { "This argument shall contain the name of the action.", "This argument shall contain the name of the action parameter." },
            "Resubmit the action with only one instance of the parameter in the request body if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ActionParameterUnknown :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ActionParameterUnknown(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ActionParameterUnknown",
            "Indicates that an action was submitted but a parameter supplied did not match any of the known parameters.",
            "This message shall be used to indicate that an action was submitted but a parameter supplied did not match any of the known parameters.",
            "The action %1 was submitted with the invalid parameter %2.",
            "Warning",
            2,
            { "string", "string" },
            { "The name of the action.", "The name of the action parameter." },
            { "This argument shall contain the name of the action.", "This argument shall contain the name of the action parameter." },
            "Correct the invalid parameter and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ActionParameterValueTypeError :
    public RedfishMessage<std::string, std::string, std::string>
{
    template <typename... Args>
    ActionParameterValueTypeError(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ActionParameterValueTypeError",
            "Indicates that a parameter was given the wrong value type, such as when a number is supplied for a parameter that requires a string.",
            "This message shall be used to indicate that a parameter was given the wrong value type, such as when a number is supplied for a parameter that requires a string.",
            "The value '%1' for the parameter %2 in the action %3 is of a different type than the parameter can accept.",
            "Warning",
            3,
            { "string", "string", "string" },
            { "The value provided for the action parameter.", "The name of the action parameter.", "The name of the action." },
            { "This argument shall contain the value provided for the action parameter.  Numeric values shall be converted to a string, and null values shall be represented by the string `null`.", "This argument shall contain the name of the action parameter.", "This argument shall contain the name of the action." },
            "Correct the value for the parameter in the request body and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ActionParameterValueFormatError :
    public RedfishMessage<std::string, std::string, std::string>
{
    template <typename... Args>
    ActionParameterValueFormatError(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ActionParameterValueFormatError",
            "Indicates that a parameter was given the correct value type but the value of that parameter was not supported.  This includes the value size or length has been exceeded.",
            "This message shall be used to indicate that a parameter was given the correct value type but the value of that parameter was not supported.",
            "The value '%1' for the parameter %2 in the action %3 is of a different format than the parameter can accept.",
            "Warning",
            3,
            { "string", "string", "string" },
            { "The value provided for the action parameter.", "The name of the action parameter.", "The name of the action." },
            { "This argument shall contain the value provided for the action parameter.  Numeric values shall be converted to a string, and null values shall be represented by the string `null`.", "This argument shall contain the name of the action parameter.", "This argument shall contain the name of the action." },
            "Correct the value for the parameter in the request body and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ActionParameterValueNotInList :
    public RedfishMessage<std::string, std::string, std::string>
{
    template <typename... Args>
    ActionParameterValueNotInList(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ActionParameterValueNotInList",
            "Indicates that a parameter was given the correct value type but the value of that parameter was not supported.  The value is not in an enumeration.",
            "This message shall be used to indicate that a parameter was given the correct value type but the value of that parameter was not supported.",
            "The value '%1' for the parameter %2 in the action %3 is not in the list of acceptable values.",
            "Warning",
            3,
            { "string", "string", "string" },
            { "The value provided for the action parameter.", "The name of the action parameter.", "The name of the action." },
            { "This argument shall contain the value provided for the action parameter.", "This argument shall contain the name of the action parameter.", "This argument shall contain the name of the action." },
            "Choose a value from the enumeration list that the implementation can support and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ActionParameterValueError :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ActionParameterValueError(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ActionParameterValueError",
            "Indicates that a parameter was given an invalid value.",
            "This message shall be used to indicate that a parameter was given an invalid value, and the value cannot or should not be included in the message.",
            "The value for the parameter %1 in the action %2 is invalid.",
            "Warning",
            2,
            { "string", "string" },
            { "The name of the action parameter.", "The name of the action." },
            { "This argument shall contain the name of the action parameter.", "This argument shall contain the name of the action." },
            "Correct the value for the parameter in the request body and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ActionParameterNotSupported :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ActionParameterNotSupported(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ActionParameterNotSupported",
            "Indicates that the parameter supplied for the action is not supported on the resource.",
            "This message shall be used to indicate that the parameter supplied for the action is not supported on the resource.",
            "The parameter %1 for the action %2 is not supported on the target resource.",
            "Warning",
            2,
            { "string", "string" },
            { "The name of the action parameter.", "The name of the action." },
            { "This argument shall contain the name of the action parameter.", "This argument shall contain the name of the action." },
            "Remove the parameter supplied and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
QueryParameterValueTypeError :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    QueryParameterValueTypeError(Args... args) :
        RedfishMessage(
            "Base.1.11.2.QueryParameterValueTypeError",
            "Indicates that a query parameter was given the wrong value type, such as when a number is supplied for a query parameter that requires a string.",
            "This message shall be used to indicate that a query parameter was given the wrong value type, such as when a number is supplied for a query parameter that requires a string.",
            "The value '%1' for the query parameter %2 is of a different type than the parameter can accept.",
            "Warning",
            2,
            { "string", "string" },
            { "The value provided for the query parameter.", "The query parameter." },
            { "This argument shall contain the value provided for the query parameter.", "This argument shall contain the name of the query parameter." },
            "Correct the value for the query parameter in the request and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
QueryParameterValueFormatError :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    QueryParameterValueFormatError(Args... args) :
        RedfishMessage(
            "Base.1.11.2.QueryParameterValueFormatError",
            "Indicates that a query parameter was given the correct value type but the value of that parameter was not supported.  This includes the value size or length has been exceeded.",
            "This message shall be used to indicate that a query parameter was given the correct value type but the value of that parameter was not supported.",
            "The value '%1' for the parameter %2 is of a different format than the parameter can accept.",
            "Warning",
            2,
            { "string", "string" },
            { "The value provided for the query parameter.", "The query parameter." },
            { "This argument shall contain the value provided for the query parameter.", "This argument shall contain the name of the query parameter." },
            "Correct the value for the query parameter in the request and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
QueryParameterValueError :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    QueryParameterValueError(Args... args) :
        RedfishMessage(
            "Base.1.11.2.QueryParameterValueError",
            "Indicates that a query parameter was given an invalid value.",
            "This message shall be used to indicate that a query parameter was given an invalid value that can not or should not be included in the message.",
            "The value for the parameter %1 is invalid.",
            "Warning",
            1,
            { "string" },
            { "The query parameter." },
            { "This argument shall contain the name of the query parameter." },
            "Correct the value for the query parameter in the request and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
QueryParameterOutOfRange :
    public RedfishMessage<std::string, std::string, std::string>
{
    template <typename... Args>
    QueryParameterOutOfRange(Args... args) :
        RedfishMessage(
            "Base.1.11.2.QueryParameterOutOfRange",
            "Indicates that a query parameter was provided that is out of range for the given resource.  This can happen with values that are too low or beyond that possible for the supplied resource, such as when a page is requested that is beyond the last page.",
            "This message shall be used to indicate that a query parameter was provided that is out of range for the given resource.",
            "The value '%1' for the query parameter %2 is out of range %3.",
            "Warning",
            3,
            { "string", "string", "string" },
            { "The value provided for the query parameter.", "The query parameter.", "The valid range for the query parameter." },
            { "This argument shall contain the value provided for the query parameter.", "This argument shall contain the name of the query parameter.", "This argument shall contain a string representation of the valid range of values supported for the query parameter." },
            "Reduce the value for the query parameter to a value that is within range, such as a start or count value that is within bounds of the number of resources in a collection or a page that is within the range of valid pages.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
QueryNotSupportedOnResource :
    public RedfishMessage<>
{
    template <typename... Args>
    QueryNotSupportedOnResource(Args... args) :
        RedfishMessage(
            "Base.1.11.2.QueryNotSupportedOnResource",
            "Indicates that query is not supported on the given resource, such as when the `$skip` query is attempted on a resource that is not a collection.",
            "This message shall be used to indicate that query is not supported on the given resource.",
            "Querying is not supported on the requested resource.",
            "Warning",
            0,
            {},
            {},
            {},
            "Remove the query parameters and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
QueryNotSupportedOnOperation :
    public RedfishMessage<>
{
    template <typename... Args>
    QueryNotSupportedOnOperation(Args... args) :
        RedfishMessage(
            "Base.1.11.2.QueryNotSupportedOnOperation",
            "Indicates that query is not supported with the given operation, such as when the `$expand` query is attempted with a PATCH operation.",
            "This message shall be used to indicate that query is not supported with the given operation.",
            "Querying is not supported with the requested operation.",
            "Warning",
            0,
            {},
            {},
            {},
            "Remove the query parameters and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
QueryNotSupported :
    public RedfishMessage<>
{
    template <typename... Args>
    QueryNotSupported(Args... args) :
        RedfishMessage(
            "Base.1.11.2.QueryNotSupported",
            "Indicates that query is not supported on the implementation.",
            "This message shall be used to indicate that query is not supported on the implementation.",
            "Querying is not supported by the implementation.",
            "Warning",
            0,
            {},
            {},
            {},
            "Remove the query parameters and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
QueryCombinationInvalid :
    public RedfishMessage<>
{
    template <typename... Args>
    QueryCombinationInvalid(Args... args) :
        RedfishMessage(
            "Base.1.11.2.QueryCombinationInvalid",
            "Indicates the request contains multiple query parameters, and that two or more of them cannot be used together.",
            "This message shall be used to indicate that the request contains multiple query parameters, and that two or more of them cannot be used together.",
            "Two or more query parameters in the request cannot be used together.",
            "Warning",
            0,
            {},
            {},
            {},
            "Remove one or more of the query parameters and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
SessionLimitExceeded :
    public RedfishMessage<>
{
    template <typename... Args>
    SessionLimitExceeded(Args... args) :
        RedfishMessage(
            "Base.1.11.2.SessionLimitExceeded",
            "Indicates that a session establishment has been requested but the operation failed due to the number of simultaneous sessions exceeding the limit of the implementation.",
            "This message shall be used to indicate that a session establishment has been requested but the operation failed due to the number of simultaneous sessions exceeding the limit of the implementation.",
            "The session establishment failed due to the number of simultaneous sessions exceeding the limit of the implementation.",
            "Critical",
            0,
            {},
            {},
            {},
            "Reduce the number of other sessions before trying to establish the session or increase the limit of simultaneous sessions, if supported.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
EventSubscriptionLimitExceeded :
    public RedfishMessage<>
{
    template <typename... Args>
    EventSubscriptionLimitExceeded(Args... args) :
        RedfishMessage(
            "Base.1.11.2.EventSubscriptionLimitExceeded",
            "Indicates that a event subscription establishment has been requested but the operation failed due to the number of simultaneous connection exceeding the limit of the implementation.",
            "This message shall be used to indicate that a event subscription establishment has been requested but the operation failed due to the number of simultaneous connection exceeding the limit of the implementation.",
            "The event subscription failed due to the number of simultaneous subscriptions exceeding the limit of the implementation.",
            "Critical",
            0,
            {},
            {},
            {},
            "Reduce the number of other subscriptions before trying to establish the event subscription or increase the limit of simultaneous subscriptions, if supported.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ResourceCannotBeDeleted :
    public RedfishMessage<>
{
    template <typename... Args>
    ResourceCannotBeDeleted(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ResourceCannotBeDeleted",
            "Indicates that a delete operation was attempted on a resource that cannot be deleted.",
            "This message shall be used to indicate that a delete operation was attempted on a resource that cannot be deleted.",
            "The delete request failed because the resource requested cannot be deleted.",
            "Critical",
            0,
            {},
            {},
            {},
            "Do not attempt to delete a non-deletable resource.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ResourceInUse :
    public RedfishMessage<>
{
    template <typename... Args>
    ResourceInUse(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ResourceInUse",
            "Indicates that a change was requested to a resource but the change was rejected due to the resource being in use or transition.",
            "This message shall be used to indicate that a change was requested to a resource but the change was rejected due to the resource being in use or transition.",
            "The change to the requested resource failed because the resource is in use or in transition.",
            "Warning",
            0,
            {},
            {},
            {},
            "Remove the condition and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ResourceAlreadyExists :
    public RedfishMessage<std::string, std::string, std::string>
{
    template <typename... Args>
    ResourceAlreadyExists(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ResourceAlreadyExists",
            "Indicates that a resource change or creation was attempted but that the operation cannot proceed because the resource already exists.",
            "This message shall be used to indicate that a resource change or creation was attempted but that the operation cannot proceed because the resource already exists.",
            "The requested resource of type %1 with the property %2 with the value '%3' already exists.",
            "Critical",
            3,
            { "string", "string", "string" },
            { "The type of resource.", "The name of the property.", "The value of the property." },
            { "This argument shall contain the schema name of the resource.", "This argument shall contain the name of the property.", "This argument shall contain the value provided for the property.  Numeric values shall be converted to a string, and null values shall be represented by the string `null`." },
            "Do not repeat the create operation as the resource has already been created.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ResourceNotFound :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ResourceNotFound(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ResourceNotFound",
            "Indicates that the operation expected a resource identifier that corresponds to an existing resource but one was not found.",
            "This message shall be used to indicate that the operation expected a resource identifier that corresponds to an existing resource but one was not found.",
            "The requested resource of type %1 named '%2' was not found.",
            "Critical",
            2,
            { "string", "string" },
            { "The type of resource.", "The `Id` of the resource." },
            { "This argument shall contain the schema name of the resource.", "This argument shall contain the value of the `Id` property of the requested resource." },
            "Provide a valid resource identifier and resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
InvalidURI :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    InvalidURI(Args... args) :
        RedfishMessage(
            "Base.1.11.2.InvalidURI",
            "Indicates that the operation encountered a URI that does not correspond to a valid resource.",
            "This message shall be used to indicate that the operation encountered a URI that does not correspond to a valid resource.",
            "The URI %1 was not found.",
            "Critical",
            1,
            { "string" },
            { "The supplied URI." },
            { "This argument shall contain the invalid URI that was encountered." },
            "Provide a valid URI and resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
CreateFailedMissingReqProperties :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    CreateFailedMissingReqProperties(Args... args) :
        RedfishMessage(
            "Base.1.11.2.CreateFailedMissingReqProperties",
            "Indicates that a create was attempted on a resource but that properties that are required for the create operation were missing from the request.",
            "This message shall be used to indicate that a create was attempted on a resource but that properties that are required for the create operation were missing from the request.",
            "The create operation failed because the required property %1 was missing from the request.",
            "Critical",
            1,
            { "string" },
            { "The name of the required property." },
            { "This argument shall contain the name of the required property." },
            "Correct the body to include the required property with a valid value and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
CreateLimitReachedForResource :
    public RedfishMessage<>
{
    template <typename... Args>
    CreateLimitReachedForResource(Args... args) :
        RedfishMessage(
            "Base.1.11.2.CreateLimitReachedForResource",
            "Indicates that no more resources can be created on the resource as it has reached its create limit.",
            "This message shall be used to indicate that no more resources can be created on the resource as it has reached its create limit.",
            "The create operation failed because the resource has reached the limit of possible resources.",
            "Critical",
            0,
            {},
            {},
            {},
            "Either delete resources and resubmit the request if the operation failed or do not resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ServiceShuttingDown :
    public RedfishMessage<>
{
    template <typename... Args>
    ServiceShuttingDown(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ServiceShuttingDown",
            "Indicates that the operation failed as the service is shutting down, such as when the service reboots.",
            "This message shall be used to indicate that the operation failed as the service is shutting down.",
            "The operation failed because the service is shutting down and can no longer take incoming requests.",
            "Critical",
            0,
            {},
            {},
            {},
            "When the service becomes available, resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ServiceInUnknownState :
    public RedfishMessage<>
{
    template <typename... Args>
    ServiceInUnknownState(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ServiceInUnknownState",
            "Indicates that the operation failed because the service is in an unknown state and cannot accept additional requests.",
            "This message shall be used to indicate that the operation failed because the service is in an unknown state and cannot accept additional requests.",
            "The operation failed because the service is in an unknown state and can no longer take incoming requests.",
            "Critical",
            0,
            {},
            {},
            {},
            "Restart the service and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
NoValidSession :
    public RedfishMessage<>
{
    template <typename... Args>
    NoValidSession(Args... args) :
        RedfishMessage(
            "Base.1.11.2.NoValidSession",
            "Indicates that the operation failed because a valid session is required in order to access any resources.",
            "This message shall be used to indicate that the operation failed because a valid session is required in order to access any resources.",
            "There is no valid session established with the implementation.",
            "Critical",
            0,
            {},
            {},
            {},
            "Establish a session before attempting any operations.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
InsufficientPrivilege :
    public RedfishMessage<>
{
    template <typename... Args>
    InsufficientPrivilege(Args... args) :
        RedfishMessage(
            "Base.1.11.2.InsufficientPrivilege",
            "Indicates that the credentials associated with the established session do not have sufficient privileges for the requested operation.",
            "This message shall be used to indicate that the credentials associated with the established session do not have sufficient privileges for the requested operation.",
            "There are insufficient privileges for the account or credentials associated with the current session to perform the requested operation.",
            "Critical",
            0,
            {},
            {},
            {},
            "Either abandon the operation or change the associated access rights and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
AccountModified :
    public RedfishMessage<>
{
    template <typename... Args>
    AccountModified(Args... args) :
        RedfishMessage(
            "Base.1.11.2.AccountModified",
            "Indicates that the account was successfully modified.",
            "This message shall be used to indicate that the account was successfully modified.",
            "The account was successfully modified.",
            "OK",
            0,
            {},
            {},
            {},
            "No resolution is required.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
AccountNotModified :
    public RedfishMessage<>
{
    template <typename... Args>
    AccountNotModified(Args... args) :
        RedfishMessage(
            "Base.1.11.2.AccountNotModified",
            "Indicates that the modification requested for the account was not successful.",
            "This message shall be used to indicate that the modification requested for the account was not successful.",
            "The account modification request failed.",
            "Warning",
            0,
            {},
            {},
            {},
            "The modification may have failed due to permission issues or issues with the request body.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
AccountRemoved :
    public RedfishMessage<>
{
    template <typename... Args>
    AccountRemoved(Args... args) :
        RedfishMessage(
            "Base.1.11.2.AccountRemoved",
            "Indicates that the account was successfully removed.",
            "This message shall be used to indicate that the account was successfully removed.",
            "The account was successfully removed.",
            "OK",
            0,
            {},
            {},
            {},
            "No resolution is required.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
AccountForSessionNoLongerExists :
    public RedfishMessage<>
{
    template <typename... Args>
    AccountForSessionNoLongerExists(Args... args) :
        RedfishMessage(
            "Base.1.11.2.AccountForSessionNoLongerExists",
            "Indicates that the account for the session has been removed, thus the session has been removed as well.",
            "This message shall be used to indicate that the account for the session has been removed, thus the session has been removed as well.",
            "The account for the current session has been removed, thus the current session has been removed as well.",
            "OK",
            0,
            {},
            {},
            {},
            "Attempt to connect with a valid account.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
InvalidObject :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    InvalidObject(Args... args) :
        RedfishMessage(
            "Base.1.11.2.InvalidObject",
            "Indicates that the object in question is invalid according to the implementation.  Examples include a firmware update malformed URI.",
            "This message shall be used to indicate that the object in question is invalid according to the implementation.",
            "The object at '%1' is invalid.",
            "Critical",
            1,
            { "string" },
            { "The URI provided." },
            { "This argument shall contain the URI provided for this operation." },
            "Either the object is malformed or the URI is not correct.  Correct the condition and resubmit the request if it failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
InternalError :
    public RedfishMessage<>
{
    template <typename... Args>
    InternalError(Args... args) :
        RedfishMessage(
            "Base.1.11.2.InternalError",
            "Indicates that the request failed for an unknown internal error but that the service is still operational.",
            "This message shall be used to indicate that the request failed for an unknown internal error but that the service is still operational.",
            "The request failed due to an internal service error.  The service is still operational.",
            "Critical",
            0,
            {},
            {},
            {},
            "Resubmit the request.  If the problem persists, consider resetting the service.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
UnrecognizedRequestBody :
    public RedfishMessage<>
{
    template <typename... Args>
    UnrecognizedRequestBody(Args... args) :
        RedfishMessage(
            "Base.1.11.2.UnrecognizedRequestBody",
            "Indicates that the service encountered an unrecognizable request body that could not even be interpreted as malformed JSON.",
            "This message shall be used to indicate that the service encountered an unrecognizable request body that could not even be interpreted as malformed JSON.",
            "The service detected a malformed request body that it was unable to interpret.",
            "Warning",
            0,
            {},
            {},
            {},
            "Correct the request body and resubmit the request if it failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ResourceMissingAtURI :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourceMissingAtURI(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ResourceMissingAtURI",
            "Indicates that the operation expected an image or other resource at the provided URI but none was found.  Examples of this are in requests that require URIs like firmware update.",
            "This message shall be used to indicate that the operation expected an image or other resource at the provided URI but none was found.",
            "The resource at the URI '%1' was not found.",
            "Critical",
            1,
            { "string" },
            { "The URI provided." },
            { "This argument shall contain the URI provided for this operation." },
            "Place a valid resource at the URI or correct the URI and resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ResourceAtUriInUnknownFormat :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourceAtUriInUnknownFormat(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ResourceAtUriInUnknownFormat",
            "Indicates that the URI was valid but the resource or image at that URI was in a format not supported by the service.",
            "This message shall be used to indicate that the URI was valid but the resource or image at that URI was in a format not supported by the service.",
            "The resource at '%1' is in a format not recognized by the service.",
            "Critical",
            1,
            { "string" },
            { "The URI provided." },
            { "This argument shall contain the URI provided for this operation." },
            "Place an image or resource or file that is recognized by the service at the URI.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ResourceAtUriUnauthorized :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ResourceAtUriUnauthorized(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ResourceAtUriUnauthorized",
            "Indicates that the attempt to access the resource, file, or image at the URI was unauthorized.",
            "This message shall be used to indicate that the attempt to access the resource, file, or image at the URI was unauthorized.",
            "While accessing the resource at '%1', the service received an authorization error '%2'.",
            "Critical",
            2,
            { "string", "string" },
            { "The URI provided.", "The error message received from the authorization service." },
            { "This argument shall contain the URI provided for this operation.", "This argument shall contain the error message received from the authorization service or software." },
            "Ensure that the appropriate access is provided for the service in order for it to access the URI.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
CouldNotEstablishConnection :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    CouldNotEstablishConnection(Args... args) :
        RedfishMessage(
            "Base.1.11.2.CouldNotEstablishConnection",
            "Indicates that the attempt to access the resource, file, or image at the URI was unsuccessful because a session could not be established.",
            "This message shall be used to indicate that the attempt to access the resource, file, or image at the URI was unsuccessful because a session could not be established.",
            "The service failed to establish a connection with the URI '%1'.",
            "Critical",
            1,
            { "string" },
            { "The URI provided." },
            { "This argument shall contain the URI provided for this operation." },
            "Ensure that the URI contains a valid and reachable node name, protocol information and other URI components.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
SourceDoesNotSupportProtocol :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    SourceDoesNotSupportProtocol(Args... args) :
        RedfishMessage(
            "Base.1.11.2.SourceDoesNotSupportProtocol",
            "Indicates that while attempting to access, connect to or transfer a resource, file, or image from another location that the other end of the connection did not support the protocol.",
            "This message shall be used to indicate that while attempting to access, connect to or transfer a resource, file, or image from another location that the other end of the connection did not support the protocol.",
            "The other end of the connection at '%1' does not support the specified protocol %2.",
            "Critical",
            2,
            { "string", "string" },
            { "The URI provided.", "The protocol specified." },
            { "This argument shall contain the URI provided for this operation.", "This argument shall contain the name of the protocol requested for this operation." },
            "Change protocols or URIs.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
AccessDenied :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    AccessDenied(Args... args) :
        RedfishMessage(
            "Base.1.11.2.AccessDenied",
            "Indicates that while attempting to access, connect to, or transfer to or from another resource, the service denied access.",
            "This message shall be used to indicate that while attempting to access, connect to, or transfer to or from another resource, the service denied access.",
            "While attempting to establish a connection to '%1', the service denied access.",
            "Critical",
            1,
            { "string" },
            { "The URI provided." },
            { "This argument shall contain the URI provided for this operation." },
            "Attempt to ensure that the URI is correct and that the service has the appropriate credentials.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ServiceTemporarilyUnavailable :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ServiceTemporarilyUnavailable(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ServiceTemporarilyUnavailable",
            "Indicates the service is temporarily unavailable.",
            "This message shall be used to indicate that the service is temporarily unavailable.",
            "The service is temporarily unavailable.  Retry in %1 seconds.",
            "Critical",
            1,
            { "string" },
            { "The retry duration in seconds." },
            { "This argument shall contain the number of seconds the client should wait before attempting the operation again." },
            "Wait for the indicated retry duration and retry the operation.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
InvalidIndex :
    public RedfishMessage<double>
{
    template <typename... Args>
    InvalidIndex(Args... args) :
        RedfishMessage(
            "Base.1.11.2.InvalidIndex",
            "The index is not valid.",
            "This message shall be used to indicate that the index is not valid.",
            "The index %1 is not a valid offset into the array.",
            "Warning",
            1,
            { "number" },
            { "The array index." },
            { "This argument shall contain the array index value provided." },
            "Verify the index value provided is within the bounds of the array.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
PropertyValueModified :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    PropertyValueModified(Args... args) :
        RedfishMessage(
            "Base.1.11.2.PropertyValueModified",
            "Indicates that a property was given the correct value type but the value of that property was modified.  Examples are truncated or rounded values.",
            "This message shall be used to indicate that a property was given the correct value type but the value of that property was modified.",
            "The property %1 was assigned the value '%2' due to modification by the service.",
            "Warning",
            2,
            { "string", "string" },
            { "The name of the property.", "The value assigned for property." },
            { "This argument shall contain the name of the property.", "This argument shall contain the value assigned for property." },
            "No resolution is required.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ResourceInStandby :
    public RedfishMessage<>
{
    template <typename... Args>
    ResourceInStandby(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ResourceInStandby",
            "Indicates that the request could not be performed because the resource is in standby.",
            "This message shall be used to indicate that the request could not be performed because the resource is in standby.",
            "The request could not be performed because the resource is in standby.",
            "Critical",
            0,
            {},
            {},
            {},
            "Ensure that the resource is in the correct power state and resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ResourceExhaustion :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourceExhaustion(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ResourceExhaustion",
            "Indicates that a resource could not satisfy the request due to some unavailability of resources.  An example is that available capacity has been allocated.",
            "This message shall be used to indicate that a resource could not satisfy the request due to some unavailability of resources.",
            "The resource '%1' was unable to satisfy the request due to unavailability of resources.",
            "Critical",
            1,
            { "string" },
            { "The URI of the resource." },
            { "This argument shall contain the URI of the relevant resource." },
            "Ensure that the resources are available and resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
StringValueTooLong :
    public RedfishMessage<std::string, double>
{
    template <typename... Args>
    StringValueTooLong(Args... args) :
        RedfishMessage(
            "Base.1.11.2.StringValueTooLong",
            "Indicates that a string value passed to the given resource exceeded its length limit.  An example is when a shorter limit is imposed by an implementation than that allowed by the specification.",
            "This message shall be used to indicate that a string value passed to the given resource exceeded its length limit.",
            "The string '%1' exceeds the length limit %2.",
            "Warning",
            2,
            { "string", "number" },
            { "The string provided.", "The maximum string length." },
            { "This argument shall contain the string value provided for this operation.", "This argument shall contain the maximum supported string length for this property." },
            "Resubmit the request with an appropriate string length.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
SessionTerminated :
    public RedfishMessage<>
{
    template <typename... Args>
    SessionTerminated(Args... args) :
        RedfishMessage(
            "Base.1.11.2.SessionTerminated",
            "Indicates that the DELETE operation on the session resource resulted in the successful termination of the session.",
            "This message shall be used to indicate that the DELETE operation on the session resource resulted in the successful termination of the session.",
            "The session was successfully terminated.",
            "OK",
            0,
            {},
            {},
            {},
            "No resolution is required.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
SubscriptionTerminated :
    public RedfishMessage<>
{
    template <typename... Args>
    SubscriptionTerminated(Args... args) :
        RedfishMessage(
            "Base.1.11.2.SubscriptionTerminated",
            "An event subscription has been terminated by the service.  No further events will be delivered.",
            "This message shall be used to indicate that an event subscription has been terminated by the service.",
            "The event subscription has been terminated.",
            "OK",
            0,
            {},
            {},
            {},
            "No resolution is required.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ResourceTypeIncompatible :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ResourceTypeIncompatible(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ResourceTypeIncompatible",
            "Indicates that the resource type of the operation does not match that for the operation destination.  Examples of when this can happen include during a POST to a resource collection using the wrong resource type, an update where the `@odata.type` properties do not match, or on a major version incompatibility.",
            "This message shall be used to indicate that the resource type of the operation does not match that for the operation destination.",
            "The @odata.type of the request body %1 is incompatible with the @odata.type of the resource, which is %2.",
            "Critical",
            2,
            { "string", "string" },
            { "The `@odata.type` of the request.", "The supported `@odata.type` value." },
            { "This argument shall contain the value of the `@odata.type` property provided in the request.", "This argument shall contain the value of the `@odata.type` property supported by the resource." },
            "Resubmit the request with a payload compatible with the resource's schema.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
PasswordChangeRequired :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PasswordChangeRequired(Args... args) :
        RedfishMessage(
            "Base.1.11.2.PasswordChangeRequired",
            "Indicates that the password for the account provided must be changed before accessing the service.  The password can be changed with a PATCH to the `Password` property in the manager account resource instance.  Implementations that provide a default password for an account may require a password change prior to first access to the service.",
            "This message shall be used to indicate that the password for the account provided must be changed before accessing the service.  The password can be changed with a PATCH to the `Password` property in the manager account resource instance.  Implementations that provide a default password for an account may require a password change prior to first access to the service.",
            "The password provided for this account must be changed before access is granted.  PATCH the Password property for this account located at the target URI '%1' to complete this process.",
            "Critical",
            1,
            { "string" },
            { "The URI of the target resource." },
            { "This argument shall contain the URI of the target manager account resource instance." },
            "Change the password for this account using a PATCH to the Password property at the URI provided.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ResetRequired :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    ResetRequired(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ResetRequired",
            "Indicates that a component reset is required for changes or operations to complete.",
            "This message shall be used to indicate that a component reset is required for changes or operations to complete.",
            "In order to complete the operation, a component reset is required with the Reset action URI '%1' and ResetType '%2'.",
            "Warning",
            2,
            { "string", "string" },
            { "The URI for the reset action of the component that requires a reset.", "The `ResetType` required in a reset action on the component URI." },
            { "This argument shall contain the URI for the reset action that is required to be invoked for changes or operations to complete.", "This argument shall contain the `ResetType` required in a reset action on the component URI for changes or operations to complete.  A reset action POST shall contain this `ResetType` parameter." },
            "Perform the required reset action on the specified component.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ChassisPowerStateOnRequired :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ChassisPowerStateOnRequired(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ChassisPowerStateOnRequired",
            "Indicates that the request requires a specified chassis to be powered on.",
            "This message shall be used to indicate that the request requires the specified chassis to be powered on as indicated by its `PowerState` property.",
            "The chassis with Id '%1' requires to be powered on to perform this request.",
            "Warning",
            1,
            { "string" },
            { "The Id of the chassis that requires to be powered on." },
            { "The value of this argument shall be a string containing the value of the `Id` property of the chassis that requires to be powered on." },
            "Power on the specified chassis and resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ChassisPowerStateOffRequired :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ChassisPowerStateOffRequired(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ChassisPowerStateOffRequired",
            "Indicates that the request requires a specified chassis to be powered off.",
            "This message shall be used to indicate that the request requires the specified chassis to be powered off as indicated by its `PowerState` property.",
            "The Chassis with Id '%1' requires to be powered off to perform this request.",
            "Warning",
            1,
            { "string" },
            { "The `Id` of the chassis that requires to be powered off." },
            { "The value of this argument shall be a string containing the value of the `Id` property of the chassis that requires to be powered off." },
            "Power off the specified chassis and resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
PropertyValueConflict :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    PropertyValueConflict(Args... args) :
        RedfishMessage(
            "Base.1.11.2.PropertyValueConflict",
            "Indicates that the requested write of a property value could not be completed, because of a conflict with another property value.",
            "This message shall be used to indicate that the requested write of a property could not be completed, because of a conflict with the value of another property.",
            "The property '%1' could not be written because its value would conflict with the value of the '%2' property.",
            "Warning",
            2,
            { "string", "string" },
            { "The name of the property for which a write was requested.", "The name of the property with which there is a conflict." },
            { "This argument shall contain the name of the property for which a write was requested.", "This argument shall contain the name of the property with which there is a conflict." },
            "No resolution is required.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
PropertyValueResourceConflict :
    public RedfishMessage<std::string, std::string, std::string>
{
    template <typename... Args>
    PropertyValueResourceConflict(Args... args) :
        RedfishMessage(
            "Base.1.11.2.PropertyValueResourceConflict",
            "Indicates that the requested write of a property value could not be completed, due to the current state or configuration of another resource.",
            "This message shall be used to indicate that the requested write of a property could not be completed, because the requested value of the property, which is a value supported by the implementation, cannot be accepted due to conflicts in the state or configuration of another resource.",
            "The property '%1' with the requested value of '%2' could not be written because the value conflicts with the state or configuration of the resource at '%3'.",
            "Warning",
            3,
            { "string", "string", "string" },
            { "The name of the property for which a write was requested.", "The value of the property that is in conflict.", "The URI of the resource in conflict." },
            { "This argument shall contain the name of the property for which a write was requested.", "This argument shall contain the value of the property that is in conflict.", "This argument shall contain the URI of the resource that is in conflict with the value of the requested property." },
            "No resolution is required.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
PropertyValueExternalConflict :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    PropertyValueExternalConflict(Args... args) :
        RedfishMessage(
            "Base.1.11.2.PropertyValueExternalConflict",
            "Indicates that the requested write of a property value could not be completed, due to the current state or configuration of the resource.  This can include configuration conflicts with other resources or parameters that are not exposed by this interface.",
            "This message shall be used to indicate that the requested write of a property could not be completed, because the requested value of the property, which is a value supported by the implementation, cannot be accepted due to conflicts in the state or configuration of the resource.",
            "The property '%1' with the requested value of '%2' could not be written because the value is not available due to a configuration conflict.",
            "Warning",
            2,
            { "string", "string" },
            { "The name of the property for which a write was requested.", "The value of the property that is in conflict." },
            { "This argument shall contain the name of the property for which a write was requested.", "This argument shall contain the value of the property that is in conflict." },
            "No resolution is required.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
PropertyValueIncorrect :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    PropertyValueIncorrect(Args... args) :
        RedfishMessage(
            "Base.1.11.2.PropertyValueIncorrect",
            "Indicates that the requested write of a property value could not be completed, because of an incorrect value of the property.  Examples include values that do not match a regular expression requirement or passwords that do not match the implementation constraints.",
            "This message shall be used to indicate that the requested write of a property could not be completed, because of an incorrect value of the property such as when it does not meet the constraints of the implementation or match the regular expression requirements.",
            "The property '%1' with the requested value of '%2' could not be written because the value does not meet the constraints of the implementation.",
            "Warning",
            2,
            { "string", "string" },
            { "Name of the property for which a write was requested.", "Value of the property that is incorrect." },
            { "This argument shall contain the name of the property for which a write was requested.", "This argument shall contain the value of the property that is incorrect." },
            "No resolution is required.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ResourceCreationConflict :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourceCreationConflict(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ResourceCreationConflict",
            "Indicates that the requested resource creation could not be completed because the service has a resource that conflicts with the request.",
            "This message shall be used to indicate that the requested creation of a resource could not be completed because the service has a resource that conflicts with the request, such as when the resource already exists or an existing resource has similar property values.",
            "The resource could not be created.  The service has a resource at URI '%1' that conflicts with the creation request.",
            "Warning",
            1,
            { "string" },
            { "URI of the resource that conflicts with the creation request." },
            { "This argument shall contain the URI of the resource with which there is a conflict." },
            "No resolution is required.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
MaximumErrorsExceeded :
    public RedfishMessage<>
{
    template <typename... Args>
    MaximumErrorsExceeded(Args... args) :
        RedfishMessage(
            "Base.1.11.2.MaximumErrorsExceeded",
            "Indicates that sufficient errors have occurred that the reporting service cannot return them all.",
            "The maximum number of errors that the implementation can report has been exceeded.  This shall not be the only message reported by the service.  The use of this message should be limited to RDE implementations.",
            "Too many errors have occurred to report them all.",
            "Critical",
            0,
            {},
            {},
            {},
            "Resolve other reported errors and retry the current operation.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
PreconditionFailed :
    public RedfishMessage<>
{
    template <typename... Args>
    PreconditionFailed(Args... args) :
        RedfishMessage(
            "Base.1.11.2.PreconditionFailed",
            "Indicates that the ETag supplied did not match the current ETag of the resource.",
            "Indicates that the ETag supplied either in the header or using that `@odata.etag` annotation in the resource did not match the one expected by the service and thus the operation on this resource failed.  The use of this message shall only be used in responses for deep operations, since HTTP status code 412 is typically used for this.",
            "The ETag supplied did not match the ETag required to change this resource.",
            "Critical",
            0,
            {},
            {},
            {},
            "Try the operation again using the appropriate ETag.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
PreconditionRequired :
    public RedfishMessage<>
{
    template <typename... Args>
    PreconditionRequired(Args... args) :
        RedfishMessage(
            "Base.1.11.2.PreconditionRequired",
            "Indicates that the request did not provide the required precondition such as an `If-Match` or `If-None-Match` header, or `@odata.etag` annotations.",
            "Indicates that the request did not provide the required precondition such as an `If-Match` or `If-None-Match` header, or `@odata.etag` annotations.  The use of this message shall only be used in responses for deep operations, since HTTP status code 428 is typically used for this.",
            "A precondition header or annotation is required to change this resource.",
            "Critical",
            0,
            {},
            {},
            {},
            "Try the operation again using an If-Match or If-None-Match header and appropriate ETag.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
OperationFailed :
    public RedfishMessage<>
{
    template <typename... Args>
    OperationFailed(Args... args) :
        RedfishMessage(
            "Base.1.11.2.OperationFailed",
            "Indicates that one of the internal operations necessary to complete the request failed.  Examples of this are when an internal service provider is unable to complete the request, such as in aggregation or RDE.",
            "This message shall be used to indicate that one of the internal operations necessary to complete the request failed.  Partial results of the client operation may be returned.",
            "An error occurred internal to the service as part of the overall request.  Partial results may have been returned.",
            "Warning",
            0,
            {},
            {},
            {},
            "Resubmit the request.  If the problem persists, consider resetting the service or provider.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
OperationTimeout :
    public RedfishMessage<>
{
    template <typename... Args>
    OperationTimeout(Args... args) :
        RedfishMessage(
            "Base.1.11.2.OperationTimeout",
            "Indicates that one of the internal operations necessary to complete the request timed out.  Examples of this are when an internal service provider is unable to complete the request, such as in aggregation or RDE.",
            "This message shall be used to indicate that one of the internal operations necessary to complete the request timed out.  Partial results of the client operation may be returned.",
            "A timeout internal to the service occured as part of the request.  Partial results may have been returned.",
            "Warning",
            0,
            {},
            {},
            {},
            "Resubmit the request.  If the problem persists, consider resetting the service or provider.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
UndeterminedFault :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    UndeterminedFault(Args... args) :
        RedfishMessage(
            "Base.1.11.2.UndeterminedFault",
            "Indicates that a fault or error condition exists but the source of the fault cannot be determined or is unknown to the service.",
            "This message shall be used to indicate that a fault or error condition has been reported to the service, but the source of the fault cannot be determined or is unknown to the service.",
            "A undetermined fault condition has been reported by '%1'.",
            "Critical",
            1,
            { "string" },
            { "The software, provider, or service that reported the fault condition." },
            { "This argument shall contain the name of the entity that reported the fault condition." },
            "None.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ConditionInRelatedResource :
    public RedfishMessage<>
{
    template <typename... Args>
    ConditionInRelatedResource(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ConditionInRelatedResource",
            "Indicates that one or more fault or error conditions exist in a related resource.",
            "This message shall be used to indicate that one or more conditions that require attention exist in a resource that affects the Health or HealthRollup of this resource.  This message can be used in place of multiple messages in a `Conditions` property to roll up conditions from a single resource.  The resource can be located using the `OriginOfCondition` property.  The value of the `MessageSeverity` property should match the highest severity of any conditions in the resource referenced by the `OriginOfCondition` property.",
            "One or more conditions exist in a related resource.  See the OriginOfCondition property.",
            "Warning",
            0,
            {},
            {},
            {},
            "Check the Conditions array in the resource shown in the OriginOfCondition property to determine the conditions that need attention.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
RestrictedRole :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    RestrictedRole(Args... args) :
        RedfishMessage(
            "Base.1.11.2.RestrictedRole",
            "Indicates that the operation was not successful because the role is restricted.",
            "This message shall be used to indicate that the operation was not successful because the role is restricted.",
            "The operation was not successful because the role '%1' is restricted.",
            "Warning",
            1,
            { "string" },
            { "The name of the restricted role." },
            { "This argument shall contain the name of the restricted role." },
            "No resolution is required.  For standard roles, consider using the role specified in the AlternateRoleId property in the Role resource.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
RestrictedPrivilege :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    RestrictedPrivilege(Args... args) :
        RedfishMessage(
            "Base.1.11.2.RestrictedPrivilege",
            "Indicates that the operation was not successful because a privilege is restricted.",
            "This message shall be used to indicate that the operation was not successful because a privilege is restricted.",
            "The operation was not successful because the privilege '%1' is restricted.",
            "Warning",
            1,
            { "string" },
            { "The name of the restricted privilege." },
            { "This argument shall contain the name of the restricted privilege." },
            "Remove restricted privileges from the request body and resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
StrictAccountTypes :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    StrictAccountTypes(Args... args) :
        RedfishMessage(
            "Base.1.11.2.StrictAccountTypes",
            "Indicates the request failed because a set of `AccountTypes` or `OEMAccountTypes` was not accepted while `StrictAccountTypes` is set to `true`.",
            "This message shall be used to indicate that the request failed because a set of `AccountTypes` or `OEMAccountTypes` was not accepted while `StrictAccountTypes` is set to `true`.",
            "The request was not possible to fulfill with the account types included in property '%1' and property StrictAccountTypes set to true.",
            "Warning",
            1,
            { "string" },
            { "The name of the property." },
            { "This argument shall contain the name of the property `AccountTypes` or `OEMAccountTypes`." },
            "Resubmit the request either with an acceptable set of AccountTypes and OEMAccountTypes or with StrictAccountTypes set to false.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
PropertyDeprecated :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    PropertyDeprecated(Args... args) :
        RedfishMessage(
            "Base.1.11.2.PropertyDeprecated",
            "Indicates the property is deprecated.",
            "This message shall be used to indicate that the property is deprecated.",
            "The deprecated property %1 was included in the request body.",
            "Warning",
            1,
            { "string" },
            { "The name of the property." },
            { "This argument shall contain the name of the property that is deprecated." },
            "Refer to the schema guide for more information.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ResourceDeprecated :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ResourceDeprecated(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ResourceDeprecated",
            "Indicates the resource is deprecated.",
            "This message shall be used to indicate that the resource is deprecated.",
            "The operation was performed on a deprecated resource '%1'.",
            "Warning",
            1,
            { "string" },
            { "The URI of the resource." },
            { "This argument shall contain the URI of the resource that is deprecated." },
            "Refer to the schema guide for more information.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
PropertyValueDeprecated :
    public RedfishMessage<std::string, std::string>
{
    template <typename... Args>
    PropertyValueDeprecated(Args... args) :
        RedfishMessage(
            "Base.1.11.2.PropertyValueDeprecated",
            "Indicates that a property was given a deprecated value.",
            "This message shall be used to indicate that a property was given a deprecated value.",
            "The value '%1' for the property %2 is deprecated.",
            "Warning",
            1,
            { "string", "string" },
            { "The value provided for the property.", "The name of the property." },
            { "This argument shall contain the deprecated value provided for the property.", "This argument shall contain the name of the property." },
            "Refer to the schema guide for more information.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ActionDeprecated :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ActionDeprecated(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ActionDeprecated",
            "Indicates the action is deprecated.",
            "This message shall be used to indicate that the action is deprecated.",
            "The action %1 is deprecated.",
            "Warning",
            1,
            { "string" },
            { "The name of the action." },
            { "This argument shall contain the name of the action that is deprecated." },
            "Refer to the schema guide for more information.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
NetworkNameResolutionNotConfigured :
    public RedfishMessage<>
{
    template <typename... Args>
    NetworkNameResolutionNotConfigured(Args... args) :
        RedfishMessage(
            "Base.1.11.2.NetworkNameResolutionNotConfigured",
            "Indicates that network-based name resolution has not been configured on the service.",
            "This message shall be used to indicate that network-based name resolution, such as DNS or WINS protocols, has not been configured on the service.",
            "Network name resolution has not been configured on this service.",
            "Warning",
            0,
            {},
            {},
            {},
            "Configure the network name resolution protocol support on this service, or update any URI values to include an IP address instead of a network name and resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
NetworkNameResolutionNotSupported :
    public RedfishMessage<>
{
    template <typename... Args>
    NetworkNameResolutionNotSupported(Args... args) :
        RedfishMessage(
            "Base.1.11.2.NetworkNameResolutionNotSupported",
            "Indicates the service does not support network-based name resolution.",
            "This message shall be used to indicate that the service does not support network-based name resolution, such as DNS or WINS protocols.  URIs provided as property values must contain an IP address as the service cannot resolve a network name.",
            "Resolution of network-based names is not supported by this service.",
            "Warning",
            0,
            {},
            {},
            {},
            "Update any URI values to include an IP address instead of a network name and resubmit the request.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

struct [[deprecated ("Use Base_1_20_0")]]
ServiceDisabled :
    public RedfishMessage<std::string>
{
    template <typename... Args>
    ServiceDisabled(Args... args) :
        RedfishMessage(
            "Base.1.11.2.ServiceDisabled",
            "Indicates that the operation failed because the service, such as the account service, is disabled and cannot accept requests.",
            "This message shall be used to indicate that the operation failed because the service, such as the account service, is disabled and cannot accept requests.",
            "The operation failed because the service at %1 is disabled and cannot accept requests.",
            "Warning",
            1,
            { "string" },
            { "The URI of the disabled service." },
            { "This argument shall contain the URI of the disabled service." },
            "Enable the service and resubmit the request if the operation failed.",
            std::make_tuple(std::forward<Args>(args)...))
    {}
};

}; // namespace Base_1_11_2
}; // namespace registries
}; // namespace redfish
