#ifndef _CONSOLEAPI_H
#define _CONSOLEAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-core-console-l1 */

/* #include <apiset.h> */
#include <apisetcconv.h>

#include <windows.h>

WINBASEAPI BOOL WINAPI AllocConsole(VOID);
WINBASEAPI UINT WINAPI GetConsoleCP(VOID);
WINBASEAPI BOOL WINAPI GetConsoleMode(HANDLE, LPDWORD);
WINBASEAPI UINT WINAPI GetConsoleOutputCP(VOID);
WINBASEAPI BOOL WINAPI GetNumberOfConsoleInputEvents(HANDLE, LPDWORD);
WINBASEAPI BOOL WINAPI PeekConsoleInputA(HANDLE, PINPUT_RECORD, DWORD, LPDWORD);
WINBASEAPI BOOL WINAPI ReadConsoleA(HANDLE, LPVOID, DWORD, LPDWORD, PCONSOLE_READCONSOLE_CONTROL);
WINBASEAPI BOOL WINAPI ReadConsoleW(HANDLE, LPVOID, DWORD, LPDWORD, PCONSOLE_READCONSOLE_CONTROL);
WINBASEAPI BOOL WINAPI ReadConsoleInputA(HANDLE, PINPUT_RECORD, DWORD, LPDWORD);
WINBASEAPI BOOL WINAPI ReadConsoleInputW(HANDLE, PINPUT_RECORD, DWORD, LPDWORD);
WINBASEAPI BOOL WINAPI SetConsoleCtrlHandler(PHANDLER_ROUTINE, BOOL);
WINBASEAPI BOOL WINAPI SetConsoleMode(HANDLE, DWORD);
WINBASEAPI BOOL WINAPI WriteConsoleA(HANDLE, CONST VOID *, DWORD, LPDWORD, LPVOID);
WINBASEAPI BOOL WINAPI WriteConsoleW(HANDLE, CONST VOID *, DWORD, LPDWORD, LPVOID);

#ifdef UNICODE
#define ReadConsole  ReadConsoleW
#define ReadConsoleInput  ReadConsoleInputW
#define WriteConsole  WriteConsoleW
#else /* !UNICODE */
#define PeekConsoleInput  PeekConsoleInputA
#define ReadConsole  ReadConsoleA
#define ReadConsoleInput  ReadConsoleInputA
#define WriteConsole  WriteConsoleA
#endif /* !UNICODE */

#endif /* _CONSOLEAPI_H */
