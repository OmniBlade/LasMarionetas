/**
 * @file
 *
 * @brief Interface for persistant buffered resource.
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

static constexpr uint32_t GZIID_cIGZPersistBufferResource = 0x06F1568E;

class cIGZBuffer;

class cIGZPersistBufferResource : public cIGZUnknown
{
public:
    /**
     * @brief Casts the internal buffer object to the interface specified by riid.
     * @param riid The GUID for the interface type to cast to.
     * @param obj A pointer to a void pointer to store the result in.
     * @return Whether the object could be successfully cast or not.
     */
    virtual bool GetBuffer(uint32_t iid, void **obj) = 0;
    /**
     * @return Fetches the internal cIGZBuffer object.
     */
    virtual cIGZBuffer *GetBuffer() = 0;
    /**
     * @brief Set the internal buffer.
     * @param buffer cIGZBuffer object to set as the internal buffer.
     */
    virtual void SetBuffer(cIGZBuffer *buffer) = 0;
};
