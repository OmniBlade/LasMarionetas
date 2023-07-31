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
#include <windows.h>

namespace
{
struct PPMMutexInternal
{
    CRITICAL_SECTION mCS;
    int32_t mnLockCount;
};

static_assert(Allocator::kMutexBufferSize >= sizeof(PPMMutexInternal) / 4, "kMutexBufferSize will not be sufficient.");
}

namespace Allocator
{
void *PPMMutexCreate(void *data)
{
    static_cast<PPMMutexInternal *>(data)->mnLockCount = 0;
    InitializeCriticalSection(&static_cast<PPMMutexInternal *>(data)->mCS);
    return data;
}
void PPMMutexDestroy(void *mutex)
{
    DeleteCriticalSection(&static_cast<PPMMutexInternal *>(mutex)->mCS);
}
void PPMMutexLock(void *mutex)
{
    EnterCriticalSection(&static_cast<PPMMutexInternal *>(mutex)->mCS);
    ++static_cast<PPMMutexInternal *>(mutex)->mnLockCount;
}
void PPMMutexUnlock(void *mutex)
{
    --static_cast<PPMMutexInternal *>(mutex)->mnLockCount;
    LeaveCriticalSection(&static_cast<PPMMutexInternal *>(mutex)->mCS);
}
int32_t PPMMutexGetLockCount(const void *mutex)
{
    return static_cast<const PPMMutexInternal *>(mutex)->mnLockCount;
}
} // namespace Allocator
