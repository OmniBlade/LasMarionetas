/**
 * @file
 *
 * @brief An interface for framework hook event subscribers.
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

static constexpr uint32_t GZIID_cIGZFrameworkHooks = 0x03FA40BF;

/**
 * @brief An interface for framework hook event subscribers.
 *
 * These are hooks that will be called by the application framework at various points in the objects lifetime.
 */
class cIGZFrameworkHooks : public cIGZUnknown
{
public:
    virtual bool PreFrameWorkInit() = 0;
    virtual bool PreAppInit() = 0;
    virtual bool PostAppInit() = 0;
    virtual bool PreAppShutdown() = 0;
    virtual bool PostAppShutdown() = 0;
    virtual bool PostSystemServiceShutdown() = 0;
    virtual bool AbortiveQuit() = 0;
    virtual bool OnInstall() = 0;
};
