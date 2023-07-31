/**
 * @file
 *
 * @brief Timer implementation based on OS calls and CPU ticks.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "rztimer.h"
#include "rzsleep.h"
#include <cstring>

#ifdef MATCH_ABI
#include <windows.h>
#endif

#if defined _MSC_VER
extern "C" uint64_t __rdtsc();
#pragma intrinsic(__rdtsc)
#endif

// A bunch of helper functions and globals.
namespace
{
uint64_t CPUTicks()
{
#if defined _MSC_VER
    return __rdtsc();
#elif defined __i386__
    int64_t ret;
    __asm__ volatile("rdtsc" : "=A"(ret));
    return ret;
#elif defined(__x86_64__) || defined(__amd64__)
    uint64_t low, high;
    __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
    return (high << 32) | low;
#elif defined(__aarch64__)
    int64_t ret;
    asm volatile("mrs %0, cntvct_el0" : "=r"(ret));
    return ret;
#elif defined __TARGET_ARCH_ARM && __TARGET_ARCH_ARM >= 6
    uint32_t pmccntr;
    uint32_t pmuseren;
    uint32_t pmcntenset;
    asm volatile("mrc p15, 0, %0, c9, c14, 0" : "=r"(pmuseren));
    if (pmuseren & 1) {
        asm volatile("mrc p15, 0, %0, c9, c12, 1" : "=r"(pmcntenset));
        if (pmcntenset & 0x80000000ul) {
            asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(pmccntr));
            return static_cast<int64_t>(pmccntr) * 64;
        }
    }
#elif defined(__powerpc__) || defined(__ppc__)
    // This returns a time-base, which is not always precisely a cycle-count.
    int64_t tbl, tbu0, tbu1;
    asm("mftbu %0" : "=r"(tbu0));
    asm("mftb  %0" : "=r"(tbl));
    asm("mftbu %0" : "=r"(tbu1));
    tbl &= -static_cast<int64>(tbu0 == tbu1);
    // high 32 bits in tbu1; low 32 bits in tbl  (tbu0 is garbage)
    return (tbu1 << 32) | tbl;
#else
#pragma message "CPUTicks not implemented for your architecture / compiler combination"
#define RZTIMER_NOCPUTICKS 1
    return 0;
#endif
}
#ifdef MATCH_ABI
static_assert(alignof(LARGE_INTEGER) == alignof(uint64_t), "Type alignment for casting not as expected");

uint64_t CalcLatency()
{
    uint64_t latency = UINT64_MAX;

    for (int i = 0; i < 8; ++i) {
        LARGE_INTEGER sample1;
        LARGE_INTEGER sample2;
        QueryPerformanceCounter(&sample1);
        QueryPerformanceCounter(&sample2);
        if (latency > uint64_t(sample2.QuadPart - sample1.QuadPart)) {
            latency = sample2.QuadPart - sample1.QuadPart;
        }
    }

    return latency;
}

float CalcFrequency()
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    return 1.0f / frequency.QuadPart;
}

const float kSecondPerTick = CalcFrequency();
const float kMillisecPerTick = kSecondPerTick * 1.000f;
const float kMicrosecPerTick = kSecondPerTick * 1.000000f;
const float kNanosecPerTick = kSecondPerTick * 1.000000000f;
const float kMinutePerTick = kSecondPerTick * (1.0f / 60.0f);
const float kTicksPerNanosec = 1.0 / kNanosecPerTick;
const float kTicksPerMicrosec = 1.0 / kMicrosecPerTick;
const float kTicksPerMillisec = 1.0 / kMillisecPerTick;
const float kTicksPerSecond = 1.0 / kSecondPerTick;
const float kTicksPerMinute = 1.0 / kMinutePerTick;
#else
uint64_t TimeSpecToNanosec(struct timespec *ts)
{
    return (uint64_t)ts->tv_sec * 1000000000 + ts->tv_nsec;
}

uint64_t CalcLatency()
{
    uint64_t latency = UINT64_MAX;

    for (int i = 0; i < 8; ++i) {
        struct timespec sample1;
        struct timespec sample2;
        clock_gettime(CLOCK_MONOTONIC, &sample1);
        clock_gettime(CLOCK_MONOTONIC, &sample2);
        if (latency > TimeSpecToNanosec(&sample2) - TimeSpecToNanosec(&sample1)) {
            latency = TimeSpecToNanosec(&sample2) - TimeSpecToNanosec(&sample1);
        }
    }

    return latency;
}

constexpr double kSecondPerTick = 0.000000001;
constexpr double kMillisecPerTick = 0.000001;
constexpr double kMicrosecPerTick = 0.001;
constexpr double kNanosecPerTick = 1.0;
constexpr double kMinutePerTick = kSecondPerTick * (1.0 / 60.0);
constexpr double kTicksPerNanosec = 1.0 / kNanosecPerTick;
constexpr double kTicksPerMicrosec = 1.0 / kMicrosecPerTick;
constexpr double kTicksPerMillisec = 1.0 / kMillisecPerTick;
constexpr double kTicksPerSecond = 1.0 / kSecondPerTick;
constexpr double kTicksPerMinute = 1.0 / kMinutePerTick;
#endif
const uint64_t gnQPCLatency = CalcLatency();
} // namespace

cRZTimer::cRZTimer(Units units)
{
    SetUnits(units);
}

void cRZTimer::Reset()
{
    std::memset(&mnElapsedTime, 0, sizeof(mnElapsedTime));
    std::memset(&mnStartTime, 0, sizeof(mnStartTime));
}

void cRZTimer::Restart()
{
    Start();
    std::memset(&mnElapsedTime, 0, sizeof(mnElapsedTime));
}

void cRZTimer::Start()
{
    if (mUnits == kProcessorTicks) {
        mnStartTime = CPUTicks();
    } else {
#ifdef MATCH_ABI
        QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&mnStartTime));
#else
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        mnStartTime = TimeSpecToNanosec(&ts);
#endif
    }
}

void cRZTimer::Stop()
{
    // TODO, Why doesn't this sample the CPUTicks when using that timer mode?
    if (IsTimerRunning()) {
#ifdef MATCH_ABI
        LARGE_INTEGER pc;
        QueryPerformanceCounter(&pc);
        uint64_t elapsed_time = pc.QuadPart - mnStartTime;
#else
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        uint64_t elapsed_time = TimeSpecToNanosec(&ts) - mnStartTime;
#endif
        if (elapsed_time <= gnQPCLatency) {
            ++mnElapsedTime;
        } else {
            mnElapsedTime += elapsed_time - gnQPCLatency;
        }

        mnStartTime = 0;
    }
}

void cRZTimer::Delay(uint32_t delay)
{
    cRZTimer delay_timer(mUnits);
    delay_timer.Start();

    while (delay_timer.GetElapsedTime() < delay) {
        RZSleep(0);
    }
}

void cRZTimer::SetUnits(Units units)
{
    mUnits = units;
    Reset();
}

void cRZTimer::GetElapsedTime(double &time)
{
    uint64_t ret = mnElapsedTime;
    if (IsTimerRunning()) {
        if (mUnits == kProcessorTicks) {
            ret += CPUTicks() - mnStartTime;
        } else {
#ifdef MATCH_ABI
            LARGE_INTEGER pc;
            QueryPerformanceCounter(&pc);
            uint64_t elapsed_time = pc.QuadPart - mnStartTime;
#else
            struct timespec ts;
            clock_gettime(CLOCK_MONOTONIC, &ts);
            uint64_t elapsed_time = TimeSpecToNanosec(&ts) - mnStartTime;
#endif
            if (elapsed_time <= gnQPCLatency) {
                ++ret;
            } else {
                ret += elapsed_time - gnQPCLatency;
            }
        }
    }

    switch (mUnits) {
        case kProcessorTicks:
        case kHighResolutionTimer: // Fallthrough
            time = ret;
            break;
        case kNanoSeconds:
            time = double(ret) * kNanosecPerTick;
            break;
        case kMicroseconds:
            time = double(ret) * kMicrosecPerTick;
            break;
        case kMilliseconds:
            time = double(ret) * kMillisecPerTick;
            break;
        case kSeconds:
            time = double(ret) * kSecondPerTick;
            break;
        case kMinutes:
            time = double(ret) * kMinutePerTick;
            break;
        default:
            time = 0.0;
            break;
    }
}

void cRZTimer::GetElapsedTime(float &time)
{
    double ret;
    GetElapsedTime(ret);
    time = float(ret);
}

void cRZTimer::GetElapsedTime(uint64_t &time)
{
    double ret;
    GetElapsedTime(ret);
    time = uint64_t(ret);
}

uint64_t cRZTimer::GetElapsedTime()
{
    uint64_t ret;
    GetElapsedTime(ret);
    return ret;
}

void cRZTimer::SetElapsedTime(uint64_t time)
{
    switch (mUnits) {
        case kProcessorTicks:
        case kHighResolutionTimer: // Fallthrough
            mnElapsedTime = time;
            break;
        case kNanoSeconds:
            mnElapsedTime = int64_t(time) * kTicksPerNanosec;
            break;
        case kMicroseconds:
            mnElapsedTime = int64_t(time) * kTicksPerMicrosec;
            break;
        case kMilliseconds:
            mnElapsedTime = int64_t(time) * kTicksPerMillisec;
            break;
        case kSeconds:
            mnElapsedTime = int64_t(time) * kTicksPerSecond;
            break;
        case kMinutes:
            mnElapsedTime = int64_t(time) * kTicksPerMinute;
            break;
        default:
            break;
    }

    Start();
}

bool cRZTimer::IsTimerRunning()
{
    return mnStartTime != 0;
}
