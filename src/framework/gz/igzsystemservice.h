/**
 * @file
 *
 * @brief Interface for a system service.
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

static constexpr uint32_t GZIID_cIGZSystemService = 0x287FB697;

class cIGZSystemService : public cIGZUnknown
{
public:
    /**
     * @return A unique id identifying this service.
     */
    virtual uint32_t GetServiceID() const = 0;

    /**
     * @brief Sets an ID for this service.
     * @param sid The ID to give this service.
     */
    virtual void SetServiceID(uint32_t sid) = 0;

    /**
     * @return This service's priority. Lower values give higher priority.
     */
    virtual int32_t GetServicePriority() const = 0;

    /**
     * @return Is the service running.
     */
    virtual bool IsServiceRunning() const = 0;

    /**
     * @brief Toggles whether or not the service is currently running.
     */
    virtual void SetServiceRunning(bool running) = 0;

    /**
     * @brief A callback for post-framework initialization.
     */
    virtual bool Init() = 0;

    /**
     * @brief A callback for pre-framework shutdown.
     */
    virtual bool Shutdown() = 0;

    /**
     * @brief A callback for each active tick when the game is focused.
     *
     * The service must be registered to receive active tick callbacks via
     * cIGZFrameWork::AddToTick(cIGZSystemService* service)
     */
    virtual bool OnTick() = 0;

    /**
     * @brief A callback for each idle tick when the game is not focused
     *
     * The service must be registered to receive idle tick callbacks via
     * cIGZFrameWork::AddToOnIdle(cIGZSystemService* service)
     */
    virtual bool OnIdle() = 0;

    /**
     * @return This service's tick priority. Lower values give higher priority.
     */
    virtual int32_t GetServiceTickPriority() const = 0;
};
