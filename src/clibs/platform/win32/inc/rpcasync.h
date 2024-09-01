#ifndef _RPCASYNC_H
#define _RPCASYNC_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows Async RPC runtime API definitions */

#if defined(__RPC_WIN64__)
#include <pshpack8.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _KRPCENV_
#include <ntddk.h>
#include <windef.h>
#endif /* _KRPCENV_ */

#define RPC_ASYNC_VERSION_1_0  sizeof(RPC_ASYNC_STATE)

#define RPC_C_NOTIFY_ON_SEND_COMPLETE  0x1
#define RPC_C_INFINITE_TIMEOUT  INFINITE

#define RpcAsyncGetCallHandle(pAsync)  (((PRPC_ASYNC_STATE) pAsync)->RuntimeInfo)

typedef enum _RPC_NOTIFICATION_TYPES {
    RpcNotificationTypeNone,
    RpcNotificationTypeEvent,
#ifndef _KRPCENV_
    RpcNotificationTypeApc,
    RpcNotificationTypeIoc,
    RpcNotificationTypeHwnd,
#endif /* _KRPCENV_ */
    RpcNotificationTypeCallback
} RPC_NOTIFICATION_TYPES;

typedef enum _RPC_ASYNC_EVENT {
    RpcCallComplete,
    RpcSendComplete,
    RpcReceiveComplete,
    RpcClientDisconnect,
#if (NTDDI_VERSION >= NTDDI_VISTA)
    RpcClientCancel,
#endif /* NTDDI_VERSION >= NTDDI_VISTA */
} RPC_ASYNC_EVENT;

struct _RPC_ASYNC_STATE;

typedef void RPC_ENTRY RPCNOTIFICATION_ROUTINE(struct _RPC_ASYNC_STATE*,void*,RPC_ASYNC_EVENT);
typedef RPCNOTIFICATION_ROUTINE *PFN_RPCNOTIFICATION_ROUTINE;

typedef union _RPC_ASYNC_NOTIFICATION_INFO {
#ifndef _KRPCENV_
    struct {
        PFN_RPCNOTIFICATION_ROUTINE NotificationRoutine;
        HANDLE hThread;
    } APC;
    struct {
        HANDLE hIOPort;
        DWORD dwNumberOfBytesTransferred;
        DWORD_PTR dwCompletionKey;
        LPOVERLAPPED lpOverlapped;
    } IOC;
    struct {
        HWND hWnd;
        UINT Msg;
    } HWND;
#endif /* _KRPCENV_ */
#ifndef _KRPCENV_
    HANDLE hEvent;
#else
    PKEVENT Event;
#endif /* _KRPCENV_ */
    PFN_RPCNOTIFICATION_ROUTINE NotificationRoutine;
} RPC_ASYNC_NOTIFICATION_INFO, *PRPC_ASYNC_NOTIFICATION_INFO;

typedef struct _RPC_ASYNC_STATE {
    unsigned int Size;
    unsigned long Signature;
    long Lock;
    unsigned long Flags;
    void *StubInfo;
    void *UserInfo;
    void *RuntimeInfo;
    RPC_ASYNC_EVENT Event;
    RPC_NOTIFICATION_TYPES NotificationType;
    RPC_ASYNC_NOTIFICATION_INFO u;
    LONG_PTR Reserved[4];
} RPC_ASYNC_STATE, *PRPC_ASYNC_STATE;

RPCRTAPI RPC_STATUS RPC_ENTRY RpcAsyncInitializeHandle(PRPC_ASYNC_STATE,unsigned int);
#ifndef _KRPCENV_
RPCRTAPI RPC_STATUS RPC_ENTRY RpcAsyncRegisterInfo(PRPC_ASYNC_STATE);
#endif /* _KRPCENV_ */
RPCRTAPI RPC_STATUS RPC_ENTRY RpcAsyncGetCallStatus(PRPC_ASYNC_STATE);
RPCRTAPI RPC_STATUS RPC_ENTRY RpcAsyncCompleteCall(PRPC_ASYNC_STATE,void*);
RPCRTAPI RPC_STATUS RPC_ENTRY RpcAsyncAbortCall(PRPC_ASYNC_STATE,unsigned long);
RPCRTAPI RPC_STATUS RPC_ENTRY RpcAsyncCancelCall(PRPC_ASYNC_STATE,BOOL);

