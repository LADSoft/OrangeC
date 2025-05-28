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

#ifndef __STDLIB_H
#define __STDLIB_H

#pragma pack(1)

#    include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef RC_INVOKED
#ifndef _DIV_T
#    define _DIV_T
    typedef struct
    {
        int quot;
        int rem;
    } div_t;
#endif

#ifndef _LDIV_T
#    define _LDIV_T
    typedef struct
    {
        long quot;
        long rem;
    } ldiv_t;
#endif

#if defined(__cplusplus) || __STDC_VERSION__ >= 199901L
#    ifndef _LLDIV_T
#        define _LLDIV_T
    typedef struct
    {
        long long quot;
        long long rem;
    } lldiv_t;
#    endif
#endif
#endif
#undef _alloca
#undef alloca
#define _alloca(x) __alloca((x))
#define alloca(x) __alloca((x))

#define MB_CUR_MAX 3

    /* Maximum value returned by "rand" function
     */

#define RAND_MAX 0x7FFFU

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#ifndef RC_INVOKED
    typedef void (*atexit_t)(void);
    typedef void* _locale_t;

    void _RTL_FUNC _IMPORT abort(void);
    void _RTL_FUNC _IMPORT _abort(void);

    int _RTL_INTRINS _IMPORT abs(int __x);
    long _RTL_INTRINS _IMPORT labs(long __x);
#if __STDC_VERSION__ >= 199901L || defined(__cplusplus)
    long long _RTL_FUNC _IMPORT llabs(long long __x);

#endif /* __ABS_DEFINED */

    int _RTL_FUNC _IMPORT atexit(void (*__func)(void));
    int _RTL_FUNC _IMPORT at_quick_exit(void (*__func)(void));
    double _RTL_FUNC _IMPORT atof(const char* ZSTR __s);
    int _RTL_FUNC _IMPORT atoi(const char* ZSTR __s);
    long _RTL_FUNC _IMPORT atol(const char* ZSTR __s);
    __int64 _RTL_FUNC _IMPORT _atoi64(const char* ZSTR __s);
#if __STDC_VERSION__ >= 199901L || defined(__cplusplus)
    long long _RTL_FUNC _IMPORT atoll(const char* ZSTR __s);
#endif

#if defined(__cplusplus) || __STDC_VERSION__ >= 202311
    const void* _RTL_FUNC _IMPORT bsearch(const void* __key, const void* __base, size_t __nelem, size_t __width,
                                    int (*fcmp)(const void*, const void*));
#else
    void* _RTL_FUNC _IMPORT bsearch(const void* __key, const void* __base, size_t __nelem, size_t __width,
                                    int (*fcmp)(const void*, const void*));
#endif
    void* _RTL_FUNC _IMPORT _MSIL_RTL calloc(size_t __nitems, size_t __size);
    div_t _RTL_INTRINS _IMPORT div(int __numer, int __denom);
    ldiv_t _RTL_INTRINS _IMPORT ldiv(long __numer, long __denom);
#if __STDC_VERSION__ >= 199901L || defined(__cplusplus)
    lldiv_t _RTL_FUNC _IMPORT lldiv(long long __numer, long long __denom);
