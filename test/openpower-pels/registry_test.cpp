// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "extensions/openpower-pels/registry.hpp"

#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

using namespace openpower::pels::message;
using namespace openpower::pels;
namespace fs = std::filesystem;

const auto registryData = R"(
{
    "PELs":
    [
        {
            "Name": "xyz.openbmc_project.Power.Fault",
            "Subsystem": "power_supply",

            "SRC":
            {
                "ReasonCode": "0x2030"
            },

            "Documentation":
            {
                "Description": "A PGOOD Fault",
                "Message": "PS had a PGOOD Fault"
            }
        },

        {
            "Name": "xyz.openbmc_project.Power.OverVoltage",
            "Subsystem": "power_control_hw",
            "Severity":
            [
                {
                    "System": "systemA",
                    "SevValue": "unrecoverable"
                },
                {
                    "System": "systemB",
                    "SevValue": "recovered"
                },
                {
                    "SevValue": "predictive"
                }
            ],
            "MfgSeverity": "non_error",
            "ActionFlags": ["service_action", "report", "call_home"],
            "MfgActionFlags": ["hidden"],

            "SRC":
            {
                "ReasonCode": "0x2333",
                "Type": "BD",
                "SymptomIDFields": ["SRCWord5", "SRCWord6", "SRCWord7"],
                "Words6To9":
                {
                    "6":
                    {
                        "Description": "Failing unit number",
                        "AdditionalDataPropSource": "PS_NUM"
                    },

                    "7":
                    {
                        "Description": "bad voltage",
                        "AdditionalDataPropSource": "VOLTAGE"
                    }
                },
                "DeconfigFlag": true,
                "CheckstopFlag": true
            },

            "Documentation":
            {
                "Description": "A PGOOD Fault",
                "Message": "PS %1 had a PGOOD Fault",
                "MessageArgSources":
                [
                    "SRCWord6"
                ],
                "Notes": [
                    "In the UserData section there is a JSON",
                    "dump that provides debug information."
                ]
            },

            "JournalCapture":
            {
                "NumLines": 7
            }
        },

        {
            "Name": "xyz.openbmc_project.Common.Error.Timeout",
            "PossibleSubsystems": ["processor", "memory"],

            "SRC":
            {
                "ReasonCode": "0x2030"
            },
            "Documentation":
            {
                "Description": "A PGOOD Fault",
                "Message": "PS had a PGOOD Fault"
            }
        },

        {
            "Name": "xyz.openbmc_project.Journal.Capture",
            "Subsystem": "power_supply",

            "SRC":
            {
                "ReasonCode": "0x2030"
            },

            "Documentation":
            {
                "Description": "journal capture test",
                "Message": "journal capture test"
            },

            "JournalCapture":
            {
                "Sections": [
                    {
                        "NumLines": 5,
                        "SyslogID": "test1"
                    },
                    {
                        "NumLines": 6,
                        "SyslogID": "test2"
                    }
                ]
            }
        },

        {
            "Name": "org.open_power.PHAL.Info.ClockDailyLog",
            "Subsystem": "cec_clocks",
            "ComponentID": "0x3000",
            "Severity": "non_error",
            "ActionFlags": ["report", "call_home", "heartbeat_call_home"],

            "SRC":
            {
                "ReasonCode": "0x300A",
                "Words6To9": {}
            },

            "Documentation":
            {
                "Description": "Informational error to house clock debug info",
                "Message": "Informational error to house clock debug info",
                "Notes": [
                    "User data includes processor and clock register state information."
                ]
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

    auto entry = registry.lookup("foo", LookupType::name);
    EXPECT_FALSE(entry);
}

TEST_F(RegistryTest, TestFindEntry)
{
    auto path = RegistryTest::writeData(registryData);
    Registry registry{path};

    auto entry = registry.lookup("xyz.openbmc_project.Power.OverVoltage",
                                 LookupType::name);
    ASSERT_TRUE(entry);
    EXPECT_EQ(entry->name, "xyz.openbmc_project.Power.OverVoltage");
    EXPECT_EQ(entry->subsystem, 0x62);

    ASSERT_EQ(entry->severity->size(), 3);
    EXPECT_EQ((*entry->severity)[0].severity, 0x40);
    EXPECT_EQ((*entry->severity)[0].system, "systemA");
    EXPECT_EQ((*entry->severity)[1].severity, 0x10);
    EXPECT_EQ((*entry->severity)[1].system, "systemB");
    EXPECT_EQ((*entry->severity)[2].severity, 0x20);
    EXPECT_EQ((*entry->severity)[2].system, "");

    EXPECT_EQ(entry->mfgSeverity->size(), 1);
    EXPECT_EQ((*entry->mfgSeverity)[0].severity, 0x00);

    EXPECT_EQ(*(entry->actionFlags), 0xA800);
    EXPECT_EQ(*(entry->mfgActionFlags), 0x4000);
    EXPECT_EQ(entry->componentID, 0x2300);
    EXPECT_FALSE(entry->eventType);
    EXPECT_FALSE(entry->eventScope);

    EXPECT_EQ(entry->src.type, 0xBD);
    EXPECT_EQ(entry->src.reasonCode, 0x2333);
    EXPECT_TRUE(entry->src.deconfigFlag);
    EXPECT_TRUE(entry->src.checkstopFlag);

    auto& hexwords = entry->src.hexwordADFields;
    EXPECT_TRUE(hexwords);
    EXPECT_EQ((*hexwords).size(), 2);

    auto word = (*hexwords).find(6);
    EXPECT_NE(word, (*hexwords).end());
    EXPECT_EQ(std::get<0>(word->second), "PS_NUM");

    word = (*hexwords).find(7);
    EXPECT_NE(word, (*hexwords).end());
    EXPECT_EQ(std::get<0>(word->second), "VOLTAGE");

    auto& sid = entry->src.symptomID;
    EXPECT_TRUE(sid);
    EXPECT_EQ((*sid).size(), 3);
    EXPECT_NE(std::find((*sid).begin(), (*sid).end(), 5), (*sid).end());
    EXPECT_NE(std::find((*sid).begin(), (*sid).end(), 6), (*sid).end());
    EXPECT_NE(std::find((*sid).begin(), (*sid).end(), 7), (*sid).end());

    EXPECT_EQ(entry->doc.description, "A PGOOD Fault");
    EXPECT_EQ(entry->doc.message, "PS %1 had a PGOOD Fault");
    auto& hexwordSource = entry->doc.messageArgSources;
    EXPECT_TRUE(hexwordSource);
    EXPECT_EQ((*hexwordSource).size(), 1);
    EXPECT_EQ((*hexwordSource).front(), "SRCWord6");

    const auto& jc = entry->journalCapture;
    ASSERT_TRUE(jc);
    ASSERT_TRUE(std::holds_alternative<size_t>(*jc));
    EXPECT_EQ(std::get<size_t>(*jc), 7);

    entry = registry.lookup("0x2333", LookupType::reasonCode);
    ASSERT_TRUE(entry);
    EXPECT_EQ(entry->name, "xyz.openbmc_project.Power.OverVoltage");

    entry = registry.lookup("org.open_power.PHAL.Info.ClockDailyLog",
                            LookupType::name);
    ASSERT_TRUE(entry);
    EXPECT_EQ(entry->name, "org.open_power.PHAL.Info.ClockDailyLog");
    EXPECT_EQ(*(entry->actionFlags), 0x2820);
}

// Check the entry that mostly uses defaults
TEST_F(RegistryTest, TestFindEntryMinimal)
{
    auto path = RegistryTest::writeData(registryData);
    Registry registry{path};

    auto entry =
        registry.lookup("xyz.openbmc_project.Power.Fault", LookupType::name);
    ASSERT_TRUE(entry);
    EXPECT_EQ(entry->name, "xyz.openbmc_project.Power.Fault");
    EXPECT_EQ(entry->subsystem, 0x61);
    EXPECT_FALSE(entry->severity);
    EXPECT_FALSE(entry->mfgSeverity);
    EXPECT_FALSE(entry->mfgActionFlags);
    EXPECT_FALSE(entry->actionFlags);
    EXPECT_EQ(entry->componentID, 0x2000);
    EXPECT_FALSE(entry->eventType);
    EXPECT_FALSE(entry->eventScope);

    EXPECT_EQ(entry->src.reasonCode, 0x2030);
    EXPECT_EQ(entry->src.type, 0xBD);
    EXPECT_FALSE(entry->src.hexwordADFields);
    EXPECT_FALSE(entry->src.symptomID);
    EXPECT_FALSE(entry->src.deconfigFlag);
    EXPECT_FALSE(entry->src.checkstopFlag);
}

TEST_F(RegistryTest, TestBadJSON)
{
    auto path = RegistryTest::writeData("bad {} json");

    Registry registry{path};

    EXPECT_FALSE(registry.lookup("foo", LookupType::name));
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

    flags.push_back("heartbeat_call_home");
    EXPECT_EQ(getActionFlags(flags), 0x9120);
    flags.clear();
    flags.push_back("heartbeat_call_home");
    EXPECT_EQ(getActionFlags(flags), 0x0020);

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
            "Description": "TEST",
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
            "Description": "TEST",
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

// Test when callouts are in the JSON.
TEST_F(RegistryTest, TestGetCallouts)
{
    std::vector<std::string> systemNames;

    {
        // Callouts without AD, that depend on system type,
        // where there is a default entry without a system type.
        auto json = R"(
        [
        {
            "System": "system1",
            "CalloutList":
            [
                {
                    "Priority": "high",
                    "LocCode": "P1-C1"
                },
                {
                    "Priority": "low",
                    "LocCode": "P1"
                },
                {
                    "Priority": "low",
                    "SymbolicFRU": "service_docs"
                },
                {
                    "Priority": "low",
                    "SymbolicFRUTrusted": "air_mover",
                    "UseInventoryLocCode": true
                }
            ]
        },
        {
            "CalloutList":
            [
                {
                    "Priority": "medium",
                    "Procedure": "BMC0001"
                },
                {
                    "Priority": "low",
                    "LocCode": "P3-C8",
                    "SymbolicFRUTrusted": "service_docs"
                }
            ]

        }
        ])"_json;

        AdditionalData ad;
        systemNames.push_back("system1");

        auto callouts = Registry::getCallouts(json, systemNames, ad);
        EXPECT_EQ(callouts.size(), 4);
        EXPECT_EQ(callouts[0].priority, "high");
        EXPECT_EQ(callouts[0].locCode, "P1-C1");
        EXPECT_EQ(callouts[0].procedure, "");
        EXPECT_EQ(callouts[0].symbolicFRU, "");
        EXPECT_EQ(callouts[0].symbolicFRUTrusted, "");
        EXPECT_EQ(callouts[1].priority, "low");
        EXPECT_EQ(callouts[1].locCode, "P1");
        EXPECT_EQ(callouts[1].procedure, "");
        EXPECT_EQ(callouts[1].symbolicFRU, "");
        EXPECT_EQ(callouts[1].symbolicFRUTrusted, "");
        EXPECT_EQ(callouts[2].priority, "low");
        EXPECT_EQ(callouts[2].locCode, "");
        EXPECT_EQ(callouts[2].procedure, "");
        EXPECT_EQ(callouts[2].symbolicFRU, "service_docs");
        EXPECT_EQ(callouts[2].symbolicFRUTrusted, "");
        EXPECT_EQ(callouts[3].priority, "low");
        EXPECT_EQ(callouts[3].locCode, "");
        EXPECT_EQ(callouts[3].procedure, "");
        EXPECT_EQ(callouts[3].symbolicFRU, "");
        EXPECT_EQ(callouts[3].symbolicFRUTrusted, "air_mover");
        EXPECT_EQ(callouts[3].useInventoryLocCode, true);

        // system2 isn't in the JSON, so it will pick the default one
        systemNames[0] = "system2";
        callouts = Registry::getCallouts(json, systemNames, ad);
        EXPECT_EQ(callouts.size(), 2);
        EXPECT_EQ(callouts[0].priority, "medium");
        EXPECT_EQ(callouts[0].locCode, "");
        EXPECT_EQ(callouts[0].procedure, "BMC0001");
        EXPECT_EQ(callouts[0].symbolicFRU, "");
        EXPECT_EQ(callouts[1].priority, "low");
        EXPECT_EQ(callouts[1].locCode, "P3-C8");
        EXPECT_EQ(callouts[1].procedure, "");
        EXPECT_EQ(callouts[1].symbolicFRU, "");
        EXPECT_EQ(callouts[1].symbolicFRUTrusted, "service_docs");
        EXPECT_EQ(callouts[1].useInventoryLocCode, false);
    }

    // Empty JSON array (treated as an error)
    {
        auto json = R"([])"_json;
        AdditionalData ad;
        systemNames[0] = "system1";
        EXPECT_THROW(Registry::getCallouts(json, systemNames, ad),
                     std::runtime_error);
    }

    {
        // Callouts without AD, that depend on system type,
        // where there isn't a default entry without a system type.
        auto json = R"(
        [
        {
            "System": "system1",
            "CalloutList":
            [
                {
                    "Priority": "high",
                    "LocCode": "P1-C1"
                },
                {
                    "Priority": "low",
                    "LocCode": "P1",
                    "SymbolicFRU": "1234567"
                }
            ]
        },
        {
            "System": "system2",
            "CalloutList":
            [
                {
                    "Priority": "medium",
                    "LocCode": "P7",
                    "CalloutType": "tool_fru"
                }
            ]

        }
        ])"_json;

        AdditionalData ad;
        systemNames[0] = "system1";

        auto callouts = Registry::getCallouts(json, systemNames, ad);
        EXPECT_EQ(callouts.size(), 2);
        EXPECT_EQ(callouts[0].priority, "high");
        EXPECT_EQ(callouts[0].locCode, "P1-C1");
        EXPECT_EQ(callouts[0].procedure, "");
        EXPECT_EQ(callouts[0].symbolicFRU, "");
        EXPECT_EQ(callouts[0].symbolicFRUTrusted, "");
        EXPECT_EQ(callouts[1].priority, "low");
        EXPECT_EQ(callouts[1].locCode, "P1");
        EXPECT_EQ(callouts[1].procedure, "");
        EXPECT_EQ(callouts[1].symbolicFRU, "1234567");
        EXPECT_EQ(callouts[1].symbolicFRUTrusted, "");

        systemNames[0] = "system2";
        callouts = Registry::getCallouts(json, systemNames, ad);
        EXPECT_EQ(callouts.size(), 1);
        EXPECT_EQ(callouts[0].priority, "medium");
        EXPECT_EQ(callouts[0].locCode, "P7");
        EXPECT_EQ(callouts[0].procedure, "");
        EXPECT_EQ(callouts[0].symbolicFRU, "");
        EXPECT_EQ(callouts[0].symbolicFRUTrusted, "");

        // There is no entry for system3 or a default system,
        // so this should fail.
        systemNames[0] = "system3";
        EXPECT_THROW(Registry::getCallouts(json, systemNames, ad),
                     std::runtime_error);
    }

    {
        // Callouts that use the AdditionalData key PROC_NUM
        // as an index into them, along with a system type.
        // It supports PROC_NUMs 0 and 1.
        auto json = R"(
        {
            "ADName": "PROC_NUM",
            "CalloutsWithTheirADValues":
            [
                {
                    "ADValue": "0",
                    "Callouts":
                    [
                        {
                            "System": "system3",
                            "CalloutList":
                            [
                                {
                                    "Priority": "high",
                                    "LocCode": "P1-C5"
                                },
                                {
                                    "Priority": "medium",
                                    "LocCode": "P1-C6",
                                    "SymbolicFRU": "1234567"
                                },
                                {
                                    "Priority": "low",
                                    "Procedure": "BMC0001",
                                    "CalloutType": "config_procedure"
                                }
                            ]
                        },
                        {
                            "CalloutList":
                            [
                                {
                                    "Priority": "low",
                                    "LocCode": "P55"
                                }
                            ]
                        }
                    ]
                },
                {
                    "ADValue": "1",
                    "Callouts":
                    [
                        {
                            "CalloutList":
                            [
                                {
                                    "Priority": "high",
                                    "LocCode": "P1-C6",
                                    "CalloutType": "external_fru"
                                }
                            ]
                        }
                    ]
                }
            ]
        })"_json;

        {
            // Find callouts for PROC_NUM 0 on system3
            std::map<std::string, std::string> adData{{"PROC_NUM", "0"}};
            AdditionalData ad{adData};
            systemNames[0] = "system3";

            auto callouts = Registry::getCallouts(json, systemNames, ad);
            EXPECT_EQ(callouts.size(), 3);
            EXPECT_EQ(callouts[0].priority, "high");
            EXPECT_EQ(callouts[0].locCode, "P1-C5");
            EXPECT_EQ(callouts[0].procedure, "");
            EXPECT_EQ(callouts[0].symbolicFRU, "");
            EXPECT_EQ(callouts[0].symbolicFRUTrusted, "");
            EXPECT_EQ(callouts[1].priority, "medium");
            EXPECT_EQ(callouts[1].locCode, "P1-C6");
            EXPECT_EQ(callouts[1].procedure, "");
            EXPECT_EQ(callouts[1].symbolicFRU, "1234567");
            EXPECT_EQ(callouts[1].symbolicFRUTrusted, "");
            EXPECT_EQ(callouts[2].priority, "low");
            EXPECT_EQ(callouts[2].locCode, "");
            EXPECT_EQ(callouts[2].procedure, "BMC0001");
            EXPECT_EQ(callouts[2].symbolicFRU, "");
            EXPECT_EQ(callouts[2].symbolicFRUTrusted, "");

            // Find callouts for PROC_NUM 0 that uses the default system entry.
            systemNames[0] = "system99";

            callouts = Registry::getCallouts(json, systemNames, ad);
            EXPECT_EQ(callouts.size(), 1);
            EXPECT_EQ(callouts[0].priority, "low");
            EXPECT_EQ(callouts[0].locCode, "P55");
            EXPECT_EQ(callouts[0].procedure, "");
            EXPECT_EQ(callouts[0].symbolicFRU, "");
            EXPECT_EQ(callouts[0].symbolicFRUTrusted, "");
        }
        {
            // Find callouts for PROC_NUM 1 that uses a default system entry.
            std::map<std::string, std::string> adData{{"PROC_NUM", "1"}};
            AdditionalData ad{adData};
            systemNames[0] = "system1";

            auto callouts = Registry::getCallouts(json, systemNames, ad);
            EXPECT_EQ(callouts.size(), 1);
            EXPECT_EQ(callouts[0].priority, "high");
            EXPECT_EQ(callouts[0].locCode, "P1-C6");
            EXPECT_EQ(callouts[0].procedure, "");
            EXPECT_EQ(callouts[0].symbolicFRU, "");
            EXPECT_EQ(callouts[0].symbolicFRUTrusted, "");
        }
        {
            // There is no entry for PROC_NUM 2, so no callouts
            std::map<std::string, std::string> adData{{"PROC_NUM", "2"}};
            AdditionalData ad{adData};

            auto callouts = Registry::getCallouts(json, systemNames, ad);
            EXPECT_TRUE(callouts.empty());
        }
    }

    {
        // Callouts with a 'CalloutsWhenNoADMatch' section that will
        // be used when the AdditionalData value doesn't match.
        auto json = R"(
        {
            "ADName": "PROC_NUM",
            "CalloutsWithTheirADValues":
            [
                {
                    "ADValue": "0",
                    "Callouts":
                    [
                        {
                            "CalloutList":
                            [
                                {
                                    "Priority": "high",
                                    "LocCode": "P0-C0"
                                }
                            ]
                        }
                    ]
                }
            ],
            "CalloutsWhenNoADMatch": [
                {
                    "CalloutList": [
                        {
                            "Priority": "medium",
                            "LocCode": "P1-C1"
                        }
                    ]
                }
            ]
        })"_json;

        // There isn't an entry in the JSON for a PROC_NUM of 8
        // so it should choose the P1-C1 callout.
        std::map<std::string, std::string> adData{{"PROC_NUM", "8"}};
        AdditionalData ad{adData};
        systemNames.clear();

        auto callouts = Registry::getCallouts(json, systemNames, ad);
        EXPECT_EQ(callouts.size(), 1);
        EXPECT_EQ(callouts[0].priority, "medium");
        EXPECT_EQ(callouts[0].locCode, "P1-C1");
    }
}

