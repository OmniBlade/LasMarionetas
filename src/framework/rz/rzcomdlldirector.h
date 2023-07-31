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

class cRZCOMDllDirector : public cIGZCOMDirector, public cIGZFrameworkHooks
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
    virtual bool QueryInterface(uint32_t riid, void **obj) override;
    virtual uint32_t AddRef() override;
    virtual uint32_t Release() override;

    // cIGZCOMDirector
    virtual bool InitializeCOM(cIGZCOM *com, const cIGZString &library_path) override;
    virtual bool OnStart(cIGZCOM *com) override;
    virtual void EnumClassObjects(ClassObjectEnumerationCallback callback, void *context) override;
    virtual bool GetClassObject(uint32_t rclsid, uint32_t riid, void **obj) override;
    virtual bool CanUnloadNow() override;
    virtual bool OnUnload() override { return true; }
    virtual uint32_t RefCount() override;
    virtual uint32_t RemoveRef() override;
    virtual cIGZFramework *FrameWork() override;
    virtual cIGZCOM *GZCOM() override;
    virtual bool GetLibraryPath(cIGZString &library_path) override;
    virtual uint32_t GetHeapAllocatedSize() override;

    // cIGZFrameworkHooks
    virtual bool PreFrameWorkInit() override;
    virtual bool PreAppInit() override;
    virtual bool PostAppInit() override;
    virtual bool PreAppShutdown() override;
    virtual bool PostAppShutdown() override;
    virtual bool PostSystemServiceShutdown() override;
    virtual bool AbortiveQuit() override;
    virtual bool OnInstall() override;

protected:
    void AddDirector(cRZCOMDllDirector *director);
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
    cRZString mzLibraryPath;
    cIGZCOM *mpCOM;
    cIGZFramework *mpFrameWork;
    ChildDirectorArray mChildDirectorArray;
    ClassObjectMap mClassObjectMap;
};

/**
 * @brief Obtains the top level cIGZCOM implementation registered with the top level COM director.
 */
cIGZCOM *GZCOM();

/**
 * @brief Obtains the cRZCOMDllDirector implementation from the application/dll.
 *
 * This function is implemented in the cGZCOMDllDirector translation unit for the main GZApp application, in plugin Dlls it
 * needs to be implemented to fetch the library COM Director.
 */
cRZCOMDllDirector *RZGetCOMDllDirector();

/**
 * PLUGIN_DLL Should be defined before inclusion of rzcomdlldirector.h in a COMDirector implementation translation unit.
 * see the demo plugin source for how this is used.
 */
#ifdef PLUGIN_DLL
/**
 * @brief Obtains the top applications level framework for a COM Plugin Dll.
 * 
 * This function is implemented in the cGZFramework translation unit for the main GZApp application.
 */
inline cIGZFramework *RZGetFramework()
{
    return RZGetCOMDllDirector()->FrameWork();
}

/**
 * @brief The game expects to be able to find and call this function as part of loading a GZ/RZ plugin.
 */
inline extern "C" cIGZCOMDirector *GZDllGetGZCOMDirector()
{
    return static_cast<cIGZCOMDirector *>(RZGetCOMDllDirector());
}
#endif
