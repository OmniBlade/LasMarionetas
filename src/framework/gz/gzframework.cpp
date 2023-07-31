/**
 * @file
 *
 * @brief Implementation for a default framework root class.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "gzframework.h"

cIGZApp *cGZFramework::mpApp = nullptr;
cGZFramework *cGZFramework::mpFramework = nullptr;
cGZFramework *cGZFramework::mpSavedFramework = nullptr;
int32_t cGZFramework::mnReturnCode = 0;

cGZFramework::cGZFramework() {}

cGZFramework::~cGZFramework() {}

bool cGZFramework::QueryInterface(uint32_t iid, void **obj)
{
    return false;
}

uint32_t cGZFramework::AddRef()
{
    return uint32_t();
}

uint32_t cGZFramework::Release()
{
    return uint32_t();
}

bool cGZFramework::AddSystemService(cIGZSystemService *service)
{
    return false;
}

bool cGZFramework::RemoveSystemService(cIGZSystemService *service)
{
    return false;
}

bool cGZFramework::GetSystemService(uint32_t srvid, uint32_t riid, void **service)
{
    return false;
}

bool cGZFramework::EnumSystemServices(cIGUnknownEnumerator &enumerator, cIGZUnknown *service, uint32_t unk)
{
    return false;
}

bool cGZFramework::AddHook(cIGZFrameworkHooks *hooks)
{
    return false;
}

bool cGZFramework::RemoveHook(cIGZFrameworkHooks *hooks)
{
    return false;
}

bool cGZFramework::AddToTick(cIGZSystemService *service)
{
    return false;
}

bool cGZFramework::RemoveFromTick(cIGZSystemService *service)
{
    return false;
}

bool cGZFramework::AddToOnIdle(cIGZSystemService *service)
{
    return false;
}

bool cGZFramework::RemoveFromOnIdle(cIGZSystemService *service)
{
    return false;
}

int32_t cGZFramework::GetOnIdleInterval()
{
    return int32_t();
}

bool cGZFramework::SetOnIdleInterval(int32_t nInterval)
{
    return false;
}

bool cGZFramework::OnTick(uint32_t time_elapsed)
{
    return false;
}

bool cGZFramework::OnIdle()
{
    return false;
}

bool cGZFramework::IsTickEnabled()
{
    return false;
}

cIGZFramework *cGZFramework::ToggleTick(bool tick)
{
    return nullptr;
}

int32_t cGZFramework::Quit(int32_t quit_reason)
{
    return int32_t();
}

void cGZFramework::AbortiveQuit(int32_t quit_reason) {}

char *cGZFramework::CommandLine()
{
    return nullptr;
}

bool cGZFramework::IsInstall()
{
    return false;
}

cIGZCOM *cGZFramework::GetCOMObject()
{
    return nullptr;
}
