// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "extensions/openpower-pels/src.hpp"
#include "mocks.hpp"
#include "pel_utils.hpp"

#include <fstream>

#include <gtest/gtest.h>

using namespace openpower::pels;
using ::testing::_;
using ::testing::DoAll;
using ::testing::InvokeWithoutArgs;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::Throw;
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
    entry.src.hexwordADFields = {
        {5, {"TEST1", "DESCR1"}}, // Not a user defined word
        {6, {"TEST1", "DESCR1"}},
        {7, {"TEST2", "DESCR2"}},
        {8, {"TEST3", "DESCR3"}},
        {9, {"TEST4", "DESCR4"}}};

    // Values for the SRC words pointed to above
    std::map<std::string, std::string> adData{
        {"TEST1", "0x12345678"},
        {"TEST2", "12345678"},
        {"TEST3", "0XDEF"},
        {"TEST4", "Z"}};
    AdditionalData ad{adData};
    NiceMock<MockDataInterface> dataIface;

    EXPECT_CALL(dataIface, getMotherboardCCIN).WillOnce(Return("ABCD"));

    SRC src{entry, ad, dataIface};

    EXPECT_TRUE(src.valid());
    EXPECT_FALSE(src.isPowerFaultEvent());
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
    // And that none of the error status flags are set
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

    std::map<std::string, std::string> adData{};
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

    std::map<std::string, std::string> adData{
        {"COMPID", "0x1"},
        {"FREQUENCY", "0x4"},
        {"DURATION", "30"},
        {"ERRORCODE", "0x01ABCDEF"}};
    AdditionalData ad{adData};
    NiceMock<MockDataInterface> dataIface;

    SRC src{*entry, ad, dataIface};
    EXPECT_TRUE(src.valid());

    auto errorDetails = src.getErrorDetails(registry, DetailLevel::message);
    ASSERT_TRUE(errorDetails);
    EXPECT_EQ(errorDetails.value(),
              "Comp 0x00000001 failed 0x00000004 times over 0x0000001E secs "
              "with ErrorCode 0x01ABCDEF");
}
// Test that an inventory path callout string is
// converted into the appropriate FRU callout.
TEST_F(SRCTest, InventoryCalloutTest)
{
    message::Entry entry;
    entry.src.type = 0xBD;
    entry.src.reasonCode = 0xABCD;
    entry.subsystem = 0x42;

    std::map<std::string, std::string> adData{
        {"CALLOUT_INVENTORY_PATH", "motherboard"}};
    AdditionalData ad{adData};
    NiceMock<MockDataInterface> dataIface;

    EXPECT_CALL(dataIface, getLocationCode("motherboard"))
        .WillOnce(Return("UTMS-P1"));

    EXPECT_CALL(dataIface, getHWCalloutFields("motherboard", _, _, _))
        .Times(1)
        .WillOnce(DoAll(SetArgReferee<1>("1234567"), SetArgReferee<2>("CCCC"),
                        SetArgReferee<3>("123456789ABC")));

    SRC src{entry, ad, dataIface};
    EXPECT_TRUE(src.valid());

    ASSERT_TRUE(src.callouts());

    EXPECT_EQ(src.callouts()->callouts().size(), 1);

    auto& callout = src.callouts()->callouts().front();

    EXPECT_EQ(callout->locationCode(), "UTMS-P1");
    EXPECT_EQ(callout->priority(), 'H');

    auto& fru = callout->fruIdentity();

    EXPECT_EQ(fru->getPN().value(), "1234567");
    EXPECT_EQ(fru->getCCIN().value(), "CCCC");
    EXPECT_EQ(fru->getSN().value(), "123456789ABC");

    // flatten and unflatten
    std::vector<uint8_t> data;
    Stream stream{data};
    src.flatten(stream);

    stream.offset(0);
    SRC newSRC{stream};
    EXPECT_TRUE(newSRC.valid());
    ASSERT_TRUE(src.callouts());
    EXPECT_EQ(src.callouts()->callouts().size(), 1);
}

// Test that when the location code can't be obtained that
// no callout is added.
TEST_F(SRCTest, InventoryCalloutNoLocCodeTest)
{
    message::Entry entry;
    entry.src.type = 0xBD;
    entry.src.reasonCode = 0xABCD;
    entry.subsystem = 0x42;

    std::map<std::string, std::string> adData{
        {"CALLOUT_INVENTORY_PATH", "motherboard"}};
    AdditionalData ad{adData};
    NiceMock<MockDataInterface> dataIface;

    auto func = []() {
        throw sdbusplus::exception::SdBusError(5, "Error");
        return std::string{};
    };

    EXPECT_CALL(dataIface, getLocationCode("motherboard"))
        .Times(1)
        .WillOnce(InvokeWithoutArgs(func));

    EXPECT_CALL(dataIface, getHWCalloutFields(_, _, _, _)).Times(0);

    SRC src{entry, ad, dataIface};
    EXPECT_TRUE(src.valid());

    ASSERT_FALSE(src.callouts());

    // flatten and unflatten
    std::vector<uint8_t> data;
    Stream stream{data};
    src.flatten(stream);

    stream.offset(0);
    SRC newSRC{stream};
    EXPECT_TRUE(newSRC.valid());
    ASSERT_FALSE(src.callouts());
}

// Test that when the VPD can't be obtained that
// a callout is still created.
TEST_F(SRCTest, InventoryCalloutNoVPDTest)
{
    message::Entry entry;
    entry.src.type = 0xBD;
    entry.src.reasonCode = 0xABCD;
    entry.subsystem = 0x42;

    std::map<std::string, std::string> adData{
        {"CALLOUT_INVENTORY_PATH", "motherboard"}};
    AdditionalData ad{adData};
    NiceMock<MockDataInterface> dataIface;

    EXPECT_CALL(dataIface, getLocationCode("motherboard"))
        .Times(1)
        .WillOnce(Return("UTMS-P10"));

    auto func = []() { throw sdbusplus::exception::SdBusError(5, "Error"); };

    EXPECT_CALL(dataIface, getHWCalloutFields("motherboard", _, _, _))
        .Times(1)
        .WillOnce(InvokeWithoutArgs(func));

    SRC src{entry, ad, dataIface};
    EXPECT_TRUE(src.valid());
    ASSERT_TRUE(src.callouts());
    EXPECT_EQ(src.callouts()->callouts().size(), 1);

    auto& callout = src.callouts()->callouts().front();
    EXPECT_EQ(callout->locationCode(), "UTMS-P10");
    EXPECT_EQ(callout->priority(), 'H');

    auto& fru = callout->fruIdentity();

    EXPECT_EQ(fru->getPN(), "");
    EXPECT_EQ(fru->getCCIN(), "");
    EXPECT_EQ(fru->getSN(), "");
    EXPECT_FALSE(fru->getMaintProc());

    // flatten and unflatten
    std::vector<uint8_t> data;
    Stream stream{data};
    src.flatten(stream);

    stream.offset(0);
    SRC newSRC{stream};
    EXPECT_TRUE(newSRC.valid());
    ASSERT_TRUE(src.callouts());
    EXPECT_EQ(src.callouts()->callouts().size(), 1);
}

