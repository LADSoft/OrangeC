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
namespace std {
extern "C" {
#endif

#if  __STDC_VERSION__ >= 199901L
int *_RTL_FUNC __GetSignGam(void);

#define signgam (*__GetSignGam())

#endif

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

#if  __STDC_VERSION__ >= 199901L
float _RTL_FUNC nanf(const char *tagp);
double _RTL_FUNC nan(const char *tagp);
long double _RTL_FUNC nanl(const char *tagp);

int _RTL_FUNC __fpclassifyf(float __x);
int _RTL_FUNC __fpclassify(double __x);
int _RTL_FUNC __fpclassifyl(long double __x);
int _RTL_FUNC __signbitf(float __x);
int _RTL_FUNC __signbit(double __x);
int _RTL_FUNC __signbitl(long double __x);
int _RTL_FUNC __nancompare(long double __x, long double __y, int type);


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

float _RTL_FUNC acosf  (float __x);
float _RTL_FUNC asinf  (float __x);
float _RTL_FUNC atan2f (float __y, float __x);
float _RTL_FUNC atanf  (float __x);
float _RTL_FUNC ceilf  (float __x);
float _RTL_FUNC cosf   (float __x);
float _RTL_FUNC coshf  (float __x);
float _RTL_FUNC expf   (float __x);
float _RTL_FUNC fabsf  (float __x);
float _RTL_FUNC floorf (float __x);
float _RTL_FUNC fmodf  (float __x, float __y);
float _RTL_FUNC frexpf (float __x, int *__exponent);
float _RTL_FUNC ldexpf (float __x, int __exponent);
float _RTL_FUNC log10f (float __x);
float _RTL_FUNC logf   (float __x);
float _RTL_FUNC modff  (float __x, float *__y);
float _RTL_FUNC powf   (float __x, float __y);
float _RTL_FUNC sinhf  (float __x);
float _RTL_FUNC sinf   (float __x);
float _RTL_FUNC sqrtf  (float __x);
float _RTL_FUNC tanhf  (float __x);
float _RTL_FUNC tanf   (float __x);



double      _RTL_FUNC acos  (double __x);
double      _RTL_FUNC asin  (double __x);
double      _RTL_FUNC atan2 (double __y, double __x);
double      _RTL_FUNC atan  (double __x);
double      _RTL_FUNC ceil    (double __x);
double      _RTL_FUNC cos     (double __x);
double      _RTL_FUNC cosh    (double __x);
double      _RTL_FUNC exp     (double __x);
double      _RTL_FUNC fabs    (double __x);
double      _RTL_FUNC floor   (double __x);
double      _RTL_FUNC fmod    (double __x, double __y);
double      _RTL_FUNC frexp   (double __x, int *__exponent);
double      _RTL_FUNC ldexp   (double __x, int __exponent);
double      _RTL_FUNC log     (double __x);
double      _RTL_FUNC log10   (double __x);
double      _RTL_FUNC modf    (double __x, double *__ipart);
double      _RTL_FUNC pow     (double __x, double __y);
double      _RTL_FUNC sin     (double __x);
double      _RTL_FUNC sinh    (double __x);
double      _RTL_FUNC sqrt    (double __x);
double      _RTL_FUNC tan     (double __x);
double      _RTL_FUNC tanh    (double __x);

long double _RTL_FUNC acosl  (long double __x);
long double _RTL_FUNC asinl  (long double __x);
long double _RTL_FUNC atan2l (long double __y, long double __x);
long double _RTL_FUNC atanl  (long double __x);
long double _RTL_FUNC ceill  (long double __x);
long double _RTL_FUNC coshl  (long double __x);
long double _RTL_FUNC cosl   (long double __x);
long double _RTL_FUNC expl   (long double __x);
long double _RTL_FUNC fabsl  (long double __x);
long double _RTL_FUNC floorl (long double __x);
long double _RTL_FUNC fmodl  (long double __x, long double __y);
long double _RTL_FUNC frexpl (long double __x, int *__exponent);
long double _RTL_FUNC ldexpl (long double __x, int __exponent);
long double _RTL_FUNC log10l (long double __x);
long double _RTL_FUNC logl   (long double __x);
long double _RTL_FUNC modfl  (long double __x, long double *__ipart);
long double _RTL_FUNC powl   (long double __x, long double __y);
long double _RTL_FUNC sinhl  (long double __x);
long double _RTL_FUNC sinl   (long double __x);
long double _RTL_FUNC sqrtl  (long double __x);
long double _RTL_FUNC tanhl  (long double __x);
long double _RTL_FUNC tanl   (long double __x);

#if  __STDC_VERSION__ >= 199901L

float _RTL_FUNC acoshf  (float __x);
float _RTL_FUNC asinhf  (float __x);
float _RTL_FUNC atanhf  (float __x);
float _RTL_FUNC cbrtf  (float __x);
float _RTL_FUNC copysignf(float __x, float __y);
float _RTL_FUNC erff   (float __x);
float _RTL_FUNC erfcf  (float __x);
float _RTL_FUNC exp2f  (float __x);
float _RTL_FUNC expm1f (float __x);
float _RTL_FUNC fdimf  (float __x, float __y);
float _RTL_FUNC fmaf   (float __x, float __y, float __z);
float _RTL_FUNC fmaxf  (float __x, float __y);
float _RTL_FUNC fminf  (float __x, float __y);
int _RTL_FUNC ilogbf (float __x);
float _RTL_FUNC lgammaf(float __x);
long _RTL_FUNC lrintf (float __x);
long long _RTL_FUNC llrintf(float __x);
float _RTL_FUNC log1pf (float __x);
float _RTL_FUNC log2f  (float __x);
float _RTL_FUNC logbf  (float __x);
long _RTL_FUNC lroundf(float __x);
long long _RTL_FUNC llroundf(float __x);
float _RTL_FUNC nearbyintf(float __x);
float _RTL_FUNC nextafterf  (float __x, float __y);
float _RTL_FUNC nexttowardf  (float __x, long double __y);
float _RTL_FUNC remainderf(float __x, float __y);
float _RTL_FUNC remquof(float __x, float __y, int *quo);
float _RTL_FUNC rintf  (float __x);
float _RTL_FUNC roundf (float __x);
float _RTL_FUNC scalbnf (float __x, int __y);
float _RTL_FUNC scalblnf (float __x, long int __y);
float _RTL_FUNC significandf(float __x);
float _RTL_FUNC tgammaf(float __x);
float _RTL_FUNC truncf (float __x);

double      _RTL_FUNC acosh  (double __x);
double      _RTL_FUNC asinh  (double __x);
double      _RTL_FUNC atanh  (double __x);
double      _RTL_FUNC cbrt    (double __x);
double      _RTL_FUNC copysign(double __x, double __y);
double      _RTL_FUNC erf     (double __x);
double      _RTL_FUNC erfc    (double __x);
double      _RTL_FUNC exp2    (double __x);
double      _RTL_FUNC expm1   (double __x);
double      _RTL_FUNC fdim    (double __x, double __y);
double      _RTL_FUNC fma     (double __x, double __y, double __z);
double      _RTL_FUNC fmax    (double __x, double __y);
double      _RTL_FUNC fmin    (double __x, double __y);
int         _RTL_FUNC ilogb   (double __x);
double      _RTL_FUNC lgamma  (double __x);
long        _RTL_FUNC lrint   (double __x);
long long   _RTL_FUNC llrint  (double __x);
double      _RTL_FUNC logb    (double __x);
double      _RTL_FUNC log1p   (double __x);
double      _RTL_FUNC log2    (double __x);
long        _RTL_FUNC lround  (double __x);
long long   _RTL_FUNC llround (double __x);
double      _RTL_FUNC nearbyint(double __x);
double      _RTL_FUNC nextafter    (double __x, double __y);
double      _RTL_FUNC nexttoward(double __x, long double __y);
double      _RTL_FUNC remainder(double __x, double __y);
double      _RTL_FUNC remquo  (double __x, double __y, int *quo);
double      _RTL_FUNC rint    (double __x);
double      _RTL_FUNC round   (double __x);
double      _RTL_FUNC scalbn  (double __x, int __y);
double      _RTL_FUNC scalbln (double __x, long int __y);
double      _RTL_FUNC significand  (double __x);
double      _RTL_FUNC tgamma  (double __x);
double      _RTL_FUNC trunc   (double __x);

long double _RTL_FUNC acoshl  (long double __x);
long double _RTL_FUNC asinhl  (long double __x);
long double _RTL_FUNC atanhl  (long double __x);
long double _RTL_FUNC cbrtl  (long double __x);
long double _RTL_FUNC copysignl(long double __x, long double __y);
long double _RTL_FUNC erfl   (long double __x);
long double _RTL_FUNC erfcl  (long double __x);
long double _RTL_FUNC exp2l  (long double __x);
long double _RTL_FUNC expm1l (long double __x);
long double _RTL_FUNC fdiml  (long double __x, long double __y);
long double _RTL_FUNC fmal  (long double __x, long double __y, long double __z);
long double _RTL_FUNC fmaxl  (long double __x, long double __y);
long double _RTL_FUNC fminl  (long double __x, long double __y);
int         _RTL_FUNC ilogbl (long double __x);
long double _RTL_FUNC lgammal(long double __x);
long        _RTL_FUNC lrintl (long double __x);
long long   _RTL_FUNC llrintl(long double __x);
long double _RTL_FUNC log1pl (long double __x);
long double _RTL_FUNC log2l  (long double __x);
long double _RTL_FUNC logbl   (long double __x);
long        _RTL_FUNC lroundl(long double __x);
long long   _RTL_FUNC llroundl(long double __x);
long double _RTL_FUNC nearbyintl(long double __x);
long double _RTL_FUNC nextafterl  (long double __x, long double __y);
long double _RTL_FUNC nexttowardl  (long double __x, long double __y);
long double _RTL_FUNC remainderl(long double __x, long double __y);
long double _RTL_FUNC remquol (long double __x, long double __y, int *quo);
long double _RTL_FUNC rintl  (long double __x);
long double _RTL_FUNC roundl (long double __x);
long double _RTL_FUNC scalbnl (long double __x, int __y);
long double _RTL_FUNC scalblnl (long double __x, long int __y);
long double _RTL_FUNC significandl(long double __x);
long double _RTL_FUNC tgammal (long double __x);
long double _RTL_FUNC truncl (long double __x);

#endif

int         _RTL_FUNC _matherr (struct _exception *__e);

#if  __STDC_VERSION__ >= 199901L
float       _RTL_FUNC hypotf (float __x, float __y);
float       _RTL_FUNC p1evlf  (float __x, float *__coeffs, int __degree);
float       _RTL_FUNC polevlf  (float __x, float *__coeffs, int __degree);
float       _RTL_FUNC polyf  (float __x, int __degree, float *__coeffs);
float       _RTL_FUNC pow10f (int __p);
#endif

double      _RTL_FUNC hypot (double __x, double __y);
double      _RTL_FUNC p1evl  (double __x, double *__coeffs, int __degree);
double      _RTL_FUNC polevl  (double __x, double *__coeffs, int __degree);
double      _RTL_FUNC poly  (double __x, int __degree, double *__coeffs);
double      _RTL_FUNC pow10 (int __p);

long double _RTL_FUNC hypotl (long double __x, long double __y);
long double _RTL_FUNC p1evll  (long double __x, long double *__coeffs, int __degree);
long double _RTL_FUNC polevll  (long double __x, long double *__coeffs, int __degree);
long double _RTL_FUNC polyl  (long double __x, int __degree, long double *__coeffs);
long double _RTL_FUNC pow10l (int __p);

#if !defined(__ABS_DEFINED)
#define __ABS_DEFINED
int         _RTL_INTRINS abs(int __x);
long        _RTL_INTRINS labs(long __x);
#if  __STDC_VERSION__ >= 199901L
long long   _RTL_FUNC llabs(long long __x);
#endif

#endif /* __ABS_DEFINED */

double      _RTL_FUNC atof  (const char *__s);
int         _matherrl (struct _exceptionl *__e);

long double  _RTL_FUNC _atold (const char *__s);

#if 0
/*
double  _RTL_FUNC _j0(double);
double  _RTL_FUNC _j1(double);
double  _RTL_FUNC _jn(int, double);
double  _RTL_FUNC _y0(double);
double  _RTL_FUNC _y1(double);
double  _RTL_FUNC _yn(int, double);
float  _RTL_FUNC _j0f(float);
float  _RTL_FUNC _j1f(float);
float  _RTL_FUNC _jnf(int, float);
float  _RTL_FUNC _y0f(float);
float  _RTL_FUNC _y1f(float);
float  _RTL_FUNC _ynf(int, float);
long double  _RTL_FUNC _j0l(long double);
long double  _RTL_FUNC _j1l(long double);
long double  _RTL_FUNC _jnl(int, long double);
long double  _RTL_FUNC _y0l(long double);
long double  _RTL_FUNC _y1l(long double);
long double  _RTL_FUNC _ynl(int, long double);
double  _RTL_FUNC j0(double);
double  _RTL_FUNC j1(double);
double  _RTL_FUNC jn(int, double);
double  _RTL_FUNC y0(double);
double  _RTL_FUNC y1(double);
double  _RTL_FUNC yn(int, double);
float  _RTL_FUNC j0f(float);
float  _RTL_FUNC j1f(float);
float  _RTL_FUNC jnf(int, float);
float  _RTL_FUNC y0f(float);
float  _RTL_FUNC y1f(float);
float  _RTL_FUNC ynf(int, float);
long double  _RTL_FUNC j0l(long double);
long double  _RTL_FUNC j1l(long double);
long double  _RTL_FUNC jnl(int, long double);
long double  _RTL_FUNC y0l(long double);
long double  _RTL_FUNC y1l(long double);
long double  _RTL_FUNC ynl(int, long double);
*/
#endif
#ifdef __cplusplus
};
};
#endif

