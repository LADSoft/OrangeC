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
/*  math.h

    Definitions for the math floating point package.

*/

#ifndef  __MATH_H
#define  __MATH_H

#pragma pack(1)

/* these are the defaults even when this file isn't included */
#pragma STDC FENV_ACCESS OFF
#pragma STDC FP_CONTRACT ON

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#ifdef __cplusplus
namespace __STD_NS__ {
extern "C" {
#endif

#if  __STDC_VERSION__ >= 199901L
int *_RTL_FUNC _IMPORT __GetSignGam(void);

#define signgam (*__GetSignGam())

#endif

#ifndef __cplusplus
#if  __STDC_VERSION__ != 0 && __STDC_VERSION__ < 199901L
struct complex      /* as used by "_cabs" function */
{
    double  x, y;
};

struct _complexl    /* as used by "_cabsl" function */
{
    long double  x, y;
};

#define cabs(z)     (hypot  ((z).x, (z).y))
#define cabsl(z)    (hypotl ((z).x, (z).y))
#define _cabsl cabsl
#define _hypotl(z)  (hypotl ((z).x, (z).y))

#endif
#endif
typedef enum
{
    DOMAIN = 1,    /* argument domain error -- log (-1)        */
    SING,          /* argument singularity  -- pow (0,-2))     */
    OVERFLOW,      /* overflow range error  -- exp (1000)      */
    UNDERFLOW,     /* underflow range error -- exp (-1000)     */
    TLOSS,         /* total loss of significance -- sin(10e70) */
    PLOSS,         /* partial loss of signif. -- not used      */
    STACKFAULT     /* floating point unit stack overflow       */
}   _mexcep;

/* Constants rounded for 21 decimals. */
#define M_E         2.71828182845904523536
#define M_LOG2E     1.44269504088896340736
#define M_LOG10E    0.434294481903251827651
#define M_LN2       0.693147180559945309417
#define M_LN10      2.30258509299404568402
#define M_PI        3.14159265358979323846
#define M_PI_2      1.57079632679489661923
#define M_PI_4      0.785398163397448309616
#define M_1_PI      0.318309886183790671538
#define M_2_PI      0.636619772367581343076
#define M_1_SQRTPI  0.564189583547756286948
#define M_2_SQRTPI  1.12837916709551257390
#define M_SQRT2     1.41421356237309504880
#define M_SQRT_2    0.707106781186547524401

#define INFINITY             (0x1.0p128F)
#define NAN                  (_NAN) /* QUIET NAN */

#define FP_ILOGB0      (-2147483647L-1)  /* INT_MIN*/
#define FP_ILOGBNAN        2147483647L     /* INT_MAX */

/* Classification */
#define FP_INFINITE 0
#define FP_NAN 1
#define FP_NORMAL 2
#define FP_SUBNORMAL 3
#define FP_ZERO 4

#define EDOM    33      /* Math argument */
#define ERANGE  34      /* Result too large */

typedef float float_t ;
typedef double double_t ;

struct  _exception
{
    int type;
    char *name;
    double  arg1, arg2, retval;
};

struct  _exceptionl
{
    int type;
    char *name;
    long double  arg1, arg2, retval;
};

/* positive infinities (x86) */
#define HUGE_VAL             0x1P+1024
#define HUGE_VALF             0x1P+128F
#define HUGE_VALL            0x1P+16384L
#define _LHUGE_VAL  HUGE_VALL

int _RTL_FUNC _IMPORTMM __fpclassifyf(float __x);
int _RTL_FUNC _IMPORTMM __fpclassify(double __x);
int _RTL_FUNC _IMPORTMM __fpclassifyl(long double __x);

#if  __STDC_VERSION__ >= 199901L || defined(__cplusplus)
float _RTL_FUNC _IMPORTMM nanf(const char *tagp);
double _RTL_FUNC _IMPORTMM nan(const char *tagp);
long double _RTL_FUNC _IMPORTMM nanl(const char *tagp);

int _RTL_FUNC _IMPORTMM __signbitf(float __x);
int _RTL_FUNC _IMPORTMM __signbit(double __x);
int _RTL_FUNC _IMPORTMM __signbitl(long double __x);
int _RTL_FUNC _IMPORTMM __nancompare(long double __x, long double __y, int type);


#define fpclassify(x) \
    ((sizeof(x) == sizeof(float)) ? __fpclassifyf(x) : \
    (sizeof(x) == sizeof(double)) ? __fpclassify(x) : \
    __fpclassifyl(x))

#define signbit(x) \
    ((sizeof(x) == sizeof(float)) ? __signbitf(x) : \
    (sizeof(x) == sizeof(double)) ? __signbit(x) : \
    __signbitl(x))

#define isfinite(x) (fpclassify(x) >= FP_NORMAL)
#define isinf(x) (fpclassify(x) == FP_INFINITE)
#define isnan(x) (fpclassify(x) == FP_NAN)
#define isnormal(x) (fpclassify(x) == FP_NORMAL)

#define isgreater(x,y) __nancompare((x),(y),1)
#define isgreaterequal(x,y) __nancompare((x),(y),2)
#define isless(x,y) __nancompare((x),(y),-1)
#define islessequal(x,y) __nancompare((x),(y),-2)
#define islessgreater(x,y) __nancompare((x),(y),-3)
#define isunordered(x,y) __nancompare((x),(y),0)

#endif

float _RTL_FUNC _IMPORTMM acosf  (float __x);
float _RTL_FUNC _IMPORTMM asinf  (float __x);
float _RTL_FUNC _IMPORTMM atan2f (float __y, float __x);
float _RTL_FUNC _IMPORTMM atanf  (float __x);
float _RTL_FUNC _IMPORTMM ceilf  (float __x);
float _RTL_FUNC _IMPORTMM cosf   (float __x);
float _RTL_FUNC _IMPORTMM coshf  (float __x);
float _RTL_FUNC _IMPORTMM expf   (float __x);
float _RTL_FUNC _IMPORTMM fabsf  (float __x);
float _RTL_FUNC _IMPORTMM floorf (float __x);
float _RTL_FUNC _IMPORTMM fmodf  (float __x, float __y);
float _RTL_FUNC _IMPORTMM frexpf (float __x, int *__exponent);
float _RTL_FUNC _IMPORTMM ldexpf (float __x, int __exponent);
float _RTL_FUNC _IMPORTMM log10f (float __x);
float _RTL_FUNC _IMPORTMM logf   (float __x);
float _RTL_FUNC _IMPORTMM modff  (float __x, float *__y);
float _RTL_FUNC _IMPORTMM powf   (float __x, float __y);
float _RTL_FUNC _IMPORTMM sinhf  (float __x);
float _RTL_FUNC _IMPORTMM sinf   (float __x);
float _RTL_FUNC _IMPORTMM sqrtf  (float __x);
float _RTL_FUNC _IMPORTMM tanhf  (float __x);
float _RTL_FUNC _IMPORTMM tanf   (float __x);



double      _RTL_FUNC _IMPORTMM acos  (double __x);
double      _RTL_FUNC _IMPORTMM asin  (double __x);
double      _RTL_FUNC _IMPORTMM atan2 (double __y, double __x);
double      _RTL_FUNC _IMPORTMM atan  (double __x);
double      _RTL_FUNC _IMPORTMM ceil    (double __x);
double      _RTL_FUNC _IMPORTMM cos     (double __x);
double      _RTL_FUNC _IMPORTMM cosh    (double __x);
double      _RTL_FUNC _IMPORTMM exp     (double __x);
double      _RTL_FUNC _IMPORTMM fabs    (double __x);
double      _RTL_FUNC _IMPORTMM floor   (double __x);
double      _RTL_FUNC _IMPORTMM fmod    (double __x, double __y);
double      _RTL_FUNC _IMPORTMM frexp   (double __x, int *__exponent);
double      _RTL_FUNC _IMPORTMM ldexp   (double __x, int __exponent);
double      _RTL_FUNC _IMPORTMM log     (double __x);
double      _RTL_FUNC _IMPORTMM log10   (double __x);
double      _RTL_FUNC _IMPORTMM modf    (double __x, double *__ipart);
double      _RTL_FUNC _IMPORTMM pow     (double __x, double __y);
double      _RTL_FUNC _IMPORTMM sin     (double __x);
double      _RTL_FUNC _IMPORTMM sinh    (double __x);
double      _RTL_FUNC _IMPORTMM sqrt    (double __x);
double      _RTL_FUNC _IMPORTMM tan     (double __x);
double      _RTL_FUNC _IMPORTMM tanh    (double __x);

long double _RTL_FUNC _IMPORTMM acosl  (long double __x);
long double _RTL_FUNC _IMPORTMM asinl  (long double __x);
long double _RTL_FUNC _IMPORTMM atan2l (long double __y, long double __x);
long double _RTL_FUNC _IMPORTMM atanl  (long double __x);
long double _RTL_FUNC _IMPORTMM ceill  (long double __x);
long double _RTL_FUNC _IMPORTMM coshl  (long double __x);
long double _RTL_FUNC _IMPORTMM cosl   (long double __x);
long double _RTL_FUNC _IMPORTMM expl   (long double __x);
long double _RTL_FUNC _IMPORTMM fabsl  (long double __x);
long double _RTL_FUNC _IMPORTMM floorl (long double __x);
long double _RTL_FUNC _IMPORTMM fmodl  (long double __x, long double __y);
long double _RTL_FUNC _IMPORTMM frexpl (long double __x, int *__exponent);
long double _RTL_FUNC _IMPORTMM ldexpl (long double __x, int __exponent);
long double _RTL_FUNC _IMPORTMM log10l (long double __x);
long double _RTL_FUNC _IMPORTMM logl   (long double __x);
long double _RTL_FUNC _IMPORTMM modfl  (long double __x, long double *__ipart);
long double _RTL_FUNC _IMPORTMM powl   (long double __x, long double __y);
long double _RTL_FUNC _IMPORTMM sinhl  (long double __x);
long double _RTL_FUNC _IMPORTMM sinl   (long double __x);
long double _RTL_FUNC _IMPORTMM sqrtl  (long double __x);
long double _RTL_FUNC _IMPORTMM tanhl  (long double __x);
long double _RTL_FUNC _IMPORTMM tanl   (long double __x);

double      _RTL_FUNC _IMPORTMM _copysign(double __x, double __y);

#if  __STDC_VERSION__ >= 199901L || defined(__cplusplus)

float _RTL_FUNC _IMPORTMM acoshf  (float __x);
float _RTL_FUNC _IMPORTMM asinhf  (float __x);
float _RTL_FUNC _IMPORTMM atanhf  (float __x);
float _RTL_FUNC _IMPORTMM cbrtf  (float __x);
float _RTL_FUNC _IMPORTMM copysignf(float __x, float __y);
float _RTL_FUNC _IMPORTMM erff   (float __x);
float _RTL_FUNC _IMPORTMM erfcf  (float __x);
float _RTL_FUNC _IMPORTMM exp2f  (float __x);
float _RTL_FUNC _IMPORTMM expm1f (float __x);
float _RTL_FUNC _IMPORTMM fdimf  (float __x, float __y);
float _RTL_FUNC _IMPORTMM fmaf   (float __x, float __y, float __z);
float _RTL_FUNC _IMPORTMM fmaxf  (float __x, float __y);
float _RTL_FUNC _IMPORTMM fminf  (float __x, float __y);
int _RTL_FUNC _IMPORTMM ilogbf (float __x);
float _RTL_FUNC _IMPORTMM lgammaf(float __x);
long _RTL_FUNC _IMPORTMM lrintf (float __x);
long long _RTL_FUNC _IMPORTMM llrintf(float __x);
float _RTL_FUNC _IMPORTMM log1pf (float __x);
float _RTL_FUNC _IMPORTMM log2f  (float __x);
float _RTL_FUNC _IMPORTMM logbf  (float __x);
long _RTL_FUNC _IMPORTMM lroundf(float __x);
long long _RTL_FUNC _IMPORTMM llroundf(float __x);
float _RTL_FUNC _IMPORTMM nearbyintf(float __x);
float _RTL_FUNC _IMPORTMM nextafterf  (float __x, float __y);
float _RTL_FUNC _IMPORTMM nexttowardf  (float __x, long double __y);
float _RTL_FUNC _IMPORTMM remainderf(float __x, float __y);
float _RTL_FUNC _IMPORTMM remquof(float __x, float __y, int *quo);
float _RTL_FUNC _IMPORTMM rintf  (float __x);
float _RTL_FUNC _IMPORTMM roundf (float __x);
float _RTL_FUNC _IMPORTMM scalbnf (float __x, int __y);
float _RTL_FUNC _IMPORTMM scalblnf (float __x, long int __y);
float _RTL_FUNC _IMPORTMM significandf(float __x);
float _RTL_FUNC _IMPORTMM tgammaf(float __x);
float _RTL_FUNC _IMPORTMM truncf (float __x);

double      _RTL_FUNC _IMPORTMM acosh  (double __x);
double      _RTL_FUNC _IMPORTMM asinh  (double __x);
double      _RTL_FUNC _IMPORTMM atanh  (double __x);
double      _RTL_FUNC _IMPORTMM cbrt    (double __x);
double      _RTL_FUNC _IMPORTMM copysign(double __x, double __y);
double      _RTL_FUNC _IMPORTMM erf     (double __x);
double      _RTL_FUNC _IMPORTMM erfc    (double __x);
double      _RTL_FUNC _IMPORTMM exp2    (double __x);
double      _RTL_FUNC _IMPORTMM expm1   (double __x);
double      _RTL_FUNC _IMPORTMM fdim    (double __x, double __y);
double      _RTL_FUNC _IMPORTMM fma     (double __x, double __y, double __z);
double      _RTL_FUNC _IMPORTMM fmax    (double __x, double __y);
double      _RTL_FUNC _IMPORTMM fmin    (double __x, double __y);
int         _RTL_FUNC _IMPORTMM ilogb   (double __x);
double      _RTL_FUNC _IMPORTMM lgamma  (double __x);
long        _RTL_FUNC _IMPORTMM lrint   (double __x);
long long   _RTL_FUNC _IMPORTMM llrint  (double __x);
double      _RTL_FUNC _IMPORTMM logb    (double __x);
double      _RTL_FUNC _IMPORTMM log1p   (double __x);
double      _RTL_FUNC _IMPORTMM log2    (double __x);
long        _RTL_FUNC _IMPORTMM lround  (double __x);
long long   _RTL_FUNC _IMPORTMM llround (double __x);
double      _RTL_FUNC _IMPORTMM nearbyint(double __x);
double      _RTL_FUNC _IMPORTMM nextafter    (double __x, double __y);
double      _RTL_FUNC _IMPORTMM nexttoward(double __x, long double __y);
double      _RTL_FUNC _IMPORTMM remainder(double __x, double __y);
double      _RTL_FUNC _IMPORTMM remquo  (double __x, double __y, int *quo);
double      _RTL_FUNC _IMPORTMM rint    (double __x);
double      _RTL_FUNC _IMPORTMM round   (double __x);
double      _RTL_FUNC _IMPORTMM scalbn  (double __x, int __y);
double      _RTL_FUNC _IMPORTMM scalbln (double __x, long int __y);
double      _RTL_FUNC _IMPORTMM significand  (double __x);
double      _RTL_FUNC _IMPORTMM tgamma  (double __x);
double      _RTL_FUNC _IMPORTMM trunc   (double __x);

long double _RTL_FUNC _IMPORTMM acoshl  (long double __x);
long double _RTL_FUNC _IMPORTMM asinhl  (long double __x);
long double _RTL_FUNC _IMPORTMM atanhl  (long double __x);
long double _RTL_FUNC _IMPORTMM cbrtl  (long double __x);
long double _RTL_FUNC _IMPORTMM copysignl(long double __x, long double __y);
long double _RTL_FUNC _IMPORTMM erfl   (long double __x);
long double _RTL_FUNC _IMPORTMM erfcl  (long double __x);
long double _RTL_FUNC _IMPORTMM exp2l  (long double __x);
long double _RTL_FUNC _IMPORTMM expm1l (long double __x);
long double _RTL_FUNC _IMPORTMM fdiml  (long double __x, long double __y);
long double _RTL_FUNC _IMPORTMM fmal  (long double __x, long double __y, long double __z);
long double _RTL_FUNC _IMPORTMM fmaxl  (long double __x, long double __y);
long double _RTL_FUNC _IMPORTMM fminl  (long double __x, long double __y);
int         _RTL_FUNC _IMPORTMM ilogbl (long double __x);
long double _RTL_FUNC _IMPORTMM lgammal(long double __x);
long        _RTL_FUNC _IMPORTMM lrintl (long double __x);
long long   _RTL_FUNC _IMPORTMM llrintl(long double __x);
long double _RTL_FUNC _IMPORTMM log1pl (long double __x);
long double _RTL_FUNC _IMPORTMM log2l  (long double __x);
long double _RTL_FUNC _IMPORTMM logbl   (long double __x);
long        _RTL_FUNC _IMPORTMM lroundl(long double __x);
long long   _RTL_FUNC _IMPORTMM llroundl(long double __x);
long double _RTL_FUNC _IMPORTMM nearbyintl(long double __x);
long double _RTL_FUNC _IMPORTMM nextafterl  (long double __x, long double __y);
long double _RTL_FUNC _IMPORTMM nexttowardl  (long double __x, long double __y);
long double _RTL_FUNC _IMPORTMM remainderl(long double __x, long double __y);
long double _RTL_FUNC _IMPORTMM remquol (long double __x, long double __y, int *quo);
long double _RTL_FUNC _IMPORTMM rintl  (long double __x);
long double _RTL_FUNC _IMPORTMM roundl (long double __x);
long double _RTL_FUNC _IMPORTMM scalbnl (long double __x, int __y);
long double _RTL_FUNC _IMPORTMM scalblnl (long double __x, long int __y);
long double _RTL_FUNC _IMPORTMM significandl(long double __x);
long double _RTL_FUNC _IMPORTMM tgammal (long double __x);
long double _RTL_FUNC _IMPORTMM truncl (long double __x);

#endif

int         _RTL_FUNC _matherr (struct _exception *__e);

#if  __STDC_VERSION__ >= 199901L || defined(__cplusplus)
float       _RTL_FUNC _IMPORTMM hypotf (float __x, float __y);
float       _RTL_FUNC _IMPORTMM p1evlf  (float __x, float *__coeffs, int __degree);
float       _RTL_FUNC _IMPORTMM polevlf  (float __x, float *__coeffs, int __degree);
float       _RTL_FUNC _IMPORTMM polyf  (float __x, int __degree, float *__coeffs);
float       _RTL_FUNC _IMPORTMM pow10f (int __p);
#endif

double      _RTL_FUNC _IMPORTMM hypot (double __x, double __y);
double      _RTL_FUNC _IMPORTMM p1evl  (double __x, double *__coeffs, int __degree);
double      _RTL_FUNC _IMPORTMM polevl  (double __x, double *__coeffs, int __degree);
double      _RTL_FUNC _IMPORTMM poly  (double __x, int __degree, double *__coeffs);
double      _RTL_FUNC _IMPORTMM pow10 (int __p);

long double _RTL_FUNC _IMPORTMM hypotl (long double __x, long double __y);
long double _RTL_FUNC _IMPORTMM p1evll  (long double __x, long double *__coeffs, int __degree);
long double _RTL_FUNC _IMPORTMM polevll  (long double __x, long double *__coeffs, int __degree);
long double _RTL_FUNC _IMPORTMM polyl  (long double __x, int __degree, long double *__coeffs);
long double _RTL_FUNC _IMPORTMM pow10l (int __p);

#ifndef __cplusplus
#if !defined(__ABS_DEFINED)
#define __ABS_DEFINED
int         _RTL_INTRINS _IMPORTMM abs(int __x);
long        _RTL_INTRINS _IMPORTMM labs(long __x);
#if  __STDC_VERSION__ >= 199901L
long long   _RTL_FUNC _IMPORTMM llabs(long long __x);
#endif
#endif

#endif /* __ABS_DEFINED */

double      _RTL_FUNC _IMPORT atof  (const char *__s);
int         _matherrl (struct _exceptionl *__e);

long double  _RTL_FUNC _IMPORT _atold (const char *__s);

#if 0
/*
double  _RTL_FUNC _IMPORT _j0(double);
double  _RTL_FUNC _IMPORT _j1(double);
double  _RTL_FUNC _IMPORT _jn(int, double);
double  _RTL_FUNC _IMPORT _y0(double);
double  _RTL_FUNC _IMPORT _y1(double);
double  _RTL_FUNC _IMPORT _yn(int, double);
float  _RTL_FUNC _IMPORT _j0f(float);
float  _RTL_FUNC _IMPORT _j1f(float);
float  _RTL_FUNC _IMPORT _jnf(int, float);
float  _RTL_FUNC _IMPORT _y0f(float);
float  _RTL_FUNC _IMPORT _y1f(float);
float  _RTL_FUNC _IMPORT _ynf(int, float);
long double  _RTL_FUNC _IMPORT _j0l(long double);
long double  _RTL_FUNC _IMPORT _j1l(long double);
long double  _RTL_FUNC _IMPORT _jnl(int, long double);
long double  _RTL_FUNC _IMPORT _y0l(long double);
long double  _RTL_FUNC _IMPORT _y1l(long double);
long double  _RTL_FUNC _IMPORT _ynl(int, long double);
double  _RTL_FUNC _IMPORT j0(double);
double  _RTL_FUNC _IMPORT j1(double);
double  _RTL_FUNC _IMPORT jn(int, double);
double  _RTL_FUNC _IMPORT y0(double);
double  _RTL_FUNC _IMPORT y1(double);
double  _RTL_FUNC _IMPORT yn(int, double);
float  _RTL_FUNC _IMPORT j0f(float);
float  _RTL_FUNC _IMPORT j1f(float);
float  _RTL_FUNC _IMPORT jnf(int, float);
float  _RTL_FUNC _IMPORT y0f(float);
float  _RTL_FUNC _IMPORT y1f(float);
float  _RTL_FUNC _IMPORT ynf(int, float);
long double  _RTL_FUNC _IMPORT j0l(long double);
long double  _RTL_FUNC _IMPORT j1l(long double);
long double  _RTL_FUNC _IMPORT jnl(int, long double);
long double  _RTL_FUNC _IMPORT y0l(long double);
long double  _RTL_FUNC _IMPORT y1l(long double);
long double  _RTL_FUNC _IMPORT ynl(int, long double);
*/
#endif
#ifdef __cplusplus

};

namespace __1
{
inline float abs(float f) { return fabsf(f); }
inline double abs(double f) { return fabs(f); }
inline long double abs(long double f) { return fabsl(f); }
inline int abs(int f) { return fabs(f); }
inline long abs(long f) { return fabs(f); }
inline long long abs(long long f) { return fabs(f); }
}
};
#endif

