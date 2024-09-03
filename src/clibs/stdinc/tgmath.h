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

#ifndef __TGMATH_H
#define __TGMATH_H

#    include <math.h>
#    include <complex.h>

#define acos(arg)                                               \
    ((__typeid(arg) == 22)                                      \
         ? (cacosl)(arg)                                        \
         : (__typeid(arg) == 21)                                \
               ? (cacos)(arg)                                   \
               : (__typeid(arg) == 20)                          \
                     ? (cacosf)(arg)                            \
                     : (__typeid(arg) == 10)                    \
                           ? (acosl)(arg)                       \
                           : (__typeid(arg) == 8) ? (acos)(arg) \
                                                  : (__typeid(arg) == 7) ? (acosf)(arg) : (__typeid(arg) < 7) ? (acos)(arg) : 0.0)
#define asin(arg)                                               \
    ((__typeid(arg) == 22)                                      \
         ? (casinl)(arg)                                        \
         : (__typeid(arg) == 21)                                \
               ? (casin)(arg)                                   \
               : (__typeid(arg) == 20)                          \
                     ? (casinf)(arg)                            \
                     : (__typeid(arg) == 10)                    \
                           ? (asinl)(arg)                       \
                           : (__typeid(arg) == 8) ? (asin)(arg) \
                                                  : (__typeid(arg) == 7) ? (asinf)(arg) : (__typeid(arg) < 7) ? (asin)(arg) : 0.0)
#define atan(arg)                                               \
    ((__typeid(arg) == 22)                                      \
         ? (catanl)(arg)                                        \
         : (__typeid(arg) == 21)                                \
               ? (catan)(arg)                                   \
               : (__typeid(arg) == 20)                          \
                     ? (catanf)(arg)                            \
                     : (__typeid(arg) == 10)                    \
                           ? (atanl)(arg)                       \
                           : (__typeid(arg) == 8) ? (atan)(arg) \
                                                  : (__typeid(arg) == 7) ? (atanf)(arg) : (__typeid(arg) < 7) ? (atan)(arg) : 0.0)
#define acosh(arg)                                \
    ((__typeid(arg) == 22)                        \
         ? (cacoshl)(arg)                         \
         : (__typeid(arg) == 21)                  \
               ? (cacosh)(arg)                    \
               : (__typeid(arg) == 20)            \
                     ? (cacoshf)(arg)             \
                     : (__typeid(arg) == 10)      \
                           ? (acoshl)(arg)        \
                           : (__typeid(arg) == 8) \
                                 ? (acosh)(arg)   \
                                 : (__typeid(arg) == 7) ? (acoshf)(arg) : (__typeid(arg) < 7) ? (acosh)(arg) : 0.0)
#define asinh(arg)                                \
    ((__typeid(arg) == 22)                        \
         ? (casinhl)(arg)                         \
         : (__typeid(arg) == 21)                  \
               ? (casinh)(arg)                    \
               : (__typeid(arg) == 20)            \
                     ? (casinhf)(arg)             \
                     : (__typeid(arg) == 10)      \
                           ? (asinhl)(arg)        \
                           : (__typeid(arg) == 8) \
                                 ? (asinh)(arg)   \
                                 : (__typeid(arg) == 7) ? (asinhf)(arg) : (__typeid(arg) < 7) ? (asinh)(arg) : 0.0)
#define atanh(arg)                                \
    ((__typeid(arg) == 22)                        \
         ? (catanhl)(arg)                         \
         : (__typeid(arg) == 21)                  \
               ? (catanh)(arg)                    \
               : (__typeid(arg) == 20)            \
                     ? (catanhf)(arg)             \
                     : (__typeid(arg) == 10)      \
                           ? (atanhl)(arg)        \
                           : (__typeid(arg) == 8) \
                                 ? (atanh)(arg)   \
                                 : (__typeid(arg) == 7) ? (atanhf)(arg) : (__typeid(arg) < 7) ? (atanh)(arg) : 0.0)
