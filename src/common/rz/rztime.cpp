/**
 * @file
 *
 * @brief Class for handling time of day information.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "rztime.h"
#include "rzdate.h"
#include <ctime>

// Handle posix variable naming difference in msvc.
#ifndef _daylight
#define _daylight daylight
#endif

#ifndef _timezone
#define _timezone timezone
#endif

cRZDate cRZTime::sRefDate(0, 0);

cRZTime::cRZTime()
{
    time_t t;
    std::time(&t);
    struct tm *ts = std::localtime(&t);
    mnTime = cRZTime(cRZDate(ts->tm_mon + 1, ts->tm_mday, ts->tm_year + 1900), ts->tm_hour, ts->tm_min, ts->tm_sec).mnTime;
}

cRZTime::cRZTime(const cRZDate &date, uint32_t hour, uint32_t minute, uint32_t day) {}

uint32_t cRZTime::Hour()
{
    return LocalSecs() % kSecondsPerDay / kSecondsPerHour;
}

uint32_t cRZTime::Minute()
{
    return LocalSecs() % kSecondsPerHour / kSecondsPerMinute;
}

bool cRZTime::IsDST()
{
    if (!_daylight) {
        return false;
    }

    uint32_t year = cRZDate(mnTime / kSecondsPerDay, 0).Year();

    return mnTime >= BeginDST(year).mnTime && mnTime < EndDST(year).mnTime;
}

cRZTime cRZTime::BuildLocal(const cRZDate &date, uint32_t offset)
{
    return cRZTime(_timezone + kSecondsPerHour * (offset + kHoursPerDay * (date.mnJulianDate - sRefDate.mnJulianDate)));
}

cRZTime cRZTime::BeginDST(uint32_t year)
{
    switch (year) {
        case 1974:
            return BuildLocal(cRZDate(1, 6, 1974), 2);
        case 1975:
            return BuildLocal(cRZDate(2, 23, 1975), 2);
        default:
            return BuildLocal(cRZDate(4, 8, year).Previous(7), 2);
    }
}

cRZTime cRZTime::EndDST(uint32_t year)
{
    return BuildLocal(cRZDate(10, 31, year).Previous(7), 1);
}

uint32_t cRZTime::LocalSecs()
{
    uint32_t local_time = mnTime - _timezone;

    if (cRZTime(local_time).IsDST()) {
        local_time += kSecondsPerHour;
    }

    return local_time;
}
