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
/*  float.h

    Defines implementation specific macros for dealing with
    floating point.  We don't currently support long doubles.

*/

#ifndef __FLOAT_H
#define __FLOAT_H

#pragma pack(1)

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#define FLT_RADIX           2
#define __FLT_RADIX__       FLT_RADIX
#define FLT_ROUNDS          1

#define FLT_GUARD           1
#define FLT_NORMALIZE       1

#define FLT_EVAL_METHOD     -1

#define DBL_MANT_DIG        53
#define FLT_MANT_DIG        24
#define LDBL_MANT_DIG       64
#define __DBL_MANT_DIG__    DBL_MANT_DIG
#define __FLT_MANT_DIG__    FLT_MANT_DIG
#define __LDBL_MANT_DIG__   LDBL_MANT_DIG

#define DBL_DIG             15
#define FLT_DIG             6
#define LDBL_DIG            18
#define __DBL_DIG__         DBL_DIG
#define __FLT_DIG__         FLT_DIG
#define __LDBL_DIG__        LDBL_DIG

#define DBL_DECIMAL_DIG     DBL_DIG
#define FLT_DECIMAL_DIG     FLT_DIG
#define LDBL_DECIMAL_DIG    LDBL_DIG

#define DECIMAL_DIG         21

#define DBL_EPSILON         0x1P-52
#define FLT_EPSILON         0x1P-23
#define LDBL_EPSILON        0x1P-63

/* smallest positive IEEE normal numbers */
#define DBL_MIN             0x1P-1022
#define FLT_MIN             0x1P-126F
#define LDBL_MIN            0x1P-16382L

#define DBL_MAX             0x1.FFFFFFFFFFFFFP+1023
#define FLT_MAX             0x1.FFFFFEP+127F
#define LDBL_MAX            0x1.FFFFFFFFFFFFFFFEP+16383L

#define DBL_MAX_EXP         +1024
#define FLT_MAX_EXP         +128
#define LDBL_MAX_EXP        +16384
#define __DBL_MAX_EXP__     DBL_MAX_EXP
#define __FLT_MAX_EXP__     FLT_MAX_EXP
#define __LDBL_MAX_EXP__    LDBL_MAX_EXP

#define DBL_MIN_EXP         -1021
#define FLT_MIN_EXP         -125
#define LDBL_MIN_EXP        -16381
#define __DBL_MIN_EXP__     DBL_MIN_EXP
#define __FLT_MIN_EXP__     FLT_MIN_EXP
#define __LDBL_MIN_EXP__    LDBL_MIN_EXP

#define DBL_MAX_10_EXP      +308
#define FLT_MAX_10_EXP      +38
#define LDBL_MAX_10_EXP     +4932
#define __DBL_MAX_10_EXP__   DBL_MAX_10_EXP
#define __FLT_MAX_10_EXP__   FLT_MAX_10_EXP
#define __LDBL_MAX_10_EXP__  LDBL_MAX_10_EXP

#define DBL_MIN_10_EXP      -307
#define FLT_MIN_10_EXP      -37
#define LDBL_MIN_10_EXP     -4931
#define __DBL_MIN_10_EXP__   DBL_MIN_10_EXP
#define __FLT_MIN_10_EXP__   FLT_MIN_10_EXP
#define __LDBL_MIN_10_EXP__  LDBL_MIN_10_EXP

#define DBL_HAS_SUBNORM     1
#define FLT_HAS_SUBNORM     1
#define LDBL_HAS_SUBNORM    1

