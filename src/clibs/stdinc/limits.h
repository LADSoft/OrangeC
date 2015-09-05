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
/*  limits.h

    Defines implementation specific limits on type values.

*/

#ifndef __LIMITS_H
#define __LIMITS_H

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#define CHAR_BIT        8               /* number of bits in a char */
#define MB_LEN_MAX      18              /* max. # bytes in multibyte char */

#define SCHAR_MIN     (-128)            /* minimum signed   char value */
#define SCHAR_MAX       127             /* maximum signed   char value */
#define UCHAR_MAX       255             /* maximum unsigned char value */

#define CHAR_MIN        SCHAR_MIN       /* minimum char value */
#define CHAR_MAX        SCHAR_MAX       /* maximum char value */

#define SHRT_MIN      (-32767-1)        /* minimum signed   short value */
#define SHRT_MAX        32767           /* maximum signed   short value */
#define USHRT_MAX       65535           /* maximum unsigned short value */

#define LONG_MIN      (-2147483647L-1)  /* minimum signed   long value */
#define LONG_MAX        2147483647L     /* maximum signed   long value */
#define ULONG_MAX       4294967295UL    /* maximum unsigned long value */

#define INT_MIN         LONG_MIN        /* minimum signed   int value */
#define INT_MAX         LONG_MAX        /* maximum signed   int value */
#define UINT_MAX        ULONG_MAX       /* maximum unsigned int value */

#if  defined(__cplusplus) || __STDC_VERSION__ >= 199901L
#define LLONG_MIN     (-9223372036854775807LL-1) /* minimum signed   long long value */
#define LLONG_MAX       9223372036854775807LL    /* maximum signed   long long value */
#define ULLONG_MAX      18446744073709551615ULL  /* maximum unsigned long long  value */
#endif

#define _I64_MIN      (-9223372036854775807i64-1) /* minimum signed   __int64 value */
#define _I64_MAX        9223372036854775807i64    /* maximum signed   __int64 value */
#define _UI64_MAX       18446744073709551615ui64  /* maximum unsigned __int64 value */

#endif  /* __LIMITS_H */
