#include "extensions/openpower-pels/src.hpp"
#include "pel_utils.hpp"

#include <gtest/gtest.h>

using namespace openpower::pels;

TEST(SRCTest, UnflattenFlattenTestNoCallouts)
{
    auto data = srcDataFactory(TestSRCType::primarySRCNoCallouts);

    Stream stream{data};
    SRC src{stream};

    EXPECT_TRUE(src.valid());

    EXPECT_EQ(src.header().id, 0x5053);
    EXPECT_EQ(src.header().size, 0x80);
    EXPECT_EQ(src.header().version, 0x01);
    EXPECT_EQ(src.header().subType, 0x01);
    EXPECT_EQ(src.header().componentID, 0x0202);

    EXPECT_EQ(src.version(), 0x02);
    EXPECT_EQ(src.flags(), 0x00);
    EXPECT_EQ(src.hexWordCount(), 9);
    EXPECT_EQ(src.size(), 0x48);

    const auto& hexwords = src.hexwordData();
    EXPECT_EQ(0x02020202, hexwords[0]);
    EXPECT_EQ(0x03030303, hexwords[1]);
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

TEST(SRCTest, UnflattenFlattenTest2Callouts)
{
    auto data = srcDataFactory(TestSRCType::primarySRC2Callouts);

    Stream stream{data};
    SRC src{stream};

    EXPECT_TRUE(src.valid());

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
