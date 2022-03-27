/**
 * @file
 *
 * @brief Interface for cursor management.
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

static constexpr uint32_t GZIID_cIGZCursor = 0x834C471E;

class cIGZBuffer;
class cIGZString;

// Handle is OS specific, HMODULE for windows, void * for unix. Use void * that can hold either type.
typedef void *GZCursorHandle;

class cIGZCursor : public cIGZUnknown
{
public:
    /**
     * @return Was the cursor initialised successfully? Always true in known examples.
     */
    virtual bool Init() = 0;
    /**
     * @return Was the cursor shut down successfully? Always true in known examples.
     */
    virtual bool Shutdown() = 0;
    /**
     * @brief Set this cursor as the active cursor.
     * @param onoff Should this cursor really be set active?
     */
    virtual void SetCursor(bool onoff) = 0;
    /**
     * @brief Set the cursor display status, an internal counter is incremented/decremented.
     * @param onoff Increment or decrement internal counter? Internal counter must be > 0 to show.
     * @return Current value of the internal counter.
     */
    virtual int32_t ShowCursor(bool onoff) = 0;
    /**
     * @brief Retrieves this cursors "hot spot".
     * @param x An integer reference to hold the X component of the coordinate.
     * @param y An integer reference to hold the Y component of the coordinate.
     */
    virtual void GetCursorHotSpot(int32_t &x, int32_t &y) = 0;
    /**
     * @brief Set this cursors "hot spot".
     * @param x An integer holding the X component of the coordinate.
     * @param y An integer holding the Y component of the coordinate.
     */
    virtual void SetCursorHotSpot(int32_t x, int32_t y) = 0;
    /**
     * @brief Loads a named cursor.
     * @param name String holding the name of the cursor to load.
     * @return Was the cursor image data successfully loaded?
     */
    virtual bool LoadCursor(const cIGZString &name) = 0;
    /**
     * @brief Set an operating system specific cursor handle.
     * @param handle Opaque handle object provided by the OS.
     */
    virtual void SetCursorHandle(GZCursorHandle handle) = 0;
    /**
     * @brief Set buffer object to hold cursor image data.
     * @param buffer cIGZBuffer object to hold image data.
     */
    virtual void SetCursorBuffer(cIGZBuffer *buffer) = 0;
    /**
     * @return Does the graphics system service support .ani formatted cursor graphics?
     */
    virtual bool SystemHasCursorSupport() = 0;
    /**
     * @return Currently set cIGZBuffer object to buffer cursor graphic data.
     */
    virtual cIGZBuffer *GetCursorBuffer() = 0;
};
