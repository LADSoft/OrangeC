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
/*  locale.h

*/
#ifndef __LOCALE_H
#define __LOCALE_H

#pragma pack(1)

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#define LC_ALL      0
#define LC_COLLATE  1
#define LC_CTYPE    2
#define LC_MONETARY 3
#define LC_NUMERIC  4
#define LC_TIME     5
#define LC_MESSAGES 6
#define LC_userdef  7
#define LC_LAST     LC_userdef

#define LC_COLLATE_MASK     (1 << LC_COLLATE)
#define LC_CTYPE_MASK       (1 << LC_CTYPE) 
#define LC_MESSAGES_MASK    (1 << LC_MESSAGES) 
#define LC_MONETARY_MASK    (1 << LC_MONETARY)
#define LC_NUMERIC_MASK     (1 << LC_NUMERIC)
#define LC_TIME_MASK        (1 << LC_TIME)
#define LC_ALL_MASK (LC_COLLATE_MASK | LC_CTYPE_MASK | LC_MESSAGES_MASK | LC_MONETARY_MASK | LC_NUMERIC_MASK | LC_TIME_MASK)

typedef void *locale_t;

#ifdef __cplusplus
namespace __STD_NS__ {
extern "C" {
#endif

struct lconv {

   char *decimal_point;
   char *thousands_sep;
   char *grouping;
   char *int_curr_symbol;
   char *currency_symbol;
   char *mon_decimal_point;
   char *mon_thousands_sep;
   char *mon_grouping;
   char *positive_sign;
   char *negative_sign;
   char int_frac_digits;
   char frac_digits;
   char p_cs_precedes;
   char p_sep_by_space;
   char n_cs_precedes;
   char n_sep_by_space;
   char p_sign_posn;
   char n_sign_posn;
   char int_p_cs_precedes; /* CHAR_MAX */
   char int_n_cs_precedes; /* CHAR_MAX */
   char int_p_sep_by_space; /* CHAR_MAX */
   char int_n_sep_by_space; /* CHAR_MAX */
   char int_p_sign_posn; /* CHAR_MAX */
   char int_n_sign_posn; /* CHAR_MAX */
} ;

char *          _RTL_FUNC _IMPORT setlocale( int __category, const char *__locale );
char *          _RTL_FUNC _IMPORT _lsetlocale( int __category, const char *__locale );
struct lconv *  _RTL_FUNC _IMPORT localeconv( void );
struct lconv *  _RTL_FUNC _IMPORT _llocaleconv( void );

#ifdef __USELOCALES__
#define setlocale  _lsetlocale
#define localeconv _llocaleconv
#endif
#ifdef __cplusplus
};
};
#endif

#pragma pack()

#endif  /* __LOCALE_H */
#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__LOCALE_H_USING_LIST)
#define __LOCALE_H_USING_LIST
    using __STD_NS_QUALIFIER lconv ;
    using __STD_NS_QUALIFIER setlocale;
    using __STD_NS_QUALIFIER _lsetlocale;
    using __STD_NS_QUALIFIER localeconv;
    using __STD_NS_QUALIFIER _llocaleconv;
#endif
