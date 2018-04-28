/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 */

/*  string.h

    Definitions for memory and string functions.

*/

#ifndef __STRINGS_H
#define __STRINGS_H

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#ifdef __cplusplus
namespace __STD_NS__ {
extern "C" {
#endif

int          _RTL_INTRINS _IMPORT bcmp(const void *, const void *, size_t);

void         _RTL_INTRINS _IMPORT bcopy(const void *, void *, size_t);

void         _RTL_INTRINS _IMPORT bzero(void *, size_t);

int          _RTL_INTRINS _IMPORT ffs(int);

char         _RTL_INTRINS _IMPORT *index(const char *, int);

char         _RTL_INTRINS _IMPORT *rindex(const char *, int);

int          _RTL_INTRINS _IMPORT strcasecmp(const char *, const char *);

int          _RTL_INTRINS _IMPORT strncasecmp(const char *, const char *, size_t);

/*
#define 	bzero(s,n) memset(s,0,n)
#define 	bcopy(src,dest,n) memmove(dest,src,n)
#define 	bcmp(a,b,n) memcmp(a,b,n)
#define 	index(a,b) strchr(a,b)
#define 	rindex(a,b) strrchr(a,b)
*/
#ifdef __cplusplus
};
};
#endif

#endif  /* __STRING_H */
#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__STRINGS_H_USING_LIST)
#define __STRINGS_H_USING_LIST
    using __STD_NS_QUALIFIER bcmp;
    using __STD_NS_QUALIFIER bcopy;
    using __STD_NS_QUALIFIER bzero;
    using __STD_NS_QUALIFIER ffs;
    using __STD_NS_QUALIFIER index;
    using __STD_NS_QUALIFIER rindex;
    using __STD_NS_QUALIFIER strcasecmp;
    using __STD_NS_QUALIFIER strncasecmp;
#endif /* __USING_CNAME__ */
