/**
 * @file
 *
 * @brief Interface for graphic buffers.
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

static constexpr uint32_t GZIID_cIGZBuffer = 0x20732180;

class cIGZBufferHardwareCache;
class cRZRect;

class cGZBufferColorType
{
public:
    uint32_t pixel_format;
    uint32_t bpp;
};

class cIGZBuffer : public cIGZUnknown
{
public:
    enum eLockFlags
    {
        kLockFlagsUnk, // TODO determine values for this enum
    };

public:
    /**
     * @brief Initialise the buffer.
     * @param w width to initialise to.
     * @param h height to initialise to.
     * @param color_type Color type to use for this buffer.
     * @return Whether the buffer was successfully initialised.
     */
    virtual bool Init(uint32_t w, uint32_t h, cGZBufferColorType color_type) = 0;
    /**
     * @return Whether the buffer was successfully shut down.
     */
    virtual bool Shutdown() = 0;
    /**
     * @return Whether the buffer was successfully uninitialised.
     */
    virtual bool Uninitialize() = 0;
    /**
     * @brief Lock the buffer.
     * @param flags Bit flags that determine the nature of the lock.
     * @return Whether the buffer was successfully locked.
     */
    virtual bool Lock(eLockFlags flags) = 0;
    /**
     * @brief Unlock the buffer.
     * @param flags Bit flags that determine the nature of the lock.
     * @return Whether the buffer was successfully unlocked.
     */
    virtual bool Unlock(eLockFlags flags) = 0;
    /**
     * @return Whether the buffer is currently locked.
     */
    virtual bool IsLocked() const = 0;
    /**
     * @return Width of the buffer.
     */
    virtual uint32_t Width() const = 0;
    /**
     * @return Height of the buffer.
     */
    virtual uint32_t Height() const = 0;
    /**
     * @brief Get the area of the buffer.
     * @param area A cRZRect object to copy the area info to.
     */
    virtual void GetBufferArea(cRZRect &area) const = 0;
    /**
     * @return A cRZRect object reference holding the area info.
     */
    virtual const cRZRect &GetBufferArea() const = 0;
    /**
     * @return Current memory used by the buffer.
     */
    virtual uint32_t GetMemoryUsage() const = 0;
    /**
     * @return Current color type used by the buffer.
     */
    virtual cGZBufferColorType GetColorType() const = 0;
    /**
     * @brief Get the color type of the buffer.
     * @param color_type A cGZBufferColorType object to copy the color info to.
     */
    virtual void GetColorType(cGZBufferColorType &color_type) const = 0;
    /**
     * @return Current bits per pixel for this buffer.
     */
    virtual uint32_t GetBitsPerPixel() const = 0;
    /**
     * @return Current bytes per pixel for this buffer.
     */
    virtual uint32_t GetBytesPerPixel() const = 0;
    /**
     * @brief Fill an area within the buffer with a single color.
     * @param color Color to fill with.
     * @param dst_rect Area to apply the fill to.
     * @param clipping_rect An area to clip the fill to rather than the whole of dst_rect.
     * @return Whether the buffer was successfully filled.
     */
    virtual bool Fill(uint32_t color, const cRZRect &dst_rect, cRZRect *clipping_rect) = 0;
    /**
     * @brief Fill an area within the buffer with a single color.
     * @param color Color to fill with.
     * @param left Left extent of the destination area.
     * @param top Top extent of the destination area.
     * @param right Right extent of the destination area.
     * @param bottom Bottom extent of the destination area.
     * @param clipping_rect An area to clip the fill to rather than the whole of dst_rect.
     * @return Whether the buffer was successfully filled.
     */
    virtual bool Fill(uint32_t color, int32_t left, int32_t top, int32_t right, int32_t bottom, cRZRect *clipping_rect) = 0;
    /**
     * @brief Fill an area within the buffer with a single color.
     * @param color Color to fill with.
     * @param clipping_rect An area to clip the fill to rather than the whole of buffer.
     * @return Whether the buffer was successfully filled.
     */
    virtual bool Fill(uint32_t color, cRZRect *clipping_rect) = 0;
    /**
     * @brief Get the current value of the pixel a certain coordinates in the buffer.
     * @param x X coordinate to get the pixel value from.
     * @param y y coordinate to get the pixel value from.
     * @return Color of the pixel.
     */
    virtual uint32_t GetPixel(uint32_t x, uint32_t y) const = 0;
    /**
     * @brief Set the current value of the pixel a certain coordinates in the buffer.
     * @param x X coordinate to set the pixel value at.
     * @param y y coordinate to set the pixel value at.
     * @param color Color of the pixel.
     */
    virtual void SetPixel(uint32_t x, uint32_t y, uint32_t color) = 0;
    /**
     * @param transparency Transparency to set the buffer to.
     */
    virtual void SetTransparency(uint32_t transparency) = 0;
    /**
     * @return Transparency the buffer is set to.
     */
    virtual uint32_t GetTransparency() const = 0;
    /**
     * @brief Is a given pixel transparent?
     * @param x X coordinate to get the pixel value from.
     * @param y y coordinate to get the pixel value from.
     * @return If the pixel is transparent.
     */
    virtual bool IsPixelTransparent(uint32_t x, uint32_t y) = 0;
    /**
     * @brief Retrieve current transparent color
     * @param color An integer reference to hold the color.
     * @return If transparent color was retrieved successfully.
     */
    virtual bool GetTransparentColor(uint32_t &color) const = 0;
    /**
     * @brief Remove transparency from the buffer.
     */
    virtual void RemoveTransparency() = 0;
    /**
     * @brief Draw a line on the buffer.
     * @param x1 X coordinate to draw the line from.
     * @param y1 y coordinate to draw the line from.
     * @param x2 X coordinate to draw the line to.
     * @param y2 y coordinate to draw the line to.
     * @param color Color to draw the line.
     * @param border Offset from the buffer edges to clip to.
     * @return Was the line drawn.
     */
    virtual bool DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color, uint32_t border) = 0;
    virtual bool Blt(cIGZBuffer *, const cRZRect &, const cRZRect &, const cRZRect *) = 0;
    /**
     * @brief Convert rgb values to a native int representation.
     * @param r Red value.
     * @param g Green value.
     * @param b Blue value.
     * @return Color in native format.
     */
    virtual uint32_t ConvertRGBValueToNative(uint8_t r, uint8_t g, uint8_t b) = 0;
    /**
     * @brief Convert native color representation to rgb.
     * @param native Native color value.
     * @param r Reference to holder for red value.
     * @param g Reference to holder for green value.
     * @param b Reference to holder for blue value.
     * @return Conversion successful.
     */
    virtual bool ConvertNativeValueToRGB(uint32_t native, uint8_t &r, uint8_t &g, uint8_t &b) = 0;
    /**
     * @brief Convert rgba values to a native int representation.
     * @param r Red value.
     * @param g Green value.
     * @param b Blue value.
     * @param a Alpha value.
     * @return Color in native format.
     */
    virtual uint32_t ConvertRGBAValueToNative(uint8_t r, uint8_t g, uint8_t b, uint8_t a) = 0;
    /**
     * @brief Convert native color representation to rgba.
     * @param native Native color value.
     * @param r Reference to holder for red value.
     * @param g Reference to holder for green value.
     * @param b Reference to holder for blue value.
     * @param a Reference to holder for alpha value.
     * @return Conversion successful.
     */
    virtual bool ConvertNativeValueToRGBA(uint32_t native, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a) = 0;
    /**
     * @return The color surface bits.
     */
    virtual uint32_t GetColorSurfaceBits() = 0;
    /**
     * @return The buffers stride.
     */
    virtual uint32_t GetColorSurfaceStride() = 0;
    /**
     * @return The current change counter.
     */
    virtual uint32_t GetChangeCounter() = 0;
    /**
     * @return The current hardware cache.
     */
    virtual cIGZBufferHardwareCache *GetHardwareCache() = 0;
    /**
     * @brief Set the hardware cache.
     * @param cache The hardware cache to set.
     * @return Was the cache set?
     */
    virtual bool SetHardwareCache(cIGZBufferHardwareCache *cache) = 0;
    /**
     * @return Is the buffer ready?
     */
    virtual bool IsReady() = 0;
};
