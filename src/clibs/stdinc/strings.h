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

/*  string.h

    Definitions for memory and string functions.

*/

#pragma GCC system_header

#ifndef __STRINGS_H
#define __STRINGS_H

#    include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef RC_INVOKED
    int _RTL_INTRINS _IMPORT bcmp(const void*, const void*, size_t);

    void _RTL_INTRINS _IMPORT bcopy(const void*, void*, size_t);

    void _RTL_INTRINS _IMPORT bzero(void*, size_t);

    int _RTL_INTRINS _IMPORT ffs(int);

    char _RTL_INTRINS _IMPORT* index(const char* ZSTR, int);

    char _RTL_INTRINS _IMPORT* rindex(const char* ZSTR, int);

    int _RTL_INTRINS _IMPORT strcasecmp(const char* ZSTR, const char* ZSTR);

    int _RTL_INTRINS _IMPORT strncasecmp(const char* ZSTR, const char* ZSTR, size_t);
#endif
/*
#define 	bzero(s,n) memset(s,0,n)
#define 	bcopy(src,dest,n) memmove(dest,src,n)
#define 	bcmp(a,b,n) memcmp(a,b,n)
#define 	index(a,b) strchr(a,b)
#define 	rindex(a,b) strrchr(a,b)
*/
#ifdef __cplusplus
};
#endif

#endif /* __STRING_H */
