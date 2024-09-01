#ifndef _NAMEDPIPEAPI_H
#define _NAMEDPIPEAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-core-namedpipe-l1-1-0 */

/* #include <apiset.h> */
#include <apisetcconv.h>
#include <minwindef.h>
#include <minwinbase.h>

WINBASEAPI BOOL WINAPI CreatePipe(PHANDLE, PHANDLE, LPSECURITY_ATTRIBUTES, DWORD);
WINBASEAPI BOOL WINAPI ConnectNamedPipe(HANDLE, LPOVERLAPPED);
WINBASEAPI BOOL WINAPI DisconnectNamedPipe(HANDLE);
WINBASEAPI BOOL WINAPI SetNamedPipeHandleState(HANDLE, LPDWORD, LPDWORD, LPDWORD);
WINBASEAPI BOOL WINAPI PeekNamedPipe(HANDLE, LPVOID, DWORD, LPDWORD, LPDWORD, LPDWORD);
WINBASEAPI BOOL WINAPI TransactNamedPipe(HANDLE, LPVOID, DWORD, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
WINBASEAPI HANDLE WINAPI CreateNamedPipeW(LPCWSTR, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, LPSECURITY_ATTRIBUTES);
WINBASEAPI BOOL WINAPI WaitNamedPipeW(LPCWSTR, DWORD);
#if (_WIN32_WINNT >= 0x0600)
WINBASEAPI BOOL WINAPI GetNamedPipeClientComputerNameW(HANDLE, LPWSTR, ULONG);
#endif /* _WIN32_WINNT >= 0x0600 */
WINADVAPI BOOL WINAPI ImpersonateNamedPipeClient(HANDLE);
WINBASEAPI BOOL WINAPI GetNamedPipeInfo(HANDLE hNamedPipe, LPDWORD, LPDWORD, LPDWORD, LPDWORD);
WINBASEAPI BOOL WINAPI GetNamedPipeHandleStateW(HANDLE, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPWSTR, DWORD);
WINBASEAPI BOOL WINAPI CallNamedPipeW(LPCWSTR, LPVOID, DWORD, LPVOID, DWORD, LPDWORD, DWORD);

#ifdef UNICODE
#define CreateNamedPipe  CreateNamedPipeW
#define WaitNamedPipe  WaitNamedPipeW
#define GetNamedPipeClientComputerName  GetNamedPipeClientComputerNameW
#define GetNamedPipeHandleState  GetNamedPipeHandleStateW
#define CallNamedPipe  CallNamedPipeW
#endif /* UNICODE */

#endif /* _NAMEDPIPEAPI_H */
