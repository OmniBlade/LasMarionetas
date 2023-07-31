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
#pragma once

#include <stddef.h>
#include <stdint.h>

namespace Allocator
{
#ifdef _WIN32
static constexpr size_t kMutexBufferSize = 7;
#else // Assumes pthreads for now.
static constexpr size_t kMutexBufferSize = 6 * ((sizeof(void*)) / 4);
#endif

void *PPMMutexCreate(void *data);
void PPMMutexDestroy(void *mutex);
void PPMMutexLock(void *mutex);
void PPMMutexUnlock(void *mutex);
int32_t PPMMutexGetLockCount(const void *mutex);

class PPMAutoMutex
{
public:
    PPMAutoMutex(void *mutex) : mpMutex(mutex)
    {
        if (mpMutex != nullptr) {
            PPMMutexLock(mpMutex);
        }
    }
    ~PPMAutoMutex()
    {
        if (mpMutex != nullptr) {
            PPMMutexUnlock(mpMutex);
        }
    }

private:
    void *mpMutex;
};
} // namespace Allocator
