/**
 * This file has no copyright assigned and _is placed in the Public Domain.
 * This file _is part of the mingw-w64 runtime package.
 * No warranty _is given; refer to the file DISCLAIMER.PD within this package.
 */

#pragma GCC system_header

#ifndef _XLOCINFO
#define _XLOCINFO

#    include <stddef.h>
#    include <ctype.h>
#    include <locale.h>
#    include <stdlib.h>
#    include <wchar.h>

#pragma pack(push,1)

#ifdef __cplusplus
extern "C" {
#endif

#define _XA 0x100
#define _XS 0x000
#define _BB _CONTROL
#define _CN _SPACE
#define _DI _DIGIT
#define _LO _LOWER
#define _PU _PUNCT
#define _SP _BLANK
#define _UP _UPPER
#define _XD _HEX

#define _X_ALL LC_ALL
#define _X_COLLATE LC_COLLATE
#define _X_CTYPE LC_CTYPE
#define _X_MONETARY LC_MONETARY
#define _X_NUMERIC LC_NUMERIC
#define _X_TIME LC_TIME
#define _X_MAX LC_MAX
#define _X_MESSAGES 6
#define _NCAT 7

#define _CATMASK(n) ((1 << (n)) >> 1)
#define _M_COLLATE _CATMASK(_X_COLLATE)
#define _M_CTYPE _CATMASK(_X_CTYPE)
#define _M_MONETARY _CATMASK(_X_MONETARY)
#define _M_NUMERIC _CATMASK(_X_NUMERIC)
#define _M_TIME _CATMASK(_X_TIME)
#define _M_MESSAGES _CATMASK(_X_MESSAGES)
#define _M_ALL (_CATMASK(_NCAT) - 1)

typedef struct _Collvec {
  unsigned long _Hand;
  unsigned int _Page;
} _Collvec;

typedef struct _Ctypevec {
  unsigned long _Hand;
  unsigned int _Page;
  const short *_Table;
  int _Delfl;
} _Ctypevec;

typedef struct _Cvtvec {
  unsigned long _Hand;
  unsigned int _Page;
} _Cvtvec;

    inline long long _strtoi64_l(const char* restrict __s, char** restrict __endptr, int __radix, _locale_t __locale) { return _strtoi64(__s, __endptr, __radix); }
    inline unsigned long long _strtoui64_l(const char* restrict __s, char** restrict __endptr, int __radix, _locale_t __locale) { return strtoull(__s, __endptr, __radix); }

    inline int _isdigit_l(int c, _locale_t larg) { return c >= '0' && c <= '9'; }
    inline int _isxdigit_l(int c, _locale_t larg) { return c >= '0' && c <= '9' || c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F'; }
    inline double _strtod_l(const char* ZSTR restrict __s, char* ZSTR* restrict __endptr, _locale_t larg)
    {
        return strtod(__s, __endptr);
    }
    inline long double _strtold_l(const char* ZSTR restrict __s, char* ZSTR* restrict __endptr, _locale_t larg)
    {
        return strtold(__s, __endptr);
    }
    inline long long _strtoll_l(const char* ZSTR restrict __s, char* ZSTR* restrict __endptr, int __radix, _locale_t larg)
    {
        return strtoll(__s, __endptr, __radix);
    }
    inline long long _strtoull_l(const char* ZSTR restrict __s, char* ZSTR* restrict __endptr, int __radix, _locale_t larg)
    {
        return strtoull(__s, __endptr, __radix);
    }
    inline int _sscanf_l(const char* ZSTR restrict __buffer, const char* ZSTR restrict __format, _locale_t larg, ...)
    {
        va_list arg;
        va_start(arg, __format);
        int rv = vsscanf(__buffer, __format, arg);
        va_end(arg);
        return rv;
    }
    inline int _strcoll_l(const char* ZSTR __s1, const char* ZSTR __s2, _locale_t larg) { return strcoll(__s1, __s2); }
    inline size_t _strxfrm_l(char* ZSTR restrict __s1, const char* ZSTR restrict __s2, size_t __n, _locale_t larg)
    {
        return strxfrm(__s1, __s2, __n);
    }
    inline int _wcscoll_l(const wchar_t* __s1, const wchar_t* __s2, _locale_t larg) { return wcscoll(__s1, __s2); }
    inline size_t _wcsxfrm_l(wchar_t* restrict __s1, const wchar_t* restrict __s2, size_t __n, _locale_t larg)
    {
        return wcsxfrm(__s1, __s2, __n);
    }
    inline int _islower_l(int __c, _locale_t larg) { return __c >= 'a' && __c <= 'z'; }
    inline int _isupper_l(int __c, _locale_t larg) { return __c >= 'A' && __c <= 'Z'; }
    inline int _tolower_l(int __ch, _locale_t larg)
    {
        if (_isupper_l(__ch, 0))
            __ch += 'a' - 'A';
        return __ch;
    }
    inline int _toupper_l(int __ch, _locale_t larg)
    {
        if (_islower_l(__ch, 0))
            __ch -= 'a' - 'A';
        return __ch;
    }
    inline int _iswcntrl_l(wint_t __wc, _locale_t larg) { return __STD_NS_QUALIFIER iswcntrl(__wc); }
    inline int _iswdigit_l(wint_t __wc, _locale_t larg) { return __wc >= '0' && __wc <= '9'; }
    inline int _iswgraph_l(wint_t __wc, _locale_t larg) { return __STD_NS_QUALIFIER iswgraph(__wc); }
    inline int _iswlower_l(wint_t __wc, _locale_t larg) { return __wc >= 'a' && __wc <= 'z'; }
    inline int _iswprint_l(wint_t __wc, _locale_t larg) { return __wc >= ' ' && __wc <= '~'; }
    inline int _iswpunct_l(wint_t __wc, _locale_t larg) { return __STD_NS_QUALIFIER iswpunct(__wc); }
    inline int _iswspace_l(wint_t __wc, _locale_t larg) { return __STD_NS_QUALIFIER iswspace(__wc); }
    inline int _iswblank_l(wint_t __wc, _locale_t larg) { return __STD_NS_QUALIFIER iswblank(__wc); }
    inline int _iswupper_l(wint_t __wc, _locale_t larg) { return __wc >= 'A' && __wc <= 'Z'; }
    inline int _iswxdigit_l(wint_t __wc, _locale_t larg)
    {
        return _iswdigit_l(__wc, (locale_t)0) || __wc >= 'a' && __wc <= 'f' || __wc >= 'A' && __wc <= 'F';
    }
    inline int _iswalpha_l(wint_t __wc, _locale_t larg) { return _iswlower_l(__wc, (locale_t)0) || _iswupper_l(__wc, (locale_t)0); }
    inline int _iswalnum_l(wint_t __wc, _locale_t larg) { return _iswalpha_l(__wc, (locale_t)0) || _iswdigit_l(__wc, (locale_t)0); }

    inline wint_t _towlower_l(wint_t __wc, _locale_t larg) { return _tolower_l(__wc, (locale_t)0); }
    inline wint_t _towupper_l(wint_t __wc, _locale_t larg) { return _toupper_l(__wc, (locale_t)0); }

    inline wint_t _btowc_l(int __c, _locale_t larg) { return btowc(__c); }
    inline int _wctob_l(wint_t __c, _locale_t larg) { return wctob(__c); }

    inline size_t _mbrtowc_l(wchar_t* restrict __pwc, const char* ZSTR restrict __s, size_t __n, mbstate_t* restrict __p,
                            _locale_t larg)
    {
        return mbrtowc(__pwc, __s, __n, __p);
    }
    inline size_t _mbtowc_l(wchar_t* restrict __pwc, const char* ZSTR restrict __s, size_t __n, _locale_t larg)
    {
        return mbtowc(__pwc, __s, __n);
    }
    inline size_t _wcrtomb_l(char* ZSTR restrict __s, wchar_t __wc, mbstate_t* restrict __ps, _locale_t larg)
    {
        return wcrtomb(__s, __wc, __ps);
    }
    inline size_t _mbrlen_l(const char* ZSTR restrict __s, size_t __n, mbstate_t* restrict __ps, _locale_t larg)
    {
        return mbrlen(__s, __n, __ps);
    }
    inline size_t _mbsrtowcs_l(wchar_t* restrict __dst, const char* ZSTR* restrict __src, size_t __len, mbstate_t* restrict __ps,
                              _locale_t larg)
    {
        return mbsrtowcs(__dst, __src, __len, __ps);
    }
    inline size_t _wcsrtombs_l(char* ZSTR restrict __dst, const wchar_t** restrict __src, size_t __len, mbstate_t* restrict __ps,
                              _locale_t larg)
    {
        return wcsrtombs(__dst, __src, __len, __ps);
    }
    inline size_t _wcsnrtombs_l(char* ZSTR restrict __dst, const wchar_t** restrict __src, size_t __nms, size_t __len,
                               mbstate_t* restrict __ps, _locale_t larg)
    {
        return wcsnrtombs(__dst, __src, __nms, __len, __ps);
    }
    inline size_t _mbsnrtowcs_l(wchar_t* restrict __dst, const char* ZSTR* restrict __src, size_t nms, size_t __len,
                               mbstate_t* restrict __ps, _locale_t larg)
    {
        return mbsnrtowcs(__dst, __src, nms, __len, __ps);
    }

    inline size_t _strftime_l(char* ZSTR restrict __s, size_t __maxsize, const char* ZSTR restrict __fmt, const struct tm* restrict __t,
                             _locale_t larg)
    {
        return strftime(__s, __maxsize, __fmt, __t);
    }







_Collvec _RTL_FUNC _Getcoll();
_Ctypevec _RTL_FUNC _Getctype();
_Cvtvec _RTL_FUNC _Getcvt();
int _RTL_FUNC _Getdateorder();
int _RTL_FUNC _Mbrtowc(wchar_t *,const char *,size_t,mbstate_t *,const _Cvtvec *);
float _RTL_FUNC _Stof(const char *,char **,long);
double _RTL_FUNC _Stod(const char *,char **,long);
long double _RTL_FUNC _Stold(const char *,char **,long);
int _RTL_FUNC _Strcoll(const char *,const char *,const char *,const char *,const _Collvec *);
size_t _RTL_FUNC _Strxfrm(char *_String1,char *_End1,const char *,const char *,const _Collvec *);
int _RTL_FUNC _Tolower(int,const _Ctypevec *);
int _RTL_FUNC _Toupper(int,const _Ctypevec *);
int _RTL_FUNC _Wcrtomb(char *,wchar_t,mbstate_t *,const _Cvtvec *);
int _RTL_FUNC _Wcscoll(const wchar_t *,const wchar_t *,const wchar_t *,const wchar_t *,const _Collvec *);
size_t _RTL_FUNC _Wcsxfrm(wchar_t *_String1,wchar_t *_End1,const wchar_t *,const wchar_t *,const _Collvec *);
short _RTL_FUNC _Getwctype(wchar_t,const _Ctypevec *);
const wchar_t *_RTL_FUNC _Getwctypes(const wchar_t *,const wchar_t *,short*,const _Ctypevec*);
wchar_t _RTL_FUNC _Towlower(wchar_t,const _Ctypevec *);
wchar_t _RTL_FUNC _Towupper(wchar_t,const _Ctypevec *);

void *_RTL_FUNC _Gettnames();
char *_RTL_FUNC _Getdays();
char *_RTL_FUNC _Getmonths();
size_t _RTL_FUNC _Strftime(char *,size_t _Maxsize,const char *,const struct tm *,void *);

_locale_t _RTL_FUNC _GetLocaleForCP(unsigned int);

// unix
inline locale_t uselocale(locale_t a) { return 0; }
inline void _free_locale(locale_t a) { }
inline void* newlocale(int __mask, const char *__locale, void* __base) { return 0; }

#ifdef __cplusplus
}
#endif

#pragma pack(pop)
#endif
