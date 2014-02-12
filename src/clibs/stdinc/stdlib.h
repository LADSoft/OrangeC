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
#ifndef __STDLIB_H
#define __STDLIB_H

#pragma pack(1)

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#ifdef __cplusplus
namespace std {
extern "C" {
#endif

#ifndef _DIV_T
#define _DIV_T
typedef struct {
        int     quot;
        int     rem;
} div_t;
#endif

#ifndef _LDIV_T
#define _LDIV_T
typedef struct {
        long    quot;
        long    rem;
} ldiv_t;
#endif

#if  __STDC_VERSION__ >= 199901L
#ifndef _LLDIV_T
#define _LLDIV_T
typedef struct {
        long long    quot;
        long long   rem;
} lldiv_t;
#endif
#endif

#define MB_CUR_MAX 3

/* Maximum value returned by "rand" function
*/

#define RAND_MAX 0x7FFFU

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

typedef void (* atexit_t)(void);


void         _RTL_FUNC abort(void);
void         _RTL_FUNC _abort(void) ;

#if !defined(__ABS_DEFINED)
#define __ABS_DEFINED

int         _RTL_INTRINS abs(int __x);
long        _RTL_INTRINS labs(long __x);
#if  __STDC_VERSION__ >= 199901L
long long   _RTL_FUNC llabs(long long __x);
#endif

#endif /* __ABS_DEFINED */

int         _RTL_FUNC atexit(void (*__func)(void));
int         _RTL_FUNC at_quick_exit(void (*__func)(void));
double      _RTL_FUNC atof(const char *__s);
int         _RTL_FUNC atoi(const char *__s);
long        _RTL_FUNC atol(const char *__s);
__int64		_RTL_FUNC _atoi64(const char *__s);
#if  __STDC_VERSION__ >= 199901L
long long   _RTL_FUNC atoll(const char *__s);
#endif

void *      _RTL_FUNC bsearch(const void *__key, const void *__base,
                           size_t __nelem, size_t __width,
                           int (*fcmp)(const void *,
                           const void *));
void *      _RTL_FUNC calloc(size_t __nitems, size_t __size);
div_t       _RTL_INTRINS div(int __numer, int __denom);
ldiv_t      _RTL_INTRINS ldiv(long __numer, long __denom);
#if  __STDC_VERSION__ >= 199901L
lldiv_t     _RTL_FUNC lldiv(long long __numer, long long __denom);
#endif
void        _RTL_FUNC quick_exit(int __status);
void        _RTL_FUNC exit(int __status);
void        _RTL_FUNC _Exit(int __status);
void        _RTL_FUNC free(void *__block);
char *      _RTL_FUNC getenv(const char *__name);
void *      _RTL_FUNC lfind(const void * __key, const void * __base,
                                size_t * __num, size_t __width,
                                int (*fcmp)(const void *, const void *));
void *      _RTL_FUNC lsearch(const void * __key, void * __base,
                                size_t * __num, size_t __width,
                                int (*fcmp)(const void *, const void *));
void *      _RTL_FUNC _lfind(const void * __key, const void * __base,
                                size_t * __num, size_t __width,
                                int (*fcmp)(const void *, const void *));
void *      _RTL_FUNC _lsearch(const void * __key, void * __base,
                                size_t * __num, size_t __width,
                                int (*fcmp)(const void *, const void *));
void *      _RTL_FUNC aligned_alloc(size_t __align, size_t __size);
void *      _RTL_FUNC malloc(size_t __size);
int         _RTL_FUNC mblen(const char *__s, size_t __n);
size_t      _RTL_FUNC mbstowcs(wchar_t * restrict __pwcs,   
                                    const char * restrict __s, size_t __n);
int         _RTL_FUNC mbtowc(wchar_t * restrict __pwc, 
                                    const char *restrict __s, size_t __n);
void        _RTL_FUNC qsort(void *__base, size_t __nelem, size_t __width,
                       int (* __fcmp)(const void *, const void *));
int         _RTL_FUNC rand(void);
void  *     _RTL_FUNC realloc(void *__block, size_t __size);
void        _RTL_FUNC srand(unsigned __seed);
char *      _RTL_FUNC strdup(const char *string);
char *      _RTL_FUNC _strdup(const char *string);
double      _RTL_FUNC strtod(const char * restrict __s, char ** restrict __endptr);
#if  __STDC_VERSION__ >= 199901L
float       _RTL_FUNC strtof(const char * restrict __s, char ** restrict __endptr);
#endif
long        _RTL_FUNC strtol(const char *__s, char **restrict __endptr,
                                    int __radix);
#if  __STDC_VERSION__ >= 199901L
long long   _RTL_FUNC strtoll(const char *restrict __s, char **restrict __endptr,
                                    int __radix);
#endif
long double _RTL_FUNC _strtold(const char *restrict __s, char ** restrict __endptr);
#if  __STDC_VERSION__ >= 199901L
long double _RTL_FUNC strtold(const char *restrict __s, char ** restrict __endptr);
#endif
unsigned long   _RTL_FUNC strtoul(const char *__s, char **__endptr,
                                       int __radix);
#if  __STDC_VERSION__ >= 199901L
long long   _RTL_FUNC strtoull(const char *restrict __s, char **restrict __endptr,
                                    int __radix);
#endif
int         _RTL_FUNC system(const char *__command);
size_t      _RTL_FUNC wcstombs(char *restrict __s, const wchar_t *restrict __pwcs,
                                    size_t __n);
int         _RTL_FUNC wctomb(char *restrict __s, wchar_t __wc);
int         _RTL_FUNC wctombflush(char *__s);


extern  char          _RTL_DATA ** _environ;
extern  int           _RTL_DATA    _fmode;
extern  unsigned char _RTL_DATA    _osmajor;
extern  unsigned char _RTL_DATA    _osminor;



/* Constants for MSC pathname functions */

#ifndef _WIN32
#define _MAX_PATH       80
#define _MAX_DRIVE      3
#define _MAX_DIR        66
#define _MAX_FNAME      9
#define _MAX_EXT        5
#else
#define _MAX_PATH       260
#define _MAX_DRIVE      3
#define _MAX_DIR        256
#define _MAX_FNAME      256
#define _MAX_EXT        256
#endif
#define _MAX_PATH2      (_MAX_PATH+4)

long double     _RTL_FUNC _atold(const char *__s);
void            _RTL_FUNC _exit(int __status);
char     *  _RTL_FUNC _fullpath(char *, const char *, size_t);
char     *  _RTL_FUNC itoa(int __value, char *__string, int __radix);
char     *  _RTL_FUNC _itoa(int __value, char *__string, int __radix);
char     *  _RTL_FUNC _i64toa(__int64 __value, char *__string, int __radix);
char     *  _RTL_FUNC ltoa(long __value, char *__string, int __radix);
char     *  _RTL_FUNC _ltoa(long __value, char *__string, int __radix);
#if  __STDC_VERSION__ >= 199901L
char * _RTL_FUNC lltoa(long long __value, char *__string, int __radix);
char * _RTL_FUNC _lltoa(long long __value, char *__string, int __radix);
#endif
void        _RTL_FUNC      _makepath( char *__path,
                                 const char *__drive,
                                 const char *__dir,
                                 const char *__name,
                                 const char *__ext );
int         _RTL_FUNC      putenv(const char *__name);
int         _RTL_FUNC      _putenv(const char *__name);


void        _RTL_FUNC _searchenv(const char *__file,
                                 const char *__varname,
                                 char *__pathname);
void        _RTL_FUNC _searchstr(const char *__file,
                                 const char *__ipath,
                                 char *__pathname);
void        _RTL_FUNC _splitpath(const char *__path,
                                 char *__drive,
                                 char *__dir,
                                 char *__name,
                                 char *__ext );
void        _RTL_FUNC _splitpath2(const char *__path, char *__outpath,
                                  char **__drive,
                                  char **__dir,
                                  char **__name,
                                  char **__ext);
void        _RTL_FUNC swab(char *__from, char *__to, int __nbytes);
void        _RTL_FUNC _swab(char *, char *, int);
char     *  _RTL_FUNC ultoa(unsigned long __value, char *__string,
                                 int __radix);
char     *  _RTL_FUNC _ultoa(unsigned long __value, char *__string,
                                 int __radix);
char     *  _RTL_FUNC _ui64toa(unsigned __int64 __value, char *__string,
                                 int __radix);
char * _RTL_FUNC utoa(unsigned __value, char *__string, int __radix);
char * _RTL_FUNC _utoa(unsigned __value, char *__string, int __radix);
#if  __STDC_VERSION__ >= 199901L
char     *  _RTL_FUNC ulltoa(unsigned __int64 __value, char *__string,
                                 int __radix);
char     *  _RTL_FUNC _ulltoa(unsigned __int64 __value, char *__string,
                                 int __radix);
#endif
char * _RTL_FUNC _ecvt(double, int, int *, int *);
char * _RTL_FUNC _fcvt(double, int, int *, int *);
char * _RTL_FUNC _gcvt(double, int, char *);
char * _RTL_FUNC ecvt(double, int, int *, int *);
char * _RTL_FUNC fcvt(double, int, int *, int *);
char * _RTL_FUNC gcvt(double, int, char *);

typedef int (__cdecl * _onexit_t)(void);
#define onexit_t _onexit_t

_onexit_t _RTL_FUNC _onexit(_onexit_t);
onexit_t _RTL_FUNC onexit(onexit_t);

void _RTL_FUNC _beep(unsigned, unsigned);
void _RTL_FUNC _sleep(unsigned long);

unsigned char   _RTL_FUNC _crotl(unsigned char __value, int __count);
unsigned char   _RTL_FUNC _crotr(unsigned char __value, int __count);
unsigned int _RTL_FUNC _rotl(unsigned int, int);
unsigned int _RTL_FUNC _rotr(unsigned int, int);
unsigned long _RTL_FUNC _lrotl(unsigned long, int);
unsigned long _RTL_FUNC _lrotr(unsigned long, int);

#define atoi(s)     ((int) atol(s))

int  *_RTL_FUNC __GetErrno(void);
int  *_RTL_FUNC __GetDosErrno(void);

#define errno (*__GetErrno())
#define _dos_errno (*__GetDosErrno())

#define sys_nerr _sys_nerr
extern int _RTL_DATA _sys_nerr ;

#define sys_errlist _sys_errlist
extern char _RTL_DATA *_sys_errlist[] ;

extern int _RTL_DATA __argc ;
extern char _RTL_DATA ** __argv ;

void      _RTL_FUNC perror(const char *__s);

#if defined(__cplusplus)

#ifndef  _TIME_T
#define  _TIME_T
typedef long time_t;
#endif

time_t _RTL_FUNC time(time_t *);

inline void _RTL_FUNC randomize(void) { srand((unsigned) time(NULL)); }
inline int _RTL_FUNC random(int num) { return ((rand()*(num))/(RAND_MAX+1)); }

inline int max(int a, int b) { return (((a) > (b)) ? (a) : (b)); }
inline int min(int a, int b) { return (((a) < (b)) ? (a) : (b)); }

/* must include the C++ header to get min and max defs */
#else /* __cplusplus */

#define random(num)((rand()*(num))/(RAND_MAX+1))

#define randomize() srand((unsigned)time(NULL))

#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

#endif

#if 0
/*
size_t _RTL_FUNC _mbstrlen(const char *s);
int    _RTL_FUNC _set_error_mode(int);
void _RTL_FUNC _seterrormode(int);

*/
#endif
#ifdef __cplusplus
};
};
#endif

