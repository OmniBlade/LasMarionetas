/**
 * @file
 *
 * @author OmniBlade
 *
 * @brief Masks differences between compilers definitions of calling conventions.
 *
 * @copyright Compat is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            3 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#ifndef STDCOMPAT_CALLCONV_H
#define STDCOMPAT_CALLCONV_H

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

/* Unless we are on */
#if defined _MSC_VER
#ifndef fastcall_abi
#if defined _M_IX86
#define fastcall_abi __fastcall
#else
#define fastcall_abi
#endif
#endif

#ifndef cdecl_abi
#if defined _M_IX86
#define cdecl_abi __cdecl
#else
#define cdecl_abi
#endif
#endif

#ifndef stdcall_abi
#if defined _M_IX86
#define stdcall_abi __stdcall
#else
#define stdcall_abi
#endif
#endif
#else
#ifndef fastcall_abi
#if __has_attribute(fastcall) && defined __i386__
#define fastcall_abi __attribute__((fastcall))
#else
#define fastcall_abi
#endif
#endif

#ifndef cdecl_abi
#if __has_attribute(cdecl) && defined __i386__
#define cdecl_abi __attribute__((cdecl))
#else
#define cdecl_abi
#endif
#endif

#ifndef stdcall_abi
#if __has_attribute(stdcall) && defined __i386__
#define stdcall_abi __attribute__((stdcall))
#else
#define stdcall_abi
#endif
#endif
#endif /* defined _MSC_VER */

#endif /* STDCOMPAT_CALLCONV_H */
