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

/*  math.h

    Definitions for the math floating point package.

*/

#pragma GCC system_header

#ifndef __MATH_H
#define __MATH_H

/* these are the defaults even when this file isn't included */
#pragma STDC FENV_ACCESS OFF
#pragma STDC FP_CONTRACT ON

#    include <stddef.h>

#ifdef __cplusplus
#include <limits>
#endif

#pragma pack(1)

#ifdef __cplusplus
    int _RTL_FUNC _IMPORTMM fpclassify(float x);

    bool _RTL_FUNC _IMPORTMM isfinite(float x);
    bool _RTL_FUNC _IMPORTMM isinf(float x);
    bool _RTL_FUNC _IMPORTMM isnan(float x);
    bool _RTL_FUNC _IMPORTMM isnormal(float x);

    bool _RTL_FUNC _IMPORTMM isgreater(float x, float y);
    bool _RTL_FUNC _IMPORTMM isgreaterequal(float x, float y);
    bool _RTL_FUNC _IMPORTMM isless(float x, float y);
    bool _RTL_FUNC _IMPORTMM islessequal(float x, float y);
    bool _RTL_FUNC _IMPORTMM islessgreater(float x, float y);
    bool _RTL_FUNC _IMPORTMM isunordered(float x, float y);
    int _RTL_FUNC _IMPORTMM fpclassify(double x);

    bool _RTL_FUNC _IMPORTMM isfinite(double x);
    bool _RTL_FUNC _IMPORTMM isinf(double x);
    bool _RTL_FUNC _IMPORTMM isnan(double x);
    bool _RTL_FUNC _IMPORTMM isnormal(double x);

    bool _RTL_FUNC _IMPORTMM isgreater(double x, double y);
    bool _RTL_FUNC _IMPORTMM isgreaterequal(double x, double y);
    bool _RTL_FUNC _IMPORTMM isless(double x, double y);
    bool _RTL_FUNC _IMPORTMM islessequal(double x, double y);
    bool _RTL_FUNC _IMPORTMM islessgreater(double x, double y);
    bool _RTL_FUNC _IMPORTMM isunordered(double x, double y);
    int _RTL_FUNC _IMPORTMM fpclassify(long double x);

    bool _RTL_FUNC _IMPORTMM isfinite(long double x);
    bool _RTL_FUNC _IMPORTMM isinf(long double x);
    bool _RTL_FUNC _IMPORTMM isnan(long double x);
    bool _RTL_FUNC _IMPORTMM isnormal(long double x);

    bool _RTL_FUNC _IMPORTMM isgreater(long double x, long double y);
    bool _RTL_FUNC _IMPORTMM isgreaterequal(long double x, long double y);
    bool _RTL_FUNC _IMPORTMM isless(long double x, long double y);
    bool _RTL_FUNC _IMPORTMM islessequal(long double x, long double y);
    bool _RTL_FUNC _IMPORTMM islessgreater(long double x, long double y);
    bool _RTL_FUNC _IMPORTMM isunordered(long double x, long double y);
