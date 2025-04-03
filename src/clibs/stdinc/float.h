/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

/*  float.h

    Defines implementation specific macros for dealing with
    floating point.  We don't currently support long doubles.

*/

#pragma GCC system_header

#ifndef __FLOAT_H
#define __FLOAT_H

#pragma pack(1)

#    include <stddef.h>

#define FLT_RADIX 2
#define __FLT_RADIX__ FLT_RADIX
#define FLT_ROUNDS 1

#define FLT_GUARD 1
#define FLT_NORMALIZE 1

#define FLT_EVAL_METHOD -1

#define DBL_MANT_DIG 53
#define FLT_MANT_DIG 24
#define LDBL_MANT_DIG 53
#define __DBL_MANT_DIG__ DBL_MANT_DIG
#define __FLT_MANT_DIG__ FLT_MANT_DIG
#define __LDBL_MANT_DIG__ LDBL_MANT_DIG

#define DBL_DIG 15
#define FLT_DIG 6
#define LDBL_DIG 15
#define __DBL_DIG__ DBL_DIG
#define __FLT_DIG__ FLT_DIG
#define __LDBL_DIG__ LDBL_DIG

#define DBL_DECIMAL_DIG DBL_DIG
#define FLT_DECIMAL_DIG FLT_DIG
#define LDBL_DECIMAL_DIG LDBL_DIG

#define DECIMAL_DIG 21

#define DBL_EPSILON 0x1P-52
#define FLT_EPSILON 0x1P-23
#define LDBL_EPSILON 0x1P-52
#define __DBL_EPSILON__ DBL_EPSILON
#define __FLT_EPSILON__ FLT_EPSILON
#define __LDBL_EPSILON__ LDBL_EPSILON

/* smallest positive IEEE normal numbers */
#define DBL_MIN 0x1P-1022
#define FLT_MIN 0x1P-126F
#define LDBL_MIN 0x1P-1022L
#define __DBL_MIN__ DBL_MIN
#define __FLT_MIN__ FLT_MIN
#define __LDBL_MIN__ LDBL_MIN

#define DBL_MAX 0x1.FFFFFFFFFFFFEP+1023
#define FLT_MAX 0x1.FFFFFCP+127F
#define LDBL_MAX 0x1.FFFFFFFFFFFFEP+1023L
#define __DBL_MAX__ DBL_MAX
#define __FLT_MAX__ FLT_MAX
#define __LDBL_MAX__ LDBL_MAX

#define DBL_MAX_EXP +1024
#define FLT_MAX_EXP +128
#define LDBL_MAX_EXP +1024
#define __DBL_MAX_EXP__ DBL_MAX_EXP
#define __FLT_MAX_EXP__ FLT_MAX_EXP
#define __LDBL_MAX_EXP__ LDBL_MAX_EXP

#define DBL_MIN_EXP -1021
#define FLT_MIN_EXP -125
#define LDBL_MIN_EXP -1021
#define __DBL_MIN_EXP__ DBL_MIN_EXP
#define __FLT_MIN_EXP__ FLT_MIN_EXP
#define __LDBL_MIN_EXP__ LDBL_MIN_EXP

#define DBL_MAX_10_EXP +308
#define FLT_MAX_10_EXP +38
#define LDBL_MAX_10_EXP +308
#define __DBL_MAX_10_EXP__ DBL_MAX_10_EXP
#define __FLT_MAX_10_EXP__ FLT_MAX_10_EXP
#define __LDBL_MAX_10_EXP__ LDBL_MAX_10_EXP

#define DBL_MIN_10_EXP -307
#define FLT_MIN_10_EXP -37
#define LDBL_MIN_10_EXP -307
#define __DBL_MIN_10_EXP__ DBL_MIN_10_EXP
#define __FLT_MIN_10_EXP__ FLT_MIN_10_EXP
#define __LDBL_MIN_10_EXP__ LDBL_MIN_10_EXP

#define DBL_HAS_SUBNORM 1
#define FLT_HAS_SUBNORM 1
#define LDBL_HAS_SUBNORM 1

