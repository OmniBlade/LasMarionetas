/**
 * @file
 *
 * @brief A base interface for COM directors
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

class cIGZCOM;
class cIGZFrameWork;
class cIGZString;

static constexpr uint32_t GZIID_cIGZCOMDirector = 0xA21EE941;

/**
 * @brief A base interface for COM directors
 *
 * COM directors are classes that can be dynamically loaded and unloaded, and provide an interface for hotpluggable code to
 * interact with the game core. Directors can also be nested. Any GZCOM-compatible library must provide a director for the
 * game to load.
 */
class cIGZCOMDirector : public cIGZUnknown
{
public:
    typedef void (*ClassObjectEnumerationCallback)(uint32_t clsid, uint32_t reserved, void *context);

    /**
     * @brief Initializes the director with the given parameters
     *
     * When initializing, the COM director should store the COM reference and library path in member variables, and should
     * initialize any child directors as well.
     */
    virtual bool InitializeCOM(cIGZCOM *com, const cIGZString &library_path) = 0;
    /**
     * @brief A callback for after directors are loaded on game start
     * @return Always true. The effects of returning false are unknown.
     */
    virtual bool OnStart(cIGZCOM *com) = 0;
    /**
     * @brief Iterates over the director's class objects with the callback
     * @param callback The function to use when iterating over class IDs
     * @param context Arbitrary data to pass as a void pointer
     */
    virtual void EnumClassObjects(ClassObjectEnumerationCallback callback, void *context) = 0;
    /**
     * @see cIGZCOM::GetClassObject(uint32_t clsid, uint32_t iid, void **obj)
     */
    virtual bool GetClassObject(uint32_t rclsid, uint32_t riid, void **obj) = 0;
    /**
     * @brief Signals to the game if it is safe for the director to unload
     */
    virtual bool CanUnloadNow() = 0;
    /**
     * @brief A callback upon this director being unloaded
     */
    virtual bool OnUnload() = 0;
    /**
     * @return The number of references to this director
     */
    virtual uint32_t RefCount() = 0;
    /**
     * @see cIGZUnknown::Release
     */
    virtual uint32_t RemoveRef() = 0;
    /**
     * @return A reference to the GZCOM framework
     */
    virtual cIGZFrameWork *FrameWork() = 0;
    /**
     * @return A reference to the GZCOM itself
     */
    virtual cIGZCOM *GZCOM() = 0;
    /**
     * @brief Obtains the director UID
     *
     * Returns a unique ID for identifying the director.
     */
    virtual uint32_t GetDirectorID() const = 0;
    /**
     * @brief Stores the path to this library in the given parameter
     * @param library_path A reference to the string to store the path in
     * @return Whether the library path was successfully stored
     */
    virtual bool GetLibraryPath(cIGZString &library_path) = 0;
    /**
     * @return The number of bytes this director allocated on the heap
     *
     * When performing heap allocations such as creating class objects or using the new keyword or malloc, the director
     * should note it and update this value. Directors that only use the stack can and should return zero.
     */
    virtual uint32_t GetHeapAllocatedSize() = 0;
};
