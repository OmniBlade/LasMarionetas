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

static constexpr uint32_t GZIID_cIGZPersistDBSegment = 0x656B8F0C;

class cGZPersistResourceKey;
class cIGZPersistDBRecord;
class cIGZPersistDBSegmentNamespace;
class cIGZPersistResourceKeyFilter;
class cIGZString;

template<typename T> class ivector;

class cIGZPersistDBSegment : public cIGZUnknown
{
    // TODO: Many of the returns in this class need confirming, particularly the "signedness" of the integer returns.
public:
    /**
     * @return Was the object initialised successfully?
     */
    virtual bool Init() = 0;
    /**
     * @return Was the object shutdown successfully?
     */
    virtual bool Shutdown() = 0;
    virtual bool Open(bool read, bool write) = 0;
    virtual bool AutoOpen(bool read, bool write) = 0;
    virtual bool IsOpen() = 0;
    virtual bool IsWritable() = 0;
    virtual bool Close() = 0;
    virtual bool Flush() = 0;
    virtual void GetPath(cIGZString &path) = 0;
    virtual bool SetPath(const cIGZString &path) = 0;
    /**
     * @return Was the object locked successfully?
     */
    virtual bool Lock() = 0;
    /**
     * @return Was the object unlocked successfully?
     */
    virtual bool Unlock() = 0;
    virtual uint32_t GetSegmentID() = 0;
    virtual void SetSegmentID(const uint32_t &id) = 0;
    virtual void SetSegmentCache(cIGZPersistDBSegment *db_segment) = 0;
    virtual int32_t GetRecordCount(cIGZPersistResourceKeyFilter *key_filter) = 0;
    virtual int32_t GetResourceKeyList(ivector<cGZPersistResourceKey> *list, cIGZPersistResourceKeyFilter *key_filter) = 0;
    virtual bool TestForRecord(const cGZPersistResourceKey &key) = 0;
    virtual uint32_t GetRecordSize(const cGZPersistResourceKey &key) = 0;
    virtual bool OpenRecord(const cGZPersistResourceKey &key, cIGZPersistDBRecord **db_record, uint32_t) = 0;
    virtual bool CreateNewRecord(const cGZPersistResourceKey &key, cIGZPersistDBRecord **db_record) = 0;
    virtual bool CloseRecord(cIGZPersistDBRecord *db_record) = 0;
    virtual bool AbortRecord(cIGZPersistDBRecord *db_record) = 0;
    virtual bool DeleteRecord(const cGZPersistResourceKey &key) = 0;
    virtual bool ReadRecord(const cGZPersistResourceKey &key, void *data, uint32_t &size) = 0;
    virtual bool WriteRecord(const cGZPersistResourceKey &key, void *data, uint32_t size) = 0;
    virtual uint32_t GetSegmentChangeID() = 0;
    virtual uint32_t GetRecordChangeID(const cGZPersistResourceKey &key, uint32_t &) = 0;
    virtual void SetNamespace(cIGZPersistDBSegmentNamespace *segment_namespace) = 0;
    virtual void GetNamespace(cIGZPersistDBSegmentNamespace **segment_namespace) = 0;
};
