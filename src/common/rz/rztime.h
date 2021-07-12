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
#pragma once

#include <stdint.h>
class cRZDate;

class cRZTime
{
    enum
    {
        kSecondsPerMinute = 60,
        kSecondsPerHour = 3600,
        kSecondsPerDay = 86400,
        kHoursPerDay = 24,
    };

public:
    cRZTime();
    cRZTime(const cRZDate &date, uint32_t hour, uint32_t minute, uint32_t day);
    cRZTime(uint32_t time) : mnTime(time) {}

    /**
     * @return The hour in the day. 
     */
    uint32_t Hour();
    /**
     * @return The minute in the hour.
     */
    uint32_t Minute();
    /**
     * @return The second in the minute.
     */
    uint32_t Seconds() { return mnTime % kSecondsPerMinute; }


private:
    bool IsDST();
    static cRZTime BuildLocal(const cRZDate &date, uint32_t offset);
    static cRZTime BeginDST(uint32_t year);
    static cRZTime EndDST(uint32_t year);
    uint32_t LocalSecs();

private:
    uint32_t mnTime;
    static cRZDate sRefDate;
};
