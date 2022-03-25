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
#include <cstdlib>
#include <cstring>
#include <pthread.h>

struct cRZCriticalSection::CritSecImpl
{
    pthread_mutex_t mutex;
    int32_t lock_count;
};

cRZCriticalSection::cRZCriticalSection() : mCriticalSection(nullptr)
{
    CritSecImpl *cs = (CritSecImpl *)std::malloc(sizeof(CritSecImpl));

    cs->lock_count = 0;
    std::memset(&cs->mutex, 0, sizeof(cs->mutex));
    mCriticalSection = cs;

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&cs->mutex, &attr);
    pthread_mutexattr_destroy(&attr);
}

cRZCriticalSection::~cRZCriticalSection()
{
    pthread_mutex_destroy(&mCriticalSection->mutex);
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
    if (pthread_mutex_lock(&mCriticalSection->mutex) == 0) {
        ++mCriticalSection->lock_count;
    }

    return mCriticalSection->lock_count;
}

int32_t cRZCriticalSection::Unlock()
{
    --mCriticalSection->lock_count;
    if (pthread_mutex_unlock(&mCriticalSection->mutex) == 0) {
        return mCriticalSection->lock_count;
    }

    return 0;
}

int32_t cRZCriticalSection::TryLock()
{
    if (pthread_mutex_trylock(&mCriticalSection->mutex) == 0) {
        ++mCriticalSection->lock_count;
    }

    return mCriticalSection->lock_count;
}

int32_t cRZCriticalSection::IsValid()
{
    return 1;
}

int32_t cRZCriticalSection::IsLocked()
{
    return mCriticalSection->lock_count;
}
