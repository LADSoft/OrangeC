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
#ifndef __FENV_H
#define __FENV_H

#pragma pack(1)

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#ifdef __cplusplus
namespace __STD_NS__ {
extern "C" {
#endif

typedef struct {
    unsigned short excInvalid : 1;
    unsigned short excDenormal : 1;
    unsigned short excZero : 1;
    unsigned short excOverflow : 1;
    unsigned short excUnderflow : 1;
    unsigned short excPrecision : 1;
    unsigned short excStack : 1;
    unsigned short errorSummary : 1;
    unsigned short filler: 8 ;
} fexcept_t;

typedef struct {
    unsigned short controlWord ;
    unsigned short __null1;
    unsigned short statusWord ;
    unsigned short __null2;
    unsigned short tagWord ;
    unsigned short __null3;
    unsigned int   fIP;
    unsigned short fCS1;
    unsigned short __null4;
    unsigned int   fOp;
    unsigned short fCS2;
    unsigned short __null5;
} fenv_t;

#define FE_DIVBYZERO    0x0004
#define FE_INEXACT      0x0020
#define FE_INVALID      0x0001
#define FE_OVERFLOW     0x0008
#define FE_UNDERFLOW    0x0010
#define FE_DENORMAL     0x0002
#define FE_STACKFAULT   0x0040
#define FE_ERRORSUMMARY 0x0080

#define FE_ALL_EXCEPT (FE_DIVBYZERO | FE_INEXACT | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW\
                        | FE_DENORMAL | FE_STACKFAULT)

#define FE_DOWNWARD     1
#define FE_TONEAREST    0
#define FE_TOWARDZERO   3
#define FE_UPWARD       2

extern fenv_t * const __fe_default_env;

#define FE_DFL_ENV  (__fe_default_env)

int feclearexcept(int __excepts);
int fegetexceptflag(fexcept_t *__flagp, int __excepts);
int feraiseexcept(int __excepts);
int fesetexceptflag(fexcept_t *__flagp, int __excepts);
int fetestexcept(int __excepts);
int fegetround(void);
int fesetround(int __round);
int fegetenv(fenv_t *__envp);
int feholdexcept(fenv_t *__envp);
int fesetenv(const fenv_t *__envp);
int feupdateenv(const fenv_t *__envp);

#ifdef __cplusplus
}
}
#endif

#pragma pack()

#endif /* __FENV_H */
#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__FENV_H_USING_LIST)
#define __FENV_H_USING_LIST
	using __STD_NS_QUALIFIER feclearexcept;
	using __STD_NS_QUALIFIER fegetexceptflag;
	using __STD_NS_QUALIFIER feraiseexcept;
	using __STD_NS_QUALIFIER fesetexceptflag;
	using __STD_NS_QUALIFIER fetestexcept;
	using __STD_NS_QUALIFIER fegetround;
	using __STD_NS_QUALIFIER fesetround;
	using __STD_NS_QUALIFIER fegetenv;
	using __STD_NS_QUALIFIER feholdexcept;
	using __STD_NS_QUALIFIER fesetenv;
	using __STD_NS_QUALIFIER feupdateenv;
    using __STD_NS_QUALIFIER fexcept_t;
    using __STD_NS_QUALIFIER fenv_t;
    using __STD_NS_QUALIFIER __fe_default_env;
#endif
