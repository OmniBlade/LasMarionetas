/**
 * @file
 *
 * @brief Atomic operations.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "rzthreadsafe.h"

#ifdef _MSC_VER
#include <windows.h>
#endif

void *RZThreadSafeExchange(void *volatile &val, void *xchg)
{
#if defined __clang__ || defined __GNUC__
    return __atomic_exchange_n(&val, xchg, __ATOMIC_SEQ_CST);
#elif defined _MSC_VER
    return InterlockedExchangePointer(&val, xchg);
#else
#pragma message "Falling back to unsafe exchange, implement compiler intrinsics for thread safe exchange here."
    void *tmp = val;
    *val = xchg;
    return val;
#endif
}

uint32_t RZThreadSafeAdd(volatile uint32_t &val, uint32_t inc)
{
#if defined __clang__ || defined __GNUC__
    return __atomic_add_fetch(&val, inc, __ATOMIC_SEQ_CST);
#elif defined _MSC_VER
    return inc + InterlockedExchangeAdd(&val, inc);
#else
#pragma message "Falling back to unsafe addition, implement compiler intrinsics for thread safe interger addition here."
    val += inc;
    return val;
#endif
}
