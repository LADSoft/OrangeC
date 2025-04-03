/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

#include <stddef.h>
#include <ctype.h>
#include <locale.h>
#include "_locale.h"

#define ct_data (*(CTYPE_DATA*)(__locale_data[LC_CTYPE]))

// yes we are redefining it without a const for this one file only
unsigned short const _RTL_DATA* _pctype;

extern MONETARY_DATA C_monetary_data;

static char C_qualified_name[] = "C";

ABBREVIATED_CTYPE_DATA C_ctype_data = {&C_qualified_name,
                                       0,
                                       {
                                           _IS_CTL,
                                           _IS_CTL,
                                           _IS_CTL,
                                           _IS_CTL,
                                           _IS_CTL,
                                           _IS_CTL,
                                           _IS_CTL,
                                           _IS_CTL,
                                           _IS_CTL,
                                           _IS_SP | _IS_CTL,
                                           _IS_SP | _IS_CTL,
                                           _IS_SP | _IS_CTL,
                                           _IS_SP | _IS_CTL,
                                           _IS_SP | _IS_CTL,
                                           _IS_CTL,
                                           _IS_CTL,
                                           _IS_CTL,
                                           _IS_CTL,
                                           _IS_CTL,
                                           _IS_CTL,
                                           _IS_CTL,
                                           _IS_CTL,
                                           _IS_CTL,
                                           _IS_CTL,
                                           _IS_CTL,
                                           _IS_CTL,
                                           _IS_CTL,
                                           _IS_CTL,
                                           _IS_CTL,
                                           _IS_CTL,
                                           _IS_CTL,
                                           _IS_CTL,
                                           _IS_SP | _IS_BLK,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_DIG | _IS_HEX,
                                           _IS_DIG | _IS_HEX,
                                           _IS_DIG | _IS_HEX,
                                           _IS_DIG | _IS_HEX,
                                           _IS_DIG | _IS_HEX,
                                           _IS_DIG | _IS_HEX,
                                           _IS_DIG | _IS_HEX,
                                           _IS_DIG | _IS_HEX,
                                           _IS_DIG | _IS_HEX,
                                           _IS_DIG | _IS_HEX,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_UPP | _IS_HEX,
                                           _IS_UPP | _IS_HEX,
                                           _IS_UPP | _IS_HEX,
                                           _IS_UPP | _IS_HEX,
                                           _IS_UPP | _IS_HEX,
                                           _IS_UPP | _IS_HEX,
                                           _IS_UPP,
                                           _IS_UPP,
                                           _IS_UPP,
                                           _IS_UPP,
                                           _IS_UPP,
                                           _IS_UPP,
                                           _IS_UPP,
                                           _IS_UPP,
                                           _IS_UPP,
                                           _IS_UPP,
                                           _IS_UPP,
                                           _IS_UPP,
                                           _IS_UPP,
                                           _IS_UPP,
                                           _IS_UPP,
                                           _IS_UPP,
                                           _IS_UPP,
                                           _IS_UPP,
                                           _IS_UPP,
                                           _IS_UPP,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_LOW | _IS_HEX,
                                           _IS_LOW | _IS_HEX,
                                           _IS_LOW | _IS_HEX,
                                           _IS_LOW | _IS_HEX,
                                           _IS_LOW | _IS_HEX,
                                           _IS_LOW | _IS_HEX,
                                           _IS_LOW,
                                           _IS_LOW,
                                           _IS_LOW,
                                           _IS_LOW,
                                           _IS_LOW,
                                           _IS_LOW,
                                           _IS_LOW,
                                           _IS_LOW,
                                           _IS_LOW,
                                           _IS_LOW,
                                           _IS_LOW,
                                           _IS_LOW,
                                           _IS_LOW,
                                           _IS_LOW,
                                           _IS_LOW,
                                           _IS_LOW,
                                           _IS_LOW,
                                           _IS_LOW,
                                           _IS_LOW,
                                           _IS_LOW,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_PUN,
                                           _IS_CTL,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                       }};
