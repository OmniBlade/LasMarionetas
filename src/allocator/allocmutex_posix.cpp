/**
 * @file
 *
 * @brief Mutex for using the allocator in multithreaded environments.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "allocator/allocmutex.h"
#include <pthread.h>

namespace
{
struct PPMMutexInternal
{
    pthread_mutex_t mCS;
    int32_t mnLockCount;
};

static_assert(Allocator::kMutexBufferSize >= sizeof(PPMMutexInternal) / 4, "kMutexBufferSize will not be sufficient.");
}

namespace Allocator
{
void *PPMMutexCreate(void *data)
{
    static_cast<PPMMutexInternal *>(data)->mnLockCount = 0;
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&static_cast<PPMMutexInternal *>(data)->mCS, &attr);
    pthread_mutexattr_destroy(&attr);
    return data;
}

void PPMMutexDestroy(void *mutex)
{
    pthread_mutex_destroy(&static_cast<PPMMutexInternal *>(mutex)->mCS);
}

void PPMMutexLock(void *mutex)
{
    pthread_mutex_lock(&static_cast<PPMMutexInternal *>(mutex)->mCS);
    ++static_cast<PPMMutexInternal *>(mutex)->mnLockCount;
}

void PPMMutexUnlock(void *mutex)
{
    --static_cast<PPMMutexInternal *>(mutex)->mnLockCount;
    pthread_mutex_unlock(&static_cast<PPMMutexInternal *>(mutex)->mCS);
}

int32_t PPMMutexGetLockCount(const void *mutex)
{
    return static_cast<const PPMMutexInternal *>(mutex)->mnLockCount;
}
} // namespace Allocator
