
#include "bmc_pos_mgr.hpp"

#include <gtest/gtest.h>

using namespace phosphor::logging;

TEST(BMCPosMgrTest, SetBMCPosition)
{
    BMCPosMgr mgr;

    // Set position 0
    {
        std::optional<size_t> bmcPos = 0;
        mgr.setBMCPosition(bmcPos);
        EXPECT_EQ(mgr.bmcPosition, 0);
    }

    // Set position 1
    {
        std::optional<size_t> bmcPos = 1;
        mgr.setBMCPosition(bmcPos);
        EXPECT_EQ(mgr.bmcPosition, 1);
    }

    // Postion not on D-Bus - std::nullopt
    {
        std::optional<size_t> bmcPos = std::nullopt;
        mgr.setBMCPosition(bmcPos);
        EXPECT_EQ(mgr.bmcPosition, std::nullopt);
    }

    // Position can't be obtained - maxint
    {
        std::optional<size_t> bmcPos = std::numeric_limits<size_t>::max();
        mgr.setBMCPosition(bmcPos);
        EXPECT_EQ(mgr.bmcPosition, std::nullopt);
    }
}

TEST(BMCPosMgrTest, ProcessEntryId)
{
    struct TestEntry
    {
        std::optional<size_t> pos; // Position coming from D-Bus
        uint32_t id;               // Current entry ID
        uint32_t expected;         // EntryID value after processEntryId()
    };

    const std::vector<TestEntry> tests = {
        // No position change no rollover
        {0, 1, 1},
        {1, 1, 0x01000001},
        {std::nullopt, 0xFF000000, 0xFF000000},

        // Change positions
        {0, 0x01000001, 0x00000001},
        {1, 0x00000001, 0x01000001},
        {std::nullopt, 0x00000001, 0xFF000001},
        {0, 0xFF000001, 0x00000001},

        // Rollovers
        {std::nullopt, 0xFFFFFFFF, 0xFF000000},
        {0, 0x00FFFFFF, 0x00000000},
        {1, 0x01FFFFFF, 0x01000000},
        {1, 0xFFFFFFFF, 0x01000000}};

    for (const auto& test : tests)
    {
        BMCPosMgr mgr;
        mgr.setBMCPosition(test.pos);

        EXPECT_EQ(mgr.processEntryId(test.id), test.expected);
    }
}