TEST_F(SRCTest, RegistryCalloutTest)
{
    message::Entry entry;
    entry.src.type = 0xBD;
    entry.src.reasonCode = 0xABCD;
    entry.src.deconfigFlag = true;
    entry.src.checkstopFlag = true;
    entry.subsystem = 0x42;

    entry.callouts = R"(
        [
        {
            "System": "systemA",
            "CalloutList":
            [
                {
                    "Priority": "high",
                    "SymbolicFRU": "service_docs"
                },
                {
                    "Priority": "medium",
                    "Procedure": "BMC0001"
                }
            ]
        },
        {
            "System": "systemB",
            "CalloutList":
            [
                {
                    "Priority": "high",
                    "LocCode": "P0-C8",
                    "SymbolicFRUTrusted": "service_docs"
                },
                {
                    "Priority": "medium",
                    "SymbolicFRUTrusted": "service_docs"
                }
            ]
        },
        {
            "System": "systemC",
            "CalloutList":
            [
                {
                    "Priority": "high",
                    "LocCode": "P0-C8"
                },
                {
                    "Priority": "medium",
                    "LocCode": "P0-C9"
                }
            ]
        }
        ])"_json;

    {
        // Call out a symbolic FRU and a procedure
        AdditionalData ad;
        NiceMock<MockDataInterface> dataIface;
        std::vector<std::string> names{"systemA"};

        EXPECT_CALL(dataIface, getSystemNames).WillOnce(Return(names));

        SRC src{entry, ad, dataIface};

        EXPECT_TRUE(
            src.getErrorStatusFlag(SRC::ErrorStatusFlags::deconfigured));
        EXPECT_TRUE(src.getErrorStatusFlag(SRC::ErrorStatusFlags::hwCheckstop));

        const auto& hexwords = src.hexwordData();
        auto mask = static_cast<uint32_t>(SRC::ErrorStatusFlags::deconfigured) |
                    static_cast<uint32_t>(SRC::ErrorStatusFlags::hwCheckstop);
        EXPECT_EQ(hexwords[5 - 2] & mask, mask);

        auto& callouts = src.callouts()->callouts();
        ASSERT_EQ(callouts.size(), 2);

        EXPECT_EQ(callouts[0]->locationCodeSize(), 0);
        EXPECT_EQ(callouts[0]->priority(), 'H');

        EXPECT_EQ(callouts[1]->locationCodeSize(), 0);
        EXPECT_EQ(callouts[1]->priority(), 'M');

        auto& fru1 = callouts[0]->fruIdentity();
        EXPECT_EQ(fru1->getPN().value(), "SVCDOCS");
        EXPECT_EQ(fru1->failingComponentType(), src::FRUIdentity::symbolicFRU);
        EXPECT_FALSE(fru1->getMaintProc());
        EXPECT_FALSE(fru1->getSN());
        EXPECT_FALSE(fru1->getCCIN());

        auto& fru2 = callouts[1]->fruIdentity();
        EXPECT_EQ(fru2->getMaintProc().value(), "BMC0001");
        EXPECT_EQ(fru2->failingComponentType(),
                  src::FRUIdentity::maintenanceProc);
        EXPECT_FALSE(fru2->getPN());
        EXPECT_FALSE(fru2->getSN());
        EXPECT_FALSE(fru2->getCCIN());
    }

    {
        // Call out a trusted symbolic FRU with a location code, and
        // another one without.
        AdditionalData ad;
        NiceMock<MockDataInterface> dataIface;
        std::vector<std::string> names{"systemB"};

        EXPECT_CALL(dataIface, expandLocationCode).WillOnce(Return("P0-C8"));
        EXPECT_CALL(dataIface, getSystemNames).WillOnce(Return(names));

        SRC src{entry, ad, dataIface};

        auto& callouts = src.callouts()->callouts();
        EXPECT_EQ(callouts.size(), 2);

        EXPECT_EQ(callouts[0]->locationCode(), "P0-C8");
        EXPECT_EQ(callouts[0]->priority(), 'H');

        EXPECT_EQ(callouts[1]->locationCodeSize(), 0);
        EXPECT_EQ(callouts[1]->priority(), 'M');

        auto& fru1 = callouts[0]->fruIdentity();
        EXPECT_EQ(fru1->getPN().value(), "SVCDOCS");
        EXPECT_EQ(fru1->failingComponentType(),
                  src::FRUIdentity::symbolicFRUTrustedLocCode);
        EXPECT_FALSE(fru1->getMaintProc());
        EXPECT_FALSE(fru1->getSN());
        EXPECT_FALSE(fru1->getCCIN());

        // It asked for a trusted symbolic FRU, but no location code
        // was provided so it is switched back to a normal one
        auto& fru2 = callouts[1]->fruIdentity();
        EXPECT_EQ(fru2->getPN().value(), "SVCDOCS");
        EXPECT_EQ(fru2->failingComponentType(), src::FRUIdentity::symbolicFRU);
        EXPECT_FALSE(fru2->getMaintProc());
        EXPECT_FALSE(fru2->getSN());
        EXPECT_FALSE(fru2->getCCIN());
    }

    {
        // Two hardware callouts
        AdditionalData ad;
        NiceMock<MockDataInterface> dataIface;
        std::vector<std::string> names{"systemC"};

        EXPECT_CALL(dataIface, getSystemNames).WillOnce(Return(names));

        EXPECT_CALL(dataIface, expandLocationCode("P0-C8", 0))
            .WillOnce(Return("UXXX-P0-C8"));

        EXPECT_CALL(dataIface, expandLocationCode("P0-C9", 0))
            .WillOnce(Return("UXXX-P0-C9"));

        EXPECT_CALL(dataIface, getInventoryFromLocCode("P0-C8", 0, false))
            .WillOnce(Return(std::vector<std::string>{
                "/xyz/openbmc_project/inventory/chassis/motherboard/cpu0"}));

        EXPECT_CALL(dataIface, getInventoryFromLocCode("P0-C9", 0, false))
            .WillOnce(Return(std::vector<std::string>{
                "/xyz/openbmc_project/inventory/chassis/motherboard/cpu1"}));

        EXPECT_CALL(
            dataIface,
            getHWCalloutFields(
                "/xyz/openbmc_project/inventory/chassis/motherboard/cpu0", _, _,
                _))
            .Times(1)
            .WillOnce(
                DoAll(SetArgReferee<1>("1234567"), SetArgReferee<2>("CCCC"),
                      SetArgReferee<3>("123456789ABC")));

        EXPECT_CALL(
            dataIface,
            getHWCalloutFields(
                "/xyz/openbmc_project/inventory/chassis/motherboard/cpu1", _, _,
                _))
            .Times(1)
            .WillOnce(
                DoAll(SetArgReferee<1>("2345678"), SetArgReferee<2>("DDDD"),
                      SetArgReferee<3>("23456789ABCD")));

        SRC src{entry, ad, dataIface};

        auto& callouts = src.callouts()->callouts();
        EXPECT_EQ(callouts.size(), 2);

        EXPECT_EQ(callouts[0]->locationCode(), "UXXX-P0-C8");
        EXPECT_EQ(callouts[0]->priority(), 'H');

        auto& fru1 = callouts[0]->fruIdentity();
        EXPECT_EQ(fru1->getPN().value(), "1234567");
        EXPECT_EQ(fru1->getCCIN().value(), "CCCC");
        EXPECT_EQ(fru1->getSN().value(), "123456789ABC");

        EXPECT_EQ(callouts[1]->locationCode(), "UXXX-P0-C9");
        EXPECT_EQ(callouts[1]->priority(), 'M');

        auto& fru2 = callouts[1]->fruIdentity();
        EXPECT_EQ(fru2->getPN().value(), "2345678");
        EXPECT_EQ(fru2->getCCIN().value(), "DDDD");
        EXPECT_EQ(fru2->getSN().value(), "23456789ABCD");
    }
}

