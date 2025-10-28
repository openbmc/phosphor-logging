// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2020 IBM Corporation

#include "extensions/openpower-pels/service_indicators.hpp"
#include "mocks.hpp"
#include "pel_utils.hpp"

#include <gtest/gtest.h>

using namespace openpower::pels;
using CalloutVector = std::vector<std::unique_ptr<src::Callout>>;
using ::testing::_;
using ::testing::Return;
using ::testing::Throw;

// Test the ignore() function works
TEST(ServiceIndicatorsTest, IgnoreTest)
{
    MockDataInterface dataIface;
    service_indicators::LightPath lightPath{dataIface};

    // PEL must have serviceable action flag set and be created
    // by the BMC or Hostboot.
    std::vector<std::tuple<char, uint16_t, bool>> testParams{
        {'O', 0xA400, false}, // BMC serviceable, don't ignore
        {'B', 0xA400, false}, // Hostboot serviceable, don't ignore
        {'H', 0xA400, true},  // PHYP serviceable, ignore
        {'O', 0x2400, true},  // BMC not serviceable, ignore
        {'B', 0x2400, true},  // Hostboot not serviceable, ignore
        {'H', 0x2400, true},  // PHYP not serviceable, ignore
    };

    for (const auto& test : testParams)
    {
        auto data = pelFactory(1, std::get<char>(test), 0x20,
                               std::get<uint16_t>(test), 500);
        PEL pel{data};

        EXPECT_EQ(lightPath.ignore(pel), std::get<bool>(test));
    }
}

// Test that only high, medium, and medium group A hardware
// callouts have their location codes extracted.
TEST(ServiceIndicatorsTest, OneCalloutPriorityTest)
{
    MockDataInterface dataIface;
    service_indicators::LightPath lightPath{dataIface};

    // The priorities to test, with the expected getLocationCodes results.
    std::vector<std::tuple<CalloutPriority, std::vector<std::string>>>
        testCallouts{{CalloutPriority::high, {"U27-P1"}},
                     {CalloutPriority::medium, {"U27-P1"}},
                     {CalloutPriority::mediumGroupA, {"U27-P1"}},
                     {CalloutPriority::mediumGroupB, {}},
                     {CalloutPriority::mediumGroupC, {}},
                     {CalloutPriority::low, {}}};

    for (const auto& test : testCallouts)
    {
        auto callout = std::make_unique<src::Callout>(
            std::get<CalloutPriority>(test), "U27-P1", "1234567", "aaaa",
            "123456789ABC");

        CalloutVector callouts;
        callouts.push_back(std::move(callout));

        EXPECT_EQ(lightPath.getLocationCodes(callouts),
                  std::get<std::vector<std::string>>(test));
    }
}

// Test that only normal hardware callouts and symbolic FRU
// callouts with trusted location codes have their location
// codes extracted.
TEST(ServiceIndicatorsTest, OneCalloutTypeTest)
{
    MockDataInterface dataIface;
    service_indicators::LightPath lightPath{dataIface};

    // Regular hardware callout
    {
        CalloutVector callouts;
        callouts.push_back(
            std::make_unique<src::Callout>(CalloutPriority::high, "U27-P1",
                                           "1234567", "aaaa", "123456789ABC"));

        EXPECT_EQ(lightPath.getLocationCodes(callouts),
                  std::vector<std::string>{"U27-P1"});
    }

    // Symbolic FRU with trusted loc code callout
    {
        CalloutVector callouts;
        callouts.push_back(std::make_unique<src::Callout>(
            CalloutPriority::high, "service_docs", "U27-P1", true));

        EXPECT_EQ(lightPath.getLocationCodes(callouts),
                  std::vector<std::string>{"U27-P1"});
    }

    // Symbolic FRU without trusted loc code callout
    {
        CalloutVector callouts;
        callouts.push_back(std::make_unique<src::Callout>(
            CalloutPriority::high, "service_docs", "U27-P1", false));

        EXPECT_EQ(lightPath.getLocationCodes(callouts),
                  std::vector<std::string>{});
    }

    // Procedure callout
    {
        CalloutVector callouts;
        callouts.push_back(
            std::make_unique<src::Callout>(CalloutPriority::high, "bmc_code"));

        EXPECT_EQ(lightPath.getLocationCodes(callouts),
                  std::vector<std::string>{});
    }
}

