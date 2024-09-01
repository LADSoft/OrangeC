#ifndef _RESTARTMANAGER_H
#define _RESTARTMANAGER_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows RestartManger API definitions */

#ifdef __cplusplus
extern "C" {
#endif

#define RM_SESSION_KEY_LEN  sizeof(GUID)
#define CCH_RM_SESSION_KEY  RM_SESSION_KEY_LEN*2
#define CCH_RM_MAX_APP_NAME 255
#define CCH_RM_MAX_SVC_NAME 63
#define RM_INVALID_TS_SESSION -1
#define RM_INVALID_PROCESS -1

typedef enum _RM_APP_TYPE {
    RmUnknownApp = 0,
    RmMainWindow = 1,
    RmOtherWindow = 2,
    RmService = 3,
    RmExplorer = 4,
    RmConsole = 5,
    RmCritical = 1000
} RM_APP_TYPE;

typedef enum _RM_SHUTDOWN_TYPE {
    RmForceShutdown = 0x1,
    RmShutdownOnlyRegistered = 0x10
} RM_SHUTDOWN_TYPE;

typedef enum _RM_APP_STATUS {
    RmStatusUnknown = 0x0,
    RmStatusRunning = 0x1,
    RmStatusStopped = 0x2,
    RmStatusStoppedOther = 0x4,
    RmStatusRestarted = 0x8,
    RmStatusErrorOnStop = 0x10,
    RmStatusErrorOnRestart = 0x20,
    RmStatusShutdownMasked = 0x40,
    RmStatusRestartMasked = 0x80
} RM_APP_STATUS;

typedef enum _RM_REBOOT_REASON {
    RmRebootReasonNone = 0x0,
    RmRebootReasonPermissionDenied = 0x1,
    RmRebootReasonSessionMismatch = 0x2,
    RmRebootReasonCriticalProcess = 0x4,
    RmRebootReasonCriticalService = 0x8,
    RmRebootReasonDetectedSelf = 0x10
} RM_REBOOT_REASON;

typedef struct _RM_UNIQUE_PROCESS {
    DWORD dwProcessId;
    FILETIME ProcessStartTime;
} RM_UNIQUE_PROCESS, *PRM_UNIQUE_PROCESS;

typedef struct _RM_PROCESS_INFO {
    RM_UNIQUE_PROCESS Process;
    WCHAR strAppName[CCH_RM_MAX_APP_NAME + 1];
    WCHAR strServiceShortName[CCH_RM_MAX_SVC_NAME + 1];
    RM_APP_TYPE ApplicationType;
    ULONG AppStatus;
    DWORD TSSessionId;
    BOOL bRestartable;
} RM_PROCESS_INFO, *PRM_PROCESS_INFO;

typedef enum _RM_FILTER_TRIGGER {
    RmFilterTriggerInvalid = 0,
    RmFilterTriggerFile,
    RmFilterTriggerProcess,
    RmFilterTriggerService
} RM_FILTER_TRIGGER;

typedef enum _RM_FILTER_ACTION {
    RmInvalidFilterAction = 0,
    RmNoRestart = 1,
    RmNoShutdown = 2
} RM_FILTER_ACTION;

typedef struct _RM_FILTER_INFO {
    RM_FILTER_ACTION FilterAction;
    RM_FILTER_TRIGGER FilterTrigger;
    DWORD cbNextOffset;
    union {
        LPWSTR strFilename;
        RM_UNIQUE_PROCESS Process;
        LPWSTR strServiceShortName;
    };
} RM_FILTER_INFO, *PRM_FILTER_INFO;

typedef void (*RM_WRITE_STATUS_CALLBACK)(UINT);

DWORD WINAPI RmStartSession(DWORD *, DWORD , WCHAR []);
DWORD WINAPI RmJoinSession(DWORD *, const WCHAR []);
DWORD WINAPI RmEndSession(DWORD);
DWORD WINAPI RmRegisterResources(DWORD, UINT, LPCWSTR[], UINT, RM_UNIQUE_PROCESS [], UINT, LPCWSTR []);
DWORD WINAPI RmGetList(DWORD, UINT *, UINT *, RM_PROCESS_INFO [], LPDWORD);
DWORD WINAPI RmShutdown(DWORD, ULONG, RM_WRITE_STATUS_CALLBACK);
DWORD WINAPI RmRestart(DWORD, DWORD, RM_WRITE_STATUS_CALLBACK);
DWORD WINAPI RmCancelCurrentTask(DWORD);
DWORD WINAPI RmAddFilter(DWORD, LPCWSTR, RM_UNIQUE_PROCESS *, LPCWSTR, RM_FILTER_ACTION);
DWORD WINAPI RmRemoveFilter(DWORD, LPCWSTR, RM_UNIQUE_PROCESS *, LPCWSTR);
DWORD WINAPI RmGetFilterList(DWORD, PBYTE, DWORD, LPDWORD);

#ifdef __cplusplus
}
#endif

#endif /* _RESTARTMANAGER_H */