#endif
    void _RTL_FUNC _IMPORT _NORETURN quick_exit(int __status);
    void _RTL_FUNC _IMPORT _NORETURN exit(int __status);
    void _RTL_FUNC _IMPORT _NORETURN _Exit(int __status);
    void _RTL_FUNC _IMPORT _MSIL_RTL free(void* __block);
    void _RTL_FUNC _IMPORT _aligned_free(void* __block);
    char* ZSTR _RTL_FUNC _IMPORT getenv(const char* ZSTR __name);
    wchar_t* ZSTR _RTL_FUNC _IMPORT _wgetenv(const wchar_t* ZSTR __name);
    void* _RTL_FUNC _IMPORT lfind(const void* __key, const void* __base, size_t* __num, size_t __width,
                                  int (*fcmp)(const void*, const void*));
    void* _RTL_FUNC _IMPORT lsearch(const void* __key, void* __base, size_t* __num, size_t __width,
                                    int (*fcmp)(const void*, const void*));
    void* _RTL_FUNC _IMPORT _lfind(const void* __key, const void* __base, size_t* __num, size_t __width,
                                   int (*fcmp)(const void*, const void*));
    void* _RTL_FUNC _IMPORT _lsearch(const void* __key, void* __base, size_t* __num, size_t __width,
                                     int (*fcmp)(const void*, const void*));
    void* _RTL_FUNC _IMPORT aligned_alloc(size_t __align, size_t __size);
    void* _RTL_FUNC _IMPORT _aligned_malloc(size_t __align, size_t __size);
    void* _RTL_FUNC _IMPORT _MSIL_RTL malloc(size_t __size);
    int _RTL_FUNC _IMPORT mblen(const char* ZSTR __s, size_t __n);
    int _RTL_FUNC _IMPORT _heap_validate(void *);
    size_t _RTL_FUNC _IMPORT _msize(void *);
    void * _RTL_FUNC _IMPORT _mm_malloc(size_t, size_t);
    void _RTL_FUNC _IMPORT _mm_free(void *);
    size_t _RTL_FUNC _IMPORT mbstowcs(wchar_t* restrict __pwcs, const char* ZSTR restrict __s, size_t __n);
    int _RTL_FUNC _IMPORT mbtowc(wchar_t* restrict __pwc, const char* ZSTR restrict __s, size_t __n);
    void _RTL_FUNC _IMPORT qsort(void* __base, size_t __nelem, size_t __width, int (*__fcmp)(const void*, const void*));
    int _RTL_FUNC _IMPORT rand(void);
    int _RTL_FUNC _IMPORT rand_s(unsigned int* __rand_value);
    void* _RTL_FUNC _IMPORT _MSIL_RTL realloc(void* __block, size_t __size);
    void _RTL_FUNC _IMPORT srand(unsigned __seed);
    char* ZSTR _RTL_FUNC _IMPORT strdup(const char* ZSTR string);
    char* ZSTR _RTL_FUNC _IMPORT _strdup(const char* ZSTR string);
    double _RTL_FUNC _IMPORT strtod(const char* ZSTR restrict __s, char* ZSTR* restrict __endptr);
#if __STDC_VERSION__ >= 199901L || defined(__cplusplus)
    float _RTL_FUNC _IMPORT strtof(const char* ZSTR restrict __s, char* ZSTR* restrict __endptr);
#endif
    long _RTL_FUNC _IMPORT strtol(const char* ZSTR __s, char* ZSTR* restrict __endptr, int __radix);
#if __STDC_VERSION__ >= 199901L || defined(__cplusplus)
    long long _RTL_FUNC _IMPORT strtoll(const char* ZSTR restrict __s, char* ZSTR* restrict __endptr, int __radix);
#endif
    long double _RTL_FUNC _IMPORT _strtold(const char* ZSTR restrict __s, char* ZSTR* restrict __endptr);
#if __STDC_VERSION__ >= 199901L || defined(__cplusplus)
    long double _RTL_FUNC _IMPORT strtold(const char* ZSTR restrict __s, char* ZSTR* restrict __endptr);
#endif
    unsigned long _RTL_FUNC _IMPORT strtoul(const char* ZSTR __s, char* ZSTR* __endptr, int __radix);
#if __STDC_VERSION__ >= 199901L || defined(__cplusplus)
    unsigned long long _RTL_FUNC _IMPORT strtoull(const char* ZSTR restrict __s, char* ZSTR* restrict __endptr, int __radix);
#endif
    long long _RTL_FUNC _IMPORT _strtoi64(const char* ZSTR restrict __s, char* ZSTR* restrict __endptr, int __radix);
    int _RTL_FUNC _IMPORT system(const char* ZSTR __command);
    size_t _RTL_FUNC _IMPORT wcstombs(char* ZSTR restrict __s, const wchar_t* restrict __pwcs, size_t __n);
    int _RTL_FUNC _IMPORT wctomb(char* ZSTR restrict __s, wchar_t __wc);
    int _RTL_FUNC _IMPORT wctombflush(char* ZSTR __s);

    size_t _RTL_FUNC memalignment(const void *);

#ifndef STDC
#    define environ _environ
#endif

