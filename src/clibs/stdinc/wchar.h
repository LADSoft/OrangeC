/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#ifndef __WCHAR_H
#define __WCHAR_H

#pragma pack(1)

#ifndef __STDDEF_H
#    include <stddef.h>
#endif

#ifndef __STDARG_H
#    include <stdarg.h>
#endif

#ifndef __STDIO_H
#    include <stdio.h>
#endif

#ifndef __TCHAR_H
#    include <tchar.h>
#endif

/* we use UTF-8 for multibyte encoding
 * wide characters are written in network order (big endian)
 *
 * already declared in stddef.h, declared here for reference
 */
#ifndef RC_INVOKED
#ifndef _WINT_T_DEFINED
#    define _WINT_T_DEFINED
typedef unsigned int wint_t;
#endif
#endif
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef RC_INVOKED

#ifndef _MBSTATE_T_DEFINED
#    define _MBSTATE_T_DEFINED
    typedef struct __mbstate_t
    {
        size_t left;
        wint_t value;
    } mbstate_t;
#endif

    struct tm;
#endif
#define WCHAR_MIN ((wchar_t)0)
#define WCHAR_MAX ((wchar_t)(~WCHAR_MIN))

#ifndef WEOF
#    define WEOF (wint_t)(-1)
#endif

#ifndef RC_INVOKED
    wchar_t* _RTL_FUNC wcscpy(wchar_t* restrict __dest, const wchar_t* restrict __src);
    wchar_t* _RTL_FUNC wcsncpy(wchar_t* restrict __dest, const wchar_t* restrict __src, size_t __n);

    wchar_t* _RTL_FUNC wcpcpy(wchar_t* restrict __dest, const wchar_t* restrict __src);
    wchar_t* _RTL_FUNC wcpncpy(wchar_t* restrict __dest, const wchar_t* restrict __src, size_t __n);

    wchar_t* _RTL_FUNC wcscat(wchar_t* restrict __dest, const wchar_t* restrict __src);
    wchar_t* _RTL_FUNC wcsncat(wchar_t* restrict __dest, const wchar_t* restrict __src, size_t __n);

    int _RTL_FUNC wcscmp(const wchar_t* __s1, const wchar_t* __s2);
    int _RTL_FUNC wcsncmp(const wchar_t* __s1, const wchar_t* __s2, size_t __n);

    int _RTL_FUNC wcsicmp(wchar_t* __s1, wchar_t* __s2);
    int _RTL_FUNC wcsnicmp(wchar_t* __s1, wchar_t* __s2, size_t __n);
    int _RTL_FUNC _wcsicmp(wchar_t* __s1, wchar_t* __s2);
    int _RTL_FUNC _wcsnicmp(wchar_t* __s1, wchar_t* __s2, size_t __n);

    /* these two do blind copy/compare irregardless of locale*/
    int _RTL_FUNC wcscoll(const wchar_t* __s1, const wchar_t* __s2);
    size_t _RTL_FUNC wcsxfrm(wchar_t* restrict __s1, const wchar_t* restrict __s2, size_t __n);

    wchar_t* _RTL_FUNC wcsdup(const wchar_t* __s);
    wchar_t* _RTL_FUNC _wcsdup(const wchar_t* __s);
    wchar_t* _RTL_FUNC wcsndup(const wchar_t* __s, size_t count);
    wchar_t* _RTL_FUNC _wcsndup(const wchar_t* __s, size_t count);
#ifdef __cplusplus
    const wchar_t* _RTL_FUNC wcschr(const wchar_t* __wcs, wchar_t __wc);
    const wchar_t* _RTL_FUNC wcsrchr(const wchar_t* __wcs, wchar_t __wc);
#else
    wchar_t* _RTL_FUNC wcschr(const wchar_t* __wcs, wchar_t __wc);
    wchar_t* _RTL_FUNC wcsrchr(const wchar_t* __wcs, wchar_t __wc);
#endif
    size_t _RTL_FUNC wcscspn(const wchar_t* __wcs, const wchar_t* __reject);
    size_t _RTL_FUNC wcsspn(const wchar_t* __wcs, const wchar_t* __accept);
#ifdef __cplusplus
    const wchar_t* _RTL_FUNC wcspbrk(const wchar_t* __wcs, const wchar_t* __accept);
    const wchar_t* _RTL_FUNC wcsstr(const wchar_t* __haystack, const wchar_t* __needle);
#else
    wchar_t* _RTL_FUNC wcspbrk(const wchar_t* __wcs, const wchar_t* __accept);
    wchar_t* _RTL_FUNC wcsstr(const wchar_t* __haystack, const wchar_t* __needle);
#endif
    wchar_t* _RTL_FUNC wcstok(wchar_t* restrict __s, const wchar_t* restrict __delim, wchar_t** ptr);

    wchar_t* _RTL_FUNC wcsupr(wchar_t* __s);
    wchar_t* _RTL_FUNC wcslwr(wchar_t* __s);
    wchar_t* _RTL_FUNC _wcsupr(wchar_t* __s);
    wchar_t* _RTL_FUNC _wcslwr(wchar_t* __s);

    size_t _RTL_FUNC wcslen(const wchar_t* __s);
    size_t _RTL_FUNC wcsnlen(const wchar_t* __s, size_t maxlen);

