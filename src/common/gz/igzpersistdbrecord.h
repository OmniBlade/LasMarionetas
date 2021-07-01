/**
 * @file
 *
 * @brief Interface for persistant resource object database record.
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

static constexpr uint32_t GZIID_cIGZPersistDBRecord = 0xE56B8F03;

class cGZPersistResourceKey;

class cIGZPersistDBRecord : public cIGZUnknown
{
public:
    virtual const cGZPersistResourceKey &GetKey() const = 0;
    virtual uint32_t SetKey(cGZPersistResourceKey const &key) = 0;
    virtual uint32_t GetAccessFlags() const = 0; // Not 100% sure on this return type.
    virtual bool Close() = 0;
    virtual bool Abort() = 0;
};
