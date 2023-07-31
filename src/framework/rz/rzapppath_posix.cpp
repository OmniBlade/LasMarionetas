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
#include "rzapppath.h"
#include <stdlib.h>
#include <unistd.h>
#include <vector>

#if defined(__linux__)
#include <linux/limits.h>
#else
#include <limits.h>
#endif

#if defined(__APPLE__)
#define _DARWIN_BETTER_REALPATH
#include <TargetConditionals.h>
#include <mach-o/dyld.h>
#elif defined(__DragonFly__) || defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__NetBSD__)
#include <sys/sysctl.h>
#endif

#if defined(__sun)
#define PROC_SELF_EXE "/proc/self/path/a.out"
#else
#define PROC_SELF_EXE "/proc/self/exe"
#endif

uint32_t RZGetCurrentAppPath(cIGZString &path)
{
    char *resolved = nullptr;
    std::vector<char> tmp(PATH_MAX);

#if defined(__linux__) || defined(__CYGWIN__) || defined(__sun)
    resolved = realpath(PROC_SELF_EXE, &tmp[0]);
#elif defined(__APPLE__)
    std::vector<char> tmp2(PATH_MAX);
    uint32_t size = PATH_MAX;

    if (_NSGetExecutablePath(&tmp2[0], &size) == -1) {
        tmp.Resize(size);

        if (_NSGetExecutablePath(&tmp2[0], &size)) {
            resolved = realpath(&tmp2[0], &tmp[0]);
        }
    }
#elif defined(__DragonFly__) || defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__NetBSD__)
#if defined(__NetBSD__)
    int mib[4] = { CTL_KERN, KERN_PROC_ARGS, -1, KERN_PROC_PATHNAME };
#else
    int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };
#endif
    std::vector<char> tmp2(PATH_MAX);
    size_t size = PATH_MAX;

    if (sysctl(mib, (u_int)(sizeof(mib) / sizeof(mib[0])), &tmp2[0], &size, NULL, 0) == 0) {
        resolved = realpath(&tmp2[0], tmp.Data());
    }
#endif

    if (resolved != nullptr) {
        path.FromChar(&tmp[0]);
    }

    return path.Strlen();
}
