#include "hooker.h"
#include "rzcriticalsection.h"
#include "rzfastalloc.h"
#include "rzstring.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>

// This global controls which address is used when needed inline in code.
const BinaryType geBinaryMode = BINARY_GAME;

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
    Hook_Function(0x00E5F174, free);
    Hook_Function(0x00E5F28C, malloc);
    Hook_Function(0x00E60E16, calloc);
    Hook_Function(0x00E61585, realloc);
    Hook_Function(0x00E60F60, fseek);
    Hook_Function(0x00E60CA4, fread);
    Hook_Function(0x00E6131E, fopen);
    Hook_Function(0x00E60FF5, fclose);
    Hook_Function(0x00E5BC0B, crt_new); // op new
    Hook_Function(0x00F0516C, crt_del); // op delete
    Hook_Function(0x00E63D42, _msize);

    // RZ Allocator wrappers.
    Hook_Function(0x0040E923, RZFastAlloc);
    Hook_Function(0x0040E934, RZFastFree);
    Hook_Function(0x0040E94A, RZFastRealloc);

    // cRZString class.
    Hook_Any(0x0040CCA7, cRZString::QueryInterface);
    Hook_Any(0x005B9B92, cRZString::AddRef);
    Hook_Any(0x0040CCC9, cRZString::Release);
    Hook_Any(0x0040FE7B, cRZString::ToChar);
    // Hook_Any(0x0040FE7B, cRZString::Data); Optimised out as code is same for ToChar
    Hook_Any(0x0040CFB2, cRZString::Strlen);
    Hook_Any(0x0040CDCE, cRZString::IsEmpty);
    Hook_Any(0x0040CCE3, cRZString::Copy);
    Hook_Any(0x0040E07D, cRZString::Resize);
    // Hook_Any(0x0040D2A1, cRZString::Erase);
    Hook_Any(0x0040CD66, cRZString::Sprintf);
    Hook_Any(0x0040E2DE, cRZString::SprintfVaList);
    Hook_Any(0x0040CDBA, cRZString::MakeLower);
    Hook_Any(0x0040CDC4, cRZString::MakeUpper);

    // cRZCriticalSection class.
    Hook_Any(0x00417AC3, cRZCriticalSection::Hookable_Ctor);
    Hook_Any(0x00417A44, cRZCriticalSection::~cRZCriticalSection);
    Hook_Any(0x00417A62, cRZCriticalSection::Release);
    Hook_Any(0x00417A77, cRZCriticalSection::Lock);
    Hook_Any(0x00417AAE, cRZCriticalSection::Unlock);
    Hook_Any(0x00417A8A, cRZCriticalSection::TryLock);
}
