/**
 * Copyright © 2019 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "bcd_time.hpp"

#include <fmt/format.h>
#include <time.h>

#include <phosphor-logging/log.hpp>

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

BCDTime getBCDTime(uint64_t epochMS)
{
    std::chrono::milliseconds ms{epochMS};
    std::chrono::time_point<std::chrono::system_clock> time{ms};

    return getBCDTime(time);
}

uint64_t getMillisecondsSinceEpoch(const BCDTime& bcdTime)
{
    // Convert a UTC tm struct to a UTC time_t struct to a timepoint.
    int year = (fromBCD(bcdTime.yearMSB) * 100) + fromBCD(bcdTime.yearLSB);
    tm utcTime;
    utcTime.tm_year = year - 1900;
    utcTime.tm_mon = fromBCD(bcdTime.month) - 1;
    utcTime.tm_mday = fromBCD(bcdTime.day);
    utcTime.tm_hour = fromBCD(bcdTime.hour);
    utcTime.tm_min = fromBCD(bcdTime.minutes);
    utcTime.tm_sec = fromBCD(bcdTime.seconds);
    utcTime.tm_isdst = 0;

    time_t t = timegm(&utcTime);
    auto timepoint = std::chrono::system_clock::from_time_t(t);
    int milliseconds = fromBCD(bcdTime.hundredths) * 10;
    timepoint += std::chrono::milliseconds(milliseconds);

    return std::chrono::duration_cast<std::chrono::milliseconds>(
               timepoint.time_since_epoch())
        .count();
}

Stream& operator>>(Stream& s, BCDTime& time)
{
    s >> time.yearMSB >> time.yearLSB >> time.month >> time.day >> time.hour;
    s >> time.minutes >> time.seconds >> time.hundredths;
    return s;
}

Stream& operator<<(Stream& s, const BCDTime& time)
{
    s << time.yearMSB << time.yearLSB << time.month << time.day << time.hour;
    s << time.minutes << time.seconds << time.hundredths;
    return s;
}

} // namespace pels
} // namespace openpower