#define DBL_TRUE_MIN 0x1P-1075
#define FLT_TRUE_MIN 0x1P-150
#define LDBL_TRUE_MIN 0x1P-1075L

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef RC_INVOKED
    unsigned int _RTL_FUNC _IMPORT _clear87(void);
    unsigned int _RTL_FUNC _IMPORT _control87(unsigned int __newcw, unsigned int __mask);
    void _RTL_FUNC _IMPORT _fpreset(void);
    unsigned int _RTL_FUNC _IMPORT _status87(void);
#endif

#define _controlfp(__a, __b) _control87((__a), ((__b) & (~EM_DENORMAL)))
#define _statusfp _status87
#define _clearfp _clear87

    /* 387 Status Word format   */

#define SW_INVALID 0x0001    /* Invalid operation            */
#define SW_DENORMAL 0x0002   /* Denormalized operand         */
#define SW_ZERODIVIDE 0x0004 /* Zero divide                  */
#define SW_OVERFLOW 0x0008   /* Overflow                     */
#define SW_UNDERFLOW 0x0010  /* Underflow                    */
#define SW_INEXACT 0x0020    /* Precision (Inexact result)   */
#define SW_STACKFAULT 0x0040 /* Stack fault                  */

    /* 387 Control Word format */

#define MCW_EM 0x003f        /* interrupt Exception Masks*/
#define EM_INVALID 0x0001    /*   invalid                */
#define EM_DENORMAL 0x0002   /*   denormal               */
#define EM_ZERODIVIDE 0x0004 /*   zero divide            */
#define EM_OVERFLOW 0x0008   /*   overflow               */
#define EM_UNDERFLOW 0x0010  /*   underflow              */
#define EM_INEXACT 0x0020    /*   inexact (precision)    */

#define MCW_IC 0x1000        /* Infinity Control */
#define IC_AFFINE 0x1000     /*   affine         */
#define IC_PROJECTIVE 0x0000 /*   projective     */

#define MCW_RC 0x0c00  /* Rounding Control     */
#define RC_CHOP 0x0c00 /*   chop               */
#define RC_UP 0x0800   /*   up                 */
#define RC_DOWN 0x0400 /*   down               */
#define RC_NEAR 0x0000 /*   near               */

#define MCW_PC 0x0300 /* Precision Control    */
#define PC_24 0x0000  /*    24 bits           */
#define PC_53 0x0200  /*    53 bits           */
#define PC_64 0x0300  /*    64 bits           */

    /* 387 Initial Control Word */
    /* use affine infinity, mask underflow and precision exceptions */

#define CW_DEFAULT _default87

#ifndef RC_INVOKED
    extern unsigned int _RTL_DATA _default87;
#endif
/*
    SIGFPE signal error types (for integer & float exceptions).
*/
#define FPE_INTOVFLOW 126 /* 80x86 Interrupt on overflow  */
#define FPE_INTDIV0 127   /* 80x86 Integer divide by zero */

#define FPE_INVALID 129     /* 80x87 invalid operation      */
#define FPE_ZERODIVIDE 131  /* 80x87 divide by zero         */
#define FPE_OVERFLOW 132    /* 80x87 arithmetic overflow    */
#define FPE_UNDERFLOW 133   /* 80x87 arithmetic underflow   */
#define FPE_INEXACT 134     /* 80x87 precision loss         */
#define FPE_STACKFAULT 135  /* 80x87 stack overflow         */
#define FPE_EXPLICITGEN 140 /* When SIGFPE is raise()'d     */

/*
            SIGSEGV signal error types.
*/
#define SEGV_BOUND 10       /* A BOUND violation (SIGSEGV)  */
#define SEGV_EXPLICITGEN 11 /* When SIGSEGV is raise()'d    */
#define SEGV_ACCESS 12      /* Access violation */
#define SEGV_STACK 13       /* Unable to grow stack */

/*
            SIGILL signal error types.
*/
#define ILL_EXECUTION 20   /* Illegal operation exception  */
#define ILL_EXPLICITGEN 21 /* When SIGILL is raise()'d     */
#define ILL_PRIVILEGED 22  /* Privileged instruction */
#define ILL_BREAKPOINT 23  /* Breakpoint exception */
#define ILL_SINGLE_STEP 24 /* Singlestep exception */

#ifdef __cplusplus
};
#endif

#pragma pack()

#endif /* __FLOAT_H */