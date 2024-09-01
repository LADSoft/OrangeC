#ifndef _WCT_H
#define _WCT_H

/* Interfaces for thread wait chain traversal */

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#define WCT_MAX_NODE_COUNT 16

#define WCT_OBJNAME_LENGTH 128

typedef enum _WCT_OBJECT_TYPE {
    WctCriticalSectionType = 1,
    WctSendMessageType,
    WctMutexType,
    WctAlpcType,
    WctComType,
    WctThreadWaitType,
    WctProcessWaitType,
    WctThreadType,
    WctComActivationType,
    WctUnknownType,
    WctSocketIoType,
    WctSmbIoType,
    WctMaxType
} WCT_OBJECT_TYPE;

typedef enum _WCT_OBJECT_STATUS {
    WctStatusNoAccess = 1,
    WctStatusRunning,
    WctStatusBlocked,
    WctStatusPidOnly,
    WctStatusPidOnlyRpcss,
    WctStatusOwned,
    WctStatusNotOwned,
    WctStatusAbandoned,
    WctStatusUnknown,
    WctStatusError,
    WctStatusMax
} WCT_OBJECT_STATUS;

typedef struct _WAITCHAIN_NODE_INFO {
    WCT_OBJECT_TYPE ObjectType;
    WCT_OBJECT_STATUS ObjectStatus;
    union {
        struct {
            WCHAR ObjectName[WCT_OBJNAME_LENGTH];
            LARGE_INTEGER Timeout;
            BOOL Alertable;
        }
        LockObject;

        struct {
            DWORD ProcessId;
            DWORD ThreadId;
            DWORD WaitTime;
            DWORD ContextSwitches;
        }
        ThreadObject;
    };
} WAITCHAIN_NODE_INFO, *PWAITCHAIN_NODE_INFO;

typedef LPVOID HWCT;

typedef VOID (CALLBACK *PWAITCHAINCALLBACK)(HWCT, DWORD_PTR, DWORD, LPDWORD, PWAITCHAIN_NODE_INFO, LPBOOL);

#define WCT_ASYNC_OPEN_FLAG  0x1
#define WCTP_OPEN_ALL_FLAGS  (WCT_ASYNC_OPEN_FLAG)

WINADVAPI HWCT WINAPI OpenThreadWaitChainSession(DWORD, PWAITCHAINCALLBACK);
WINADVAPI VOID WINAPI CloseThreadWaitChainSession(HWCT);

#define WCT_OUT_OF_PROC_FLAG  0x1
#define WCT_OUT_OF_PROC_COM_FLAG  0x2
#define WCT_OUT_OF_PROC_CS_FLAG  0x4
#define WCT_NETWORK_IO_FLAG  0x8
#define WCTP_GETINFO_ALL_FLAGS  (WCT_OUT_OF_PROC_FLAG|WCT_OUT_OF_PROC_COM_FLAG|WCT_OUT_OF_PROC_CS_FLAG)

WINADVAPI BOOL WINAPI GetThreadWaitChain(HWCT, DWORD_PTR, DWORD, DWORD, LPDWORD, PWAITCHAIN_NODE_INFO, LPBOOL);

typedef HRESULT (*PCOGETCALLSTATE)(int, PULONG);
typedef HRESULT (*PCOGETACTIVATIONSTATE)(GUID, DWORD, DWORD *);

WINADVAPI VOID WINAPI RegisterWaitChainCOMCallback(PCOGETCALLSTATE, PCOGETACTIVATIONSTATE);

#endif /* _WCT_H */
