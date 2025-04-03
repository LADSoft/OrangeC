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

#pragma GCC system_header

#ifndef __STDARG_H
#define __STDARG_H

#    include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef RC_INVOKED
#ifdef __MSIL__
    typedef void* __va_list__ va_list;
#else
typedef void* va_list;
#endif
#endif
#ifdef __cplusplus
};
#endif


#ifdef __MSIL__
#    define va_start(ap, parmN) ap = __va_start__()
#    define va_arg(ap, type) (*((type*)__va_arg__(ap, __va_typeof__(type))))
#else
#    define __sizeof__(x) ((sizeof(x) + sizeof(int) - 1) & ~(sizeof(int) - 1))
#if __STDC_VERSION__ < 202311L
#    define va_start(ap, parmN) ap = (va_list)((char*)(&parmN) + sizeof(parmN))
#    define va_arg(ap, type) (*(type*)(((*(char**)&(ap)) += __sizeof__(type)) - (__sizeof__(type))))
#else
#    define va_start(ap, ...) __va_start__(&ap)
#    define va_arg(ap, type) (*((type*)__va_arg__(&ap, __sizeof__(type))))
#endif
#endif
#    define va_end(ap)

#if __STDC_VERSION__ >= 199901L || __cplusplus >= 201103L
#    define va_copy(ap_d, ap_s) (void)(ap_d = ap_s)
#endif

#define _va_ptr (...)

#endif /* __STDARG_H */
