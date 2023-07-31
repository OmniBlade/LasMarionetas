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
#pragma once
#include "gzcomlibrary.h"
#include "igzcom.h"
#include "rzrandom.h"
#include "rzsystemservice.h"
#include "rzthreadsafeobject.h"
#include <map>
#include <set>

class cRZCOMDllDirector;

static constexpr uint32_t kGZCLSID_cGZCOM = 0xA3CD8DB3;

class cGZCOM : public cIGZCOM, public cRZThreadSafeObject, public cRZSystemService
{
    typedef std::pair<uint32_t, cGZCOMLibrary *> tMapData;
    typedef std::map<uint32_t, tMapData, std::less<uint32_t>, cRZFastSTLAllocator<std::pair<const uint32_t, tMapData>>>
        tIDMap;
    typedef std::set<cGZCOMLibrary, std::less<cGZCOMLibrary>, cRZFastSTLAllocator<cGZCOMLibrary>> tLibrarySet;

public:
    cGZCOM();
    virtual ~cGZCOM();

    /**
     * @brief Casts the object to the interface specified by riid
     * @param riid The GUID for the interface type to cast to
     * @param obj A pointer to a void pointer to store the result in
     * @return Whether the object could be successfully cast or not
     */
    virtual bool QueryInterface(uint32_t iid, void **obj) override;
    /**
     * @brief Adds a reference to this object
     * @return The new reference count for this object
     */
    virtual uint32_t AddRef() override;
    /**
     * @brief Removes a reference to this object
     * @return The new reference count for this object
     *
     * If the reference count is zero, the object should delete itself and
     * return zero unless it has pretty good reason not to, such as
     * refcounting for the object being handled by some other service.
     */
    virtual uint32_t Release() override;
    /**
     * @brief Creates an object of class type clsid with interface iid
     *
     * For a class registered with the given class ID in the COM, this
     * method creates a new object of that class type, casts it to the
     * interface with the given IID and stores it in the pointer pointed
     * to by ppvObj.
     *
     * @param clsid The class type of the object to create
     * @param iid The interface type to cast the object to
     * @param ppvObj A pointer to a pointer for the object reference
     * @return Whether the object was successfully created and stored
     */
    virtual bool GetClassObject(uint32_t clsid, uint32_t iid, void **obj) override;
    /**
     * @brief Returns a pointer to the framework instance
     */
    virtual cIGZFramework *FrameWork() override;
    /**
     * @brief Loads the library at the given path
     * @return Whether the library was successfully loaded
     */
    virtual bool AddLibrary(const cIGZString &path) override;
    /**
     * @brief Prunes and unloads libraries with no references
     */
    virtual void FreeUnusedLibraries() override;
    /**
     * @brief Creates a random GUID and stores it in pGuid
     * @param pGuid Pointer to an unsigned int to store the result in
     * @return Whether the GUID was successfully generated and stored
     */
    virtual bool CreateGuid(uint32_t *guid) override;
    /**
     * @brief Creates a sequence of random GUIDs
     * @param pGuidArray The array to store the GUIDs in
     * @param nCount The number of GUIDs to generate
     * @see CreateGuid(uint32_t* pGuid)
     */
    virtual bool CreateGuids(uint32_t guids[], uint32_t count) override;
    /**
     * @brief Instantiates references to the COM director and framework
     */
    virtual bool RealInit() override;
    /**
     * @brief Unloads all libraries and prepares for termination
     */
    virtual void RealShutdown() override;
    /**
     * @brief Does what it says on the tin.
     */
    virtual void SetServiceRunning(bool running) override;
    /**
     * @brief Stores a list of loaded libraries in the given array
     * @param pLibraries The array to store the library references in
     * @param nCount A reference to an unsigned int counting the number of libraries
     * @return The number of libraries loaded
     */
    virtual uint32_t EnumerateLibraries(cIGZCOMLibrary *libraries[], uint32_t &count) override;
    /**
     * @brief Iterates over all loaded libraries using the iterator function
     * @param iterationFunction The iterator function to use
     * @param pContext Arbitrary data to pass to the iterator as a void pointer
     */
    virtual void EnumerateLibraries(GZCOMLibraryIterationFunction iterator_func, void *context) override;
    /**
     * @brief A callback for post-framework initialization.
     */
    virtual bool Init() override;
    /**
     * @brief A callback for pre-framework shutdown.
     */
    virtual bool Shutdown() override;

    void FreeAllLibraries();
    bool GetLibObject(cGZCOMLibrary &lib, uint32_t clsid, uint32_t iid, void **obj);
    bool UpdateClassRegistry(cGZCOMLibrary &lib);
    bool CanUnloadLibrary(cGZCOMLibrary &lib);

    static void AddEntryCallback(uint32_t clsid, uint32_t version, void *context);

private:
    bool mbIsInitialized;
    uint32_t field_30; // TODO Unused.
    tIDMap mIDMap;
    tLibrarySet mLibraries;
    cGZCOMLibrary *mpCurrentLib;
    cIGZFramework *mpFramework;
    cRZCOMDllDirector *mpCOMDirector;
    cRZRandom mRng;
    uint32_t field_64; // TODO Unused.
    uint32_t field_68; // TODO Unused.
    uint32_t field_6C; // TODO Unused.
    int32_t mnNextGuidGenTime;
};