extern "C"
{
#endif

#if __STDC_VERSION__ >= 199901L
#ifndef RC_INVOKED
    int* _RTL_FUNC _IMPORT __GetSignGam(void);
#endif

#    define signgam (*__GetSignGam())

#endif

#ifndef __cplusplus
#    if defined(__STDC_VERSION__) && __STDC_VERSION__ < 199901L
#ifndef RC_INVOKED
    struct complex /* as used by "_cabs" function */
    {
        double x, y;
    };

    struct _complexl /* as used by "_cabsl" function */
    {
        long double x, y;
    };
#endif
#        define cabs(z) (hypot((z).x, (z).y))
#        define cabsl(z) (hypotl((z).x, (z).y))
#        define _cabsl cabsl
#        define _hypotl(z) (hypotl((z).x, (z).y))

#    endif
#endif
#ifndef RC_INVOKED
    typedef enum
    {
        DOMAIN = 1, /* argument domain error -- log (-1)        */
        SING,       /* argument singularity  -- pow (0,-2))     */
        OVERFLOW,   /* overflow range error  -- exp (1000)      */
        UNDERFLOW,  /* underflow range error -- exp (-1000)     */
        TLOSS,      /* total loss of significance -- sin(10e70) */
        PLOSS,      /* partial loss of signif. -- not used      */
        STACKFAULT  /* floating point unit stack overflow       */
    } _mexcep;
#endif

/* Constants rounded for 21 decimals. */
#define M_E 2.71828182845904523536
#define M_LOG2E 1.44269504088896340736
#define M_LOG10E 0.434294481903251827651
#define M_LN2 0.693147180559945309417
#define M_LN10 2.30258509299404568402
#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923
#define M_PI_4 0.785398163397448309616
#define M_1_PI 0.318309886183790671538
#define M_2_PI 0.636619772367581343076
#define M_1_SQRTPI 0.564189583547756286948
#define M_2_SQRTPI 1.12837916709551257390
#define M_SQRT2 1.41421356237309504880
#define M_SQRT_2 0.707106781186547524401
#define M_LN_2PI 1.837877066409345483561
#define M_LN_SQRT_2PI 0.918938533204672741780
#define M_LN_SQRT_PI 0.572364942924700087072
#define M_SQRT_2dPI	0.797884560802865355880
#define M_LN_SQRT_PId2	0.225791352644727432363

#define INFINITY (0x1.0p128F)
#define NAN (_NAN) /* QUIET NAN */

#define FP_ILOGB0 (-2147483647L - 1) /* INT_MIN*/
#define FP_ILOGBNAN 2147483647L      /* INT_MAX */

/* Classification */
#define FP_INFINITE 0
#define FP_NAN 1
#define FP_NORMAL 2
#define FP_SUBNORMAL 3
#define FP_ZERO 4

#define EDOM 33   /* Math argument */
#define ERANGE 34 /* Result too large */

#ifndef RC_INVOKED

    typedef float float_t;
    typedef double double_t;

    struct _exception
    {
        int type;
        char* name;
        double arg1, arg2, retval;
    };

    struct _exceptionl
    {
        int type;
        char* name;
        long double arg1, arg2, retval;
    };
#endif

/* positive infinities (x86) */
#define HUGE_VAL 0x1P+1024
#define HUGE_VALF 0x1P+128F
#define HUGE_VALL 0x1P+16384L
#define _LHUGE_VAL HUGE_VALL

#ifndef RC_INVOKED
    int _RTL_FUNC _IMPORTMM __fpclassifyf(float __x);
    int _RTL_FUNC _IMPORTMM __fpclassify(double __x);
    int _RTL_FUNC _IMPORTMM __fpclassifyl(long double __x);

#if __STDC_VERSION__ >= 199901L || defined(__cplusplus)
    float _RTL_FUNC _IMPORTMM nanf(const char* ZSTR tagp);
    double _RTL_FUNC _IMPORTMM nan(const char* ZSTR tagp);
    long double _RTL_FUNC _IMPORTMM nanl(const char* ZSTR tagp);

#ifdef __cplusplus
    bool _RTL_FUNC _IMPORTMM signbitf(float __x);
    bool _RTL_FUNC _IMPORTMM signbit(double __x);
    bool _RTL_FUNC _IMPORTMM signbitl(long double __x);
#else
    int _RTL_FUNC _IMPORTMM signbitf(float __x);
    int _RTL_FUNC _IMPORTMM signbit(double __x);
    int _RTL_FUNC _IMPORTMM signbitl(long double __x);
#endif
    int _RTL_FUNC _IMPORTMM __nancompare(long double __x, long double __y, int type);


#endif

#    define fpclassify(x) \
        ((sizeof(x) == sizeof(float)) ? __fpclassifyf(x) : (sizeof(x) == sizeof(double)) ? __fpclassify(x) : __fpclassifyl(x))

#    define signbit(x) ((sizeof(x) == sizeof(float)) ? signbitf(x) : (sizeof(x) == sizeof(double)) ? signbit(x) : signbitl(x))

#    define isfinite(x) (fpclassify(x) >= FP_NORMAL)
#    define isinf(x) (fpclassify(x) == FP_INFINITE)
#    define isnan(x) (fpclassify(x) == FP_NAN)
#    define isnormal(x) (fpclassify(x) == FP_NORMAL)

#    define isgreater(x, y) ((x) > (y))
#    define isgreaterequal(x, y) ((x) >= (y))
#    define isless(x, y) ((x) < (y))
#    define islessequal(x, y) ((x) <= (y))
#    define islessgreater(x, y) ((x) > (y) || (x) < (y))
#    define isunordered(x, y) (isnan(x) || isnan(y))

#endif
#ifndef RC_INVOKED
    int _RTL_FUNC finitef(float x);
    int _RTL_FUNC finite(double x);
    int _RTL_FUNC finitel(long double x);

    float _RTL_FUNC _IMPORTMM acosf(float __x);
    float _RTL_FUNC _IMPORTMM asinf(float __x);
    float _RTL_FUNC _IMPORTMM atan2f(float __y, float __x);
    float _RTL_FUNC _IMPORTMM atanf(float __x);
    float _RTL_FUNC _IMPORTMM ceilf(float __x);
    float _RTL_FUNC _IMPORTMM cosf(float __x);
    float _RTL_FUNC _IMPORTMM coshf(float __x);
    float _RTL_FUNC _IMPORTMM expf(float __x);
    float _RTL_FUNC _IMPORTMM fabsf(float __x);
    float _RTL_FUNC _IMPORTMM floorf(float __x);
    float _RTL_FUNC _IMPORTMM fmodf(float __x, float __y);
    float _RTL_FUNC _IMPORTMM frexpf(float __x, int* __exponent);
    float _RTL_FUNC _IMPORTMM ldexpf(float __x, int __exponent);
    float _RTL_FUNC _IMPORTMM log10f(float __x);
    float _RTL_FUNC _IMPORTMM logf(float __x);
    float _RTL_FUNC _IMPORTMM modff(float __x, float	* __y);
    float _RTL_FUNC _IMPORTMM powf(float __x, float __y);
    float _RTL_FUNC _IMPORTMM sinhf(float __x);
    float _RTL_FUNC _IMPORTMM sinf(float __x);
    float _RTL_FUNC _IMPORTMM sqrtf(float __x);
    float _RTL_FUNC _IMPORTMM tanhf(float __x);
    float _RTL_FUNC _IMPORTMM tanf(float __x);

    double _RTL_FUNC _IMPORTMM acos(double __x);
    double _RTL_FUNC _IMPORTMM asin(double __x);
    double _RTL_FUNC _IMPORTMM atan2(double __y, double __x);
    double _RTL_FUNC _IMPORTMM atan(double __x);
    double _RTL_FUNC _IMPORTMM ceil(double __x);
    double _RTL_FUNC _IMPORTMM cos(double __x);
    double _RTL_FUNC _IMPORTMM cosh(double __x);
    double _RTL_FUNC _IMPORTMM exp(double __x);
    double _RTL_FUNC _IMPORTMM fabs(double __x);
    double _RTL_FUNC _IMPORTMM floor(double __x);
    double _RTL_FUNC _IMPORTMM fmod(double __x, double __y);
    double _RTL_FUNC _IMPORTMM frexp(double __x, int* __exponent);
    double _RTL_FUNC _IMPORTMM ldexp(double __x, int __exponent);
    double _RTL_FUNC _IMPORTMM log(double __x);
    double _RTL_FUNC _IMPORTMM log10(double __x);
    double _RTL_FUNC _IMPORTMM modf(double __x, double* __ipart);
    double _RTL_FUNC _IMPORTMM pow(double __x, double __y);
    double _RTL_FUNC _IMPORTMM sin(double __x);
    double _RTL_FUNC _IMPORTMM sinh(double __x);
    double _RTL_FUNC _IMPORTMM sqrt(double __x);
    double _RTL_FUNC _IMPORTMM tan(double __x);
    double _RTL_FUNC _IMPORTMM tanh(double __x);

    long double _RTL_FUNC _IMPORTMM acosl(long double __x);
    long double _RTL_FUNC _IMPORTMM asinl(long double __x);
    long double _RTL_FUNC _IMPORTMM atan2l(long double __y, long double __x);
    long double _RTL_FUNC _IMPORTMM atanl(long double __x);
    long double _RTL_FUNC _IMPORTMM ceill(long double __x);
    long double _RTL_FUNC _IMPORTMM coshl(long double __x);
    long double _RTL_FUNC _IMPORTMM cosl(long double __x);
    long double _RTL_FUNC _IMPORTMM expl(long double __x);
    long double _RTL_FUNC _IMPORTMM fabsl(long double __x);
    long double _RTL_FUNC _IMPORTMM floorl(long double __x);
    long double _RTL_FUNC _IMPORTMM fmodl(long double __x, long double __y);
    long double _RTL_FUNC _IMPORTMM frexpl(long double __x, int* __exponent);
    long double _RTL_FUNC _IMPORTMM ldexpl(long double __x, int __exponent);
    long double _RTL_FUNC _IMPORTMM log10l(long double __x);
    long double _RTL_FUNC _IMPORTMM logl(long double __x);
    long double _RTL_FUNC _IMPORTMM modfl(long double __x, long double* __ipart);
    long double _RTL_FUNC _IMPORTMM powl(long double __x, long double __y);
    long double _RTL_FUNC _IMPORTMM sinhl(long double __x);
    long double _RTL_FUNC _IMPORTMM sinl(long double __x);
    long double _RTL_FUNC _IMPORTMM sqrtl(long double __x);
    long double _RTL_FUNC _IMPORTMM tanhl(long double __x);
    long double _RTL_FUNC _IMPORTMM tanl(long double __x);

    double _RTL_FUNC _IMPORTMM _copysign(double __x, double __y);

#if __STDC_VERSION__ >= 199901L || defined(__cplusplus)

    float _RTL_FUNC _IMPORTMM acoshf(float __x);
    float _RTL_FUNC _IMPORTMM asinhf(float __x);
    float _RTL_FUNC _IMPORTMM atanhf(float __x);
    float _RTL_FUNC _IMPORTMM cbrtf(float __x);
    float _RTL_FUNC _IMPORTMM copysignf(float __x, float __y);
    float _RTL_FUNC _IMPORTMM erff(float __x);
    float _RTL_FUNC _IMPORTMM erfcf(float __x);
    float _RTL_FUNC _IMPORTMM exp2f(float __x);
    float _RTL_FUNC _IMPORTMM expm1f(float __x);
    float _RTL_FUNC _IMPORTMM fdimf(float __x, float __y);
    float _RTL_FUNC _IMPORTMM fmaf(float __x, float __y, float __z);
    float _RTL_FUNC _IMPORTMM fmaxf(float __x, float __y);
    float _RTL_FUNC _IMPORTMM fminf(float __x, float __y);
    int _RTL_FUNC _IMPORTMM ilogbf(float __x);
    float _RTL_FUNC _IMPORTMM lgammaf(float __x);
    long _RTL_FUNC _IMPORTMM lrintf(float __x);
    long long _RTL_FUNC _IMPORTMM llrintf(float __x);
    float _RTL_FUNC _IMPORTMM log1pf(float __x);
    float _RTL_FUNC _IMPORTMM log2f(float __x);
    float _RTL_FUNC _IMPORTMM logbf(float __x);
    long _RTL_FUNC _IMPORTMM lroundf(float __x);
    long long _RTL_FUNC _IMPORTMM llroundf(float __x);
    float _RTL_FUNC _IMPORTMM nearbyintf(float __x);
    float _RTL_FUNC _IMPORTMM nextafterf(float __x, float __y);
    float _RTL_FUNC _IMPORTMM nexttowardf(float __x, long double __y);
    float _RTL_FUNC _IMPORTMM remainderf(float __x, float __y);
    float _RTL_FUNC _IMPORTMM remquof(float __x, float __y, int* quo);
    float _RTL_FUNC _IMPORTMM rintf(float __x);
    float _RTL_FUNC _IMPORTMM roundf(float __x);
    float _RTL_FUNC _IMPORTMM scalbnf(float __x, int __y);
    float _RTL_FUNC _IMPORTMM scalblnf(float __x, long int __y);
    float _RTL_FUNC _IMPORTMM significandf(float __x);
    float _RTL_FUNC _IMPORTMM tgammaf(float __x);
    float _RTL_FUNC _IMPORTMM truncf(float __x);

    double _RTL_FUNC _IMPORTMM acosh(double __x);
    double _RTL_FUNC _IMPORTMM asinh(double __x);
    double _RTL_FUNC _IMPORTMM atanh(double __x);
    double _RTL_FUNC _IMPORTMM cbrt(double __x);
    double _RTL_FUNC _IMPORTMM copysign(double __x, double __y);
    double _RTL_FUNC _IMPORTMM erf(double __x);
    double _RTL_FUNC _IMPORTMM erfc(double __x);
    double _RTL_FUNC _IMPORTMM exp2(double __x);
    double _RTL_FUNC _IMPORTMM expm1(double __x);
    double _RTL_FUNC _IMPORTMM fdim(double __x, double __y);
    double _RTL_FUNC _IMPORTMM fma(double __x, double __y, double __z);
    double _RTL_FUNC _IMPORTMM fmax(double __x, double __y);
    double _RTL_FUNC _IMPORTMM fmin(double __x, double __y);
    int _RTL_FUNC _IMPORTMM ilogb(double __x);
    double _RTL_FUNC _IMPORTMM lgamma(double __x);
    long _RTL_FUNC _IMPORTMM lrint(double __x);
    long long _RTL_FUNC _IMPORTMM llrint(double __x);
    double _RTL_FUNC _IMPORTMM logb(double __x);
    double _RTL_FUNC _IMPORTMM log1p(double __x);
    double _RTL_FUNC _IMPORTMM log2(double __x);
    long _RTL_FUNC _IMPORTMM lround(double __x);
    long long _RTL_FUNC _IMPORTMM llround(double __x);
    double _RTL_FUNC _IMPORTMM nearbyint(double __x);
    double _RTL_FUNC _IMPORTMM nextafter(double __x, double __y);
    double _RTL_FUNC _IMPORTMM nexttoward(double __x, long double __y);
    double _RTL_FUNC _IMPORTMM remainder(double __x, double __y);
    double _RTL_FUNC _IMPORTMM remquo(double __x, double __y, int* quo);
    double _RTL_FUNC _IMPORTMM rint(double __x);
    double _RTL_FUNC _IMPORTMM round(double __x);
    double _RTL_FUNC _IMPORTMM scalbn(double __x, int __y);
    double _RTL_FUNC _IMPORTMM scalbln(double __x, long int __y);
    double _RTL_FUNC _IMPORTMM significand(double __x);
    double _RTL_FUNC _IMPORTMM tgamma(double __x);
    double _RTL_FUNC _IMPORTMM trunc(double __x);

    long double _RTL_FUNC _IMPORTMM acoshl(long double __x);
    long double _RTL_FUNC _IMPORTMM asinhl(long double __x);
    long double _RTL_FUNC _IMPORTMM atanhl(long double __x);
    long double _RTL_FUNC _IMPORTMM cbrtl(long double __x);
    long double _RTL_FUNC _IMPORTMM copysignl(long double __x, long double __y);
    long double _RTL_FUNC _IMPORTMM erfl(long double __x);
    long double _RTL_FUNC _IMPORTMM erfcl(long double __x);
    long double _RTL_FUNC _IMPORTMM exp2l(long double __x);
    long double _RTL_FUNC _IMPORTMM expm1l(long double __x);
    long double _RTL_FUNC _IMPORTMM fdiml(long double __x, long double __y);
    long double _RTL_FUNC _IMPORTMM fmal(long double __x, long double __y, long double __z);
    long double _RTL_FUNC _IMPORTMM fmaxl(long double __x, long double __y);
    long double _RTL_FUNC _IMPORTMM fminl(long double __x, long double __y);
    int _RTL_FUNC _IMPORTMM ilogbl(long double __x);
    long double _RTL_FUNC _IMPORTMM lgammal(long double __x);
    long _RTL_FUNC _IMPORTMM lrintl(long double __x);
    long long _RTL_FUNC _IMPORTMM llrintl(long double __x);
    long double _RTL_FUNC _IMPORTMM log1pl(long double __x);
    long double _RTL_FUNC _IMPORTMM log2l(long double __x);
    long double _RTL_FUNC _IMPORTMM logbl(long double __x);
    long _RTL_FUNC _IMPORTMM lroundl(long double __x);
    long long _RTL_FUNC _IMPORTMM llroundl(long double __x);
    long double _RTL_FUNC _IMPORTMM nearbyintl(long double __x);
    long double _RTL_FUNC _IMPORTMM nextafterl(long double __x, long double __y);
    long double _RTL_FUNC _IMPORTMM nexttowardl(long double __x, long double __y);
    long double _RTL_FUNC _IMPORTMM remainderl(long double __x, long double __y);
    long double _RTL_FUNC _IMPORTMM remquol(long double __x, long double __y, int* quo);
    long double _RTL_FUNC _IMPORTMM rintl(long double __x);
    long double _RTL_FUNC _IMPORTMM roundl(long double __x);
    long double _RTL_FUNC _IMPORTMM scalbnl(long double __x, int __y);
    long double _RTL_FUNC _IMPORTMM scalblnl(long double __x, long int __y);
    long double _RTL_FUNC _IMPORTMM significandl(long double __x);
    long double _RTL_FUNC _IMPORTMM tgammal(long double __x);
    long double _RTL_FUNC _IMPORTMM truncl(long double __x);

#endif

    int _RTL_FUNC _matherr(struct _exception* __e);

#if __STDC_VERSION__ >= 199901L || defined(__cplusplus)
    float _RTL_FUNC _IMPORTMM hypotf(float __x, float __y);
    float _RTL_FUNC _IMPORTMM p1evlf(float __x, float* __coeffs, int __degree);
    float _RTL_FUNC _IMPORTMM polevlf(float __x, float* __coeffs, int __degree);
    float _RTL_FUNC _IMPORTMM polyf(float __x, int __degree, float* __coeffs);
    float _RTL_FUNC _IMPORTMM pow10f(int __p);
#endif

    double _RTL_FUNC _IMPORTMM hypot(double __x, double __y);
    double _RTL_FUNC _IMPORTMM p1evl(double __x, double* __coeffs, int __degree);
    double _RTL_FUNC _IMPORTMM polevl(double __x, double* __coeffs, int __degree);
    double _RTL_FUNC _IMPORTMM poly(double __x, int __degree, double* __coeffs);
    double _RTL_FUNC _IMPORTMM pow10(int __p);

    long double _RTL_FUNC _IMPORTMM hypotl(long double __x, long double __y);
    long double _RTL_FUNC _IMPORTMM p1evll(long double __x, long double* __coeffs, int __degree);
    long double _RTL_FUNC _IMPORTMM polevll(long double __x, long double* __coeffs, int __degree);
    long double _RTL_FUNC _IMPORTMM polyl(long double __x, int __degree, long double* __coeffs);
    long double _RTL_FUNC _IMPORTMM pow10l(int __p);

#if !defined(__ABS_DEFINED)
#    define __ABS_DEFINED
    int _RTL_INTRINS _IMPORTMM abs(int __x);
    long _RTL_INTRINS _IMPORTMM labs(long __x);
#    if __STDC_VERSION__ >= 199901L || defined(__cplusplus)
    long long _RTL_FUNC _IMPORTMM llabs(long long __x);
#    endif
#endif

    double _RTL_FUNC _IMPORT atof(const char* ZSTR __s);
    int _matherrl(struct _exceptionl* __e);

    long double _RTL_FUNC _IMPORT _atold(const char* ZSTR __s);
#endif

#ifdef __cplusplus
    inline float _RTL_CONSTEXPR __builtin_acosf(float __x) { return acosf( __x); }
    inline double _RTL_CONSTEXPR __builtin_acos(double __x) { return acos( __x); }
    inline long double _RTL_CONSTEXPR __builtin_acosl(long double __x) { return acosl( __x); }
    inline float _RTL_CONSTEXPR __builtin_asinf(float __x) { return asinf( __x); }
    inline double _RTL_CONSTEXPR __builtin_asin(double __x) { return asin( __x); }
    inline long double _RTL_CONSTEXPR __builtin_asinl(long double __x) { return asinl( __x); }
    inline float _RTL_CONSTEXPR __builtin_atan2f(float __y, float __x) { return atan2f( __y,  __x); }
    inline double _RTL_CONSTEXPR __builtin_atan2(double __y, double __x) { return atan2( __y,  __x); }
    inline long double _RTL_CONSTEXPR __builtin_atan2l(long double __y, long double __x) { return atan2l( __y,  __x); }
    inline float _RTL_CONSTEXPR __builtin_atanf(float __x) { return atanf( __x); }
    inline double _RTL_CONSTEXPR __builtin_atan(double __x) { return atan( __x); }
    inline long double _RTL_CONSTEXPR __builtin_atanl(long double __x) { return atanl( __x); }
    inline float _RTL_CONSTEXPR __builtin_ceilf(float __x) { return ceilf( __x); }
    inline double _RTL_CONSTEXPR __builtin_ceil(double __x) { return ceil( __x); }
    inline long double _RTL_CONSTEXPR __builtin_ceill(long double __x) { return ceill( __x); }
    inline float _RTL_CONSTEXPR __builtin_cosf(float __x) { return cosf( __x); }
    inline double _RTL_CONSTEXPR __builtin_cos(double __x) { return cos( __x); }
    inline long double _RTL_CONSTEXPR __builtin_cosl(long double __x) { return cosl( __x); }
    inline float _RTL_CONSTEXPR __builtin_coshf(float __x) { return coshf( __x); }
    inline double _RTL_CONSTEXPR __builtin_cosh(double __x) { return cosh( __x); }
    inline long double _RTL_CONSTEXPR __builtin_coshl(long double __x) { return coshl( __x); }
    inline float _RTL_CONSTEXPR __builtin_expf(float __x) { return expf( __x); }
    inline double _RTL_CONSTEXPR __builtin_exp(double __x) { return exp( __x); }
    inline long double _RTL_CONSTEXPR __builtin_expl(long double __x) { return expl( __x); }
#ifndef _FABS_
#define _FABS_
    inline float _RTL_CONSTEXPR __builtin_fabsf(float __x) { return fabsf( __x); }
    inline double _RTL_CONSTEXPR __builtin_fabs(double __x) { return fabs( __x); }
    inline long double _RTL_CONSTEXPR __builtin_fabsl(long double __x) { return fabsl( __x); }
#endif
    inline float _RTL_CONSTEXPR __builtin_floorf(float __x) { return floorf( __x); }
    inline double _RTL_CONSTEXPR __builtin_floor(double __x) { return floor( __x); }
    inline long double _RTL_CONSTEXPR __builtin_floorl(long double __x) { return floorl( __x); }
    inline float _RTL_CONSTEXPR __builtin_fmodf(float __x, float __y) { return fmodf( __x,  __y); }
    inline double _RTL_CONSTEXPR __builtin_fmod(double __x, double __y) { return fmod( __x,  __y); }
    inline long double _RTL_CONSTEXPR __builtin_fmodl(long double __x, long double __y) { return fmodl( __x,  __y); }
    inline float _RTL_CONSTEXPR __builtin_frexpf(float __x, int* __exponent) { return frexpf( __x, __exponent); }
    inline double _RTL_CONSTEXPR __builtin_frexp(double __x, int* __exponent) { return frexp( __x, __exponent); }
    inline long double _RTL_CONSTEXPR __builtin_frexpl(long double __x, int* __exponent) { return frexpl( __x, __exponent); }
    inline float _RTL_CONSTEXPR __builtin_ldexpf(float __x, int __exponent) { return ldexpf( __x, __exponent); }
    inline double _RTL_CONSTEXPR __builtin_ldexp(double __x, int __exponent) { return ldexp( __x, __exponent); }
    inline long double _RTL_CONSTEXPR __builtin_ldexpl(long double __x, int __exponent) { return ldexpl( __x, __exponent); }
    inline float _RTL_CONSTEXPR __builtin_logf(float __x) { return logf( __x); }
    inline double _RTL_CONSTEXPR __builtin_log(double __x) { return log( __x); }
    inline long double _RTL_CONSTEXPR __builtin_logl(long double __x) { return logl( __x); }
    inline float _RTL_CONSTEXPR __builtin_log10f(float __x) { return log10f( __x); }
    inline double _RTL_CONSTEXPR __builtin_log10(double __x) { return log10( __x); }
    inline long double _RTL_CONSTEXPR __builtin_log10l(long double __x) { return log10l( __x); }
    inline float _RTL_CONSTEXPR __builtin_modff(float __x, float*  __ipart) { return modff( __x, __ipart); }
    inline double _RTL_CONSTEXPR __builtin_modf(double __x, double* __ipart) { return modf( __x, __ipart); }
    inline long double _RTL_CONSTEXPR __builtin_modfl(long double __x, long double* __ipart) { return modfl( __x, __ipart); }
    inline float _RTL_CONSTEXPR __builtin_powf(float __x, float __y) { return powf( __x,  __y); }
    inline double _RTL_CONSTEXPR __builtin_pow(double __x, double __y) { return pow( __x,  __y); }
    inline long double _RTL_CONSTEXPR __builtin_powl(long double __x, long double __y) { return powl( __x,  __y); }
    inline float _RTL_CONSTEXPR __builtin_sinf(float __x) { return sinf( __x); }
    inline double _RTL_CONSTEXPR __builtin_sin(double __x) { return sin( __x); }
    inline long double _RTL_CONSTEXPR __builtin_sinl(long double __x) { return sinl( __x); }
    inline float _RTL_CONSTEXPR __builtin_sinhf(float __x) { return sinhf( __x); }
    inline double _RTL_CONSTEXPR __builtin_sinh(double __x) { return sinh( __x); }
    inline long double _RTL_CONSTEXPR __builtin_sinhl(long double __x) { return sinhl( __x); }
    inline float _RTL_CONSTEXPR __builtin_sqrtf(float __x) { return sqrtf( __x); }
    inline double _RTL_CONSTEXPR __builtin_sqrt(double __x) { return sqrt( __x); }
    inline long double _RTL_CONSTEXPR __builtin_sqrtl(long double __x) { return sqrtl( __x); }
    inline float _RTL_CONSTEXPR __builtin_tanf(float __x) { return tanf( __x); }
    inline double _RTL_CONSTEXPR __builtin_tan(double __x) { return tan( __x); }
    inline long double _RTL_CONSTEXPR __builtin_tanl(long double __x) { return tanl( __x); }
    inline float _RTL_CONSTEXPR __builtin_tanhf(float __x) { return tanhf( __x); }
    inline double _RTL_CONSTEXPR __builtin_tanh(double __x) { return tanh( __x); }
    inline long double _RTL_CONSTEXPR __builtin_tanhl(long double __x) { return tanhl( __x); }
    inline float _RTL_CONSTEXPR __builtin_acoshf(float __x) { return acoshf( __x); }
    inline double _RTL_CONSTEXPR __builtin_acosh(double __x) { return acosh( __x); }
    inline long double _RTL_CONSTEXPR __builtin_acoshl(long double __x) { return acoshl( __x); }
    inline float _RTL_CONSTEXPR __builtin_asinhf(float __x) { return asinhf( __x); }
    inline double _RTL_CONSTEXPR __builtin_asinh(double __x) { return asinh( __x); }
    inline long double _RTL_CONSTEXPR __builtin_asinhl(long double __x) { return asinhl( __x); }
    inline float _RTL_CONSTEXPR __builtin_atanhf(float __x) { return atanhf( __x); }
    inline double _RTL_CONSTEXPR __builtin_atanh(double __x) { return atanh( __x); }
    inline long double _RTL_CONSTEXPR __builtin_atanhl(long double __x) { return atanhl( __x); }
    inline float _RTL_CONSTEXPR __builtin_cbrtf(float __x) { return cbrtf( __x); }
    inline double _RTL_CONSTEXPR __builtin_cbrt(double __x) { return cbrt( __x); }
    inline long double _RTL_CONSTEXPR __builtin_cbrtl(long double __x) { return cbrtl( __x); }
    inline float _RTL_CONSTEXPR __builtin_copysignf(float __x, float __y) { return copysignf( __x,  __y); }
    inline double _RTL_CONSTEXPR __builtin_copysign(double __x, double __y) { return copysign( __x,  __y); }
    inline long double _RTL_CONSTEXPR __builtin_copysignl(long double __x, long double __y) { return copysignl( __x,  __y); }
    inline float _RTL_CONSTEXPR __builtin_erff(float __x) { return erff( __x); }
    inline double _RTL_CONSTEXPR __builtin_erf(double __x) { return erf( __x); }
    inline long double _RTL_CONSTEXPR __builtin_erfl(long double __x) { return erfl( __x); }
    inline float _RTL_CONSTEXPR __builtin_erfcf(float __x) { return erfcf( __x); }
    inline double _RTL_CONSTEXPR __builtin_erfc(double __x) { return erfc( __x); }
    inline long double _RTL_CONSTEXPR __builtin_erfcl(long double __x) { return erfcl( __x); }
    inline float _RTL_CONSTEXPR __builtin_exp2f(float __x) { return exp2f( __x); }
    inline double _RTL_CONSTEXPR __builtin_exp2(double __x) { return exp2( __x); }
    inline long double _RTL_CONSTEXPR __builtin_exp2l(long double __x) { return exp2l( __x); }
    inline float _RTL_CONSTEXPR __builtin_expm1f(float __x) { return expm1f( __x); }
    inline double _RTL_CONSTEXPR __builtin_expm1(double __x) { return expm1( __x); }
    inline long double _RTL_CONSTEXPR __builtin_expm1l(long double __x) { return expm1l( __x); }
    inline float _RTL_CONSTEXPR __builtin_fdimf(float __x, float __y) { return fdimf( __x,  __y); }
    inline double _RTL_CONSTEXPR __builtin_fdim(double __x, double __y) { return fdim( __x,  __y); }
    inline long double _RTL_CONSTEXPR __builtin_fdiml(long double __x, long double __y) { return fdiml( __x,  __y); }
    inline float _RTL_CONSTEXPR __builtin_fmaf(float __x, float __y, float __z) { return fmaf( __x,  __y,  __z); }
    inline double _RTL_CONSTEXPR __builtin_fma(double __x, double __y, double __z) { return fma( __x,  __y,  __z); }
    inline long double _RTL_CONSTEXPR __builtin_fmal(long double __x, long double __y, long double __z) { return fmal( __x,  __y,  __z); }
    inline float _RTL_CONSTEXPR __builtin_fmaxf(float __x, float __y) { return fmaxf( __x,  __y); }
    inline double _RTL_CONSTEXPR __builtin_fmax(double __x, double __y) { return fmax( __x,  __y); }
    inline long double _RTL_CONSTEXPR __builtin_fmaxl(long double __x, long double __y) { return fmaxl( __x,  __y); }
    inline float _RTL_CONSTEXPR __builtin_fminf(float __x, float __y) { return fminf( __x,  __y); }
    inline double _RTL_CONSTEXPR __builtin_fmin(double __x, double __y) { return fmin( __x,  __y); }
    inline long double _RTL_CONSTEXPR __builtin_fminl(long double __x, long double __y) { return fminl( __x,  __y); }
    inline int _RTL_CONSTEXPR __builtin_ilogbf(float __x) { return ilogbf( __x); }
    inline int _RTL_CONSTEXPR __builtin_ilogb(double __x) { return ilogb( __x); }
    inline int _RTL_CONSTEXPR __builtin_ilogbl(long double __x) { return ilogbl( __x); }
    inline float _RTL_CONSTEXPR __builtin_lgammaf(float __x) { return lgammaf( __x); }
    inline double _RTL_CONSTEXPR __builtin_lgamma(double __x) { return lgamma( __x); }
    inline long double _RTL_CONSTEXPR __builtin_lgammal(long double __x) { return lgammal( __x); }
    inline long _RTL_CONSTEXPR __builtin_lrintf(float __x) { return lrintf( __x); }
    inline long _RTL_CONSTEXPR __builtin_lrint(double __x) { return lrint( __x); }
    inline long _RTL_CONSTEXPR __builtin_lrintl(long double __x) { return lrintl( __x); }
    inline long long _RTL_CONSTEXPR __builtin_llrintf(float __x) { return llrintf( __x); }
    inline long long _RTL_CONSTEXPR __builtin_llrint(double __x) { return llrint( __x); }
    inline long long _RTL_CONSTEXPR __builtin_llrintl(long double __x) { return llrintl( __x); }
    inline float _RTL_CONSTEXPR __builtin_logbf(float __x) { return logbf( __x); }
    inline double _RTL_CONSTEXPR __builtin_logb(double __x) { return logb( __x); }
    inline long double _RTL_CONSTEXPR __builtin_logbl(long double __x) { return logbl( __x); }
    inline float _RTL_CONSTEXPR __builtin_log1pf(float __x) { return log1pf( __x); }
    inline double _RTL_CONSTEXPR __builtin_log1p(double __x) { return log1p( __x); }
    inline long double _RTL_CONSTEXPR __builtin_log1pl(long double __x) { return log1pl( __x); }
    inline float _RTL_CONSTEXPR __builtin_log2f(float __x) { return log2f( __x); }
    inline double _RTL_CONSTEXPR __builtin_log2(double __x) { return log2( __x); }
    inline long double _RTL_CONSTEXPR __builtin_log2l(long double __x) { return log2l( __x); }
    inline long _RTL_CONSTEXPR __builtin_lroundf(float __x) { return lroundf( __x); }
    inline long _RTL_CONSTEXPR __builtin_lround(double __x) { return lround( __x); }
    inline long _RTL_CONSTEXPR __builtin_lroundl(long double __x) { return lroundl( __x); }
    inline long long _RTL_CONSTEXPR __builtin_llroundf(float __x) { return llroundf( __x); }
    inline long long _RTL_CONSTEXPR __builtin_llround(double __x) { return llround( __x); }
    inline long long _RTL_CONSTEXPR __builtin_llroundl(long double __x) { return llroundl( __x); }
    inline float _RTL_CONSTEXPR __builtin_nearbyintf(float __x) { return nearbyintf( __x); }
    inline double _RTL_CONSTEXPR __builtin_nearbyint(double __x) { return nearbyint( __x); }
    inline long double _RTL_CONSTEXPR __builtin_nearbyintl(long double __x) { return nearbyintl( __x); }
    inline float _RTL_CONSTEXPR __builtin_nextafterf(float __x, float __y) { return nextafterf( __x,  __y); }
    inline double _RTL_CONSTEXPR __builtin_nextafter(double __x, double __y) { return nextafter( __x,  __y); }
    inline long double _RTL_CONSTEXPR __builtin_nextafterl(long double __x, long double __y) { return nextafterl( __x,  __y); }
    inline float _RTL_CONSTEXPR __builtin_nexttowardf(float __x, long double __y) { return nexttowardf( __x,  __y); }
    inline double _RTL_CONSTEXPR __builtin_nexttoward(double __x, long double __y) { return nexttoward( __x,  __y); }
    inline long double _RTL_CONSTEXPR __builtin_nexttowardl(long double __x, long double __y) { return nexttowardl( __x,  __y); }
    inline float _RTL_CONSTEXPR __builtin_remainderf(float __x, float __y) { return remainderf( __x,  __y); }
    inline double _RTL_CONSTEXPR __builtin_remainder(double __x, double __y) { return remainder( __x,  __y); }
    inline long double _RTL_CONSTEXPR __builtin_remainderl(long double __x, long double __y) { return remainderl( __x,  __y); }
    inline float _RTL_CONSTEXPR __builtin_remquof(float __x, float __y, int* quo) { return remquof( __x,  __y, quo); }
    inline double _RTL_CONSTEXPR __builtin_remquo(double __x, double __y, int* quo) { return remquo( __x,  __y, quo); }
    inline long double _RTL_CONSTEXPR __builtin_remquol(long double __x, long double __y, int* quo) { return remquol( __x,  __y, quo); }
    inline float _RTL_CONSTEXPR __builtin_rintf(float __x) { return rintf( __x); }
    inline double _RTL_CONSTEXPR __builtin_rint(double __x) { return rint( __x); }
    inline long double _RTL_CONSTEXPR __builtin_rintl(long double __x) { return rintl( __x); }
    inline float _RTL_CONSTEXPR __builtin_roundf(float __x) { return roundf( __x); }
    inline double _RTL_CONSTEXPR __builtin_round(double __x) { return round( __x); }
    inline long double _RTL_CONSTEXPR __builtin_roundl(long double __x) { return roundl( __x); }
    inline float _RTL_CONSTEXPR __builtin_scalbnf(float __x, int __y) { return scalbnf( __x, __y); }
    inline double _RTL_CONSTEXPR __builtin_scalbn(double __x, int __y) { return scalbn( __x, __y); }
    inline long double _RTL_CONSTEXPR __builtin_scalbnl(long double __x, int __y) { return scalbnl( __x, __y); }
    inline float _RTL_CONSTEXPR __builtin_scalblnf(float __x, long int __y) { return scalblnf( __x, __y); }
    inline double _RTL_CONSTEXPR __builtin_scalbln(double __x, long int __y) { return scalbln( __x, __y); }
    inline long double _RTL_CONSTEXPR __builtin_scalblnl(long double __x, long int __y) { return scalblnl( __x, __y); }
    inline float _RTL_CONSTEXPR __builtin_significandf(float __x) { return significandf( __x); }
    inline double _RTL_CONSTEXPR __builtin_significand(double __x) { return significand( __x); }
    inline long double _RTL_CONSTEXPR __builtin_significandl(long double __x) { return significandl( __x); }
    inline float _RTL_CONSTEXPR __builtin_tgammaf(float __x) { return tgammaf( __x); }
    inline double _RTL_CONSTEXPR __builtin_tgamma(double __x) { return tgamma( __x); }
    inline long double _RTL_CONSTEXPR __builtin_tgammal(long double __x) { return tgammal( __x); }
    inline float _RTL_CONSTEXPR __builtin_truncf(float __x) { return truncf( __x); }
    inline double _RTL_CONSTEXPR __builtin_trunc(double __x) { return trunc( __x); }
    inline long double _RTL_CONSTEXPR __builtin_truncl(long double __x) { return truncl( __x); }
    inline float _RTL_CONSTEXPR __builtin_hypotf(float __x, float __y) { return hypotf(__x, __y); }
    inline double _RTL_CONSTEXPR __builtin_hypot(double __x, double __y) { return hypot(__x, __y); }
    inline long double _RTL_CONSTEXPR __builtin_hypotl(long double __x, long double __y) { return hypotl(__x, __y); }
#endif
#ifdef __cplusplus
};
#endif

#pragma pack()

#endif /* math.h */
