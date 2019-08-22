#include "peltool_utility.hpp"
namespace openpower
{
namespace pels
{

int  bcdToDec(uint8_t val)
{
  return ( (val/16*10) + (val%16) );
}

int decToBcd(uint8_t val)
{
  return ( (val/10*16) + (val%10) );
}

int hexCharToInt(char a){
    if(a>='0' && a<='9')
        return(a-48);
    else if(a>='A' && a<='Z')
        return(a-55);
    else
        return(a-87);
}

string hexToString(string str){
    std::stringstream HexString;
    for(long unsigned int i=0;i<str.length();i++){
        char a = str.at(i++);
        char b = str.at(i);
        int x = hexCharToInt(a);
        int y = hexCharToInt(b);
        HexString << (char)((16*x)+y);
    }
    return HexString.str();
}

string parseTimeStamp(const BCDTime& timeStamp)
{
    std::string _str;
    std::stringstream ss;
    ss << to_string(openpower::pels::bcdToDec(timeStamp.yearMSB)) << to_string(openpower::pels::bcdToDec(timeStamp.yearLSB))<< "/" 
<< to_string(bcdToDec(timeStamp.month)) << "/" << to_string(bcdToDec(timeStamp.day)) << " " << to_string(bcdToDec(timeStamp.hour)) << ":" << to_string(bcdToDec(timeStamp.minutes)) << ":" << to_string(bcdToDec(timeStamp.seconds)) << ":" << to_string(bcdToDec(timeStamp.hundredths));
     _str=ss.str();
    return _str;
}

string printLine()
{ 
   return "|------------------------------------------------------------------------------|";
}

string printLineBreak()
{
   return "\n";
}


}
}
