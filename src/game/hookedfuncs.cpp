#include "hooker.h"
#include "bitmapgimex.h"
#include "jpeggimex.h"
#include "pnggimex.h"
#include "rzcriticalsection.h"
#include "rzfastalloc.h"
#include "rzstring.h"
#include "shpgimex.h"
#include "targagimex.h"
#include <gimex.h>
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

    // Gimex FSH
    Hook_Function(0x00762D1C, FSH_wopen);
    Hook_Function(0x00762D99, FSH_write);
    Hook_Function(0x00762DCC, FSH_wclose);
    Hook_Function(0x00762E15, FSH_about);
    Hook_Function(0x00762F0E, FSH_is);
    Hook_Function(0x00762F6D, FSH_open);
    Hook_Function(0x00762FE2, FSH_info);
    Hook_Function(0x00763C45, FSH_read);
    Hook_Function(0x00763C7B, FSH_close);

    // Gimex JPG
    Hook_Function(0x0076232B, JPG_wopen);
    Hook_Function(0x0076237B, JPG_write);
    // Hook_Function(0x00762DCC, JPG_wclose);
    Hook_Function(0x007627C0, JPG_about);
    Hook_Function(0x00762894, JPG_is);
    Hook_Function(0x007628F3, JPG_open);
    Hook_Function(0x00762939, JPG_info);
    Hook_Function(0x00762B97, JPG_read);
    Hook_Function(0x0076116F, JPG_close); // Also hooks JPG_wclose and PNG_wclose

    // Gimex IJL
    Hook_Function(0x007618BF, IJL_wopen);
    Hook_Function(0x007618F1, IJL_write);
    // Hook_Function(0x0075ED21, IJL_wclose);
    Hook_Function(0x00761BE1, IJL_about);
    Hook_Function(0x00761CEB, IJL_is);
    Hook_Function(0x00761D53, IJL_open);
    Hook_Function(0x00761DF0, IJL_info);
    Hook_Function(0x00761F98, IJL_read);
    Hook_Function(0x007621BA, IJL_close);

    // Gimex PNG
    // Hook_Function(0x0076232B, PNG_wopen);
    Hook_Function(0x00760ECE, PNG_write);
    // Hook_Function(0x0076116F, PNG_wclose);
    Hook_Function(0x00761312, PNG_about);
    Hook_Function(0x007613C8, PNG_is);
    Hook_Function(0x00761414, PNG_open);
    Hook_Function(0x0076150F, PNG_info);
    Hook_Function(0x00761791, PNG_read);
    Hook_Function(0x0076188C, PNG_close);

    // Gimex Bitmap
    // Hook_Function(0x0076232B, BMP_wopen);
    Hook_Function(0x0075FC25, BMP_write);
    // Hook_Function(0x0075ED21, BMP_wclose);
    Hook_Function(0x007605F7, BMP_about);
    Hook_Function(0x007606B4, BMP_is);
    Hook_Function(0x007606ED, BMP_open);
    Hook_Function(0x00760733, BMP_info);
    Hook_Function(0x00760ADA, BMP_read);
    Hook_Function(0x0075ED21, BMP_close);

    // Gimex TGA
    // Hook_Function(0x0076232B, TGA_wopen);
    Hook_Function(0x0075E9C3, TGA_write);
    // Hook_Function(0x0075ED21, TGA_wclose);
    Hook_Function(0x0075F1C7, TGA_about);
    Hook_Function(0x0075F2AB, TGA_is);
    Hook_Function(0x0075F3BC, TGA_open);
    Hook_Function(0x0075F402, TGA_info);
    Hook_Function(0x0075F81D, TGA_read);
    // Hook_Function(0x0075ED21, TGA_close);

    // Gimex
    Hook_Function(0x0075E309, GIMEX_about);
    Hook_Function(0x0075E318, GIMEX_is);
    Hook_Function(0x0075E327, GIMEX_open);
    Hook_Function(0x0075E336, GIMEX_wopen);
    Hook_Function(0x0075E345, GIMEX_info);
    Hook_Function(0x0075E354, GIMEX_read);
    Hook_Function(0x0075E37C, GIMEX_close);
    Hook_Function(0x0075E398, GIMEX_wclose);
    Hook_Function(0x0075E3B4, GIMEX_write);
    Hook_Function(0x0075E3DC, GIMEX_get);
    Hook_Function(0x0075E3FB, GIMEX_set);
    Hook_Function(0x0075E417, GIMEX_find);
    Hook_Function(0x0075E4F6, galloc);
    Hook_Function(0x0075E503, gfree);
    Hook_Function(0x0075E512, gread);
    Hook_Function(0x0075E539, gwrite);
    //Hook_Function(0x0075E560, gseek);
    //Hook_Function(0x0075E579, glen);
}
