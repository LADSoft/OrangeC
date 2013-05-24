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

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h>

#include "ctype_u.h"

int __Inflate(char *out, char *in);

#pragma startup expand 225

wchar_t _RTL_DATA * __ctype_u ;
const wchar_t *__ctype_u_tolower; /* Case conversions.  */
const wchar_t *__ctype_u_toupper; /* Case conversions.  */
static struct props
{
    char *name ;
    wctype_t type ;
} proplist[] = { "alnum", _IS_ALNUM, "alpha", _IS_ALPHA, "cntrl", _IS_CTL,
            "digit", _IS_DIG, "graph", _IS_GPH, "lower" , _IS_LOW, "print", _IS_PRINT,
            "punct", _IS_PUN, "space", _IS_SP,  "upper", _IS_UPP, "xdigit", _IS_HEX,
            "blank", _IS_BLK} ;

static void expand(void)
{
    int len = *(int *)__ctype_u_zip;
    __ctype_u = (wchar_t *)calloc(len,1);
    if (__ctype_u) {
        __Inflate((char *)__ctype_u,__ctype_u_zip + 4);
        __ctype_u_tolower = __ctype_u + 0x10000;
        __ctype_u_toupper = __ctype_u + 0x20000;
    }
}

int _RTL_FUNC iswctype (wint_t wc, wctype_t desc)
{
  if (wc != WEOF && wc < 0x10000)
      return __ctype_u[wc] & desc;
  return 0;
}
wctype_t _RTL_FUNC wctype (const char *property)
{
    int i;
    for (i=0; i < sizeof(proplist)/ sizeof(struct props); i++)
        if (!strcmp(property, proplist[i].name))
            return proplist[i].type;
    return 0;
}
int _RTL_FUNC (iswalnum) (wint_t __wc)
{
    return iswctype(__wc, _IS_ALNUM);
}
int _RTL_FUNC (iswalpha) (wint_t __wc)
{
    return iswctype(__wc, _IS_ALPHA);
}

int _RTL_FUNC (iswcntrl) (wint_t __wc)
{
    return iswctype(__wc, _IS_CTL);
}

int _RTL_FUNC (iswdigit) (wint_t __wc)
{
    return iswctype(__wc, _IS_DIG);
}

int _RTL_FUNC (iswgraph) (wint_t __wc)
{
    return iswctype(__wc, _IS_GPH);
}

int _RTL_FUNC (iswlower) (wint_t __wc)
{
    return iswctype(__wc, _IS_LOW);
}

int _RTL_FUNC (iswprint) (wint_t __wc)
{
    return iswctype(__wc, _IS_PRINT);
}

int _RTL_FUNC (iswpunct) (wint_t __wc)
{
    return iswctype(__wc, _IS_PUN);
}

int _RTL_FUNC (iswspace) (wint_t __wc)
{
    return iswctype(__wc, _IS_SP);
}

int _RTL_FUNC (iswupper) (wint_t __wc)
{
    return iswctype(__wc, _IS_UPP);
}

int _RTL_FUNC (iswxdigit) (wint_t __wc)
{
    return iswctype(__wc, _IS_HEX);
}

int _RTL_FUNC (iswblank) (wint_t __wc)
{
    return iswctype(__wc, _IS_BLK);
}

wctrans_t _RTL_FUNC  wctrans(const char *s)
{
    if (!strcmp(s,"tolower"))
        return __ctype_u_tolower ;
    if (!strcmp(s,"toupper"))
        return __ctype_u_toupper ;
    return 0;
}
wint_t _RTL_FUNC towctrans(wint_t __wc, wctrans_t prop)
{
    
    if (prop && __wc < 0x10000) {
        if (prop[__wc])
            return prop[__wc];
        return __wc;
    }
    return __wc;
}
wint_t _RTL_FUNC  (towlower)(wint_t __wc)
{
    return towctrans(__wc, __ctype_u_toupper);
}
wint_t _RTL_FUNC  (towupper)(wint_t __wc)
{
    return towctrans(__wc, __ctype_u_toupper);
}