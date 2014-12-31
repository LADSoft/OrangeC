/*
	Software License Agreement (BSD License)
	
	Copyright (c) 1997-2008, David Lindauer, (LADSoft).
	All rights reserved.
	
	Redistribution and use of this software in source and binary forms, with or without modification, are
	permitted provided that the following conditions are met:
	
	* Redistributions of source code must retain the above
	  copyright notice, this list of conditions and the
	  following disclaimer.
	
	* Redistributions in binary form must reproduce the above
	  copyright notice, this list of conditions and the
	  following disclaimer in the documentation and/or other
	  materials provided with the distribution.
	
	* Neither the name of LADSoft nor the names of its
	  contributors may be used to endorse or promote products
	  derived from this software without specific prior
	  written permission of LADSoft.
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
	PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
	TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef __STDINT_H
#define __STDINT_H

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#ifdef __cplusplus
namespace __STD_NS__ {
#endif
typedef char int8_t ;
typedef unsigned char uint8_t ;
typedef char int_least8_t ;
typedef unsigned char uint_least8_t ;
typedef char int_fast8_t ;
typedef unsigned char uint_fast8_t ;

typedef short int16_t ;
typedef unsigned short uint16_t ;
typedef short int_least16_t ;
typedef unsigned short uint_least16_t ;
typedef short int_fast16_t ;
typedef unsigned short uint_fast16_t ;

typedef int int32_t ;
typedef unsigned uint32_t ;
typedef int int_least32_t ;
typedef unsigned uint_least32_t ;
typedef int int_fast32_t ;
typedef unsigned uint_fast32_t ;

#if  __STDC_VERSION__ >= 199901L
typedef long long int64_t ;
typedef unsigned long long uint64_t ;
typedef long long int_least64_t ;
typedef unsigned long long uint_least64_t ;
typedef long long int_fast64_t ;
typedef unsigned long long uint_fast64_t ;
#else
typedef __int64 int64_t ;
typedef unsigned __int64 uint64_t ;
typedef __int64 int_least64_t ;
typedef unsigned __int64 uint_least64_t ;
typedef __int64 int_fast64_t ;
typedef unsigned __int64 uint_fast64_t ;
#endif

typedef int32_t intptr_t ;
typedef uint32_t uintptr_t ;

typedef int64_t intmax_t ;
typedef uint64_t uintmax_t ;

#ifdef __cplusplus
};
#endif
#if !defined(__cplusplus) || defined(__STDC_LIMIT_MACROS)

#define INT8_MIN -128
#define INT8_MAX 127
#define INT_LEAST8_MIN -128
#define INT_LEAST8_MAX 127
#define INT_FAST8_MIN -128
#define INT_FAST8_MAX 127
#define UINT8_MAX 255
#define UINT_LEAST8_MAX 255
#define UINT_FAST8_MAX 255

#define INT16_MIN -32768
#define INT16_MAX 32767
#define INT_LEAST16_MIN -32768
#define INT_LEAST16_MAX 32767
#define INT_FAST16_MIN -32768
#define INT_FAST16_MAX 32767
#define UINT16_MAX 65535
#define UINT_LEAST16_MAX 65535
#define UINT_FAST16_MAX 65535

#define INT32_MIN (-0x7fffffff-1)
#define INT32_MAX 0x7fffffff
#define INT_LEAST32_MIN (-0x7fffffff-1)
#define INT_LEAST32_MAX 0x7fffffff
#define INT_FAST32_MIN (-0x7fffffff - 1)
#define INT_FAST32_MAX 0x7fffffff
#define UINT32_MAX 0xffffffff
#define UINT_LEAST32_MAX 0xffffffff
#define UINT_FAST32_MAX 0xffffffff

#if  __STDC_VERSION__ >= 199901L

#define INT64_MIN (-0x7fffffffffffffffLL-1)
#define INT64_MAX 0x7fffffffffffffffLL
#define INT_LEAST64_MIN (-0x7fffffffffffffffLL-1)
#define INT_LEAST64_MAX 0x7fffffffffffffffLL
#define INT_FAST64_MIN (-0x7fffffffffffffffLL - 1)
#define INT_FAST64_MAX 0x7fffffffffffffffLL
#define UINT64_MAX 0xffffffffffffffffLL
#define UINT_LEAST64_MAX 0xffffffffffffffffLL
#define UINT_FAST64_MAX 0xffffffffffffffffLL
#endif

#define INTPTR_MIN INT32_MIN
#define INTPTR_MAX INT32_MAX
#define UINTPTR_MAX UINT32_MAX

#define INTMAX_MIN INT64_MIN
#define INTMAX_MAX INT64_MAX
#define UINTMAX_MAX UINT64_MAX

#define PTRDIFF_MIN INT32_MIN
#define PTRDIFF_MAX INT32_MAX

#define SIZE_MAX UINT32_MAX

#define WCHAR_MIN ((wchar_t) 0)
#define WCHAR_MAX ((wchar_t)(~WCHAR_MIN))

#define SIG_ATOMIC_MIN INT_MIN
#define SIG_ATOMIC_MAX INT_MAX

#define WINT_MIN WCHAR_MIN
#define WINT_MAX WCHAR_MAX

#endif

#if !defined(__cplusplus) || defined(__STDC_CONSTANT_MACROS)

#define INT8_C(x)    ((int_least8_t)x)
#define UINT8_C(x)   ((uint_least8_t)x##U)
#define INT16_C(x)   ((int_least16_t)x)
#define UINT16_C(x)  ((uint_least16_t)x##U)
#define INT32_C(x)   (x)
#define UINT32_C(x)  (x##U)
#define INT64_C(x)   (x##LL)
#define UINT64_C(x)  (x##LLU)

#define INTMAX_C(x)	 (x##LL)
#define UINTMAX_C(x) (x##LLU)

#endif

#endif /* __STDINT_H */
#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__STDINT_H_USING_LIST)
#define __STDINT_H_USING_LIST
    using __STD_NS_QUALIFIER int8_t ;
    using __STD_NS_QUALIFIER uint8_t ;
    using __STD_NS_QUALIFIER int_least8_t ;
    using __STD_NS_QUALIFIER uint_least8_t ;
    using __STD_NS_QUALIFIER int_fast8_t ;
    using __STD_NS_QUALIFIER uint_fast8_t ;

    using __STD_NS_QUALIFIER int16_t ;
    using __STD_NS_QUALIFIER uint16_t ;
    using __STD_NS_QUALIFIER int_least16_t ;
    using __STD_NS_QUALIFIER uint_least16_t ;
    using __STD_NS_QUALIFIER int_fast16_t ;
    using __STD_NS_QUALIFIER uint_fast16_t ;

    using __STD_NS_QUALIFIER int32_t ;
    using __STD_NS_QUALIFIER uint32_t ;
    using __STD_NS_QUALIFIER int_least32_t ;
    using __STD_NS_QUALIFIER uint_least32_t ;
    using __STD_NS_QUALIFIER int_fast32_t ;
    using __STD_NS_QUALIFIER uint_fast32_t ;

#if  __STDC_VERSION__ >= 199901L
    using __STD_NS_QUALIFIER int64_t ;
    using __STD_NS_QUALIFIER uint64_t ;
    using __STD_NS_QUALIFIER int_least64_t ;
    using __STD_NS_QUALIFIER uint_least64_t ;
    using __STD_NS_QUALIFIER int_fast64_t ;
    using __STD_NS_QUALIFIER uint_fast64_t ;
#endif

    using __STD_NS_QUALIFIER intptr_t ;
    using __STD_NS_QUALIFIER uintptr_t ;

    using __STD_NS_QUALIFIER intmax_t ;
    using __STD_NS_QUALIFIER uintmax_t ;

#endif