TEST_F(RegistryTest, TestGetCalloutsWithSystems)
{
    std::vector<std::string> systemNames;

    auto json = R"(
        [
        {
            "Systems": ["system1", "system2"],
            "CalloutList":
            [
                {
                    "Priority": "high",
                    "LocCode": "P1-C1"
                },
                {
                    "Priority": "low",
                    "LocCode": "P1"
                },
                {
                    "Priority": "low",
                    "SymbolicFRU": "service_docs"
                },
                {
                    "Priority": "low",
                    "SymbolicFRUTrusted": "air_mover",
                    "UseInventoryLocCode": true
                }
            ]
        },
        {
            "CalloutList":
            [
                {
                    "Priority": "medium",
                    "Procedure": "BMC0001"
                },
                {
                    "Priority": "low",
                    "LocCode": "P3-C8",
                    "SymbolicFRUTrusted": "service_docs"
                }
            ]

        }
        ])"_json;

    AdditionalData ad;
    systemNames.push_back("system1");

    auto callouts = Registry::getCallouts(json, systemNames, ad);
    EXPECT_EQ(callouts.size(), 4);
    EXPECT_EQ(callouts[0].priority, "high");
    EXPECT_EQ(callouts[0].locCode, "P1-C1");
    EXPECT_EQ(callouts[0].procedure, "");
    EXPECT_EQ(callouts[0].symbolicFRU, "");
    EXPECT_EQ(callouts[0].symbolicFRUTrusted, "");
    EXPECT_EQ(callouts[1].priority, "low");
    EXPECT_EQ(callouts[1].locCode, "P1");
    EXPECT_EQ(callouts[1].procedure, "");
    EXPECT_EQ(callouts[1].symbolicFRU, "");
    EXPECT_EQ(callouts[1].symbolicFRUTrusted, "");
    EXPECT_EQ(callouts[2].priority, "low");
    EXPECT_EQ(callouts[2].locCode, "");
    EXPECT_EQ(callouts[2].procedure, "");
    EXPECT_EQ(callouts[2].symbolicFRU, "service_docs");
    EXPECT_EQ(callouts[2].symbolicFRUTrusted, "");
    EXPECT_EQ(callouts[3].priority, "low");
    EXPECT_EQ(callouts[3].locCode, "");
    EXPECT_EQ(callouts[3].procedure, "");
    EXPECT_EQ(callouts[3].symbolicFRU, "");
    EXPECT_EQ(callouts[3].symbolicFRUTrusted, "air_mover");
    EXPECT_EQ(callouts[3].useInventoryLocCode, true);

    // System3 isn't in the JSON, so it will pick the default one
    systemNames[0] = "system3";

    callouts = Registry::getCallouts(json, systemNames, ad);
    EXPECT_EQ(callouts.size(), 2);
    EXPECT_EQ(callouts[0].priority, "medium");
    EXPECT_EQ(callouts[0].locCode, "");
    EXPECT_EQ(callouts[0].procedure, "BMC0001");
    EXPECT_EQ(callouts[0].symbolicFRU, "");
    EXPECT_EQ(callouts[1].priority, "low");
    EXPECT_EQ(callouts[1].locCode, "P3-C8");
    EXPECT_EQ(callouts[1].procedure, "");
    EXPECT_EQ(callouts[1].symbolicFRU, "");
    EXPECT_EQ(callouts[1].symbolicFRUTrusted, "service_docs");
    EXPECT_EQ(callouts[1].useInventoryLocCode, false);
}

