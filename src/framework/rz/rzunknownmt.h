/**
 * @file
 *
 * @brief The base COM object for multithread safe objects.
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

class cRZUnknownMT : public cIGZUnknown
{
public:
    cRZUnknownMT();
    virtual ~cRZUnknownMT() {}

    /**
     * @brief Casts the object to the interface specified by riid.
     * @param riid The GUID for the interface type to cast to.
     * @param ppvObj A pointer to a void pointer to store the result in.
     * @return Whether the object could be successfully cast or not.
     */
    virtual bool QueryInterface(uint32_t iid, void **obj) override;
    /**
     * @brief Adds a reference to this object.
     * @return The new reference count for this object.
     */
    virtual uint32_t AddRef() override;
    /**
     * @brief Removes a reference to this object.
     * @return The new reference count for this object.
     *
     * If the reference count is zero, the object should delete itself and
     * return zero unless it has pretty good reason not to, such as
     * refcounting for the object being handled by some other service.
     */
    virtual uint32_t Release() override;
    /**
     * @brief Decrements number of references by one.
     * @return Remaining references to this object.
     */
    virtual uint32_t RemoveRef();
    /**
     * @return Remaining references to this object.
     */
    virtual uint32_t RefCount() { return mnRefCount; }

protected:
    volatile uint32_t mnRefCount;
};