#if defined(__LSCRTL_DLL)
    extern char _IMPORT** _environ;
#    ifndef _FMODE_EXT
#        define _FMODE_EXT
    extern int _IMPORT _fmode;
#    endif
    extern unsigned char _IMPORT _osmajor;
    extern unsigned char _IMPORT _osminor;
#else
extern char _RTL_DATA** _environ;
#    ifndef _FMODE_EXT
#        define _FMODE_EXT
extern int _RTL_DATA _fmode;
#    endif
extern unsigned char _RTL_DATA _osmajor;
extern unsigned char _RTL_DATA _osminor;
#endif
#endif

    /* Constants for MSC pathname functions */

#ifndef _WIN32
#    define _MAX_PATH 80
#    define _MAX_DRIVE 3
#    define _MAX_DIR 66
#    define _MAX_FNAME 9
#    define _MAX_EXT 5
#else
#    define _MAX_PATH 260
#    define _MAX_DRIVE 3
#    define _MAX_DIR 256
#    define _MAX_FNAME 256
#    define _MAX_EXT 256
#endif
#define _MAX_PATH2 (_MAX_PATH + 4)

#ifndef RC_INVOKED
    long double _RTL_FUNC _IMPORT _atold(const char* ZSTR __s);
    void _RTL_FUNC _IMPORT _NORETURN _exit(int __status);
    char* ZSTR _RTL_FUNC _IMPORT _fullpath(char* ZSTR, const char* ZSTR, size_t);
    wchar_t* ZSTR _RTL_FUNC _IMPORT _wfullpath(wchar_t* ZSTR, const wchar_t* ZSTR, size_t);
    char* ZSTR _RTL_FUNC _IMPORTMM itoa(int __value, char* ZSTR __stringValue, int __radix);
    char* ZSTR _RTL_FUNC _IMPORTMM _itoa(int __value, char* ZSTR __stringValue, int __radix);
    char* ZSTR _RTL_FUNC _IMPORTMM _i64toa(__int64 __value, char* ZSTR __stringValue, int __radix);
    char* ZSTR _RTL_FUNC _IMPORTMM ltoa(long __value, char* ZSTR __stringValue, int __radix);
    char* ZSTR _RTL_FUNC _IMPORTMM _ltoa(long __value, char* ZSTR __stringValue, int __radix);
#if __STDC_VERSION__ >= 199901L || defined(__cplusplus)
    char* ZSTR _RTL_FUNC _IMPORT lltoa(long long __value, char* ZSTR __stringValue, int __radix);
    char* ZSTR _RTL_FUNC _IMPORT _lltoa(long long __value, char* ZSTR __stringValue, int __radix);
