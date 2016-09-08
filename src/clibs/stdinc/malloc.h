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
/*  malloc.h

    memory management functions and variables.

*/

#ifndef __MALLOC_H
#define __MALLOC_H

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#define _HEAPEMPTY      1
#define _HEAPOK         2
#define _FREEENTRY      3
#define _USEDENTRY      4
#define _HEAPEND        5
#define _HEAPCORRUPT    -1
#define _BADNODE        -2
#define _BADVALUE       -3

#ifdef __cplusplus
namespace __STD_NS__ {
extern "C" {
#endif

#ifdef __CC386__
#define _fmalloc(size) farmalloc(size)
#define _ffree(size)  farfree(size)
#endif

void        _RTL_FUNC _IMPORT _MSIL_RTL *calloc(size_t __nitems, size_t __size);
void        _RTL_FUNC _IMPORT _MSIL_RTL free(void *__block);
void        _RTL_FUNC _IMPORT _MSIL_RTL *malloc(size_t __size);
void        _RTL_FUNC _IMPORT _MSIL_RTL *realloc(void *__block, size_t __size);
#ifdef __CC386__
void        _RTL_FUNC _IMPORT _FAR *farmalloc(size_t __size) ;
void        _RTL_FUNC _IMPORT farfree(void _FAR *__block) ;
#endif

#define _alloca(x) __alloca((x))
#define alloca(x) __alloca((x))

#ifdef __cplusplus
} ;
} ;
#endif

#endif  /* __MALLOC_H */
#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__MALLOC_H_USING_LIST)
#define __MALLOC_H_USING_LIST
    using __STD_NS_QUALIFIER calloc ;
    using __STD_NS_QUALIFIER free ;
    using __STD_NS_QUALIFIER malloc ;
    using __STD_NS_QUALIFIER realloc ;
#ifdef __CC386__
    using __STD_NS_QUALIFIER farmalloc ;
    using __STD_NS_QUALIFIER farfree ;
#endif

#endif