TEST_F(RegistryTest, TestGetCalloutsWithSystemAndSystems)
{
    std::vector<std::string> systemNames;

    auto json = R"(
        [
        {
            "Systems": ["system1", "system2"],
            "CalloutList":
            [
                {
                    "Priority": "high",
                    "LocCode": "P1-C1"
                },
                {
                    "Priority": "low",
                    "LocCode": "P1"
                }
            ]
        },
        {
            "System": "system1",
            "CalloutList":
            [
                {
                    "Priority": "low",
                    "SymbolicFRU": "service_docs"
                },
                {
                    "Priority": "low",
                    "SymbolicFRUTrusted": "air_mover",
                    "UseInventoryLocCode": true
                }
            ]
        },
        {
            "CalloutList":
            [
                {
                    "Priority": "medium",
                    "Procedure": "BMC0001"
                },
                {
                    "Priority": "low",
                    "LocCode": "P3-C8",
                    "SymbolicFRUTrusted": "service_docs"
                }
            ]
        }
        ])"_json;

    AdditionalData ad;
    systemNames.push_back("system1");

    auto callouts = Registry::getCallouts(json, systemNames, ad);
    EXPECT_EQ(callouts.size(), 4);
    EXPECT_EQ(callouts[0].priority, "high");
    EXPECT_EQ(callouts[0].locCode, "P1-C1");
    EXPECT_EQ(callouts[0].procedure, "");
    EXPECT_EQ(callouts[0].symbolicFRU, "");
    EXPECT_EQ(callouts[0].symbolicFRUTrusted, "");
    EXPECT_EQ(callouts[1].priority, "low");
    EXPECT_EQ(callouts[1].locCode, "P1");
    EXPECT_EQ(callouts[1].procedure, "");
    EXPECT_EQ(callouts[1].symbolicFRU, "");
    EXPECT_EQ(callouts[1].symbolicFRUTrusted, "");
    EXPECT_EQ(callouts[2].priority, "low");
    EXPECT_EQ(callouts[2].locCode, "");
    EXPECT_EQ(callouts[2].procedure, "");
    EXPECT_EQ(callouts[2].symbolicFRU, "service_docs");
    EXPECT_EQ(callouts[2].symbolicFRUTrusted, "");
    EXPECT_EQ(callouts[3].priority, "low");
    EXPECT_EQ(callouts[3].locCode, "");
    EXPECT_EQ(callouts[3].procedure, "");
    EXPECT_EQ(callouts[3].symbolicFRU, "");
    EXPECT_EQ(callouts[3].symbolicFRUTrusted, "air_mover");
    EXPECT_EQ(callouts[3].useInventoryLocCode, true);

    // if system name is "System2"
    systemNames[0] = "system2";

    callouts = Registry::getCallouts(json, systemNames, ad);
    EXPECT_EQ(callouts.size(), 2);
    EXPECT_EQ(callouts[0].priority, "high");
    EXPECT_EQ(callouts[0].locCode, "P1-C1");
    EXPECT_EQ(callouts[0].procedure, "");
    EXPECT_EQ(callouts[0].symbolicFRU, "");
    EXPECT_EQ(callouts[0].symbolicFRUTrusted, "");
    EXPECT_EQ(callouts[1].priority, "low");
    EXPECT_EQ(callouts[1].locCode, "P1");
    EXPECT_EQ(callouts[1].procedure, "");
    EXPECT_EQ(callouts[1].symbolicFRU, "");
    EXPECT_EQ(callouts[1].symbolicFRUTrusted, "");

    // system name is System3 which is not in json thereby will take default
    systemNames[0] = "system3";

    callouts = Registry::getCallouts(json, systemNames, ad);
    EXPECT_EQ(callouts.size(), 2);
    EXPECT_EQ(callouts[0].priority, "medium");
    EXPECT_EQ(callouts[0].locCode, "");
    EXPECT_EQ(callouts[0].procedure, "BMC0001");
    EXPECT_EQ(callouts[0].symbolicFRU, "");
    EXPECT_EQ(callouts[1].priority, "low");
    EXPECT_EQ(callouts[1].locCode, "P3-C8");
    EXPECT_EQ(callouts[1].procedure, "");
    EXPECT_EQ(callouts[1].symbolicFRU, "");
    EXPECT_EQ(callouts[1].symbolicFRUTrusted, "service_docs");
    EXPECT_EQ(callouts[1].useInventoryLocCode, false);
}

