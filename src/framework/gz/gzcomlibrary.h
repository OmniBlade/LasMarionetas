/**
 * @file
 *
 * @brief Implementation of the GZCOM library encapsulation class.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once
#include "igzcomlibrary.h"
#include "rzstring.h"

class cGZCOMLibrary : public cIGZCOMLibrary
{
public:
    cGZCOMLibrary(const cIGZString &library_path);
    virtual ~cGZCOMLibrary() {}

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
     * @brief Get the path on disk to the dynamic library.
     * @param path A string reference to recieve the path.
     */
    virtual void GetPath(cIGZString &path) const override;
    /**
     * @return Pointer to the libray COM directory interface.
     */
    virtual cIGZCOMDirector *GetDirector() const override;
    /**
     * @return OS specific dynamic library handle.
     */
    virtual tGZLibraryHandle GetHandle() const override;
    /**
     * @return Is this library currently loaded.
     */
    virtual bool IsLoaded() const override;

    bool operator<(const cGZCOMLibrary &rhs) const;
    bool Load();
    bool Free();

protected:
    bool mbLoaded;
    uint32_t mnRefCount;
    cIGZCOMDirector *mpDirector;
    cRZString mzLibraryPath;
    tGZLibraryHandle mHandle;
};
