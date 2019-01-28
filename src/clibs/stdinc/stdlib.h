/* Software License Agreement
 * 
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     As a special exception, if other files instantiate templates or
 *     use macros or inline functions from this file, or you compile
 *     this file and link it with other works to produce a work based
 *     on this file, this file does not by itself cause the resulting
 *     work to be covered by the GNU General Public License. However
 *     the source code for this file must still be made available in
 *     accordance with section (3) of the GNU General Public License.
 *     
 *     This exception does not invalidate any other reasons why a work
 *     based on this file might be covered by the GNU General Public
 *     License.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#ifndef __STDLIB_H
#define __STDLIB_H

#pragma pack(1)

#ifndef __STDDEF_H
#    include <stddef.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

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

#define MB_CUR_MAX 3

    /* Maximum value returned by "rand" function
     */

#define RAND_MAX 0x7FFFU

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

    typedef void (*atexit_t)(void);

    void _RTL_FUNC _IMPORT abort(void);
    void _RTL_FUNC _IMPORT _abort(void);

    int _RTL_INTRINS _IMPORT abs(int __x);
    long _RTL_INTRINS _IMPORT labs(long __x);
#if __STDC_VERSION__ >= 199901L || defined(__cplusplus)
    long long _RTL_FUNC _IMPORT llabs(long long __x);

#endif /* __ABS_DEFINED */

    int _RTL_FUNC _IMPORT atexit(void (*__func)(void));
    int _RTL_FUNC _IMPORT at_quick_exit(void (*__func)(void));
    double _RTL_FUNC _IMPORT atof(const char* __s);
    int _RTL_FUNC _IMPORT atoi(const char* __s);
    long _RTL_FUNC _IMPORT atol(const char* __s);
    __int64 _RTL_FUNC _IMPORT _atoi64(const char* __s);
#if __STDC_VERSION__ >= 199901L || defined(__cplusplus)
    long long _RTL_FUNC _IMPORT atoll(const char* __s);
#endif

    void* _RTL_FUNC _IMPORT bsearch(const void* __key, const void* __base, size_t __nelem, size_t __width,
                                    int (*fcmp)(const void*, const void*));
    void* _RTL_FUNC _IMPORT _MSIL_RTL calloc(size_t __nitems, size_t __size);
    div_t _RTL_INTRINS _IMPORT div(int __numer, int __denom);
    ldiv_t _RTL_INTRINS _IMPORT ldiv(long __numer, long __denom);
#if __STDC_VERSION__ >= 199901L || defined(__cplusplus)
    lldiv_t _RTL_FUNC _IMPORT lldiv(long long __numer, long long __denom);
#endif
    void _RTL_FUNC _IMPORT quick_exit(int __status);
    void _RTL_FUNC _IMPORT exit(int __status);
    void _RTL_FUNC _IMPORT _Exit(int __status);
    void _RTL_FUNC _IMPORT _MSIL_RTL free(void* __block);
    char* _RTL_FUNC _IMPORT getenv(const char* __name);
    void* _RTL_FUNC _IMPORT lfind(const void* __key, const void* __base, size_t* __num, size_t __width,
                                  int (*fcmp)(const void*, const void*));
    void* _RTL_FUNC _IMPORT lsearch(const void* __key, void* __base, size_t* __num, size_t __width,
                                    int (*fcmp)(const void*, const void*));
    void* _RTL_FUNC _IMPORT _lfind(const void* __key, const void* __base, size_t* __num, size_t __width,
                                   int (*fcmp)(const void*, const void*));
    void* _RTL_FUNC _IMPORT _lsearch(const void* __key, void* __base, size_t* __num, size_t __width,
                                     int (*fcmp)(const void*, const void*));
    void* _RTL_FUNC _IMPORT aligned_alloc(size_t __align, size_t __size);
    void* _RTL_FUNC _IMPORT _MSIL_RTL malloc(size_t __size);
    int _RTL_FUNC _IMPORT mblen(const char* __s, size_t __n);
    size_t _RTL_FUNC _IMPORT mbstowcs(wchar_t* restrict __pwcs, const char* restrict __s, size_t __n);
    int _RTL_FUNC _IMPORT mbtowc(wchar_t* restrict __pwc, const char* restrict __s, size_t __n);
    void _RTL_FUNC _IMPORT qsort(void* __base, size_t __nelem, size_t __width, int (*__fcmp)(const void*, const void*));
    int _RTL_FUNC _IMPORT rand(void);
    int _RTL_FUNC _IMPORT rand_s(unsigned int* __rand_value);
    void* _RTL_FUNC _IMPORT _MSIL_RTL realloc(void* __block, size_t __size);
    void _RTL_FUNC _IMPORT srand(unsigned __seed);
    char* _RTL_FUNC _IMPORT strdup(const char* string);
    char* _RTL_FUNC _IMPORT _strdup(const char* string);
    double _RTL_FUNC _IMPORT strtod(const char* restrict __s, char** restrict __endptr);
#if __STDC_VERSION__ >= 199901L || defined(__cplusplus)
    float _RTL_FUNC _IMPORT strtof(const char* restrict __s, char** restrict __endptr);
#endif
    long _RTL_FUNC _IMPORT strtol(const char* __s, char** restrict __endptr, int __radix);
#if __STDC_VERSION__ >= 199901L || defined(__cplusplus)
    long long _RTL_FUNC _IMPORT strtoll(const char* restrict __s, char** restrict __endptr, int __radix);
#endif
    long double _RTL_FUNC _IMPORT _strtold(const char* restrict __s, char** restrict __endptr);
