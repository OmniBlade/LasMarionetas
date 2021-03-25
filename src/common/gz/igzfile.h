/**
 * @file
 *
 * @brief Interface handling file IO.
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
#include <ctype.h>

static constexpr uint32_t GZIID_cIGZFile = 0xE0203645;

class cIGZString;

/**
 * @brief An interface for file IO.
 */
class cIGZFile : public cIGZUnknown
{
public:
    enum
    {
        kShareModeRead = 1,
        kShareModeWrite = 2,
        kCreateModeCreateAlways = 1,
        kCreateModeOpenExisting = 2,
        kCreateModeOpenAlways = 3,
        kCreateModeTruncateExisting = 4,
        kOpenModeRead = 1,
        kOpenWriteMode = 2,
        kSeekStart = 0,
        kSeekCurrent = 1,
        kSeekEnd = 2,
    };

    /**
     * @brief Opens a file for IO.
     * @param open_mode IO mode to open the file, any bitwise combination of cIGZFile::kOpenMode* constants.
     * @param create_mode How should existing files be handled any one of cIGZFile::kCreateMode* constants.
     * @param share_mode Share mode to set on the fileany bitwise combination of cIGZFile::kShareMode* constants.
     * @return If the file was opened successfully.
     */
    virtual bool Open(int32_t open_mode, int32_t create_mode, int32_t share_mode) = 0;
    /**
     * @return Is the file currently open.
     */
    virtual bool IsOpen() const = 0;
    /**
     * @brief Closes the file.
     */
    virtual bool Close() = 0;
    /**
     * @return The current byte position in the file.
     */
    virtual int32_t Position() = 0;
    /**
     * @return The total length of the current file.
     */
    virtual uint32_t Length() = 0;
    /**
     * @brief Sets the length of the file.
     * @return Was the file length set successfully.
     */
    virtual bool SetLength(uint32_t length) = 0;
    /**
     * @brief Seek to the start of the file.
     * @return New position in the file, should be the start.
     */
    virtual uint32_t SeekToBegin() = 0;
    /**
     * @brief Seek to the end of the file.
     * @return New position in the file, should be the end.
     */
    virtual uint32_t SeekToEnd() = 0;
    /**
     * @brief Seek to a position relative to the current position in the file.
     * @return New position in the file.
     */
    virtual uint32_t SeekToRelativePosition(int pos) = 0;
    /**
     * @brief Seek to an absolute position in the current file.
     * @return New position in the file.
     */
    virtual uint32_t SeekToPosition(uint32_t pos) = 0;
    /**
     * @brief Seek to a position in the current file.
     * @param pos Position to move to relative to whence.
     * @param whence Position to use as the start position for the seek. Any of cIGZFile::kSeek* constants
     * @return New position in the file.
     */
    virtual uint32_t Seek(int pos, int whence) = 0;
    /**
     * @brief Reads an arbitrary amount of data from a file.
     * @param dst Pointer to a buffer to recieve the data.
     * @param length The amount of data in bytes to read.
     * @return Was the read successful.
     */
    virtual bool Read(void *dst, uint32_t length) = 0;
    /**
     * @brief Reads an arbitrary amount of data from a file.
     * @param dst Pointer to a buffer to recieve the data.
     * @param length Reference to a variable holding the length to read. Will hold actual length read after return.
     * @return Was the read successful.
     */
    virtual bool ReadWithCount(void *dst, uint32_t &length) = 0;
    /**
     * @brief Writes an arbitrary amount of data to a file.
     * @param src Pointer to a buffer holding data to write.
     * @param length The amount of data in bytes to write.
     * @return Was the write successful.
     */
    virtual bool Write(const void *src, uint32_t length) = 0;
    /**
     * @brief Writes an arbitrary amount of data to a file.
     * @param src Pointer to a buffer holding data to write.
     * @param length Reference to a variable holding the length to write. Will hold actual length written after return.
     * @return Was the write successful.
     */
    virtual bool WriteWithCount(void *src, uint32_t &length) = 0;
    /**
     * @brief Flushes pending data to the underlying IO object.
     */
    virtual bool Flush() = 0;
    /**
     * @brief Flushes pending data to the file system.
     */
    virtual bool SysFlush() = 0;
    /**
     * @brief Deletes the file.
     */
    virtual bool Remove() = 0;
    /**
     * @brief Renames the file.
     */
    virtual bool Rename(const cIGZString *name) = 0;
    /**
     * @return Does the file exist in the file system.
     */
    virtual bool Exists() = 0;
    /**
     * @brief Copies the file to a new location.
     * @param dst Path to copy to.
     * @param overwrite_existing Should we allow overwriting a file if it exists at the destination path?
     * @return Did the copy succeed.
     */
    virtual bool Copy(const cIGZString *dst, bool overwite_existing) = 0;
    /**
     * @brief Copies the file to a new location.
     * @param dst File object set to the path to copy to.
     * @param overwrite_existing Should we allow overwriting a file if it exists at the destination path?
     * @return Did the copy succeed.
     */
    virtual bool Copy(const cIGZFile *dst, bool overwite_existing) = 0;
    /**
     * @brief Gets the path of the current file on the file system.
     * @param path String object to recieve the path.
     * @return Reference to the passed string object.
     */
    virtual const cIGZString &GetPath(cIGZString &path) const = 0;
    /**
     * @brief Gets the path of the current file on the file system.
     * @return Reference to the internal path string object.
     */
    virtual const cIGZString &GetPath() const = 0;
    /**
     * @brief Gets the path of the current file on the file system.
     * @return Returns null terminated C string pointer.
     */
    virtual const char *GetPathCStr() const = 0;
    /**
     * @brief Sets the path of the current file on the file system.
     * @param path String object containing the new path.
     * @return Reference to the internal path string object.
     */
    virtual const cIGZString &SetPath(const cIGZString &path) = 0;
    /**
     * @brief Sets the path of the current file on the file system.
     * @param path Null teminated C string holding new path.
     * @return Reference to the internal path string object.
     */
    virtual const cIGZString &SetPath(const char *path) = 0;
    /**
     * @brief Retrieves the modes that the file is set to.
     * @param open_mode IO mode to open the file, any bitwise combination of cIGZFile::kOpenMode* constants.
     * @param create_mode How should existing files be handled any one of cIGZFile::kCreateMode* constants.
     * @param share_mode Share mode to set on the fileany bitwise combination of cIGZFile::kShareMode* constants.
     * @return If the modes were retrieved successfully, false if the file isn't open.
     */
    virtual bool GetOpenModes(int32_t &open_mode, int32_t &create_mode, int32_t &share_mode) const = 0;
    /**
     * @brief Retrieves the current buffer sizes.
     * @param read_buffer Size of the buffer for reading data.
     * @param write_buffer Size of the buffer for writing data.
     */
    virtual void GetBuffering(uint32_t &read_buffer, uint32_t &write_buffer) const = 0;
    /**
     * @brief Sets the current buffer sizes.
     * @param read_buffer Size of the buffer for reading data.
     * @param write_buffer Size of the buffer for writing data.
     * @return Were the buffer sizes set?
     */
    virtual bool SetBuffering(uint32_t read_buffer, uint32_t write_buffer) = 0;
};