#define cos(arg)                                               \
    ((__typeid(arg) == 22)                                     \
         ? (ccosl)(arg)                                        \
         : (__typeid(arg) == 21)                               \
               ? (ccos)(arg)                                   \
               : (__typeid(arg) == 20)                         \
                     ? (ccosf)(arg)                            \
                     : (__typeid(arg) == 10)                   \
                           ? (cosl)(arg)                       \
                           : (__typeid(arg) == 8) ? (cos)(arg) \
                                                  : (__typeid(arg) == 7) ? (cosf)(arg) : (__typeid(arg) < 7) ? (cos)(arg) : 0.0)
#define sin(arg)                                               \
    ((__typeid(arg) == 22)                                     \
         ? (csinl)(arg)                                        \
         : (__typeid(arg) == 21)                               \
               ? (csin)(arg)                                   \
               : (__typeid(arg) == 20)                         \
                     ? (csinf)(arg)                            \
                     : (__typeid(arg) == 10)                   \
                           ? (sinl)(arg)                       \
                           : (__typeid(arg) == 8) ? (sin)(arg) \
                                                  : (__typeid(arg) == 7) ? (sinf)(arg) : (__typeid(arg) < 7) ? (sin)(arg) : 0.0)
#define tan(arg)                                               \
    ((__typeid(arg) == 22)                                     \
         ? (ctanl)(arg)                                        \
         : (__typeid(arg) == 21)                               \
               ? (ctan)(arg)                                   \
               : (__typeid(arg) == 20)                         \
                     ? (ctanf)(arg)                            \
                     : (__typeid(arg) == 10)                   \
                           ? (tanl)(arg)                       \
                           : (__typeid(arg) == 8) ? (tan)(arg) \
                                                  : (__typeid(arg) == 7) ? (tanf)(arg) : (__typeid(arg) < 7) ? (tan)(arg) : 0.0)
#define cosh(arg)                                               \
    ((__typeid(arg) == 22)                                      \
         ? (ccoshl)(arg)                                        \
         : (__typeid(arg) == 21)                                \
               ? (ccosh)(arg)                                   \
               : (__typeid(arg) == 20)                          \
                     ? (ccoshf)(arg)                            \
                     : (__typeid(arg) == 10)                    \
                           ? (coshl)(arg)                       \
                           : (__typeid(arg) == 8) ? (cosh)(arg) \
                                                  : (__typeid(arg) == 7) ? (coshf)(arg) : (__typeid(arg) < 7) ? (cosh)(arg) : 0.0)
#define sinh(arg)                                               \
    ((__typeid(arg) == 22)                                      \
         ? (csinhl)(arg)                                        \
         : (__typeid(arg) == 21)                                \
               ? (csinh)(arg)                                   \
               : (__typeid(arg) == 20)                          \
                     ? (csinhf)(arg)                            \
                     : (__typeid(arg) == 10)                    \
                           ? (sinhl)(arg)                       \
                           : (__typeid(arg) == 8) ? (sinh)(arg) \
                                                  : (__typeid(arg) == 7) ? (sinhf)(arg) : (__typeid(arg) < 7) ? (sinh)(arg) : 0.0)
#define tanh(arg)                                               \
    ((__typeid(arg) == 22)                                      \
         ? (ctanhl)(arg)                                        \
         : (__typeid(arg) == 21)                                \
               ? (ctanh)(arg)                                   \
               : (__typeid(arg) == 20)                          \
                     ? (ctanhf)(arg)                            \
                     : (__typeid(arg) == 10)                    \
                           ? (tanhl)(arg)                       \
                           : (__typeid(arg) == 8) ? (tanh)(arg) \
                                                  : (__typeid(arg) == 7) ? (tanhf)(arg) : (__typeid(arg) < 7) ? (tanh)(arg) : 0.0)
