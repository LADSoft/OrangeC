#ifndef _LMAT_H
#define _LMAT_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Schedule service API definitions */

#ifdef __cplusplus
extern "C" {
#endif

#define JOB_RUN_PERIODICALLY  0x01
#define JOB_EXEC_ERROR  0x02
#define JOB_RUNS_TODAY  0x04
#define JOB_ADD_CURRENT_DATE  0x08
#define JOB_NONINTERACTIVE  0x10

#define JOB_INPUT_FLAGS  (JOB_RUN_PERIODICALLY|JOB_ADD_CURRENT_DATE|JOB_NONINTERACTIVE)
#define JOB_OUTPUT_FLAGS  (JOB_RUN_PERIODICALLY|JOB_EXEC_ERROR|JOB_RUNS_TODAY|JOB_NONINTERACTIVE)

typedef struct _AT_ENUM {
    DWORD JobId;
    DWORD_PTR JobTime;
    DWORD DaysOfMonth;
    UCHAR DaysOfWeek;
    UCHAR Flags;
    LPWSTR Command;
} AT_ENUM, *PAT_ENUM, *LPAT_ENUM;

typedef struct _AT_INFO {
    DWORD_PTR JobTime;
    DWORD DaysOfMonth;
    UCHAR DaysOfWeek;
    UCHAR Flags;
    LPWSTR Command;
} AT_INFO, *PAT_INFO, *LPAT_INFO;

NET_API_STATUS WINAPI NetScheduleJobAdd(LPCWSTR,PBYTE,LPDWORD);
NET_API_STATUS WINAPI NetScheduleJobDel(LPCWSTR,DWORD,DWORD);
NET_API_STATUS WINAPI NetScheduleJobEnum(LPCWSTR,PBYTE*,DWORD,PDWORD,PDWORD,PDWORD);
NET_API_STATUS WINAPI NetScheduleJobGetInfo(LPCWSTR,DWORD,PBYTE*);

#ifdef __cplusplus
}
#endif

#endif /* _LMAT_H */
