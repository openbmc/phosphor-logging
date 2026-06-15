#include "cper_diagnostics.hpp"

#include <map>
#include <string>

#include <gtest/gtest.h>

using namespace phosphor::logging::diagnostics;

static constexpr auto CPER_MESSAGE_ID =
    "xyz.openbmc_project.Logging.CperDetected";

/* Mock / Test Helpers */

/**
 * @brief Mock Entry used to validate CPER interface attachment
 *
 * Simulates the Entry interface and captures parameters passed to
 * createCperInterface(). This allows verification that the correct
 * CPER type, diagnostic JSON string, and optional object path are
 * propagated during attachCperInterface() execution.
 */
class MockEntry
{
  public:
    // Indicates whether interface creation was triggered
    bool interfaceInvoked = false;

    // Captured CPER diagnostic type
    std::string capturedType;

    // Captured JSON-formatted diagnostic information
    std::string capturedDiagnosticInfo;

    // Captured AdditionalDataObject path
    std::string capturedObjectPath;

    /**
     * @brief Mock implementation of CPER interface creation
     *
     * Captures input parameters for validation in unit tests.
     */
    void createCperInterface(const std::string& inputType,
                             const std::string& inputDiagnosticInfo,
                             const std::string& inputObjectPath)
    {
        // Indicates that the interface creation was invoked
        interfaceInvoked = true;

        // Stores the provided CPER type
        capturedType = inputType;

        // Stores the provided JSON diagnostic information
        capturedDiagnosticInfo = inputDiagnosticInfo;

        // Stores the provided object path
        capturedObjectPath = inputObjectPath;
    }
};

/**
 * @brief Fake CPER DBus interface for unit testing
 *
 * Emulates the behavior of the CPER DBus interface by storing values
 * set through its APIs. This is useful for validating serialization
 * and integration logic without requiring a live DBus environment.
 */
class FakeCperIface
{
  public:
    // Stores CPER type set by implementation
    std::string storedType;

    // Stores JSON-formatted diagnostic data
    std::string storedDiagnosticInfo;

    // Stores additional data object path
    std::string storedAdditionalDataObjectPath;

    /**
     * @brief Set CPER type value
     */
    void type(const std::string& inputType)
    {
        // Stores the provided type value
        storedType = inputType;
    }

    /**
     * @brief Set diagnostic information (JSON string)
     */
    void diagnosticInfo(const std::string& diagnosticInfoString)
    {
        // Stores JSON-formatted diagnostic content
        storedDiagnosticInfo = diagnosticInfoString;
    }

    /**
     * @brief Set additional data object path
     */
    void additionalDataObject(const std::string& objectPath)
    {
        // Stores reference to extended diagnostic data
        storedAdditionalDataObjectPath = objectPath;
    }
};

/* =========================
 * collectCperData() Tests
 * ========================= */

TEST(CollectCperDataTest, ReturnsNotPresentWhenMessageIdMissing)
{
    std::map<std::string, std::string> additionalData{{"TYPE", "CPER"}};

    auto result = collectCperData("error", 1, additionalData);

    EXPECT_FALSE(result.present);
}

TEST(CollectCperDataTest, ReturnsNotPresentForDifferentMessageId)
{
    std::map<std::string, std::string> additionalData{
        {"MESSAGE_ID", "xyz.other.Event"}, {"TYPE", "CPER"}};

    auto result = collectCperData("error", 1, additionalData);

    EXPECT_FALSE(result.present);
}

TEST(CollectCperDataTest, UsesDefaultsWhenFieldsMissing)
{
    std::map<std::string, std::string> additionalData{
        {"MESSAGE_ID", CPER_MESSAGE_ID}};

    auto result = collectCperData("error", 1, additionalData);

    EXPECT_TRUE(result.present);
    EXPECT_EQ(result.type, "CPER");
    EXPECT_EQ(result.diagnosticInfo, R"({"Message":"CPER event detected"})");
    EXPECT_TRUE(result.additionalDataObject.empty());
}

TEST(CollectCperDataTest, ExtractsAllFieldsAndErasesThem)
{
    std::map<std::string, std::string> additionalData{
        {"MESSAGE_ID", CPER_MESSAGE_ID},
        {"TYPE", "CPERSection"},
        {"DATA", R"({"severity":"fatal"})"},
        {"ADDITIONALDATAURI", "/xyz/openbmc_project/dump/entry/1"}};

    auto result = collectCperData("error", 42, additionalData);

    EXPECT_TRUE(result.present);
    EXPECT_EQ(result.type, "CPERSection");
    EXPECT_EQ(result.diagnosticInfo, R"({"severity":"fatal"})");
    EXPECT_EQ(result.additionalDataObject, "/xyz/openbmc_project/dump/entry/1");

    // Ensures consumed keys are removed
    EXPECT_EQ(additionalData.count("TYPE"), 0);
    EXPECT_EQ(additionalData.count("DATA"), 0);
    EXPECT_EQ(additionalData.count("ADDITIONALDATAURI"), 0);

    // MESSAGE_ID remains
    EXPECT_EQ(additionalData.count("MESSAGE_ID"), 1);
}

TEST(CollectCperDataTest, FallsBackToDefaultsWhenFieldsEmpty)
{
    std::map<std::string, std::string> additionalData{
        {"MESSAGE_ID", CPER_MESSAGE_ID}, {"TYPE", ""}, {"DATA", ""}};

    auto result = collectCperData("error", 1, additionalData);

    EXPECT_TRUE(result.present);
    EXPECT_EQ(result.type, "CPER");
    EXPECT_EQ(result.diagnosticInfo, R"({"Message":"CPER event detected"})");
}

TEST(CollectCperDataTest, SupportsOnlyAdditionalDataObject)
{
    std::map<std::string, std::string> additionalData{
        {"MESSAGE_ID", CPER_MESSAGE_ID}, {"ADDITIONALDATAURI", "/xyz/dump/2"}};

    auto result = collectCperData("error", 1, additionalData);

    EXPECT_TRUE(result.present);
    EXPECT_EQ(result.type, "CPER");
    EXPECT_EQ(result.diagnosticInfo, R"({"Message":"CPER event detected"})");
    EXPECT_EQ(result.additionalDataObject, "/xyz/dump/2");
}

TEST(CollectCperDataTest, UnknownTypeFallsBackToOEM)
{
    std::map<std::string, std::string> additionalData{
        {"MESSAGE_ID", CPER_MESSAGE_ID}, {"TYPE", "UNKNOWN_TYPE"}};

    auto result = collectCperData("error", 1, additionalData);

    EXPECT_TRUE(result.present);
    EXPECT_EQ(result.type, "OEM");
}

/* =========================
 * attachCperInterface() Tests
 * ========================= */

TEST(AttachCperInterfaceTest, DoesNothingWhenNotPresent)
{
    MockEntry entry;
    CperData data{};

    attachCperInterface(entry, data);

    EXPECT_FALSE(entry.interfaceInvoked);
}

TEST(AttachCperInterfaceTest, InvokesEntryWhenPresent)
{
    MockEntry entry;

    CperData data;
    data.present = true;
    data.type = "CPERSection";
    data.diagnosticInfo = R"({"key":"value"})";
    data.additionalDataObject = "/object/path";

    attachCperInterface(entry, data);

    EXPECT_TRUE(entry.interfaceInvoked);
    EXPECT_EQ(entry.capturedType, "CPERSection");
    EXPECT_EQ(entry.capturedDiagnosticInfo, R"({"key":"value"})");
    EXPECT_EQ(entry.capturedObjectPath, "/object/path");
}
