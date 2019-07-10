#include "extensions/openpower-pels/log_id.hpp"
#include "extensions/openpower-pels/paths.hpp"

#include <arpa/inet.h>

#include <filesystem>

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