TEST_F(RegistryTest, TestGetCalloutsWithOnlySystemAndSystems)
{
    std::vector<std::string> systemNames;

    auto json = R"(
        [
        {
            "Systems": ["system1", "system2"],
            "CalloutList":
            [
                {
                    "Priority": "high",
                    "LocCode": "P1-C1"
                },
                {
                    "Priority": "low",
                    "LocCode": "P1"
                }
            ]
        },
        {
            "System": "system1",
            "CalloutList":
            [
                {
                    "Priority": "low",
                    "SymbolicFRU": "service_docs"
                },
                {
                    "Priority": "low",
                    "SymbolicFRUTrusted": "air_mover",
                    "UseInventoryLocCode": true
                }
            ]
        }
        ])"_json;

    AdditionalData ad;
    systemNames.push_back("system1");

    auto callouts = Registry::getCallouts(json, systemNames, ad);
    EXPECT_EQ(callouts.size(), 4);
    EXPECT_EQ(callouts[0].priority, "high");
    EXPECT_EQ(callouts[0].locCode, "P1-C1");
    EXPECT_EQ(callouts[0].procedure, "");
    EXPECT_EQ(callouts[0].symbolicFRU, "");
    EXPECT_EQ(callouts[0].symbolicFRUTrusted, "");
    EXPECT_EQ(callouts[1].priority, "low");
    EXPECT_EQ(callouts[1].locCode, "P1");
    EXPECT_EQ(callouts[1].procedure, "");
    EXPECT_EQ(callouts[1].symbolicFRU, "");
    EXPECT_EQ(callouts[1].symbolicFRUTrusted, "");
    EXPECT_EQ(callouts[2].priority, "low");
    EXPECT_EQ(callouts[2].locCode, "");
    EXPECT_EQ(callouts[2].procedure, "");
    EXPECT_EQ(callouts[2].symbolicFRU, "service_docs");
    EXPECT_EQ(callouts[2].symbolicFRUTrusted, "");
    EXPECT_EQ(callouts[3].priority, "low");
    EXPECT_EQ(callouts[3].locCode, "");
    EXPECT_EQ(callouts[3].procedure, "");
    EXPECT_EQ(callouts[3].symbolicFRU, "");
    EXPECT_EQ(callouts[3].symbolicFRUTrusted, "air_mover");
    EXPECT_EQ(callouts[3].useInventoryLocCode, true);

    // if system name is "System2"
    systemNames[0] = "system2";

    callouts = Registry::getCallouts(json, systemNames, ad);
    EXPECT_EQ(callouts.size(), 2);
    EXPECT_EQ(callouts[0].priority, "high");
    EXPECT_EQ(callouts[0].locCode, "P1-C1");
    EXPECT_EQ(callouts[0].procedure, "");
    EXPECT_EQ(callouts[0].symbolicFRU, "");
    EXPECT_EQ(callouts[0].symbolicFRUTrusted, "");
    EXPECT_EQ(callouts[1].priority, "low");
    EXPECT_EQ(callouts[1].locCode, "P1");
    EXPECT_EQ(callouts[1].procedure, "");
    EXPECT_EQ(callouts[1].symbolicFRU, "");
    EXPECT_EQ(callouts[1].symbolicFRUTrusted, "");

    // There is no entry for system3 or a default system,
    // so this should fail.
    systemNames[0] = "system3";
    EXPECT_THROW(Registry::getCallouts(json, systemNames, ad),
                 std::runtime_error);
}

