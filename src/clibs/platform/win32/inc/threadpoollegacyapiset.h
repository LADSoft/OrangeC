#ifndef _THREADPOOLLEGACYAPISET_H
#define _THREADPOOLLEGACYAPISET_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-core-threadpool-l1 */

/* #include <apiset.h> */
#include <apisetcconv.h>
#include <minwindef.h>
#include <minwinbase.h>

WINBASEAPI BOOL WINAPI QueueUserWorkItem(LPTHREAD_START_ROUTINE, PVOID, ULONG);
WINBASEAPI BOOL WINAPI UnregisterWaitEx(HANDLE, HANDLE);
WINBASEAPI HANDLE WINAPI CreateTimerQueue(VOID);
WINBASEAPI BOOL WINAPI CreateTimerQueueTimer(PHANDLE, HANDLE, WAITORTIMERCALLBACK, PVOID, DWORD, DWORD, ULONG);
WINBASEAPI BOOL WINAPI ChangeTimerQueueTimer(HANDLE, HANDLE, ULONG, ULONG);
WINBASEAPI BOOL WINAPI DeleteTimerQueueTimer(HANDLE, HANDLE, HANDLE);
WINBASEAPI BOOL WINAPI DeleteTimerQueueEx(HANDLE, HANDLE);

#endif /* _THREADPOOLLEGACYAPISET_H */