#define exp(arg)                                               \
    ((__typeid(arg) == 22)                                     \
         ? (cexpl)(arg)                                        \
         : (__typeid(arg) == 21)                               \
               ? (cexp)(arg)                                   \
               : (__typeid(arg) == 20)                         \
                     ? (cexpf)(arg)                            \
                     : (__typeid(arg) == 10)                   \
                           ? (expl)(arg)                       \
                           : (__typeid(arg) == 8) ? (exp)(arg) \
                                                  : (__typeid(arg) == 7) ? (expf)(arg) : (__typeid(arg) < 7) ? (exp)(arg) : 0.0)
#define log(arg)                                               \
    ((__typeid(arg) == 22)                                     \
         ? (clogl)(arg)                                        \
         : (__typeid(arg) == 21)                               \
               ? (clog)(arg)                                   \
               : (__typeid(arg) == 20)                         \
                     ? (clogf)(arg)                            \
                     : (__typeid(arg) == 10)                   \
                           ? (logl)(arg)                       \
                           : (__typeid(arg) == 8) ? (log)(arg) \
                                                  : (__typeid(arg) == 7) ? (logf)(arg) : (__typeid(arg) < 7) ? (log)(arg) : 0.0)
#define pow(arg1, arg2) ((__typeid(arg1) == 22 || __typeid(arg2) == 22) ? (cpowl)(arg1,arg2) :\
                    (__typeid(arg1) == 21 || __typeid(arg2) == 21) ? (cpow)(arg1,arg2) :\
                    (__typeid(arg1) == 20 || __typeid(arg2) == 20) ? (cpowf)(arg1,arg2) :\
                    (__typeid(arg1) == 10 || __typeid(arg2) == 10) ? (powl)(arg1,arg2) :\
                    (__typeid(arg1) == 8 || __typeid(arg2) == 8) ? (pow)(arg1,arg2) :\
                    (__typeid(arg1) == 7 || __typeid(arg2) == 7) ? (powf)(arg1,arg2) :\
                    (__typeid(arg1) <7 && __typeid(arg2) < 7 ? (pow)(arg) : 0.0)
#define sqrt(arg)                                               \
    ((__typeid(arg) == 22)                                      \
         ? (csqrtl)(arg)                                        \
         : (__typeid(arg) == 21)                                \
               ? (csqrt)(arg)                                   \
               : (__typeid(arg) == 20)                          \
                     ? (csqrtf)(arg)                            \
                     : (__typeid(arg) == 10)                    \
                           ? (sqrtl)(arg)                       \
                           : (__typeid(arg) == 8) ? (sqrt)(arg) \
                                                  : (__typeid(arg) == 7) ? (sqrtf)(arg) : (__typeid(arg) < 7) ? (sqrt)(arg) : 0.0)

#define fabs(arg)                                               \
    ((__typeid(arg) == 22)                                      \
         ? (cabsl)(arg)                                         \
         : (__typeid(arg) == 21)                                \
               ? (cabs)(arg)                                    \
               : (__typeid(arg) == 20)                          \
                     ? (cabsf)(arg)                             \
                     : (__typeid(arg) == 10)                    \
                           ? (fabsl)(arg)                       \
                           : (__typeid(arg) == 8) ? (fabs)(arg) \
                                                  : (__typeid(arg) == 7) ? (fabsf)(arg) : (__typeid(arg) < 7) ? (fabs)(arg) : 0.0)

#define atan2(arg1, arg2)                                     \
    ((__typeid(arg1) >= 15 || __typeid(arg2) >= 15)           \
         ? 0.0                                                \
         : (__typeid(arg1) == 10 || __typeid(arg2) == 10)     \
               ? (atan2l)(arg1, arg2)                         \
               : (__typeid(arg1) == 8 || __typeid(arg2) == 8) \
                     ? (atan2)(arg1, arg2)                    \
                     : (__typeid(arg1) == 7 || __typeid(arg2) == 7) ? (atan2f)(arg1, arg2) : (atan2)(arg1, arg2))
