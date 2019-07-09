#include "bcd_time.hpp"

namespace openpower
{
namespace pels
{

bool BCDTime::operator==(const BCDTime& right) const
{
    return (yearMSB == right.yearMSB) && (yearLSB == right.yearLSB) &&
           (month == right.month) && (day == right.day) &&
           (hour == right.hour) && (minutes == right.minutes) &&
           (seconds == right.seconds) && (hundredths == right.hundredths);
}

bool BCDTime::operator!=(const BCDTime& right) const
{
    return !(*this == right);
}

BCDTime getBCDTime(std::chrono::time_point<std::chrono::system_clock>& time)
{
    BCDTime bcd;

    using namespace std::chrono;
    time_t t = system_clock::to_time_t(time);
    tm* localTime = localtime(&t);
    assert(localTime != nullptr);

    int year = 1900 + localTime->tm_year;
    bcd.yearMSB = toBCD(year / 100);
    bcd.yearLSB = toBCD(year % 100);
    bcd.month = toBCD(localTime->tm_mon + 1);
    bcd.day = toBCD(localTime->tm_mday);
    bcd.hour = toBCD(localTime->tm_hour);
    bcd.minutes = toBCD(localTime->tm_min);
    bcd.seconds = toBCD(localTime->tm_sec);

    auto ms = duration_cast<milliseconds>(time.time_since_epoch()).count();
    int hundredths = (ms % 1000) / 10;
    bcd.hundredths = toBCD(hundredths);

    return bcd;
}

Stream& operator>>(Stream& s, BCDTime& time)
{
    s >> time.yearMSB >> time.yearLSB >> time.month >> time.day >> time.hour;
    s >> time.minutes >> time.seconds >> time.hundredths;
    return s;
}

Stream& operator<<(Stream& s, BCDTime& time)
{
    s << time.yearMSB << time.yearLSB << time.month << time.day << time.hour;
    s << time.minutes << time.seconds << time.hundredths;
    return s;
}

} // namespace pels
} // namespace openpower
