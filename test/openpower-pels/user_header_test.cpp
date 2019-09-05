#include "extensions/openpower-pels/private_header.hpp"
#include "extensions/openpower-pels/user_header.hpp"
#include "pel_utils.hpp"

#include <gtest/gtest.h>

using namespace openpower::pels;

TEST(UserHeaderTest, SizeTest)
{
    EXPECT_EQ(UserHeader::flattenedSize(), 24);
}

TEST(UserHeaderTest, UnflattenFlattenTest)
{
    auto data = pelDataFactory(TestPelType::userHeaderSimple);

    Stream stream(*data);
    UserHeader uh(stream);
    EXPECT_EQ(uh.valid(), true);

    EXPECT_EQ(uh.header().id, 0x5548);
    EXPECT_EQ(uh.header().size, UserHeader::flattenedSize());
    EXPECT_EQ(uh.header().version, 0x01);
    EXPECT_EQ(uh.header().subType, 0x0A);
    EXPECT_EQ(uh.header().componentID, 0x0B0C);

    EXPECT_EQ(uh.subsystem(), 0x10);
    EXPECT_EQ(uh.scope(), 0x04);
    EXPECT_EQ(uh.severity(), 0x20);
    EXPECT_EQ(uh.eventType(), 0x00);
    EXPECT_EQ(uh.problemDomain(), 0x03);
    EXPECT_EQ(uh.problemVector(), 0x04);
    EXPECT_EQ(uh.actionFlags(), 0x80C0);

    // Now flatten into a vector and check that this vector
    // matches the original one.
    std::vector<uint8_t> newData;
    Stream newStream(newData);

    uh.flatten(newStream);
    EXPECT_EQ(*data, newData);

    // Change a field, then flatten and unflatten again
    uh.subsystem() = 0x44;

    newStream.offset(0);
    newData.clear();
    uh.flatten(newStream);
    EXPECT_NE(*data, newData);

    newStream.offset(0);
    UserHeader newUH(newStream);

    EXPECT_TRUE(newUH.valid());
    EXPECT_EQ(newUH.subsystem(), 0x44);
}

TEST(UserHeaderTest, ShortDataTest)
{
    auto data = pelDataFactory(TestPelType::userHeaderSimple);
    data->resize(data->size() - 1);

    Stream stream(*data);
    UserHeader uh(stream);

    EXPECT_EQ(uh.valid(), false);
}

TEST(UserHeaderTest, CorruptDataTest1)
{
    auto data = pelDataFactory(TestPelType::userHeaderSimple);
    data->resize(data->size() - 1);

    data->at(0) = 0; // corrupt the section ID

    Stream stream(*data);
    UserHeader uh(stream);

    EXPECT_EQ(uh.valid(), false);
}

TEST(UserHeaderTest, CorruptDataTest2)
{
    auto data = pelDataFactory(TestPelType::userHeaderSimple);

    data->at(4) = 0x22; // corrupt the version

    Stream stream(*data);
    UserHeader uh(stream);

    EXPECT_EQ(uh.valid(), false);
}
