#include "extensions/openpower-pels/pel.hpp"
#include "pel_utils.hpp"

#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

namespace fs = std::filesystem;
using namespace openpower::pels;

class PELTest : public CleanLogID
{
};

TEST_F(PELTest, FlattenTest)
{
    auto data = pelDataFactory(TestPelType::pelSimple);
    auto pel = std::make_unique<PEL>(*data);

    // Check a few fields
    EXPECT_TRUE(pel->valid());
    EXPECT_EQ(pel->id(), 0x80818283);
    EXPECT_EQ(pel->plid(), 0x50515253);
    EXPECT_EQ(pel->userHeader()->subsystem(), 0x10);
    EXPECT_EQ(pel->userHeader()->actionFlags(), 0x80C0);

    // Test that data in == data out
    auto flattenedData = pel->data();
    ASSERT_EQ(*data, flattenedData);
}

TEST_F(PELTest, CommitTimeTest)
{
    auto data = pelDataFactory(TestPelType::pelSimple);
    auto pel = std::make_unique<PEL>(*data);

    auto origTime = pel->commitTime();
    pel->setCommitTime();
    auto newTime = pel->commitTime();

    ASSERT_NE(origTime, newTime);

    // Make a new PEL and check new value is still there
    auto newData = pel->data();
    auto newPel = std::make_unique<PEL>(newData);
    ASSERT_EQ(newTime, newPel->commitTime());
}

TEST_F(PELTest, AssignIDTest)
{
    auto data = pelDataFactory(TestPelType::pelSimple);
    auto pel = std::make_unique<PEL>(*data);

    auto origID = pel->id();
    pel->assignID();
    auto newID = pel->id();

    ASSERT_NE(origID, newID);

    // Make a new PEL and check new value is still there
    auto newData = pel->data();
    auto newPel = std::make_unique<PEL>(newData);
    ASSERT_EQ(newID, newPel->id());
}

TEST_F(PELTest, WithLogIDTest)
{
    auto data = pelDataFactory(TestPelType::pelSimple);
    auto pel = std::make_unique<PEL>(*data, 0x42);

    EXPECT_TRUE(pel->valid());
    EXPECT_EQ(pel->obmcLogID(), 0x42);
}

TEST_F(PELTest, InvalidPELTest)
{
    auto data = pelDataFactory(TestPelType::pelSimple);

    // Too small
    data->resize(PrivateHeader::flattenedSize());

    auto pel = std::make_unique<PEL>(*data);

    EXPECT_TRUE(pel->privateHeader()->valid());
    EXPECT_FALSE(pel->userHeader()->valid());
    EXPECT_FALSE(pel->valid());

    // Ensure we can still flatten bad data
    auto newData = pel->data();
    EXPECT_EQ(*data, newData);

    // Now corrupt the private header
    data = pelDataFactory(TestPelType::pelSimple);
    data->at(0) = 0;
    pel = std::make_unique<PEL>(*data);

    EXPECT_FALSE(pel->privateHeader()->valid());
    EXPECT_TRUE(pel->userHeader()->valid());
    EXPECT_FALSE(pel->valid());
}

TEST_F(PELTest, EmptyDataTest)
{
    std::vector<uint8_t> data;
    auto pel = std::make_unique<PEL>(data);

    EXPECT_FALSE(pel->privateHeader()->valid());
    EXPECT_FALSE(pel->userHeader()->valid());
    EXPECT_FALSE(pel->valid());
}
