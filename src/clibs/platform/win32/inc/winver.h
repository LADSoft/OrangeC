#ifndef _WINVER_H
#define _WINVER_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows version management definitions */

#include <verrsrc.h>

#ifdef __cplusplus
extern "C" {
#endif

DWORD APIENTRY VerFindFileA(DWORD,LPSTR,LPSTR,LPSTR,LPSTR,PUINT,LPSTR,PUINT);
DWORD APIENTRY VerFindFileW(DWORD,LPWSTR,LPWSTR,LPWSTR,LPWSTR,PUINT,LPWSTR,PUINT);
DWORD APIENTRY VerInstallFileA(DWORD,LPSTR,LPSTR,LPSTR,LPSTR,LPSTR,LPSTR,PUINT);
DWORD APIENTRY VerInstallFileW(DWORD,LPWSTR,LPWSTR,LPWSTR,LPWSTR,LPWSTR,LPWSTR,PUINT);
DWORD APIENTRY GetFileVersionInfoSizeA(LPCSTR,PDWORD);
DWORD APIENTRY GetFileVersionInfoSizeW(LPCWSTR,PDWORD);
BOOL APIENTRY GetFileVersionInfoA(LPCSTR,DWORD,DWORD,PVOID);
BOOL APIENTRY GetFileVersionInfoW(LPCWSTR,DWORD,DWORD,PVOID);
DWORD APIENTRY GetFileVersionInfoSizeExA(DWORD,LPCSTR,LPDWORD);
DWORD APIENTRY GetFileVersionInfoSizeExW(DWORD,LPCWSTR,LPDWORD);
BOOL APIENTRY GetFileVersionInfoExA(DWORD,LPCSTR,DWORD,DWORD,LPVOID);
BOOL APIENTRY GetFileVersionInfoExW(DWORD,LPCWSTR,DWORD,DWORD,LPVOID);
DWORD APIENTRY VerLanguageNameA(DWORD,LPSTR,DWORD);
DWORD APIENTRY VerLanguageNameW(DWORD,LPWSTR,DWORD);
BOOL APIENTRY VerQueryValueA(LPCVOID,LPCSTR,PVOID*,PUINT);
BOOL APIENTRY VerQueryValueW(LPCVOID,LPCWSTR,PVOID*,PUINT);

#ifdef UNICODE
#define VerFindFile VerFindFileW
#define VerInstallFile VerInstallFileW
#define GetFileVersionInfoSize GetFileVersionInfoSizeW
#define GetFileVersionInfo GetFileVersionInfoW
#define GetFileVersionInfoSizeEx  GetFileVersionInfoSizeExW
#define GetFileVersionInfoEx  GetFileVersionInfoExW
#define VerLanguageName VerLanguageNameW
#define VerQueryValue VerQueryValueW
#else /* !UNICODE */
#define VerFindFile VerFindFileA
#define VerInstallFile VerInstallFileA
#define GetFileVersionInfoSize GetFileVersionInfoSizeA
#define GetFileVersionInfo GetFileVersionInfoA
#define GetFileVersionInfoSizeEx  GetFileVersionInfoSizeExA
#define GetFileVersionInfoEx  GetFileVersionInfoExA
#define VerLanguageName VerLanguageNameA
#define VerQueryValue VerQueryValueA
#endif /* !UNICODE */

#ifdef __cplusplus
}
#endif

#endif /* _WINVER_H */
