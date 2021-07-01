/**
 * @file
 *
 * @brief Interface for persistant resource objects.
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

static constexpr uint32_t GZIID_cIGZPersistResource = 0x456B8F1D;

class cGZPersistResourceKey
{
public:
    uint32_t mnType;
    uint32_t mnGroup;
    uint32_t mnInstance;
    uint32_t mnUnk;
};

class cIGZPersistResource : public cIGZUnknown
{
    virtual cGZPersistResourceKey GetKey() = 0;
    virtual bool SetKey(const cGZPersistResourceKey &) = 0;
    virtual uint32_t GetType() = 0;
    virtual uint32_t GetGroup() = 0;
    virtual uint32_t GetInstance() = 0;
    virtual bool SetType(uint32_t type) = 0;
    virtual bool SetGroup(uint32_t group) = 0;
    virtual bool SetInstance(uint32_t instance) = 0;
    virtual uint32_t GetRefCount() = 0;
};
