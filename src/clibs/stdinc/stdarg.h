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
 #ifndef __STDARG_H
#define __STDARG_H

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#ifdef __cplusplus
namespace __STD_NS__ {
extern "C" {
#endif

#ifdef __MSIL__
typedef void * __va_list__ va_list;
#else
typedef void *va_list;
#endif
#ifdef __cplusplus
} ;
} ;
#endif

#ifdef __MSIL__

#define va_start(ap, parmN) ap = __va_start__()
#define va_arg(ap, type)  (*((type *)__va_arg__(ap, __va_typeof__(type))))
#define va_end(ap)  

#else

#define __sizeof__(x) ((sizeof(x)+sizeof(int)-1) & ~(sizeof(int)-1))

#define va_start(ap, parmN) ap = (va_list)((char *)(&parmN)+sizeof(parmN))
#define va_arg(ap, type) (*(type *)(((*(char **)&(ap))+=__sizeof__(type))-(__sizeof__(type))))
#define va_end(ap)

#endif

#if  __STDC_VERSION__ >= 199901L
#define va_copy(ap_d, ap_s)  (void)(ap_d = ap_s)
#endif


#define _va_ptr             (...)

#endif  /* __STDARG_H */

#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__STDARG_H_USING_LIST)
#define __STDARG_H_USING_LIST
using __STD_NS_QUALIFIER va_list ;
#endif
