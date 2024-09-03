#ifndef _HANDLEAPI_H
#define _HANDLEAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-core-handle-l1-1-0 */

/* #include <apiset.h> */
#include <apisetcconv.h>
#include <minwindef.h>

#define INVALID_HANDLE_VALUE  ((HANDLE)(LONG_PTR)-1)

WINBASEAPI BOOL WINAPI CloseHandle(HANDLE);
WINBASEAPI BOOL WINAPI DuplicateHandle(HANDLE, HANDLE, HANDLE, LPHANDLE, DWORD, BOOL, DWORD);
WINBASEAPI BOOL WINAPI CompareObjectHandles(HANDLE, HANDLE);
WINBASEAPI BOOL WINAPI GetHandleInformation(HANDLE, LPDWORD);
WINBASEAPI BOOL WINAPI SetHandleInformation(HANDLE, DWORD, DWORD);

#endif /* _HANDLEAPI_H */