#if defined(__cplusplus) || __STDC_VERSION__ >= 202311
    const wchar_t* _RTL_FUNC wmemchr(const wchar_t* __s, wchar_t __c, size_t __n);
#else
    wchar_t* _RTL_FUNC wmemchr(const wchar_t* __s, wchar_t __c, size_t __n);
#endif
    int wmemcmp(const wchar_t* __s1, const wchar_t* __s2, size_t __n);
    wchar_t* _RTL_FUNC wmemcpy(wchar_t* restrict __s1, const wchar_t* restrict __s2, size_t __n);
    wchar_t* _RTL_FUNC wmemmove(wchar_t* __s1, const wchar_t* __s2, size_t __n);
    wchar_t* _RTL_FUNC wmemset(wchar_t* __s, wchar_t __c, size_t __n);

    wint_t _RTL_FUNC btowc(int __c);
    int _RTL_FUNC wctob(wint_t __c);

    int _RTL_FUNC mbsinit(const mbstate_t* __ps);

    size_t _RTL_FUNC mbrtowc(wchar_t* restrict __pwc, const char* restrict __s, size_t __n, mbstate_t* restrict __p);
    size_t _RTL_FUNC wcrtomb(char* restrict __s, wchar_t __wc, mbstate_t* restrict __ps);

    size_t _RTL_FUNC mbrlen(const char* restrict __s, size_t __n, mbstate_t* restrict __ps);

    size_t _RTL_FUNC mbsrtowcs(wchar_t* restrict __dst, const char** restrict __src, size_t __len, mbstate_t* restrict __ps);
    size_t _RTL_FUNC wcsrtombs(char* restrict __dst, const wchar_t** restrict __src, size_t __len, mbstate_t* restrict __ps);
    size_t _RTL_FUNC mbsnrtowcs(wchar_t* restrict __dst, const char** restrict __src, size_t __nms, size_t __len,
                                mbstate_t* restrict __ps);
    size_t _RTL_FUNC wcsnrtombs(char* restrict __dst, const wchar_t** restrict __src, size_t __nms, size_t __len,
                                mbstate_t* restrict __ps);

    float _RTL_FUNC wcstof(const wchar_t* restrict __nptr, wchar_t** restrict __endptr);
    double _RTL_FUNC wcstod(const wchar_t* restrict __nptr, wchar_t** restrict __endptr);
    long double _RTL_FUNC wcstold(const wchar_t* restrict __nptr, wchar_t** restrict __endptr);
    long int _RTL_FUNC wcstol(const wchar_t* restrict __nptr, wchar_t** restrict __endptr, int __base);
    unsigned long int _RTL_FUNC wcstoul(const wchar_t* restrict __nptr, wchar_t** restrict __endptr, int __base);
#if __STDC_VERSION__ >= 199901L || defined(__cplusplus)
    long long int _RTL_FUNC wcstoll(const wchar_t* restrict __nptr, wchar_t** restrict __endptr, int __base);
    unsigned long long int _RTL_FUNC wcstoull(const wchar_t* restrict __nptr, wchar_t** restrict __endptr, int __base);
#endif
    int _RTL_FUNC _wtoi(const wchar_t *);
    long _RTL_FUNC _wtol(const wchar_t *);
    long long _RTL_FUNC _wtoll(const wchar_t *);
    double _RTL_FUNC wtof(const wchar_t* __stringValue);
    long double _RTL_FUNC _wtold(const wchar_t* __stringValue);
    wchar_t* _RTL_FUNC itow(int __value, wchar_t* __stringValue, int __radix);
    wchar_t* _RTL_FUNC ltow(long __value, wchar_t* __stringValue, int __radix);
    wchar_t* _RTL_FUNC ultow(unsigned long __value, wchar_t* __stringValue, int __radix);
    wchar_t* _RTL_FUNC _itow(int __value, wchar_t* __stringValue, int __radix);
    wchar_t* _RTL_FUNC _ltow(long __value, wchar_t* __stringValue, int __radix);
    wchar_t* _RTL_FUNC _ultow(unsigned long __value, wchar_t* __stringValue, int __radix);


    wchar_t * _RTL_FUNC wcsnset(wchar_t *, wchar_t, size_t);
    wchar_t * _RTL_FUNC wcsrev(wchar_t *);
    wchar_t * _RTL_FUNC _wcsnset(wchar_t *, wchar_t, size_t);
    wchar_t * _RTL_FUNC _wcsrev(wchar_t *);

    wchar_t * _RTL_FUNC _wcserror(int);
    wchar_t * _RTL_FUNC _wcsichr(const wchar_t *, wchar_t);
    wchar_t * _RTL_FUNC _wcsistr(const wchar_t *, const wchar_t *);
    wchar_t * _RTL_FUNC _wcsrichr(const wchar_t *, wchar_t);
    wchar_t * _RTL_FUNC _wcstok_ms(wchar_t * restrict, const wchar_t * restrict);  /* 06-03-28 */
    wchar_t * _RTL_FUNC _wmemichr(const wchar_t *, wchar_t, size_t);
    int _RTL_FUNC _wmemicmp(const wchar_t *, const wchar_t *, size_t);


    size_t _RTL_FUNC wcsftime(wchar_t* restrict __str, size_t __maxsize, const wchar_t* restrict __format,
                              const struct tm* restrict __t);

    FILE* _RTL_FUNC open_wmemstream(wchar_t** ptr, size_t* sizeloc);
    int _RTL_FUNC fwprintf(FILE* restrict __stream, const wchar_t* restrict __format, ...);
    int _RTL_FUNC fwscanf(FILE* restrict __stream, const wchar_t* restrict __format, ...);