#if (NTDDI_VERSION >= NTDDI_WINXP)
typedef enum tagExtendedErrorParamTypes {
    eeptAnsiString = 1,
    eeptUnicodeString,
    eeptLongVal,
    eeptShortVal,
    eeptPointerVal,
    eeptNone,
    eeptBinary
} ExtendedErrorParamTypes;

#define MaxNumberOfEEInfoParams  4
#define RPC_EEINFO_VERSION  1

typedef struct tagBinaryParam {
    void *Buffer;
    short Size;
} BinaryParam;

typedef struct tagRPC_EE_INFO_PARAM {
    ExtendedErrorParamTypes ParameterType;
    union
        {
        LPSTR AnsiString;
        LPWSTR UnicodeString;
        long LVal;
        short SVal;
        ULONGLONG PVal;
        BinaryParam BVal;
        } u;
} RPC_EE_INFO_PARAM;

#define EEInfoPreviousRecordsMissing  1
#define EEInfoNextRecordsMissing  2
#define EEInfoUseFileTime  4

#define EEInfoGCCOM  11
#if (NTDDI_VERSION >= NTDDI_WS03)
#define EEInfoGCFRS  12
#endif /* NTDDI_VERSION >= NTDDI_WS03 */

typedef struct tagRPC_EXTENDED_ERROR_INFO {
    ULONG Version;
    LPWSTR ComputerName;
    ULONG ProcessID;
    union
        {
#ifndef _KRPCENV_
        SYSTEMTIME SystemTime;
        FILETIME FileTime;
#else /* _KRPCENV_ */
        LARGE_INTEGER KernelTime;
#endif /* _KRPCENV_ */
        } u;
    ULONG GeneratingComponent;
    ULONG Status;
    USHORT DetectionLocation;
    USHORT Flags;
    int NumberOfParameters;
    RPC_EE_INFO_PARAM Parameters[MaxNumberOfEEInfoParams];
} RPC_EXTENDED_ERROR_INFO;

typedef struct tagRPC_ERROR_ENUM_HANDLE {
    ULONG Signature;
    void *CurrentPos;
    void *Head;
} RPC_ERROR_ENUM_HANDLE;

RPCRTAPI RPC_STATUS RPC_ENTRY RpcErrorStartEnumeration(RPC_ERROR_ENUM_HANDLE*);
RPCRTAPI RPC_STATUS RPC_ENTRY RpcErrorGetNextRecord(RPC_ERROR_ENUM_HANDLE*,BOOL,RPC_EXTENDED_ERROR_INFO*);
RPCRTAPI RPC_STATUS RPC_ENTRY RpcErrorEndEnumeration(RPC_ERROR_ENUM_HANDLE*);
RPCRTAPI RPC_STATUS RPC_ENTRY RpcErrorResetEnumeration(RPC_ERROR_ENUM_HANDLE*);
RPCRTAPI RPC_STATUS RPC_ENTRY RpcErrorGetNumberOfRecords(RPC_ERROR_ENUM_HANDLE*,int*);
RPCRTAPI RPC_STATUS RPC_ENTRY RpcErrorSaveErrorInfo(RPC_ERROR_ENUM_HANDLE*,PVOID*,size_t*);
RPCRTAPI RPC_STATUS RPC_ENTRY RpcErrorLoadErrorInfo(PVOID,size_t,RPC_ERROR_ENUM_HANDLE*);
RPCRTAPI RPC_STATUS RPC_ENTRY RpcErrorAddRecord(RPC_EXTENDED_ERROR_INFO*);
RPCRTAPI void RPC_ENTRY RpcErrorClearInformation(void);

#if !defined(_KRPCENV_)
RPCRTAPI RPC_STATUS RPC_ENTRY RpcAsyncCleanupThread(DWORD);
RPCRTAPI RPC_STATUS RPC_ENTRY RpcGetAuthorizationContextForClient(RPC_BINDING_HANDLE,BOOL,PVOID,PLARGE_INTEGER,LUID,DWORD,PVOID,PVOID*);
RPCRTAPI RPC_STATUS RPC_ENTRY RpcFreeAuthorizationContext(PVOID*);
RPCRTAPI RPC_STATUS RPC_ENTRY RpcSsContextLockExclusive(RPC_BINDING_HANDLE,PVOID);
RPCRTAPI RPC_STATUS RPC_ENTRY RpcSsContextLockShared(RPC_BINDING_HANDLE,PVOID);

