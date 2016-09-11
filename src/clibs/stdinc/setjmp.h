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
/*  setjmp.h

    Defines typedef and functions for setjmp/longjmp.

*/

#ifndef __SETJMP_H
#define __SETJMP_H

#pragma pack(1)

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#ifdef __cplusplus
namespace __STD_NS__ {
extern "C" {
#endif

typedef struct __jmp_buf {
#ifdef _i386_
    unsigned	j_eax;
    unsigned	j_ecx;
    unsigned	j_edx;
    unsigned	j_ebx;
    unsigned	j_esp;
    unsigned	j_ebp;
    unsigned	j_esi;
    unsigned	j_edi;
    unsigned	j_flag;
    unsigned	j_pc;
#else
    unsigned    j_d0;
    unsigned    j_d1;
    unsigned    j_d2;
    unsigned    j_d3;
    unsigned    j_d4;
    unsigned    j_d5;
    unsigned    j_d6;
    unsigned    j_d7;
    unsigned    j_a0;
    unsigned    j_a1;
    unsigned    j_a2;
    unsigned    j_a3;
    unsigned    j_a4;
    unsigned    j_a5;
    unsigned    j_a6;
    unsigned    j_a7;
    unsigned	j_flag;
    unsigned    j_pc;
#endif

}   jmp_buf[1];


void    _RTL_FUNC _IMPORT longjmp(jmp_buf __jmpb, int __retval);
int     _RTL_FUNC _IMPORT setjmp(jmp_buf __jmpb);
int     _RTL_FUNC _IMPORT _setjmp(jmp_buf __jmpb);

#define setjmp(__jmpb)  setjmp(__jmpb)
#define _setjmp(__jmpb) setjmp(__jmpb);

#ifdef __cplusplus
};
};
#endif

#pragma pack()

#endif  /* __SETJMP_H */

#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__SETJMP_H_USING_LIST)
#define __SETJMP_H_USING_LIST
using __STD_NS_QUALIFIER __jmp_buf;
using __STD_NS_QUALIFIER jmp_buf;
using __STD_NS_QUALIFIER longjmp ;
using __STD_NS_QUALIFIER setjmp ;
#endif
