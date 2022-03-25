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
#include "igzcomdirector.h"
#include "igzframeworkhooks.h"
#include "rzfastalloc.h"
#include "rzstring.h"

#ifdef MATCH_ABI // STLPort is needed to match at ABI level and uses none standard hash_map.
#include <hash_map>
#else
#include <unordered_map>
#endif

#include <vector>

class cRZCOMDllDirector : public cIGZCOMDirector, public cIGZFrameWorkHooks
{
protected:
    enum FactorFunctionType
    {
        kFactorFunctionType1 = 1,
        kFactorFunctionType2 = 2
    };

public:
    typedef void (*DummyFunctionPtr)();
    typedef cIGZUnknown *(*FactoryFunctionPtr1)();
    typedef bool (*FactoryFunctionPtr2)(uint32_t, void **);

public:
    cRZCOMDllDirector();
    virtual ~cRZCOMDllDirector();

    // cIGZUnknown
    virtual bool QueryInterface(uint32_t riid, void **ppvObj) override;
    virtual uint32_t AddRef() override;
    virtual uint32_t Release() override;

    // cIGZCOMDirector
    virtual bool InitializeCOM(cIGZCOM *pCOM, const cIGZString &sLibraryPath) override;
    virtual bool OnStart(cIGZCOM *pCOM) override;
    virtual void EnumClassObjects(ClassObjectEnumerationCallback pCallback, void *pContext) override;
    virtual bool GetClassObject(uint32_t rclsid, uint32_t riid, void **ppvObj) override;
    virtual bool CanUnloadNow() override;
    virtual bool OnUnload() override { return true; }
    virtual uint32_t RefCount() override;
    virtual uint32_t RemoveRef() override;
    virtual cIGZFrameWork *FrameWork() override;
    virtual cIGZCOM *GZCOM() override;
    virtual bool GetLibraryPath(cIGZString &sLibraryPath) override;
    virtual uint32_t GetHeapAllocatedSize() override;

    // cIGZFrameWorkHooks
    virtual bool PreFrameWorkInit() override;
    virtual bool PreAppInit() override;
    virtual bool PostAppInit() override;
    virtual bool PreAppShutdown() override;
    virtual bool PostAppShutdown() override;
    virtual bool PostSystemServiceShutdown() override;
    virtual bool AbortiveQuit() override;
    virtual bool OnInstall() override;

protected:
    void AddDirector(cIGZCOMDirector *pCOMDirector);
    void AddCls(uint32_t clsid, FactoryFunctionPtr1 pff1);
    void AddCls(uint32_t clsid, FactoryFunctionPtr2 pff2);

protected:
    typedef std::vector<cRZCOMDllDirector *, cRZFastSTLAllocator<cRZCOMDllDirector *>> ChildDirectorArray;
    typedef uint32_t ClassObjectID;
    typedef std::pair<DummyFunctionPtr, int32_t> FactoryFuncRecord;
#ifdef MATCH_ABI
    typedef std::hash_map<ClassObjectID,
        FactoryFuncRecord,
        std::hash<ClassObjectID>,
        std::equal_to<ClassObjectID>,
        cRZFastSTLAllocator<std::pair<const ClassObjectID, FactoryFuncRecord>>>
        ClassObjectMap;
#else
    typedef std::unordered_map<ClassObjectID,
        FactoryFuncRecord,
        std::hash<ClassObjectID>,
        std::equal_to<ClassObjectID>,
        cRZFastSTLAllocator<std::pair<const ClassObjectID, FactoryFuncRecord>>>
        ClassObjectMap;
#endif

    uint32_t mnRefCount;
    uint32_t mDirectorID;
    cRZString msLibraryPath;
    cIGZCOM *mpCOM;
    cIGZFrameWork *mpFrameWork;
    ChildDirectorArray mChildDirectorArray;
    ClassObjectMap mClassObjectMap;
};

/**
 * @brief Obtains the cRZCOMDllDirector implementation from the application/dll.
 *
 * This function should be implemented in the application code.
 */
cRZCOMDllDirector *RZGetCOMDllDirector();
