// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "extensions/openpower-pels/callouts.hpp"
#include "pel_utils.hpp"

#include <format>

#include <gtest/gtest.h>

using namespace openpower::pels;
using namespace openpower::pels::src;

TEST(CalloutsTest, UnflattenFlattenTest)
{
    std::vector<uint8_t> data{0xC0, 0x00, 0x00,
                              0x00}; // ID, flags, length in words

    // Add 2 callouts
    auto callout = srcDataFactory(TestSRCType::calloutStructureA);
    data.insert(data.end(), callout.begin(), callout.end());

    callout = srcDataFactory(TestSRCType::calloutStructureB);
    data.insert(data.end(), callout.begin(), callout.end());

    Stream stream{data};

    // Set the actual word length value at offset 2
    uint16_t wordLength = data.size() / 4;
    stream.offset(2);
    stream << wordLength;
    stream.offset(0);

    Callouts callouts{stream};

    EXPECT_EQ(callouts.flattenedSize(), data.size());
    EXPECT_EQ(callouts.callouts().size(), 2);

    // spot check that each callout has the right substructures
    EXPECT_TRUE(callouts.callouts().front()->fruIdentity());
    EXPECT_FALSE(callouts.callouts().front()->pceIdentity());
    EXPECT_FALSE(callouts.callouts().front()->mru());

    EXPECT_TRUE(callouts.callouts().back()->fruIdentity());
    EXPECT_TRUE(callouts.callouts().back()->pceIdentity());
    EXPECT_TRUE(callouts.callouts().back()->mru());

    // Flatten
    std::vector<uint8_t> newData;
    Stream newStream{newData};

    callouts.flatten(newStream);
    EXPECT_EQ(data, newData);
}

TEST(CalloutsTest, BadDataTest)
{
    // Start out with a valid 2 callout object, then truncate it.
    std::vector<uint8_t> data{0xC0, 0x00, 0x00,
                              0x00}; // ID, flags, length in words

    // Add 2 callouts
    auto callout = srcDataFactory(TestSRCType::calloutStructureA);
    data.insert(data.end(), callout.begin(), callout.end());

    callout = srcDataFactory(TestSRCType::calloutStructureB);
    data.insert(data.end(), callout.begin(), callout.end());

    Stream stream{data};

    // Set the actual word length value at offset 2
    uint16_t wordLength = data.size() / 4;
    stream.offset(2);
    stream << wordLength;
    stream.offset(0);

    // Shorten the data by an arbitrary amount so unflattening goes awry.
    data.resize(data.size() - 37);

    EXPECT_THROW(Callouts callouts{stream}, std::out_of_range);
}

TEST(CalloutsTest, TestAddCallouts)
{
    Callouts callouts;

    // Empty Callouts size
    size_t lastSize = 4;

    for (size_t i = 0; i < maxNumberOfCallouts; i++)
    {
        auto loc = std::format("U1-P{}", i);
        auto callout = std::make_unique<Callout>(
            CalloutPriority::high, loc, "1234567", "ABCD", "123456789ABC");
        auto calloutSize = callout->flattenedSize();

        callouts.addCallout(std::move(callout));

        EXPECT_EQ(callouts.flattenedSize(), lastSize + calloutSize);

        lastSize = callouts.flattenedSize();

        EXPECT_EQ(callouts.callouts().size(), i + 1);
    }

    // Try to add an 11th callout.  Shouldn't work

    auto callout = std::make_unique<Callout>(CalloutPriority::high, "U1-P1",
                                             "1234567", "ABCD", "123456789ABC");
    callouts.addCallout(std::move(callout));

    EXPECT_EQ(callouts.callouts().size(), maxNumberOfCallouts);
}

