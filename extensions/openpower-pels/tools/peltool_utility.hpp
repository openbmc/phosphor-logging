#pragma once

#include "../bcd_time.hpp"

#include <iostream> // std::cout
#include <sstream>  // std::stringstream
#include <string>   // std::string

using namespace std;
namespace openpower
{
namespace pels
{
int bcdToDec(uint8_t val);
int decToBcd(uint8_t val);
int hexCharToInt(char a);
string hexToString(string str);
string parseTimeStamp(const BCDTime& timeStamp);
string printLine();
string printLineBreak();
} // namespace pels
} // namespace openpower
