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