// Test that a symbolic FRU with a trusted location code callout
// from the registry can get its location from the
// CALLOUT_INVENTORY_PATH AdditionalData entry.
TEST_F(SRCTest, SymbolicFRUWithInvPathTest)
{
    message::Entry entry;
    entry.src.type = 0xBD;
    entry.src.reasonCode = 0xABCD;
    entry.subsystem = 0x42;

    entry.callouts = R"(
        [{
            "CalloutList":
            [
                {
                    "Priority": "high",
                    "SymbolicFRUTrusted": "service_docs",
                    "UseInventoryLocCode": true
                },
                {
                    "Priority": "medium",
                    "LocCode": "P0-C8",
                    "SymbolicFRUTrusted": "pwrsply"
                }
            ]
        }])"_json;

    {
        // The location code for the first symbolic FRU callout will
        // come from this inventory path since UseInventoryLocCode is set.
        // In this case there will be no normal FRU callout for the motherboard.
        std::map<std::string, std::string> adData{
            {"CALLOUT_INVENTORY_PATH", "motherboard"}};
        AdditionalData ad{adData};
        NiceMock<MockDataInterface> dataIface;
        std::vector<std::string> names{"systemA"};

        EXPECT_CALL(dataIface, getSystemNames).WillOnce(Return(names));

        EXPECT_CALL(dataIface, getLocationCode("motherboard"))
            .Times(1)
            .WillOnce(Return("Ufcs-P10"));

        EXPECT_CALL(dataIface, expandLocationCode("P0-C8", 0))
            .WillOnce(Return("Ufcs-P0-C8"));

        SRC src{entry, ad, dataIface};

        auto& callouts = src.callouts()->callouts();
        EXPECT_EQ(callouts.size(), 2);

        // The location code for the first symbolic FRU callout with a
        // trusted location code comes from the motherboard.
        EXPECT_EQ(callouts[0]->locationCode(), "Ufcs-P10");
        EXPECT_EQ(callouts[0]->priority(), 'H');
        auto& fru1 = callouts[0]->fruIdentity();
        EXPECT_EQ(fru1->getPN().value(), "SVCDOCS");
        EXPECT_EQ(fru1->failingComponentType(),
                  src::FRUIdentity::symbolicFRUTrustedLocCode);

        // The second trusted symbolic FRU callouts uses the location
        // code in the registry as usual.
        EXPECT_EQ(callouts[1]->locationCode(), "Ufcs-P0-C8");
        EXPECT_EQ(callouts[1]->priority(), 'M');
        auto& fru2 = callouts[1]->fruIdentity();
        EXPECT_EQ(fru2->getPN().value(), "PWRSPLY");
        EXPECT_EQ(fru2->failingComponentType(),
                  src::FRUIdentity::symbolicFRUTrustedLocCode);
    }

    {
        // This time say we want to use the location code from
        // the inventory, but don't pass it in and the callout should
        // end up a regular symbolic FRU
        entry.callouts = R"(
        [{
            "CalloutList":
            [
                {
                    "Priority": "high",
                    "SymbolicFRUTrusted": "service_docs",
                    "UseInventoryLocCode": true
                }
            ]
        }])"_json;

        AdditionalData ad;
        NiceMock<MockDataInterface> dataIface;
        std::vector<std::string> names{"systemA"};

        EXPECT_CALL(dataIface, getSystemNames).WillOnce(Return(names));

        SRC src{entry, ad, dataIface};

        auto& callouts = src.callouts()->callouts();
        EXPECT_EQ(callouts.size(), 1);

        EXPECT_EQ(callouts[0]->locationCode(), "");
        EXPECT_EQ(callouts[0]->priority(), 'H');
        auto& fru1 = callouts[0]->fruIdentity();
        EXPECT_EQ(fru1->getPN().value(), "SVCDOCS");
        EXPECT_EQ(fru1->failingComponentType(), src::FRUIdentity::symbolicFRU);
    }
}