NUMERIC_DATA C_numeric_data = {
    &C_qualified_name,
    ".",
    "",
    "",
};
void* __locale_data[LC_LAST + 1] = {0, 0, &C_ctype_data, &C_monetary_data, &C_numeric_data, 0, 0, 0};

void __ctype_init(void) { _pctype = (unsigned short*)(((unsigned char*)__locale_data[LC_CTYPE]) + offsetof(CTYPE_DATA, ctypetab)); }
int _RTL_FUNC(tolower)(int c)
{
    if ((unsigned)c >= 256)
        return c;
    if (ct_data.flags & 1)
        c = ct_data.tolower[c];
    else if (_pctype[c] & _IS_UPP)
        c += 32;
    return c;
}
int _RTL_FUNC(toupper)(int c)
{
    if ((unsigned)c >= 256)
        return c;
    if (ct_data.flags & 1)
        c = ct_data.toupper[c];
    else if (_pctype[c] & _IS_LOW)
        c -= 32;
    return c;
}
int _RTL_FUNC(_tolower)(int c) { return tolower(c); }
int _RTL_FUNC(_toupper)(int c) { return toupper(c); }
int _RTL_FUNC(_ltolower)(int c) { return tolower(c); }
int _RTL_FUNC(_ltoupper)(int c) { return toupper(c); }
int _RTL_FUNC(isspace)(int c)
{
    if ((unsigned)c >= 256)
        return 0;
    return (_pctype[c] & _IS_SP);
}
int _RTL_FUNC(isblank)(int c)
{
    if ((unsigned)c >= 256)
        return 0;
    return (_pctype[c] & _IS_BLK);
}
int _RTL_FUNC(isalpha)(int c)
{
    if ((unsigned)c >= 256)
        return 0;
    return (_pctype[c] & (_IS_UPP | _IS_LOW));
}
int _RTL_FUNC(ispunct)(int c)
{
    if ((unsigned)c >= 256)
        return 0;
    return (_pctype[c] & _IS_PUN);
}
int _RTL_FUNC(islower)(int c)
{
    if ((unsigned)c >= 256)
        return 0;
    return (_pctype[c] & _IS_LOW);
}
int _RTL_FUNC(isupper)(int c)
{
    if ((unsigned)c >= 256)
        return 0;
    return (_pctype[c] & _IS_UPP);
}
int _RTL_FUNC(isprint)(int c)
{
    if ((unsigned)c >= 256)
        return 0;
    return (_pctype[c] & (_IS_PRINT));
}

int _RTL_FUNC(isalnum)(int c)
{
    if ((unsigned)c >= 256)
        return 0;
    return (_pctype[c] & (_IS_UPP | _IS_LOW | _IS_DIG));
}
int _RTL_FUNC(isgraph)(int c)
{
    if ((unsigned)c >= 256)
        return 0;
    return (_pctype[c] & _IS_GRAPH);
}
int _RTL_FUNC(isdigit)(int c)
{
    if ((unsigned)c >= 256)
        return 0;
    return (_pctype[c] & _IS_DIG);
}
int _RTL_FUNC(isxdigit)(int c)
{
    if ((unsigned)c >= 256)
        return 0;
    return (_pctype[c] & _IS_HEX);
}
int _RTL_FUNC(toascii)(int c) { return (c & 0x7f); }
int _RTL_FUNC(__toascii)(int c) { return (c & 0x7f); }
int _RTL_FUNC(isascii)(int c)
{
    if ((unsigned)c >= 256)
        return 0;
    return (c < 0x80);
}
int _RTL_FUNC(__isascii)(int c)
{
    if ((unsigned)c >= 256)
        return 0;
    return (c < 0x80);
}
int _RTL_FUNC(iscntrl)(int c)
{
    if ((unsigned)c >= 256)
        return 0;
    return (_pctype[c] & _IS_CTL);
}
int _RTL_FUNC(_isctype)(int c, int flag)
{
    if ((unsigned)c >= 256)
        return 0;
    return (_pctype[c] & flag);
}
int _RTL_FUNC(__iscsymf)(int c) { return isalpha(c) || c == '_'; }
int _RTL_FUNC(__iscsym)(int c) { return isalnum(c) || c == '_'; }
