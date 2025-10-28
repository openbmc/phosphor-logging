// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "extensions/openpower-pels/ascii_string.hpp"
#include "extensions/openpower-pels/registry.hpp"

#include <gtest/gtest.h>

using namespace openpower::pels;

TEST(AsciiStringTest, AsciiStringTest)
{
    // Build the ASCII string from a message registry entry
    message::Entry entry;
    entry.src.type = 0xBD;
    entry.src.reasonCode = 0xABCD;
    entry.subsystem = 0x37;

    src::AsciiString as{entry};

    auto data = as.get();

    EXPECT_EQ(data, "BD37ABCD                        ");

    // Now flatten it
    std::vector<uint8_t> flattenedData;
    Stream stream{flattenedData};

    as.flatten(stream);

    for (size_t i = 0; i < 32; i++)
    {
        EXPECT_EQ(data[i], flattenedData[i]);
    }
}

// A 0x11 power SRC doesn't have the subsystem in it
TEST(AsciiStringTest, PowerErrorTest)
{
    message::Entry entry;
    entry.src.type = 0x11;
    entry.src.reasonCode = 0xABCD;
    entry.subsystem = 0x37;

    src::AsciiString as{entry};
    auto data = as.get();

    EXPECT_EQ(data, "1100ABCD                        ");
}

TEST(AsciiStringTest, UnflattenTest)
{
    std::vector<uint8_t> rawData{'B', 'D', '5', '6', '1', '2', 'A', 'B'};

    for (int i = 8; i < 32; i++)
    {
        rawData.push_back(' ');
    }

    Stream stream{rawData};
    src::AsciiString as{stream};

    auto data = as.get();

    EXPECT_EQ(data, "BD5612AB                        ");
}

TEST(AsciiStringTest, UnderflowTest)
{
    std::vector<uint8_t> rawData{'B', 'D', '5', '6'};
    Stream stream{rawData};

    EXPECT_THROW(src::AsciiString as{stream}, std::out_of_range);
}

TEST(AsciiStringTest, MissingSubsystemTest)
{
    message::Entry entry;
    entry.src.type = 0xBD;
    entry.src.reasonCode = 0xABCD;

    src::AsciiString as{entry};
    auto data = as.get();

    // Default subsystem is 0x70
    EXPECT_EQ(data, "BD70ABCD                        ");
}