#pragma pack()

#endif /* math.h */
#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__MATH_H_USING_LIST)
#define __MATH_H_USING_LIST
        using __STD_NS_QUALIFIER __fpclassifyf;
        using __STD_NS_QUALIFIER __fpclassify;
        using __STD_NS_QUALIFIER __fpclassifyl;
        using __STD_NS_QUALIFIER __signbitf;
        using __STD_NS_QUALIFIER __signbit;
        using __STD_NS_QUALIFIER __signbitl;
        using __STD_NS_QUALIFIER __nancompare;
        using __STD_NS_QUALIFIER acosf;
        using __STD_NS_QUALIFIER asinf;
        using __STD_NS_QUALIFIER atan2f;
        using __STD_NS_QUALIFIER atanf;
        using __STD_NS_QUALIFIER ceilf;
        using __STD_NS_QUALIFIER cosf;
        using __STD_NS_QUALIFIER expf;
        using __STD_NS_QUALIFIER fabsf;
        using __STD_NS_QUALIFIER floorf;
        using __STD_NS_QUALIFIER fmodf;
        using __STD_NS_QUALIFIER frexpf;
        using __STD_NS_QUALIFIER ldexpf;
        using __STD_NS_QUALIFIER log10f;
        using __STD_NS_QUALIFIER logf;
        using __STD_NS_QUALIFIER modff;
        using __STD_NS_QUALIFIER powf;
        using __STD_NS_QUALIFIER sinhf;
        using __STD_NS_QUALIFIER sinf;
        using __STD_NS_QUALIFIER sqrtf;
        using __STD_NS_QUALIFIER tanhf;
        using __STD_NS_QUALIFIER tanf;
        using __STD_NS_QUALIFIER acos;
        using __STD_NS_QUALIFIER asin;
        using __STD_NS_QUALIFIER atan2;
        using __STD_NS_QUALIFIER atan;
        using __STD_NS_QUALIFIER ceil;
        using __STD_NS_QUALIFIER cos;
        using __STD_NS_QUALIFIER cosh;
        using __STD_NS_QUALIFIER exp;
        using __STD_NS_QUALIFIER fabs;
        using __STD_NS_QUALIFIER floor;
        using __STD_NS_QUALIFIER fmod;
        using __STD_NS_QUALIFIER frexp;
        using __STD_NS_QUALIFIER ldexp;
        using __STD_NS_QUALIFIER log;
        using __STD_NS_QUALIFIER log10;
        using __STD_NS_QUALIFIER modf;
        using __STD_NS_QUALIFIER pow;
        using __STD_NS_QUALIFIER sin;
        using __STD_NS_QUALIFIER sinh;
        using __STD_NS_QUALIFIER sqrt;
        using __STD_NS_QUALIFIER tan;
        using __STD_NS_QUALIFIER tanh;
        using __STD_NS_QUALIFIER acosl;
        using __STD_NS_QUALIFIER asinl;
        using __STD_NS_QUALIFIER atan2l;
        using __STD_NS_QUALIFIER atanl;
        using __STD_NS_QUALIFIER ceill;
        using __STD_NS_QUALIFIER coshl;
        using __STD_NS_QUALIFIER cosl;
        using __STD_NS_QUALIFIER expl;
        using __STD_NS_QUALIFIER fabsl;
        using __STD_NS_QUALIFIER floorl;
        using __STD_NS_QUALIFIER fmodl;
        using __STD_NS_QUALIFIER frexpl;
        using __STD_NS_QUALIFIER ldexpl;
        using __STD_NS_QUALIFIER log10l;
        using __STD_NS_QUALIFIER logl;
        using __STD_NS_QUALIFIER modfl;
        using __STD_NS_QUALIFIER powl;
        using __STD_NS_QUALIFIER sinhl;
        using __STD_NS_QUALIFIER sinl;
        using __STD_NS_QUALIFIER sqrtl;
        using __STD_NS_QUALIFIER tanhl;
        using __STD_NS_QUALIFIER tanl;
        using __STD_NS_QUALIFIER _copysign;
        using __STD_NS_QUALIFIER acoshf;
        using __STD_NS_QUALIFIER asinhf;
        using __STD_NS_QUALIFIER atanhf;
        using __STD_NS_QUALIFIER cbrtf;
        using __STD_NS_QUALIFIER copysignf;
        using __STD_NS_QUALIFIER coshf;
        using __STD_NS_QUALIFIER erff;
        using __STD_NS_QUALIFIER erfcf;
        using __STD_NS_QUALIFIER exp2f;
        using __STD_NS_QUALIFIER expm1f;
        using __STD_NS_QUALIFIER fdimf;
        using __STD_NS_QUALIFIER fmaf;
        using __STD_NS_QUALIFIER fmaxf;
        using __STD_NS_QUALIFIER fminf;
        using __STD_NS_QUALIFIER ilogbf;
        using __STD_NS_QUALIFIER lgammaf;
        using __STD_NS_QUALIFIER lrintf;
        using __STD_NS_QUALIFIER llrintf;
        using __STD_NS_QUALIFIER log1pf;
        using __STD_NS_QUALIFIER log2f;
        using __STD_NS_QUALIFIER logbf;
        using __STD_NS_QUALIFIER lroundf;
        using __STD_NS_QUALIFIER llroundf;
        using __STD_NS_QUALIFIER nearbyintf;
        using __STD_NS_QUALIFIER nextafterf;
        using __STD_NS_QUALIFIER nexttowardf;
        using __STD_NS_QUALIFIER remainderf;
        using __STD_NS_QUALIFIER remquof;
        using __STD_NS_QUALIFIER rintf;
        using __STD_NS_QUALIFIER roundf;
        using __STD_NS_QUALIFIER scalbnf;
        using __STD_NS_QUALIFIER scalblnf;
    using __STD_NS_QUALIFIER significandf;
        using __STD_NS_QUALIFIER tgammaf;
        using __STD_NS_QUALIFIER truncf;
        using __STD_NS_QUALIFIER acosh;
        using __STD_NS_QUALIFIER asinh;
        using __STD_NS_QUALIFIER atanh;
        using __STD_NS_QUALIFIER cbrt;
        using __STD_NS_QUALIFIER copysign;
        using __STD_NS_QUALIFIER erf;
        using __STD_NS_QUALIFIER erfc;
        using __STD_NS_QUALIFIER exp2;
        using __STD_NS_QUALIFIER expm1;
        using __STD_NS_QUALIFIER fdim;
        using __STD_NS_QUALIFIER fma;
        using __STD_NS_QUALIFIER fmax;
        using __STD_NS_QUALIFIER fmin;
        using __STD_NS_QUALIFIER ilogb;
        using __STD_NS_QUALIFIER lgamma;
        using __STD_NS_QUALIFIER lrint;
        using __STD_NS_QUALIFIER llrint;
        using __STD_NS_QUALIFIER logb;
        using __STD_NS_QUALIFIER log1p;
        using __STD_NS_QUALIFIER log2;
        using __STD_NS_QUALIFIER lround;
        using __STD_NS_QUALIFIER llround;
        using __STD_NS_QUALIFIER nearbyint;
        using __STD_NS_QUALIFIER nextafter;
        using __STD_NS_QUALIFIER nexttoward;
        using __STD_NS_QUALIFIER remainder;
        using __STD_NS_QUALIFIER remquo;
        using __STD_NS_QUALIFIER rint;
        using __STD_NS_QUALIFIER round;
        using __STD_NS_QUALIFIER scalbn;
        using __STD_NS_QUALIFIER scalbln;
    using __STD_NS_QUALIFIER significand;
        using __STD_NS_QUALIFIER tgamma;
        using __STD_NS_QUALIFIER trunc;
        using __STD_NS_QUALIFIER acoshl;
        using __STD_NS_QUALIFIER asinhl;
        using __STD_NS_QUALIFIER atanhl;
        using __STD_NS_QUALIFIER cbrtl;
        using __STD_NS_QUALIFIER copysignl;
        using __STD_NS_QUALIFIER erfl;
        using __STD_NS_QUALIFIER erfcl;
        using __STD_NS_QUALIFIER exp2l;
        using __STD_NS_QUALIFIER expm1l;
        using __STD_NS_QUALIFIER fdiml;
        using __STD_NS_QUALIFIER fmal;
        using __STD_NS_QUALIFIER fmaxl;
        using __STD_NS_QUALIFIER fminl;
        using __STD_NS_QUALIFIER ilogbl;
        using __STD_NS_QUALIFIER lgammal;
        using __STD_NS_QUALIFIER lrintl;
        using __STD_NS_QUALIFIER llrintl;
        using __STD_NS_QUALIFIER log1pl;
        using __STD_NS_QUALIFIER log2l;
        using __STD_NS_QUALIFIER logbl;
        using __STD_NS_QUALIFIER lroundl;
        using __STD_NS_QUALIFIER llroundl;
        using __STD_NS_QUALIFIER nearbyintl;
        using __STD_NS_QUALIFIER nextafterl;
        using __STD_NS_QUALIFIER nexttowardl;
        using __STD_NS_QUALIFIER remainderl;
            using __STD_NS_QUALIFIER remquol;
        using __STD_NS_QUALIFIER rintl;
        using __STD_NS_QUALIFIER roundl;
        using __STD_NS_QUALIFIER scalbnl;
        using __STD_NS_QUALIFIER scalblnl;
    using __STD_NS_QUALIFIER significandl;
        using __STD_NS_QUALIFIER tgammal;
        using __STD_NS_QUALIFIER truncl;
        using __STD_NS_QUALIFIER _matherr;
        using __STD_NS_QUALIFIER hypotf;
        using __STD_NS_QUALIFIER polyf;
        using __STD_NS_QUALIFIER pow10f;
        using __STD_NS_QUALIFIER hypot;
        using __STD_NS_QUALIFIER poly;
        using __STD_NS_QUALIFIER pow10;
        using __STD_NS_QUALIFIER hypotl;
        using __STD_NS_QUALIFIER polyl;
        using __STD_NS_QUALIFIER pow10l;
        using __STD_NS_QUALIFIER abs;
                
        using __STD_NS_QUALIFIER atof;
        using __STD_NS_QUALIFIER _matherrl;
        using __STD_NS_QUALIFIER _atold;
    using __STD_NS_QUALIFIER float_t;
    using __STD_NS_QUALIFIER double_t;
    using __STD_NS_QUALIFIER DOMAIN;        /* argument domain error -- log (-1)        */
    using __STD_NS_QUALIFIER SING;          /* argument singularity  -- pow (0,-2))     */
    using __STD_NS_QUALIFIER OVERFLOW;      /* overflow range error  -- exp (1000)      */
    using __STD_NS_QUALIFIER UNDERFLOW;     /* underflow range error -- exp (-1000)     */
    using __STD_NS_QUALIFIER TLOSS;         /* total loss of significance -- sin(10e70) */
    using __STD_NS_QUALIFIER PLOSS;         /* partial loss of signif. -- not used      */
    using __STD_NS_QUALIFIER STACKFAULT;     /* floating point unit stack overflow       */
    using __STD_NS_QUALIFIER _mexcep;
#endif  /* __MATH_H */
