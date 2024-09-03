#ifndef _PROCESSTOPOLOGYAPI_H
#define _PROCESSTOPOLOGYAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-core-processtopology-l1 */

/* #include <apiset.h> */
#include <apisetcconv.h>
#include <minwindef.h>
#include <minwinbase.h>

#if (_WIN32_WINNT >= 0x0601)

WINBASEAPI BOOL WINAPI GetProcessGroupAffinity(HANDLE, PUSHORT, PUSHORT);
WINBASEAPI BOOL WINAPI GetThreadGroupAffinity(HANDLE, PGROUP_AFFINITY);
WINBASEAPI BOOL WINAPI SetThreadGroupAffinity(HANDLE, CONST GROUP_AFFINITY *, PGROUP_AFFINITY);

#endif /* _WIN32_WINNT >= 0x0601 */

#endif /* _PROCESSTOPOLOGYAPI_H */