TEST(CalloutsTest, TestSortCallouts)
{
    Callouts callouts;

    // Add  callouts with different priorities to test sorting in descending
    // order

    auto c0 = std::make_unique<Callout>(CalloutPriority::high, "U1-P1",
                                        "1234567", "ABC", "123456789ABC");

    callouts.addCallout(std::move(c0));

    auto c1 = std::make_unique<Callout>(CalloutPriority::medium, "U1-P2",
                                        "1234567", "ABCD", "123456789ABC");

    callouts.addCallout(std::move(c1));

    auto c2 = std::make_unique<Callout>(CalloutPriority::low, "U1-P3",
                                        "1234567", "ABCDE", "123456789ABC");

    callouts.addCallout(std::move(c2));

    auto c3 = std::make_unique<Callout>(CalloutPriority::high, "U1-P4",
                                        "1234567", "ABCDE1", "123456789ABC");

    callouts.addCallout(std::move(c3));

    auto c4 = std::make_unique<Callout>(CalloutPriority::high, "U1-P5",
                                        "1234567", "ABCDE2", "123456789ABC");

    callouts.addCallout(std::move(c4));

    auto c5 = std::make_unique<Callout>(CalloutPriority::low, "U1-P6",
                                        "1234567", "ABCDE2", "123456789ABC");

    callouts.addCallout(std::move(c5));

    auto c6 = std::make_unique<Callout>(CalloutPriority::medium, "U1-P7",
                                        "1234567", "ABCD2", "123456789ABC");

    callouts.addCallout(std::move(c6));

    auto c7 = std::make_unique<Callout>(CalloutPriority::mediumGroupA, "U1-P8",
                                        "1234567", "ABCDE3", "123456789ABC");

    callouts.addCallout(std::move(c7));

    auto c8 = std::make_unique<Callout>(CalloutPriority::mediumGroupC, "U1-P9",
                                        "1234567", "ABCDE4", "123456789ABC");

    callouts.addCallout(std::move(c8));

    auto c9 = std::make_unique<Callout>(CalloutPriority::low, "U1-P10",
                                        "1234567", "ABCDE3", "123456789ABC");

    callouts.addCallout(std::move(c9));

    const auto& calloutObjects = callouts.callouts();
    EXPECT_EQ(calloutObjects[0]->locationCode(), "U1-P1");
    EXPECT_EQ(calloutObjects[0]->priority(), 'H');
    EXPECT_EQ(calloutObjects[1]->locationCode(), "U1-P4");
    EXPECT_EQ(calloutObjects[1]->priority(), 'H');
    EXPECT_EQ(calloutObjects[2]->locationCode(), "U1-P5");
    EXPECT_EQ(calloutObjects[2]->priority(), 'H');
    EXPECT_EQ(calloutObjects[3]->locationCode(), "U1-P2");
    EXPECT_EQ(calloutObjects[3]->priority(), 'M');
    EXPECT_EQ(calloutObjects[4]->locationCode(), "U1-P7");
    EXPECT_EQ(calloutObjects[4]->priority(), 'M');
    EXPECT_EQ(calloutObjects[5]->locationCode(), "U1-P8");
    EXPECT_EQ(calloutObjects[5]->priority(), 'A');
    EXPECT_EQ(calloutObjects[6]->locationCode(), "U1-P9");
    EXPECT_EQ(calloutObjects[6]->priority(), 'C');
    EXPECT_EQ(calloutObjects[7]->locationCode(), "U1-P3");
    EXPECT_EQ(calloutObjects[7]->priority(), 'L');
    EXPECT_EQ(calloutObjects[8]->locationCode(), "U1-P6");
    EXPECT_EQ(calloutObjects[8]->priority(), 'L');
    EXPECT_EQ(calloutObjects[9]->locationCode(), "U1-P10");
    EXPECT_EQ(calloutObjects[9]->priority(), 'L');
}

TEST(CalloutsTest, TestDupCallouts)
{
    {
        // Duplicate callouts, keep the high priority one
        Callouts callouts;
        auto c0 = std::make_unique<Callout>(CalloutPriority::medium, "U1-P1",
                                            "1234567", "ABC", "123456789ABC");
        callouts.addCallout(std::move(c0));

        auto c1 = std::make_unique<Callout>(CalloutPriority::high, "U1-P1",
                                            "1234567", "ABCD", "123456789ABC");
        callouts.addCallout(std::move(c1));

        EXPECT_EQ(callouts.callouts().size(), 1);
        const auto& calloutObjects = callouts.callouts();
        EXPECT_EQ(calloutObjects[0]->priority(), 'H');
    }

    {
        // Different callouts, keep them both
        Callouts callouts;
        auto c0 = std::make_unique<Callout>(CalloutPriority::high, "U1-P1",
                                            "1234567", "ABC", "123456789ABC");
        callouts.addCallout(std::move(c0));

        auto c1 = std::make_unique<Callout>(CalloutPriority::medium, "U1-P2",
                                            "1234567", "ABCD", "123456789ABC");
        callouts.addCallout(std::move(c1));

        EXPECT_EQ(callouts.callouts().size(), 2);
    }

    {
        // Two duplicates and two unique.  Needs sorting.
        Callouts callouts;
        auto c0 = std::make_unique<Callout>(CalloutPriority::low, "U1-P9",
                                            "1234567", "ABCD", "123456789ABC");
        callouts.addCallout(std::move(c0));

        auto c1 = std::make_unique<Callout>(CalloutPriority::low, "U1-P1",
                                            "1234567", "ABC", "123456789ABC");
        callouts.addCallout(std::move(c1));

        auto c2 = std::make_unique<Callout>(CalloutPriority::high, "U1-P1",
                                            "1234567", "ABC", "123456789ABC");
        callouts.addCallout(std::move(c2));

        auto c3 = std::make_unique<Callout>(CalloutPriority::medium, "U1-P5",
                                            "1234567", "ABCD", "123456789ABC");
        callouts.addCallout(std::move(c3));

        const auto& calloutObjects = callouts.callouts();
        EXPECT_EQ(callouts.callouts().size(), 3);
        EXPECT_EQ(calloutObjects[0]->priority(), 'H');
        EXPECT_EQ(calloutObjects[0]->locationCode(), "U1-P1");
        EXPECT_EQ(calloutObjects[1]->priority(), 'M');
        EXPECT_EQ(calloutObjects[1]->locationCode(), "U1-P5");
        EXPECT_EQ(calloutObjects[2]->priority(), 'L');
        EXPECT_EQ(calloutObjects[2]->locationCode(), "U1-P9");
    }
}
