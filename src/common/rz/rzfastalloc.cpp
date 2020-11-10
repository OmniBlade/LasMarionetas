/**
 * @file
 *
 * @brief Custom "fast" memory allocators.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "rzfastalloc.h"
#include <cstdlib>

void *RZFastAlloc(size_t nCount)
{
    return std::malloc(nCount);
}

void RZFastFree(void *pvMem)
{
    std::free(pvMem);
}

void *RZFastRealloc(void *pvMem, size_t nCount)
{
    return std::realloc(pvMem, nCount);
}
