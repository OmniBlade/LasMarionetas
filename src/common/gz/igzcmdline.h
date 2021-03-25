/**
 * @file
 *
 * @brief Command Line parsing interface.
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

static constexpr uint32_t GZIID_cIGZCmdLine = 0xA31E539C;

class cIGZString;

class cIGZCmdLine : public cIGZUnknown
{
public:
    /**
     * @return Number of arguments passed on the command line.
     */
    virtual int32_t ArgC() const = 0;
    /**
     * @brief Retrieves an argument from the command line.
     * @param index Index of the argument to retrieve.
     * @return C string pointer to the argument.
     */
    virtual const char *ArgV(int32_t index) const = 0;
    /**
     * @brief Retrieves full text of the command line.
     * @param cmd_line cIGZString reference to object to use to hold the text.
     * @return If the command line was retrieved.
     */
    virtual bool GetCommandLineText(cIGZString &cmd_line) const = 0;
    /**
     * @brief Finds an argument in the command line.
     * @param text Command line entry to find.
     * @param case_sensitive Should the search be case sensitive?
     * @return Index of the first matching argument.
     */
    virtual int32_t Find(const cIGZString &text, bool case_sensitive) const = 0;
    /**
     * @brief Finds substring in an argument in the command line.
     * @param text Command line entry to find.
     * @param case_sensitive Should the search be case sensitive?
     * @return Index of the first matching argument.
     */
    virtual int32_t FindSubString(const cIGZString &text, bool case_sensitive) const = 0;
    /**
     * @brief Checks if a switch is present in the command line.
     * @param short_switch Command line switch to check.
     * @param param cIGZString reference to hold the switch option.
     * @param trim_quotes Should the option if any have its quotes trimmed?
     * @return Is the switch present.
     */
    virtual bool IsSwitchPresent(char short_switch, cIGZString &param, bool trim_quotes) const = 0;
    /**
     * @brief Checks if a switch is present in the command line.
     * @param long_switch Command line switch to check.
     * @return Is the switch present.
     */
    virtual bool IsSwitchPresent(const cIGZString &long_switch) const = 0;
    /**
     * @brief Checks if a switch is present in the command line.
     * @param long_switch Command line switch to check.
     * @param param cIGZString reference to hold the switch option.
     * @param trim_quotes Should the option if any have its quotes trimmed?
     * @return Is the switch present.
     */
    virtual bool IsSwitchPresent(const cIGZString &long_switch, cIGZString &param, bool trim_quotes) const = 0;
    /**
     * @brief Gets the index of a switch on the command line.
     * @param short_switch Command line switch to check.
     * @param start_index Index to search from.
     * @return Index of the switch, returns -1 if not found.
     */
    virtual int32_t GetIndexOfSwitch(char short_switch, int32_t start_index) const = 0;
    /**
     * @brief Gets the index of a switch on the command line.
     * @param long_switch Command line switch to check.
     * @param start_index Index to search from.
     * @return Index of the switch, returns -1 if not found.
     */
    virtual int32_t GetIndexOfSwitch(const cIGZString &long_switch, int32_t start_index) const = 0;
    /**
     * @brief Inserts an argument into the command line.
     * @param arg Argument to add.
     * @param index Index to add the argument at.
     * @return Was the argument added successfully?
     */
    virtual bool InsertArgument(const cIGZString &arg, int32_t index) = 0;
    /**
     * @brief Removes an argument from the command line.
     * @param index Index to erase the argument at.
     * @return Was the argument removed successfully?
     */
    virtual bool EraseArgument(int32_t index) = 0;
};