#pragma pack()

#endif  /* __STDLIB_H */

#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__STDLIB_H_USING_LIST)
#define __STDLIB_H_USING_LIST
    using std::abort;
    using std::_abort;
    using std::abs;
    using std::labs;
#if  __STDC_VERSION__ >= 199901L
    using std::llabs;
#endif
    using std::atexit;
    using std::at_quick_exit;
    using std::atof;
    using std::atoi;
    using std::atol;
    using std::_atoi64;
    using std::bsearch;
    using std::calloc;
    using std::div;
    using std::ldiv;
#if  __STDC_VERSION__ >= 199901L
    using std::lldiv;
#endif
    using std::quick_exit;
    using std::exit;
    using std::_Exit;
    using std::free;
    using std::getenv;
    using std::lfind;
    using std::lsearch;
    using std::_lfind;
    using std::_lsearch;
    using std::aligned_alloc;
    using std::malloc;
    using std::mblen;
    using std::mbstowcs;
    using std::mbtowc;
    using std::qsort;
    using std::rand;
    using std::realloc;
    using std::srand;
    using std::strdup;
    using std::strtod;
#if  __STDC_VERSION__ >= 199901L
    using std::strtof;
#endif
    using std::strtol;
#if  __STDC_VERSION__ >= 199901L
    using std::strtoll;