// Test that only the callouts in the first group have their location
// codes extracted, where a group is one or more callouts listed
// together with priorities of high, medium, or medium group A
// (and medium is only ever contains 1 item).
TEST(ServiceIndicatorsTest, CalloutGroupingTest)
{
    MockDataInterface dataIface;
    service_indicators::LightPath lightPath{dataIface};

    // high/high/medium/high just grabs the first 2
    {
        CalloutVector callouts;
        callouts.push_back(
            std::make_unique<src::Callout>(CalloutPriority::high, "U27-P1",
                                           "1234567", "aaaa", "123456789ABC"));
        callouts.push_back(
            std::make_unique<src::Callout>(CalloutPriority::high, "U27-P2",
                                           "1234567", "aaaa", "123456789ABC"));
        callouts.push_back(
            std::make_unique<src::Callout>(CalloutPriority::medium, "U27-P3",
                                           "1234567", "aaaa", "123456789ABC"));
        // This high priority callout after a medium isn't actually valid, since
        // callouts are sorted, but test it anyway.
        callouts.push_back(
            std::make_unique<src::Callout>(CalloutPriority::high, "U27-P4",
                                           "1234567", "aaaa", "123456789ABC"));

        EXPECT_EQ(lightPath.getLocationCodes(callouts),
                  (std::vector<std::string>{"U27-P1", "U27-P2"}));
    }

    // medium/medium just grabs the first medium
    {
        CalloutVector callouts;
        callouts.push_back(
            std::make_unique<src::Callout>(CalloutPriority::medium, "U27-P1",
                                           "1234567", "aaaa", "123456789ABC"));
        callouts.push_back(
            std::make_unique<src::Callout>(CalloutPriority::medium, "U27-P2",
                                           "1234567", "aaaa", "123456789ABC"));

        EXPECT_EQ(lightPath.getLocationCodes(callouts),
                  std::vector<std::string>{"U27-P1"});
    }

    // mediumA/mediumA/medium just grabs the first 2
    {
        CalloutVector callouts;
        callouts.push_back(std::make_unique<src::Callout>(
            CalloutPriority::mediumGroupA, "U27-P1", "1234567", "aaaa",
            "123456789ABC"));

        callouts.push_back(std::make_unique<src::Callout>(
            CalloutPriority::mediumGroupA, "U27-P2", "1234567", "aaaa",
            "123456789ABC"));

        callouts.push_back(
            std::make_unique<src::Callout>(CalloutPriority::medium, "U27-P3",
                                           "1234567", "aaaa", "123456789ABC"));

        EXPECT_EQ(lightPath.getLocationCodes(callouts),
                  (std::vector<std::string>{"U27-P1", "U27-P2"}));
    }
}

// Test that if any callouts in group are not HW/trusted symbolic
// FRU callouts then no location codes will be extracted
TEST(ServiceIndicatorsTest, CalloutMixedTypesTest)
{
    MockDataInterface dataIface;
    service_indicators::LightPath lightPath{dataIface};

    // Mixing FRU with trusted symbolic FRU is OK
    {
        CalloutVector callouts;
        callouts.push_back(
            std::make_unique<src::Callout>(CalloutPriority::high, "U27-P1",
                                           "1234567", "aaaa", "123456789ABC"));
        callouts.push_back(std::make_unique<src::Callout>(
            CalloutPriority::high, "service_docs", "U27-P2", true));

        EXPECT_EQ(lightPath.getLocationCodes(callouts),
                  (std::vector<std::string>{"U27-P1", "U27-P2"}));
    }

    // Normal FRU callout with a non-trusted symbolic FRU callout not OK
    {
        CalloutVector callouts;
        callouts.push_back(
            std::make_unique<src::Callout>(CalloutPriority::high, "U27-P1",
                                           "1234567", "aaaa", "123456789ABC"));
        callouts.push_back(std::make_unique<src::Callout>(
            CalloutPriority::high, "service_docs", "U27-P2", false));

        EXPECT_EQ(lightPath.getLocationCodes(callouts),
                  (std::vector<std::string>{}));
    }

    // Normal FRU callout with a procedure callout not OK
    {
        CalloutVector callouts;
        callouts.push_back(
            std::make_unique<src::Callout>(CalloutPriority::high, "U27-P1",
                                           "1234567", "aaaa", "123456789ABC"));
        callouts.push_back(
            std::make_unique<src::Callout>(CalloutPriority::high, "bmc_code"));

        EXPECT_EQ(lightPath.getLocationCodes(callouts),
                  (std::vector<std::string>{}));
    }

    // Trusted symbolic FRU callout with a non-trusted symbolic
    // FRU callout not OK
    {
        CalloutVector callouts;
        callouts.push_back(std::make_unique<src::Callout>(
            CalloutPriority::high, "service_docs", "U27-P2", true));

        callouts.push_back(std::make_unique<src::Callout>(
            CalloutPriority::high, "service_docs", "U27-P2", false));

        EXPECT_EQ(lightPath.getLocationCodes(callouts),
                  (std::vector<std::string>{}));
    }
}

