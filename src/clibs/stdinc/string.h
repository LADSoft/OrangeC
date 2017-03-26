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
/*  string.h

    Definitions for memory and string functions.

*/

#ifndef __STRING_H
#define __STRING_H

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#ifdef __cplusplus
//namespace __STD_NS__ {
extern "C" {
#endif

int          _RTL_INTRINS _IMPORT bcmp(const void *, const void *, size_t);

void         _RTL_INTRINS _IMPORT bcopy(const void *, void *, size_t);

void         _RTL_INTRINS _IMPORT bzero(void *, size_t);

int          _RTL_INTRINS _IMPORT ffs(int);

//char         _RTL_INTRINS _IMPORT *index(const char *, int);

//char         _RTL_INTRINS _IMPORT *rindex(const char *, int);

int          _RTL_INTRINS _IMPORT strcasecmp(const char *, const char *);

int          _RTL_INTRINS _IMPORT strncasecmp(const char *, const char *, size_t);

int          _RTL_INTRINS _IMPORT memcmp(const void *__s1,
                                       const void *__s2, size_t __n);
void *       _RTL_INTRINS _IMPORT memcpy(void *restrict __dest, const void *restrict __src,
                                       size_t __n);
void *       _RTL_INTRINS _IMPORT memmove(void *__dest, const void *__src,
                                        size_t __n);
void *       _RTL_INTRINS _IMPORT memset(void *__s, int __c, size_t __n);
char *       _RTL_INTRINS _IMPORT strcat(char *restrict __dest, const char *restrict __src);
size_t 		 _RTL_FUNC _IMPORT strlcat (char * dst, const char * src, size_t n);
int          _RTL_INTRINS _IMPORT strcmp(const char *__s1, const char *__s2);
char *       _RTL_INTRINS _IMPORT strcpy(char *restrict __dest, const char *restrict __src);
size_t       _RTL_INTRINS _IMPORT strlcpy (char * dst, const char * src, size_t n);
char *       _RTL_INTRINS _IMPORT stpcpy(char *restrict __dest, const char *restrict __src);
size_t       _RTL_FUNC _IMPORT strcspn(const char *__s1, const char *__s2);
char *       _RTL_FUNC _IMPORT strdup(const char *__src);
char *       _RTL_FUNC _IMPORT strerror(int __errnum);
size_t       _RTL_INTRINS _IMPORT strlen(const char *__s);
char *       _RTL_INTRINS _IMPORT strncat(char *restrict __dest, const char *restrict __src,
                                        size_t __maxlen);
int          _RTL_INTRINS _IMPORT strncmp(const char *__s1, const char *__s2,
                                        size_t __maxlen);
char *       _RTL_INTRINS _IMPORT strncpy(char *restrict __dest, const char *restrict __src,
                                        size_t __maxlen);
char *       _RTL_INTRINS _IMPORT stpncpy(char *restrict __dest, const char *restrict __src,
                                        size_t __maxlen);
size_t       _RTL_FUNC _IMPORT strspn(const char *__s1, const char *__s2);
char *       _RTL_FUNC _IMPORT strtok(char *restrict __s1, const char *restrict __s2);
char *       _RTL_FUNC _IMPORT _strerror(const char *__s);


void *       _RTL_INTRINS _IMPORT memchr(const void *__s, int __c, size_t __n);
char *       _RTL_INTRINS _IMPORT strchr(const char * __s, int __c);
char *       _RTL_INTRINS _IMPORT strrchr(const char *__s, int __c);
char *       _RTL_FUNC _IMPORT strpbrk(const char *__s1, const char *__s2);
char *       _RTL_FUNC _IMPORT strstr(const char *__s1, const char *__s2);

int          _RTL_FUNC _IMPORT _lstrcoll(const char *__s1, const char *__s2);
size_t       _RTL_FUNC _IMPORT _lstrxfrm(char *restrict __s1, const char *restrict __s2,
                                            size_t __n );
int          _RTL_FUNC _IMPORT strcoll(const char *__s1, const char *__s2);
size_t       _RTL_FUNC _IMPORT strxfrm(char *restrict __s1, const char *restrict __s2,
                                          size_t __n );

int    		 _RTL_FUNC _IMPORT memicmp(const void *, const void *, size_t);
void *       _RTL_FUNC _IMPORT memccpy(void *, const void *, int, size_t);
char * 		 _RTL_FUNC _IMPORT strset(char *, int);
char *  	 _RTL_FUNC _IMPORT strnset(char *, int, size_t);
char *		 _RTL_FUNC _IMPORT strrev(char *);
char *       _RTL_FUNC _IMPORT strupr(char * __s);
char *       _RTL_FUNC _IMPORT strlwr(char * __s);
int          _RTL_FUNC _IMPORT strnicmp(const char *__s1, const char *__s2, size_t __n);
int          _RTL_FUNC _IMPORT stricmp(const char *__s1, const char *__s2);
int          _RTL_FUNC _IMPORT strncmpi(const char *__s1, const char *__s2, size_t __n);
int          _RTL_FUNC _IMPORT strcmpi(const char *__s1, const char *__s2);

int    		 _RTL_FUNC _IMPORT _memicmp(const void *, const void *, unsigned int);
void *       _RTL_FUNC _IMPORT _memccpy(void *, const void *, int, unsigned int);
char * 		 _RTL_FUNC _IMPORT _strset(char *, int);
char * 		 _RTL_FUNC _IMPORT _strnset(char *, int, size_t);
char * 		 _RTL_FUNC _IMPORT _strrev(char *);

char *       _RTL_FUNC _IMPORT _strdup(const char *string);
int    		 _RTL_FUNC _IMPORT _stricmp(const char *, const char *);
int    		 _RTL_FUNC _IMPORT _strcmpi(const char *, const char *);
int    		 _RTL_FUNC _IMPORT _strnicmp(const char *, const char *, size_t);
int    		 _RTL_FUNC _IMPORT _strncmpi(const char *, const char *, size_t);
char * 		 _RTL_FUNC _IMPORT _strlwr(char *);
char * 		 _RTL_FUNC _IMPORT _strupr(char *);

#if defined(__USELOCALES__)
#define  strupr   _lstrupr
#define  strlwr   _lstrlwr
#define  strcoll  _lstrcoll
#define  strxfrm  _lstrxfrm
#endif  /* __USELOCALES__ */


#ifdef __cplusplus
//};
};
#endif

