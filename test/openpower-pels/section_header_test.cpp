// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "extensions/openpower-pels/section_header.hpp"

#include <gtest/gtest.h>

using namespace openpower::pels;

TEST(SectionHeaderTest, SizeTest)
{
    EXPECT_EQ(SectionHeader::flattenedSize(), 8);
}

TEST(SectionHeaderTest, UnflattenTest)
{
    std::vector<uint8_t> data{0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    Stream reader{data};
    SectionHeader header;

    reader >> header;

    EXPECT_EQ(header.id, 0x1122);
    EXPECT_EQ(header.size, 0x3344);
    EXPECT_EQ(header.version, 0x55);
    EXPECT_EQ(header.subType, 0x66);
    EXPECT_EQ(header.componentID, 0x7788);
}

TEST(SectionHeaderTest, FlattenTest)
{
    SectionHeader header{0xAABB, 0xCCDD, 0xEE, 0xFF, 0xA0A0};

    std::vector<uint8_t> data;
    Stream writer{data};

    writer << header;

    std::vector<uint8_t> expected{0xAA, 0xBB, 0xCC, 0xDD,
                                  0xEE, 0xFF, 0xA0, 0xA0};
    EXPECT_EQ(data, expected);
}
