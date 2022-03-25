/**
 * @file
 *
 * @author CCHyper
 * @author jonwil
 * @author LRFLEW
 * @author OmniBlade
 * @author Saberhawk
 *
 * @brief Hooking system for interacting with original binary.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 *
 * Originally based on work by the Tiberian Technologies team to hook Renegade, rewritten based on work by LRFLEW for
 * OpenMC2. Provides methods for accessing data and functions in an existing binary and replacing functions with new
 * implementations from an injected DLL.
 */
#pragma once

#include <windows.h>

#include <stdint.h>

enum BinaryType
{
    BINARY_GAME,
    BINARY_EDITOR,
    BINARY_INSTALLER,
    BINARY_HOUSEED,
    BINARY_CITY,
    BINARY_MAPED,
};

extern const BinaryType geBinaryMode;

inline constexpr uintptr_t Pick_Address(
    uintptr_t game, uintptr_t shop = 0, uintptr_t pkgs = 0, uintptr_t house = 0, uintptr_t city = 0, uintptr_t map = 0)
{
    switch (geBinaryMode) {
        case BINARY_GAME:
            return game;
        case BINARY_EDITOR:
            return shop;
        case BINARY_INSTALLER:
            return pkgs;
        case BINARY_HOUSEED:
            return house;
        case BINARY_CITY:
            return city;
        case BINARY_MAPED:
            return map;
        default:
            break;
    }

    return 0;
}

// Use this to set up hooks which hook the same functions in different binaries.
void SetupCommonHooks();

// Helper Functions based on those from OpenMC2

// Use Make_Global to access global variables in the original exe once you know
// the correct type and address. This should not change from run to run if the
// exe loads at a standard base address.

// Typical use will be to use define to create a friendly name, e.g:
// #define SomeGlobalVar (Make_Global<bool>(0x00FF00FF))
// allows you to use SomeGlobalVar as though it was a bool you declared, though
// it will reflect the value the original exe sees at address 0x00FF00FF
template<typename T> __forceinline T &Make_Global(const uintptr_t address)
{
    return *reinterpret_cast<T *>(address);
}

template<typename T> __forceinline T *Make_Pointer(const uintptr_t address)
{
    return reinterpret_cast<T *>(address);
}

// Call_Function and Call_Method can be used to call functions from the original
// binary where they are required and a replacement hasn't been written yet.
template<typename T, typename... Types> __forceinline T Call_Function(const uintptr_t address, Types... args)
{
#ifdef DLL_BUILD
    return reinterpret_cast<T(__cdecl *)(Types...)>(address)(args...);
#else
    return T();
#endif
}

template<typename T, typename... Types> __forceinline T Call__StdCall_Function(const uintptr_t address, Types... args)
{
#ifdef DLL_BUILD
    return reinterpret_cast<T(__stdcall *)(Types...)>(address)(args...);
#else
    return T();
#endif
}

template<typename T, typename X, typename... Types>
__forceinline T Call_Method(const uintptr_t address, X *const self, Types... args)
{
#ifdef DLL_BUILD
    return reinterpret_cast<T(__thiscall *)(X *, Types...)>(address)(self, args...);
#else
    return T();
#endif
}

// A nice struct to pack the assembly in for jumping into replacement code.
// So long as the calling conventions and arguments for the replaced and
// replacement functions are the same, everything should just work.
#pragma pack(push, 1)
struct x86_jump
{
    const uint8_t cmd = 0xE9;
    uintptr_t addr;
};
#pragma pack(pop)

// Use these to hook existing functions and replace them with newly written ones
// to either replace them permenantly or just to test correctness of a newly
// written one.

// Base hooking function to apply the actual hook.
inline void Hook_Func(uintptr_t in, uintptr_t out)
{
#ifdef DLL_BUILD
    static_assert(sizeof(x86_jump) == 5, "Jump struct not expected size.");
    if (in == 0 || out == 0) {
        // Bail if we don't have an address to hook.
        return;
    }
    x86_jump cmd;
    cmd.addr = out - in - 5;
    WriteProcessMemory(GetCurrentProcess(), (LPVOID)in, &cmd, 5, nullptr);
#endif
}

// Hook regular functions and static methods.
template<typename T> void Hook_Function(uintptr_t in, T out)
{
    Hook_Func(in, reinterpret_cast<uintptr_t>(out));
}

// Method pointers need funky syntax to get the underlying function address
// hence the odd cast to void for the out pointer.
template<typename T> void Hook_Method(uintptr_t in, T out)
{
    Hook_Func(in, reinterpret_cast<uintptr_t>((void *&)out));
}

// Virtuals are even trickier so resort to inline asm for those curtesy of the TTScripts guys.
#define Hook_Any(in, out) \
    do { \
        uintptr_t __hook_input = (in); \
        __asm { \
        __asm push out \
        __asm push __hook_input \
        __asm call Hook_Func \
        __asm add esp, 8 } \
    } while (false)
