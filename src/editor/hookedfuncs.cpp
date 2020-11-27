#include "hooker.h"
#include "rzfastalloc.h"
#include "rzstring.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>

// This global controls which address is used when needed inline in code.
const BinaryType geBinaryMode = BINARY_EDITOR;

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

    // RZ Allocator wrappers.
    Hook_Function(0x00596B2A, RZFastAlloc);
    Hook_Function(0x00596B3B, RZFastFree);
    Hook_Function(0x00596B51, RZFastRealloc);
#if 0
    // cRZString class.
    Hook_Any(0x0059E3C1, cRZString::QueryInterface);
    Hook_Any(0x0083EF7B, cRZString::AddRef);
    Hook_Any(0x0059E3E3, cRZString::Release);
    Hook_Any(0x0059E4CF, cRZString::ToChar);
    // Hook_Any(0x0040FE7B, cRZString::Data); Optimised out as code is same for ToChar
    Hook_Any(0x0059E577, cRZString::Strlen);
    Hook_Any(0x0059E4D3, cRZString::IsEmpty);
    Hook_Any(0x0059E3FD, cRZString::Copy);
    Hook_Any(0x0059F13E, cRZString::Resize);
    Hook_Any(0x0059E761, cRZString::Erase);
    Hook_Any(0x0059E480, cRZString::Sprintf);
    Hook_Any(0x0059F39F, cRZString::SprintfVaList);
    Hook_Any(0x0059E4BB, cRZString::MakeLower);
    Hook_Any(0x0059E4C5, cRZString::MakeUpper);
#endif
}
