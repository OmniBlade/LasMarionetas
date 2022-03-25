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
#pragma once
#include <memory>
#include <new>
#include <stdint.h>
#include <type_traits>

void *RZFastAlloc(size_t nCount);
void RZFastFree(void *pvMem);
void *RZFastRealloc(void *pvMem, size_t nCount);

/**
 * @brief A STL allocator that uses the game's memory pool
 *
 * This is an allocator for STL containers like lists and sets which uses the
 * game's built-in memory allocator, macOS version appears to just use malloc
 * and friends for this.
 */
template<typename T> class cRZFastSTLAllocator : public std::allocator<T>
{
public:
    typedef T value_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef T &reference;
    typedef const T &const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    template<class U> struct rebind
    {
        typedef cRZFastSTLAllocator<U> other;
    };

public:
    cRZFastSTLAllocator() {}
    cRZFastSTLAllocator(const cRZFastSTLAllocator &) {}
    template<class U> cRZFastSTLAllocator(const cRZFastSTLAllocator<U> &) {}

    pointer allocate(size_type nCount) { return (pointer)(RZFastAlloc(sizeof(T) * nCount)); }
    pointer allocate(size_type nCount, void const *pHint) { return allocate(nCount); }
    void deallocate(pointer pElem, size_type nCount) { RZFastFree((void *)pElem); }
    void construct(pointer pElem, const_reference sValue) { new (pElem) T(sValue); }
    void destroy(pointer pElem) { pElem->~T(); }

    cRZFastSTLAllocator<T> &operator=(const cRZFastSTLAllocator &) { return *this; }
    size_type max_size() const { return size_t(-1); }

    pointer address(reference sValue) { return &sValue; }
    const_pointer address(const_reference sValue) const { return &sValue; }
};
