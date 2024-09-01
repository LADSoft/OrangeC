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

#ifndef __COMPLEX_H
#define __COMPLEX_H

#    include <stddef.h>

/* this is the default even when this file isn't included */
#pragma STDC CX_LIMITED_RANGE OFF

#define complex _Complex
#define _Complex_I (0.0F + __I)

#define imaginary _Imaginary
#define _Imaginary_I (__I)

#define I _Imaginary_I

#define CMPLX(x, y) ((double complex)((double)(x) + _Imaginary_I * (double)(y)))
#define CMPLXF(x, y) ((float complex)((float)(x) + _Imaginary_I * (float)(y)))
#define CMPLXL(x, y) ((long double complex)((long double)(x) + _Imaginary_I * (long double)(y)))

#ifndef RC_INVOKED
#ifdef __cplusplus
extern "C"
{
#endif

    float complex _RTL_FUNC _IMPORT cacosf(float complex);
    double complex _RTL_FUNC _IMPORT cacos(double complex);
    long double complex _RTL_FUNC _IMPORT cacosl(long double complex);

    float complex _RTL_FUNC _IMPORT casinf(float complex);
    double complex _RTL_FUNC _IMPORT casin(double complex);
    long double complex _RTL_FUNC _IMPORT casinl(long double complex);

    float complex _RTL_FUNC _IMPORT catanf(float complex);
    double complex _RTL_FUNC _IMPORT catan(double complex);
    long double complex _RTL_FUNC _IMPORT catanl(long double complex);

    float complex _RTL_FUNC _IMPORT ccosf(float complex);
    double complex _RTL_FUNC _IMPORT ccos(double complex);
    long double complex _RTL_FUNC _IMPORT ccosl(long double complex);

    float complex _RTL_FUNC _IMPORT csinf(float complex);
    double complex _RTL_FUNC _IMPORT csin(double complex);
    long double complex _RTL_FUNC _IMPORT csinl(long double complex);

    float complex _RTL_FUNC _IMPORT ctanf(float complex);
    double complex _RTL_FUNC _IMPORT ctan(double complex);
    long double complex _RTL_FUNC _IMPORT ctanl(long double complex);

    float complex _RTL_FUNC _IMPORT cacoshf(float complex);
    double complex _RTL_FUNC _IMPORT cacosh(double complex);
    long double complex _RTL_FUNC _IMPORT cacoshl(long double complex);

    float complex _RTL_FUNC _IMPORT casinhf(float complex);
    double complex _RTL_FUNC _IMPORT casinh(double complex);
    long double complex _RTL_FUNC _IMPORT casinhl(long double complex);

    float complex _RTL_FUNC _IMPORT catanhf(float complex);
    double complex _RTL_FUNC _IMPORT catanh(double complex);
    long double complex _RTL_FUNC _IMPORT catanhl(long double complex);

    float complex _RTL_FUNC _IMPORT ccoshf(float complex);
    double complex _RTL_FUNC _IMPORT ccosh(double complex);
    long double complex _RTL_FUNC _IMPORT ccoshl(long double complex);

    float complex _RTL_FUNC _IMPORT csinhf(float complex);
    double complex _RTL_FUNC _IMPORT csinh(double complex);
    long double complex _RTL_FUNC _IMPORT csinhl(long double complex);

    float complex _RTL_FUNC _IMPORT ctanhf(float complex);
    double complex _RTL_FUNC _IMPORT ctanh(double complex);
    long double complex _RTL_FUNC _IMPORT ctanhl(long double complex);

    float complex _RTL_FUNC _IMPORT cexpf(float complex);
    double complex _RTL_FUNC _IMPORT cexp(double complex);
    long double complex _RTL_FUNC _IMPORT cexpl(long double complex);

    float complex _RTL_FUNC _IMPORT clogf(float complex);
    double complex _RTL_FUNC _IMPORT clog(double complex);
    long double complex _RTL_FUNC _IMPORT clogl(long double complex);

    float _RTL_FUNC _IMPORT cabsf(float complex);
    double _RTL_FUNC _IMPORT cabs(double complex);
    long double _RTL_FUNC _IMPORT cabsl(long double complex);

    float complex _RTL_FUNC _IMPORT cpowf(float complex, float complex);
    double complex _RTL_FUNC _IMPORT cpow(double complex, double complex);
    long double complex _RTL_FUNC _IMPORT cpowl(long double complex, long double complex);

    float complex _RTL_FUNC _IMPORT csqrtf(float complex);
    double complex _RTL_FUNC _IMPORT csqrt(double complex);
    long double complex _RTL_FUNC _IMPORT csqrtl(long double complex);

    float _RTL_FUNC _IMPORT cargf(float complex);
    double _RTL_FUNC _IMPORT carg(double complex);
    long double _RTL_FUNC _IMPORT cargl(long double complex);

    float _RTL_FUNC _IMPORT cimagf(float complex);
    double _RTL_FUNC _IMPORT cimag(double complex);
    long double _RTL_FUNC _IMPORT cimagl(long double complex);

    float complex _RTL_FUNC _IMPORT conjf(float complex);
    double complex _RTL_FUNC _IMPORT conj(double complex);
    long double complex _RTL_FUNC _IMPORT conjl(long double complex);

    float complex _RTL_FUNC _IMPORT cprojf(float complex);
    double complex _RTL_FUNC _IMPORT cproj(double complex);
    long double complex _RTL_FUNC _IMPORT cprojl(long double complex);

    float _RTL_FUNC _IMPORT crealf(float complex);
    double _RTL_FUNC _IMPORT creal(double complex);
    long double _RTL_FUNC _IMPORT creall(long double complex);

#ifdef __cplusplus
};
#endif
#endif
#endif /* __COMPLEX_H */
