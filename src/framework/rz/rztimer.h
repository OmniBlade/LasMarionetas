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
#pragma once

#include <stdint.h>
#include <time.h>

class cRZTimer
{
public:
    enum Units
    {
        kProcessorTicks,
        kHighResolutionTimer,
        kNanoSeconds,
        kMicroseconds,
        kMilliseconds,
        kSeconds,
        kMinutes,
    };

public:
    cRZTimer(Units units = kMilliseconds);

    /**
     * @brief Resets the timer to 0 and stops it.
     */
    void Reset();
    /**
     * @brief Retarts the timer after being stopped.
     */
    void Restart();
    /**
     * @brief Starts the timer.
     */
    void Start();
    /**
     * @brief Stops the timer.
     */
    void Stop();
    /**
     * @brief Yields the thread for a specified time.
     * @param delay The delay in the current timer precision to yield.
     * @warning Actual delay depends on the operating system scheduler and may be much longer at high precision.
     */
    void Delay(uint32_t delay);
    /**
     * @return The current precision setting for the timer.
     */
    Units GetUnits() { return mUnits; }
    /**
     * @brief Set the precision for the timer.
     * @param units Enum constant to specify the precision the timer should use.
     * @warning Timer may not actually tick with the granularity of the requested precision.
     */
    void SetUnits(Units units);
    /**
     * @brief Get the elapsed time at the timer precision.
     * @param time Variable reference to recieve the elapsed time.
     */
    void GetElapsedTime(double &time);
    /**
     * @brief Get the elapsed time at the timer precision.
     * @param time Variable reference to recieve the elapsed time.
     */
    void GetElapsedTime(float &time);
    /**
     * @brief Get the elapsed time at the timer precision.
     * @param time Variable reference to recieve the elapsed time.
     */
    void GetElapsedTime(uint64_t &time);
    /**
     * @brief Get the elapsed time.
     * @return The elapsed time at the timer precision.
     */
    uint64_t GetElapsedTime();
    /**
     * @brief Set the elapsed time at the timer precision.
     * @param time Time to set the timer to have elapsed.
     */
    void SetElapsedTime(uint64_t time);
    /**
     * @return Is the timer currently running?
     */
    bool IsTimerRunning();

private:
    Units mUnits;
    uint64_t mnStartTime;
    uint64_t mnElapsedTime;
};