#if (NTDDI_VERSION >= NTDDI_VISTA)
typedef enum tagRpcLocalAddressFormat {
    rlafInvalid = 0,
    rlafIPv4,
    rlafIPv6
} RpcLocalAddressFormat;

typedef struct _RPC_CALL_LOCAL_ADDRESS_V1 {
    unsigned int Version;
    void *Buffer;
    unsigned long BufferSize;
    RpcLocalAddressFormat AddressFormat;
} RPC_CALL_LOCAL_ADDRESS_V1, *PRPC_CALL_LOCAL_ADDRESS_V1;

#if (NTDDI_VERSION >= NTDDI_VISTA) && (NTDDI_VERSION < NTDDI_WIN8)
#define RPC_CALL_ATTRIBUTES_VERSION  (2)
#endif /* (NTDDI_VERSION >= NTDDI_VISTA) && (NTDDI_VERSION < NTDDI_WIN8) */

#define RPC_QUERY_SERVER_PRINCIPAL_NAME  (0x02)
#define RPC_QUERY_CLIENT_PRINCIPAL_NAME  (0x04)
#define RPC_QUERY_CALL_LOCAL_ADDRESS  (0x08)
#define RPC_QUERY_CLIENT_PID  (0x10)
#define RPC_QUERY_IS_CLIENT_LOCAL  (0x20)
#define RPC_QUERY_NO_AUTH_REQUIRED  (0x40)
#endif /* NTDDI_VERSION >= NTDDI_VISTA */

#if (NTDDI_VERSION >= NTDDI_WIN8)
#define RPC_CALL_ATTRIBUTES_VERSION  (3)
#define RPC_QUERY_CLIENT_ID  (0x80)
#endif /* NTDDI_VERSION >= NTDDI_WIN8 */

#if (NTDDI_VERSION >= NTDDI_WINXP && NTDDI_VERSION < NTDDI_VISTA)
#define RPC_CALL_ATTRIBUTES_VERSION  (1)
#define RPC_QUERY_SERVER_PRINCIPAL_NAME  (2)
#define RPC_QUERY_CLIENT_PRINCIPAL_NAME  (4)
#endif /* NTDDI_VERSION >= NTDDI_WINXP && NTDDI_VERSION < NTDDI_VISTA */

typedef struct tagRPC_CALL_ATTRIBUTES_V1_W {
    unsigned int Version;
    unsigned long Flags;
    unsigned long ServerPrincipalNameBufferLength;
    unsigned short *ServerPrincipalName;
    unsigned long ClientPrincipalNameBufferLength;
    unsigned short *ClientPrincipalName;
    unsigned long AuthenticationLevel;
    unsigned long AuthenticationService;
    BOOL NullSession;
} RPC_CALL_ATTRIBUTES_V1_W;

typedef struct tagRPC_CALL_ATTRIBUTES_V1_A {
    unsigned int Version;
    unsigned long Flags;
    unsigned long ServerPrincipalNameBufferLength;
    unsigned char *ServerPrincipalName;
    unsigned long ClientPrincipalNameBufferLength;
    unsigned char *ClientPrincipalName;
    unsigned long AuthenticationLevel;
    unsigned long AuthenticationService;
    BOOL NullSession;
} RPC_CALL_ATTRIBUTES_V1_A;

#if (NTDDI_VERSION >= NTDDI_VISTA)
#define RPC_CALL_STATUS_CANCELLED  0x01
#define RPC_CALL_STATUS_DISCONNECTED  0x02

typedef enum tagRpcCallType {
    rctInvalid = 0,
    rctNormal,
    rctTraining,
    rctGuaranteed
} RpcCallType;

typedef enum tagRpcCallClientLocality {
    rcclInvalid = 0,
    rcclLocal,
    rcclRemote,
    rcclClientUnknownLocality
} RpcCallClientLocality;

