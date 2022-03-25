#include "unistd.h"
#include <stdarg.h>
#include <stdlib.h>

int __cdecl minipos_chdir(char const *_Path)
{
    int size = MultiByteToWideChar(CP_UTF8, 0, _Path, -1, 0, 0);
    wchar_t *buffer = malloc(size * sizeof(wchar_t));
    int ret = -1;

    if (buffer) {
        MultiByteToWideChar(CP_UTF8, 0, _Path, -1, buffer, size);
        ret = _wchdir(buffer);
        free(buffer);
    }

    return ret;
}

int __cdecl minipos_rmdir(char const *_Path)
{
    int size = MultiByteToWideChar(CP_UTF8, 0, _Path, -1, 0, 0);
    wchar_t *buffer = malloc(size * sizeof(wchar_t));
    int ret = -1;

    if (buffer) {
        MultiByteToWideChar(CP_UTF8, 0, _Path, -1, buffer, size);
        ret = _wrmdir(buffer);
        free(buffer);
    }

    return ret;
}

int __cdecl minipos_unlink(char const *_FileName)
{
    int size = MultiByteToWideChar(CP_UTF8, 0, _FileName, -1, 0, 0);
    wchar_t *buffer = malloc(size * sizeof(wchar_t));
    int ret = -1;

    if (buffer) {
        MultiByteToWideChar(CP_UTF8, 0, _FileName, -1, buffer, size);
        ret = _wunlink(buffer);
        free(buffer);
    }

    return ret;
}

int __cdecl minipos_open(char const *_FileName, int _OpenFlag, ...)
{
    va_list ap;
    int size = MultiByteToWideChar(CP_UTF8, 0, _FileName, -1, 0, 0);
    wchar_t *buffer = malloc(size * sizeof(wchar_t));
    int ret = -1;
    int opt_arg = -1;

    va_start(ap, _OpenFlag);
    opt_arg = va_arg(ap, int);
    va_end(ap);

    if (buffer) {
        MultiByteToWideChar(CP_UTF8, 0, _FileName, -1, buffer, size);

        if (opt_arg >= 0) {
            ret = _wopen(buffer, _OpenFlag, opt_arg);
        } else {
            ret = _wopen(buffer, _OpenFlag);
        }

        free(buffer);
    }

    return ret;
}
