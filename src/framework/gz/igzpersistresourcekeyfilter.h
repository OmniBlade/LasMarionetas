/**
 * @file
 *
 * @brief Interface for filtering resources by key.
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

static constexpr uint32_t GZIID_cIGZPersistResourceKeyFilter = 0x8865CD89;

class cGZPersistResourceKey;

class cIGZPersistResourceKeyFilter : public cIGZUnknown
{
public: 
    /**
     * @param key cGZPersistResourceKey to filter against.
     * @return Is the provided key included in this filter?
     */
    virtual bool IsKeyIncluded(cGZPersistResourceKey const &key) = 0;
};
