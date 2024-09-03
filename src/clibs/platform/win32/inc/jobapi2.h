#ifndef _JOBAPI2_H
#define _JOBAPI2_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-core-job-l2 */

/* #include <apiset.h> */
#include <apisetcconv.h>
#include <minwindef.h>
#include <minwinbase.h>

typedef struct JOBOBJECT_IO_RATE_CONTROL_INFORMATION {
    LONG64 MaxIops;
    LONG64 MaxBandwidth;
    LONG64 ReservationIops;
    PCWSTR VolumeName;
    ULONG BaseIoSize;
    ULONG ControlFlags;
} JOBOBJECT_IO_RATE_CONTROL_INFORMATION;

WINBASEAPI HANDLE WINAPI CreateJobObjectW(LPSECURITY_ATTRIBUTES, LPCWSTR);
WINBASEAPI VOID WINAPI FreeMemoryJobObject(VOID *);
WINBASEAPI HANDLE WINAPI OpenJobObjectW(DWORD, BOOL, LPCWSTR);
WINBASEAPI BOOL WINAPI AssignProcessToJobObject(HANDLE, HANDLE);
WINBASEAPI BOOL WINAPI TerminateJobObject(HANDLE, UINT);
WINBASEAPI BOOL WINAPI SetInformationJobObject(HANDLE, JOBOBJECTINFOCLASS, LPVOID, DWORD);
WINBASEAPI DWORD WINAPI SetIoRateControlInformationJobObject(HANDLE hJob, JOBOBJECT_IO_RATE_CONTROL_INFORMATION *);
WINBASEAPI BOOL WINAPI QueryInformationJobObject(HANDLE, JOBOBJECTINFOCLASS, LPVOID, DWORD, LPDWORD);
WINBASEAPI DWORD WINAPI QueryIoRateControlInformationJobObject(HANDLE, PCWSTR, JOBOBJECT_IO_RATE_CONTROL_INFORMATION **, ULONG *);

#endif /* _JOBAPI2_H */
