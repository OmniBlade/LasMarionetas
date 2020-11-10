#include "hooker.h"
#include "rzfastalloc.h"
#include <new>
#include <stdlib.h>
#include <stdio.h>

// This global controls which address is used when needed inline in code.
const BinaryType geBinaryMode = BINARY_INSTALLER;

void *crt_new(size_t n)
{
    return ::operator new(n);
}

void crt_del(void *p)
{
    ::operator delete(p);
}

void SetupHooks()
{
    // all to current crt
    Hook_Function(0x006E62C4, free);
    Hook_Function(0x006E62B2, malloc);
    Hook_Function(0x006E6AC2, calloc);
    Hook_Function(0x006EA3A0, realloc);
    Hook_Function(0x006E8448, fseek);
    Hook_Function(0x006E8080, fread);
    Hook_Function(0x006EA336, fopen);
    Hook_Function(0x006E84DD, fclose);
    Hook_Function(0x006E497B, crt_new); // op new
    Hook_Function(0x006E5004, crt_del); // op delete
    Hook_Function(0x006EBFA0, _msize);

    // RZ Allocator wrappers.
    Hook_Function(0x004FFE47, RZFastAlloc);
    Hook_Function(0x004FFE58, RZFastFree);
    Hook_Function(0x004FFE6E, RZFastRealloc);
}