#if defined(__MSIL__) && defined(__MANAGED__)
    int _RTL_FUNC swprintf(wchar_t* restrict s, size_t n, const wchar_t* restrict __format, ...);
#elif defined(__MSVCRT_DLL) || defined(__CRTDLL_DLL) || defined(__MSIL__)
int _RTL_FUNC swprintf(wchar_t* restrict s, const wchar_t* restrict __format, ...);
#else
int _RTL_FUNC swprintf(wchar_t* restrict s, size_t n, const wchar_t* restrict __format, ...);
#endif
    int _RTL_FUNC snwprintf(wchar_t* restrict buffer, size_t n, const wchar_t* restrict format, ...);
    int _RTL_FUNC _snwprintf(wchar_t* restrict buffer, size_t n, const wchar_t* restrict format, ...);
    int _RTL_FUNC swscanf(const wchar_t* restrict s, const wchar_t* restrict __format, ...);
    int _RTL_FUNC vfwprintf(FILE* restrict __stream, const wchar_t* restrict __format, va_list __arg);
    int _RTL_FUNC vfwscanf(FILE* restrict __stream, const wchar_t* restrict __format, va_list __arg);
#if defined(__MSIL__) && defined(__MANAGED__)
    int _RTL_FUNC vswprintf(wchar_t* restrict s, size_t n, const wchar_t* restrict __format, va_list __arg);
#elif defined(__MSVCRT_DLL) || defined(__CRTDLL_DLL) || defined(__MSIL__) && !defined(__MANAGED__)
int _RTL_FUNC vswprintf(wchar_t* restrict s, const wchar_t* restrict __format, va_list __arg);
#else
int _RTL_FUNC vswprintf(wchar_t* restrict s, size_t n, const wchar_t* restrict __format, va_list __arg);
#endif
    int _RTL_FUNC vsnwprintf(wchar_t* restrict buf, size_t n, const wchar_t* restrict format, va_list arglist);
    int _RTL_FUNC _vsnwprintf(wchar_t* restrict buf, size_t n, const wchar_t* restrict format, va_list arglist);
    int _RTL_FUNC vswscanf(const wchar_t* restrict s, const wchar_t* restrict __format, va_list __arg);
    int _RTL_FUNC vwprintf(const wchar_t* restrict __format, va_list __arg);
    int _RTL_FUNC wprintf(const wchar_t* restrict __format, ...);
    int _RTL_FUNC wscanf(const wchar_t* restrict __format, ...);
    int _RTL_FUNC vwscanf(const wchar_t* restrict __format, va_list __arg);
    wint_t _RTL_FUNC fgetwc(FILE* __stream);
    wchar_t* _RTL_FUNC fgetws(wchar_t* restrict __s, int __n, FILE* restrict __stream);
    wint_t _RTL_FUNC fputwc(wchar_t c, FILE* __stream);
    int _RTL_FUNC fputws(const wchar_t* restrict s, FILE* restrict __stream);
    int _RTL_FUNC fwide(FILE* __stream, int __mode);
    wint_t _RTL_FUNC getwc(FILE* __stream);
    wint_t _RTL_FUNC getwchar(void);
    wint_t _RTL_FUNC putwc(wchar_t c, FILE* __stream);
    wint_t _RTL_FUNC putwchar(wchar_t __c);
    wint_t _RTL_FUNC ungetwc(wint_t c, FILE* __stream);
    int fwide(FILE* __stream, int __mode);

    wint_t _RTL_FUNC getwc_unlocked(FILE* stream);
    wint_t _RTL_FUNC getwchar_unlocked(void);
    wint_t _RTL_FUNC fgetwc_unlocked(FILE* stream);
    wint_t _RTL_FUNC fputwc_unlocked(wchar_t wc, FILE* stream);
    wint_t _RTL_FUNC putwc_unlocked(wchar_t wc, FILE* stream);
    wint_t _RTL_FUNC putwchar_unlocked(wchar_t wc);
    wchar_t* _RTL_FUNC fgetws_unlocked(wchar_t* ws, int n, FILE* stream);
    int _RTL_FUNC fputws_unlocked(const wchar_t* ws, FILE* stream);
#endif

#ifdef __cplusplus
};
#endif

#pragma pack()

#endif /* wchar.h  */