#if __STDC_VERSION__ >= 199901L || defined(__cplusplus)
    long double _RTL_FUNC _IMPORT strtold(const char* restrict __s, char** restrict __endptr);
#endif
    unsigned long _RTL_FUNC _IMPORT strtoul(const char* __s, char** __endptr, int __radix);
#if __STDC_VERSION__ >= 199901L || defined(__cplusplus)
    long long _RTL_FUNC _IMPORT strtoull(const char* restrict __s, char** restrict __endptr, int __radix);
#endif
    int _RTL_FUNC _IMPORT system(const char* __command);
    size_t _RTL_FUNC _IMPORT wcstombs(char* restrict __s, const wchar_t* restrict __pwcs, size_t __n);
    int _RTL_FUNC _IMPORT wctomb(char* restrict __s, wchar_t __wc);
    int _RTL_FUNC _IMPORT wctombflush(char* __s);

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

    long double _RTL_FUNC _IMPORT _atold(const char* __s);
    void _RTL_FUNC _IMPORT _exit(int __status);
    char* _RTL_FUNC _IMPORT _fullpath(char*, const char*, size_t);
    char* _RTL_FUNC _IMPORTMM itoa(int __value, char* __stringValue, int __radix);
    char* _RTL_FUNC _IMPORTMM _itoa(int __value, char* __stringValue, int __radix);
    char* _RTL_FUNC _IMPORTMM _i64toa(__int64 __value, char* __stringValue, int __radix);
    char* _RTL_FUNC _IMPORTMM ltoa(long __value, char* __stringValue, int __radix);
    char* _RTL_FUNC _IMPORTMM _ltoa(long __value, char* __stringValue, int __radix);
#if __STDC_VERSION__ >= 199901L || defined(__cplusplus)
    char* _RTL_FUNC _IMPORT lltoa(long long __value, char* __stringValue, int __radix);
    char* _RTL_FUNC _IMPORT _lltoa(long long __value, char* __stringValue, int __radix);
#endif
    void _RTL_FUNC _IMPORT _makepath(char* __path, const char* __drive, const char* __dir, const char* __name, const char* __ext);
    int _RTL_FUNC _IMPORT putenv(const char* __name);
    int _RTL_FUNC _IMPORT _putenv(const char* __name);
    int _RTL_FUNC _IMPORT _putenv_s(const char* __name, const char* value);
    int _RTL_FUNC _IMPORT setenv(const char* name, const char* value, int overwrite);
    int _RTL_FUNC _IMPORT unsetenv(const char* name);

    void _RTL_FUNC _IMPORT _searchenv(const char* __file, const char* __varname, char* __pathname);
    void _RTL_FUNC _IMPORT _searchstr(const char* __file, const char* __ipath, char* __pathname);
    void _RTL_FUNC _IMPORT _splitpath(const char* __path, char* __drive, char* __dir, char* __name, char* __ext);
    void _RTL_FUNC _IMPORT _splitpath2(const char* __path, char* __outpath, char** __drive, char** __dir, char** __name,
                                       char** __ext);
    void _RTL_FUNC _IMPORT swab(char* __from, char* __to, int __nbytes);
    void _RTL_FUNC _IMPORT _swab(char*, char*, int);
    char* _RTL_FUNC _IMPORT ultoa(unsigned long __value, char* __stringValue, int __radix);
    char* _RTL_FUNC _IMPORT _ultoa(unsigned long __value, char* __stringValue, int __radix);
    char* _RTL_FUNC _IMPORT _ui64toa(unsigned __int64 __value, char* __stringValue, int __radix);
    char* _RTL_FUNC _IMPORT utoa(unsigned __value, char* __stringValue, int __radix);
    char* _RTL_FUNC _IMPORT _utoa(unsigned __value, char* __stringValue, int __radix);
#if __STDC_VERSION__ >= 199901L || defined(__cplusplus)
    char* _RTL_FUNC _IMPORT ulltoa(unsigned __int64 __value, char* __stringValue, int __radix);
    char* _RTL_FUNC _IMPORT _ulltoa(unsigned __int64 __value, char* __stringValue, int __radix);
#endif
    char* _RTL_FUNC _IMPORT _ecvt(double, int, int*, int*);
    char* _RTL_FUNC _IMPORT _fcvt(double, int, int*, int*);
    char* _RTL_FUNC _IMPORT _gcvt(double, int, char*);
    char* _RTL_FUNC _IMPORT ecvt(double, int, int*, int*);
    char* _RTL_FUNC _IMPORT fcvt(double, int, int*, int*);
    char* _RTL_FUNC _IMPORT gcvt(double, int, char*);

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

#if defined(__MSIL__)
    int* _RTL_FUNC _IMPORT _errno(void);
#else
int* _RTL_FUNC _IMPORT __GetErrno(void);
#endif
    int* _RTL_FUNC _IMPORT __GetDosErrno(void);

#ifndef __cplusplus
#    if defined(__MSIL__)
#        define errno (*_errno())
#    else
#        define errno (*__GetErrno())
#    endif
#    define _dos_errno (*__GetDosErrno())
#endif
#define sys_nerr _sys_nerr
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

    void _RTL_FUNC _IMPORT perror(const char* __s);

#if defined(__cplusplus)

#    ifndef _TIME_T
#        define _TIME_T
    typedef long time_t;
#    endif

    time_t _RTL_FUNC _IMPORT time(time_t*);

    inline void _RTL_FUNC _IMPORT randomize(void) { srand((unsigned)time(NULL)); }
    inline int _RTL_FUNC _IMPORT random(int num) { return ((rand() * (num)) / (RAND_MAX + 1)); }

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

#ifdef __cplusplus
};
#endif

#pragma pack()

#endif /* __STDLIB_H */