#endif  /* __STRING_H */
#if 0
#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__STRING_H_USING_LIST)
#define __STRING_H_USING_LIST
    using __STD_NS_QUALIFIER memcmp;
    using __STD_NS_QUALIFIER memcpy;
    using __STD_NS_QUALIFIER memccpy;
    using __STD_NS_QUALIFIER memmove;
    using __STD_NS_QUALIFIER memset;
    using __STD_NS_QUALIFIER strcat;
    using __STD_NS_QUALIFIER strcmp;
    using __STD_NS_QUALIFIER strcpy;
    using __STD_NS_QUALIFIER stpcpy;
    using __STD_NS_QUALIFIER strcspn;
    using __STD_NS_QUALIFIER strdup;
    using __STD_NS_QUALIFIER strerror;
    using __STD_NS_QUALIFIER strlen;
    using __STD_NS_QUALIFIER strncat;
    using __STD_NS_QUALIFIER strncmp;
    using __STD_NS_QUALIFIER strncpy;
    using __STD_NS_QUALIFIER stpncpy;
    using __STD_NS_QUALIFIER strspn;
    using __STD_NS_QUALIFIER strtok;
    using __STD_NS_QUALIFIER _strerror;
    using __STD_NS_QUALIFIER memchr;
    using __STD_NS_QUALIFIER strchr;
    using __STD_NS_QUALIFIER strrchr;
    using __STD_NS_QUALIFIER strpbrk;
    using __STD_NS_QUALIFIER strstr;
    using __STD_NS_QUALIFIER _lstrcoll;
    using __STD_NS_QUALIFIER _lstrxfrm;
    using __STD_NS_QUALIFIER strcoll;
    using __STD_NS_QUALIFIER strxfrm;
    using __STD_NS_QUALIFIER strupr;
    using __STD_NS_QUALIFIER strlwr;
    using __STD_NS_QUALIFIER strnicmp;
    using __STD_NS_QUALIFIER stricmp;
    using __STD_NS_QUALIFIER strncmpi;
    using __STD_NS_QUALIFIER strcmpi;
    using __STD_NS_QUALIFIER _strdup;
    using __STD_NS_QUALIFIER _memicmp;
    using __STD_NS_QUALIFIER memicmp;
    using __STD_NS_QUALIFIER _strset;
    using __STD_NS_QUALIFIER strset;
    using __STD_NS_QUALIFIER _strnset;
    using __STD_NS_QUALIFIER strnset;
    using __STD_NS_QUALIFIER _strrev;
    using __STD_NS_QUALIFIER strrev;
    using __STD_NS_QUALIFIER _stricmp;
    using __STD_NS_QUALIFIER _strcmpi;
    using __STD_NS_QUALIFIER _strnicmp;
    using __STD_NS_QUALIFIER _strncmpi;
    using __STD_NS_QUALIFIER _strlwr;
    using __STD_NS_QUALIFIER _strupr;
    using __STD_NS_QUALIFIER strlcpy;
    using __STD_NS_QUALIFIER strlcat;
    using __STD_NS_QUALIFIER bcmp;
    using __STD_NS_QUALIFIER bcopy;
    using __STD_NS_QUALIFIER bzero;
    using __STD_NS_QUALIFIER ffs;
    using __STD_NS_QUALIFIER strcasecmp;
    using __STD_NS_QUALIFIER strncasecmp;
#endif /* __USING_CNAME__ */
#endif