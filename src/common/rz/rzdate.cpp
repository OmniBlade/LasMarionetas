/**
 * @file
 *
 * @brief Class for handling date information.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "rzdate.h"
#include "rzstring.h"
#include <ctime>
#include <inttypes.h>

cRZDate::cRZDate()
{
    time_t now = std::time(nullptr);
    struct tm *tmp = std::localtime(&now);

    mnJulianDate = Jday(tmp->tm_mon + 1, tmp->tm_mday, tmp->tm_year + 1900);
}

cRZDate::cRZDate(uint32_t month, uint32_t day, uint32_t year)
{
    mnJulianDate = Jday(month, day, year);
}

cRZDate::cRZDate(uint32_t day, uint32_t year)
{
    if (year != 0) {
        mnJulianDate = day + Jday(12, 31, year - 1);
    } else {
        mnJulianDate = day + 2415386;
    }
}

void cRZDate::DateString(cRZString &str, uint32_t format) const
{
    switch (format) {
        case 0:
        case 1:
        case UINT32_MAX:
            str.Sprintf("%" PRIu32 "/%" PRIu32 "/%4" PRIu32, Month(), DayOfMonth(), Year());
            break;
        case 3:
        case 4:
        case 10:
        case 11:
            str.Sprintf("%" PRIu32 ".%" PRIu32 ".%4" PRIu32, DayOfMonth(), Month(), Year());
            break;
        case 7:
        case 8:
        case 14:
            str.Sprintf("%" PRIu32 "-%" PRIu32 "-%4" PRIu32, DayOfMonth(), Month(), Year());
            break;
        case 9:
            str.Sprintf("%4" PRIu32 ".%" PRIu32 ".%" PRIu32, Year(), Month(), DayOfMonth());
            break;
        case 15:
            str.Sprintf("%4" PRIu32 "/%" PRIu32 "/%" PRIu32, Year(), Month(), DayOfMonth());
            break;
        default:
            str.Sprintf("%" PRIu32 "/%" PRIu32 "/%4" PRIu32, DayOfMonth(), Month(), Year());
            break;
    };
}

uint32_t cRZDate::Year() const
{
    uint32_t month;
    uint32_t day;
    uint32_t year;

    Mdy(month, day, year);

    return year;
}

uint32_t cRZDate::Month() const
{
    uint32_t month;
    uint32_t day;
    uint32_t year;

    Mdy(month, day, year);

    return month;
}

uint32_t cRZDate::DayOfMonth() const
{
    uint32_t month;
    uint32_t day;
    uint32_t year;

    Mdy(month, day, year);

    return day;
}

uint32_t cRZDate::DayOfWeek() const
{
    return ((mnJulianDate + 1) % 7u + 6) % 7 + 1;
}

cRZDate cRZDate::Previous(uint32_t days) const
{
    uint32_t day_of_week = DayOfWeek() - 1;
    uint32_t d = days - 1;
    uint32_t offset;

    if (d <= day_of_week) {
        offset = day_of_week - d;
    } else {
        offset = 7 - d + day_of_week;
    }

    return cRZDate(mnJulianDate - offset);
}

void cRZDate::Mdy(uint32_t &month, uint32_t &day, uint32_t &year) const
{
    uint32_t a = 4 * (mnJulianDate - 1721119);
    uint32_t b = (a - 1) / 146097u;
    uint32_t c = (a - 146097 * b - 1) & 0xFFFFFFFC;
    uint32_t d = (c + 3) / 1461;
    uint32_t e = 5 * ((c - 1461 * d + 7) >> 2);
    uint32_t f = (e - 3) / 153;

    month = f;
    day = (e - 153 * f + 2) / 5;
    year = d + 100 * b;

    if (month > 9) {
        month -= 9;
        ++year;
    } else {
        month += 3;
    }
}

bool cRZDate::IsValidMonth(uint32_t month)
{
    if (month == 0 || month > 12) {
        return false;
    }

    return true;
}

bool cRZDate::IsLeapYear(uint32_t year)
{
    return ((year % 4) == 0 && (year % 100) != 0) || (year % 400) == 0;
}

bool cRZDate::DayWithinMonth(uint32_t month, uint32_t day, uint32_t year)
{
    static const uint8_t _days_in_month[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if (day == 0 || !IsValidMonth(month)) {
        return false;
    }

    uint32_t days = _days_in_month[month];

    if (IsLeapYear(year) && month == 2) {
        ++days;
    }

    return days >= day;
}

uint32_t cRZDate::Jday(uint32_t month, uint32_t day, uint32_t year)
{
    if (!DayWithinMonth(month, day, year)) {
        return 0;
    }

    uint32_t adj_year = year;
    uint32_t adj_month;

    if (month <= 2) {
        adj_month = month + 9;
        adj_year = year - 1;
    } else {
        adj_month = month - 3;
    }

    return day + (153 * adj_month + 2) / 5 + ((1461 * (adj_year % 100)) >> 2) + ((146097 * (adj_year / 100)) >> 2) + 1721119;
}
