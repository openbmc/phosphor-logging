// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "extensions/openpower-pels/extended_user_data.hpp"
#include "pel_utils.hpp"

#include <gtest/gtest.h>

using namespace openpower::pels;

TEST(ExtUserDataTest, UnflattenFlattenTest)
{
    auto eudSectionData = pelDataFactory(TestPELType::extendedUserDataSection);
    Stream stream(eudSectionData);
    ExtendedUserData eud(stream);

    EXPECT_TRUE(eud.valid());
    EXPECT_EQ(eud.header().id, 0x4544);
    EXPECT_EQ(eud.header().size, eudSectionData.size());
    EXPECT_EQ(eud.header().version, 0x01);
    EXPECT_EQ(eud.header().subType, 0x02);
    EXPECT_EQ(eud.header().componentID, 0x2000);
    EXPECT_EQ(eud.creatorID(), 'O');

    const auto& data = eud.data();

    // The eudSectionData itself starts 4B after the 8B header
    EXPECT_EQ(data.size(), eudSectionData.size() - 12);

    for (size_t i = 0; i < data.size(); i++)
    {
        EXPECT_EQ(data[i], eudSectionData[i + 12]);
    }

    // Now flatten
    std::vector<uint8_t> newData;
    Stream newStream(newData);
    eud.flatten(newStream);

    EXPECT_EQ(eudSectionData, newData);
}

TEST(ExtUserDataTest, BadDataTest)
{
    auto data = pelDataFactory(TestPELType::extendedUserDataSection);
    data.resize(8); // Too small

    Stream stream(data);
    ExtendedUserData eud(stream);
    EXPECT_FALSE(eud.valid());
}

TEST(ExtUserDataTest, BadSizeFieldTest)
{
    auto data = pelDataFactory(TestPELType::extendedUserDataSection);

    {
        data[3] = 0xFF; // Set the size field too large
        Stream stream(data);
        ExtendedUserData eud(stream);
        EXPECT_FALSE(eud.valid());
    }
    {
        data[3] = 0x7; // Set the size field too small
        Stream stream(data);
        ExtendedUserData eud(stream);
        EXPECT_FALSE(eud.valid());
    }
}

TEST(ExtUserDataTest, ConstructorTest)
{
    std::vector<uint8_t> data{0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

    ExtendedUserData eud{0x1112, 0x42, 0x01, 'B', data};

    EXPECT_TRUE(eud.valid());
    EXPECT_EQ(eud.header().id, 0x4544);
    EXPECT_EQ(eud.header().size, 20);
    EXPECT_EQ(eud.header().subType, 0x42);
    EXPECT_EQ(eud.header().version, 0x01);
    EXPECT_EQ(eud.header().componentID, 0x1112);
    EXPECT_EQ(eud.flattenedSize(), 20);
    EXPECT_EQ(eud.creatorID(), 'B');

    const auto& d = eud.data();

    EXPECT_EQ(d, data);
}

TEST(ExtUserDataTest, ShrinkTest)
{
    std::vector<uint8_t> data(100, 0xFF);

    ExtendedUserData eud(0x1112, 0x42, 0x01, 'O', data);
    EXPECT_TRUE(eud.valid());

    // 4B aligned
    EXPECT_TRUE(eud.shrink(88));
    EXPECT_EQ(eud.flattenedSize(), 88);
    EXPECT_EQ(eud.header().size, 88);

    // rounded off
    EXPECT_TRUE(eud.shrink(87));
    EXPECT_EQ(eud.flattenedSize(), 84);
    EXPECT_EQ(eud.header().size, 84);

    // too big
    EXPECT_FALSE(eud.shrink(200));
    EXPECT_EQ(eud.flattenedSize(), 84);
    EXPECT_EQ(eud.header().size, 84);

    // way too small
    EXPECT_FALSE(eud.shrink(3));
    EXPECT_EQ(eud.flattenedSize(), 84);
    EXPECT_EQ(eud.header().size, 84);

    // the smallest it can go
    EXPECT_TRUE(eud.shrink(16));
    EXPECT_EQ(eud.flattenedSize(), 16);
    EXPECT_EQ(eud.header().size, 16);

    // one too small
    EXPECT_FALSE(eud.shrink(15));
    EXPECT_EQ(eud.flattenedSize(), 16);
    EXPECT_EQ(eud.header().size, 16);
}
