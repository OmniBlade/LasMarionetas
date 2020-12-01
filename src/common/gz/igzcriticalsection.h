/**
 * @file
 *
 * @brief An interface for critical section objects.
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

class cIGZCriticalSection
{
public:
    /**
     * @brief Releases the object.
     * @return Always 0.
     */
    virtual int32_t Release() = 0;
    /**
     * @brief Enter the critical section, will block.
     * @return None zero if successful.
     */
    virtual int32_t Lock() = 0;
    /**
     * @brief Leave the critical section.
     * @return Zero if last lock was removed, none zero otherwise.
     */
    virtual int32_t Unlock() = 0;
    /**
     * @brief Enter the critical section, will not block.
     * @return None zero if critical section was successfully entered.
     */
    virtual int32_t TryLock() = 0;
    /**
     * @brief Check if the object is in a valid state.
     * @return None zero if the object is valid.
     */
    virtual int32_t IsValid() = 0;
    /**
     * @brief Check if the object is locked.
     * @return None zero if the object is locked.
     */
    virtual int32_t IsLocked() = 0;
};
