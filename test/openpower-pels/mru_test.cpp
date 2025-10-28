// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "extensions/openpower-pels/mru.hpp"

#include <gtest/gtest.h>

using namespace openpower::pels;
using namespace openpower::pels::src;

TEST(MRUTest, TestConstructor)
{
    std::vector<uint8_t> data{
        'M',  'R',  0x28, 0x04, // ID, size, flags
        0x00, 0x00, 0x00, 0x00, // Reserved
        0x00, 0x00, 0x00, 'H',  // priority for MRU ID 0
        0x01, 0x01, 0x01, 0x01, // MRU ID 0
        0x00, 0x00, 0x00, 'M',  // priority for MRU ID 1
        0x02, 0x02, 0x02, 0x02, // MRU ID 1
        0x00, 0x00, 0x00, 'L',  // priority for MRU ID 2
        0x03, 0x03, 0x03, 0x03, // MRU ID 2
        0x00, 0x00, 0x00, 'H',  // priority for MRU ID 3
        0x04, 0x04, 0x04, 0x04, // MRU ID 3
    };

    Stream stream{data};

    MRU mru{stream};

    EXPECT_EQ(mru.flattenedSize(), data.size());
    EXPECT_EQ(mru.mrus().size(), 4);
    EXPECT_EQ(mru.flags(), 4);

    EXPECT_EQ(mru.mrus().at(0).priority, 'H');
    EXPECT_EQ(mru.mrus().at(0).id, 0x01010101);
    EXPECT_EQ(mru.mrus().at(1).priority, 'M');
    EXPECT_EQ(mru.mrus().at(1).id, 0x02020202);
    EXPECT_EQ(mru.mrus().at(2).priority, 'L');
    EXPECT_EQ(mru.mrus().at(2).id, 0x03030303);
    EXPECT_EQ(mru.mrus().at(3).priority, 'H');
    EXPECT_EQ(mru.mrus().at(3).id, 0x04040404);

    // Now flatten
    std::vector<uint8_t> newData;
    Stream newStream{newData};

    mru.flatten(newStream);
    EXPECT_EQ(data, newData);
}

TEST(MRUTest, TestBadData)
{
    // 4 MRUs expected, but only 1
    std::vector<uint8_t> data{
        'M',  'R',  0x28, 0x04, // ID, size, flags
        0x00, 0x00, 0x00, 0x00, // Reserved
        0x00, 0x00, 0x00, 'H',  // priority 0
        0x01, 0x01, 0x01, 0x01, // MRU ID 0
    };

    Stream stream{data};
    EXPECT_THROW(MRU mru{stream}, std::out_of_range);
}

TEST(MRUTest, TestVectorConstructor)
{
    {
        std::vector<MRU::MRUCallout> mrus{{'H', 1}, {'M', 2}, {'L', 3}};

        MRU mru{mrus};

        EXPECT_EQ(mru.mrus().size(), 3);
        EXPECT_EQ(mru.flags(), 3);

        EXPECT_EQ(mru.mrus().at(0).priority, 'H');
        EXPECT_EQ(mru.mrus().at(0).id, 1);
        EXPECT_EQ(mru.mrus().at(1).priority, 'M');
        EXPECT_EQ(mru.mrus().at(1).id, 2);
        EXPECT_EQ(mru.mrus().at(2).priority, 'L');
        EXPECT_EQ(mru.mrus().at(2).id, 3);

        // Flatten and unflatten
        std::vector<uint8_t> data;
        Stream stream{data};

        mru.flatten(stream);
        EXPECT_EQ(mru.size(), data.size());

        stream.offset(0);
        MRU newMRU{stream};

        EXPECT_EQ(newMRU.flattenedSize(), data.size());
        EXPECT_EQ(newMRU.size(), data.size());
        EXPECT_EQ(newMRU.mrus().size(), 3);

        EXPECT_EQ(newMRU.mrus().at(0).priority, 'H');
        EXPECT_EQ(newMRU.mrus().at(0).id, 1);
        EXPECT_EQ(newMRU.mrus().at(1).priority, 'M');
        EXPECT_EQ(newMRU.mrus().at(1).id, 2);
        EXPECT_EQ(newMRU.mrus().at(2).priority, 'L');
        EXPECT_EQ(newMRU.mrus().at(2).id, 3);
    }

    {
        // Too many MRUs
        std::vector<MRU::MRUCallout> mrus;
        for (uint32_t i = 0; i < 20; i++)
        {
            MRU::MRUCallout mru = {'H', i};
            mrus.push_back(mru);
        }

        MRU mru{mrus};

        EXPECT_EQ(mru.mrus().size(), 15);
        EXPECT_EQ(mru.flags(), 15);
    }

    {
        // Too few MRUs
        std::vector<MRU::MRUCallout> mrus;
        EXPECT_THROW(MRU mru{mrus}, std::runtime_error);
    }
}
