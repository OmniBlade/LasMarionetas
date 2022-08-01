/**
 * Based on gettimeofday.c from mingw-w64
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given.
 */
#include <sys/time.h>
#include <sys/timeb.h>
#include <time.h>
#include <windows.h>

#define FILETIME_1970 116444736000000000ull /* seconds between 1/1/1601 and 1/1/1970 */
#define HECTONANOSEC_PER_SEC 10000000ull

int getntptimeofday(struct timespec *tp, struct timezone *z)
{
    int res = 0;
    union
    {
        unsigned long long ns100; /*time since 1 Jan 1601 in 100ns units */
        FILETIME ft;
    } _now;
    TIME_ZONE_INFORMATION TimeZoneInformation;
    DWORD tzi;

    if (z != NULL) {
        if ((tzi = GetTimeZoneInformation(&TimeZoneInformation)) != TIME_ZONE_ID_INVALID) {
            z->tz_minuteswest = TimeZoneInformation.Bias;
            if (tzi == TIME_ZONE_ID_DAYLIGHT)
                z->tz_dsttime = 1;
            else
                z->tz_dsttime = 0;
        } else {
            z->tz_minuteswest = 0;
            z->tz_dsttime = 0;
        }
    }

    if (tp != NULL) {
        GetSystemTimeAsFileTime(&_now.ft); /* 100-nanoseconds since 1-1-1601 */
        /* The actual accuracy on XP seems to be 125,000 nanoseconds = 125 microseconds = 0.125 milliseconds */
        _now.ns100 -= FILETIME_1970; /* 100 nano-seconds since 1-1-1970 */
        tp->tv_sec = _now.ns100 / HECTONANOSEC_PER_SEC; /* seconds since 1-1-1970 */
        tp->tv_nsec = (long)(_now.ns100 % HECTONANOSEC_PER_SEC) * 100; /* nanoseconds */
    }
    return res;
}

int gettimeofday(struct timeval *tp, void *tzp)
{
    struct timespec t;

    if (getntptimeofday(&t, (struct timezone *)tzp))
        return -1;
    tp->tv_sec = t.tv_sec;
    tp->tv_usec = (t.tv_nsec / 1000);
    return 0;
}