#define ceil(arg)                                   \
    ((__typeid(arg) >= 15)                          \
         ? 0.0                                      \
         : (__typeid(arg) == 10)                    \
               ? (ceill)(arg)                       \
               : (__typeid(arg) == 8) ? (ceil)(arg) \
                                      : (__typeid(arg) == 7) ? (ceilf)(arg) : (__typeid(arg) < 7) ? (ceil)(arg) : 0.0)
#define cbrt(arg)                                   \
    ((__typeid(arg) >= 15)                          \
         ? 0.0                                      \
         : (__typeid(arg) == 10)                    \
               ? (cbrtl)(arg)                       \
               : (__typeid(arg) == 8) ? (cbrt)(arg) \
                                      : (__typeid(arg) == 7) ? (cbrtf)(arg) : (__typeid(arg) < 7) ? (cbrt)(arg) : 0.0)
#define copysign(arg1, arg2)                                  \
    ((__typeid(arg1) >= 15 || __typeid(arg2) >= 15)           \
         ? 0.0                                                \
         : (__typeid(arg1) == 10 || __typeid(arg2) == 10)     \
               ? (copysignl)(arg1, arg2)                      \
               : (__typeid(arg1) == 8 || __typeid(arg2) == 8) \
                     ? (copysign)(arg1, arg2)                 \
                     : (__typeid(arg1) == 7 || __typeid(arg2) == 7) ? (copysignf)(arg1, arg2) : (copysign)(arg1, arg2))
#define erf(arg)                 \
    ((__typeid(arg) >= 15)       \
         ? 0.0                   \
         : (__typeid(arg) == 10) \
               ? (erfl)(arg)     \
               : (__typeid(arg) == 8) ? (erf)(arg) : (__typeid(arg) == 7) ? (erff)(arg) : (__typeid(arg) < 7) ? (erf)(arg) : 0.0)
#define erfc(arg)                                   \
    ((__typeid(arg) >= 15)                          \
         ? 0.0                                      \
         : (__typeid(arg) == 10)                    \
               ? (erfcl)(arg)                       \
               : (__typeid(arg) == 8) ? (erfc)(arg) \
                                      : (__typeid(arg) == 7) ? (erfcf)(arg) : (__typeid(arg) < 7) ? (erfc)(arg) : 0.0)
#define exp2(arg)                                   \
    ((__typeid(arg) >= 15)                          \
         ? 0.0                                      \
         : (__typeid(arg) == 10)                    \
               ? (exp2l)(arg)                       \
               : (__typeid(arg) == 8) ? (exp2)(arg) \
                                      : (__typeid(arg) == 7) ? (exp2f)(arg) : (__typeid(arg) < 7) ? (exp2)(arg) : 0.0)
#define expm1(arg)                                   \
    ((__typeid(arg) >= 15)                           \
         ? 0.0                                       \
         : (__typeid(arg) == 10)                     \
               ? (expm1l)(arg)                       \
               : (__typeid(arg) == 8) ? (expm1)(arg) \
                                      : (__typeid(arg) == 7) ? (expm1f)(arg) : (__typeid(arg) < 7) ? (expm1)(arg) : 0.0)
#define fdim(arg1, arg2)                                      \
    ((__typeid(arg1) >= 15 || __typeid(arg2) >= 15)           \
         ? 0.0                                                \
         : (__typeid(arg1) == 10 || __typeid(arg2) == 10)     \
               ? (fdiml)(arg1, arg2)                          \
               : (__typeid(arg1) == 8 || __typeid(arg2) == 8) \
                     ? (fdim)(arg1, arg2)                     \
                     : (__typeid(arg1) == 7 || __typeid(arg2) == 7) ? (fdimf)(arg1, arg2) : (fdim)(arg1, arg2))
