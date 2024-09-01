/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

#pragma GCC system_header

#ifndef __DEFS_H__
#    define __DEFS_H__

/* RTL Function and data declarators */
#    if defined(__BUILDING_LSCRTL_DLL)
#        define _RTL_FUNC __declspec(dllexport)
#        define _RTL_DATA __declspec(dllexport)
#        define _IMPORT
#        define _IMPORTMM
#        define _RTL_CLASS __declspec(dllexport)
#        define _MSIL_RTL
#    elif defined(__LSCRTL_DLL) || defined(__CRTDLL_DLL)
#        define _RTL_FUNC
#        define _RTL_DATA
#        define _IMPORT __declspec(dllimport)
#        define _IMPORTMM __declspec(dllimport)
#        define _RTL_CLASS __declspec(dllimport)
#        define _MSIL_RTL
#    elif defined(__MSVCRT_DLL)
#        define _RTL_FUNC
#        define _RTL_DATA
#        define _IMPORT __declspec(dllimport)
#        define _IMPORTMM
#        define _RTL_CLASS
#        define _MSIL_RTL
#    elif defined(__MSIL__)
#        define _RTL_FUNC
#        define _RTL_DATA
#        define _IMPORT
#        define _IMPORTMM
#        define _RTL_CLASS
#        define _MSIL_RTL __msil_rtl
#    else
#        define _RTL_FUNC
#        define _RTL_DATA
#        define _RTL_CLASS
#        define _IMPORT
#        define _IMPORTMM
#        define _MSIL_RTL
#    endif

#    if !defined(__STDC__) || defined(__STDC_VERSION__)
#        define _RTL_INTRINS _RTL_FUNC _intrinsic
#    else
#        define _RTL_INTRINS
#    endif

#    if defined(__CC386__) && !defined(_WIN32)
#        define _FAR far
#    else
#        define _FAR
#    endif

#    define ZSTR __attribute__((zstring))

#    ifdef __cplusplus
#        define __NOTHROW throw()
#        define __STD_NS__ std
#        define __STD_NS_QUALIFIER __STD_NS__::
#    else
#        define __NOTHROW
#        define __STD_NS_QUALIFIER
#    endif

#    if __STDC_VERSION__ < 201112L || defined(__cplusplus)
#        define _NORETURN
#    else
#        define _NORETURN _Noreturn
#    endif

/* the headers use the restrict keyword, which is not valid prior to
 * C99.  If not compiling for C99 define it as an empty macro to allow
 * compilation to procede
 */
#    if __STDC_VERSION__ < 199901L
#        define restrict
#        define __restrict restrict
#    endif

#endif
