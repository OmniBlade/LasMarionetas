/**
 * @file
 *
 * @brief Interface for cursor management.
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

static constexpr uint32_t GZIID_cIGZCursorManager = 0xA34C498A;

class cIGZCursor;
class cIGZString;

class cIGZCursorManager : public cIGZUnknown
{
public:
    /**
     * @return Was the cursor manager initialised successfully? Always true in known examples.
     */
    virtual bool Init() = 0;
    /**
     * @return Was the cursor manager shut down successfully? Always true in known examples.
     */
    virtual bool Shutdown() = 0;
    /**
     * @brief Add a cIGZCursor object to the manager with a given ID.
     * @param cursor Pointer to the object to add.
     * @param id ID number to access this cursor from the manager.
     * @return Was the cursor added successfully? Always true in known examples.
     */
    virtual bool AddCursor(cIGZCursor *cursor, uint32_t id) = 0;
    /**
     * @brief Remove a cIGZCursor object from the manager with a given ID.
     * @param id ID number of the cursor to remove.
     * @return Was the cursor removed successfully? False if ID not found.
     */
    virtual bool RemoveCursor(uint32_t id) = 0;
    /**
     * @brief Retrieve a cIGZCursor object from the manager with a given ID.
     * @param id ID number of the cursor to retrieve.
     * @return Pointer to the retrieved object. nullptr if ID not found.
     */
    virtual cIGZCursor *GetCursor(uint32_t id) = 0;
    /**
     * @brief Retrieve current cIGZCursor object from the manager.
     * @return Pointer to the retrieved object.
     */
    virtual cIGZCursor *GetCursor() = 0;
    /**
     * @brief Set the current cursor from ID.
     * @param id ID of the cursor to set as current.
     * @return Was the current cursor set successfully?
     */
    virtual bool SetCursor(uint32_t id) = 0;
    /**
     * @brief Set the current cursor from a cIGZCursor object.
     * @param cursor Object to set as current cursor.
     * @return Was the current cursor set successfully?
     */
    virtual bool SetCursor(cIGZCursor *cursor) = 0;
    /**
     * @brief Set the current cursor from ID and adds it to a stack.
     * @param id ID of the cursor to set as current.
     * @return Was the current cursor set successfully?
     */
    virtual bool PushAutoCursor(uint32_t id) = 0;
    /**
     * @brief Set the previous cursor in the stack back as current and removes the most recent.
     * @return Was the current cursor set successfully?
     */
    virtual bool PopAutoCursor() = 0;
    /**
     * @brief Add a cIGZCursor object to the manager with a given ID.
     * @param name String holding the name of the cursor to add.
     * @param id ID number to access this cursor from the manager.
     * @return Was the cursor added successfully?
     */
    virtual bool AddCursor(const cIGZString &name, uint32_t id) = 0;
    /**
     * @brief Force the cursor to be shown.
     */
    virtual void ShowCursor() = 0;
    /**
     * @brief Force the cursor to be hidden.
     */
    virtual void HideCursor() = 0;
    /**
     * @return Is the cursor currently shown?
     */
    virtual bool IsCursorVisible() = 0;
};