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
#ifndef __COMPLEX_H
#define __COMPLEX_H

#ifndef __STDDEF_H
#include <stddef.h>
#endif

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

#ifdef __cplusplus
namespace __STD_NS__ {
extern "C" {
#endif

float complex _RTL_FUNC _IMPORT cacosf(float complex) ;
double complex _RTL_FUNC _IMPORT cacos(double complex) ;
long double complex _RTL_FUNC _IMPORT cacosl(long double complex) ;

float complex _RTL_FUNC _IMPORT casinf(float complex) ;
double complex _RTL_FUNC _IMPORT casin(double complex) ;
long double complex _RTL_FUNC _IMPORT casinl(long double complex) ;

float complex _RTL_FUNC _IMPORT catanf(float complex) ;
double complex _RTL_FUNC _IMPORT catan(double complex) ;
long double complex _RTL_FUNC _IMPORT catanl(long double complex) ;

float complex _RTL_FUNC _IMPORT ccosf(float complex) ;
double complex _RTL_FUNC _IMPORT ccos(double complex) ;
long double complex _RTL_FUNC _IMPORT ccosl(long double complex) ;

float complex _RTL_FUNC _IMPORT csinf(float complex) ;
double complex _RTL_FUNC _IMPORT csin(double complex) ;
long double complex _RTL_FUNC _IMPORT csinl(long double complex) ;

float complex _RTL_FUNC _IMPORT ctanf(float complex) ;
double complex _RTL_FUNC _IMPORT ctan(double complex) ;
long double complex _RTL_FUNC _IMPORT ctanl(long double complex) ;

float complex _RTL_FUNC _IMPORT cacoshf(float complex) ;
double complex _RTL_FUNC _IMPORT cacosh(double complex) ;
long double complex _RTL_FUNC _IMPORT cacoshl(long double complex) ;

float complex _RTL_FUNC _IMPORT casinhf(float complex) ;
double complex _RTL_FUNC _IMPORT casinh(double complex) ;
long double complex _RTL_FUNC _IMPORT casinhl(long double complex) ;

float complex _RTL_FUNC _IMPORT catanhf(float complex) ;
double complex _RTL_FUNC _IMPORT catanh(double complex) ;
long double complex _RTL_FUNC _IMPORT catanhl(long double complex) ;

float complex _RTL_FUNC _IMPORT ccoshf(float complex) ;
double complex _RTL_FUNC _IMPORT ccosh(double complex) ;
long double complex _RTL_FUNC _IMPORT ccoshl(long double complex) ;

float complex _RTL_FUNC _IMPORT csinhf(float complex) ;
double complex _RTL_FUNC _IMPORT csinh(double complex) ;
long double complex _RTL_FUNC _IMPORT csinhl(long double complex) ;

float complex _RTL_FUNC _IMPORT ctanhf(float complex) ;
double complex _RTL_FUNC _IMPORT ctanh(double complex) ;
long double complex _RTL_FUNC _IMPORT ctanhl(long double complex) ;

float complex _RTL_FUNC _IMPORT cexpf(float complex) ;
double complex _RTL_FUNC _IMPORT cexp(double complex) ;
long double complex _RTL_FUNC _IMPORT cexpl(long double complex) ;

float complex _RTL_FUNC _IMPORT clogf(float complex) ;
double complex _RTL_FUNC _IMPORT clog(double complex) ;
long double complex _RTL_FUNC _IMPORT clogl(long double complex) ;

float _RTL_FUNC _IMPORT cabsf(float complex) ;
double _RTL_FUNC _IMPORT cabs(double complex) ;
long double _RTL_FUNC _IMPORT cabsl(long double complex) ;

float complex _RTL_FUNC _IMPORT cpowf(float complex, float complex) ;
double complex _RTL_FUNC _IMPORT cpow(double complex, double complex) ;
long double complex _RTL_FUNC _IMPORT cpowl(long double complex, long double complex) ;

float complex _RTL_FUNC _IMPORT csqrtf(float complex) ;
double complex _RTL_FUNC _IMPORT csqrt(double complex) ;
long double complex _RTL_FUNC _IMPORT csqrtl(long double complex) ;

float _RTL_FUNC _IMPORT cargf(float complex) ;
double _RTL_FUNC _IMPORT carg(double complex) ;
long double _RTL_FUNC _IMPORT cargl(long double complex) ;

float _RTL_FUNC _IMPORT cimagf(float complex) ;
double _RTL_FUNC _IMPORT cimag(double complex) ;
long double _RTL_FUNC _IMPORT cimagl(long double complex) ;

float complex _RTL_FUNC _IMPORT conjf(float complex) ;
double complex _RTL_FUNC _IMPORT conj(double complex) ;
long double complex _RTL_FUNC _IMPORT conjl(long double complex) ;

float complex _RTL_FUNC _IMPORT cprojf(float complex) ;
double complex _RTL_FUNC _IMPORT cproj(double complex) ;
long double complex _RTL_FUNC _IMPORT cprojl(long double complex) ;

float _RTL_FUNC _IMPORT crealf(float complex) ;
double _RTL_FUNC _IMPORT creal(double complex) ;
long double _RTL_FUNC _IMPORT creall(long double complex) ;

#ifdef __cplusplus
} ;
} ;
#endif
#endif /* __COMPLEX_H */