#endif
    void _RTL_FUNC _IMPORT _makepath(char* ZSTR __path, const char* ZSTR __drive, const char* ZSTR __dir, const char* ZSTR __name,
                                     const char* ZSTR __ext);
    void _RTL_FUNC _IMPORT _wmakepath(wchar_t* ZSTR __path, const wchar_t* ZSTR __drive, const wchar_t* ZSTR __dir, const wchar_t* ZSTR __name,
                                     const wchar_t* ZSTR __ext);
    int _RTL_FUNC _IMPORT putenv(const char* ZSTR __name);
    int _RTL_FUNC _IMPORT _wputenv(const wchar_t* ZSTR __name);
    int _RTL_FUNC _IMPORT _putenv(const char* ZSTR __name);
    int _RTL_FUNC _IMPORT _putenv_s(const char* ZSTR __name, const char* ZSTR value);
    int _RTL_FUNC _IMPORT setenv(const char* ZSTR name, const char* ZSTR value, int overwrite);
    int _RTL_FUNC _IMPORT _wsetenv(const wchar_t* ZSTR name, const wchar_t* ZSTR value, int overwrite);
    int _RTL_FUNC _IMPORT unsetenv(const char* ZSTR name);
    int _RTL_FUNC _IMPORT _wunsetenv(const wchar_t* ZSTR name);

    void _RTL_FUNC _IMPORT _searchenv(const char* ZSTR __file, const char* ZSTR __varname, char* ZSTR __pathname);
    void _RTL_FUNC _IMPORT _searchstr(const char* ZSTR __file, const char* ZSTR __ipath, char* ZSTR __pathname);
    void _RTL_FUNC _IMPORT _splitpath(const char* ZSTR __path, char* ZSTR __drive, char* ZSTR __dir, char* ZSTR __name,
                                      char* ZSTR __ext);
    void _RTL_FUNC _IMPORT _wsplitpath(const wchar_t* ZSTR __path, wchar_t* ZSTR __drive, wchar_t* ZSTR __dir, wchar_t* ZSTR __name,
                                      wchar_t* ZSTR __ext);
    void _RTL_FUNC _IMPORT _splitpath2(const char* ZSTR __path, char* ZSTR __outpath, char* ZSTR* __drive, char* ZSTR* __dir,
                                       char* ZSTR* __name, char* ZSTR* __ext);
    void _RTL_FUNC _IMPORT _wsplitpath2(const wchar_t* ZSTR __path, wchar_t* ZSTR __outpath, wchar_t* ZSTR* __drive, wchar_t* ZSTR* __dir,
                                       wchar_t* ZSTR* __name, wchar_t* ZSTR* __ext);
    void _RTL_FUNC _IMPORT swab(char* ZSTR __from, char* ZSTR __to, int __nbytes);
    void _RTL_FUNC _IMPORT _swab(char* ZSTR, char* ZSTR, int);
    char* ZSTR _RTL_FUNC _IMPORT ultoa(unsigned long __value, char* ZSTR __stringValue, int __radix);
    char* ZSTR _RTL_FUNC _IMPORT _ultoa(unsigned long __value, char* ZSTR __stringValue, int __radix);
    char* ZSTR _RTL_FUNC _IMPORT _ui64toa(unsigned __int64 __value, char* ZSTR __stringValue, int __radix);
    char* ZSTR _RTL_FUNC _IMPORT utoa(unsigned __value, char* ZSTR __stringValue, int __radix);
    char* ZSTR _RTL_FUNC _IMPORT _utoa(unsigned __value, char* ZSTR __stringValue, int __radix);
#if __STDC_VERSION__ >= 199901L || defined(__cplusplus)
    char* ZSTR _RTL_FUNC _IMPORT ulltoa(unsigned __int64 __value, char* ZSTR __stringValue, int __radix);
    char* ZSTR _RTL_FUNC _IMPORT _ulltoa(unsigned __int64 __value, char* ZSTR __stringValue, int __radix);
#endif
    char* ZSTR _RTL_FUNC _IMPORT _ecvt(double, int, int*, int*);
    char* ZSTR _RTL_FUNC _IMPORT _fcvt(double, int, int*, int*);
    char* ZSTR _RTL_FUNC _IMPORT _gcvt(double, int, char* ZSTR);
    char* ZSTR _RTL_FUNC _IMPORT ecvt(double, int, int*, int*);
    char* ZSTR _RTL_FUNC _IMPORT fcvt(double, int, int*, int*);
    char* ZSTR _RTL_FUNC _IMPORT gcvt(double, int, char* ZSTR);

    typedef int(__cdecl* _onexit_t)(void);
#define onexit_t _onexit_t

    _onexit_t _RTL_FUNC _IMPORT _onexit(_onexit_t);
    onexit_t _RTL_FUNC _IMPORT onexit(onexit_t);

    void _RTL_FUNC _IMPORT _beep(unsigned, unsigned);
    void _RTL_FUNC _IMPORT _sleep(unsigned long);

    unsigned char _RTL_FUNC _IMPORT _crotl(unsigned char __value, int __count);
    unsigned char _RTL_FUNC _IMPORT _crotr(unsigned char __value, int __count);
    unsigned int _RTL_FUNC _IMPORT _rotl(unsigned int, int);
    unsigned int _RTL_FUNC _IMPORT _rotr(unsigned int, int);
    unsigned long _RTL_FUNC _IMPORT _lrotl(unsigned long, int);
    unsigned long _RTL_FUNC _IMPORT _lrotr(unsigned long, int);
    unsigned long long _RTL_FUNC _IMPORT _llrotl(unsigned long long, int);
    unsigned long long _RTL_FUNC _IMPORT _llrotr(unsigned long long, int);

    unsigned int _RTL_FUNC _IMPORT _bswap(unsigned int);
    unsigned short _RTL_FUNC _IMPORT _bswap16(unsigned short);
    unsigned long long _RTL_FUNC _IMPORT _bswap64(unsigned long long);

    const char* ZSTR _RTL_FUNC getexecname(void);
    char* ZSTR _RTL_FUNC realpath(const char* ZSTR path, char* ZSTR resolved_path);
    char* ZSTR _RTL_FUNC canonicalize_file_name(const char* ZSTR path);

