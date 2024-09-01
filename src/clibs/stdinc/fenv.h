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

#pragma GCC system_header

#ifndef __FENV_H
#define __FENV_H

#pragma pack(1)

#    include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif
#ifndef RC_INVOKED
#ifdef __cplusplus
    struct fexcept_t
#else
    typedef struct
#endif
    {
#ifdef __cplusplus
        fexcept_t() : mask(0) { }
        fexcept_t(unsigned short msk) : mask(msk) { }
#endif
        union
        {
            unsigned short mask;
            struct {
                unsigned short excInvalid : 1;
                unsigned short excDenormal : 1;
                unsigned short excZero : 1;
                unsigned short excOverflow : 1;
                unsigned short excUnderflow : 1;
                unsigned short excPrecision : 1;
                unsigned short excStack : 1;
                unsigned short errorSummary : 1;
                unsigned short filler : 8;
             };
         };
    } 
#ifndef __cplusplus
       fexcept_t
#endif
    ;

    typedef struct
    {
        unsigned short controlWord;
        unsigned short __null1;
        unsigned short statusWord;
        unsigned short __null2;
        unsigned short tagWord;
        unsigned short __null3;
        unsigned int fIP;
        unsigned short fCS1;
        unsigned short __null4;
        unsigned int fOp;
        unsigned short fCS2;
        unsigned short __null5;
    } fenv_t;
#endif

#define FE_DIVBYZERO 0x0004
#define FE_INEXACT 0x0020
#define FE_INVALID 0x0001
#define FE_OVERFLOW 0x0008
#define FE_UNDERFLOW 0x0010
#define FE_DENORMAL 0x0002
#define FE_STACKFAULT 0x0040
#define FE_ERRORSUMMARY 0x0080

#define FE_ALL_EXCEPT (FE_DIVBYZERO | FE_INEXACT | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW | FE_DENORMAL | FE_STACKFAULT)

#define FE_DOWNWARD 1
#define FE_TONEAREST 0
#define FE_TOWARDZERO 3
#define FE_UPWARD 2

#ifndef RC_INVOKED
    extern fenv_t* const __fe_default_env;
#endif

#define FE_DFL_ENV (__fe_default_env)

#ifndef RC_INVOKED
    int feclearexcept(int __excepts);
    int fegetexceptflag(fexcept_t* __flagp, int __excepts);
    int feraiseexcept(int __excepts);
    int fesetexceptflag(fexcept_t* __flagp, int __excepts);
    int fetestexcept(int __excepts);
    int fegetround(void);
    int fesetround(int __round);
    int fegetenv(fenv_t* __envp);
    int feholdexcept(fenv_t* __envp);
    int fesetenv(const fenv_t* __envp);
    int feupdateenv(const fenv_t* __envp);
#endif

#ifdef __cplusplus
}
#endif

#pragma pack()

#endif /* __FENV_H */
