/**
 * @file
 *
 * @brief Implementation of the GZCOM library encapsulation class.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "gzcomlibrary.h"
#include "convertstring.h"
#include "rzcomdlldirector.h"
#include "rzlocale.h"
#include "unichar.h"

#ifdef MATCH_ABI
#include <windows.h>
#include <vector>
#else
#include <dlfcn.h>
#endif

static constexpr char kCOMDirectorFunctionName[] = "GZDllGetGZCOMDirector";

cGZCOMLibrary::cGZCOMLibrary(const cIGZString &library_path) :
    mbLoaded(false), mnRefCount(0), mpDirector(nullptr), mzLibraryPath(library_path), mHandle(nullptr)
{
}

bool cGZCOMLibrary::QueryInterface(uint32_t iid, void **obj)
{
    switch (iid) {
        case GZIID_cIGZCOMLibrary:
            *obj = static_cast<cIGZCOMLibrary *>(this);
            AddRef();
            return true;
        case GZIID_cIGZUnknown:
            *obj = static_cast<cIGZUnknown *>(this);
            AddRef();
            return true;
        default:
            break;
    }

    return false;
}

uint32_t cGZCOMLibrary::AddRef()
{
    return ++mnRefCount;
}

uint32_t cGZCOMLibrary::Release()
{
    return --mnRefCount;
}

void cGZCOMLibrary::GetPath(cIGZString &path) const
{
    path.Copy(mzLibraryPath);
}

cIGZCOMDirector *cGZCOMLibrary::GetDirector() const
{
    return mpDirector;
}

tGZLibraryHandle cGZCOMLibrary::GetHandle() const
{
    return mHandle;
}

bool cGZCOMLibrary::IsLoaded() const
{
    return mbLoaded;
}

bool cGZCOMLibrary::operator<(const cGZCOMLibrary &rhs) const
{
    cRZString rhs_path;
    rhs.GetPath(rhs_path);

    return mzLibraryPath < rhs_path;
}

bool cGZCOMLibrary::Load()
{
    if (!mbLoaded) {
        cRZString systemcp_path;

#if MATCH_ABI // Probably not strictly needed as the dlopen wrapper returns the output from LoadLibrary on windows anyhow.
        ConvertStringEncoding(mzLibraryPath, systemcp_path, kSystemCodePage);

        if (GetVersion() & 0x80000000) {
            mHandle = LoadLibraryA(systemcp_path.ToChar());
        } else {
            uint32_t utf16_length = mzLibraryPath.size();
            std::vector<unichar_t> utf16_path_buff(utf16_length + 1);
            ConvertStringEncoding(mzLibraryPath, &utf16_path_buff[0], utf16_length, kUTF16CodePage);
            utf16_path_buff[utf16_length] = U_CHAR('\0');
            mHandle = LoadLibraryW(&utf16_path_buff[0]);
        }

        if (mHandle == nullptr) {
#if 0 // The original does this but does nothing with the generated string?
            cRZString error_msg;
            char *win32_format_msg = nullptr;

            FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr,
                GetLastError(),
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPSTR)&win32_format_msg,
                0,
                nullptr);

            if (win32_format_msg != nullptr) {
                error_msg.Sprintf("Failed to load the requested dll \"%s\"\nWindows error message - \"%s\"",
                    systemcp_path.ToChar(),
                    win32_format_msg);
                LocalFree(win32_format_msg);
            }
#endif
            return false;
        }

#else
        systemcp_path = mzLibraryPath; // Internal encoding should be utf8 for all paths.
        mHandle = dlopen(systemcp_path.ToChar(), RTLD_NOW);

        if (mHandle == nullptr) {
            return false;
        }
#endif

        mbLoaded = true;
        typedef cIGZCOMDirector *(*tCOMDllFunc)();
        tCOMDllFunc DllGetCOMDirectorFunc = nullptr;

#ifdef MATCH_ABI
        DllGetCOMDirectorFunc = tCOMDllFunc(GetProcAddress(static_cast<HINSTANCE>(mHandle), kCOMDirectorFunctionName));
#else
        DllGetCOMDirectorFunc = tCOMDllFunc(dlsym(mHandle, kCOMDirectorFunctionName));
#endif
        if (DllGetCOMDirectorFunc == nullptr) {
            Free();

            return false;
        }

        cIGZCOMDirector *const director = DllGetCOMDirectorFunc();

        if (director == nullptr) {
#if 0 // More code unused in final builds? Missing debug logging in release?
            cRZString error_msg;
            error_msg.Sprintf(
                "cGZCOMLibrary::Load: Failed to aquire GZCOM director from library: \"%s\"\n", mzLibraryPath.ToChar());
#endif
            Free();

            return false;
        }

        mpDirector = director;

        if (!director->InitializeCOM(GZCOM(), mzLibraryPath)) {
#if 0 // More code unused in final builds? Missing debug logging in release?
            cRZString error_msg;
            error_msg.Sprintf(
                "cGZCOMLibrary::Load: GZCOM Director failed initialization in library: \"%s\"\n", mzLibraryPath.ToChar());
#endif
            Free();

            return false;
        }
    }

    return true;
}

bool cGZCOMLibrary::Free()
{
    if (mbLoaded) {
        if (mpDirector != nullptr) {
            mpDirector = nullptr;
        }

#ifdef MATCH_ABI
        FreeLibrary(static_cast<HINSTANCE>(mHandle));
#else
        dlclose(mHandle);
#endif
        mHandle = nullptr;
        mbLoaded = false;
    }

    return true;
}
