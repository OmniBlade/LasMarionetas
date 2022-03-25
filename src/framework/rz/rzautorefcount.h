/**
 * @file
 *
 * @brief Scoped wrapper for holding a cIGZUnknown based pointer.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once

#include "igzunknown.h"
#include <type_traits>

template<class T> class cRZAutoRefCount
{
    static_assert(std::is_base_of<cIGZUnknown, T>::value, "T must extend cIGZUnknown");

public:
    cRZAutoRefCount() : mpObject(nullptr) {}
    cRZAutoRefCount(T *object) : mpObject(object) {}

    cRZAutoRefCount<T> &operator=(T *other)
    {
        if (mpObject != other) {
            if (mpObject) {
                mpObject->Release();
            }

            mpObject = other;
            if (other) {
                other->AddRef();
            }
        }

        return *this;
    }

    ~cRZAutoRefCount()
    {
        if (mpObject) {
            mpObject->Release();
        }
    }

    T *operator->() const { return mpObject; }
    T &operator*() const { return *mpObject; }
    operator T *() const { return mpObject; }

private:
    T *mpObject;
};