#define floor(arg)                                   \
    ((__typeid(arg) >= 15)                           \
         ? 0.0                                       \
         : (__typeid(arg) == 10)                     \
               ? (floorl)(arg)                       \
               : (__typeid(arg) == 8) ? (floor)(arg) \
                                      : (__typeid(arg) == 7) ? (floorf)(arg) : (__typeid(arg) < 7) ? (floor)(arg) : 0.0)
#define fma(arg1, arg2, arg3)                                                                                       \
    ((__typeid(arg1) >= 15 || __typeid(arg2) >= 15 || __typeid(arg3) >= 15)                                         \
         ? 0.0                                                                                                      \
         : (__typeid(arg1) == 10 || __typeid(arg2) == 10 || __typeid(arg3) == 10)                                   \
               ? fmal(arg1, arg2, arg3)                                                                             \
               : (__typeid(arg1) == 8 || __typeid(arg2) == 8 || __typeid(arg3) == 8)                                \
                     ? fma(arg1, arg2, arg3)                                                                        \
                     : (__typeid(arg1) == 7 || __typeid(arg2) == 7 || __typeid(arg3) == 7) ? fmaf(arg1, arg2, arg3) \
                                                                                           : fma(arg1, arg2, arg3))
#define fmax(arg1, arg2)                                      \
    ((__typeid(arg1) >= 15 || __typeid(arg2) >= 15)           \
         ? 0.0                                                \
         : (__typeid(arg1) == 10 || __typeid(arg2) == 10)     \
               ? (fmaxl)(arg1, arg2)                          \
               : (__typeid(arg1) == 8 || __typeid(arg2) == 8) \
                     ? (fmax)(arg1, arg2)                     \
                     : (__typeid(arg1) == 7 || __typeid(arg2) == 7) ? (fmaxf)(arg1, arg2) : (fmax)(arg1, arg2))
#define fmin(arg1, arg2)                                      \
    ((__typeid(arg1) >= 15 || __typeid(arg2) >= 15)           \
         ? 0.0                                                \
         : (__typeid(arg1) == 10 || __typeid(arg2) == 10)     \
               ? (fminl)(arg1, arg2)                          \
               : (__typeid(arg1) == 8 || __typeid(arg2) == 8) \
                     ? (fmin)(arg1, arg2)                     \
                     : (__typeid(arg1) == 7 || __typeid(arg2) == 7) ? (fminf)(arg1, arg2) : (fmin)(arg1, arg2))
#define fmod(arg1, arg2)                                      \
    ((__typeid(arg1) >= 15 || __typeid(arg2) >= 15)           \
         ? 0.0                                                \
         : (__typeid(arg1) == 10 || __typeid(arg2) == 10)     \
               ? (fmodl)(arg1, arg2)                          \
               : (__typeid(arg1) == 8 || __typeid(arg2) == 8) \
                     ? (fmod)(arg1, arg2)                     \
                     : (__typeid(arg1) == 7 || __typeid(arg2) == 7) ? (fmodf)(arg1, arg2) : (fmod)(arg1, arg2))
#define frexp(arg, arg1)                                                                             \
    ((__typeid(arg) >= 15)                                                                           \
         ? 0.0                                                                                       \
         : (__typeid(arg) == 10) ? (frexpl)(arg, arg1)                                               \
                                 : (__typeid(arg) == 8) ? (frexp)(arg, arg1)                         \
                                                        : (__typeid(arg) == 7) ? (frexpf)(arg, arg1) \
                                                                               : (__typeid(arg) < 7) ? (frexp)(arg, arg1) : 0.0)
