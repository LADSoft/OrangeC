/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

/*  ctype.h

    Defines the locale aware ctype macros.

*/

#ifndef __pctype_H
#define __pctype_H

#ifndef __STDDEF_H
#    include <stddef.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif
#ifndef RC_INVOKED
    extern unsigned short const _IMPORT _RTL_DATA* _pctype;
#endif
    /* character classes */

#ifndef _IS_CONSTANTS
#    define _IS_CONSTANTS

#    define _IS_UPP 1   /* upper case */
#    define _IS_LOW 2   /* lower case */
#    define _IS_DIG 4   /* digit */
#    define _IS_SP 8    /* space */
#    define _IS_PUN 16  /* punctuation */
#    define _IS_CTL 32  /* control */
#    define _IS_BLK 64  /* blank */
#    define _IS_HEX 128 /* [0..9] or [A-F] or [a-f] */
#    define _IS_GPH 512

#    define _IS_ALPHA (0x100 | _IS_UPP | _IS_LOW)
#    define _IS_ALNUM (_IS_DIG | _IS_ALPHA)
#    define _IS_GRAPH (_IS_ALNUM | _IS_HEX | _IS_PUN)
#    define _IS_PRINT (_IS_GRAPH | _IS_BLK)
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

#ifndef RC_INVOKED
    int _RTL_FUNC _IMPORT isalnum(int __c);
    int _RTL_FUNC _IMPORT isalpha(int __c);
    int _RTL_FUNC _IMPORT isblank(int __c);
    int _RTL_FUNC _IMPORT iscntrl(int __c);
    int _RTL_FUNC _IMPORT isdigit(int __c);
    int _RTL_FUNC _IMPORT isgraph(int __c);
    int _RTL_FUNC _IMPORT islower(int __c);
    int _RTL_FUNC _IMPORT isprint(int __c);
    int _RTL_FUNC _IMPORT ispunct(int __c);
    int _RTL_FUNC _IMPORT isspace(int __c);
    int _RTL_FUNC _IMPORT isupper(int __c);
    int _RTL_FUNC _IMPORT isxdigit(int __c);
    int _RTL_FUNC _IMPORT isascii(int __c);
    int _RTL_FUNC _IMPORT toascii(int);
    int _RTL_FUNC _IMPORT __isascii(int);
    int _RTL_FUNC _IMPORT __toascii(int);

    int _RTL_FUNC _IMPORT tolower(int __ch);
    int _RTL_FUNC _IMPORT _ltolower(int __ch);
    int _RTL_FUNC _IMPORT toupper(int __ch);
    int _RTL_FUNC _IMPORT _ltoupper(int __ch);

    int _RTL_FUNC _IMPORT _isctype(int, int);
    int _RTL_FUNC _IMPORT __iscsymf(int);
    int _RTL_FUNC _IMPORT __iscsym(int);
#endif
#ifdef __cplusplus
};
#endif

#ifndef __cplusplus
#    define isalnum(c) (_pctype[(c)] & (_IS_ALNUM))

#    define isalpha(c) (_pctype[(c)] & (_IS_ALPHA))

#    define isblank(c) (_pctype[(c)] & (_IS_BLK))

#    define iscntrl(c) (_pctype[(c)] & (_IS_CTL))

#    define isdigit(c) (_pctype[(c)] & (_IS_DIG))

#    define isgraph(c) (_pctype[(c)] & (_IS_GRAPH))

#    define islower(c) (_pctype[(c)] & (_IS_LOW))

#    define isprint(c) (_pctype[(c)] & (_IS_PRINT))

#    define ispunct(c) (_pctype[(c)] & (_IS_PUN))

#    define isspace(c) (_pctype[(c)] & (_IS_SP))

#    define isupper(c) (_pctype[(c)] & (_IS_UPP))

#    define isxdigit(c) (_pctype[(c)] & (_IS_HEX))

#    define _toupper(c) ((c) + 'A' - 'a')
#    define _tolower(c) ((c) + 'a' - 'A')
#    define isascii(c) ((unsigned)(c) < 128)
#    define toascii(c) ((c)&0x7f)
#endif

#endif