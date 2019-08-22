#include "extensions/openpower-pels/private_header.hpp"
#include "extensions/openpower-pels/tools/peltool.hpp"
#include "extensions/openpower-pels/tools/peltool_utility.hpp"
#include "extensions/openpower-pels/user_header.hpp"
#include "pel_utils.hpp"

#include <iostream> // std::cout
#include <sstream>  // std::stringstream
#include <string>   // std::string

#include <gtest/gtest.h>
using namespace std;
using namespace openpower::pels;

TEST(PelToolTest, UserHeaderIdParseTest)
{
    auto data = pelDataFactory(TestPelType::userHeaderSimple);

    std::cout << "AAtir testing: " << '\n';
    Stream stream(*data);
    UserHeader uh(stream);
    std::stringstream ss;
    ss << std::hex << uh.header().id;
    // ss.flags(std::ios::hex);
    std::string foo;
    ss >> foo;
    std::cout << "foo: " << foo << '\n';
    foo = hexToString(foo);
    ss << foo;
    std::cout << "foo: " << foo << '\n';
    EXPECT_EQ(uh.valid(), true);
    EXPECT_EQ(foo, "UH");
}
TEST(PelToolTest, PrivateHeaderCreateTimeStampParseTest)
{
    auto data = pelDataFactory(TestPelType::privateHeaderSimple);
    string n;
    std::cout << "AAtir testing private header timestamp: " << '\n';
    Stream stream(*data);
    PrivateHeader ph(stream);
    std::stringstream ss;
    BCDTime foo;
    std::string pts;
    foo = ph.createTimestamp();
    std::string wholeTimestamp;
    wholeTimestamp = parseTimeStamp(foo);
    // std::cout << "pts: " << pts << '\n';
    std::string _str, year, month, day, hour, minutes, seconds, hundredths;
    // std::stringstream ss;
    // year
    // int _intForDate;
    int yrindec = bcdToDec(foo.yearMSB);
    int yrlsbindec = bcdToDec(foo.yearLSB);
    EXPECT_EQ(yrlsbindec, 30);
    EXPECT_EQ(yrindec, 20);
    // ss << std::hex << yrindec;
    // ss >> _str;
    //_str = hexToString(_str);
    // std::cout << "year: in hex " << _str << '\n';
    //_str = hexToString(_str);
    // ss << std::hex << foo.yearMSB;
    // ss >> _intForDate;
    // ss << year;
    // ss >> _intForDate;
    std::cout << "whole time stamp" << wholeTimestamp << '\n';
    std::cout << "year: converted to decimal " << yrindec << '\n';
    std::cout << "year: converted to decimal " << yrlsbindec << '\n';
}