#pragma pack()

#endif /* math.h */
#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__MATH_H_USING_LIST)
#define __MATH_H_USING_LIST
#if  __STDC_VERSION__ >= 199901L
        using std::__fpclassifyf;
        using std::__fpclassify;
        using std::__fpclassifyl;
        using std::__signbitf;
        using std::__signbit;
        using std::__signbitl;
        using std::__nancompare;
        using std::acosf;
        using std::asinf;
        using std::atan2f;
        using std::atanf;
        using std::ceilf;
        using std::cosf;
        using std::expf;
        using std::fabsf;
        using std::floorf;
        using std::fmodf;
        using std::frexpf;
        using std::ldexpf;
        using std::log10f;
        using std::logf;
        using std::modff;
        using std::powf;
        using std::sinhf;
        using std::sinf;
        using std::sqrtf;
        using std::tanhf;
        using std::tanf;
#endif
        using std::acos;
        using std::asin;
        using std::atan2;
        using std::atan;
        using std::ceil;
        using std::cos;
        using std::cosh;
        using std::exp;
        using std::fabs;
        using std::floor;
        using std::fmod;
        using std::frexp;
        using std::ldexp;
        using std::log;
        using std::log10;
        using std::modf;
        using std::pow;
        using std::sin;
        using std::sinh;
        using std::sqrt;
        using std::tan;
        using std::tanh;
        using std::acosl;
        using std::asinl;
        using std::atan2l;
        using std::atanl;
        using std::ceill;
        using std::coshl;
        using std::cosl;
        using std::expl;
        using std::fabsl;
        using std::floorl;
        using std::fmodl;
        using std::frexpl;
        using std::ldexpl;
        using std::log10l;
        using std::logl;
        using std::modfl;
        using std::powl;
        using std::sinhl;
        using std::sinl;
        using std::sqrtl;
        using std::tanhl;
        using std::tanl;
