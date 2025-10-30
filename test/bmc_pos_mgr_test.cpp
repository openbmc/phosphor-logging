// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright OpenBMC Authors

#include "bmc_pos_mgr.hpp"

#include <gtest/gtest.h>

using namespace phosphor::logging;

TEST(BMCPosMgrTest, ProcessEntryId)
{
    auto bus = sdbusplus::bus::new_default();
    struct TestEntry
    {
        uint8_t pos;       // Position
        uint32_t id;       // Current entry ID
        uint32_t expected; // EntryID value after processEntryId()
    };

    const std::vector<TestEntry> tests = {
        // No position change no rollover
        {0, 1, 1},
        {1, 1, 0x01000001},
        {0xFF, 0xFF000000, 0xFF000000},

        // Change positions
        {0, 0x01000001, 0x00000001},
        {1, 0x00000001, 0x01000001},
        {0xFF, 0x00000001, 0xFF000001},
        {0, 0xFF000001, 0x00000001},

        // Rollovers
        {0xFF, 0xFFFFFFFF, 0xFF000000},
        {0, 0x00FFFFFF, 0x00000000},
        {1, 0x01FFFFFF, 0x01000000},
        {1, 0xFFFFFFFF, 0x01000000}};

    for (const auto& test : tests)
    {
        BMCPosMgr mgr{bus};
        mgr.bmcPosition = test.pos;

        EXPECT_EQ(mgr.processEntryId(test.id), test.expected);
    }
}

TEST(BMCPosMgrTest, IdContainsCurrentPosition)
{
    auto bus = sdbusplus::bus::new_default();
    BMCPosMgr mgr{bus};

    mgr.bmcPosition = 0;

    EXPECT_TRUE(mgr.idContainsCurrentPosition(0x00000001));
    EXPECT_FALSE(mgr.idContainsCurrentPosition(0x01000001));

    mgr.bmcPosition = 0xFF;

    EXPECT_TRUE(mgr.idContainsCurrentPosition(0xFF000001));
    EXPECT_FALSE(mgr.idContainsCurrentPosition(0x01000001));
}
