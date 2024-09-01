#ifndef _REALTIMEAPISET_H
#define _REALTIMEAPISET_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-core-realtime-l1 */

/* #include <apiset.h> */
#include <apisetcconv.h>
#include <minwindef.h>

#if (_WIN32_WINNT >= 0x0600)
WINBASEAPI BOOL WINAPI QueryThreadCycleTime(HANDLE, PULONG64);
WINBASEAPI BOOL WINAPI QueryProcessCycleTime(HANDLE, PULONG64);
WINBASEAPI BOOL WINAPI QueryIdleProcessorCycleTime(PULONG, PULONG64);
#endif /* _WIN32_WINNT >= 0x0600 */

#if (_WIN32_WINNT >= 0x0601)
WINBASEAPI BOOL WINAPI QueryIdleProcessorCycleTimeEx(USHORT, PULONG, PULONG64);
#endif /* _WIN32_WINNT >= 0x0601 */

WINBASEAPI VOID WINAPI QueryInterruptTimePrecise(PULONGLONG);
WINBASEAPI VOID WINAPI QueryUnbiasedInterruptTimePrecise(PULONGLONG);
WINBASEAPI VOID WINAPI QueryInterruptTime(PULONGLONG);

#if (_WIN32_WINNT >= 0x0601)
WINBASEAPI BOOL WINAPI QueryUnbiasedInterruptTime(PULONGLONG);
#endif /* _WIN32_WINNT >= 0x0601 */

#endif /* _REALTIMEAPISET_H */
