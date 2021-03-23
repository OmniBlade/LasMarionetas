/**
 * @file
 *
 * @brief An interface for the COM root class.
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

class cGZCOMLibrary;
class cIGZCOMLibrary;
class cIGZFrameWork;
class cIGZString;
class cIGZSystemService;

static constexpr uint32_t GZIID_cIGZCOM = 0x4241330D;

/**
 * @brief The COM root for the game
 *
 * cIGZCOM is the highest-level interface for working with application
 * objects. This class provides methods for creating instances of other class
 * instances given their class and interface IDs, gives access to the
 * framework, and allows the generation of GUIDs.
 */
class cIGZCOM : public cIGZUnknown
{
public:
    typedef bool (*GZCOMLibraryIterationFunction)(cGZCOMLibrary *, void *);
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
    virtual bool GetClassObject(uint32_t clsid, uint32_t iid, void **ppvObj) = 0;
    /**
     * @brief Returns a pointer to the framework instance
     */
    virtual cIGZFrameWork *FrameWork() = 0;
    /**
     * @brief Loads the library at the given path
     * @return Whether the library was successfully loaded
     */
    virtual bool AddLibrary(const cIGZString &sPath) = 0;
    /**
     * @brief Prunes and unloads libraries with no references
     */
    virtual void FreeUnusedLibraries() = 0;
    /**
     * @brief Creates a random GUID and stores it in pGuid
     * @param pGuid Pointer to an unsigned int to store the result in
     * @return Whether the GUID was successfully generated and stored
     */
    virtual bool CreateGuid(uint32_t *pGuid) = 0;
    /**
     * @brief Creates a sequence of random GUIDs
     * @param pGuidArray The array to store the GUIDs in
     * @param nCount The number of GUIDs to generate
     * @see CreateGuid(uint32_t* pGuid)
     */
    virtual bool CreateGuids(uint32_t pGuidArray[], uint32_t nCount) = 0;
    /**
     * @brief Instantiates references to the COM director and framework
     */
    virtual bool RealInit() = 0;
    /**
     * @brief Unloads all ibraries and prepares for termination
     */
    virtual void RealShutdown() = 0;
    /**
     * @brief Does what it says on the tin.
     */
    virtual cIGZSystemService *SetServiceRunning(bool bRunning) = 0;
    /**
     * @brief Stores a list of loaded libraries in the given array
     * @param pLibraries The array to store the library references in
     * @param nCount A reference to an unsigned int counting the number of libraries
     * @return The number of libraries loaded
     */
    virtual uint32_t EnumerateLibraries(cIGZCOMLibrary *pLibraries[], uint32_t &nCount) = 0;
    /**
     * @brief Iterates over all loaded libraries using the iterator function
     * @param iterationFunction The iterator function to use
     * @param pContext Arbitrary data to pass to the iterator as a void pointer
     */
    virtual void EnumerateLibraries(GZCOMLibraryIterationFunction iterationFunction, void *pContext) = 0;
};

/**
 * @brief Obtains the cIGZCOM root implementation from the application/dll.
 *
 * The implementation for this is found with the cRZCOMDllDirector implementation in rz/rzcomdlldirector.cpp.
 *
 * Each application and plugin shares the same implementation but will have its own version linked.
 *
 * @see cRZCOMDllDirector
 */
cIGZCOM *GZCOM();
