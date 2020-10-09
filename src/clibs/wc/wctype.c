/* Software License Agreement
 *
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h>

#include "ctype_u.h"

int __Inflate(char* out, char* in);

#pragma startup expand 225

wchar_t _RTL_DATA* __ctype_u;
const wchar_t* __ctype_u_tolower; /* Case conversions.  */
const wchar_t* __ctype_u_toupper; /* Case conversions.  */
static struct props
{
    char* name;
    wctype_t type;
} proplist[] = {"alnum", _IS_ALNUM, "alpha", _IS_ALPHA, "cntrl", _IS_CTL, "digit", _IS_DIG, "graph",  _IS_GPH, "lower", _IS_LOW,
                "print", _IS_PRINT, "punct", _IS_PUN,   "space", _IS_SP,  "upper", _IS_UPP, "xdigit", _IS_HEX, "blank", _IS_BLK};

static void expand(void)
{
    int len = *(int*)__ctype_u_zip;
    __ctype_u = (wchar_t*)calloc(len, 1);
    if (__ctype_u)
    {
        __Inflate((char*)__ctype_u, __ctype_u_zip + 4);
        __ctype_u_tolower = __ctype_u + 0x10000;
        __ctype_u_toupper = __ctype_u + 0x20000;
    }
}

int _RTL_FUNC iswctype(wint_t wc, wctype_t desc)
{
    if (wc != WEOF && wc < 0x10000)
        return __ctype_u[wc] & desc;
    return 0;
}
wctype_t _RTL_FUNC wctype(const char* property)
{
    int i;
    for (i = 0; i < sizeof(proplist) / sizeof(struct props); i++)
        if (!strcmp(property, proplist[i].name))
            return proplist[i].type;
    return 0;
}
int _RTL_FUNC(iswalnum)(wint_t __wc) { return iswctype(__wc, _IS_ALNUM); }
int _RTL_FUNC(iswalpha)(wint_t __wc) { return iswctype(__wc, _IS_ALPHA); }

int _RTL_FUNC(iswcntrl)(wint_t __wc) { return iswctype(__wc, _IS_CTL); }

int _RTL_FUNC(iswdigit)(wint_t __wc) { return iswctype(__wc, _IS_DIG); }

int _RTL_FUNC(iswgraph)(wint_t __wc) { return iswctype(__wc, _IS_GPH); }

int _RTL_FUNC(iswlower)(wint_t __wc) { return iswctype(__wc, _IS_LOW); }

int _RTL_FUNC(iswprint)(wint_t __wc) { return iswctype(__wc, _IS_PRINT); }

int _RTL_FUNC(iswpunct)(wint_t __wc) { return iswctype(__wc, _IS_PUN); }

int _RTL_FUNC(iswspace)(wint_t __wc) { return iswctype(__wc, _IS_SP); }

int _RTL_FUNC(iswupper)(wint_t __wc) { return iswctype(__wc, _IS_UPP); }

int _RTL_FUNC(iswxdigit)(wint_t __wc) { return iswctype(__wc, _IS_HEX); }

int _RTL_FUNC(iswblank)(wint_t __wc) { return iswctype(__wc, _IS_BLK); }

wctrans_t _RTL_FUNC wctrans(const char* s)
{
    if (!strcmp(s, "tolower"))
        return __ctype_u_tolower;
    if (!strcmp(s, "toupper"))
        return __ctype_u_toupper;
    return 0;
}
wint_t _RTL_FUNC towctrans(wint_t __wc, wctrans_t prop)
{

    if (prop && __wc < 0x10000)
    {
        if (prop[__wc])
            return prop[__wc];
        return __wc;
    }
    return __wc;
}
wint_t _RTL_FUNC(towlower)(wint_t __wc) { return towctrans(__wc, __ctype_u_toupper); }
wint_t _RTL_FUNC(towupper)(wint_t __wc) { return towctrans(__wc, __ctype_u_toupper); }