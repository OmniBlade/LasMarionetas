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

#include "igzbuffer.h"
#include "igzunknown.h"
#include "rzrect.h"

static constexpr uint32_t GZIID_cIGZGraphicSystem = 0x0073283C;

class cGZDisplayMetrics;
class cGZDisplayTiming;
class cIGZCanvas;
class cIGZScreenBuffer;
class cIGZString;

class cGZScreenMetrics
{
public:
    uint32_t width;
    uint32_t height;
    uint32_t bit_depth;
};

class cIGZGraphicSystem : public cIGZUnknown
{
public:
    enum eVideoMemoryType
    {
        kVideoMemoryDefault,
    };

public:
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
    // TODO Unclear if this returns the same or a different type as GetScreenMetrics
    virtual cGZScreenMetrics GetGameMetrics() = 0;
    /**
     * @return Is the program currently running full screen?
     */
    virtual bool IsFullScreenMode() = 0;
    /**
     * @return Is the program currently running in a paletted display mode?
     */
    virtual bool IsScreenPaletted() = 0;
    /**
     * @brief Get the graphics system canvas.
     * @param riid The GUID for the canvas interface to retrieve.
     * @param obj A pointer to a void pointer to store the result in.
     * @return Whether the object could be successfully retrieved or not.
     */
    virtual bool GetCanvas(uint32_t iid, void **obj) = 0;
    /**
     * @return The graphics system canvas.
     */
    virtual cIGZCanvas *GetCanvas() = 0;
    /**
     * @return The canvas clipping rect.
     */
    virtual cRZRect CanvasRect() = 0;
    /**
     * @return Was the draw call successful?
     */
    virtual bool Draw() = 0;
    /**
     * @return Was drawing paused successful?
     */
    virtual bool PauseDraw() = 0;
    /**
     * @return Was drawing unpaused successful?
     */
    virtual bool UnpauseDraw() = 0;
    /**
     * @return Is drawing paused?
     */
    virtual bool IsDrawPaused() = 0;
    /**
     * @brief Display a message box with an OK button.
     * @param text Message box body text.
     * @param caption Message box title.
     * @return 1 if box was displayed and clicked.
     */
    virtual int32_t MessageBoxA(cIGZString const &text, cIGZString const &caption) = 0;
    /**
     * @param type Memory type to query.
     * @return Amount of video memory free.
     */
    virtual uint32_t VideoMemoryFree(eVideoMemoryType type) = 0;
    /**
     * @param type Memory type to query.
     * @return Amount of video memory in total.
     */
    virtual uint32_t VideoMemoryTotal(eVideoMemoryType type) = 0;
    /**
     * @param type Memory type to query.
     * @param unk Unknown bool.
     * @return Unknown value.
     */
    virtual uint32_t VideoMemory(eVideoMemoryType type, bool unk) = 0;
    virtual void TraceDriverInformation() = 0;
    virtual bool GetVideoDriverVersionInformation(uint32_t &, uint32_t &) = 0;
    virtual uint32_t GetVideoDriverInfoCount() = 0;
    virtual bool GetVideoDriverInfo(cIGZString &, cIGZString &, uint32_t) = 0;
    virtual bool GetVideoDriverInfoCurrent(cIGZString &, cIGZString &) = 0;
    /**
     * @param onoff State to set the cursor.
     * @return Index of the current cursor.
     */
    virtual int32_t ShowCursor(bool onoff) = 0;
    virtual void SetCursorBuffer(cIGZBuffer *buffer, int32_t hot_x, int32_t hot_y) = 0;
    virtual bool DoesSystemHaveCursorSupport() = 0;
    virtual bool GetCursorPosition(int32_t &x, int32_t &y) = 0;
    virtual void ConvertRGBValueToNative(uint32_t &native, uint8_t r, uint8_t g, uint8_t b) = 0;
    virtual uint32_t ConvertRGBValueToNative(uint8_t r, uint8_t g, uint8_t b) = 0;
    virtual void ConvertNativeValueToRGB(uint32_t native, uint8_t &r, uint8_t &g, uint8_t &b) = 0;
    virtual void RGBToHSB(uint8_t r, uint8_t g, uint8_t b, uint8_t &h, uint8_t &s, uint8_t &v) = 0;
    virtual void RGBToHSB(uint8_t r, uint8_t g, uint8_t b, float &h, float &s, float &v) = 0;
    virtual void HSBToRGB(uint8_t h, uint8_t s, uint8_t v, uint8_t &r, uint8_t &g, uint8_t &b) = 0;
    virtual void HSBToRGB(float h, float s, float v, uint8_t &r, uint8_t &g, uint8_t &b) = 0;
    virtual void RGBToHLS(uint8_t r, uint8_t g, uint8_t b, uint8_t &h, uint8_t &l, uint8_t &s) = 0;
    virtual void RGBToHLS(uint8_t r, uint8_t g, uint8_t b, float &h, float &l, float &s) = 0;
    virtual void HLSToRGB(uint8_t h, uint8_t l, uint8_t s, uint8_t &r, uint8_t &g, uint8_t &b) = 0;
    virtual void HLSToRGB(float h, float l, float s, uint8_t &r, uint8_t &g, uint8_t &b) = 0;
    virtual void RGBToCYM(uint8_t r, uint8_t g, uint8_t b, uint8_t &c, uint8_t &m, uint8_t &y) = 0;
    virtual void CYMToRGB(uint8_t c, uint8_t m, uint8_t y, uint8_t &r, uint8_t &g, uint8_t &b) = 0;
    virtual void RGBToCYMK(uint8_t r, uint8_t g, uint8_t b, uint8_t &c, uint8_t &m, uint8_t &y, uint8_t &k) = 0;
    virtual void CYMKToRGB(uint8_t c, uint8_t m, uint8_t y, uint8_t k, uint8_t &r, uint8_t &g, uint8_t &b) = 0;
    virtual void GetScreenTiming(cGZDisplayTiming &timing) = 0;
    virtual void GetRefreshRate(float &rate) = 0;
    virtual void GetRefreshRateAvg(float &rate) = 0;
    virtual void GetRefreshRateAvgLong(float &rate) = 0;
    virtual void PreInitSetDesiredGameResolution(cGZDisplayMetrics const &metrics) = 0;
    virtual void PreInitSetHardware3DAccellerationUse(bool onoff) = 0;
    virtual void PreInitSetWindowedMode(bool onoff) = 0;
    virtual void PreInitSetDesiredVRAMForTextures(uint32_t vram_size) = 0;
    virtual void PreInitSetDesiredVideoDriverIdentifier(uint8_t *id) = 0;
    virtual uint32_t GetTotalSurfaceMemory() = 0;
    virtual uint32_t GetTotalSystemSurfaceMemory() = 0;
    virtual uint32_t GetTotalVideoSurfaceMemory() = 0;
    virtual void IncrementTotalSystemSurfaceMemory(uint32_t step) = 0;
    virtual void DecrementTotalSystemSurfaceMemory(uint32_t step) = 0;
    virtual void IncrementTotalVideoSurfaceMemory(uint32_t step) = 0;
    virtual void DecrementTotalVideoSurfaceMemory(uint32_t step) = 0;
    virtual void MakeMemoryUsageString() = 0;
    virtual void OnCanvasInit() = 0;
    virtual void OnCanvasShutdown() = 0;
    virtual void OnCanvasReInit() = 0;
};