TEST_F(RegistryTest, TestGetCalloutsWithOnlySystems)
{
    std::vector<std::string> systemNames;

    auto json = R"(
        [
        {
            "Systems": ["system1", "system2"],
            "CalloutList":
            [
                {
                    "Priority": "high",
                    "LocCode": "P1-C1"
                },
                {
                    "Priority": "low",
                    "LocCode": "P1"
                }
            ]
        }
        ])"_json;

    AdditionalData ad;
    systemNames.push_back("system1");

    // system1 is available in JSON array
    auto callouts = Registry::getCallouts(json, systemNames, ad);
    EXPECT_EQ(callouts.size(), 2);
    EXPECT_EQ(callouts[0].priority, "high");
    EXPECT_EQ(callouts[0].locCode, "P1-C1");
    EXPECT_EQ(callouts[0].procedure, "");
    EXPECT_EQ(callouts[0].symbolicFRU, "");
    EXPECT_EQ(callouts[0].symbolicFRUTrusted, "");
    EXPECT_EQ(callouts[1].priority, "low");
    EXPECT_EQ(callouts[1].locCode, "P1");
    EXPECT_EQ(callouts[1].procedure, "");
    EXPECT_EQ(callouts[1].symbolicFRU, "");
    EXPECT_EQ(callouts[1].symbolicFRUTrusted, "");

    // There is no entry for system3 or a default system,
    // so this should fail.
    systemNames[0] = "system3";
    EXPECT_THROW(Registry::getCallouts(json, systemNames, ad),
                 std::runtime_error);
}

