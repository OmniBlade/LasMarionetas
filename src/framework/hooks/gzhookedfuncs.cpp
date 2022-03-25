#include "hooker.h"
#include "rzcriticalsection.h"
#include "rzfastalloc.h"
#include "rzstring.h"
#include <new>
#include <stdio.h>

void *crt_new(size_t n)
{
    return ::operator new(n);
}

void crt_del(void *p)
{
    ::operator delete(p);
}

void SetupCommonHooks()
{
    // We take control of basic CRT functions here that rely on state within the runtime.
    // This lets us use these functions to work with data returned from the originals and vice versa
    // without having to control all sites the that data is handled.
    Hook_Function(Pick_Address(0x00E5F174, 0x0099A5E9, 0x006E62C4), free);
    Hook_Function(Pick_Address(0x00E5F28C, 0x0099A701, 0x006E62B2), malloc);
    Hook_Function(Pick_Address(0x00E60E16, 0x0099B5B7, 0x006E6AC2), calloc);
    Hook_Function(Pick_Address(0x00E61585, 0x0099A713, 0x006EA3A0), realloc);
    Hook_Function(Pick_Address(0x00E60F60, 0x0099B701, 0x006E8448), fseek);
    Hook_Function(Pick_Address(0x00E60CA4, 0x0099B273, 0x006E8080), fread);
    Hook_Function(Pick_Address(0x00E6131E, 0x0099CCEA, 0x006EA336), fopen);
    Hook_Function(Pick_Address(0x00E60FF5, 0x0099B796, 0x006E84DD), fclose);
    Hook_Function(Pick_Address(0x00E5BC0B, 0x009979CB, 0x006E497B), crt_new); // op new
    Hook_Function(Pick_Address(0x00F0516C, 0x009982F1, 0x006E5004), crt_del); // op delete
    Hook_Function(Pick_Address(0x00E63D42, 0x009A1F16, 0x006EBFA0), _msize);

    // Similarly these functions call into an allocator class we haven't reimplemented.
    // By hooking them with simple forwarding calls to the CRT allocators we avoid having to.
    Hook_Function(Pick_Address(0x0040E923, 0x00596B2A, 0x004FFE47), RZFastAlloc);
    Hook_Function(Pick_Address(0x0040E934, 0x00596B3B, 0x004FFE58), RZFastFree);
    Hook_Function(Pick_Address(0x0040E94A, 0x00596B51, 0x004FFE6E), RZFastRealloc);

    // cRZString class.
    Hook_Any(Pick_Address(0x0040CCA7, 0 /*0x0059E3C1*/, 0x005039EE), cRZString::QueryInterface);
    Hook_Any(Pick_Address(0x005B9B92, 0 /*0x0083EF7B*/, 0x005D5780), cRZString::AddRef);
    Hook_Any(Pick_Address(0x0040CCC9, 0 /*0x0059E3E3*/, 0x00503A10), cRZString::Release);
    Hook_Any(Pick_Address(0x0040FE7B, 0 /*0x0059E4CF*/, 0x005008FE), cRZString::ToChar);
    Hook_Any(Pick_Address(0x0040CFB2, 0 /*0x0059E577*/, 0x00503B55), cRZString::Strlen);
    Hook_Any(Pick_Address(0x0040CDCE, 0 /*0x0059E4D3*/, 0x00503AFC), cRZString::IsEmpty);
    Hook_Any(Pick_Address(0x0040CCE3, 0 /*0x0059E3FD*/, 0x00503A2A), cRZString::Copy);
    Hook_Any(Pick_Address(0x0040E07D, 0 /*0x0059F13E*/, 0x0050442B), cRZString::Resize);
    Hook_Any(Pick_Address(0x0040CD66, 0 /*0x0059E480*/, 0x00503AAD), cRZString::Sprintf);
    Hook_Any(Pick_Address(0x0040E2DE, 0 /*0x0059F39F*/, 0x0050468C), cRZString::SprintfVaList);
    Hook_Any(Pick_Address(0x0040CDBA, 0 /*0x0059E4BB*/, 0x00503AE8), cRZString::MakeLower);
    Hook_Any(Pick_Address(0x0040CDC4, 0 /*0x0059E4C5*/, 0x00503AF2), cRZString::MakeUpper);

    // cRZCriticalSection class.
    Hook_Any(Pick_Address(0x00417AC3, 0x005BCA8C, 0), cRZCriticalSection::Hookable_Ctor);
    Hook_Any(Pick_Address(0x00417A44, 0x005BCA09, 0x00519170), cRZCriticalSection::~cRZCriticalSection);
    Hook_Any(Pick_Address(0x00417A62, 0x005BCA27, 0x0051918E), cRZCriticalSection::Release);
    Hook_Any(Pick_Address(0x00417A77, 0x005BCA40, 0x005191A3), cRZCriticalSection::Lock);
    Hook_Any(Pick_Address(0x00417AAE, 0x005BCA77, 0x005191DA), cRZCriticalSection::Unlock);
    Hook_Any(Pick_Address(0x00417A8A, 0x005BCA53, 0x005191B6), cRZCriticalSection::TryLock);
}