// Test the activate() function
TEST(ServiceIndicatorsTest, ActivateTest)
{
    // pelFactory() will create a PEL with 1 callout with location code
    // U42.  Test the LED for that gets activated.
    {
        MockDataInterface dataIface;
        service_indicators::LightPath lightPath{dataIface};

        EXPECT_CALL(dataIface, getInventoryFromLocCode("U42", 0, true))
            .WillOnce(
                Return(std::vector<std::string>{"/system/chassis/processor"}));

        EXPECT_CALL(dataIface,
                    setFunctional("/system/chassis/processor", false))
            .Times(1);

        EXPECT_CALL(dataIface,
                    setCriticalAssociation("/system/chassis/processor"))
            .Times(1);

        auto data = pelFactory(1, 'O', 0x20, 0xA400, 500);
        PEL pel{data};

        lightPath.activate(pel);
    }

    // With the same U42 callout, have it be associated with two
    // inventory paths
    {
        MockDataInterface dataIface;
        service_indicators::LightPath lightPath{dataIface};

        EXPECT_CALL(dataIface, getInventoryFromLocCode("U42", 0, true))
            .WillOnce(Return(std::vector<std::string>{"/system/chassis/cpu0",
                                                      "/system/chassis/cpu1"}));

        EXPECT_CALL(dataIface, setFunctional("/system/chassis/cpu0", false))
            .Times(1);
        EXPECT_CALL(dataIface, setFunctional("/system/chassis/cpu1", false))
            .Times(1);

        EXPECT_CALL(dataIface, setCriticalAssociation("/system/chassis/cpu0"))
            .Times(1);
        EXPECT_CALL(dataIface, setCriticalAssociation("/system/chassis/cpu1"))
            .Times(1);

        auto data = pelFactory(1, 'O', 0x20, 0xA400, 500);
        PEL pel{data};

        lightPath.activate(pel);
    }

    // A non-info BMC PEL with no callouts will set the platform SAI LED.
    {
        MockDataInterface dataIface;
        service_indicators::LightPath lightPath{dataIface};

        EXPECT_CALL(dataIface,
                    assertLEDGroup("/xyz/openbmc_project/led/groups/"
                                   "platform_system_attention_indicator",
                                   true))
            .Times(1);

        auto data = pelDataFactory(TestPELType::pelSimple);
        PEL pel{data};

        lightPath.activate(pel);
    }

    // Make getInventoryFromLocCode fail - will set the platform SAI LED
    {
        MockDataInterface dataIface;
        service_indicators::LightPath lightPath{dataIface};

        EXPECT_CALL(dataIface, getInventoryFromLocCode("U42", 0, true))
            .WillOnce(Throw(std::runtime_error("Fail")));

        EXPECT_CALL(dataIface, setFunctional).Times(0);

        EXPECT_CALL(dataIface,
                    assertLEDGroup("/xyz/openbmc_project/led/groups/"
                                   "platform_system_attention_indicator",
                                   true))
            .Times(1);

        auto data = pelFactory(1, 'O', 0x20, 0xA400, 500);
        PEL pel{data};

        lightPath.activate(pel);
    }

    // Make setFunctional fail
    {
        MockDataInterface dataIface;
        service_indicators::LightPath lightPath{dataIface};

        EXPECT_CALL(dataIface, getInventoryFromLocCode("U42", 0, true))
            .WillOnce(
                Return(std::vector<std::string>{"/system/chassis/processor"}));

        EXPECT_CALL(dataIface,
                    setFunctional("/system/chassis/processor", false))
            .WillOnce(Throw(std::runtime_error("Fail")));

        auto data = pelFactory(1, 'O', 0x20, 0xA400, 500);
        PEL pel{data};

        lightPath.activate(pel);
    }

    // Test setCriticalAssociation fail
    {
        MockDataInterface dataIface;
        service_indicators::LightPath lightPath{dataIface};

        EXPECT_CALL(dataIface, getInventoryFromLocCode("U42", 0, true))
            .WillOnce(
                Return(std::vector<std::string>{"/system/chassis/processor"}));

        EXPECT_CALL(dataIface,
                    setCriticalAssociation("/system/chassis/processor"))
            .WillOnce(Throw(std::runtime_error("Fail")));

        auto data = pelFactory(1, 'O', 0x20, 0xA400, 500);
        PEL pel{data};

        lightPath.activate(pel);
    }
}
