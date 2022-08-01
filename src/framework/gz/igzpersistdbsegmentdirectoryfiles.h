/**
 * @file
 *
 * @brief Interface for persistant resource object database segment.
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

static constexpr uint32_t GZIID_cIGZPersistDBSegmentDirectoryFiles = 0x856C8CF7;

class cGZPersistResourceKey;
class cIGZFileNameToResKeyConverter;
class cIGZPersistDBSegment;
class cIGZPersistRESIDtoExtensionMap;
class cIGZString;

class cIGZPersistDBSegmentDirectoryFiles : public cIGZUnknown
{
public:
    virtual cIGZPersistDBSegment *AsIDBSegment() = 0;
    virtual bool Init() = 0;
    virtual bool Shutdown() = 0;
    virtual bool Open(bool read, bool write) = 0;
    virtual bool Path() = 0;
    virtual bool GetPath(cIGZString &path) = 0;
    virtual bool SetPath(const cIGZString &path) = 0;
    virtual bool SetDefaultGroupId(uint32_t gid) = 0;
    virtual uint32_t GroupId() = 0;
    virtual bool ExtensionMap() = 0;
    virtual bool GetRESIDtoExtensionMap(cIGZPersistRESIDtoExtensionMap **) = 0;
    virtual bool AddExtensionAssociation(uint32_t, const cIGZString &extension) = 0;
    virtual bool SetAllowedIDPositions(bool, bool) = 0;
    virtual bool SetAllowTextAsKey(bool onoff) = 0;
    virtual bool GetFileNameFromKey(const cGZPersistResourceKey &key, cIGZString &name) = 0;
    virtual bool GetFilePathFromKey(const cGZPersistResourceKey &key, cIGZString &path) = 0;
    virtual bool GetKeyFromFileName(const cIGZString &, const cIGZString &, cGZPersistResourceKey &key) = 0;
    virtual bool IsFileNameValid(const cIGZString &name) = 0;
    /**
     * @brief Generates a new key.
     * @param type Type value to set on the new key.
     * @param key Key object to store new values into.
     * @return Was the key generated successfully?
     */
    virtual bool GenerateNewKey(uint32_t type, cGZPersistResourceKey &key) = 0;
    virtual uint32_t EnumerateRecordFiles(cIGZString **file_names, uint32_t max_files) = 0;
    virtual bool SetFileNameConverter(cIGZFileNameToResKeyConverter *cvrt) = 0;
    virtual cIGZFileNameToResKeyConverter *GetFileNameConverter() = 0;
    virtual bool GetFileChangeDetection() = 0;
    /**
     * @brief Enables/disables periodic file change detection.
     * @param onoff Set the file change detection on or off.
     * @param check_interval Minimum time in seconds between checks.
     * @return Was the file change detection state altered successfully?
     */
    virtual bool SetFileChangeDetection(bool onoff, uint32_t check_interval) = 0;
    /**
     * @return Have files changed?
     */
    virtual bool DoFileChangeCheck() = 0;
    /**
     * @brief Enables/disables promotion of legacy format DB segments.
     * @param onoff Set the promotion feature on or off.
     */
    virtual void EnableLegacyPromotions(bool onoff) = 0;
    /**
     * @brief Enables/disables demotion to legacy format DB segments.
     * @param onoff Set the demotion feature on or off.
     */
    virtual void EnableLegacyDemotions(bool onoff) = 0;
};
