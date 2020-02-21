/**
 * Copyright © 2019 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "extensions/openpower-pels/src.hpp"
#include "mocks.hpp"
#include "pel_utils.hpp"

#include <fstream>

#include <gtest/gtest.h>

using namespace openpower::pels;
using ::testing::NiceMock;
using ::testing::Return;
namespace fs = std::filesystem;

const auto testRegistry = R"(
{
"PELs":
[
    {
        "Name": "xyz.openbmc_project.Error.Test",
        "Subsystem": "bmc_firmware",
        "SRC":
        {
            "ReasonCode": "0xABCD",
            "Words6To9":
            {
                "6":
                {
                    "Description": "Component ID",
                    "AdditionalDataPropSource": "COMPID"
                },
                "7":
                {
                    "Description": "Failure count",
                    "AdditionalDataPropSource": "FREQUENCY"
                },
                "8":
                {
                    "Description": "Time period",
                    "AdditionalDataPropSource": "DURATION"
                },
                "9":
                {
                    "Description": "Error code",
                    "AdditionalDataPropSource": "ERRORCODE"
                }
            }
        },
        "Documentation":
        {
            "Description": "A Component Fault",
            "Message": "Comp %1 failed %2 times over %3 secs with ErrorCode %4",
            "MessageArgSources":
            [
                "SRCWord6", "SRCWord7", "SRCWord8", "SRCWord9"
            ]
        }
    }
]
}
)";

class SRCTest : public ::testing::Test
{
  protected:
    static void SetUpTestCase()
    {
        char path[] = "/tmp/srctestXXXXXX";
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

fs::path SRCTest::regDir{};

TEST_F(SRCTest, UnflattenFlattenTestNoCallouts)
{
    auto data = pelDataFactory(TestPELType::primarySRCSection);

    Stream stream{data};
    SRC src{stream};

    EXPECT_TRUE(src.valid());

    EXPECT_EQ(src.header().id, 0x5053);
    EXPECT_EQ(src.header().size, 0x50);
    EXPECT_EQ(src.header().version, 0x01);
    EXPECT_EQ(src.header().subType, 0x01);
    EXPECT_EQ(src.header().componentID, 0x0202);

    EXPECT_EQ(src.version(), 0x02);
    EXPECT_EQ(src.flags(), 0x00);
    EXPECT_EQ(src.hexWordCount(), 9);
    EXPECT_EQ(src.size(), 0x48);

    const auto& hexwords = src.hexwordData();
    EXPECT_EQ(0x02020255, hexwords[0]);
    EXPECT_EQ(0x03030310, hexwords[1]);
    EXPECT_EQ(0x04040404, hexwords[2]);
    EXPECT_EQ(0x05050505, hexwords[3]);
    EXPECT_EQ(0x06060606, hexwords[4]);
    EXPECT_EQ(0x07070707, hexwords[5]);
    EXPECT_EQ(0x08080808, hexwords[6]);
    EXPECT_EQ(0x09090909, hexwords[7]);

    EXPECT_EQ(src.asciiString(), "BD8D5678                        ");
    EXPECT_FALSE(src.callouts());

    // Flatten
    std::vector<uint8_t> newData;
    Stream newStream{newData};

    src.flatten(newStream);
    EXPECT_EQ(data, newData);
}

TEST_F(SRCTest, UnflattenFlattenTest2Callouts)
{
    auto data = pelDataFactory(TestPELType::primarySRCSection2Callouts);

    Stream stream{data};
    SRC src{stream};

    EXPECT_TRUE(src.valid());
    EXPECT_EQ(src.flags(), 0x01); // Additional sections within the SRC.

    // Spot check the SRC fields, but they're the same as above
    EXPECT_EQ(src.asciiString(), "BD8D5678                        ");

    // There should be 2 callouts
    const auto& calloutsSection = src.callouts();
    ASSERT_TRUE(calloutsSection);
    const auto& callouts = calloutsSection->callouts();
    EXPECT_EQ(callouts.size(), 2);

    // spot check that each callout has the right substructures
    EXPECT_TRUE(callouts.front()->fruIdentity());
    EXPECT_FALSE(callouts.front()->pceIdentity());
    EXPECT_FALSE(callouts.front()->mru());

    EXPECT_TRUE(callouts.back()->fruIdentity());
    EXPECT_TRUE(callouts.back()->pceIdentity());
    EXPECT_TRUE(callouts.back()->mru());

    // Flatten
    std::vector<uint8_t> newData;
    Stream newStream{newData};

    src.flatten(newStream);
    EXPECT_EQ(data, newData);
}

// Create an SRC from the message registry
TEST_F(SRCTest, CreateTestNoCallouts)
{
    message::Entry entry;
    entry.src.type = 0xBD;
    entry.src.reasonCode = 0xABCD;
    entry.subsystem = 0x42;
    entry.src.powerFault = true;
    entry.src.hexwordADFields = {{5, "TEST1"}, // Not a user defined word
                                 {6, "TEST1"},
                                 {7, "TEST2"},
                                 {8, "TEST3"},
                                 {9, "TEST4"}};

    // Values for the SRC words pointed to above
    std::vector<std::string> adData{"TEST1=0x12345678", "TEST2=12345678",
                                    "TEST3=0XDEF", "TEST4=Z"};
    AdditionalData ad{adData};
    NiceMock<MockDataInterface> dataIface;

    EXPECT_CALL(dataIface, getMotherboardCCIN).WillOnce(Return("ABCD"));

    SRC src{entry, ad, dataIface};

    EXPECT_TRUE(src.valid());
    EXPECT_TRUE(src.isPowerFaultEvent());
    EXPECT_EQ(src.size(), baseSRCSize);

    const auto& hexwords = src.hexwordData();

    // The spec always refers to SRC words 2 - 9, and as the hexwordData()
    // array index starts at 0 use the math in the [] below to make it easier
    // to tell what is being accessed.
    EXPECT_EQ(hexwords[2 - 2] & 0xF0000000, 0);    // Partition dump status
    EXPECT_EQ(hexwords[2 - 2] & 0x00F00000, 0);    // Partition boot type
    EXPECT_EQ(hexwords[2 - 2] & 0x000000FF, 0x55); // SRC format
    EXPECT_EQ(hexwords[3 - 2] & 0x000000FF, 0x10); // BMC position
    EXPECT_EQ(hexwords[3 - 2] & 0xFFFF0000, 0xABCD0000); // Motherboard CCIN

    // Validate more fields here as the code starts filling them in.

    // Ensure hex word 5 wasn't allowed to be set to TEST1's contents
    EXPECT_EQ(hexwords[5 - 2], 0);

    // The user defined hex word fields specifed in the additional data.
    EXPECT_EQ(hexwords[6 - 2], 0x12345678); // TEST1
    EXPECT_EQ(hexwords[7 - 2], 12345678);   // TEST2
    EXPECT_EQ(hexwords[8 - 2], 0xdef);      // TEST3
    EXPECT_EQ(hexwords[9 - 2], 0);          // TEST4, but can't convert a 'Z'

    EXPECT_EQ(src.asciiString(), "BD42ABCD                        ");

    // No callouts
    EXPECT_FALSE(src.callouts());

    // May as well spot check the flatten/unflatten
    std::vector<uint8_t> data;
    Stream stream{data};
    src.flatten(stream);

    stream.offset(0);
    SRC newSRC{stream};

    EXPECT_TRUE(newSRC.valid());
    EXPECT_EQ(newSRC.isPowerFaultEvent(), src.isPowerFaultEvent());
    EXPECT_EQ(newSRC.asciiString(), src.asciiString());
    EXPECT_FALSE(newSRC.callouts());
}

// Test when the CCIN string isn't a 4 character number
TEST_F(SRCTest, BadCCINTest)
{
    message::Entry entry;
    entry.src.type = 0xBD;
    entry.src.reasonCode = 0xABCD;
    entry.subsystem = 0x42;
    entry.src.powerFault = false;

    std::vector<std::string> adData{};
    AdditionalData ad{adData};
    NiceMock<MockDataInterface> dataIface;

    // First it isn't a number, then it is too long,
    // then it is empty.
    EXPECT_CALL(dataIface, getMotherboardCCIN)
        .WillOnce(Return("X"))
        .WillOnce(Return("12345"))
        .WillOnce(Return(""));

    // The CCIN in the first half should still be 0 each time.
    {
        SRC src{entry, ad, dataIface};
        EXPECT_TRUE(src.valid());
        const auto& hexwords = src.hexwordData();
        EXPECT_EQ(hexwords[3 - 2] & 0xFFFF0000, 0x00000000);
    }

    {
        SRC src{entry, ad, dataIface};
        EXPECT_TRUE(src.valid());
        const auto& hexwords = src.hexwordData();
        EXPECT_EQ(hexwords[3 - 2] & 0xFFFF0000, 0x00000000);
    }

    {
        SRC src{entry, ad, dataIface};
        EXPECT_TRUE(src.valid());
        const auto& hexwords = src.hexwordData();
        EXPECT_EQ(hexwords[3 - 2] & 0xFFFF0000, 0x00000000);
    }
}

// Test the getErrorDetails function
TEST_F(SRCTest, MessageSubstitutionTest)
{
    auto path = SRCTest::writeData(testRegistry);
    message::Registry registry{path};
    auto entry = registry.lookup("0xABCD", message::LookupType::reasonCode);

    std::vector<std::string> adData{"COMPID=0x1", "FREQUENCY=0x4",
                                    "DURATION=30", "ERRORCODE=0x01ABCDEF"};
    AdditionalData ad{adData};
    NiceMock<MockDataInterface> dataIface;

    SRC src{*entry, ad, dataIface};
    EXPECT_TRUE(src.valid());

    auto errorDetails = src.getErrorDetails(registry, DetailLevel::message);
    ASSERT_TRUE(errorDetails);
    EXPECT_EQ(
        errorDetails.value(),
        "Comp 0x1 failed 0x4 times over 0x1E secs with ErrorCode 0x1ABCDEF");
}