#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__COMPLEX_H_USING_LIST)
#define __COMPLEX_H_USING_LIST

using __STD_NS_QUALIFIER cacosf ;
using __STD_NS_QUALIFIER cacos ;
using __STD_NS_QUALIFIER cacosl ;
using __STD_NS_QUALIFIER casinf ;
using __STD_NS_QUALIFIER casin ;
using __STD_NS_QUALIFIER casinl ;
using __STD_NS_QUALIFIER catanf ;
using __STD_NS_QUALIFIER catan ;
using __STD_NS_QUALIFIER catanl ;
using __STD_NS_QUALIFIER ccosf ;
using __STD_NS_QUALIFIER ccos ;
using __STD_NS_QUALIFIER ccosl ;
using __STD_NS_QUALIFIER csinf ;
using __STD_NS_QUALIFIER csin ;
using __STD_NS_QUALIFIER csinl ;
using __STD_NS_QUALIFIER ctanf ;
using __STD_NS_QUALIFIER ctan ;
using __STD_NS_QUALIFIER ctanl ;
using __STD_NS_QUALIFIER cacoshf ;
using __STD_NS_QUALIFIER cacosh ;
using __STD_NS_QUALIFIER cacoshl ;
using __STD_NS_QUALIFIER casinhf ;
using __STD_NS_QUALIFIER casinh ;
using __STD_NS_QUALIFIER casinhl ;
using __STD_NS_QUALIFIER catanhf ;
using __STD_NS_QUALIFIER catanh ;
using __STD_NS_QUALIFIER catanhl ;
using __STD_NS_QUALIFIER ccoshf ;
using __STD_NS_QUALIFIER ccosh ;
using __STD_NS_QUALIFIER ccoshl ;
using __STD_NS_QUALIFIER csinhf ;
using __STD_NS_QUALIFIER csinh ;
using __STD_NS_QUALIFIER csinhl ;
using __STD_NS_QUALIFIER ctanhf ;
using __STD_NS_QUALIFIER ctanh ;
using __STD_NS_QUALIFIER ctanhl ;
using __STD_NS_QUALIFIER cexpf ;
using __STD_NS_QUALIFIER cexp ;
using __STD_NS_QUALIFIER cexpl ;
using __STD_NS_QUALIFIER clogf ;
using __STD_NS_QUALIFIER clog ;
using __STD_NS_QUALIFIER clogl ;
using __STD_NS_QUALIFIER cabsf ;
using __STD_NS_QUALIFIER cabs ;
using __STD_NS_QUALIFIER cabsl ;
using __STD_NS_QUALIFIER csqrtf ;
using __STD_NS_QUALIFIER csqrt ;
using __STD_NS_QUALIFIER csqrtl ;
using __STD_NS_QUALIFIER cargf ;
using __STD_NS_QUALIFIER carg ;
using __STD_NS_QUALIFIER cargl ;
using __STD_NS_QUALIFIER cimagf ;
using __STD_NS_QUALIFIER cimag ;
using __STD_NS_QUALIFIER cimagl ;
using __STD_NS_QUALIFIER conjf ;
using __STD_NS_QUALIFIER conj ;
using __STD_NS_QUALIFIER conjl ;
using __STD_NS_QUALIFIER cprojf ;
using __STD_NS_QUALIFIER cproj ;
using __STD_NS_QUALIFIER cprojl ;
using __STD_NS_QUALIFIER crealf ;
using __STD_NS_QUALIFIER creal ;
using __STD_NS_QUALIFIER creall ;
#endif
