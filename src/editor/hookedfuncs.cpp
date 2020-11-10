#include "hooker.h"
#include <new>
#include <stdlib.h>
#include <stdio.h>

// This global controls which address is used when needed inline in code.
const BinaryType geBinaryMode = BINARY_EDITOR;

void *crt_new(size_t n)
{
    return ::operator new(n);
}

void crt_del(void* p)
{
    ::operator delete(p);
}

void SetupHooks()
{
    // all to current crt
    Hook_Function(0x0099A5E9, free);
    Hook_Function(0x0099A701, malloc);
    Hook_Function(0x0099B5B7, calloc);
    Hook_Function(0x0099A713, realloc);
    Hook_Function(0x0099B701, fseek);
    Hook_Function(0x0099B273, fread);
    Hook_Function(0x0099CCEA, fopen);
    Hook_Function(0x0099B796, fclose);
    Hook_Function(0x009979CB, crt_new); // op new
    Hook_Function(0x009982F1, crt_del); // op delete
    Hook_Function(0x009A1F16, _msize);
}
