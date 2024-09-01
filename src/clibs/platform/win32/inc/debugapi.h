#ifndef _DEBUGAPI_H
#define _DEBUGAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-core-debug-l1 */

/* #include <apiset.h> */
#include <apisetcconv.h>
#include <minwindef.h>
#include <minwinbase.h>

#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
WINBASEAPI BOOL WINAPI IsDebuggerPresent(VOID);
#endif /* (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400) */
WINBASEAPI VOID WINAPI DebugBreak(VOID);
WINBASEAPI VOID WINAPI OutputDebugStringA(LPCSTR);
WINBASEAPI VOID WINAPI OutputDebugStringW(LPCWSTR);
WINBASEAPI BOOL APIENTRY ContinueDebugEvent(DWORD, DWORD, DWORD);
WINBASEAPI BOOL APIENTRY WaitForDebugEvent(LPDEBUG_EVENT, DWORD);
WINBASEAPI BOOL APIENTRY DebugActiveProcess(DWORD);
WINBASEAPI BOOL APIENTRY DebugActiveProcessStop(DWORD);
#if (_WIN32_WINNT >= 0x0501)
WINBASEAPI BOOL WINAPI CheckRemoteDebuggerPresent(HANDLE, PBOOL);
#endif /* _WIN32_WINNT >= 0x0501 */
WINBASEAPI BOOL APIENTRY WaitForDebugEventEx(LPDEBUG_EVENT, DWORD);

#ifdef UNICODE
#define OutputDebugString  OutputDebugStringW
#else /* !UNICODE */
#define OutputDebugString  OutputDebugStringA
#endif /* !UNICODE */

#endif /* _DEBUGAPI_H */
