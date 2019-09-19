#include "extensions/openpower-pels/registry.hpp"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

#include <gtest/gtest.h>

using namespace openpower::pels::message;
namespace fs = std::filesystem;

const auto registryData = R"(
{
    "PELs":
    [
        {
            "Name": "xyz.openbmc_project.Power.Fault",
            "Subsystem": "power_supply",
            "ActionFlags": ["service_action", "report"],

            "SRC":
            {
                "ReasonCode": "0x2030"
            }
        },

        {
            "Name": "xyz.openbmc_project.Power.OverVoltage",
            "Subsystem": "power_control_hw",
            "Severity": "unrecoverable",
            "MfgSeverity": "non_error",
            "ActionFlags": ["service_action", "report", "call_home"],
            "MfgActionFlags": ["hidden"],

            "SRC":
            {
                "ReasonCode": "0x2333",
                "Type": "BD",
                "SymptomIDFields": ["SRCWord5", "SRCWord6", "SRCWord7"],
                "PowerFault": true,
                "Words6To9":
                {
                    "6":
                    {
                        "description": "Failing unit number",
                        "AdditionalDataPropSource": "PS_NUM"
                    },

                    "7":
                    {
                        "description": "bad voltage",
                        "AdditionalDataPropSource": "VOLTAGE"
                    }
                }
            }
        }
    ]
}
)";

class RegistryTest : public ::testing::Test
{
  protected:
    static void SetUpTestCase()
    {
        char path[] = "/tmp/regtestXXXXXX";
        regDir = mkdtemp(path);
    }

    static void TearDownTestCase()
    {
        fs::remove_all(regDir);
    }

    static std::string writeData(const char* data)
    {
        fs::path path = regDir / "registry.json";
        std::ofstream stream{path};
        stream << data;
        return path;
    }

    static fs::path regDir;
};

fs::path RegistryTest::regDir{};

TEST_F(RegistryTest, TestNoEntry)
{
    auto path = RegistryTest::writeData(registryData);
    Registry registry{path};

    auto entry = registry.lookup("foo");
    EXPECT_FALSE(entry);
}

TEST_F(RegistryTest, TestFindEntry)
{
    auto path = RegistryTest::writeData(registryData);
    Registry registry{path};

    auto entry = registry.lookup("xyz.openbmc_project.Power.OverVoltage");
    ASSERT_TRUE(entry);
    EXPECT_EQ(entry->name, "xyz.openbmc_project.Power.OverVoltage");
    EXPECT_EQ(entry->subsystem, 0x62);
    EXPECT_EQ(*(entry->severity), 0x40);
    EXPECT_EQ(*(entry->mfgSeverity), 0x00);
    EXPECT_EQ(entry->actionFlags, 0xA800);
    EXPECT_EQ(*(entry->mfgActionFlags), 0x4000);
    EXPECT_FALSE(entry->eventType);
    EXPECT_FALSE(entry->eventScope);

    // TODO: compare SRC fields
}

// Check the entry that mostly uses defaults
TEST_F(RegistryTest, TestFindEntryMinimal)
{
    auto path = RegistryTest::writeData(registryData);
    Registry registry{path};

    auto entry = registry.lookup("xyz.openbmc_project.Power.Fault");
    ASSERT_TRUE(entry);
    EXPECT_EQ(entry->name, "xyz.openbmc_project.Power.Fault");
    EXPECT_EQ(entry->subsystem, 0x61);
    EXPECT_FALSE(entry->severity);
    EXPECT_FALSE(entry->mfgSeverity);
    EXPECT_FALSE(entry->mfgActionFlags);
    EXPECT_EQ(entry->actionFlags, 0xA000);
    EXPECT_FALSE(entry->eventType);
    EXPECT_FALSE(entry->eventScope);
}

TEST_F(RegistryTest, TestBadJSON)
{
    auto path = RegistryTest::writeData("bad {} json");

    Registry registry{path};

    EXPECT_FALSE(registry.lookup("foo"));
}

// Test the helper functions the use the pel_values data.
TEST_F(RegistryTest, TestHelperFunctions)
{
    using namespace openpower::pels::message::helper;
    EXPECT_EQ(getSubsystem("input_power_source"), 0xA1);
    EXPECT_THROW(getSubsystem("foo"), std::runtime_error);

    EXPECT_EQ(getSeverity("symptom_recovered"), 0x71);
    EXPECT_THROW(getSeverity("foo"), std::runtime_error);

    EXPECT_EQ(getEventType("dump_notification"), 0x08);
    EXPECT_THROW(getEventType("foo"), std::runtime_error);

    EXPECT_EQ(getEventScope("possibly_multiple_platforms"), 0x04);
    EXPECT_THROW(getEventScope("foo"), std::runtime_error);

    std::vector<std::string> flags{"service_action", "dont_report",
                                   "termination"};
    EXPECT_EQ(getActionFlags(flags), 0x9100);

    flags.clear();
    flags.push_back("foo");
    EXPECT_THROW(getActionFlags(flags), std::runtime_error);
}