typedef struct tagRPC_CALL_ATTRIBUTES_V2_W {
    unsigned int Version;
    unsigned long Flags;
    unsigned long ServerPrincipalNameBufferLength;
    unsigned short *ServerPrincipalName;
    unsigned long ClientPrincipalNameBufferLength;
    unsigned short *ClientPrincipalName;
    unsigned long AuthenticationLevel;
    unsigned long AuthenticationService;
    BOOL NullSession;
    BOOL KernelModeCaller;
    unsigned long ProtocolSequence;
    RpcCallClientLocality IsClientLocal;
    HANDLE ClientPID; 
    unsigned long CallStatus;
    RpcCallType CallType;
    RPC_CALL_LOCAL_ADDRESS_V1 *CallLocalAddress;
    unsigned short OpNum;
    UUID InterfaceUuid;
} RPC_CALL_ATTRIBUTES_V2_W;
    
typedef struct tagRPC_CALL_ATTRIBUTES_V2_A {
    unsigned int Version;
    unsigned long Flags;
    unsigned long ServerPrincipalNameBufferLength;
    unsigned char *ServerPrincipalName;
    unsigned long ClientPrincipalNameBufferLength;
    unsigned char *ClientPrincipalName;
    unsigned long AuthenticationLevel;
    unsigned long AuthenticationService;
    BOOL NullSession;
    BOOL KernelModeCaller;
    unsigned long ProtocolSequence;
    unsigned long IsClientLocal;
    HANDLE ClientPID;
    unsigned long CallStatus;
    RpcCallType CallType;
    RPC_CALL_LOCAL_ADDRESS_V1 *CallLocalAddress;
    unsigned short OpNum;
    UUID InterfaceUuid;
} RPC_CALL_ATTRIBUTES_V2_A;
#endif /* NTDDI_VERSION >= NTDDI_VISTA */

#if (NTDDI_VERSION >= NTDDI_WIN8)
typedef struct tagRPC_CALL_ATTRIBUTES_V3_W {
    unsigned int Version;
    unsigned long Flags;
    unsigned long ServerPrincipalNameBufferLength;
    unsigned short *ServerPrincipalName;
    unsigned long ClientPrincipalNameBufferLength;
    unsigned short *ClientPrincipalName;
    unsigned long AuthenticationLevel;
    unsigned long AuthenticationService;
    BOOL NullSession;
    BOOL KernelModeCaller;
    unsigned long ProtocolSequence;
    RpcCallClientLocality IsClientLocal;
    HANDLE ClientPID;
    unsigned long CallStatus;
    RpcCallType CallType;
    RPC_CALL_LOCAL_ADDRESS_V1 *CallLocalAddress;
    unsigned short OpNum;
    UUID InterfaceUuid;
    unsigned long ClientIdentifierBufferLength;
    unsigned char *ClientIdentifier;
} RPC_CALL_ATTRIBUTES_V3_W;

typedef struct tagRPC_CALL_ATTRIBUTES_V3_A {
    unsigned int Version;
    unsigned long Flags;
    unsigned long ServerPrincipalNameBufferLength;
    unsigned char *ServerPrincipalName;
    unsigned long ClientPrincipalNameBufferLength;
    unsigned char *ClientPrincipalName;
    unsigned long AuthenticationLevel;
    unsigned long AuthenticationService;
    BOOL NullSession;
    BOOL KernelModeCaller;
    unsigned long ProtocolSequence;
    unsigned long IsClientLocal;
    HANDLE ClientPID;
    unsigned long CallStatus;
    RpcCallType CallType;
    RPC_CALL_LOCAL_ADDRESS_V1 *CallLocalAddress;
    unsigned short OpNum;
    UUID InterfaceUuid;
    unsigned long ClientIdentifierBufferLength;
    unsigned char *ClientIdentifier;
} RPC_CALL_ATTRIBUTES_V3_A;
#endif /* NTDDI_VERSION >= NTDDI_WIN8 */

RPCRTAPI RPC_STATUS RPC_ENTRY RpcServerInqCallAttributesW(RPC_BINDING_HANDLE,void*);
RPCRTAPI RPC_STATUS RPC_ENTRY RpcServerInqCallAttributesA(RPC_BINDING_HANDLE,void*);

