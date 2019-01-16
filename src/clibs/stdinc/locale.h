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
#endif

#pragma pack()

#endif  /* __LOCALE_H */