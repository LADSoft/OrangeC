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
/*  stddef.h  

    Definitions for common types, and NULL

*/

#ifndef __STDDEF_H
#define __STDDEF_H

#ifndef __DEFS_H__
#include <_defs.h>
#endif

#ifdef __cplusplus
namespace __STD_NS__ {

typedef decltype(nullptr) nullptr_t; 
   
#endif

#if defined(__cplusplus) || __STDC_VERSION__ >= 201112L
typedef long double max_align_t;
#endif

#ifndef _PTRDIFF_T
#define _PTRDIFF_T
typedef int    ptrdiff_t;
#endif

#ifndef _SIZE_T
#define _SIZE_T
#define _USING_STDDEF_SIZE_T
typedef unsigned size_t;
typedef int ssize_t;
#endif

#ifndef _WCHAR_T_DEFINED
#define _WCHAR_T_DEFINED
#ifndef __cplusplus
typedef unsigned short wchar_t;
#endif
#endif

#ifndef FAR
#define FAR _FAR
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef offsetof
#define offsetof( s_name, m_name )  (size_t)&(((s_name *)0)->m_name)
#endif

#ifndef _WCTYPE_T_DEFINED
typedef unsigned int wctype_t;
#define _WCTYPE_T_DEFINED
#endif

#ifndef _WINT_T_DEFINED
#define _WINT_T_DEFINED
typedef unsigned int wint_t;
#endif

#ifdef __cplusplus
} ;
#endif
#endif  /* __STDDEF_H */
#if defined(__cplusplus) /*&& !defined(__USING_CNAME__)*/ && !defined(__STDDEF_H_USING_LIST)
#define __STDDEF_H_USING_LIST
using __STD_NS_QUALIFIER ptrdiff_t ;
#ifdef _USING_STDDEF_SIZE_T
using __STD_NS_QUALIFIER size_t ;
using __STD_NS_QUALIFIER ssize_t ;
#endif
using __STD_NS_QUALIFIER wint_t;
using __STD_NS_QUALIFIER wctype_t ;
#endif
