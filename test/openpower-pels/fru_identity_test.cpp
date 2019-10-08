#include "extensions/openpower-pels/fru_identity.hpp"

#include <gtest/gtest.h>

using namespace openpower::pels;
using namespace openpower::pels::src;

// Unflatten a FRUIdentity that is a HW FRU callout
TEST(FRUIdentityTest, TestHardwareFRU)
{
    // Has PN, SN, CCIN
    std::vector<uint8_t> data{'I', 'D', 0x1C, 0x1D, // type, size, flags
                              '1', '2', '3',  '4',  // PN
                              '5', '6', '7',  0x00, 'A', 'A', 'A', 'A', // CCIN
                              '1', '2', '3',  '4',  '5', '6', '7', '8', // SN
                              '9', 'A', 'B',  'C'};

    Stream stream{data};

    FRUIdentity fru{stream};

    EXPECT_EQ(fru.failingComponentType(), FRUIdentity::hardwareFRU);
    EXPECT_EQ(fru.flattenedSize(), data.size());
    EXPECT_TRUE(fru.hasPN());
    EXPECT_TRUE(fru.hasCCIN());
    EXPECT_TRUE(fru.hasSN());
    EXPECT_FALSE(fru.hasMP());

    EXPECT_EQ(fru.getPN().value(), "1234567");
    EXPECT_EQ(fru.getCCIN().value(), "AAAA");
    EXPECT_EQ(fru.getSN().value(), "123456789ABC");
    EXPECT_FALSE(fru.getMaintProc());

    // Flatten
    std::vector<uint8_t> newData;
    Stream newStream{newData};
    fru.flatten(newStream);
    EXPECT_EQ(data, newData);
}

// Unflatten a FRUIdentity that is a Maintenance Procedure callout
TEST(FRUIdentityTest, TestMaintProcedure)
{
    // Only contains the maintenance procedure
    std::vector<uint8_t> data{
        0x49, 0x44, 0x0C, 0x42,                     // type, size, flags
        '1',  '2',  '3',  '4',  '5', '6', '7', 0x00 // Procedure
    };

    Stream stream{data};

    FRUIdentity fru{stream};

    EXPECT_EQ(fru.failingComponentType(), FRUIdentity::maintenanceProc);
    EXPECT_EQ(fru.flattenedSize(), data.size());
    EXPECT_FALSE(fru.hasPN());
    EXPECT_FALSE(fru.hasCCIN());
    EXPECT_FALSE(fru.hasSN());
    EXPECT_TRUE(fru.hasMP());

    EXPECT_EQ(fru.getMaintProc().value(), "1234567");
    EXPECT_FALSE(fru.getPN());
    EXPECT_FALSE(fru.getCCIN());
    EXPECT_FALSE(fru.getSN());

    // Flatten
    std::vector<uint8_t> newData;
    Stream newStream{newData};
    fru.flatten(newStream);
    EXPECT_EQ(data, newData);
}

// Try to unflatten garbage data
TEST(FRUIdentityTest, BadDataTest)
{
    std::vector<uint8_t> data{0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                              0xFF, 0xFF, 0xFF, 0xFF};

    Stream stream{data};

    EXPECT_THROW(FRUIdentity fru{stream}, std::out_of_range);
}
