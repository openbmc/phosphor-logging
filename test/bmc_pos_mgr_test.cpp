// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright OpenBMC Authors

#include "bmc_pos_mgr.hpp"

#include <fstream>

#include <gtest/gtest.h>

using namespace phosphor::logging;

class PosFile
{
  public:
    PosFile(size_t pos)
    {
        auto fd = mkstemp(path.data());
        if (fd == -1)
        {
            throw std::runtime_error{
                std::string{"Unable to create temporary file: "} +
                strerror(errno)};
        }
        close(fd);
        write(pos);
    }

    ~PosFile()
    {
        remove();
    }

    void write(size_t pos)
    {
        std::ofstream stream{path};
        if (!stream)
        {
            throw std::runtime_error("Could not open " + path);
        }
        stream << pos;
    }

    void remove()
    {
        std::error_code ec;
        std::filesystem::remove(path, ec);
    }

    std::string path{std::filesystem::temp_directory_path() / "bmc-pos-XXXXXX"};
};

TEST(BMCPosMgrTest, ReadBMCPosition)
{
    PosFile file{0};
    BMCPosMgr mgr{file.path};

    EXPECT_EQ(mgr.getBMCPosition(), 0);

    file.write(1);
    mgr.readBMCPosition();
    EXPECT_EQ(mgr.getBMCPosition(), 1);

    file.write(0xFF);
    mgr.readBMCPosition();
    EXPECT_EQ(mgr.getBMCPosition(), 0xFF);

    file.write(std::numeric_limits<size_t>::max());
    mgr.readBMCPosition();
    EXPECT_EQ(mgr.getBMCPosition(), 0xFF);

    file.remove();
    mgr.readBMCPosition();
    EXPECT_EQ(mgr.getBMCPosition(), 0xFF);
}

TEST(BMCPosMgrTest, ProcessEntryId)
{
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
        PosFile file{test.pos};
        BMCPosMgr mgr{file.path};

        EXPECT_EQ(mgr.processEntryId(test.id), test.expected);
    }
}
