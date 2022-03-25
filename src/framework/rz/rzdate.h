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
#pragma once

#include <stdint.h>

class cRZString;

class cRZDate
{
    friend class cRZTime;

public:
    cRZDate();
    cRZDate(uint32_t month, uint32_t day, uint32_t year);
    cRZDate(uint32_t day, uint32_t year);
    cRZDate(const cRZDate &that) : mnJulianDate(that.mnJulianDate) {}
    cRZDate(uint32_t jday) : mnJulianDate(jday) {}

    /**
     * @brief Formats a day into a name string.
     * @param str String to hold the formatted data.
     * @param day Day to get the name for.
     */
    virtual void NameOfDayString(cRZString &str, uint32_t day) {}
    /**
     * @brief Formats a month into a name string.
     * @param str String to hold the formatted data.
     * @param month Month to get the name for.
     */
    virtual void NameOfMonthString(cRZString &str, uint32_t month) {}
    /**
     * @brief Formats the date into a string.
     * @param str String to hold the formatted data.
     * @param format Formatting constant to set the string format to use.
     */
    virtual void DateString(cRZString &str, uint32_t format) const;

    /**
     * @return The year as an integer.
     */
    uint32_t Year() const;
    /**
     * @return The month and an integer.
     */
    uint32_t Month() const;
    /**
     * @return The day of the month.
     */
    uint32_t DayOfMonth() const;
    /**
     * @return The day of the week as an integer.
     */
    uint32_t DayOfWeek() const;
    /**
     * @brief Caculates a previous date.
     * @param days How many dates previous to calculate.
     * @return Calculated date.
     */
    cRZDate Previous(uint32_t days) const;

private:
    void Mdy(uint32_t &month, uint32_t &day, uint32_t &year) const;

    static bool IsValidMonth(uint32_t month);
    static bool IsLeapYear(uint32_t year);
    static bool DayWithinMonth(uint32_t month, uint32_t day, uint32_t year);
    static uint32_t Jday(uint32_t month, uint32_t day, uint32_t year);

private:
    uint32_t mnJulianDate;
};
