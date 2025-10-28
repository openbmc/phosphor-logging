// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "extensions/openpower-pels/pce_identity.hpp"

#include <gtest/gtest.h>

using namespace openpower::pels;
using namespace openpower::pels::src;

TEST(PCEIdentityTest, TestConstructor)
{
    std::vector<uint8_t> data{
        'P', 'E', 0x24, 0x00,                      // type, size, flags
        'T', 'T', 'T',  'T',  '-', 'M', 'M',  'M', // MTM
        '1', '2', '3',  '4',  '5', '6', '7',       // SN
        '8', '9', 'A',  'B',  'C', 'P', 'C',  'E', // Name + null padded
        'N', 'A', 'M',  'E',  '1', '2', 0x00, 0x00, 0x00};

    Stream stream{data};

    PCEIdentity pce{stream};

    EXPECT_EQ(pce.flattenedSize(), data.size());
    EXPECT_EQ(pce.enclosureName(), "PCENAME12");
    EXPECT_EQ(pce.mtms().machineTypeAndModel(), "TTTT-MMM");
    EXPECT_EQ(pce.mtms().machineSerialNumber(), "123456789ABC");

    // Flatten it
    std::vector<uint8_t> newData;
    Stream newStream{newData};
    pce.flatten(newStream);

    EXPECT_EQ(data, newData);
}

TEST(PCEIdentityTest, TestBadData)
{
    std::vector<uint8_t> data{
        'P', 'E', 0x20, 0x00, 'T', 'T', 'T', 'T', '-',
    };

    Stream stream{data};
    EXPECT_THROW(PCEIdentity pce{stream}, std::out_of_range);
}
