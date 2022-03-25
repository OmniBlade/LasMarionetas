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
#include "rzcriticalsection.h"
#include <stdlib.h>
#include <string.h>
#include <windows.h>

struct cRZCriticalSection::CritSecImpl
{
    CRITICAL_SECTION mutex;
    int32_t lock_count;
};

cRZCriticalSection::cRZCriticalSection() : mCriticalSection(nullptr)
{
    CritSecImpl *cs = (CritSecImpl *)malloc(sizeof(CritSecImpl));

    cs->lock_count = 0;
    memset(&cs->mutex, 0, sizeof(cs->mutex));
    mCriticalSection = cs;
    InitializeCriticalSection(&mCriticalSection->mutex);
}

cRZCriticalSection::~cRZCriticalSection()
{
    DeleteCriticalSection(&mCriticalSection->mutex);
    free(mCriticalSection);
    mCriticalSection = nullptr;
}

int32_t cRZCriticalSection::Release()
{
    delete this;
    return 0;
}

int32_t cRZCriticalSection::Lock()
{
    EnterCriticalSection(&mCriticalSection->mutex);
    ++mCriticalSection->lock_count;

    return mCriticalSection->lock_count;
}

int32_t cRZCriticalSection::Unlock()
{
    --mCriticalSection->lock_count;
    LeaveCriticalSection(&mCriticalSection->mutex);

    return mCriticalSection->lock_count;
}

int32_t cRZCriticalSection::TryLock()
{
    bool locked = TryEnterCriticalSection(&mCriticalSection->mutex);

    if (locked) {
        ++mCriticalSection->lock_count;
        return mCriticalSection->lock_count;
    }

    return 0;
}

int32_t cRZCriticalSection::IsValid()
{
    return 1;
}

int32_t cRZCriticalSection::IsLocked()
{
    return mCriticalSection->lock_count;
}
