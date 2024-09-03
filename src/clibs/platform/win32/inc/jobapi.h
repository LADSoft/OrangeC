#ifndef _JOBAPI_H
#define _JOBAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-core-job-l1 */

/* #include <apiset.h> */
#include <apisetcconv.h>
#include <minwindef.h>
#include <minwinbase.h>

#if (_WIN32_WINNT >= 0x0501)
WINBASEAPI BOOL WINAPI IsProcessInJob(HANDLE, HANDLE, PBOOL);
#endif /* _WIN32_WINNT >= 0x0501 */

#endif /* _JOBAPI_H */
