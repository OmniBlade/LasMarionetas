/**
 * @file
 *
 * @brief Implementation for the COM root class.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "gzcom.h"
#include "gzframework.h"
#include "rzapppath.h"
#include "rzcomdlldirector.h"

cGZCOM::cGZCOM() :
    cRZSystemService(kGZCLSID_cGZCOM, cIGZSystemService::kPriorityAboveNormal),
    mbIsInitialized(false),
    field_30(0),
    mIDMap(),
    mLibraries(),
    mpCurrentLib(nullptr),
    mpFramework(nullptr),
    mpCOMDirector(nullptr),
    mnNextGuidGenTime(0),
    mRng()
{
}

cGZCOM::~cGZCOM() {}

bool cGZCOM::QueryInterface(uint32_t iid, void **obj)
{
    switch (iid) {
        case GZIID_cIGZCOM:
            *obj = static_cast<cIGZCOM *>(this);
            AddRef();
            return true;
        default:
            break;
    }

    return cRZSystemService::QueryInterface(iid, obj);
}

uint32_t cGZCOM::AddRef()
{
    return cRZSystemService::AddRef();
}

uint32_t cGZCOM::Release()
{
    return RemoveRef();
}

bool cGZCOM::GetClassObject(uint32_t clsid, uint32_t iid, void **obj)
{
    mCriticalSection.Lock();
    auto it = mIDMap.find(clsid);
    if (it == mIDMap.end()) {
        mCriticalSection.Unlock();

        if (clsid == kGZCLSID_cGZFrameWork) {
            return mpFramework->QueryInterface(iid, obj);
        }

        return mpCOMDirector->GetClassObject(clsid, iid, obj);
    }

    cGZCOMLibrary *const com_lib = (*it).second.second;
    mCriticalSection.Unlock();

    return GetLibObject(*com_lib, clsid, iid, obj);
}

cIGZFramework *cGZCOM::FrameWork()
{
    return mpFramework;
}

bool cGZCOM::AddLibrary(const cIGZString &path)
{
    cGZCOMLibrary lib(path);
    mCriticalSection.Lock();
    auto it(mLibraries.find(lib));

    if (it != mLibraries.end()) {
        mCriticalSection.Unlock();

        return true;
    }

    it = mLibraries.insert(lib).first;
    mCriticalSection.Unlock();
    cGZCOMLibrary &inserted_lib = const_cast<cGZCOMLibrary &>(*it);

    if (UpdateClassRegistry(inserted_lib)) {
        cIGZCOMDirector *const lib_director = inserted_lib.GetDirector();

        if (lib_director) {
            lib_director->OnStart(this);
        }

        return true;
    }

    return false;
}

void cGZCOM::FreeUnusedLibraries()
{
    mCriticalSection.Lock();

    for (auto it = mLibraries.begin(); it != mLibraries.end(); ++it) {
        cGZCOMLibrary &library = const_cast<cGZCOMLibrary &>(*it);

        if (library.IsLoaded() && CanUnloadLibrary(library)) {
            library.Free();
        }
    }

    mCriticalSection.Unlock();
}

bool cGZCOM::CreateGuid(uint32_t *pGuid)
{
    return false;
}

bool cGZCOM::CreateGuids(uint32_t pGuidArray[], uint32_t nCount)
{
    return false;
}

bool cGZCOM::RealInit()
{
    if (!mbIsInitialized) {
        mpFramework = RZGetFramework();
        mpCOMDirector = RZGetCOMDllDirector();

        cRZString app_path;
        RZGetCurrentAppPath(app_path);

        if (mpCOMDirector->InitializeCOM(this, app_path)) {
            mbIsInitialized = true;

            return true;
        }

        return false;
    }

    return true;
}

void cGZCOM::RealShutdown()
{
    if (mbIsInitialized) {
        FreeAllLibraries();
        mpFramework = nullptr;
        mbIsInitialized = false;
    }
}

void cGZCOM::SetServiceRunning(bool running)
{
    cRZSystemService::SetServiceRunning(running);
}

uint32_t cGZCOM::EnumerateLibraries(cIGZCOMLibrary *libraries[], uint32_t &count)
{
    if (libraries != nullptr) {
        auto it = mLibraries.begin();
        count = std::min(size_t(count), mLibraries.size());

        for (unsigned i = 0; i < count; ++i) {
            cGZCOMLibrary *const library = const_cast<cGZCOMLibrary *>(&(*it));
            libraries[i] = library;
            libraries[i]->AddRef();
            ++it;
        }
    } else {
        count = mLibraries.size();
    }

    return count;
}

void cGZCOM::EnumerateLibraries(GZCOMLibraryIterationFunction iterator_func, void *context)
{
    if (iterator_func != nullptr) {
        for (auto it = mLibraries.begin(); it != mLibraries.end(); ++it) {
            cGZCOMLibrary *const library = const_cast<cGZCOMLibrary *>(&(*it));

            if (!iterator_func(library, context)) {
                break;
            }
        }
    }
}

bool cGZCOM::Init()
{
    return true;
}

bool cGZCOM::Shutdown()
{
    return true;
}

void cGZCOM::FreeAllLibraries()
{
    mCriticalSection.Lock();

    for (auto it = mLibraries.begin(); it != mLibraries.end(); ++it) {
        cGZCOMLibrary &library = const_cast<cGZCOMLibrary &>(*it);

        if (library.IsLoaded()) {
            library.Free();
        }
    }

    mCriticalSection.Unlock();
}

bool cGZCOM::UpdateClassRegistry(cGZCOMLibrary &lib)
{
    if (lib.Load()) {
        cIGZCOMDirector *const lib_director = lib.GetDirector();
        mCriticalSection.Lock();
        mpCurrentLib = &lib;
        lib_director->EnumClassObjects(cGZCOM::AddEntryCallback, static_cast<cGZCOM *>(this));
        mCriticalSection.Unlock();

        return true;
    }

    return false;
}

bool cGZCOM::GetLibObject(cGZCOMLibrary &lib, uint32_t clsid, uint32_t iid, void **obj)
{
    if (!lib.IsLoaded() && !lib.Load()) {
        return false;
    }

    cIGZCOMDirector *const lib_director = lib.GetDirector();

    return lib_director->GetClassObject(clsid, iid, obj);
}

bool cGZCOM::CanUnloadLibrary(cGZCOMLibrary &lib)
{
    if (lib.IsLoaded()) {
        cIGZCOMDirector *const lib_director = lib.GetDirector();

        return lib_director->CanUnloadNow();
    }

    return true;
}

void cGZCOM::AddEntryCallback(uint32_t clsid, uint32_t version, void *context)
{
    cGZCOM *const com_obj = reinterpret_cast<cGZCOM *>(context);
    auto it = com_obj->mIDMap.find(clsid);

    if (it != com_obj->mIDMap.end() && (*it).second.first < version) {
        com_obj->mIDMap.erase(it);
    }

    com_obj->mIDMap.insert(std::pair<uint32_t, tMapData>(clsid, tMapData(version, com_obj->mpCurrentLib)));
}