TEST_F(SRCTest, RegistryCalloutCantGetLocTest)
{
    message::Entry entry;
    entry.src.type = 0xBD;
    entry.src.reasonCode = 0xABCD;
    entry.src.deconfigFlag = true;
    entry.src.checkstopFlag = true;
    entry.subsystem = 0x42;

    entry.callouts = R"(
        [{
            "CalloutList":
            [
                {
                    "Priority": "high",
                    "LocCode": "P0-C8"
                },
                {
                    "Priority": "medium",
                    "LocCode": "P0-C9"
                }
            ]
        }])"_json;

    {
        // The calls to expand the location codes will fail, but it should
        // still create the callouts with the unexpanded values and no HW
        // fields.
        AdditionalData ad;
        NiceMock<MockDataInterface> dataIface;
        std::vector<std::string> names{"systemC"};

        EXPECT_CALL(dataIface, getSystemNames).WillOnce(Return(names));

        EXPECT_CALL(dataIface, expandLocationCode("P0-C8", 0))
            .WillRepeatedly(Throw(std::runtime_error("Fail")));

        EXPECT_CALL(dataIface, expandLocationCode("P0-C9", 0))
            .WillRepeatedly(Throw(std::runtime_error("Fail")));

        EXPECT_CALL(dataIface, getInventoryFromLocCode(_, _, _)).Times(0);

        EXPECT_CALL(dataIface, getHWCalloutFields(_, _, _, _)).Times(0);

        SRC src{entry, ad, dataIface};

        auto& callouts = src.callouts()->callouts();
        ASSERT_EQ(callouts.size(), 2);

        // Only unexpanded location codes
        EXPECT_EQ(callouts[0]->locationCode(), "P0-C8");
        EXPECT_EQ(callouts[0]->priority(), 'H');

        auto& fru1 = callouts[0]->fruIdentity();
        EXPECT_EQ(fru1->getPN().value(), "");
        EXPECT_EQ(fru1->getCCIN().value(), "");
        EXPECT_EQ(fru1->getSN().value(), "");

        EXPECT_EQ(callouts[1]->locationCode(), "P0-C9");
        EXPECT_EQ(callouts[1]->priority(), 'M');

        auto& fru2 = callouts[1]->fruIdentity();
        EXPECT_EQ(fru2->getPN().value(), "");
        EXPECT_EQ(fru2->getCCIN().value(), "");
        EXPECT_EQ(fru2->getSN().value(), "");
    }
}

TEST_F(SRCTest, TrustedSymbolicFRUCantGetLocTest)
{
    message::Entry entry;
    entry.src.type = 0xBD;
    entry.src.reasonCode = 0xABCD;
    entry.subsystem = 0x42;

    entry.callouts = R"(
        [{
            "CalloutList":
            [
                {
                    "Priority": "medium",
                    "LocCode": "P0-C8",
                    "SymbolicFRUTrusted": "pwrsply"
                }
            ]
        }])"_json;

    std::map<std::string, std::string> adData;
    AdditionalData ad{adData};
    NiceMock<MockDataInterface> dataIface;
    std::vector<std::string> names{"systemA"};

    EXPECT_CALL(dataIface, getSystemNames).WillOnce(Return(names));

    // The call to expand the location code will fail, but it should
    // still create the callout with the unexpanded value and the
    // symbolic FRU can't be trusted.
    EXPECT_CALL(dataIface, expandLocationCode("P0-C8", 0))
        .WillRepeatedly(Throw(std::runtime_error("Fail")));

    SRC src{entry, ad, dataIface};

    auto& callouts = src.callouts()->callouts();
    ASSERT_EQ(callouts.size(), 1);

    EXPECT_EQ(callouts[0]->locationCode(), "P0-C8");
    EXPECT_EQ(callouts[0]->priority(), 'M');
    auto& fru = callouts[0]->fruIdentity();
    EXPECT_EQ(fru->getPN().value(), "PWRSPLY");
    EXPECT_EQ(fru->failingComponentType(), src::FRUIdentity::symbolicFRU);
}

