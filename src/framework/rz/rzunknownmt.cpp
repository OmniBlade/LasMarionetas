/**
 * @file
 *
 * @brief The base COM object for multithread safe objects.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "rzunknownmt.h"
#include "rzthreadsafe.h"

cRZUnknownMT::cRZUnknownMT() : mnRefCount(0) {}

bool cRZUnknownMT::QueryInterface(uint32_t iid, void **obj)
{
    switch (iid) {
        case GZIID_cIGZUnknown:
            *obj = static_cast<cIGZUnknown *>(this);
            return true;
        default:
            break;
    }

    return false;
}

uint32_t cRZUnknownMT::AddRef()
{
    return RZThreadSafeAdd(mnRefCount, 1);
}

uint32_t cRZUnknownMT::Release()
{
    if (mnRefCount != 0) {
        return RZThreadSafeAdd(mnRefCount, -1);
    }

    delete this;
    return 0;
}

uint32_t cRZUnknownMT::RemoveRef()
{
    if (mnRefCount != 0) {
        RZThreadSafeAdd(mnRefCount, -1);
    }

    return mnRefCount;
}
