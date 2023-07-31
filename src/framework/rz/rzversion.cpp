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
#include "rzversion.h"
#include "convertstring.h"
#include "rzapppath.h"
#include "rzlocale.h"
#include <cpuid.h>
#include <inttypes.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <powerbase.h>
#include <sysinfoapi.h>
#include <winnt.h>
#else
#ifdef __linux__
#include <sys/sysinfo.h>
#endif

#include <sys/utsname.h>
#include <limits.h>
#include <pwd.h>
#include <unistd.h>
#endif

namespace
{
#ifdef _WIN32
// MSDN says this definition is missing from winnt.h and should just be copied into code using it.
// https://docs.microsoft.com/en-gb/windows/win32/power/processor-power-information-str?redirectedfrom=MSDN
typedef struct _PROCESSOR_POWER_INFORMATION
{
    ULONG Number;
    ULONG MaxMhz;
    ULONG CurrentMhz;
    ULONG MhzLimit;
    ULONG MaxIdleState;
    ULONG CurrentIdleState;
} PROCESSOR_POWER_INFORMATION, *PPROCESSOR_POWER_INFORMATION;
typedef LONG NTSTATUS, *PNTSTATUS;
typedef NTSTATUS(WINAPI *RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
#define STATUS_SUCCESS (0x00000000)

RTL_OSVERSIONINFOW GetRealOSVersion()
{
    // ntdll.dll is always loaded into a process.
    HMODULE hMod = GetModuleHandleW(L"ntdll.dll");

    if (hMod) {
        RtlGetVersionPtr fxPtr = reinterpret_cast<RtlGetVersionPtr>(GetProcAddress(hMod, "RtlGetVersion"));

        if (fxPtr != nullptr) {
            RTL_OSVERSIONINFOW rovi = { 0 };
            rovi.dwOSVersionInfoSize = sizeof(rovi);

            if (STATUS_SUCCESS == fxPtr(&rovi)) {
                return rovi;
            }
        }
    }

    RTL_OSVERSIONINFOW rovi = { 0 };

    return rovi;
}
#endif

constexpr int kMebibyteDivisor = 1048576;
} // namespace

#ifdef GetUserName // Work around dumb windows API macros.
#undef GetUserName
#endif

size_t cRZVersion::mSystemMemoryTotalOverride;
size_t cRZVersion::mSystemMemoryFreeOverride;

cRZVersion::cRZVersion() : field_18(0), mpVersionInfo(nullptr), mpVersionBuffer(nullptr)
{
    RZGetCurrentAppPath(mzModulePath);
}

cRZVersion::cRZVersion(cRZString &module_path) :
    mzModulePath(module_path), field_18(0), mpVersionInfo(nullptr), mpVersionBuffer(nullptr)
{
}

cRZVersion::~cRZVersion()
{
    if (mpVersionInfo != nullptr) {
        delete static_cast<char *>(mpVersionInfo);
        mpVersionInfo = nullptr;
    }
}

bool cRZVersion::GetSystemVersion(cRZString &version)
{
#ifdef _WIN32
    RTL_OSVERSIONINFOW rovi = GetRealOSVersion();
    version.Sprintf("Windows %s %u.%u",
        rovi.dwPlatformId == VER_PLATFORM_WIN32_NT ? "NT" : "",
        rovi.dwMajorVersion,
        rovi.dwMinorVersion);
#else // Assumes posix
    struct utsname uts;
    uname(&uts);
    version.Sprintf("%s %d.%d.%d", uts.sysname, uts.release);
#endif
    return true;
}

void cRZVersion::GetRawCPUVersionData(uint32_t *raw_version)
{
    cpuid(raw_version, 1);
}

uint32_t cRZVersion::GetCPUSpeed()
{
#ifdef _WIN32
    // This differs from the original implementation that calculates the speed using QueryPerformanceCounter.
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    PROCESSOR_POWER_INFORMATION *ppi = new PROCESSOR_POWER_INFORMATION[sys_info.dwNumberOfProcessors];

    if (CallNtPowerInformation(ProcessorInformation, nullptr, 0, ppi, sizeof(*ppi) * sys_info.dwNumberOfProcessors)
        != STATUS_SUCCESS) {
        delete[] ppi;
        return 0;
    }

    uint32_t speed = uint32_t(ppi[0].CurrentMhz);
    delete[] ppi;

    return speed;
#else
#pragma message "CPU speed info not retrieved for this platform."
    return 2000; // Some arbitrary acceptable frequency, 2.0GHz is min for Spore which is last gzcom engine game.
#endif
}

uint32_t cRZVersion::GetCPUFlags()
{
    // Original checks for GenuineIntel string on leaf 0 and sets a global that isn't used if CPUID is available.
    uint32_t regs[4];
    cpuid(regs, 1);
    return regs[2];
}

bool cRZVersion::GetCPUName(cRZString &name)
{
    uint32_t regs[4];
    cpuid(regs, 0);
    
    char cpu_name[13] = { 0 };
    strncpy(cpu_name, reinterpret_cast<char *>(&regs[1]), 4);
    strncpy(cpu_name + 4, reinterpret_cast<char *>(&regs[3]), 4);
    strncpy(cpu_name + 8, reinterpret_cast<char *>(&regs[2]), 4);
    name = cpu_name[0] == '\0' ? "NotX86" : cpu_name;

    return true;
}

bool cRZVersion::GetSystemMemory(size_t &total, size_t &free)
{
    static constexpr uintmax_t kMaxSizeT = sizeof(size_t) == sizeof(uint32_t) ? UINT32_MAX : UINT64_MAX;

    if (mSystemMemoryTotalOverride != 0) {
        total = mSystemMemoryTotalOverride;
        free = mSystemMemoryFreeOverride;
    } else {
#if defined _WIN32
        MEMORYSTATUSEX statex;
        statex.dwLength = sizeof(statex);

        if (!GlobalMemoryStatusEx(&statex)) {
            return false; // Original uses GlobalMemoryStatus which just succeeds.
        }

        total = size_t(statex.ullTotalPhys / kMebibyteDivisor > kMaxSizeT ? kMaxSizeT : statex.ullTotalPhys / kMebibyteDivisor);
        free = size_t(statex.ullAvailPhys / kMebibyteDivisor > kMaxSizeT ? kMaxSizeT : statex.ullAvailPhys / kMebibyteDivisor);
#else // Assuming posix with a few gnu extensions?
        long page_size = sysconf(_SC_PAGE_SIZE);
        intmax_t tmp_total = sysconf(_SC_PHYS_PAGES);

        if (tmp_total != -1) {
            tmp_total *= page_size;
            total = tmp_total / kMebibyteDivisor > kMaxSizeT ? kMaxSizeT : tmp_total / kMebibyteDivisor;
        } else { // if we didn't get a valid return from the query pretend we have more than the min requirement
            total = 1024;
        }

        intmax_t tmp_free = sysconf(_SC_AVPHYS_PAGES);
        if (tmp_free != -1) {
            tmp_free *= page_size;
            free = tmp_free / kMebibyteDivisor > kMaxSizeT ? kMaxSizeT : tmp_free / kMebibyteDivisor;
        } else { // if we didn't get a valid return from the query pretend we have more than the min requirement
            free = 1024;
        }
#endif

        // Some rounding to the nearest 4 MiB?
        total = ((total - 1) | 3) + 1;
        free = ((free - 1) | 3) + 1;
    }

    return true;
}

bool cRZVersion::GetModulePath(cRZString &path)
{
    path = mzModulePath;
    return true;
}

bool cRZVersion::GetModuleName(cRZString &name)
{
    if (mpVersionInfo == nullptr) {
        LoadVersionInfo();

        if (mpVersionInfo == nullptr) {
            return false;
        }
    }

#ifdef _WIN32
    UINT len;
    void *info_buffer; 
    char sub_block[256];

    snprintf(sub_block,
        sizeof(sub_block),
        "\\StringFileInfo\\%04" PRIx16 "%04" PRIx16 "\\%s",
        static_cast<uint16_t *>(mpVersionBuffer)[0],
        static_cast<uint16_t *>(mpVersionBuffer)[1],
        "FileDescription");

    if (VerQueryValueA(mpVersionInfo, sub_block, &info_buffer, &len)) {
        name = static_cast<char *>(info_buffer);

        return true;
    }
#endif

    name.Erase(0, -1);

    return false;
}

bool cRZVersion::GetModuleVersion(cRZString &version)
{

    if (mpVersionInfo == nullptr) {
        LoadVersionInfo();

        if (mpVersionInfo == nullptr) {
            return false;
        }
    }

#ifdef _WIN32
    UINT len;
    void *info_buffer;
    char sub_block[256];

    snprintf(sub_block,
        sizeof(sub_block),
        "\\StringFileInfo\\%04" PRIx16 "%04" PRIx16 "\\%s",
        static_cast<uint16_t *>(mpVersionBuffer)[0],
        static_cast<uint16_t *>(mpVersionBuffer)[1],
        "FileVersion");

    if (VerQueryValueA(mpVersionInfo, sub_block, &info_buffer, &len)) {
        version = static_cast<char *>(info_buffer);

        return true;
    }
#endif

    version.Erase(0, -1);

    return false;
}

bool cRZVersion::GetModuleDebugVersion(cRZString &version)
{

    if (mpVersionInfo == nullptr) {
        LoadVersionInfo();

        if (mpVersionInfo == nullptr) {
            return false;
        }
    }

#ifdef _WIN32
    UINT len;
    void *info_buffer;
    char sub_block[256];

    snprintf(sub_block,
        sizeof(sub_block),
        "\\StringFileInfo\\%04" PRIx16 "%04" PRIx16 "\\%s",
        static_cast<uint16_t *>(mpVersionBuffer)[0],
        static_cast<uint16_t *>(mpVersionBuffer)[1],
        "Debug Version");

    if (VerQueryValueA(mpVersionInfo, sub_block, &info_buffer, &len)) {
        version = static_cast<char *>(info_buffer);

        return true;
    }
#endif

    version.Erase(0, -1);

    return false;
}

bool cRZVersion::GetUserName(cRZString &name, cRZString *def_val)
{
#ifdef _WIN32
    char buffer[512];
    DWORD buff_size = sizeof(buffer);

    if (GetUserNameA(buffer, &buff_size)) {
        name = buffer;
        return true;
    }
#else // Assume posix
    struct passwd *pw = getpwuid(geteuid());

    if (pw != nullptr) {
        name = pw->pw_name;
        return true;
    }
#endif
    if (def_val != nullptr) {
        name = *def_val;
    } else {
        name = "?";
    }

    return false;
}

bool cRZVersion::GetHostName(cRZString &name, cRZString *def_val)
{
#ifdef _WIN32
    char buffer[512];
    DWORD buff_size = sizeof(buffer);

    if (GetComputerNameA(buffer, &buff_size)) {
        name = buffer;
        return true;
    }
#else // Assume posix
    char buffer[HOST_NAME_MAX];

    if (gethostname(buffer, sizeof(buffer)) == 0) {
        name = buffer;
        return true;
    }
#endif
    if (def_val != nullptr) {
        name = *def_val;
    } else {
        name = "localhost";
    }

    return false;
}

void cRZVersion::SetSystemMemoryOverride(size_t total, size_t free)
{
    mSystemMemoryTotalOverride = total;
    mSystemMemoryFreeOverride = free;
}

void cRZVersion::LoadVersionInfo()
{
#ifdef _WIN32
    if (!mpVersionInfo) {
        cRZString path;
        DWORD handle;
        ConvertStringEncoding(mzModulePath, path, kSystemCodePage);
        DWORD size = GetFileVersionInfoSizeA(path.ToChar(), &handle);

        if (size != 0) {
            mpVersionInfo = new uint8_t[size];

            if (GetFileVersionInfoA(path.ToChar(), handle, size, mpVersionInfo)) {
                UINT len;

                if (!VerQueryValueA(mpVersionInfo, "\\VarFileInfo\\Translation", &mpVersionBuffer, &len)) {
                    delete[] static_cast<uint8_t *>(mpVersionInfo);
                    mpVersionInfo = nullptr;
                }
            }
        }
    }
#endif
}