#define hypot(arg1, arg2)                                     \
    ((__typeid(arg1) >= 15 || __typeid(arg2) >= 15)           \
         ? 0.0                                                \
         : (__typeid(arg1) == 10 || __typeid(arg2) == 10)     \
               ? (hypotl)(arg1, arg2)                         \
               : (__typeid(arg1) == 8 || __typeid(arg2) == 8) \
                     ? (hypot)(arg1, arg2)                    \
                     : (__typeid(arg1) == 7 || __typeid(arg2) == 7) ? (hypotf)(arg1, arg2) : (hypot)(arg1, arg2))
#define ilogb(arg)                                   \
    ((__typeid(arg) >= 15)                           \
         ? 0.0                                       \
         : (__typeid(arg) == 10)                     \
               ? (ilogbl)(arg)                       \
               : (__typeid(arg) == 8) ? (ilogb)(arg) \
                                      : (__typeid(arg) == 7) ? (ilogbf)(arg) : (__typeid(arg) < 7) ? (ilogb)(arg) : 0.0)
#define ldexp(arg, arg1)                                                                             \
    ((__typeid(arg) >= 15)                                                                           \
         ? 0.0                                                                                       \
         : (__typeid(arg) == 10) ? (ldexpl)(arg, arg1)                                               \
                                 : (__typeid(arg) == 8) ? (ldexp)(arg, arg1)                         \
                                                        : (__typeid(arg) == 7) ? (ldexpf)(arg, arg1) \
                                                                               : (__typeid(arg) < 7) ? (ldexp)(arg, arg1) : 0.0)
#define lgamma(arg)                                   \
    ((__typeid(arg) >= 15)                            \
         ? 0.0                                        \
         : (__typeid(arg) == 10)                      \
               ? (lgammal)(arg)                       \
               : (__typeid(arg) == 8) ? (lgamma)(arg) \
                                      : (__typeid(arg) == 7) ? (lgammaf)(arg) : (__typeid(arg) < 7) ? (lgamma)(arg) : 0.0)
#define llrint(arg)                                   \
    ((__typeid(arg) >= 15)                            \
         ? 0.0                                        \
         : (__typeid(arg) == 10)                      \
               ? (llrintl)(arg)                       \
               : (__typeid(arg) == 8) ? (llrint)(arg) \
                                      : (__typeid(arg) == 7) ? (llrintf)(arg) : (__typeid(arg) < 7) ? (llrint)(arg) : 0.0)
#define llround(arg)                                   \
    ((__typeid(arg) >= 15)                             \
         ? 0.0                                         \
         : (__typeid(arg) == 10)                       \
               ? (llroundl)(arg)                       \
               : (__typeid(arg) == 8) ? (llround)(arg) \
                                      : (__typeid(arg) == 7) ? (llroundf)(arg) : (__typeid(arg) < 7) ? (llround)(arg) : 0.0)
#define log10(arg)                                   \
    ((__typeid(arg) >= 15)                           \
         ? 0.0                                       \
         : (__typeid(arg) == 10)                     \
               ? (log10l)(arg)                       \
               : (__typeid(arg) == 8) ? (log10)(arg) \
                                      : (__typeid(arg) == 7) ? (log10f)(arg) : (__typeid(arg) < 7) ? (log10)(arg) : 0.0)
#define log1p(arg)                                   \
    ((__typeid(arg) >= 15)                           \
         ? 0.0                                       \
         : (__typeid(arg) == 10)                     \
               ? (log1pl)(arg)                       \
               : (__typeid(arg) == 8) ? (log1p)(arg) \
                                      : (__typeid(arg) == 7) ? (log1pf)(arg) : (__typeid(arg) < 7) ? (log1p)(arg) : 0.0)
#define log2(arg)                                   \
    ((__typeid(arg) >= 15)                          \
         ? 0.0                                      \
         : (__typeid(arg) == 10)                    \
               ? (log2l)(arg)                       \
               : (__typeid(arg) == 8) ? (log2)(arg) \
                                      : (__typeid(arg) == 7) ? (log2f)(arg) : (__typeid(arg) < 7) ? (log2)(arg) : 0.0)
