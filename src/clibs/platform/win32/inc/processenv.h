#ifndef _PROCESSENV_H
#define _PROCESSENV_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-core-processenvironment-l1 */

/* #include <apiset.h> */
#include <apisetcconv.h>

#include <minwindef.h>

WINBASEAPI LPCH WINAPI GetEnvironmentStrings(VOID);
WINBASEAPI LPWCH WINAPI GetEnvironmentStringsW(VOID);
WINBASEAPI BOOL WINAPI SetEnvironmentStringsW(LPWCH);
WINBASEAPI BOOL WINAPI FreeEnvironmentStringsA(LPCH);
WINBASEAPI BOOL WINAPI FreeEnvironmentStringsW(LPWCH);
WINBASEAPI HANDLE WINAPI GetStdHandle(DWORD);
WINBASEAPI BOOL WINAPI SetStdHandle(DWORD, HANDLE);
#if (_WIN32_WINNT >= 0x0600)
WINBASEAPI BOOL WINAPI SetStdHandleEx(DWORD, HANDLE, PHANDLE);
#endif /* _WIN32_WINNT >= 0x0600 */
WINBASEAPI LPSTR WINAPI GetCommandLineA(VOID);
WINBASEAPI LPWSTR WINAPI GetCommandLineW(VOID);
WINBASEAPI DWORD WINAPI GetEnvironmentVariableA(LPCSTR, LPSTR, DWORD);
WINBASEAPI DWORD WINAPI GetEnvironmentVariableW(LPCWSTR, LPWSTR, DWORD);
WINBASEAPI BOOL WINAPI SetEnvironmentVariableA(LPCSTR, LPCSTR);
WINBASEAPI BOOL WINAPI SetEnvironmentVariableW(LPCWSTR, LPCWSTR);
WINBASEAPI DWORD WINAPI ExpandEnvironmentStringsA(LPCSTR, LPSTR, DWORD);
WINBASEAPI DWORD WINAPI ExpandEnvironmentStringsW(LPCWSTR, LPWSTR, DWORD);
WINBASEAPI BOOL WINAPI SetCurrentDirectoryA(LPCSTR);
WINBASEAPI BOOL WINAPI SetCurrentDirectoryW(LPCWSTR);
WINBASEAPI DWORD WINAPI GetCurrentDirectoryA(DWORD, LPSTR);
WINBASEAPI DWORD WINAPI GetCurrentDirectoryW(DWORD, LPWSTR);
WINBASEAPI DWORD APIENTRY SearchPathA(LPCSTR, LPCSTR, LPCSTR, DWORD, LPSTR, LPSTR *);
WINBASEAPI DWORD WINAPI SearchPathW(LPCWSTR, LPCWSTR, LPCWSTR, DWORD, LPWSTR, LPWSTR *);
#if _WIN32_WINNT >= 0x0502
WINBASEAPI BOOL WINAPI NeedCurrentDirectoryForExePathA(LPCSTR);
WINBASEAPI BOOL WINAPI NeedCurrentDirectoryForExePathW(LPCWSTR);
#endif /* _WIN32_WINNT >= 0x0502 */

#ifdef UNICODE
#define GetEnvironmentStrings  GetEnvironmentStringsW
#define SetEnvironmentStrings  SetEnvironmentStringsW
#define FreeEnvironmentStrings  FreeEnvironmentStringsW
#define GetCommandLine  GetCommandLineW
#define GetEnvironmentVariable  GetEnvironmentVariableW
#define SetEnvironmentVariable  SetEnvironmentVariableW
#define ExpandEnvironmentStrings  ExpandEnvironmentStringsW
#define SetCurrentDirectory  SetCurrentDirectoryW
#define GetCurrentDirectory  GetCurrentDirectoryW
#define SearchPath  SearchPathW
#define NeedCurrentDirectoryForExePath  NeedCurrentDirectoryForExePathW
#else /* !UNICODE */
#define GetEnvironmentStringsA  GetEnvironmentStrings
#define FreeEnvironmentStrings  FreeEnvironmentStringsA
#define GetCommandLine  GetCommandLineA
#define GetEnvironmentVariable  GetEnvironmentVariableA
#define SetEnvironmentVariable  SetEnvironmentVariableA
#define ExpandEnvironmentStrings  ExpandEnvironmentStringsA
#define SetCurrentDirectory  SetCurrentDirectoryA
#define GetCurrentDirectory  GetCurrentDirectoryA
#define SearchPath  SearchPathA
#define NeedCurrentDirectoryForExePath  NeedCurrentDirectoryForExePathA
#endif /* !UNICODE */

#endif /* _PROCESSENV_H */
