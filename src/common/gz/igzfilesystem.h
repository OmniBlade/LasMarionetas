/**
 * @file
 *
 * @brief Interface querying the application filesystem.
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

static constexpr uint32_t GZIID_cIGZFileSystem = 0xE0203660;

class cIGZString;

class cIGZFileSystem : public cIGZUnknown
{
public:
    /**
     * @return Drive the directory containing the application resides on.
     */
    virtual const char *AppDrive() = 0;
    /**
     * @return Path to the directory containing the application.
     */
    virtual const char *AppDirectory() = 0;
    /**
     * @return Name of the application.
     */
    virtual const char *AppName() = 0;
    /**
     * @return Extension for the application.
     */
    virtual const char *AppExtension() = 0;
    /**
     * @return Path to the application data directory.
     */
    virtual const char *DataDirectory() = 0;
    /**
     * @return Path to the application binary plug-in directory.
     */
    virtual const char *PlugInDirectory() = 0;
    /**
     * @return Drive the CD directory resides on.
     */
    virtual const char *CDAppDrive() = 0;
    /**
     * @return Path to the CD directory.
     */
    virtual const char *CDAppDirectory() = 0;
    /**
     * @return Path to the CD data directory.
     */
    virtual const char *CDDataDirectory() = 0;
    /**
     * @return Path to the CD binary plug-in directory.
     */
    virtual const char *CDPlugInDirectory() = 0;
    /**
     * @brief Attempt to locate the CD drive for the application.
     * @return Was the CD drive successfully found.
     */
    virtual bool FindCDDrive() = 0;
    /**
     * @brief Set application to look for the configuration .ini file on a CD.
     * @param use_cd Should the CD ini file be used?
     */
    virtual void UseCDIniFile(bool use_cd) = 0;
    /**
     * @brief Set application to do recursive CD searches.
     * @param allow Should recursive searches be used?
     */
    virtual void AllowRecursiveCDSearch(bool allow) = 0;
    /**
     * @brief Sets the expected name of the CD.
     * @param name String containing the expected name.
     */
    virtual void SetExpectedCDName(const cIGZString &name) = 0;
    /**
     * @brief Sets the expected path of the CD.
     * @param path String containing the expected path.
     */
    virtual void SetExpectedCDAppPath(const cIGZString &path) = 0;
    /**
     * @brief Sets the application directory.
     * @param path String containing the path to treat as the application directory.
     */
    virtual void SetAppDirectory(const cIGZString &path) = 0;
    /**
     * @brief Sets the data directory.
     * @param path String containing the path to treat as the data directory.
     */
    virtual void SetDataDirectory(const cIGZString &path) = 0;
    /**
     * @brief Sets the binary plug in directory.
     * @param path String containing the path to treat as the plug in directory.
     */
    virtual void SetPlugInDirectory(const cIGZString &path) = 0;
    /**
     * @brief Gets the name of the binary plugin directory.
     * @param name String to hold the name.
     */
    virtual void GetPluginDirectoryName(cIGZString &name) = 0;
    /**
     * @brief Sets the name of the binary plugin directory.
     * @param name String containing the name.
     */
    virtual void SetPluginDirectoryName(const cIGZString &name) = 0;
};