#define logb(arg)                                   \
    ((__typeid(arg) >= 15)                          \
         ? 0.0                                      \
         : (__typeid(arg) == 10)                    \
               ? (logbl)(arg)                       \
               : (__typeid(arg) == 8) ? (logb)(arg) \
                                      : (__typeid(arg) == 7) ? (logbf)(arg) : (__typeid(arg) < 7) ? (logb)(arg) : 0.0)
#define lrint(arg)                                   \
    ((__typeid(arg) >= 15)                           \
         ? 0.0                                       \
         : (__typeid(arg) == 10)                     \
               ? (lrintl)(arg)                       \
               : (__typeid(arg) == 8) ? (lrint)(arg) \
                                      : (__typeid(arg) == 7) ? (lrintf)(arg) : (__typeid(arg) < 7) ? (lrint)(arg) : 0.0)
#define lround(arg)                                   \
    ((__typeid(arg) >= 15)                            \
         ? 0.0                                        \
         : (__typeid(arg) == 10)                      \
               ? (lroundl)(arg)                       \
               : (__typeid(arg) == 8) ? (lround)(arg) \
                                      : (__typeid(arg) == 7) ? (lroundf)(arg) : (__typeid(arg) < 7) ? (lround)(arg) : 0.0)
#define nearbyint(arg)                                   \
    ((__typeid(arg) >= 15)                               \
         ? 0.0                                           \
         : (__typeid(arg) == 10)                         \
               ? (nearbyintl)(arg)                       \
               : (__typeid(arg) == 8) ? (nearbyint)(arg) \
                                      : (__typeid(arg) == 7) ? (nearbyintf)(arg) : (__typeid(arg) < 7) ? (nearbyint)(arg) : 0.0)
#define nextafter(arg)                                   \
    ((__typeid(arg) >= 15)                               \
         ? 0.0                                           \
         : (__typeid(arg) == 10)                         \
               ? (nextafterl)(arg)                       \
               : (__typeid(arg) == 8) ? (nextafter)(arg) \
                                      : (__typeid(arg) == 7) ? (nextafterf)(arg) : (__typeid(arg) < 7) ? (nextafter)(arg) : 0.0)
#define nexttoward(arg, arg1)                  \
    ((__typeid(arg) >= 15)                     \
         ? 0.0                                 \
         : (__typeid(arg) == 10)               \
               ? (nexttowardl)(arg, arg1)      \
               : (__typeid(arg) == 8)          \
                     ? (nexttoward)(arg, arg1) \
                     : (__typeid(arg) == 7) ? (nexttowardf)(arg, arg1) : (__typeid(arg) < 7) ? (nexttoward)(arg, arg1) : 0.0)
#define remainder(arg1, arg2)                                 \
    ((__typeid(arg1) >= 15 || __typeid(arg2) >= 15)           \
         ? 0.0                                                \
         : (__typeid(arg1) == 10 || __typeid(arg2) == 10)     \
               ? (remainderl)(arg1, arg2)                     \
               : (__typeid(arg1) == 8 || __typeid(arg2) == 8) \
                     ? (remainder)(arg1, arg2)                \
                     : (__typeid(arg1) == 7 || __typeid(arg2) == 7) ? (remainderf)(arg1, arg2) : (remainder)(arg1, arg2))
#define remquo(arg1, arg2)                                    \
    ((__typeid(arg1) >= 15 || __typeid(arg2) >= 15)           \
         ? 0.0                                                \
         : (__typeid(arg1) == 10 || __typeid(arg2) == 10)     \
               ? (remquol)(arg1, arg2)                        \
               : (__typeid(arg1) == 8 || __typeid(arg2) == 8) \
                     ? (remquo)(arg1, arg2)                   \
                     : (__typeid(arg1) == 7 || __typeid(arg2) == 7) ? (remquof)(arg1, arg2) : (remquo)(arg1, arg2))
