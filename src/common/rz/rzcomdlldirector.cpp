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
    msLibraryPath(),
    mpCOM(nullptr),
    mpFrameWork(nullptr),
    mChildDirectorArray(),
    mClassObjectMap()
{
    // Empty
}

cRZCOMDllDirector::~cRZCOMDllDirector(void)
{
    // Empty
}

bool cRZCOMDllDirector::QueryInterface(uint32_t riid, void **ppvObj)
{
    switch (riid) {
        case GZIID_cIGZCOMDirector:
            *ppvObj = static_cast<cIGZCOMDirector *>(this);
            AddRef();
            return true;

        case GZIID_cIGZFrameWorkHooks:
            *ppvObj = static_cast<cIGZFrameWorkHooks *>(this);
            AddRef();
            return true;

        case GZIID_cIGZUnknown:
            *ppvObj = static_cast<cIGZUnknown *>(static_cast<cIGZCOMDirector *>(this));
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

bool cRZCOMDllDirector::InitializeCOM(cIGZCOM *pCOM, const cIGZString &sLibraryPath)
{
    if (pCOM != nullptr) {
        mpCOM = pCOM;
        mpFrameWork = pCOM->FrameWork();
        msLibraryPath = sLibraryPath;

        for (ChildDirectorArray::iterator it = mChildDirectorArray.begin(); it != mChildDirectorArray.end(); ++it) {
            cRZCOMDllDirector *const pDirector = *it;
            pDirector->InitializeCOM(pCOM, sLibraryPath);
        }

        return true;
    }

    return false;
}

bool cRZCOMDllDirector::OnStart(cIGZCOM *pCOM)
{
    return true;
}

void cRZCOMDllDirector::EnumClassObjects(ClassObjectEnumerationCallback pCallback, void *pContext)
{
    for (ChildDirectorArray::iterator it(mChildDirectorArray.begin()); it != mChildDirectorArray.end(); ++it) {
        cRZCOMDllDirector *const pDirector = *it;
        pDirector->EnumClassObjects(pCallback, pContext);
    }

    for (ClassObjectMap::iterator it2(mClassObjectMap.begin()); it2 != mClassObjectMap.end(); ++it2) {
        const uint32_t classID = (*it2).first;
        pCallback(classID, 0, pContext);
    }
}

bool cRZCOMDllDirector::GetClassObject(uint32_t rclsid, uint32_t riid, void **ppvObj)
{
    for (ChildDirectorArray::iterator it(mChildDirectorArray.begin()); it != mChildDirectorArray.end(); ++it) {
        cRZCOMDllDirector *const pDirector = *it;
        if (pDirector->GetClassObject(rclsid, riid, ppvObj)) {
            return true;
        }
    }

    ClassObjectMap::iterator it2 = mClassObjectMap.find(rclsid);
    if (it2 != mClassObjectMap.end()) {
        FactoryFuncRecord &ffr = (*it2).second;
        switch (ffr.second) {
            case kFactorFunctionType1: {
                FactoryFunctionPtr1 const ffp1 = (FactoryFunctionPtr1)ffr.first;
                cIGZUnknown *const pObj = ffp1();
                if (pObj->QueryInterface(riid, ppvObj)) {
                    return true;
                }

                pObj->AddRef();
                pObj->Release();
                break;
            }

            case kFactorFunctionType2: {
                FactoryFunctionPtr2 ffp2 = (FactoryFunctionPtr2)ffr.first;
                return ffp2(riid, ppvObj);
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
            cRZCOMDllDirector *const pCOMDirectorTemp = *it;
            if (!pCOMDirectorTemp->CanUnloadNow()) {
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

cIGZFrameWork *cRZCOMDllDirector::FrameWork()
{
    return mpFrameWork;
}

cIGZCOM *cRZCOMDllDirector::GZCOM()
{
    return mpCOM;
}

bool cRZCOMDllDirector::GetLibraryPath(cIGZString &sLibraryPath)
{
    sLibraryPath = msLibraryPath;
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

void cRZCOMDllDirector::AddDirector(cIGZCOMDirector *pDirector)
{
    cRZCOMDllDirector *pCOMDirector = reinterpret_cast<cRZCOMDllDirector *>(pDirector);
    pCOMDirector->InitializeCOM(GZCOM(), msLibraryPath);

    for (auto it(pCOMDirector->mChildDirectorArray.begin()); it != pCOMDirector->mChildDirectorArray.end(); ++it) {
        cRZCOMDllDirector *const pCOMDirectorTemp = *it;
        AddDirector(pCOMDirectorTemp);
    }

    mChildDirectorArray.push_back(pCOMDirector);
}

void cRZCOMDllDirector::AddCls(uint32_t clsid, FactoryFunctionPtr1 pff1)
{
    ClassObjectMap::iterator it(mClassObjectMap.find(clsid));

    const ClassObjectMap::value_type entry(clsid, FactoryFuncRecord(DummyFunctionPtr(pff1), kFactorFunctionType1));
    mClassObjectMap.insert(entry);
}

void cRZCOMDllDirector::AddCls(uint32_t clsid, FactoryFunctionPtr2 pff2)
{
    ClassObjectMap::iterator it(mClassObjectMap.find(clsid));

    const ClassObjectMap::value_type entry(clsid, FactoryFuncRecord(DummyFunctionPtr(pff2), kFactorFunctionType2));
    mClassObjectMap.insert(entry);
}
