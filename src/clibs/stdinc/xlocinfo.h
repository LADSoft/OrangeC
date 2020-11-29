/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _XLOCINFO
#define _XLOCINFO

#ifndef __STDDEF_H
#    include <stddef.h>
#endif
#ifndef CTYPE_H
#include <ctype.h>
#endif
#ifndef LOCALE_H
#include <locale.h>
#endif
#ifndef STDLIB_H 
#include <stdlib.h>
#endif
#ifndef WCHAR_H
#include <wchar.h>
#endif

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

inline int _islower_l(int __c, locale_t larg) { return __c >= 'a' && __c <= 'z'; }
inline int _isupper_l(int __c, locale_t larg) { return __c >= 'A' && __c <= 'Z'; }

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

#ifdef __cplusplus
}
#endif

#pragma pack(pop)
#endif
