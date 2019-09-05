#include "extensions/openpower-pels/private_header.hpp"
#include "pel_utils.hpp"

#include <gtest/gtest.h>

using namespace openpower::pels;

TEST(PrivateHeaderTest, SizeTest)
{
    EXPECT_EQ(PrivateHeader::flattenedSize(), 48);
}

TEST(PrivateHeaderTest, UnflattenFlattenTest)
{
    auto data = pelDataFactory(TestPelType::privateHeaderSimple);

    Stream stream(*data);
    PrivateHeader ph(stream);
    EXPECT_EQ(ph.valid(), true);

    EXPECT_EQ(ph.header().id, 0x5048);
    EXPECT_EQ(ph.header().size, PrivateHeader::flattenedSize());
    EXPECT_EQ(ph.header().version, 0x01);
    EXPECT_EQ(ph.header().subType, 0x02);
    EXPECT_EQ(ph.header().componentID, 0x0304);

    auto& ct = ph.createTimestamp();
    EXPECT_EQ(ct.yearMSB, 0x20);
    EXPECT_EQ(ct.yearLSB, 0x30);
    EXPECT_EQ(ct.month, 0x05);
    EXPECT_EQ(ct.day, 0x09);
    EXPECT_EQ(ct.hour, 0x011);
    EXPECT_EQ(ct.minutes, 0x1E);
    EXPECT_EQ(ct.seconds, 0x01);
    EXPECT_EQ(ct.hundredths, 0x63);

    auto& mt = ph.commitTimestamp();
    EXPECT_EQ(mt.yearMSB, 0x20);
    EXPECT_EQ(mt.yearLSB, 0x31);
    EXPECT_EQ(mt.month, 0x06);
    EXPECT_EQ(mt.day, 0x0F);
    EXPECT_EQ(mt.hour, 0x09);
    EXPECT_EQ(mt.minutes, 0x22);
    EXPECT_EQ(mt.seconds, 0x3A);
    EXPECT_EQ(mt.hundredths, 0x00);

    EXPECT_EQ(ph.creatorID(), 0xAA);
    EXPECT_EQ(ph.logType(), 0x00);
    EXPECT_EQ(ph.sectionCount(), 0x02);
    EXPECT_EQ(ph.obmcLogID(), 0x90919293);

    char expected[] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x00};
    EXPECT_TRUE(memcmp(ph.creatorVersion().version, expected, 8) == 0);

    EXPECT_EQ(ph.plid(), 0x50515253);
    EXPECT_EQ(ph.id(), 0x80818283);

    // Now flatten into a vector and check that this vector
    // matches the original one.
    std::vector<uint8_t> newData;
    Stream newStream(newData);

    ph.flatten(newStream);
    EXPECT_EQ(*data, newData);

    // Change a field, then flatten and unflatten again
    ph.creatorID() = 0x55;

    newStream.offset(0);
    newData.clear();
    ph.flatten(newStream);
    EXPECT_NE(*data, newData);

    newStream.offset(0);
    PrivateHeader newPH(newStream);

    EXPECT_TRUE(newPH.valid());
    EXPECT_EQ(newPH.creatorID(), 0x55);
}

TEST(PrivateHeaderTest, ShortDataTest)
{
    auto data = pelDataFactory(TestPelType::privateHeaderSimple);
    data->resize(PrivateHeader::flattenedSize() - 1);
    Stream stream(*data);

    PrivateHeader ph(stream);

    EXPECT_EQ(ph.valid(), false);
}

TEST(PrivateHeaderTest, CorruptDataTest1)
{
    auto data = pelDataFactory(TestPelType::privateHeaderSimple);
    Stream stream(*data);

    data->at(0) = 0; // corrupt the section ID

    PrivateHeader ph(stream);

    EXPECT_EQ(ph.valid(), false);
}

TEST(PrivateHeaderTest, CorruptDataTest2)
{
    auto data = pelDataFactory(TestPelType::privateHeaderSimple);
    Stream stream(*data);

    data->at(4) = 0x22; // corrupt the version

    PrivateHeader ph(stream);

    EXPECT_EQ(ph.valid(), false);
}

TEST(PrivateHeaderTest, CorruptDataTest3)
{
    auto data = pelDataFactory(TestPelType::privateHeaderSimple);
    Stream stream(*data);

    data->at(27) = 1; // corrupt the section count

    PrivateHeader ph(stream);

    EXPECT_EQ(ph.valid(), false);
}
