#ifndef _MINWINDEF_H
#define _MINWINDEF_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows Basic Type Definitions */

#ifndef NO_STRICT
#ifndef STRICT
#define STRICT  1
#endif /* STRICT */
#endif /* NO_STRICT */

#ifndef WIN32
#define WIN32
#endif

#ifdef __cplusplus
extern "C" {
#endif


#ifndef BASETYPES
#define BASETYPES
typedef unsigned long ULONG;
typedef ULONG *PULONG;
typedef unsigned short USHORT;
typedef USHORT *PUSHORT;
typedef unsigned char UCHAR;
typedef UCHAR *PUCHAR;
typedef char *PSZ;
#endif /* !BASETYPES */

#define MAX_PATH  260

#ifndef NULL
#define NULL  ((void *)0)
#endif /* NULL */

#ifndef FALSE
#define FALSE  0
#endif

#ifndef TRUE
#define TRUE  1
#endif

#define cdecl
#ifndef CDECL
#define CDECL
#endif

#define CALLBACK  __stdcall
#define WINAPI  __stdcall
#define WINAPIV  __cdecl
#define APIENTRY  WINAPI
#define APIPRIVATE  __stdcall
#define PASCAL  __stdcall

#ifndef WINAPI_INLINE
#define WINAPI_INLINE  WINAPI
#endif

#ifndef CONST
#define CONST const
#endif /* CONST */

#undef far
#define far

#undef near
#define near

#undef pascal
#ifdef _STDCALL_SUPPORTED
#define pascal  __stdcall
#else /* !_STDCALL_SUPPORTED */
#define pascal
#endif /* !_STDCALL_SUPPORTED */

#undef FAR
#define FAR far

#undef NEAR
#define NEAR near

typedef unsigned long DWORD;
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef float FLOAT;
typedef FLOAT *PFLOAT;
typedef BOOL *PBOOL;
typedef BOOL *LPBOOL;
typedef BYTE *PBYTE;
typedef BYTE *LPBYTE;
typedef int *PINT;
typedef int *LPINT;
typedef WORD *PWORD;
typedef WORD *LPWORD;
typedef long *LPLONG;
typedef DWORD *PDWORD;
typedef DWORD *LPDWORD;
typedef void *LPVOID;
typedef CONST void *LPCVOID;

typedef int INT;
typedef unsigned int UINT;
typedef unsigned int *PUINT;

#ifndef NT_INCLUDED
#include <winnt.h>
#endif /* NT_INCLUDED */

typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;
typedef LONG_PTR LRESULT;

#ifndef NOMINMAX
#ifndef max
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif /* max */
#ifndef min
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif /* min */
#endif /* NOMINMAX */

#define MAKEWORD(a,b)  ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
#define MAKELONG(a,b)  ((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#define LOWORD(l)      ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#define HIWORD(l)      ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#define LOBYTE(w)      ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#define HIBYTE(w)      ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))

typedef HANDLE *SPHANDLE;
typedef HANDLE *LPHANDLE;
typedef HANDLE HGLOBAL;
typedef HANDLE HLOCAL;
typedef HANDLE GLOBALHANDLE;
typedef HANDLE LOCALHANDLE;

#ifdef _WIN64
typedef INT_PTR (WINAPI *FARPROC)();
typedef INT_PTR (WINAPI *NEARPROC)();
typedef INT_PTR (WINAPI *PROC)();
#else /* !_WIN64 */
typedef int (WINAPI *FARPROC)();
typedef int (WINAPI *NEARPROC)();
typedef int (WINAPI *PROC)();
#endif /* !_WIN64 */

typedef WORD ATOM;

DECLARE_HANDLE(HKEY);
typedef HKEY *PHKEY;
DECLARE_HANDLE(HMETAFILE);
DECLARE_HANDLE(HINSTANCE);
typedef HINSTANCE HMODULE;
DECLARE_HANDLE(HRGN);
DECLARE_HANDLE(HRSRC);
DECLARE_HANDLE(HSPRITE);
DECLARE_HANDLE(HLSURF);
DECLARE_HANDLE(HSTR);
DECLARE_HANDLE(HTASK);
DECLARE_HANDLE(HWINSTA);
DECLARE_HANDLE(HKL);

typedef int HFILE;

typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;
#define _FILETIME_


#ifdef __cplusplus
}
#endif

#endif /* _MINWINDEF_H */
