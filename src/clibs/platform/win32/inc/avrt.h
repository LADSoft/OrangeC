#ifndef _AVRT_H
#define _AVRT_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Multimedia class scheduler API definitions */

#ifdef __cplusplus
extern "C" {
#endif

#define THREAD_ORDER_GROUP_INFINITE_TIMEOUT  (-1LL)

#define AVRTAPI  DECLSPEC_IMPORT

typedef enum _AVRT_PRIORITY {
    AVRT_PRIORITY_LOW = -1,
    AVRT_PRIORITY_NORMAL,
    AVRT_PRIORITY_HIGH,
    AVRT_PRIORITY_CRITICAL
} AVRT_PRIORITY, *PAVRT_PRIORITY;

AVRTAPI HANDLE WINAPI AvSetMmThreadCharacteristicsA(LPCSTR, LPDWORD);
AVRTAPI HANDLE WINAPI AvSetMmThreadCharacteristicsW(LPCWSTR, LPDWORD);
AVRTAPI HANDLE WINAPI AvSetMmMaxThreadCharacteristicsA(LPCSTR, LPCSTR, LPDWORD);
AVRTAPI HANDLE WINAPI AvSetMmMaxThreadCharacteristicsW(LPCWSTR, LPCWSTR, LPDWORD);
AVRTAPI BOOL WINAPI AvRevertMmThreadCharacteristics(HANDLE);
AVRTAPI BOOL WINAPI AvSetMmThreadPriority(HANDLE, AVRT_PRIORITY);
AVRTAPI BOOL WINAPI AvRtCreateThreadOrderingGroup(PHANDLE, PLARGE_INTEGER, GUID *, PLARGE_INTEGER);
AVRTAPI BOOL WINAPI AvRtCreateThreadOrderingGroupExA(PHANDLE, PLARGE_INTEGER, GUID *, PLARGE_INTEGER, LPCSTR);
AVRTAPI BOOL WINAPI AvRtCreateThreadOrderingGroupExW(PHANDLE, PLARGE_INTEGER, GUID *, PLARGE_INTEGER, LPCWSTR);
AVRTAPI BOOL WINAPI AvRtJoinThreadOrderingGroup(PHANDLE, GUID *, BOOL);
AVRTAPI BOOL WINAPI AvRtWaitOnThreadOrderingGroup(HANDLE);
AVRTAPI BOOL WINAPI AvRtLeaveThreadOrderingGroup(HANDLE);
AVRTAPI BOOL WINAPI AvRtDeleteThreadOrderingGroup(HANDLE);
AVRTAPI BOOL WINAPI AvQuerySystemResponsiveness(HANDLE, PULONG);

#ifdef UNICODE
#define AvSetMmThreadCharacteristics  AvSetMmThreadCharacteristicsW
#define AvSetMmMaxThreadCharacteristics  AvSetMmMaxThreadCharacteristicsW
#define AvRtCreateThreadOrderingGroupEx  AvRtCreateThreadOrderingGroupExW
#else /* !UNICODE */
#define AvSetMmThreadCharacteristics  AvSetMmThreadCharacteristicsA
#define AvSetMmMaxThreadCharacteristics  AvSetMmMaxThreadCharacteristicsA
#define AvRtCreateThreadOrderingGroupEx  AvRtCreateThreadOrderingGroupExA
#endif /* !UNICODE */

#ifdef __cplusplus
}
#endif

#endif /* _AVRT_H */
