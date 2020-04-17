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

/*  string.h

    Definitions for memory and string functions.

*/

#ifndef __STRING_H
#define __STRING_H

#ifndef __STDDEF_H
#    include <stddef.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    int _RTL_INTRINS _IMPORT bcmp(const void*, const void*, size_t);

    void _RTL_INTRINS _IMPORT bcopy(const void*, void*, size_t);

    void _RTL_INTRINS _IMPORT bzero(void*, size_t);

    int _RTL_INTRINS _IMPORT ffs(int);

    // char         _RTL_INTRINS _IMPORT *index(const char *ZSTR , int);

    // char         _RTL_INTRINS _IMPORT *rindex(const char *ZSTR , int);

    int _RTL_INTRINS _IMPORT strcasecmp(const char *ZSTR , const char *ZSTR );

    int _RTL_INTRINS _IMPORT strncasecmp(const char *ZSTR , const char *ZSTR , size_t);

    int _RTL_INTRINS _IMPORT memcmp(const void* __s1, const void* __s2, size_t __n);
    void* _RTL_INTRINS _IMPORT memcpy(void* restrict __dest, const void* restrict __src, size_t __n);
    void* _RTL_INTRINS _IMPORT memmove(void* __dest, const void* __src, size_t __n);
    void* _RTL_INTRINS _IMPORT memset(void* __s, int __c, size_t __n);
    char *_RTL_INTRINS _IMPORT strcat(char *ZSTR  restrict __dest, const char *ZSTR  restrict __src);
    size_t _RTL_FUNC _IMPORT strlcat(char *ZSTR  dst, const char *ZSTR  src, size_t n);
    int _RTL_INTRINS _IMPORT strcmp(const char *ZSTR  __s1, const char *ZSTR  __s2);
    char *_RTL_INTRINS _IMPORT strcpy(char *ZSTR  restrict __dest, const char *ZSTR  restrict __src);
    size_t _RTL_INTRINS _IMPORT strlcpy(char *ZSTR  dst, const char *ZSTR  src, size_t n);
    char *_RTL_INTRINS _IMPORT stpcpy(char *ZSTR  restrict __dest, const char *ZSTR  restrict __src);
    size_t _RTL_FUNC _IMPORT strcspn(const char *ZSTR  __s1, const char *ZSTR  __s2);
    char *_RTL_FUNC _IMPORT strdup(const char *ZSTR  __src);
    char *_RTL_FUNC _IMPORT strerror(int __errnum);
    size_t _RTL_INTRINS _IMPORT strlen(const char *ZSTR  __s);
    char *_RTL_INTRINS _IMPORT strncat(char *ZSTR  restrict __dest, const char *ZSTR  restrict __src, size_t __maxlen);
    int _RTL_INTRINS _IMPORT strncmp(const char *ZSTR  __s1, const char *ZSTR  __s2, size_t __maxlen);
    char *_RTL_INTRINS _IMPORT strncpy(char *ZSTR  restrict __dest, const char *ZSTR  restrict __src, size_t __maxlen);
    char *_RTL_INTRINS _IMPORT stpncpy(char *ZSTR  restrict __dest, const char *ZSTR  restrict __src, size_t __maxlen);
    size_t _RTL_FUNC _IMPORT strspn(const char *ZSTR  __s1, const char *ZSTR  __s2);
    char *_RTL_FUNC _IMPORT strtok(char *ZSTR  restrict __s1, const char *ZSTR  restrict __s2);
    char *_RTL_FUNC _IMPORT _strerror(const char *ZSTR  __s);

    void* _RTL_INTRINS _IMPORT memchr(const void* __s, int __c, size_t __n);
    char *_RTL_INTRINS _IMPORT strchr(const char *ZSTR  __s, int __c);
    char *_RTL_INTRINS _IMPORT strrchr(const char *ZSTR  __s, int __c);
    char *_RTL_FUNC _IMPORT strpbrk(const char *ZSTR  __s1, const char *ZSTR  __s2);
    char *_RTL_FUNC _IMPORT strstr(const char *ZSTR  __s1, const char *ZSTR  __s2);

    int _RTL_FUNC _IMPORT _lstrcoll(const char *ZSTR  __s1, const char *ZSTR  __s2);
    size_t _RTL_FUNC _IMPORT _lstrxfrm(char *ZSTR  restrict __s1, const char *ZSTR  restrict __s2, size_t __n);
    int _RTL_FUNC _IMPORT strcoll(const char *ZSTR  __s1, const char *ZSTR  __s2);
    size_t _RTL_FUNC _IMPORT strxfrm(char *ZSTR  restrict __s1, const char *ZSTR  restrict __s2, size_t __n);

    int _RTL_FUNC _IMPORT memicmp(const void*, const void*, size_t);
    void* _RTL_FUNC _IMPORT memccpy(void*, const void*, int, size_t);
    char *_RTL_FUNC _IMPORT strset(char *ZSTR , int);
    char *_RTL_FUNC _IMPORT strnset(char *ZSTR , int, size_t);
    char *_RTL_FUNC _IMPORT strrev(char *ZSTR );
    char *_RTL_FUNC _IMPORT strupr(char *ZSTR  __s);
    char *_RTL_FUNC _IMPORT strlwr(char *ZSTR  __s);
    int _RTL_FUNC _IMPORT strnicmp(const char *ZSTR  __s1, const char *ZSTR  __s2, size_t __n);
    int _RTL_FUNC _IMPORT stricmp(const char *ZSTR  __s1, const char *ZSTR  __s2);
    int _RTL_FUNC _IMPORT strncmpi(const char *ZSTR  __s1, const char *ZSTR  __s2, size_t __n);
    int _RTL_FUNC _IMPORT strcmpi(const char *ZSTR  __s1, const char *ZSTR  __s2);

    int _RTL_FUNC _IMPORT _memicmp(const void*, const void*, unsigned int);
    void* _RTL_FUNC _IMPORT _memccpy(void*, const void*, int, unsigned int);
    char *_RTL_FUNC _IMPORT _strset(char *ZSTR , int);
    char *_RTL_FUNC _IMPORT _strnset(char *ZSTR , int, size_t);
    char *_RTL_FUNC _IMPORT _strrev(char *ZSTR );

    char *_RTL_FUNC _IMPORT _strdup(const char *ZSTR  string);
    int _RTL_FUNC _IMPORT _stricmp(const char *ZSTR , const char *ZSTR );
    int _RTL_FUNC _IMPORT _strcmpi(const char *ZSTR , const char *ZSTR );
    int _RTL_FUNC _IMPORT _strnicmp(const char *ZSTR , const char *ZSTR , size_t);
    int _RTL_FUNC _IMPORT _strncmpi(const char *ZSTR , const char *ZSTR , size_t);
    char *_RTL_FUNC _IMPORT _strlwr(char *ZSTR );
    char *_RTL_FUNC _IMPORT _strupr(char *ZSTR );

#if defined(__USELOCALES__)
#    define strupr _lstrupr
#    define strlwr _lstrlwr
#    define strcoll _lstrcoll
#    define strxfrm _lstrxfrm
#endif /* __USELOCALES__ */

#ifdef __cplusplus
};
#endif

#endif /* __STRING_H */