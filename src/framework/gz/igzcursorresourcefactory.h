/**
 * @file
 *
 * @brief Interface for a cursor resource factory.
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

static constexpr uint32_t GZIID_cIGZCursorResourceFactory = 0x08696779;

class cIGZCursor;
class cIGZPersistDBSerialRecord;

class cIGZCursorResourceFactory : public cIGZUnknown
{
public:
    /**
     * @brief Loads a cursor from a PersistDB record.
     * @param cursor Pointer to a pointer to hold a cIGZCursor object once loaded.
     * @param db_record Pointer to a record to retrieve the resource from.
     * @return Was the resource loaded successfully.
     */
    virtual bool LoadFromSerialRecord(cIGZCursor **cursor, cIGZPersistDBSerialRecord *db_record) = 0;
};
