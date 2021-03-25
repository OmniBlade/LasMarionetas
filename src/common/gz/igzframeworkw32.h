/**
 * @file
 *
 * @brief Base framework interface for supporting Win32 applications.
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
#include <windows.h>

static constexpr uint32_t GZIID_cIGZFrameWorkW32 = 0x23EA70A1;

class cIGZFrameWork;

class cIGZFrameWorkW32 : public cIGZUnknown
{
public:
    /**
     * @return Returns a pointer to the class using the cIZGFrameWork derived interface.
     */
    virtual cIGZFrameWork *AsIGZFrameWork() = 0;
    /**
     * @return Returns the windows instance of the application.
     */
    virtual void *GetWindowsInstance() = 0;
    /**
     * @return Returns the main window handle of the application.
     */
    virtual void *GetMainHWND() = 0;
    /**
     * @brief Sets the main window handle of the application.
     */
    virtual void SetMainHWND(void *hwnd) = 0;
};
