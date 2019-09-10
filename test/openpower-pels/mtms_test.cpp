#include "extensions/openpower-pels/mtms.hpp"

#include <gtest/gtest.h>

using namespace openpower::pels;

TEST(MTMSTest, SizeTest)
{
    EXPECT_EQ(MTMS::flattenedSize(), 20);
}

TEST(MTMSTest, ConstructorTest)
{
    {
        std::string tm{"TTTT-MMM"};
        std::string sn{"123456789ABC"};

        MTMS mtms{tm, sn};

        std::array<uint8_t, 8> t{'T', 'T', 'T', 'T', '-', 'M', 'M', 'M'};
        EXPECT_EQ(t, mtms.machineTypeAndModel());

        std::array<uint8_t, 12> s{'1', '2', '3', '4', '5', '6',
                                  '7', '8', '9', 'A', 'B', 'C'};
        EXPECT_EQ(s, mtms.machineSerialNumber());
    }

    {
        // too long- truncate it
        std::string tm{"TTTT-MMME"};
        std::string sn{"123456789ABCE"};

        MTMS mtms{tm, sn};

        std::array<uint8_t, 8> t{'T', 'T', 'T', 'T', '-', 'M', 'M', 'M'};
        EXPECT_EQ(t, mtms.machineTypeAndModel());

        std::array<uint8_t, 12> s{'1', '2', '3', '4', '5', '6',
                                  '7', '8', '9', 'A', 'B', 'C'};
        EXPECT_EQ(s, mtms.machineSerialNumber());
    }

    {
        // short
        std::string tm{"TTTT"};
        std::string sn{"1234"};

        MTMS mtms{tm, sn};

        std::array<uint8_t, 8> t{'T', 'T', 'T', 'T', 0, 0, 0, 0};
        EXPECT_EQ(t, mtms.machineTypeAndModel());

        std::array<uint8_t, 12> s{'1', '2', '3', '4', 0, 0, 0, 0, 0, 0, 0, 0};
        EXPECT_EQ(s, mtms.machineSerialNumber());
    }

    {
        // Stream constructor
        std::vector<uint8_t> data{'T', 'T', 'T', 'T', '-', 'M', 'M',
                                  'M', '1', '2', '3', '4', '5', '6',
                                  '7', '8', '9', 'A', 'B', 'C'};
        Stream stream{data};

        MTMS mtms{stream};

        std::array<uint8_t, 8> t{'T', 'T', 'T', 'T', '-', 'M', 'M', 'M'};
        EXPECT_EQ(t, mtms.machineTypeAndModel());

        std::array<uint8_t, 12> s{'1', '2', '3', '4', '5', '6',
                                  '7', '8', '9', 'A', 'B', 'C'};
        EXPECT_EQ(s, mtms.machineSerialNumber());
    }
}

TEST(MTMSTest, OperatorExtractTest)
{
    std::string tm{"TTTT-MMM"};
    std::string sn{"123456789ABC"};

    MTMS mtms{tm, sn};

    // Check that we can extract the same data
    std::vector<uint8_t> data;
    Stream stream{data};
    stream << mtms;

    std::vector<uint8_t> expected{'T', 'T', 'T', 'T', '-', 'M', 'M',
                                  'M', '1', '2', '3', '4', '5', '6',
                                  '7', '8', '9', 'A', 'B', 'C'};
    EXPECT_EQ(expected, data);
}

TEST(MTMSTest, OperatorInsertTest)
{
    std::vector<uint8_t> data{'T', 'T', 'T', 'T', '-', 'M', 'M', 'M', '1', '2',
                              '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C'};
    Stream stream{data};

    // Check that when we insert data it is what's expected
    MTMS mtms;
    stream >> mtms;

    std::array<uint8_t, 8> t{'T', 'T', 'T', 'T', '-', 'M', 'M', 'M'};
    EXPECT_EQ(t, mtms.machineTypeAndModel());

    std::array<uint8_t, 12> s{'1', '2', '3', '4', '5', '6',
                              '7', '8', '9', 'A', 'B', 'C'};
    EXPECT_EQ(s, mtms.machineSerialNumber());
}