// Test looking up device path fails in the callout jSON.
TEST_F(SRCTest, DevicePathCalloutTest)
{
    message::Entry entry;
    entry.src.type = 0xBD;
    entry.src.reasonCode = 0xABCD;
    entry.subsystem = 0x42;

    const auto calloutJSON = R"(
    {
        "I2C":
        {
            "14":
            {
                "114":
                {
                    "Callouts":[
                    {
                        "Name": "/chassis/motherboard/cpu0",
                        "LocationCode": "P1-C40",
                        "Priority": "H"
                    },
                    {
                        "Name": "/chassis/motherboard",
                        "LocationCode": "P1",
                        "Priority": "M"
                    },
                    {
                        "Name": "/chassis/motherboard/bmc",
                        "LocationCode": "P1-C15",
                        "Priority": "L"
                    }
                    ],
                    "Dest": "proc 0 target"
                }
            }
        }
    })";

    auto dataPath = getPELReadOnlyDataPath();
    std::ofstream file{dataPath / "systemA_dev_callouts.json"};
    file << calloutJSON;
    file.close();

    NiceMock<MockDataInterface> dataIface;
    std::vector<std::string> names{"systemA"};

    EXPECT_CALL(dataIface, getSystemNames)
        .Times(5)
        .WillRepeatedly(Return(names));

    EXPECT_CALL(dataIface, getInventoryFromLocCode("P1-C40", 0, false))
        .Times(3)
        .WillRepeatedly(Return(std::vector<std::string>{
            "/xyz/openbmc_project/inventory/chassis/motherboard/cpu0"}));

    EXPECT_CALL(dataIface, getInventoryFromLocCode("P1", 0, false))
        .Times(3)
        .WillRepeatedly(Return(std::vector<std::string>{
            "/xyz/openbmc_project/inventory/chassis/motherboard"}));

    EXPECT_CALL(dataIface, getInventoryFromLocCode("P1-C15", 0, false))
        .Times(3)
        .WillRepeatedly(Return(std::vector<std::string>{
            "/xyz/openbmc_project/inventory/chassis/motherboard/bmc"}));

    EXPECT_CALL(dataIface, expandLocationCode("P1-C40", 0))
        .Times(3)
        .WillRepeatedly(Return("Ufcs-P1-C40"));

    EXPECT_CALL(dataIface, expandLocationCode("P1", 0))
        .Times(3)
        .WillRepeatedly(Return("Ufcs-P1"));

    EXPECT_CALL(dataIface, expandLocationCode("P1-C15", 0))
        .Times(3)
        .WillRepeatedly(Return("Ufcs-P1-C15"));

    EXPECT_CALL(dataIface,
                getHWCalloutFields(
                    "/xyz/openbmc_project/inventory/chassis/motherboard/cpu0",
                    _, _, _))
        .Times(3)
        .WillRepeatedly(
            DoAll(SetArgReferee<1>("1234567"), SetArgReferee<2>("CCCC"),
                  SetArgReferee<3>("123456789ABC")));
    EXPECT_CALL(
        dataIface,
        getHWCalloutFields("/xyz/openbmc_project/inventory/chassis/motherboard",
                           _, _, _))
        .Times(3)
        .WillRepeatedly(
            DoAll(SetArgReferee<1>("7654321"), SetArgReferee<2>("MMMM"),
                  SetArgReferee<3>("CBA987654321")));
    EXPECT_CALL(dataIface,
                getHWCalloutFields(
                    "/xyz/openbmc_project/inventory/chassis/motherboard/bmc", _,
                    _, _))
        .Times(3)
        .WillRepeatedly(
            DoAll(SetArgReferee<1>("7123456"), SetArgReferee<2>("BBBB"),
                  SetArgReferee<3>("C123456789AB")));

    // Call this below with different AdditionalData values that
    // result in the same callouts.
    auto checkCallouts = [&entry, &dataIface](const auto& items) {
        AdditionalData ad{items};
        SRC src{entry, ad, dataIface};

        ASSERT_TRUE(src.callouts());
        auto& callouts = src.callouts()->callouts();

        ASSERT_EQ(callouts.size(), 3);

        {
            EXPECT_EQ(callouts[0]->priority(), 'H');
            EXPECT_EQ(callouts[0]->locationCode(), "Ufcs-P1-C40");

            auto& fru = callouts[0]->fruIdentity();
            EXPECT_EQ(fru->getPN().value(), "1234567");
            EXPECT_EQ(fru->getCCIN().value(), "CCCC");
            EXPECT_EQ(fru->getSN().value(), "123456789ABC");
        }
        {
            EXPECT_EQ(callouts[1]->priority(), 'M');
            EXPECT_EQ(callouts[1]->locationCode(), "Ufcs-P1");

            auto& fru = callouts[1]->fruIdentity();
            EXPECT_EQ(fru->getPN().value(), "7654321");
            EXPECT_EQ(fru->getCCIN().value(), "MMMM");
            EXPECT_EQ(fru->getSN().value(), "CBA987654321");
        }
        {
            EXPECT_EQ(callouts[2]->priority(), 'L');
            EXPECT_EQ(callouts[2]->locationCode(), "Ufcs-P1-C15");

            auto& fru = callouts[2]->fruIdentity();
            EXPECT_EQ(fru->getPN().value(), "7123456");
            EXPECT_EQ(fru->getCCIN().value(), "BBBB");
            EXPECT_EQ(fru->getSN().value(), "C123456789AB");
        }
    };

    {
        // Callouts based on the device path
        std::map<std::string, std::string> items{
            {"CALLOUT_ERRNO", "5"},
            {"CALLOUT_DEVICE_PATH",
             "/sys/devices/platform/ahb/ahb:apb/ahb:apb:bus@1e78a000/1e78a340.i2c-bus/i2c-14/14-0072"}};

        checkCallouts(items);
    }

    {
        // Callouts based on the I2C bus and address
        std::map<std::string, std::string> items{{"CALLOUT_ERRNO", "5"},
                                                 {"CALLOUT_IIC_BUS", "14"},
                                                 {"CALLOUT_IIC_ADDR", "0x72"}};
        checkCallouts(items);
    }

    {
        // Also based on I2C bus and address, but with bus = /dev/i2c-14
        std::map<std::string, std::string> items{{"CALLOUT_ERRNO", "5"},
                                                 {"CALLOUT_IIC_BUS", "14"},
                                                 {"CALLOUT_IIC_ADDR", "0x72"}};
        checkCallouts(items);
    }

    {
        // Callout not found
        std::map<std::string, std::string> items{
            {"CALLOUT_ERRNO", "5"},
            {"CALLOUT_DEVICE_PATH",
             "/sys/devices/platform/ahb/ahb:apb/ahb:apb:bus@1e78a000/1e78a340.i2c-bus/i2c-24/24-0012"}};

        AdditionalData ad{items};
        SRC src{entry, ad, dataIface};

        EXPECT_FALSE(src.callouts());
        ASSERT_EQ(src.getDebugData().size(), 1);
        EXPECT_EQ(src.getDebugData()[0],
                  "Problem looking up I2C callouts on 24 18: "
                  "[json.exception.out_of_range.403] key '24' not found");
    }

    {
        // Callout not found
        std::map<std::string, std::string> items{{"CALLOUT_ERRNO", "5"},
                                                 {"CALLOUT_IIC_BUS", "22"},
                                                 {"CALLOUT_IIC_ADDR", "0x99"}};
        AdditionalData ad{items};
        SRC src{entry, ad, dataIface};

        EXPECT_FALSE(src.callouts());
        ASSERT_EQ(src.getDebugData().size(), 1);
        EXPECT_EQ(src.getDebugData()[0],
                  "Problem looking up I2C callouts on 22 153: "
                  "[json.exception.out_of_range.403] key '22' not found");
    }
}

TEST_F(SRCTest, DevicePathCantGetLocTest)
{
    message::Entry entry;
    entry.src.type = 0xBD;
    entry.src.reasonCode = 0xABCD;
    entry.subsystem = 0x42;

    const auto calloutJSON = R"(
    {
        "I2C":
        {
            "14":
            {
                "114":
                {
                    "Callouts":[
                    {
                        "Name": "/chassis/motherboard/cpu0",
                        "LocationCode": "P1-C40",
                        "Priority": "H"
                    },
                    {
                        "Name": "/chassis/motherboard",
                        "LocationCode": "P1",
                        "Priority": "M"
                    }
                    ],
                    "Dest": "proc 0 target"
                }
            }
        }
    })";

    auto dataPath = getPELReadOnlyDataPath();
    std::ofstream file{dataPath / "systemA_dev_callouts.json"};
    file << calloutJSON;
    file.close();

    NiceMock<MockDataInterface> dataIface;
    std::vector<std::string> names{"systemA"};

    EXPECT_CALL(dataIface, getSystemNames).WillRepeatedly(Return(names));

    // The calls to expand the location codes will fail, so still create
    // the callouts with the unexpanded values and no HW fields

    EXPECT_CALL(dataIface, expandLocationCode("P1-C40", 0))
        .WillRepeatedly(Throw(std::runtime_error("Fail")));

    EXPECT_CALL(dataIface, expandLocationCode("P1", 0))
        .WillRepeatedly(Throw(std::runtime_error("Fail")));

    EXPECT_CALL(dataIface, getInventoryFromLocCode("P1-C40", 0, false))
        .Times(0);
    EXPECT_CALL(dataIface, getInventoryFromLocCode("P1", 0, false)).Times(0);

    std::map<std::string, std::string> items{
        {"CALLOUT_ERRNO", "5"},
        {"CALLOUT_DEVICE_PATH",
         "/sys/devices/platform/ahb/ahb:apb/ahb:apb:bus@1e78a000/1e78a340.i2c-bus/i2c-14/14-0072"}};

    AdditionalData ad{items};
    SRC src{entry, ad, dataIface};

    ASSERT_TRUE(src.callouts());
    auto& callouts = src.callouts()->callouts();

    ASSERT_EQ(callouts.size(), 2);

    // Should just contain the unexpanded location codes
    {
        EXPECT_EQ(callouts[0]->priority(), 'H');
        EXPECT_EQ(callouts[0]->locationCode(), "P1-C40");

        auto& fru = callouts[0]->fruIdentity();
        EXPECT_EQ(fru->getPN().value(), "");
        EXPECT_EQ(fru->getCCIN().value(), "");
        EXPECT_EQ(fru->getSN().value(), "");
    }
    {
        EXPECT_EQ(callouts[1]->priority(), 'M');
        EXPECT_EQ(callouts[1]->locationCode(), "P1");

        auto& fru = callouts[1]->fruIdentity();
        EXPECT_EQ(fru->getPN().value(), "");
        EXPECT_EQ(fru->getCCIN().value(), "");
        EXPECT_EQ(fru->getSN().value(), "");
    }

    fs::remove_all(dataPath);
}