#endif
    using std::_strtold;
#if  __STDC_VERSION__ >= 199901L
    using std::strtold;
#endif
    using std::strtoul;
#if  __STDC_VERSION__ >= 199901L
    using std::strtoull;
#endif
    using std::system;
    using std::wcstombs;
    using std::wctomb;
    using std::wctombflush;
    using std::_atold;
    using std::_crotl;
    using std::_crotr;
    using std::_exit;
    using std::itoa;
    using std::ltoa;
#if  __STDC_VERSION__ >= 199901L
    using std::lltoa:
    using std::_lltoa:
#endif
    using std::_itoa;
    using std::_ltoa;
    using std::_i64toa;
    using std::_makepath;
    using std::_fullpath;
    using std::putenv;
    using std::_putenv;
    using std::_rotl;
    using std::_rotr;
    using std::_lrotl;
    using std::_lrotr;
    using std::_searchenv;
    using std::_searchstr;
    using std::_splitpath;
    using std::_splitpath2;
    using std::swab;
    using std::_swab;
    using std::ultoa;
    using std::_ultoa;
    using std::_ui64toa;
#if  __STDC_VERSION__ >= 199901L
    using std::ulltoa;
    using std::_ulltoa:
#endif
    using std::utoa;
    using std::_utoa;
    using std::perror;
    using std::time;
    using std::randomize;
    using std::random;
    using std::_strdup;
    using std::_ecvt;
    using std::ecvt;
    using std::_fcvt;
    using std::fcvt;
    using std::_gcvt;
    using std::gcvt;
    using std::_onexit;
    using std::onexit;
    using std::_beep;
    using std::_sleep;
#endif /* __USING_CNAME__ */
