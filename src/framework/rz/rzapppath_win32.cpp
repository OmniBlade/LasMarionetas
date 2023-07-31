/**
 * @file
 *
 * @brief Implements a function to retrieve the applications canonical path in the file system.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "convertstring.h"
#include "rzapppath.h"
#include "rzlocale.h"

#include <windows.h>
#include <vector>

#ifndef NTFS_MAX_PATH
#define NTFS_MAX_PATH 32768
#endif

uint32_t RZGetCurrentAppPath(cIGZString &path)
{
    static bool _initialized = false;
    static DWORD(__stdcall * GetModuleFileNameWPtr)(HMODULE, LPWSTR, DWORD) = nullptr;

    // This should not fail on anything newer than Win XP.
    if (!_initialized) {
        HMODULE handle = GetModuleHandleA("Kernel32.dll");

        if (handle != nullptr) {
            GetModuleFileNameWPtr = (DWORD(__stdcall *)(HMODULE, LPWSTR, DWORD))GetProcAddress(handle, "GetModuleFileNameW");
        }

        _initialized = true;
    }

    // Original code uses MAX_PATH here which may not hold on newer systems.
    if (GetModuleFileNameWPtr != nullptr) {
        std::vector<wchar_t> wide_path(NTFS_MAX_PATH);
        DWORD len = GetModuleFileNameWPtr(nullptr, &wide_path[0], NTFS_MAX_PATH);

        if (len != 0) {
            ConvertStringEncoding(&wide_path[0], len, kUTF16CodePage, path);
        }
    }

    if (path.Strlen() == 0) {
        char narrow_path[MAX_PATH];
        DWORD len = GetModuleFileNameA(nullptr, narrow_path, MAX_PATH);

        if (len != 0) {
            ConvertStringEncoding(narrow_path, len, kSystemCodePage, path);
        }
    }

    return path.Strlen();
}
