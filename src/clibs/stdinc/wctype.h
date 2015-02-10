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
#ifndef __WCTYPE_H
#define __WCTYPE_H

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#if !defined(__TCHAR_H)
#  include <tchar.h>
#endif

/* these are actually defined in stddef.h, replicated here for reference */
#ifndef _WCTYPE_T_DEFINED
typedef unsigned int wctype_t;
#define _WCTYPE_T_DEFINED
#endif

#ifndef _WINT_T_DEFINED
#define _WINT_T_DEFINED
typedef unsigned int wint_t;
#endif

typedef const wchar_t *wctrans_t;

#ifndef WEOF
#define WEOF (wint_t)(-1)
#endif

#ifndef _IS_CONSTANTS
#define _IS_CONSTANTS
#define _IS_UPP     1           /* upper case */
#define _IS_LOW     2           /* lower case */
#define _IS_DIG     4           /* digit */
#define _IS_SP      8           /* space */
#define _IS_PUN    16           /* punctuation */
#define _IS_CTL    32           /* control */
#define _IS_BLK    64           /* blank */
#define _IS_HEX   128           /* [0..9] or [A-F] or [a-f] */
#define _IS_GPH   512

#define _IS_ALPHA    (0x100 | _IS_UPP | _IS_LOW)
#define _IS_ALNUM    (_IS_DIG | _IS_ALPHA)
#define _IS_GRAPH    (_IS_ALNUM | _IS_HEX | _IS_PUN)
#define _IS_PRINT    (_IS_GRAPH | _IS_BLK)
#endif

#ifdef __cplusplus
namespace __STD_NS__ {
extern "C" {
#endif
int iswalnum (wint_t __wc);
int iswalpha (wint_t __wc);
int iswcntrl (wint_t __wc);
int iswdigit (wint_t __wc);
int iswgraph (wint_t __wc);
int iswlower (wint_t __wc);
int iswprint (wint_t __wc);
int iswpunct (wint_t __wc);
int iswspace (wint_t __wc);
int iswupper (wint_t __wc);
int iswxdigit (wint_t __wc);

/* this next is an extenstion to the standard */
int iswblank  (wint_t __wc);

wctype_t wctype (const char *__property);
int iswctype (wint_t __wc, wctype_t __desc);

wint_t towlower (wint_t __wc);
wint_t towupper (wint_t __wc);

wctrans_t wctrans (const char *__property);
wint_t towctrans (wint_t __wc, wctrans_t __desc);

extern const wchar_t *__ctype_u_tolower; /* Case conversions.  */
extern const wchar_t *__ctype_u_toupper; /* Case conversions.  */

#ifdef __cplusplus
} ;
} ;
#endif
#ifndef	__NO_WCTYPE
#define	iswalnum(wc)	__STD_NS_QUALIFIER  iswctype ((wc), _IS_ALNUM)
#define	iswalpha(wc)	__STD_NS_QUALIFIER  iswctype ((wc), _IS_ALPHA)
#define	iswcntrl(wc)	__STD_NS_QUALIFIER  iswctype ((wc), _IS_CTL)
#define	iswdigit(wc)	__STD_NS_QUALIFIER  iswctype ((wc), _IS_DIG)
#define	iswlower(wc)	__STD_NS_QUALIFIER  iswctype ((wc), _IS_LOW)
#define	iswgraph(wc)	__STD_NS_QUALIFIER  iswctype ((wc), _IS_GRAPH)
#define	iswprint(wc)	__STD_NS_QUALIFIER  iswctype ((wc), _IS_PRINT)
#define	iswpunct(wc)	__STD_NS_QUALIFIER  iswctype ((wc), _IS_PUN)
#define	iswspace(wc)	__STD_NS_QUALIFIER  iswctype ((wc), _IS_SP)
#define	iswupper(wc)	__STD_NS_QUALIFIER  iswctype ((wc), _IS_UPP)
#define	iswxdigit(wc)	__STD_NS_QUALIFIER  iswctype ((wc), _IS_HEX)

/* this next is an extension to the standard */
#define	iswblank(wc)	__STD_NS_QUALIFIER  iswctype ((wc), _IS_BLK)


#define	towlower(wc)	__STD_NS_QUALIFIER towctrans (wc, __STD_NS_QUALIFIER __ctype_u_tolower)
#define	towupper(wc)	__STD_NS_QUALIFIER towctrans (wc, __STD_NS_QUALIFIER __ctype_u_toupper)

#endif /*  __NO_WCTYPE.  */

#endif /* __WCTYPE_H  */
#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__WCTYPE_H_USING_LIST)
#define __WCTYPE_H_USING_LIST
	using __STD_NS_QUALIFIER iswalnum;
	using __STD_NS_QUALIFIER iswalpha;
	using __STD_NS_QUALIFIER iswcntrl;
	using __STD_NS_QUALIFIER iswdigit;
	using __STD_NS_QUALIFIER iswgraph;
	using __STD_NS_QUALIFIER iswlower;
	using __STD_NS_QUALIFIER iswprint;
	using __STD_NS_QUALIFIER iswpunct;
	using __STD_NS_QUALIFIER iswspace;
	using __STD_NS_QUALIFIER iswupper;
	using __STD_NS_QUALIFIER iswxdigit;
	using __STD_NS_QUALIFIER wctype;
	using __STD_NS_QUALIFIER iswctype;
	using __STD_NS_QUALIFIER towlower;
	using __STD_NS_QUALIFIER towupper;
	using __STD_NS_QUALIFIER wctrans;
	using __STD_NS_QUALIFIER towctrans;
#endif