#ifdef __cplusplus
namespace __STD_NS__ {
extern "C" {
#endif

unsigned int _RTL_FUNC _IMPORT            _clear87(void);
unsigned int _RTL_FUNC _IMPORT            _control87(unsigned int __newcw, unsigned int __mask);
void         _RTL_FUNC _IMPORT            _fpreset(void);
unsigned int _RTL_FUNC _IMPORT            _status87(void);

#define      _controlfp(__a, __b) _control87((__a), ((__b)& (~EM_DENORMAL)))
#define      _statusfp            _status87
#define      _clearfp             _clear87

/* 387 Status Word format   */

#define SW_INVALID      0x0001  /* Invalid operation            */
#define SW_DENORMAL     0x0002  /* Denormalized operand         */
#define SW_ZERODIVIDE   0x0004  /* Zero divide                  */
#define SW_OVERFLOW     0x0008  /* Overflow                     */
#define SW_UNDERFLOW    0x0010  /* Underflow                    */
#define SW_INEXACT      0x0020  /* Precision (Inexact result)   */
#define SW_STACKFAULT   0x0040  /* Stack fault                  */

/* 387 Control Word format */

#define MCW_EM              0x003f  /* interrupt Exception Masks*/
#define     EM_INVALID      0x0001  /*   invalid                */
#define     EM_DENORMAL     0x0002  /*   denormal               */
#define     EM_ZERODIVIDE   0x0004  /*   zero divide            */
#define     EM_OVERFLOW     0x0008  /*   overflow               */
#define     EM_UNDERFLOW    0x0010  /*   underflow              */
#define     EM_INEXACT      0x0020  /*   inexact (precision)    */

#define MCW_IC              0x1000  /* Infinity Control */
#define     IC_AFFINE       0x1000  /*   affine         */
#define     IC_PROJECTIVE   0x0000  /*   projective     */

#define MCW_RC          0x0c00  /* Rounding Control     */
#define     RC_CHOP     0x0c00  /*   chop               */
#define     RC_UP       0x0800  /*   up                 */
#define     RC_DOWN     0x0400  /*   down               */
#define     RC_NEAR     0x0000  /*   near               */

#define MCW_PC          0x0300  /* Precision Control    */
#define     PC_24       0x0000  /*    24 bits           */
#define     PC_53       0x0200  /*    53 bits           */
#define     PC_64       0x0300  /*    64 bits           */


/* 387 Initial Control Word */
/* use affine infinity, mask underflow and precision exceptions */

#define CW_DEFAULT  _default87
extern unsigned int _RTL_DATA _default87;


/*
    SIGFPE signal error types (for integer & float exceptions).
*/
#define FPE_INTOVFLOW       126 /* 80x86 Interrupt on overflow  */
#define FPE_INTDIV0         127 /* 80x86 Integer divide by zero */

#define FPE_INVALID         129 /* 80x87 invalid operation      */
#define FPE_ZERODIVIDE      131 /* 80x87 divide by zero         */
#define FPE_OVERFLOW        132 /* 80x87 arithmetic overflow    */
#define FPE_UNDERFLOW       133 /* 80x87 arithmetic underflow   */
#define FPE_INEXACT         134 /* 80x87 precision loss         */
#define FPE_STACKFAULT      135 /* 80x87 stack overflow         */
#define FPE_EXPLICITGEN     140 /* When SIGFPE is raise()'d     */

/*
            SIGSEGV signal error types.
*/
#define SEGV_BOUND          10  /* A BOUND violation (SIGSEGV)  */
#define SEGV_EXPLICITGEN    11  /* When SIGSEGV is raise()'d    */
#define SEGV_ACCESS         12  /* Access violation */
#define SEGV_STACK          13  /* Unable to grow stack */

/*
            SIGILL signal error types.
*/
#define ILL_EXECUTION       20  /* Illegal operation exception  */
#define ILL_EXPLICITGEN     21  /* When SIGILL is raise()'d     */
#define ILL_PRIVILEGED      22  /* Privileged instruction */
#define ILL_BREAKPOINT      23  /* Breakpoint exception */
#define ILL_SINGLE_STEP     24  /* Singlestep exception */

#ifdef __cplusplus
} ;
} ;
#endif

#pragma pack()

#endif /* __FLOAT_H */

#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__FLOAT_H_USING_LIST)
#define __FLOAT_H_USING_LIST
using __STD_NS_QUALIFIER _clear87 ;
using __STD_NS_QUALIFIER _control87;
using __STD_NS_QUALIFIER _fpreset;
using __STD_NS_QUALIFIER _status87 ;
using __STD_NS_QUALIFIER _default87;
#endif
