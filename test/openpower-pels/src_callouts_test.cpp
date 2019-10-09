#include "extensions/openpower-pels/callouts.hpp"
#include "pel_utils.hpp"

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
