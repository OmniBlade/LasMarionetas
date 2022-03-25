/**
 * @file
 *
 * @brief The base COM object that all others inherit from.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once
#include <stdint.h>

static constexpr uint32_t GZIID_cIGZUnknown = 1;

/**
 * @brief The base COM object that all others inherit from
 *
 * Any COM object that interacts with the game or will be used by directors
 * should inherit from this object and implement refcounting as specified.
 */
class cIGZUnknown
{
public:
    /**
     * @brief Casts the object to the interface specified by riid
     * @param riid The GUID for the interface type to cast to
     * @param ppvObj A pointer to a void pointer to store the result in
     * @return Whether the object could be successfully cast or not
     */
    virtual bool QueryInterface(uint32_t iid, void **obj) = 0;

    /**
     * @brief Adds a reference to this object
     * @return The new reference count for this object
     */
    virtual uint32_t AddRef() = 0;

    /**
     * @brief Removes a reference to this object
     * @return The new reference count for this object
     *
     * If the reference count is zero, the object should delete itself and
     * return zero unless it has pretty good reason not to, such as
     * refcounting for the object being handled by some other service.
     */
    virtual uint32_t Release() = 0;
};
