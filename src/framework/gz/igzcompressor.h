/**
 * @file
 *
 * @brief Interface for a compressor/decompressor object.
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

static constexpr uint32_t GZIID_cIGZCompressor = 0x8359908E;

class cIGZString;

/**
 * @brief An interface for data compression.
 */
class cIGZCompressor : public cIGZUnknown
{
public:
    /**
     * @brief Compress arbitrary data.
     * @param src Pointer to the data to compress.
     * @param src_length Length of the data to compress.
     * @param dst Pointer to a buffer to hold the compressed results.
     * @param dst_length Reference to a variable to hold the length of the compressed data.
     * @return Was the data compressed successfully.
     */
    virtual bool CompressData(const void *src, uint32_t src_length, uint8_t *dst, uint32_t &dst_length) = 0;
    /**
     * @brief Decompress data.
     * @param src Pointer to the data to decompress.
     * @param src_length Length of the data to decompress.
     * @param dst Pointer to a buffer to hold the decompressed results.
     * @param dst_length Reference to a variable to hold the length of the decompressed data.
     * @return Was the data decompressed successfully.
     */
    virtual bool DecompressData(const uint8_t *src, uint32_t src_length, void *dst, uint32_t, uint32_t &dst_length) = 0;
    /**
     * @param uncompressed_size Length of data to get max size for.
     * @return Worst case size of the compressed data.
     */
    virtual uint32_t GetMaxLengthRequiredForCompressedData(uint32_t uncompressed_size) = 0;
    /**
     * @param data Pointer to compressed data.
     * @return Length of the compressed data.
     */
    virtual uint32_t GetLengthOfCompressedData(const uint8_t *data) = 0;
    /**
     * @param data Pointer to compressed data.
     * @return Length of the uncompressed data.
     */
    virtual uint32_t GetLengthOfDecompressedData(const uint8_t *data) = 0;
    /**
     * @brief Get a string indicating the version of the compression algorithm implmented.
     * @param string String reference to receive the version string.
     */
    virtual void GetCompressionVersionString(cIGZString &string) = 0;
};
