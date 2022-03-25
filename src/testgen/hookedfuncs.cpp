#include "hooker.h"
#include "rzfastalloc.h"
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

int __stdcall Main_Func(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

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

    Hook_Function(0x00EE3B69, Main_Func);
}
