/**
 * @file
 *
 * @brief Interface for a GZCOM Library.
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

static constexpr uint32_t GZIID_cIGZCOMLibrary = 0x07D4922E;

class cIGZCOMDirector;
class cIGZString;

// Handle is OS specific, HMODULE for windows, void * for unix. Use void * that can hold either type.
typedef void *GZLibraryHandle;

class cIGZCOMLibrary : public cIGZUnknown
{
public:
    /**
     * @brief Get the path on disk to the dynamic library.
     * @param path A string reference to recieve the path.
     * @return Was the path successfully retrieved.
     */
    virtual bool GetPath(cIGZString &path) const = 0;
    /**
     * @return Pointer to the libray COM directory interface.
     */
    virtual cIGZCOMDirector *GetDirector() const = 0;
    /**
     * @return OS specific dynamic library handle.
     */
    virtual GZLibraryHandle GetHandle() const = 0;
    /**
     * @return Is this library currently loaded.
     */
    virtual bool IsLoaded() const = 0;
};
