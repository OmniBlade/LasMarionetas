/**
 * @file
 *
 * @brief An implementation for critical section objects.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once
#include "igzcriticalsection.h"
#include <new>

/**
 * @brief Scoped lock for cIGZCriticalSection objects.
 */
class cRZLock
{
public:
    cRZLock(cIGZCriticalSection &cs) : mCS(cs) { mCS.Lock(); }
    virtual ~cRZLock() { mCS.Unlock(); }

private:
    cIGZCriticalSection &mCS;
};

class cRZCriticalSection : public cIGZCriticalSection
{
    struct CritSecImpl;

public:
    cRZCriticalSection();
    virtual ~cRZCriticalSection();

    /**
     * @brief Releases the object.
     * @return Always 0.
     */
    virtual int32_t Release() override;
    /**
     * @brief Enter the critical section, will block.
     * @return Number of times the object has been locked.
     */
    virtual int32_t Lock() override;
    /**
     * @brief Leave the critical section.
     * @return Zero if last lock was removed, remaining lock count otherwise.
     */
    virtual int32_t Unlock() override;
    /**
     * @brief Enter the critical section, will not block.
     * @return Number of times the object has been locked, increases on successful lock.
     */
    virtual int32_t TryLock() override;
    /**
     * @brief Check if the object is in a valid state.
     * @return None zero if the object is valid.
     */
    virtual int32_t IsValid() override;
    /**
     * @brief Check if the object is locked.
     * @return Number of remaining locks, zero if unlocked.
     */
    virtual int32_t IsLocked() override;

    /**
     * Used to hook the constructor when injecting as a dll.
     */
    cRZCriticalSection *Hookable_Ctor() { return new (this) cRZCriticalSection(); }

private:
    CritSecImpl *mCriticalSection;
};
