/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

/*  limits.h

    Defines implementation specific limits on type values.

*/

#ifndef __LIMITS_H
#define __LIMITS_H

#ifndef __STDDEF_H
#    include <stddef.h>
#endif


#define CHAR_BIT 8    /* number of bits in a char */
#define MB_LEN_MAX 18 /* max. # bytes in multibyte char */

#define SCHAR_MIN (-128) /* minimum signed   char value */
#define SCHAR_MAX 127    /* maximum signed   char value */

#define UCHAR_MAX 255    /* maximum unsigned char value */

#define CHAR_WIDTH CHAR_BITS
#define CHAR_MIN SCHAR_MIN /* minimum char value */
#define CHAR_MAX SCHAR_MAX /* maximum char value */

#define SHRT_MIN (-32767 - 1) /* minimum signed   short value */
#define SHRT_MAX 32767        /* maximum signed   short value */
#define USHRT_MAX 65535       /* maximum unsigned short value */

#define LONG_MIN (-2147483647L - 1) /* minimum signed   long value */
#define LONG_MAX 2147483647L        /* maximum signed   long value */
#define ULONG_MAX 4294967295UL      /* maximum unsigned long value */

#define INT_MIN LONG_MIN   /* minimum signed   int value */
#define INT_MAX LONG_MAX   /* maximum signed   int value */
#define UINT_MAX ULONG_MAX /* maximum unsigned int value */

#if defined(__cplusplus) || __STDC_VERSION__ >= 199901L
#    define LLONG_MIN (-9223372036854775807LL - 1) /* minimum signed   long long value */
#    define LLONG_MAX 9223372036854775807LL        /* maximum signed   long long value */
#    define ULLONG_MAX 18446744073709551615ULL     /* maximum unsigned long long  value */
#endif

#define _I64_MIN (-9223372036854775807i64 - 1) /* minimum signed   __int64 value */
#define _I64_MAX 9223372036854775807i64        /* maximum signed   __int64 value */
#define _UI64_MAX 18446744073709551615ui64     /* maximum unsigned __int64 value */

#define BITINT_MAXWIDTH 16384


#define BOOL_WIDTH 1

#if __STDC_VERSION >= 202311L
#    define SCHAR_WIDTH CHAR_BITS
#    define UCHAR_WIDTH CHAR_BITS
#    define USHRT_WIDTH (CHAR_BITS * sizeof(unsigned short))
#    define UINT_WIDTH (CHAR_BITS * sizeof(unsigned int))
#    define ULONG_WIDTH (CHAR_BITS * sizeof(unsigned long))
#    define ULLONG_WIDTH (CHAR_BITS * sizeof(unsigned long long))
#endif

#endif /* __LIMITS_H */
