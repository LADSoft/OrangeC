/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2008, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, with or without modification, are
    permitted provided thatst the following conditions are met:
    
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
/*  ctype.h

    Defines the locale aware ctype macros.

*/


#ifndef __pctype_H
#define __pctype_H

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#ifdef __cplusplus
//namespace __STD_NS__ {
extern "C" {
#endif

extern unsigned short const _RTL_DATA * _pctype;

/* character classes */

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

#define _SPACE _IS_SP
#define _BLANK _IS_BLK
#define _PUNCT _IS_PUN
#define _ALPHA _IS_ALPHA
#define _DIGIT _IS_DIG
#define _CONTROL _IS_CTL
#define _UPPER _IS_UPP
#define _LOWER _IS_LOW
#define _HEX _IS_HEX

int      _RTL_FUNC _IMPORT isalnum (int __c);
int      _RTL_FUNC _IMPORT isalpha (int __c);
int      _RTL_FUNC _IMPORT isblank (int __c);
int      _RTL_FUNC _IMPORT iscntrl (int __c);
int      _RTL_FUNC _IMPORT isdigit (int __c);
int      _RTL_FUNC _IMPORT isgraph (int __c);
int      _RTL_FUNC _IMPORT islower (int __c);
int      _RTL_FUNC _IMPORT isprint (int __c);
int      _RTL_FUNC _IMPORT ispunct (int __c);
int      _RTL_FUNC _IMPORT isspace (int __c);
int      _RTL_FUNC _IMPORT isupper (int __c);
int      _RTL_FUNC _IMPORT isxdigit(int __c);
int      _RTL_FUNC _IMPORT isascii (int __c);
int 	 _RTL_FUNC _IMPORT toascii(int);
int		 _RTL_FUNC _IMPORT __isascii(int);
int		 _RTL_FUNC _IMPORT __toascii(int);

int      _RTL_FUNC _IMPORT tolower(int __ch);
int      _RTL_FUNC _IMPORT _ltolower(int __ch);
int      _RTL_FUNC _IMPORT toupper(int __ch);
int      _RTL_FUNC _IMPORT _ltoupper(int __ch);

int		 _RTL_FUNC _IMPORT _isctype(int, int);
int		 _RTL_FUNC _IMPORT __iscsymf(int);
int		 _RTL_FUNC _IMPORT __iscsym(int);
#ifdef __cplusplus
};
//};
#endif

#ifndef __cplusplus
#define isalnum(c)   ( __STD_NS_QUALIFIER  _pctype[ (c) ] & (_IS_ALNUM))
                     
#define isalpha(c)   ( __STD_NS_QUALIFIER  _pctype[ (c) ] & (_IS_ALPHA))
                     
#define isblank(c)   ( __STD_NS_QUALIFIER  _pctype[ (c) ] & (_IS_BLK))
                     
#define iscntrl(c)   ( __STD_NS_QUALIFIER  _pctype[ (c) ] & (_IS_CTL))
                     
#define isdigit(c)   ( __STD_NS_QUALIFIER  _pctype[ (c) ] & (_IS_DIG))
                     
#define isgraph(c)   ( __STD_NS_QUALIFIER  _pctype[ (c) ] & (_IS_GRAPH))
                     
#define islower(c)   ( __STD_NS_QUALIFIER  _pctype[ (c) ] & (_IS_LOW))
                     
#define isprint(c)   ( __STD_NS_QUALIFIER  _pctype[ (c) ] & (_IS_PRINT))
                     
#define ispunct(c)   ( __STD_NS_QUALIFIER  _pctype[ (c) ] & (_IS_PUN))
                     
#define isspace(c)   ( __STD_NS_QUALIFIER  _pctype[ (c) ] & (_IS_SP))
                     
#define isupper(c)   ( __STD_NS_QUALIFIER  _pctype[ (c) ] & (_IS_UPP))
                     
#define isxdigit(c)  ( __STD_NS_QUALIFIER  _pctype[ (c) ] & (_IS_HEX))

#define _toupper(c) ((c) + 'A' - 'a')
#define _tolower(c) ((c) + 'a' - 'A')
#define isascii(c)  ((unsigned)(c) < 128)
#define toascii(c)  ((c) & 0x7f)
#endif


#endif 

/*
#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__pctype_H_USING_LIST)
#define __pctype_H_USING_LIST
    using __STD_NS_QUALIFIER isalnum;
    using __STD_NS_QUALIFIER isalpha;
    using __STD_NS_QUALIFIER isblank;
    using __STD_NS_QUALIFIER iscntrl;
    using __STD_NS_QUALIFIER isdigit;
    using __STD_NS_QUALIFIER isgraph;
    using __STD_NS_QUALIFIER islower;
    using __STD_NS_QUALIFIER isprint;
    using __STD_NS_QUALIFIER ispunct;
    using __STD_NS_QUALIFIER isspace;
    using __STD_NS_QUALIFIER isupper;
    using __STD_NS_QUALIFIER isxdigit;
    using __STD_NS_QUALIFIER isascii;
    using __STD_NS_QUALIFIER toascii;
    using __STD_NS_QUALIFIER __isascii;
    using __STD_NS_QUALIFIER __toascii;
    using __STD_NS_QUALIFIER tolower;
    using __STD_NS_QUALIFIER _ltolower;
    using __STD_NS_QUALIFIER toupper;
    using __STD_NS_QUALIFIER _ltoupper;
    using __STD_NS_QUALIFIER _pctype;
    using __STD_NS_QUALIFIER _isctype;
    using __STD_NS_QUALIFIER __iscsymf;
    using __STD_NS_QUALIFIER __iscsym;
#endif

*/