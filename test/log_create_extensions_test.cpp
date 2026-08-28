#include "log_create_extensions.hpp"

#include <nlohmann/json.hpp>
#include <xyz/openbmc_project/Logging/event.hpp>

#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

namespace
{

constexpr auto processInterface =
    "xyz.openbmc_project.Logging.Extension.CPER.Processed";
constexpr auto rawInterface = "xyz.openbmc_project.Logging.Extension.CPER.Raw";

/** Create a Cleared event, apply an extension, and return its serialization. */
nlohmann::json extendEvent(const std::string& interface,
                           const std::string& payload)
{
    nlohmann::json j = {
        {"xyz.openbmc_project.Logging.Cleared", {{"NUMBER_OF_LOGS", 42}}}};
    try
    {
        sdbusplus::exception::throw_via_json(j);
    }
    catch (sdbusplus::exception::generated_event_base& e)
    {
        extendLogEvent(e, interface, nlohmann::json::parse(payload));
        return e.to_json();
    }
    return {};
}

nlohmann::json extendEventExpectThrow(const std::string& interface,
                                      const std::string& payload)
{
    nlohmann::json j = {
        {"xyz.openbmc_project.Logging.Cleared", {{"NUMBER_OF_LOGS", 42}}}};
    try
    {
        sdbusplus::exception::throw_via_json(j);
    }
    catch (sdbusplus::exception::generated_event_base& e)
    {
        extendLogEvent(e, interface, nlohmann::json::parse(payload));
    }
    return {};
}

} // namespace

TEST(LogCreateExtensions, ProcessedInterface)
{
    const auto extensions = extendEvent(processInterface,
                                        R"({
                        "DiagnosticDataType": "xyz.openbmc_project.Logging.CPER.Types.ContentType.CPER",
                        "NotificationType": "notification",
                        "SectionType": "section",
                        "Oem": {"Vendor": "{\"key\":\"value\"}"}
                    })");

    const auto& ext = extensions.at("xyz.openbmc_project.Logging.Cleared")
                          .at("_EXTENSIONS")
                          .at(processInterface);
    EXPECT_EQ(ext.at("DiagnosticDataType"),
              "xyz.openbmc_project.Logging.CPER.Types.ContentType.CPER");
    EXPECT_EQ(ext.at("NotificationType"), "notification");
    EXPECT_EQ(ext.at("SectionType"), "section");
    EXPECT_EQ(ext.at("Oem").at("Vendor"), "{\"key\":\"value\"}");
}

TEST(LogCreateExtensions, RawInterface)
{
    const auto extensions = extendEvent(rawInterface, R"({"Data": [1, 2, 3]})");

    const auto& ext = extensions.at("xyz.openbmc_project.Logging.Cleared")
                          .at("_EXTENSIONS")
                          .at(rawInterface);
    EXPECT_EQ(ext.at("Data"), nlohmann::json({1, 2, 3}));
}

TEST(LogCreateExtensions, MultipleInterfaces)
{
    const auto processedJson = extendEvent(
        processInterface, R"({"NotificationType": "notification"})");
    const auto rawJson = extendEvent(rawInterface, R"({"Data": [9]})");

    const auto& extensions0 =
        processedJson.at("xyz.openbmc_project.Logging.Cleared")
            .at("_EXTENSIONS");
    const auto& extensions1 =
        rawJson.at("xyz.openbmc_project.Logging.Cleared").at("_EXTENSIONS");
    EXPECT_TRUE(extensions0.contains(processInterface));
    EXPECT_FALSE(extensions0.contains(rawInterface));
    EXPECT_TRUE(extensions1.contains(rawInterface));
}

TEST(LogCreateExtensions, UnknownInterfaceThrows)
{
    EXPECT_THROW(extendEventExpectThrow("xyz.openbmc_project.Logging.Extension."
                                        "Unknown",
                                        R"({})"),
                 std::invalid_argument);
}

TEST(LogCreateExtensions, InvalidJsonThrows)
{
    EXPECT_THROW(extendEventExpectThrow(processInterface, R"({not json})"),
                 nlohmann::json::parse_error);
}

TEST(LogCreateExtensions, WrongPropertyTypeThrows)
{
    EXPECT_THROW(
        extendEventExpectThrow(processInterface,
                               R"({"DiagnosticDataType": "NotAnEnum"})"),
        sdbusplus::exception::InvalidEnumString);
}
