/**
 * @file
 *
 * @brief Interface for a graphics system.
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
#include "igzbuffer.h"

static constexpr uint32_t GZIID_cIGZGraphicSystem = 0x0073283C;

class cIGZScreenBuffer;

class cGZScreenMetrics
{
public:
    uint32_t width;
    uint32_t height;
    uint32_t bit_depth;
};

class cIGZGraphicSystem : public cIGZUnknown
{
    /**
     * @brief Creates a buffer with a custom specified interface.
     * @param iid Interface ID to cast the returned buffer to.
     * @param buffer Pointer to a pointer to recieve the new buffer object.
     * @return Whether the buffer was successfully created.
     */
    virtual bool CreateBuffer(uint32_t iid, void **buffer) = 0;
    /**
     * @brief Creates a buffer with a cIGZBuffer interface.
     * @param buffer Pointer to a pointer to recieve the new buffer object.
     * @return Whether the buffer was successfully created.
     */
    virtual bool CreateBuffer(cIGZBuffer **buffer) = 0;
    /**
     * @return Is the system set to try and force surfaces in system ram?
     */
    virtual bool IsForceSystemRAMSurfaces() = 0;
    /**
     * @brief Sets whether the system should try and force surfaces into system ram.
     * @param force State we want to set it to.
     */
    virtual void SetForceSystemRAMSurfaces(bool force) = 0;
    /**
     * @return The current screen buffer.
     */
    virtual cIGZScreenBuffer *ScreenBuffer() = 0;
    /**
     * @return The current draw buffer.
     */
    virtual cIGZBuffer *ActiveDrawBuffer() = 0;
    /**
     * @return The current screen color type.
     */
    virtual cGZBufferColorType GetScreenColorType() = 0;
    /**
     * @return The current screen metrics.
     */
    virtual cGZScreenMetrics GetScreenMetrics() = 0;
    /**
     * @return The current game buffer.
     */
    virtual cGZScreenMetrics GetGameMetrics() = 0; // TODO Unclear if this returns the same or a different type as GetScreenMetrics
};
