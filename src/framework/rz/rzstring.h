/**
 * @file
 *
 * @brief An implementation of cIGZString based around std::string.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once
#include "igzstring.h"
#include "rzfastalloc.h"
#include <string>

/**
 * @brief An implementation of cIGZString based around std::string
 *
 * This class currently implements the complete virtual interface and should be abi compatable with
 * the games internal functions as well. Additional class specific functionality is partial and will
 * be implemented as required.
 */
class cRZString : public cIGZString, public std::basic_string<char, std::char_traits<char>, cRZFastSTLAllocator<char>>
{
    // std::string using a custom allocator.
    typedef std::basic_string<char, std::char_traits<char>, cRZFastSTLAllocator<char>> string_data;

public:
    cRZString();
    cRZString(const string_data &source);
    cRZString(const cRZString &source, size_t start = 0, size_t end = string_data::npos);
    cRZString(const char *source);
    cRZString(const char *source, size_t length);
    cRZString(const cIGZString &source);
    cRZString(char character, size_t repetitions);

    virtual ~cRZString() {}

    virtual bool QueryInterface(uint32_t riid, void **obj) override;
    virtual uint32_t AddRef() override;
    virtual uint32_t Release() override;

    virtual void FromChar(const char *source) override;
    virtual void FromChar(const char *source, uint32_t length) override;
    virtual const char *ToChar() const override;
    virtual const char *Data() const override;

    virtual uint32_t Strlen() const override;
    virtual bool IsEmpty() const override;

    virtual bool IsEqual(const cIGZString *other, bool case_sensitive) const override;
    virtual bool IsEqual(const cIGZString &other, bool case_sensitive) const override;
    virtual bool IsEqual(const char *other, bool case_sensitive) const override;
    virtual bool IsEqual(const char *other, uint32_t length, bool case_sensitive) const override;

    virtual int32_t CompareTo(const cIGZString &other, bool case_sensitive) const override;
    virtual int32_t CompareTo(const char *other, bool case_sensitive) const override;
    virtual int32_t CompareTo(const char *other, uint32_t length, bool case_sensitive) const override;

    virtual cIGZString &operator=(const cIGZString &other) override;

    virtual void Copy(const cIGZString &other) override;
    virtual void Resize(uint32_t new_size) override;

    virtual cIGZString *Append(const char *other) override;
    virtual cIGZString *Append(const char *other, uint32_t length) override;
    virtual cIGZString *Append(const cIGZString &other) override;
    virtual cIGZString *Insert(uint32_t position, const char *other, uint32_t length) override;
    virtual cIGZString *Insert(uint32_t position, const cIGZString &other) override;
    virtual cIGZString *Replace(uint32_t start_position, const char *other, uint32_t length) override;
    virtual cIGZString *Replace(uint32_t start_position, const cIGZString &other) override;
    virtual cIGZString *Erase(uint32_t start_position, uint32_t end_position) override;

    virtual int32_t Find(const char *other, uint32_t position, bool case_sensitive) const override;
    virtual int32_t Find(const cIGZString &other, uint32_t position, bool case_sensitive) const override;
    virtual int32_t RFind(const char *other, uint32_t position, bool case_sensitive) const override;
    virtual int32_t RFind(const cIGZString &other, uint32_t position, bool case_sensitive) const override;

    virtual cIGZString *Sprintf(const char *format, ...) override;
    virtual cIGZString *SprintfVaList(const char *format, va_list arg_list) override;

    void MakeLower();
    void MakeUpper();

    void LTrim();
    void RTrim();
    void Trim();

    void LTrim(const char *pattern) { erase(0, find_first_not_of(pattern)); }
    void RTrim(const char *pattern) { erase(find_last_not_of(pattern) + 1); }
    void Trim(const char *pattern)
    {
        LTrim(pattern);
        RTrim(pattern);
    }

    cRZString &operator=(const cRZString &that)
    {
        if (this != &that) {
            static_cast<string_data &>(*this) = static_cast<const string_data &>(that);
        }

        return *this;
    }

private:
    uint32_t mnRefCount;
};