// Test when callouts are passed in via JSON
TEST_F(SRCTest, JsonCalloutsTest)
{
    const auto jsonCallouts = R"(
        [
            {
                "LocationCode": "P0-C1",
                "Priority": "H",
                "MRUs": [
                    {
                        "ID": 42,
                        "Priority": "H"
                    },
                    {
                        "ID": 43,
                        "Priority": "M"
                    }
                ]
            },
            {
                "InventoryPath": "/inv/system/chassis/motherboard/cpu0",
                "Priority": "M",
                "Guarded": true,
                "Deconfigured": true
            },
            {
                "Procedure": "PROCEDU",
                "Priority": "A"
            },
            {
                "SymbolicFRU": "TRUSTED",
                "Priority": "B",
                "TrustedLocationCode": true,
                "LocationCode": "P1-C23"
            },
            {
                "SymbolicFRU": "FRUTST1",
                "Priority": "C",
                "LocationCode": "P1-C24"
            },
            {
                "SymbolicFRU": "FRUTST2LONG",
                "Priority": "L"
            },
            {
                "Procedure": "fsi_path",
                "Priority": "L"
            },
            {
                "SymbolicFRU": "ambient_temp",
                "Priority": "L"
            }
        ]
    )"_json;

    message::Entry entry;
    entry.src.type = 0xBD;
    entry.src.reasonCode = 0xABCD;
    entry.subsystem = 0x42;

    AdditionalData ad;
    NiceMock<MockDataInterface> dataIface;

    // Callout 0 mock calls
    {
        EXPECT_CALL(dataIface, expandLocationCode("P0-C1", 0))
            .Times(1)
            .WillOnce(Return("UXXX-P0-C1"));
        EXPECT_CALL(dataIface, getInventoryFromLocCode("P0-C1", 0, false))
            .Times(1)
            .WillOnce(Return(std::vector<std::string>{
                "/inv/system/chassis/motherboard/bmc"}));
        EXPECT_CALL(
            dataIface,
            getHWCalloutFields("/inv/system/chassis/motherboard/bmc", _, _, _))
            .Times(1)
            .WillOnce(
                DoAll(SetArgReferee<1>("1234567"), SetArgReferee<2>("CCCC"),
                      SetArgReferee<3>("123456789ABC")));
    }
    // Callout 1 mock calls
    {
        EXPECT_CALL(dataIface,
                    getLocationCode("/inv/system/chassis/motherboard/cpu0"))
            .WillOnce(Return("UYYY-P5"));
        EXPECT_CALL(
            dataIface,
            getHWCalloutFields("/inv/system/chassis/motherboard/cpu0", _, _, _))
            .Times(1)
            .WillOnce(
                DoAll(SetArgReferee<1>("2345678"), SetArgReferee<2>("DDDD"),
                      SetArgReferee<3>("23456789ABCD")));
    }
    // Callout 3 mock calls
    {
        EXPECT_CALL(dataIface, expandLocationCode("P1-C23", 0))
            .Times(1)
            .WillOnce(Return("UXXX-P1-C23"));
    }
    // Callout 4 mock calls
    {
        EXPECT_CALL(dataIface, expandLocationCode("P1-C24", 0))
            .Times(1)
            .WillOnce(Return("UXXX-P1-C24"));
    }

    SRC src{entry, ad, jsonCallouts, dataIface};
    ASSERT_TRUE(src.callouts());

    // Check the guarded and deconfigured flags
    EXPECT_TRUE(src.hexwordData()[3] & 0x03000000);

    const auto& callouts = src.callouts()->callouts();
    ASSERT_EQ(callouts.size(), 8);

    // Check callout 0
    {
        EXPECT_EQ(callouts[0]->priority(), 'H');
        EXPECT_EQ(callouts[0]->locationCode(), "UXXX-P0-C1");

        auto& fru = callouts[0]->fruIdentity();
        EXPECT_EQ(fru->getPN().value(), "1234567");
        EXPECT_EQ(fru->getCCIN().value(), "CCCC");
        EXPECT_EQ(fru->getSN().value(), "123456789ABC");
        EXPECT_EQ(fru->failingComponentType(), src::FRUIdentity::hardwareFRU);

        auto& mruCallouts = callouts[0]->mru();
        ASSERT_TRUE(mruCallouts);
        auto& mrus = mruCallouts->mrus();
        ASSERT_EQ(mrus.size(), 2);
        EXPECT_EQ(mrus[0].id, 42);
        EXPECT_EQ(mrus[0].priority, 'H');
        EXPECT_EQ(mrus[1].id, 43);
        EXPECT_EQ(mrus[1].priority, 'M');
    }

    // Check callout 1
    {
        EXPECT_EQ(callouts[1]->priority(), 'M');
        EXPECT_EQ(callouts[1]->locationCode(), "UYYY-P5");

        auto& fru = callouts[1]->fruIdentity();
        EXPECT_EQ(fru->getPN().value(), "2345678");
        EXPECT_EQ(fru->getCCIN().value(), "DDDD");
        EXPECT_EQ(fru->getSN().value(), "23456789ABCD");
        EXPECT_EQ(fru->failingComponentType(), src::FRUIdentity::hardwareFRU);
    }

    // Check callout 2
    {
        EXPECT_EQ(callouts[2]->priority(), 'A');
        EXPECT_EQ(callouts[2]->locationCode(), "");

        auto& fru = callouts[2]->fruIdentity();
        EXPECT_EQ(fru->getMaintProc().value(), "PROCEDU");
        EXPECT_EQ(fru->failingComponentType(),
                  src::FRUIdentity::maintenanceProc);
    }

    // Check callout 3
    {
        EXPECT_EQ(callouts[3]->priority(), 'B');
        EXPECT_EQ(callouts[3]->locationCode(), "UXXX-P1-C23");

        auto& fru = callouts[3]->fruIdentity();
        EXPECT_EQ(fru->getPN().value(), "TRUSTED");
        EXPECT_EQ(fru->failingComponentType(),
                  src::FRUIdentity::symbolicFRUTrustedLocCode);
    }

    // Check callout 4
    {
        EXPECT_EQ(callouts[4]->priority(), 'C');
        EXPECT_EQ(callouts[4]->locationCode(), "UXXX-P1-C24");

        auto& fru = callouts[4]->fruIdentity();
        EXPECT_EQ(fru->getPN().value(), "FRUTST1");
        EXPECT_EQ(fru->failingComponentType(), src::FRUIdentity::symbolicFRU);
    }

    // Check callout 5
    {
        EXPECT_EQ(callouts[5]->priority(), 'L');
        EXPECT_EQ(callouts[5]->locationCode(), "");

        auto& fru = callouts[5]->fruIdentity();
        EXPECT_EQ(fru->getPN().value(), "FRUTST2");
        EXPECT_EQ(fru->failingComponentType(), src::FRUIdentity::symbolicFRU);
    }

    // Check callout 6
    {
        EXPECT_EQ(callouts[6]->priority(), 'L');
        EXPECT_EQ(callouts[6]->locationCode(), "");

        auto& fru = callouts[6]->fruIdentity();
        EXPECT_EQ(fru->getMaintProc().value(), "BMC0004");
        EXPECT_EQ(fru->failingComponentType(),
                  src::FRUIdentity::maintenanceProc);
    }

    // Check callout 7
    {
        EXPECT_EQ(callouts[7]->priority(), 'L');
        EXPECT_EQ(callouts[7]->locationCode(), "");

        auto& fru = callouts[7]->fruIdentity();
        EXPECT_EQ(fru->getPN().value(), "AMBTEMP");
        EXPECT_EQ(fru->failingComponentType(), src::FRUIdentity::symbolicFRU);
    }

    // Check that it didn't find any errors
    const auto& data = src.getDebugData();
    EXPECT_TRUE(data.empty());
}