TEST_F(RegistryTest, TestGetCalloutsWithOnlyDefaults)
{
    std::vector<std::string> systemNames;

    auto json = R"(
        [
        {
            "CalloutList":
            [
                {
                    "Priority": "high",
                    "LocCode": "P1-C1"
                },
                {
                    "Priority": "low",
                    "LocCode": "P1"
                }
            ]
        }
        ])"_json;

    AdditionalData ad;
    systemNames.push_back("system1");

    // Since neither System or Systems available, it will pick the default one
    // only
    auto callouts = Registry::getCallouts(json, systemNames, ad);
    EXPECT_EQ(callouts.size(), 2);
    EXPECT_EQ(callouts[0].priority, "high");
    EXPECT_EQ(callouts[0].locCode, "P1-C1");
    EXPECT_EQ(callouts[0].procedure, "");
    EXPECT_EQ(callouts[0].symbolicFRU, "");
    EXPECT_EQ(callouts[0].symbolicFRUTrusted, "");
    EXPECT_EQ(callouts[1].priority, "low");
    EXPECT_EQ(callouts[1].locCode, "P1");
    EXPECT_EQ(callouts[1].procedure, "");
    EXPECT_EQ(callouts[1].symbolicFRU, "");
    EXPECT_EQ(callouts[1].symbolicFRUTrusted, "");
}

TEST_F(RegistryTest, TestNoSubsystem)
{
    auto path = RegistryTest::writeData(registryData);
    Registry registry{path};

    auto entry = registry.lookup("xyz.openbmc_project.Common.Error.Timeout",
                                 LookupType::name);
    ASSERT_TRUE(entry);
    EXPECT_FALSE(entry->subsystem);
}

TEST_F(RegistryTest, TestJournalSectionCapture)
{
    auto path = RegistryTest::writeData(registryData);
    Registry registry{path};

    auto entry = registry.lookup("xyz.openbmc_project.Journal.Capture",
                                 LookupType::name);
    ASSERT_TRUE(entry);

    const auto& jc = entry->journalCapture;
    ASSERT_TRUE(jc);
    ASSERT_TRUE(std::holds_alternative<AppCaptureList>(*jc));
    const auto& acl = std::get<AppCaptureList>(*jc);

    ASSERT_EQ(acl.size(), 2);

    EXPECT_EQ(acl[0].syslogID, "test1");
    EXPECT_EQ(acl[0].numLines, 5);

    EXPECT_EQ(acl[1].syslogID, "test2");
    EXPECT_EQ(acl[1].numLines, 6);
}