#define rint(arg)                                   \
    ((__typeid(arg) >= 15)                          \
         ? 0.0                                      \
         : (__typeid(arg) == 10)                    \
               ? (rintl)(arg)                       \
               : (__typeid(arg) == 8) ? (rint)(arg) \
                                      : (__typeid(arg) == 7) ? (rintf)(arg) : (__typeid(arg) < 7) ? (rint)(arg) : 0.0)
#define round(arg)                                   \
    ((__typeid(arg) >= 15)                           \
         ? 0.0                                       \
         : (__typeid(arg) == 10)                     \
               ? (roundl)(arg)                       \
               : (__typeid(arg) == 8) ? (round)(arg) \
                                      : (__typeid(arg) == 7) ? (roundf)(arg) : (__typeid(arg) < 7) ? (round)(arg) : 0.0)
#define scalbn(arg, arg1)                                                                             \
    ((__typeid(arg) >= 15)                                                                            \
         ? 0.0                                                                                        \
         : (__typeid(arg) == 10) ? (scalbnl)(arg, arg1)                                               \
                                 : (__typeid(arg) == 8) ? (scalbn)(arg, arg1)                         \
                                                        : (__typeid(arg) == 7) ? (scalbnf)(arg, arg1) \
                                                                               : (__typeid(arg) < 7) ? (scalbn)(arg, arg1) : 0.0)
#define scalbln(arg, arg1)                                                                             \
    ((__typeid(arg) >= 15)                                                                             \
         ? 0.0                                                                                         \
         : (__typeid(arg) == 10) ? (scalblnl)(arg, arg1)                                               \
                                 : (__typeid(arg) == 8) ? (scalbln)(arg, arg1)                         \
                                                        : (__typeid(arg) == 7) ? (scalblnf)(arg, arg1) \
                                                                               : (__typeid(arg) < 7) ? (scalbln)(arg, arg1) : 0.0)
#define tgamma(arg)                                   \
    ((__typeid(arg) >= 15)                            \
         ? 0.0                                        \
         : (__typeid(arg) == 10)                      \
               ? (tgammal)(arg)                       \
               : (__typeid(arg) == 8) ? (tgamma)(arg) \
                                      : (__typeid(arg) == 7) ? (tgammaf)(arg) : (__typeid(arg) < 7) ? (tgamma)(arg) : 0.0)
#define trunc(arg)                                   \
    ((__typeid(arg) >= 15)                           \
         ? 0.0                                       \
         : (__typeid(arg) == 10)                     \
               ? (truncl)(arg)                       \
               : (__typeid(arg) == 8) ? (trunc)(arg) \
                                      : (__typeid(arg) == 7) ? (truncf)(arg) : (__typeid(arg) < 7) ? (trunc)(arg) : 0.0)

#define carg(arg) \
    ((__typeid(arg) == 22) ? (cargl)(arg) : (__typeid(arg) == 21) ? (carg)(arg) : (__typeid(arg) == 20) ? (cargf)(arg) : arg)
#define cimag(arg) \
    ((__typeid(arg) == 22) ? (cimagl)(arg) : (__typeid(arg) == 21) ? (cimag)(arg) : (__typeid(arg) == 20) ? (cimagf)(arg) : 0.0)
#define conj(arg) \
    ((__typeid(arg) == 22) ? (conjl)(arg) : (__typeid(arg) == 21) ? (conj)(arg) : (__typeid(arg) == 20) ? (conjf)(arg) : arg)
#define cproj(arg) \
    ((__typeid(arg) == 22) ? (cprojl)(arg) : (__typeid(arg) == 21) ? (cproj)(arg) : (__typeid(arg) == 20) ? (cprojf)(arg) : arg)
#define creal(arg) \
    ((__typeid(arg) == 22) ? (creall)(arg) : (__typeid(arg) == 21) ? (creal)(arg) : (__typeid(arg) == 20) ? (crealf)(arg) : arg)

#endif
