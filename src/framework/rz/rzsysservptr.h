/**
 * @file
 *
 * @brief Scoped wrapper for obtaining a service pointer.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once
#include "igzframework.h"
#include "rzcomdlldirector.h"

template<class T, uint32_t riid, uint32_t srvid> class cRZSysServPtr
{
    static_assert(std::is_base_of<cIGZUnknown, T>::value, "T must extend cIGZUnknown");

public:
    cRZSysServPtr() : mpService(nullptr)
    {
        cIGZFramework *frame_work = RZGetFrameWork();
        if (frame_work) {
            frame_work->GetSystemService(srvid, riid, (void **)&mpService);
        }
    }

    ~cRZSysServPtr()
    {
        if (mpService) {
            mpService->Release();
        }
    }

    T *operator->() const { return mpService; }
    T &operator*() const { return *mpService; }
    operator T *() const { return mpService; }

protected:
    T *mpService;
};