#if  __STDC_VERSION__ >= 199901L

        using std::acoshf;
        using std::asinhf;
        using std::atanhf;
        using std::cbrtf;
        using std::copysignf;
        using std::coshf;
        using std::erff;
        using std::erfcf;
        using std::exp2f;
        using std::expm1f;
        using std::fdimf;
        using std::fmaf;
        using std::fmaxf;
        using std::fminf;
        using std::ilogbf;
        using std::lgammaf;
        using std::lrintf;
        using std::llrintf;
        using std::log1pf;
        using std::log2f;
        using std::logbf;
        using std::lroundf;
        using std::llroundf;
        using std::nearbyintf;
        using std::nextafterf;
        using std::nexttowardf;
        using std::remainderf;
        using std::remquof;
        using std::rintf;
        using std::roundf;
        using std::scalbnf;
        using std::scalblnf;
    using std::significandf;
        using std::tgammaf;
        using std::truncf;
        using std::acosh;
        using std::asinh;
        using std::atanh;
        using std::cbrt;
        using std::copysign;
        using std::erf;
        using std::erfc;
        using std::exp2;
        using std::expm1;
        using std::fdim;
        using std::fma;
        using std::fmax;
        using std::fmin;
        using std::ilogb;
        using std::lgamma;
        using std::lrint;
        using std::llrint;
        using std::logb;
        using std::log1p;
        using std::log2;
        using std::lround;
        using std::llround;
        using std::nearbyint;
        using std::nextafter;
        using std::nexttoward;
        using std::remainder;
        using std::remquo;
        using std::rint;
        using std::round;
        using std::scalbn;
        using std::scalbln;
    using std::significand;
        using std::tgamma;
        using std::trunc;
        using std::acoshl;
        using std::asinhl;
        using std::atanhl;
        using std::cbrtl;
        using std::copysignl;
        using std::erfl;
        using std::erfcl;
        using std::exp2l;
        using std::expm1l;
        using std::fdiml;
        using std::fmal;
        using std::fmaxl;
        using std::fminl;
        using std::ilogbl;
        using std::lgammal;
        using std::lrintl;
        using std::llrintl;
        using std::log1pl;
        using std::log2l;
        using std::logbl;
        using std::lroundl;
        using std::llroundl;
        using std::nearbyintl;
        using std::nextafterl;
        using std::nexttowardl;
        using std::remainderl;
        using std::remquol;
        using std::rintl;
        using std::roundl;
        using std::scalbnl;
        using std::scalblnl;
    using std::significandl;
        using std::tgammal;
        using std::truncl;
#endif
        using std::_matherr;
#if  __STDC_VERSION__ >= 199901L
        using std::hypotf;
        using std::polyf;
        using std::pow10f;
#endif
        using std::hypot;
        using std::poly;
        using std::pow10;
        using std::hypotl;
        using std::polyl;
        using std::pow10l;
        using std::abs;
        using std::labs;
#if  __STDC_VERSION__ >= 199901L
        using std::llabs;
#endif
        using std::atof;
        using std::_matherrl;
        using std::_atold;
#if  __STDC_VERSION__ >= 199901L
    using std::complex;
    using std::_complexl;
#endif
    using std::float_t;
    using std::double_t;
    using std::DOMAIN;        /* argument domain error -- log (-1)        */
    using std::SING;          /* argument singularity  -- pow (0,-2))     */
    using std::OVERFLOW;      /* overflow range error  -- exp (1000)      */
    using std::UNDERFLOW;     /* underflow range error -- exp (-1000)     */
    using std::TLOSS;         /* total loss of significance -- sin(10e70) */
    using std::PLOSS;         /* partial loss of signif. -- not used      */
    using std::STACKFAULT;     /* floating point unit stack overflow       */
    using std::_mexcep;
#endif  /* __MATH_H */
