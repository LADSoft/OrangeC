#ifndef _FIBERSAPI_H
#define _FIBERSAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-core-fibers-l1 */

/* #include <apiset.h> */
#include <apisetcconv.h>
#include <minwindef.h>

#if (_WIN32_WINNT >= 0x0600)

#ifndef FLS_OUT_OF_INDEXES
#define FLS_OUT_OF_INDEXES  ((DWORD)0xFFFFFFFF)
#endif /* _WIN32_WINNT >= 0x0600 */

WINBASEAPI DWORD WINAPI FlsAlloc(PFLS_CALLBACK_FUNCTION);
WINBASEAPI PVOID WINAPI FlsGetValue(DWORD);
WINBASEAPI BOOL WINAPI FlsSetValue(DWORD, PVOID);
WINBASEAPI BOOL WINAPI FlsFree(DWORD);
WINBASEAPI BOOL WINAPI IsThreadAFiber(VOID);

#endif /* _WIN32_WINNT >= 0x0600 */

#endif /* _FIBERSAPI_H */
