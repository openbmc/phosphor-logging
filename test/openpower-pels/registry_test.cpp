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
    EXPECT_EQ(entry->componentID, 0x2300);
    EXPECT_FALSE(entry->eventType);
    EXPECT_FALSE(entry->eventScope);

    EXPECT_EQ(entry->src.type, 0xBD);
    EXPECT_EQ(entry->src.reasonCode, 0x2333);
    EXPECT_EQ(*(entry->src.powerFault), true);

    auto& hexwords = entry->src.hexwordADFields;
    EXPECT_TRUE(hexwords);
    EXPECT_EQ((*hexwords).size(), 2);

    auto word = (*hexwords).find(6);
    EXPECT_NE(word, (*hexwords).end());
    EXPECT_EQ(word->second, "PS_NUM");

    word = (*hexwords).find(7);
    EXPECT_NE(word, (*hexwords).end());
    EXPECT_EQ(word->second, "VOLTAGE");

    auto& sid = entry->src.symptomID;
    EXPECT_TRUE(sid);
    EXPECT_EQ((*sid).size(), 3);
    EXPECT_NE(std::find((*sid).begin(), (*sid).end(), 5), (*sid).end());
    EXPECT_NE(std::find((*sid).begin(), (*sid).end(), 6), (*sid).end());
    EXPECT_NE(std::find((*sid).begin(), (*sid).end(), 7), (*sid).end());
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
    EXPECT_EQ(entry->componentID, 0x2000);
    EXPECT_FALSE(entry->eventType);
    EXPECT_FALSE(entry->eventScope);

    EXPECT_EQ(entry->src.reasonCode, 0x2030);
    EXPECT_EQ(entry->src.type, 0xBD);
    EXPECT_FALSE(entry->src.powerFault);
    EXPECT_FALSE(entry->src.hexwordADFields);
    EXPECT_FALSE(entry->src.symptomID);
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

TEST_F(RegistryTest, TestGetSRCReasonCode)
{
    using namespace openpower::pels::message::helper;
    EXPECT_EQ(getSRCReasonCode(R"({"ReasonCode": "0x5555"})"_json, "foo"),
              0x5555);

    EXPECT_THROW(getSRCReasonCode(R"({"ReasonCode": "ZZZZ"})"_json, "foo"),
                 std::runtime_error);
}

TEST_F(RegistryTest, TestGetSRCType)
{
    using namespace openpower::pels::message::helper;
    EXPECT_EQ(getSRCType(R"({"Type": "11"})"_json, "foo"), 0x11);
    EXPECT_EQ(getSRCType(R"({"Type": "BF"})"_json, "foo"), 0xBF);

    EXPECT_THROW(getSRCType(R"({"Type": "1"})"_json, "foo"),
                 std::runtime_error);

    EXPECT_THROW(getSRCType(R"({"Type": "111"})"_json, "foo"),
                 std::runtime_error);
}

TEST_F(RegistryTest, TestGetSRCHexwordFields)
{
    using namespace openpower::pels::message::helper;
    const auto hexwords = R"(
    {"Words6To9":
      {
        "8":
        {
            "AdditionalDataPropSource": "TEST"
        }
      }
    })"_json;

    auto fields = getSRCHexwordFields(hexwords, "foo");
    EXPECT_TRUE(fields);
    auto word = fields->find(8);
    EXPECT_NE(word, fields->end());

    const auto theInvalidRWord = R"(
    {"Words6To9":
      {
        "R":
        {
            "AdditionalDataPropSource": "TEST"
        }
      }
    })"_json;

    EXPECT_THROW(getSRCHexwordFields(theInvalidRWord, "foo"),
                 std::runtime_error);
}

TEST_F(RegistryTest, TestGetSRCSymptomIDFields)
{
    using namespace openpower::pels::message::helper;
    const auto sID = R"(
    {
        "SymptomIDFields": ["SRCWord3", "SRCWord4", "SRCWord5"]
    })"_json;

    auto fields = getSRCSymptomIDFields(sID, "foo");
    EXPECT_NE(std::find(fields->begin(), fields->end(), 3), fields->end());
    EXPECT_NE(std::find(fields->begin(), fields->end(), 4), fields->end());
    EXPECT_NE(std::find(fields->begin(), fields->end(), 5), fields->end());

    const auto badField = R"(
    {
        "SymptomIDFields": ["SRCWord3", "SRCWord4", "SRCWord"]
    })"_json;

    EXPECT_THROW(getSRCSymptomIDFields(badField, "foo"), std::runtime_error);
}

TEST_F(RegistryTest, TestGetComponentID)
{
    using namespace openpower::pels::message::helper;

    // Get it from the JSON
    auto id =
        getComponentID(0xBD, 0x4200, R"({"ComponentID":"0x4200"})"_json, "foo");
    EXPECT_EQ(id, 0x4200);

    // Get it from the reason code on a 0xBD SRC
    id = getComponentID(0xBD, 0x6700, R"({})"_json, "foo");
    EXPECT_EQ(id, 0x6700);

    // Not present on a 0x11 SRC
    EXPECT_THROW(getComponentID(0x11, 0x8800, R"({})"_json, "foo"),
                 std::runtime_error);
}
