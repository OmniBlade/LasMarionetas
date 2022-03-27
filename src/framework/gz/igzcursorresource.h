/**
 * @file
 *
 * @brief Interface for a cursor resource.
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

static constexpr uint32_t GZIID_cIGZCursorResource = 0x88696706;

class cIGZCursor;
class cIGZPersistDBSerialRecord;

class cIGZCursorResource : public cIGZUnknown
{
public:
    /**
     * @brief Sets the cursor for this resource.
     * @param cursor Pointer cIGZCursor object to set as cursor.
     * @return Was the cursor set?
     */
    virtual bool SetCursor(cIGZCursor *cursor) = 0;
    /**
     * @brief Gets the cursor from this resource.
     * @param cursor Pointer to a pointer to hold the retrieved cIGZCursor object.
     * @return Was the cursor retrieved?
     */
    virtual bool GetCursor(cIGZCursor **cursor) = 0;
    /**
     * @brief Gets the cursor from this resource.
     * @param iid Interface ID to treat the retrieved cursor as.
     * @param obj Pointer to a pointer to hold the retrieved object.
     * @return Was the cursor retrieved?
     */
    virtual bool GetCursorAs(uint32_t iid, void **obj) = 0;
    /**
     * @brief Loads a cursor from a PersistDB record.
     * @param db_record Pointer to a record to retrieve the resource from.
     * @return Was the resource loaded successfully.
     */
    virtual bool LoadFromSerialRecord(cIGZPersistDBSerialRecord *db_record) = 0;
};
