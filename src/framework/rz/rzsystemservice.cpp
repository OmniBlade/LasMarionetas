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
#include "rzsystemservice.h"

cRZSystemService::cRZSystemService(uint32_t sid, uint32_t priority) :
    mnServiceID(sid), mnServicePriority(priority), mnServiceTickPriority(priority), mbIsServiceRunning(false)
{
}

cRZSystemService::cRZSystemService(uint32_t sid, uint32_t priority, uint32_t tick_priority) :
    mnServiceID(sid), mnServicePriority(priority), mnServiceTickPriority(tick_priority), mbIsServiceRunning(false)
{
}

cRZSystemService::~cRZSystemService() {}

bool cRZSystemService::QueryInterface(uint32_t iid, void **obj)
{
    switch (iid) {
        case GZIID_cIGZSystemService:
            *obj = static_cast<cIGZSystemService *>(this);
            return true;
        default:
            break;
    }

    return cRZUnknownMT::QueryInterface(iid, obj);
}

uint32_t cRZSystemService::AddRef()
{
    return cRZUnknownMT::AddRef();
}

uint32_t cRZSystemService::Release()
{
    return cRZUnknownMT::Release();
}

uint32_t cRZSystemService::GetServiceID() const
{
    return mnServiceID;
}

void cRZSystemService::SetServiceID(uint32_t sid)
{
    mnServiceID = sid;
}

int32_t cRZSystemService::GetServicePriority() const
{
    return mnServicePriority;
}

bool cRZSystemService::IsServiceRunning() const
{
    return mbIsServiceRunning;
}

void cRZSystemService::SetServiceRunning(bool running)
{
    mbIsServiceRunning = running;
}

bool cRZSystemService::Init()
{
    return true;
}

bool cRZSystemService::Shutdown()
{
    return true;
}

bool cRZSystemService::OnTick()
{
    return true;
}

bool cRZSystemService::OnIdle()
{
    return true;
}

int32_t cRZSystemService::GetServiceTickPriority() const
{
    return mnServiceTickPriority;
}
