#include "hooker.h"
#include "rzfastalloc.h"
#include "rzstring.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>

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

    // cRZString class.
    Hook_Any(0x005039EE, cRZString::QueryInterface);
    Hook_Any(0x005D5780, cRZString::AddRef);
    Hook_Any(0x00503A10, cRZString::Release);
    Hook_Any(0x005008FE, cRZString::ToChar);
    // Hook_Any(0x0040FE7B, cRZString::Data); Optimised out as code is same for ToChar
    Hook_Any(0x00503B55, cRZString::Strlen);
    Hook_Any(0x00503AFC, cRZString::IsEmpty);
    Hook_Any(0x00503A2A, cRZString::Copy);
    Hook_Any(0x0050442B, cRZString::Resize);
    // Hook_Any(0x00503CFD, cRZString::Erase);
    Hook_Any(0x00503AAD, cRZString::Sprintf);
    Hook_Any(0x0050468C, cRZString::SprintfVaList);
    Hook_Any(0x00503AE8, cRZString::MakeLower);
    Hook_Any(0x00503AF2, cRZString::MakeUpper);
}