TEST_F(SRCTest, JsonBadCalloutsTest)
{
    // The first call will have a Throw in a mock call.
    // The second will have a different Throw in a mock call.
    // The others have issues with the Priority field.
    const auto jsonCallouts = R"(
        [
            {
                "LocationCode": "P0-C1",
                "Priority": "H"
            },
            {
                "LocationCode": "P0-C2",
                "Priority": "H"
            },
            {
                "LocationCode": "P0-C3"
            },
            {
                "LocationCode": "P0-C4",
                "Priority": "X"
            }
        ]
    )"_json;

    message::Entry entry;
    entry.src.type = 0xBD;
    entry.src.reasonCode = 0xABCD;
    entry.subsystem = 0x42;

    AdditionalData ad;
    NiceMock<MockDataInterface> dataIface;

    // Callout 0 mock calls
    // Expand location code will fail, so the unexpanded location
    // code should show up in the callout instead.
    {
        EXPECT_CALL(dataIface, expandLocationCode("P0-C1", 0))
            .WillOnce(Throw(std::runtime_error("Fail")));

        EXPECT_CALL(dataIface, getInventoryFromLocCode("P0-C1", 0, false))
            .Times(1)
            .WillOnce(Return(std::vector<std::string>{
                "/inv/system/chassis/motherboard/bmc"}));
        EXPECT_CALL(
            dataIface,
            getHWCalloutFields("/inv/system/chassis/motherboard/bmc", _, _, _))
            .Times(1)
            .WillOnce(
                DoAll(SetArgReferee<1>("1234567"), SetArgReferee<2>("CCCC"),
                      SetArgReferee<3>("123456789ABC")));
    }

    // Callout 1 mock calls
    // getInventoryFromLocCode will fail, so a callout with just the
    // location code will be created.
    {
        EXPECT_CALL(dataIface, expandLocationCode("P0-C2", 0))
            .Times(1)
            .WillOnce(Return("UXXX-P0-C2"));

        EXPECT_CALL(dataIface, getInventoryFromLocCode("P0-C2", 0, false))
            .Times(1)
            .WillOnce(Throw(std::runtime_error("Fail")));
    }

    SRC src{entry, ad, jsonCallouts, dataIface};

    ASSERT_TRUE(src.callouts());

    const auto& callouts = src.callouts()->callouts();

    // The first callout will have the unexpanded location code.
    ASSERT_EQ(callouts.size(), 2);

    EXPECT_EQ(callouts[0]->priority(), 'H');
    EXPECT_EQ(callouts[0]->locationCode(), "P0-C1");

    auto& fru0 = callouts[0]->fruIdentity();
    EXPECT_EQ(fru0->getPN().value(), "1234567");
    EXPECT_EQ(fru0->getCCIN().value(), "CCCC");
    EXPECT_EQ(fru0->getSN().value(), "123456789ABC");
    EXPECT_EQ(fru0->failingComponentType(), src::FRUIdentity::hardwareFRU);

    // The second callout will have empty HW details.
    EXPECT_EQ(callouts[1]->priority(), 'H');
    EXPECT_EQ(callouts[1]->locationCode(), "UXXX-P0-C2");

    auto& fru1 = callouts[1]->fruIdentity();
    EXPECT_EQ(fru1->getPN().value(), "");
    EXPECT_EQ(fru1->getCCIN().value(), "");
    EXPECT_EQ(fru1->getSN().value(), "");
    EXPECT_EQ(fru1->failingComponentType(), src::FRUIdentity::hardwareFRU);

    const auto& data = src.getDebugData();
    ASSERT_EQ(data.size(), 4);
    EXPECT_STREQ(data[0].c_str(), "Unable to expand location code P0-C1: Fail");
    EXPECT_STREQ(
        data[1].c_str(),
        "Unable to get inventory path from location code: P0-C2: Fail");
    EXPECT_STREQ(data[2].c_str(),
                 "Failed extracting callout data from JSON: "
                 "[json.exception.out_of_range.403] key 'Priority' not found");
    EXPECT_STREQ(data[3].c_str(),
                 "Failed extracting callout data from JSON: Invalid "
                 "priority 'X' found in JSON callout");
}

