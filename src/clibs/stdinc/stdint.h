/* Software License Agreement
 * 
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     As a special exception, if other files instantiate templates or
 *     use macros or inline functions from this file, or you compile
 *     this file and link it with other works to produce a work based
 *     on this file, this file does not by itself cause the resulting
 *     work to be covered by the GNU General Public License. However
 *     the source code for this file must still be made available in
 *     accordance with section (3) of the GNU General Public License.
 *     
 *     This exception does not invalidate any other reasons why a work
 *     based on this file might be covered by the GNU General Public
 *     License.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#ifndef __STDINT_H
#define __STDINT_H

#ifndef __STDDEF_H
#    include <stddef.h>
#endif

typedef char int8_t;
typedef unsigned char uint8_t;
typedef char int_least8_t;
typedef unsigned char uint_least8_t;
typedef char int_fast8_t;
typedef unsigned char uint_fast8_t;

typedef short int16_t;
typedef unsigned short uint16_t;
typedef short int_least16_t;
typedef unsigned short uint_least16_t;
typedef short int_fast16_t;
typedef unsigned short uint_fast16_t;

typedef int int32_t;
typedef unsigned uint32_t;
typedef int int_least32_t;
typedef unsigned uint_least32_t;
typedef int int_fast32_t;
typedef unsigned uint_fast32_t;

#if __STDC_VERSION__ >= 199901L
typedef long long int64_t;
typedef unsigned long long uint64_t;
typedef long long int_least64_t;
typedef unsigned long long uint_least64_t;
typedef long long int_fast64_t;
typedef unsigned long long uint_fast64_t;
#else
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
typedef __int64 int_least64_t;
typedef unsigned __int64 uint_least64_t;
typedef __int64 int_fast64_t;
typedef unsigned __int64 uint_fast64_t;
#endif

typedef int32_t intptr_t;
typedef uint32_t uintptr_t;

typedef int64_t intmax_t;
typedef uint64_t uintmax_t;

#if !defined(__cplusplus) || defined(__STDC_LIMIT_MACROS)

#    define INT8_MIN -128
#    define INT8_MAX 127
#    define INT_LEAST8_MIN -128
#    define INT_LEAST8_MAX 127
#    define INT_FAST8_MIN -128
#    define INT_FAST8_MAX 127
#    define UINT8_MAX 255
#    define UINT_LEAST8_MAX 255
#    define UINT_FAST8_MAX 255

#    define INT16_MIN -32768
#    define INT16_MAX 32767
#    define INT_LEAST16_MIN -32768
#    define INT_LEAST16_MAX 32767
#    define INT_FAST16_MIN -32768
#    define INT_FAST16_MAX 32767
#    define UINT16_MAX 65535
#    define UINT_LEAST16_MAX 65535
#    define UINT_FAST16_MAX 65535

#    define INT32_MIN (-0x7fffffff - 1)
#    define INT32_MAX 0x7fffffff
#    define INT_LEAST32_MIN (-0x7fffffff - 1)
#    define INT_LEAST32_MAX 0x7fffffff
#    define INT_FAST32_MIN (-0x7fffffff - 1)
#    define INT_FAST32_MAX 0x7fffffff
#    define UINT32_MAX 0xffffffff
#    define UINT_LEAST32_MAX 0xffffffff
#    define UINT_FAST32_MAX 0xffffffff

#    if __STDC_VERSION__ >= 199901L

#        define INT64_MIN (-0x7fffffffffffffffLL - 1)
#        define INT64_MAX 0x7fffffffffffffffLL
#        define INT_LEAST64_MIN (-0x7fffffffffffffffLL - 1)
#        define INT_LEAST64_MAX 0x7fffffffffffffffLL
#        define INT_FAST64_MIN (-0x7fffffffffffffffLL - 1)
#        define INT_FAST64_MAX 0x7fffffffffffffffLL
#        define UINT64_MAX 0xffffffffffffffffLL
#        define UINT_LEAST64_MAX 0xffffffffffffffffLL
#        define UINT_FAST64_MAX 0xffffffffffffffffLL
#    endif

#    define INTPTR_MIN INT32_MIN
#    define INTPTR_MAX INT32_MAX
#    define UINTPTR_MAX UINT32_MAX

#    define INTMAX_MIN INT64_MIN
#    define INTMAX_MAX INT64_MAX
#    define UINTMAX_MAX UINT64_MAX

#    define PTRDIFF_MIN INT32_MIN
#    define PTRDIFF_MAX INT32_MAX

#    define SIZE_MAX UINT32_MAX
#    ifndef WCHAR_MIN
#        define WCHAR_MIN ((wchar_t)0)
#        define WCHAR_MAX ((wchar_t)(~WCHAR_MIN))
#    endif
#    define SIG_ATOMIC_MIN INT_MIN
#    define SIG_ATOMIC_MAX INT_MAX

#    define WINT_MIN WCHAR_MIN
#    define WINT_MAX WCHAR_MAX

#endif

#if !defined(__cplusplus) || defined(__STDC_CONSTANT_MACROS)

#    define INT8_C(x) ((int_least8_t)x)
#    define UINT8_C(x) ((uint_least8_t)x##U)
#    define INT16_C(x) ((int_least16_t)x)
#    define UINT16_C(x) ((uint_least16_t)x##U)
#    define INT32_C(x) (x)
#    define UINT32_C(x) (x##U)
#    define INT64_C(x) (x##LL)
#    define UINT64_C(x) (x##LLU)

#    define INTMAX_C(x) (x##LL)
#    define UINTMAX_C(x) (x##LLU)

#endif

#endif /* __STDINT_H */