#if defined(__MSIL__)
    int* _RTL_FUNC _IMPORT _errno(void);
#else
int* _RTL_FUNC _IMPORT __GetErrno(void);
#endif
    int* _RTL_FUNC _IMPORT __GetDosErrno(void);
#endif

#ifndef __cplusplus
#    if defined(__MSIL__)
#        define errno (*_errno())
#    else
#        define errno (*__GetErrno())
#    endif
#    define _dos_errno (*__GetDosErrno())
#endif
#define sys_nerr _sys_nerr
#ifndef RC_INVOKED
#if defined(__LSCRTL_DLL)
    extern int _IMPORT _sys_nerr;
#else
extern int _RTL_DATA _sys_nerr;
#endif

#define sys_errlist _sys_errlist
#if defined(__LSCRTL_DLL)
    extern char _IMPORT* _sys_errlist[];
#else
extern char _RTL_DATA* _sys_errlist[];
#endif

#if defined(__LSCRTL_DLL)
    extern int _IMPORT __argc;
    extern char _IMPORT** __argv;
#else
extern int _RTL_DATA __argc;
extern char _RTL_DATA** __argv;
#endif

    void _RTL_FUNC _IMPORT perror(const char* ZSTR __s);
#endif
#if defined(__cplusplus)

#ifndef RC_INVOKED
#ifndef _TIME_T
#    define _TIME_T
typedef long __time_t_32;
typedef long long __time_t_64;

#if defined(__MSVCRT_DLL) || defined(__CRTDLL_DLL) || defined (_USE_32BIT_TIME_T) || defined(__MSIL__)
    typedef __time_t_32 time_t;
#else
    typedef __time_t_64 time_t;
#endif
#endif

    time_t _RTL_FUNC _IMPORT time(time_t*);

    inline void _RTL_FUNC _IMPORT randomize(void) { srand((unsigned)time(NULL)); }
    inline int _RTL_FUNC _IMPORT random(int num) { return ((rand() * (num)) / (RAND_MAX + 1)); }
#endif

/* must include the C++ header to get min and max defs */
#else /* __cplusplus */

#    define random(num) ((rand() * (num)) / (RAND_MAX + 1))

#    define randomize() srand((unsigned)time(NULL))

#    ifndef max
#        define max(a, b) (((a) > (b)) ? (a) : (b))
#    endif
#    ifndef min
#        define min(a, b) (((a) < (b)) ? (a) : (b))
#    endif

#endif


#if 0
extern _CRTIMP int __cdecl _heap_validate(void *);
extern _CRTIMP size_t __cdecl _msize(void *);
#endif

#ifdef __cplusplus
    long double _RTL_FUNC _IMPORTMM fabsl(long double __x);
    long _RTL_CONSTEXPR __builtin_labs(long __x) { return labs(__x); }
    long long _RTL_CONSTEXPR __builtin_llabs(long long __x) { return llabs(__x); }
#ifndef _FABS_
#define _FABS_
    float _RTL_FUNC _IMPORTMM fabsf(float __x);
    double _RTL_FUNC _IMPORTMM fabs(double __x);
    inline float _RTL_CONSTEXPR __builtin_fabsf(float __x) { return fabsf( __x); }
    inline double _RTL_CONSTEXPR __builtin_fabs(double __x) { return fabs( __x); }
    inline long double _RTL_CONSTEXPR __builtin_fabsl(long double __x) { return fabsl( __x); }
#endif
#endif
#ifdef __cplusplus
};
#endif

#pragma pack()

#endif /* __STDLIB_H */