/**
 * @file
 *
 * @brief Command Line parsing class.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once

#include "igzcmdline.h"
#include "rzstring.h"
#include <vector>

class cRZCmdLine : public cIGZCmdLine
{
public:
    cRZCmdLine(const char *cmdline);
    cRZCmdLine(int argc, char **argv);
    cRZCmdLine(const cIGZCmdLine &that);

    virtual ~cRZCmdLine() {}
    virtual const cRZString &operator[](int32_t index) const;

    /**
     * @brief Casts the object to the interface specified by riid
     * @param riid The GUID for the interface type to cast to
     * @param ppvObj A pointer to a void pointer to store the result in
     * @return Whether the object could be successfully cast or not
     */
    virtual bool QueryInterface(uint32_t iid, void **obj) override;

    /**
     * @brief Adds a reference to this object
     * @return The new reference count for this object
     */
    virtual uint32_t AddRef() override;

    /**
     * @brief Removes a reference to this object
     * @return The new reference count for this object
     *
     * If the reference count is zero, the object should delete itself and
     * return zero unless it has pretty good reason not to, such as
     * refcounting for the object being handled by some other service.
     */
    virtual uint32_t Release() override;

    /**
     * @return Number of arguments passed on the command line.
     */
    virtual int32_t ArgC() const;
    /**
     * @brief Retrieves an argument from the command line.
     * @param index Index of the argument to retrieve.
     * @return C string pointer to the argument.
     */
    virtual const char *ArgV(int32_t index) const;
    /**
     * @brief Retrieves full text of the command line.
     * @param cmd_line cIGZString reference to object to use to hold the text.
     * @return If the command line was retrieved.
     */
    virtual bool GetCommandLineText(cIGZString &cmd_line) const;
    /**
     * @brief Finds an argument in the command line.
     * @param text Command line entry to find.
     * @param case_sensitive Should the search be case sensitive?
     * @return Index of the first matching argument.
     */
    virtual int32_t Find(const cIGZString &text, bool case_sensitive) const;
    /**
     * @brief Finds substring in an argument in the command line.
     * @param text Command line entry to find.
     * @param case_sensitive Should the search be case sensitive?
     * @return Index of the first matching argument.
     */
    virtual int32_t FindSubString(const cIGZString &text, bool case_sensitive) const;
    /**
     * @brief Checks if a switch is present in the command line.
     * @param short_switch Command line switch to check.
     * @param param cIGZString reference to hold the switch option.
     * @param trim_quotes Should the option if any have its quotes trimmed?
     * @return Is the switch present.
     */
    virtual bool IsSwitchPresent(char short_switch, cIGZString &param, bool trim_quotes) const;
    /**
     * @brief Checks if a switch is present in the command line.
     * @param long_switch Command line switch to check.
     * @return Is the switch present.
     */
    virtual bool IsSwitchPresent(const cIGZString &long_switch) const;
    /**
     * @brief Checks if a switch is present in the command line.
     * @param long_switch Command line switch to check.
     * @param param cIGZString reference to hold the switch option.
     * @param trim_quotes Should the option if any have its quotes trimmed?
     * @return Is the switch present.
     */
    virtual bool IsSwitchPresent(const cIGZString &long_switch, cIGZString &param, bool trim_quotes) const;
    /**
     * @brief Gets the index of a switch on the command line.
     * @param short_switch Command line switch to check.
     * @param start_index Index to search from.
     * @return Index of the switch, returns -1 if not found.
     */
    virtual int32_t GetIndexOfSwitch(char short_switch, int32_t start_index) const;
    /**
     * @brief Gets the index of a switch on the command line.
     * @param long_switch Command line switch to check.
     * @param start_index Index to search from.
     * @return Index of the switch, returns -1 if not found.
     */
    virtual int32_t GetIndexOfSwitch(const cIGZString &long_switch, int32_t start_index) const;
    /**
     * @brief Inserts an argument into the command line.
     * @param arg Argument to add.
     * @param index Index to add the argument at.
     * @return Was the argument added successfully?
     */
    virtual bool InsertArgument(const cIGZString &arg, int32_t index);
    /**
     * @brief Removes an argument from the command line.
     * @param index Index to erase the argument at.
     * @return Was the argument removed successfully?
     */
    virtual bool EraseArgument(int32_t index);

private:
    void ConvertStringArrayToString(const cRZString &string, std::vector<cRZString> &array);
    void ConvertStringArrayToString(const std::vector<cRZString> &array, cRZString &string);

private:
    int32_t mnRefCount;
    std::vector<cRZString> mArguments;
    cRZString mzCmdLineText;
    cRZString mzDefaultArgument;
};
