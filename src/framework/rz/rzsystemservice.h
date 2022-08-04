/**
 * @file
 *
 * @brief Basic implementation of the SystemService interface.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once

#include "igzsystemservice.h"
#include "rzunknownmt.h"

class cRZSystemService : public cIGZSystemService, public cRZUnknownMT
{
public:
    cRZSystemService(uint32_t sid, uint32_t priority);
    cRZSystemService(uint32_t sid, uint32_t priority, uint32_t tick_priority);
    virtual ~cRZSystemService() override;

    /**
     * @brief Casts the object to the interface specified by riid.
     * @param riid The GUID for the interface type to cast to.
     * @param ppvObj A pointer to a void pointer to store the result in.
     * @return Whether the object could be successfully cast or not.
     */
    virtual bool QueryInterface(uint32_t iid, void **obj) override;
    /**
     * @brief Adds a reference to this object.
     * @return The new reference count for this object.
     */
    virtual uint32_t AddRef() override;
    /**
     * @brief Removes a reference to this object.
     * @return The new reference count for this object.
     *
     * If the reference count is zero, the object should delete itself and
     * return zero unless it has pretty good reason not to, such as
     * refcounting for the object being handled by some other service.
     */
    virtual uint32_t Release() override;

    /**
     * @return A unique id identifying this service.
     */
    virtual uint32_t GetServiceID() const override;
    /**
     * @brief Sets an ID for this service.
     * @param sid The ID to give this service.
     */
    virtual void SetServiceID(uint32_t sid) override;
    /**
     * @return This service's priority. Lower values give higher priority.
     */
    virtual int32_t GetServicePriority() const override;
    /**
     * @return Is the service running.
     */
    virtual bool IsServiceRunning() const override;
    /**
     * @brief Toggles whether or not the service is currently running.
     */
    virtual void SetServiceRunning(bool running) override;
    /**
     * @brief A callback for post-framework initialization.
     */
    virtual bool Init() override;
    /**
     * @brief A callback for pre-framework shutdown.
     */
    virtual bool Shutdown() override;
    /**
     * @brief A callback for each active tick when the game is focused.
     *
     * The service must be registered to receive active tick callbacks via
     * cIGZFrameWork::AddToTick(cIGZSystemService* service)
     */
    virtual bool OnTick() override;
    /**
     * @brief A callback for each idle tick when the game is not focused
     *
     * The service must be registered to receive idle tick callbacks via
     * cIGZFrameWork::AddToOnIdle(cIGZSystemService* service)
     */
    virtual bool OnIdle() override;
    /**
     * @return This service's tick priority. Lower values give higher priority.
     */
    virtual int32_t GetServiceTickPriority() const override;

protected:
    uint32_t mnServiceID;
    int32_t mnServicePriority;
    int32_t mnServiceTickPriority;
    bool mbIsServiceRunning;
};
