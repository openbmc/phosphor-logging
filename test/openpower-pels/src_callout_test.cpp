#include "extensions/openpower-pels/callout.hpp"
#include "pel_utils.hpp"

#include <gtest/gtest.h>

using namespace openpower::pels;
using namespace openpower::pels::src;

// Unflatten the callout section with all three substructures
TEST(CalloutTest, TestUnflattenAllSubstructures)
{
    // The base data.
    std::vector<uint8_t> data{
        0xFF, 0x2F, 'H', 8, // size, flags, priority, LC length
        'U',  '1',  '2', '-', 'P', '1', 0x00, 0x00 // LC
    };

    auto fruIdentity = srcDataFactory(TestSRCType::fruIdentityStructure);
    auto pceIdentity = srcDataFactory(TestSRCType::pceIdentityStructure);
    auto mrus = srcDataFactory(TestSRCType::mruStructure);

    // Add all 3 substructures
    data.insert(data.end(), fruIdentity.begin(), fruIdentity.end());
    data.insert(data.end(), pceIdentity.begin(), pceIdentity.end());
    data.insert(data.end(), mrus.begin(), mrus.end());

    // The final size
    data[0] = data.size();

    Stream stream{data};
    Callout callout{stream};

    EXPECT_EQ(callout.flattenedSize(), data.size());
    EXPECT_EQ(callout.priority(), 'H');
    EXPECT_EQ(callout.locationCode(), "U12-P1");

    // Spot check the 3 substructures
    EXPECT_TRUE(callout.fruIdentity());
    EXPECT_EQ(callout.fruIdentity()->getSN(), "123456789ABC");

    EXPECT_TRUE(callout.pceIdentity());
    EXPECT_EQ(callout.pceIdentity()->enclosureName(), "PCENAME12");

    EXPECT_TRUE(callout.mru());
    EXPECT_EQ(callout.mru()->mrus().size(), 4);
    EXPECT_EQ(callout.mru()->mrus().at(3).id, 0x04040404);

    // Now flatten
    std::vector<uint8_t> newData;
    Stream newStream{newData};

    callout.flatten(newStream);
    EXPECT_EQ(data, newData);
}

TEST(CalloutTest, TestUnflattenOneSubstructure)
{
    std::vector<uint8_t> data{
        0xFF, 0x28, 'H', 0x08, // size, flags, priority, LC length
        'U',  '1',  '2', '-',  'P', '1', 0x00, 0x00 // LC
    };

    auto fruIdentity = srcDataFactory(TestSRCType::fruIdentityStructure);

    data.insert(data.end(), fruIdentity.begin(), fruIdentity.end());

    // The final size
    data[0] = data.size();

    Stream stream{data};
    Callout callout{stream};

    EXPECT_EQ(callout.flattenedSize(), data.size());

    // Spot check the substructure
    EXPECT_TRUE(callout.fruIdentity());
    EXPECT_EQ(callout.fruIdentity()->getSN(), "123456789ABC");

    // Not present
    EXPECT_FALSE(callout.pceIdentity());
    EXPECT_FALSE(callout.mru());

    // Now flatten
    std::vector<uint8_t> newData;
    Stream newStream{newData};

    callout.flatten(newStream);
    EXPECT_EQ(data, newData);
}

TEST(CalloutTest, TestUnflattenTwoSubstructures)
{
    std::vector<uint8_t> data{
        0xFF, 0x2B, 'H', 0x08, // size, flags, priority, LC length
        'U',  '1',  '2', '-',  'P', '1', 0x00, 0x00 // LC
    };

    auto fruIdentity = srcDataFactory(TestSRCType::fruIdentityStructure);
    auto pceIdentity = srcDataFactory(TestSRCType::pceIdentityStructure);

    data.insert(data.end(), fruIdentity.begin(), fruIdentity.end());
    data.insert(data.end(), pceIdentity.begin(), pceIdentity.end());

    // The final size
    data[0] = data.size();

    Stream stream{data};
    Callout callout{stream};

    EXPECT_EQ(callout.flattenedSize(), data.size());

    // Spot check the 2 substructures
    EXPECT_TRUE(callout.fruIdentity());
    EXPECT_EQ(callout.fruIdentity()->getSN(), "123456789ABC");

    EXPECT_TRUE(callout.pceIdentity());
    EXPECT_EQ(callout.pceIdentity()->enclosureName(), "PCENAME12");

    // Not present
    EXPECT_FALSE(callout.mru());

    // Now flatten
    std::vector<uint8_t> newData;
    Stream newStream{newData};

    callout.flatten(newStream);
    EXPECT_EQ(data, newData);
}

TEST(CalloutTest, TestNoLocationCode)
{
    std::vector<uint8_t> data{
        0xFF, 0x2B, 'H', 0x00 // size, flags, priority, LC length
    };

    auto fruIdentity = srcDataFactory(TestSRCType::fruIdentityStructure);
    data.insert(data.end(), fruIdentity.begin(), fruIdentity.end());

    // The final size
    data[0] = data.size();

    Stream stream{data};
    Callout callout{stream};

    EXPECT_TRUE(callout.locationCode().empty());
}
