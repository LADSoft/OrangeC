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
#ifndef __DEFS_H__
#define __DEFS_H__

/* RTL Function and data declarators */
#if defined(__BUILDING_LSCRTL_DLL)
#define _RTL_FUNC  _export
#define _RTL_DATA  _export
#define _IMPORT
#define _IMPORTMM
#define _RTL_CLASS
#define _MSIL_RTL
#elif defined( __LSCRTL_DLL) || defined(__CRTDLL_DLL)
#define _RTL_FUNC
#define _RTL_DATA
#define _IMPORT _import
#define _IMPORTMM _import
#define _RTL_CLASS
#define _MSIL_RTL
#elif defined(__MSVCRT_DLL)
#define _RTL_FUNC
#define _RTL_DATA
#define _IMPORT _import
#define _IMPORTMM
#define _RTL_CLASS
#define _MSIL_RTL
#elif defined(__MSIL__)
#define _RTL_FUNC
#define _RTL_DATA
#define _IMPORT
#define _IMPORTMM
#define _RTL_CLASS
#define _MSIL_RTL __msil_rtl
#else
#define _RTL_FUNC
#define _RTL_DATA
#define _RTL_CLASS
#define _IMPORT
#define _IMPORTMM
#define _MSIL_RTL
#endif

#if !defined(__STDC__) || defined(__STDC_VERSION__)
#define _RTL_INTRINS _RTL_FUNC _intrinsic
#else
#define _RTL_INTRINS
#endif

#if defined(__CC386__) && !defined(_WIN32)
#define _FAR far
#else
#define _FAR
#endif 

#ifdef __cplusplus
#define __NOTHROW throw ( )
#define __STD_NS__ std
#define __STD_NS_QUALIFIER __STD_NS__::
#else
#define __NOTHROW 
#define __STD_NS_QUALIFIER
#endif

/* the headers use the restrict keyword, which is not valid prior to
 * C99.  If not compiling for C99 define it as an empty macro to allow
 * compilation to procede
 */
#if  __STDC_VERSION__ < 199901L
#define restrict
#define __restrict restrict
#endif

#if defined(_WIN32)
#define __declspec(x) x
#define dllexport _export
#define dllimport _import
#endif

#endif
