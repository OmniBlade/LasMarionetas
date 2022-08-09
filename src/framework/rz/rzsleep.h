/**
 * @file
 *
 * @brief Implements a millisecond sleep function with Win32 api behaviour.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once

#ifdef _WIN32
#include <windows.h>
#else /* Assuming recent posix*/
#ifndef __USE_POSIX199309
#define __USE_POSIX199309
#endif
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif
#include <time.h>
#include <sched.h>
#endif

/**
 * Yield the current thread for at least ms milliseconds.
 */
inline void RZSleep(unsigned ms)
{
#ifdef _WIN32
    Sleep(ms);
#else
    if (ms == 0) {
        sched_yield();
    } else {
        struct timespec ts;
        ts.tv_sec = ms / 1000;
        ts.tv_nsec = (ms % 1000) * 1000000;
        nanosleep(&ts, nullptr);
    }
#endif
}
