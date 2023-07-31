/**
 * @file
 *
 * @brief Class for querying information about the system and OS.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once

#include "rzstring.h"

class cRZVersion
{
public:
    cRZVersion();
    cRZVersion(cRZString &module_path);
    virtual ~cRZVersion();

    /**
     * @brief Retrives the OS name and version as a string.
     * @param version String object to hold the version string.
     * @return Was the version retrieved successfully?
     */
    bool GetSystemVersion(cRZString &version);
    /**
     * @brief Retrives the cpuid leaf 1.
     * @param raw_version Pointer to an array of 32bit ints at least 4 in size.
     */
    void GetRawCPUVersionData(uint32_t *raw_version);
    /**
     * @return CPU speed in Mhz.
     */
    uint32_t GetCPUSpeed();
    /**
     * @return Flags from leaf 1 of cpuid, 0 on none x86.
     */
    uint32_t GetCPUFlags();
    /**
     * @brief Retrieves the CPU make string if available.
     * @param name String object to hold the retrieved string.
     * @return Was the string retrieved?
     */
    bool GetCPUName(cRZString &name);
    /**
     * @brief Retrieves the total and free physical memory available in MiB.
     * @param total Reference to an integer to hold the total size.
     * @param free Reference to an integer to hold the free size.
     * @return Was the available memory retrieved.
     */
    bool GetSystemMemory(size_t &total, size_t &free);
    /**
     * @brief Retrieves the path to the executable.
     * @param path Reference to a cRZString object to hold the path.
     * @return Was the path retrieved.
     */
    bool GetModulePath(cRZString &path);
    /**
     * @brief Retrieves the name of the executable.
     * @param name Reference to a cRZString object to hold the name.
     * @return Was the name retrieved.
     */
    bool GetModuleName(cRZString &name);
    /**
     * @brief Retrieves the version info embeded in the executable.
     * @param version Reference to a cRZString object to hold the version.
     * @return Was the version retrieved.
     */
    bool GetModuleVersion(cRZString &version);
    /**
     * @brief Retrieves the debug version info embeded in the executable.
     * @param version Reference to a cRZString object to hold the version.
     * @return Was the version retrieved.
     */
    bool GetModuleDebugVersion(cRZString &version);
    /**
     * @brief Retrieves the username of the user who ran the executable.
     * @param name Reference to a cRZString object to hold the name.
     * @param default Pointer to a cRZString object to holding a default name.
     * @return Was the name retrieved.
     */
    bool GetUserName(cRZString &name, cRZString *def_val =  nullptr);
    /**
     * @brief Retrieves the name of the host the executable is running on.
     * @param name Reference to a cRZString object to hold the name.
     * @param default Pointer to a cRZString object to holding a default name.
     * @return Was the name retrieved.
     */
    bool GetHostName(cRZString &name, cRZString *def_val =  nullptr);

    static void SetSystemMemoryOverride(size_t total, size_t free);

private:
    void LoadVersionInfo();

private:
    cRZString mzModulePath;
    int32_t field_18;
    void *mpVersionInfo;
    void *mpVersionBuffer;

    static size_t mSystemMemoryTotalOverride;
    static size_t mSystemMemoryFreeOverride;
};