#ifdef UNICODE
#define RPC_CALL_ATTRIBUTES_V1 RPC_CALL_ATTRIBUTES_V1_W
#if (NTDDI_VERSION >= NTDDI_VISTA)
#define RPC_CALL_ATTRIBUTES_V2 RPC_CALL_ATTRIBUTES_V2_W
#endif /* NTDDI_VERSION >= NTDDI_VISTA */
#if (NTDDI_VERSION >= NTDDI_WIN8)
#define RPC_CALL_ATTRIBUTES_V3 RPC_CALL_ATTRIBUTES_V3_W
#endif /* NTDDI_VERSION >= NTDDI_WIN8 */
#define RpcServerInqCallAttributes RpcServerInqCallAttributesW
#else /* !UNICODE */
#define RPC_CALL_ATTRIBUTES_V1 RPC_CALL_ATTRIBUTES_V1_A
#if (NTDDI_VERSION >= NTDDI_VISTA)
#define RPC_CALL_ATTRIBUTES_V2 RPC_CALL_ATTRIBUTES_V2_A
#endif /* NTDDI_VERSION >= NTDDI_VISTA */
#if (NTDDI_VERSION >= NTDDI_WIN8)
#define RPC_CALL_ATTRIBUTES_V3 RPC_CALL_ATTRIBUTES_V3_A
#endif /* NTDDI_VERSION >= NTDDI_WIN8 */
#define RpcServerInqCallAttributes RpcServerInqCallAttributesA
#endif /* !UNICODE */

#if (NTDDI_VERSION >= NTDDI_WIN8)
typedef RPC_CALL_ATTRIBUTES_V3 RPC_CALL_ATTRIBUTES;
#elif (NTDDI_VERSION >= NTDDI_VISTA)
typedef RPC_CALL_ATTRIBUTES_V2 RPC_CALL_ATTRIBUTES;
#else
typedef RPC_CALL_ATTRIBUTES_V1 RPC_CALL_ATTRIBUTES;
#endif /* NTDDI_VERSION >= NTDDI_WIN8 */

#if (NTDDI_VERSION >= NTDDI_VISTA)
typedef enum _RPC_NOTIFICATIONS {
    RpcNotificationCallNone = 0,
    RpcNotificationClientDisconnect = 1,
    RpcNotificationCallCancel = 2
} RPC_NOTIFICATIONS;

#define RpcNotificationCallStatusChange  RpcNotificationClientDisconnect

RPCRTAPI RPC_STATUS RPC_ENTRY RpcServerSubscribeForNotification(RPC_BINDING_HANDLE,RPC_NOTIFICATIONS,RPC_NOTIFICATION_TYPES,RPC_ASYNC_NOTIFICATION_INFO*);
RPCRTAPI RPC_STATUS RPC_ENTRY RpcServerUnsubscribeForNotification(RPC_BINDING_HANDLE,RPC_NOTIFICATIONS,unsigned long*);
#endif /* NTDDI_VERSION >= NTDDI_VISTA */
#endif /* _KRPCENV_ */
 
#if (NTDDI_VERSION >= NTDDI_VISTA)  
RPCRTAPI RPC_STATUS RPC_ENTRY RpcBindingBind(PRPC_ASYNC_STATE,RPC_BINDING_HANDLE,RPC_IF_HANDLE);
RPCRTAPI RPC_STATUS RPC_ENTRY RpcBindingUnbind(RPC_BINDING_HANDLE);

#define RPC_DE_USE_CURRENT_EEINFO  (1)

RPCRTAPI RPC_STATUS RPC_ENTRY RpcDiagnoseError(RPC_BINDING_HANDLE,RPC_IF_HANDLE,RPC_STATUS,RPC_ERROR_ENUM_HANDLE*,ULONG,HWND);

#endif /* NTDDI_VERSION >= NTDDI_VISTA */
#endif /* NTDDI_VERSION >= NTDDI_WINXP */

RPC_STATUS RPC_ENTRY I_RpcAsyncSetHandle(PRPC_MESSAGE,PRPC_ASYNC_STATE);
RPC_STATUS RPC_ENTRY I_RpcAsyncAbortCall(PRPC_ASYNC_STATE,unsigned long);

#if (NTDDI_VERSION >= NTDDI_WXP)
int RPC_ENTRY I_RpcExceptionFilter(unsigned long);
#endif /* NTDDI_VERSION >= NTDDI_WXP */

#if (NTDDI_VERSION >= NTDDI_WIN7)
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcBindingInqClientTokenAttributes(RPC_BINDING_HANDLE,LUID*,LUID*,LUID*);
#endif /* NTDDI_VERSION >= NTDDI_WIN7 */

#ifdef __cplusplus
}
#endif

#if defined(__RPC_WIN64__)
#include <poppack.h>
#endif

#endif /* _RPCASYNC_H */
