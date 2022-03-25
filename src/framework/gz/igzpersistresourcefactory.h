/**
 * @file
 *
 * @brief Interface for persistant resource object factories.
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

static constexpr uint32_t GZIID_cIGZPersistResourceFactory = 0xA56B8F17;

class cIGZPersistDBRecord;
class cIGZPersistResource;

class cIGZPersistResourceFactory : public cIGZUnknown
{
public:
    virtual bool CreateInstance(uint32_t clsid, uint32_t iid, void **dst, uint32_t, cIGZUnknown *) = 0;
    virtual bool CreateInstance(cIGZPersistDBRecord &db_record, uint32_t iid, void **dst, uint32_t, cIGZUnknown *) = 0;
    /**
     * @brief Reads a resource from a DB record.
     * @param dst cIGZPersistResource based object recieving the data.
     * @param db_record cIGZPersistDBRecord based object providing the data.
     * @return Was the data read successfully?
     */
    virtual bool Read(cIGZPersistResource &dst, cIGZPersistDBRecord &db_record) = 0;
    /**
     * @brief Writes a resource to a DB record.
     * @param src cIGZPersistResource based object providing the data.
     * @param db_record cIGZPersistDBRecord based object recieving the data.
     * @return Was the data written successfully?
     */
    virtual bool Write(const cIGZPersistResource &src, cIGZPersistDBRecord &db_record) = 0;
};