// Test that an inventory path callout can have
// a different priority than H.
TEST_F(SRCTest, InventoryCalloutTestPriority)
{
    message::Entry entry;
    entry.src.type = 0xBD;
    entry.src.reasonCode = 0xABCD;
    entry.subsystem = 0x42;

    std::map<std::string, std::string> adData{
        {"CALLOUT_INVENTORY_PATH", "motherboard"}, {"CALLOUT_PRIORITY", "M"}};
    AdditionalData ad{adData};
    NiceMock<MockDataInterface> dataIface;

    EXPECT_CALL(dataIface, getLocationCode("motherboard"))
        .WillOnce(Return("UTMS-P1"));

    EXPECT_CALL(dataIface, getHWCalloutFields("motherboard", _, _, _))
        .Times(1)
        .WillOnce(DoAll(SetArgReferee<1>("1234567"), SetArgReferee<2>("CCCC"),
                        SetArgReferee<3>("123456789ABC")));

    SRC src{entry, ad, dataIface};
    EXPECT_TRUE(src.valid());

    ASSERT_TRUE(src.callouts());

    EXPECT_EQ(src.callouts()->callouts().size(), 1);

    auto& callout = src.callouts()->callouts().front();

    EXPECT_EQ(callout->locationCode(), "UTMS-P1");
    EXPECT_EQ(callout->priority(), 'M');
}

// Test SRC with additional data - PEL_SUBSYSTEM
TEST_F(SRCTest, TestPELSubsystem)
{
    message::Entry entry;
    entry.src.type = 0xBD;
    entry.src.reasonCode = 0xABCD;
    entry.subsystem = 0x42;

    // Values for the SRC words pointed to above
    std::map<std::string, std::string> adData{{"PEL_SUBSYSTEM", "0x20"}};
    AdditionalData ad{adData};
    NiceMock<MockDataInterface> dataIface;

    EXPECT_CALL(dataIface, getMotherboardCCIN).WillOnce(Return("ABCD"));

    SRC src{entry, ad, dataIface};

    EXPECT_TRUE(src.valid());

    EXPECT_EQ(src.asciiString(), "BD20ABCD                        ");
}

void setAsciiString(std::vector<uint8_t>& src, const std::string& value)
{
    assert(40 + value.size() <= src.size());

    for (size_t i = 0; i < value.size(); i++)
    {
        src[40 + i] = value[i];
    }
}

TEST_F(SRCTest, TestGetProgressCode)
{
    {
        // A real SRC with CC009184
        std::vector<uint8_t> src{
            2,  8,   0,  9,   0,   0,  0,  72, 0,  0,  0,  224, 0,  0,  0,
            0,  204, 0,  145, 132, 0,  0,  0,  0,  0,  0,  0,   0,  0,  0,
            0,  0,   0,  0,   0,   0,  0,  0,  0,  0,  67, 67,  48, 48, 57,
            49, 56,  52, 32,  32,  32, 32, 32, 32, 32, 32, 32,  32, 32, 32,
            32, 32,  32, 32,  32,  32, 32, 32, 32, 32, 32, 32};

        EXPECT_EQ(SRC::getProgressCode(src), 0xCC009184);
    }

    {
        // A real SRC with STANDBY
        std::vector<uint8_t> src{
            2,  0,  0,  1,  0,  0,  0,  72, 0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  83, 84, 65, 78, 68,
            66, 89, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
            32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32};

        EXPECT_EQ(SRC::getProgressCode(src), 0);
    }

    {
        // A real SRC with CC009184, but 1 byte too short
        std::vector<uint8_t> src{
            2,  8,   0,  9,   0,   0,  0,  72, 0,  0,  0,  224, 0,  0,  0,
            0,  204, 0,  145, 132, 0,  0,  0,  0,  0,  0,  0,   0,  0,  0,
            0,  0,   0,  0,   0,   0,  0,  0,  0,  0,  67, 67,  48, 48, 57,
            49, 56,  52, 32,  32,  32, 32, 32, 32, 32, 32, 32,  32, 32, 32,
            32, 32,  32, 32,  32,  32, 32, 32, 32, 32, 32, 32};
        src.resize(71);
        EXPECT_EQ(SRC::getProgressCode(src), 0);
    }

    {
        // A few different ones
        const std::map<std::string, uint32_t> progressCodes{
            {"12345678", 0x12345678}, {"ABCDEF00", 0xABCDEF00},
            {"abcdef00", 0xABCDEF00}, {"X1234567", 0},
            {"1234567X", 0},          {"1       ", 0}};

        std::vector<uint8_t> src(72, 0x0);

        for (const auto& [code, expected] : progressCodes)
        {
            setAsciiString(src, code);
            EXPECT_EQ(SRC::getProgressCode(src), expected);
        }

        // empty
        src.clear();
        EXPECT_EQ(SRC::getProgressCode(src), 0);
    }
}

// Test progress is in right SRC hex data field
TEST_F(SRCTest, TestProgressCodeField)
{
    message::Entry entry;
    entry.src.type = 0xBD;
    entry.src.reasonCode = 0xABCD;
    entry.subsystem = 0x42;

    AdditionalData ad;
    NiceMock<MockDataInterface> dataIface;
    EXPECT_CALL(dataIface, getRawProgressSRC())
        .WillOnce(Return(std::vector<uint8_t>{
            2,  8,   0,  9,   0,   0,  0,  72, 0,  0,  0,  224, 0,  0,  0,
            0,  204, 0,  145, 132, 0,  0,  0,  0,  0,  0,  0,   0,  0,  0,
            0,  0,   0,  0,   0,   0,  0,  0,  0,  0,  67, 67,  48, 48, 57,
            49, 56,  52, 32,  32,  32, 32, 32, 32, 32, 32, 32,  32, 32, 32,
            32, 32,  32, 32,  32,  32, 32, 32, 32, 32, 32, 32}));

    SRC src{entry, ad, dataIface};
    EXPECT_TRUE(src.valid());

    // Verify that the hex vlue is set at the right hexword
    EXPECT_EQ(src.hexwordData()[2], 0xCC009184);
}
