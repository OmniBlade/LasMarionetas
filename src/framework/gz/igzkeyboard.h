/**
 * @file
 *
 * @brief Interface for tracking keyboard state.
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

class cIGZKeyboard
{
public:
    /**
     * @brief Clear the tracked state.
     */
    virtual void ClearKeyboard() = 0;
    /**
     * @param vkey Key to check tracked state of.
     * @return Is the key down.
     */
    virtual bool IsKeyDown(uint32_t vkey) = 0;
    /**
     * @param vkey Key to check tracked state of.
     * @param state Set of current modifier states.
     * @return Is the key down.
     */
    virtual bool IsKeyDown(uint32_t vkey, uint32_t state) = 0;
    /**
     * @param vkey Key to check current state of.
     * @return Is the key down.
     */
    virtual bool IsKeyDownNow(uint32_t vkey) = 0;
    /**
     * @param vkey Key to check current state of.
     * @return Is the key a modifier key.
     */
    virtual bool CharIsModifier(uint32_t vkey) = 0;
    /**
     * @return Current state of modifier keys.
     */
    virtual uint32_t GetCurrentModifierState() = 0;
    /**
     * @param vkey Key to check current state of.
     * @return Is the key toggled.
     */
    virtual bool IsToggleKeySet(uint32_t vkey) = 0;
    /**
     * @brief Tracks a key as being down.
     * @param vkey Key to process.
     */
    virtual void ProcessKeyDown(uint32_t vkey) = 0;
    /**
     * @brief Tracks a key as being up.
     * @param vkey Key to process.
     */
    virtual void ProcessKeyUp(uint32_t vkey) = 0;
};
