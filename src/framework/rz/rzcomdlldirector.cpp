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
#include "rzcomdlldirector.h"
#include "igzcom.h"

cIGZCOM *GZCOM()
{
    return RZGetCOMDllDirector()->GZCOM();
}

cRZCOMDllDirector::cRZCOMDllDirector() :
    mnRefCount(0),
    mDirectorID(0),
    mzLibraryPath(),
    mpCOM(nullptr),
    mpFrameWork(nullptr),
    mChildDirectorArray(),
    mClassObjectMap()
{
    // Empty
}

cRZCOMDllDirector::~cRZCOMDllDirector()
{
    // Empty
}

bool cRZCOMDllDirector::QueryInterface(uint32_t riid, void **obj)
{
    switch (riid) {
        case GZIID_cIGZCOMDirector:
            *obj = static_cast<cIGZCOMDirector *>(this);
            AddRef();
            return true;

        case GZIID_cIGZFrameworkHooks:
            *obj = static_cast<cIGZFrameworkHooks *>(this);
            AddRef();
            return true;

        case GZIID_cIGZUnknown:
            *obj = static_cast<cIGZUnknown *>(static_cast<cIGZCOMDirector *>(this));
            AddRef();
            return true;
    }

    return false;
}

uint32_t cRZCOMDllDirector::AddRef()
{
    return ++mnRefCount;
}

uint32_t cRZCOMDllDirector::Release()
{
    return RemoveRef();
}

bool cRZCOMDllDirector::InitializeCOM(cIGZCOM *com, const cIGZString &library_path)
{
    if (com != nullptr) {
        mpCOM = com;
        mpFrameWork = com->FrameWork();
        mzLibraryPath = library_path;

        for (ChildDirectorArray::iterator it = mChildDirectorArray.begin(); it != mChildDirectorArray.end(); ++it) {
            cRZCOMDllDirector *const pDirector = *it;
            pDirector->InitializeCOM(com, library_path);
        }

        return true;
    }

    return false;
}

bool cRZCOMDllDirector::OnStart(cIGZCOM *com)
{
    return true;
}

void cRZCOMDllDirector::EnumClassObjects(ClassObjectEnumerationCallback callback, void *context)
{
    for (ChildDirectorArray::iterator it(mChildDirectorArray.begin()); it != mChildDirectorArray.end(); ++it) {
        cRZCOMDllDirector *const child_director = *it;
        child_director->EnumClassObjects(callback, context);
    }

    for (ClassObjectMap::iterator it2(mClassObjectMap.begin()); it2 != mClassObjectMap.end(); ++it2) {
        const uint32_t class_id = (*it2).first;
        callback(class_id, 0, context);
    }
}

bool cRZCOMDllDirector::GetClassObject(uint32_t rclsid, uint32_t riid, void **obj)
{
    for (ChildDirectorArray::iterator it(mChildDirectorArray.begin()); it != mChildDirectorArray.end(); ++it) {
        cRZCOMDllDirector *const director = *it;

        if (director->GetClassObject(rclsid, riid, obj)) {
            return true;
        }
    }

    ClassObjectMap::iterator it2 = mClassObjectMap.find(rclsid);

    if (it2 != mClassObjectMap.end()) {
        FactoryFuncRecord &ffr = (*it2).second;

        switch (ffr.second) {
            case kFactorFunctionType1: {
                FactoryFunctionPtr1 const ffp1 = reinterpret_cast<FactoryFunctionPtr1>(ffr.first);
                cIGZUnknown *const new_obj = ffp1();

                if (new_obj->QueryInterface(riid, obj)) {
                    return true;
                }

                new_obj->AddRef();
                new_obj->Release();
                break;
            }

            case kFactorFunctionType2: {
                FactoryFunctionPtr2 ffp2 = reinterpret_cast<FactoryFunctionPtr2>(ffr.first);
                return ffp2(riid, obj);
            }

            default:
                break;
        }
    }

    return false;
}

bool cRZCOMDllDirector::CanUnloadNow()
{
    if (mnRefCount == 0) {
        for (ChildDirectorArray::iterator it(mChildDirectorArray.begin()); it != mChildDirectorArray.end(); ++it) {
            cRZCOMDllDirector *const director = *it;

            if (!director->CanUnloadNow()) {
                return false;
            }
        }
    }

    return true;
}

uint32_t cRZCOMDllDirector::RefCount()
{
    return mnRefCount;
}

uint32_t cRZCOMDllDirector::RemoveRef()
{
    if (mnRefCount > 0) {
        --mnRefCount;
    }

    return mnRefCount;
}

cIGZFramework *cRZCOMDllDirector::FrameWork()
{
    return mpFrameWork;
}

cIGZCOM *cRZCOMDllDirector::GZCOM()
{
    return mpCOM;
}

bool cRZCOMDllDirector::GetLibraryPath(cIGZString &library_path)
{
    library_path = mzLibraryPath;
    return true;
}

uint32_t cRZCOMDllDirector::GetHeapAllocatedSize()
{
    return 0;
}

bool cRZCOMDllDirector::PreFrameWorkInit()
{
    return true;
}

bool cRZCOMDllDirector::PreAppInit()
{
    return true;
}

bool cRZCOMDllDirector::PostAppInit()
{
    return true;
}

bool cRZCOMDllDirector::PreAppShutdown()
{
    return true;
}

bool cRZCOMDllDirector::PostAppShutdown()
{
    return true;
}

bool cRZCOMDllDirector::PostSystemServiceShutdown()
{
    return true;
}

bool cRZCOMDllDirector::AbortiveQuit()
{
    return true;
}

bool cRZCOMDllDirector::OnInstall()
{
    return true;
}

void cRZCOMDllDirector::AddDirector(cRZCOMDllDirector *director)
{
    director->InitializeCOM(GZCOM(), mzLibraryPath);

    for (auto it(director->mChildDirectorArray.begin()); it != director->mChildDirectorArray.end(); ++it) {
        cRZCOMDllDirector *const child_director = *it;
        AddDirector(child_director);
    }

    mChildDirectorArray.push_back(director);
}

void cRZCOMDllDirector::AddCls(uint32_t clsid, FactoryFunctionPtr1 pff1)
{
    // ClassObjectMap::iterator it(mClassObjectMap.find(clsid));

    const ClassObjectMap::value_type entry(clsid, FactoryFuncRecord(DummyFunctionPtr(pff1), kFactorFunctionType1));
    mClassObjectMap.insert(entry);
}

void cRZCOMDllDirector::AddCls(uint32_t clsid, FactoryFunctionPtr2 pff2)
{
    // ClassObjectMap::iterator it(mClassObjectMap.find(clsid));

    const ClassObjectMap::value_type entry(clsid, FactoryFuncRecord(DummyFunctionPtr(pff2), kFactorFunctionType2));
    mClassObjectMap.insert(entry);
}
