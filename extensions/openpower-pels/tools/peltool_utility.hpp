#pragma once

#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream
#include "../bcd_time.hpp"

using namespace std; 
namespace openpower
{
namespace pels
{
int  bcdToDec(uint8_t val);
int decToBcd(uint8_t val);
int hexCharToInt(char a);
string hexToString(string str);
string parseTimeStamp(const BCDTime& timeStamp);
string printLine();
string printLineBreak();
}
}
