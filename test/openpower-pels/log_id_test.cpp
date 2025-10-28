// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "extensions/openpower-pels/log_id.hpp"
#include "extensions/openpower-pels/paths.hpp"

#include <arpa/inet.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <thread>

#include <gtest/gtest.h>

using namespace openpower::pels;
namespace fs = std::filesystem;

TEST(LogIdTest, TimeBasedIDTest)
{
    uint32_t lastID = 0;
    for (int i = 0; i < 10; i++)
    {
        auto id = detail::getTimeBasedLogID();

        EXPECT_EQ(id & 0xFF000000, 0x50000000);
        EXPECT_NE(id, lastID);
        lastID = id;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

TEST(LogIdTest, IDTest)
{
    EXPECT_EQ(generatePELID(), 0x50000001);
    EXPECT_EQ(generatePELID(), 0x50000002);
    EXPECT_EQ(generatePELID(), 0x50000003);
    EXPECT_EQ(generatePELID(), 0x50000004);
    EXPECT_EQ(generatePELID(), 0x50000005);
    EXPECT_EQ(generatePELID(), 0x50000006);

    auto backingFile = getPELIDFile();
    fs::remove(backingFile);
    EXPECT_EQ(generatePELID(), 0x50000001);
    EXPECT_EQ(generatePELID(), 0x50000002);
    EXPECT_EQ(generatePELID(), 0x50000003);

    fs::remove_all(fs::path{backingFile}.parent_path());
}

TEST(LogIdTest, PELIDTest)
{
    // Get PEL ID file updated with binary zeros
    auto backingFile = getPELIDFile();
    std::ofstream wf{backingFile, std::ios::binary};
    char id = '\0';
    for (int i = 0; i < 4; i++)
    {
        wf.write(&id, sizeof(id));
    }
    wf.close();

    // Expect existing PEL ID file to be deleted and
    // new PEL ID regenerated
    EXPECT_EQ(generatePELID(), 0x50000001);
    EXPECT_EQ(generatePELID(), 0x50000002);
    EXPECT_EQ(generatePELID(), 0x50000003);
    EXPECT_EQ(generatePELID(), 0x50000004);
    EXPECT_EQ(generatePELID(), 0x50000005);

    // Get PEL ID file updated with binary zeros again
    std::ofstream fw{backingFile, std::ios::binary};
    for (int i = 0; i < 4; i++)
    {
        fw.write(&id, sizeof(id));
    }
    fw.close();

    // This time PEL IDs are random generated
    EXPECT_NE(generatePELID(), 0x50000001);
    EXPECT_NE(generatePELID(), 0x50000002);
    EXPECT_NE(generatePELID(), 0x50000003);
    EXPECT_NE(generatePELID(), 0x50000004);
    EXPECT_NE(generatePELID(), 0x50000005);

    fs::remove_all(fs::path{backingFile}.parent_path());
}
