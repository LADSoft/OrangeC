#ifndef _SYSTEMTOPOLOGYAPI_H
#define _SYSTEMTOPOLOGYAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-core-systemtopology-l1 */

/* #include <apiset.h> */
#include <apisetcconv.h>
#include <minwindef.h>
#include <minwinbase.h>

WINBASEAPI BOOL WINAPI GetNumaHighestNodeNumber(PULONG);

#if _WIN32_WINNT >= 0x0601
WINBASEAPI BOOL WINAPI GetNumaNodeProcessorMaskEx(USHORT, PGROUP_AFFINITY);
#endif /* _WIN32_WINNT >= 0x0601 */

#if _WIN32_WINNT >= 0x0601
WINBASEAPI BOOL WINAPI GetNumaProximityNodeEx(ULONG, PUSHORT);
#endif /* _WIN32_WINNT >= 0x0601 */

#endif /* _SYSTEMTOPOLOGYAPI_H */
