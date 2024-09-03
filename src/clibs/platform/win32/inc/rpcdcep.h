#ifndef _RPCDCEP_H
#define _RPCDCEP_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows Private RPC runtime API definition */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _RPC_VERSION {
    unsigned short MajorVersion;
    unsigned short MinorVersion;
} RPC_VERSION;

typedef struct _RPC_SYNTAX_IDENTIFIER {
    GUID SyntaxGUID;
    RPC_VERSION SyntaxVersion;
} RPC_SYNTAX_IDENTIFIER, *PRPC_SYNTAX_IDENTIFIER;

typedef struct _RPC_MESSAGE {
    RPC_BINDING_HANDLE Handle;
    unsigned long DataRepresentation;
    void *Buffer;
    unsigned int BufferLength;
    unsigned int ProcNum;
    PRPC_SYNTAX_IDENTIFIER TransferSyntax;
    void *RpcInterfaceInformation;
    void *ReservedForRuntime;
    RPC_MGR_EPV *ManagerEpv;
    void *ImportContext;
    unsigned long RpcFlags;
} RPC_MESSAGE, *PRPC_MESSAGE;


typedef RPC_STATUS RPC_ENTRY RPC_FORWARD_FUNCTION(UUID *, RPC_VERSION *, UUID *, unsigned char *, void **);

enum RPC_ADDRESS_CHANGE_TYPE {
    PROTOCOL_NOT_LOADED = 1,
    PROTOCOL_LOADED,
    PROTOCOL_ADDRESS_CHANGE
};

typedef void RPC_ENTRY RPC_ADDRESS_CHANGE_FN(void *);

#define RPC_CONTEXT_HANDLE_DEFAULT_GUARD  ((void *)(ULONG_PTR)0xFFFFF00D)

#define RPC_CONTEXT_HANDLE_DEFAULT_FLAGS  0x00000000UL
#define RPC_CONTEXT_HANDLE_FLAGS  0x30000000UL
#define RPC_CONTEXT_HANDLE_SERIALIZE  0x10000000UL
#define RPC_CONTEXT_HANDLE_DONT_SERIALIZE  0x20000000UL
#if (NTDDI_VERSION >= NTDDI_VISTA)
#define RPC_TYPE_STRICT_CONTEXT_HANDLE  0x40000000UL
#endif /* NTDDI_VERSION >= NTDDI_VISTA */

#define RPC_NCA_FLAGS_DEFAULT  0x00000000
#define RPC_NCA_FLAGS_IDEMPOTENT  0x00000001
#define RPC_NCA_FLAGS_BROADCAST  0x00000002
#define RPC_NCA_FLAGS_MAYBE  0x00000004

#if (NTDDI_VERSION >= NTDDI_VISTA)
#define RPCFLG_HAS_GUARANTEE  0x00000010UL
#endif /* NTDDI_VERSION >= NTDDI_VISTA */

#define RPC_BUFFER_COMPLETE  0x00001000
#define RPC_BUFFER_PARTIAL  0x00002000
#define RPC_BUFFER_EXTRA  0x00004000
#define RPC_BUFFER_ASYNC  0x00008000
#define RPC_BUFFER_NONOTIFY  0x00010000


#define RPCFLG_MESSAGE  0x01000000UL
#define RPCFLG_AUTO_COMPLETE  0x08000000UL
#define RPCFLG_LOCAL_CALL  0x10000000UL
#define RPCFLG_INPUT_SYNCHRONOUS  0x20000000UL
#define RPCFLG_ASYNCHRONOUS  0x40000000UL
#define RPCFLG_NON_NDR  0x80000000UL

#if (NTDDI_VERSION >= NTDDI_WINXP)
#define RPCFLG_HAS_MULTI_SYNTAXES  0x02000000UL
#define RPCFLG_HAS_CALLBACK  0x04000000UL
#endif /* NTDDI_VERSION >= NTDDI_WXP */

#if (NTDDI_VERSION >= NTDDI_VISTA)
#define RPCFLG_ACCESSIBILITY_BIT1  0x00100000UL
#define RPCFLG_ACCESSIBILITY_BIT2  0x00200000UL
#define RPCFLG_ACCESS_LOCAL  0x00400000UL

#define NDR_CUSTOM_OR_DEFAULT_ALLOCATOR  0x10000000UL
#define NDR_DEFAULT_ALLOCATOR  0x20000000UL
#endif /* NTDDI_VERSION >= NTDDI_VISTA */

#if (NTDDI_VERSION >= NTDDI_WINBLUE)
#define RPCFLG_SENDER_WAITING_FOR_REPLY 0x00800000UL 
#endif /* NTDDI_VERSION >= NTDDI_WINBLUE */

#define RPC_FLAGS_VALID_BIT  0x00008000

#define NT351_INTERFACE_SIZE  0x40
#define RPC_INTERFACE_HAS_PIPES  0x0001

typedef void (__RPC_STUB * RPC_DISPATCH_FUNCTION)(PRPC_MESSAGE);

typedef struct {
    unsigned int DispatchTableCount;
    RPC_DISPATCH_FUNCTION *DispatchTable;
    LONG_PTR Reserved;
} RPC_DISPATCH_TABLE, *PRPC_DISPATCH_TABLE;

typedef struct _RPC_PROTSEQ_ENDPOINT {
    unsigned char *RpcProtocolSequence;
    unsigned char *Endpoint;
} RPC_PROTSEQ_ENDPOINT, *PRPC_PROTSEQ_ENDPOINT;

typedef struct _RPC_SERVER_INTERFACE {
    unsigned int Length;
    RPC_SYNTAX_IDENTIFIER InterfaceId;
    RPC_SYNTAX_IDENTIFIER TransferSyntax;
    PRPC_DISPATCH_TABLE DispatchTable;
    unsigned int RpcProtseqEndpointCount;
    PRPC_PROTSEQ_ENDPOINT RpcProtseqEndpoint;
    RPC_MGR_EPV *DefaultManagerEpv;
    void const *InterpreterInfo;
    unsigned int Flags;
} RPC_SERVER_INTERFACE, *PRPC_SERVER_INTERFACE;

typedef struct _RPC_CLIENT_INTERFACE {
    unsigned int Length;
    RPC_SYNTAX_IDENTIFIER InterfaceId;
    RPC_SYNTAX_IDENTIFIER TransferSyntax;
    PRPC_DISPATCH_TABLE DispatchTable;
    unsigned int RpcProtseqEndpointCount;
    PRPC_PROTSEQ_ENDPOINT RpcProtseqEndpoint;
    ULONG_PTR Reserved;
    void const *InterpreterInfo;
    unsigned int Flags;
} RPC_CLIENT_INTERFACE, *PRPC_CLIENT_INTERFACE;

#if (NTDDI_VERSION >= NTDDI_WINXP)
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcNegotiateTransferSyntax(RPC_MESSAGE *);
#endif /* NTDDI_VERSION >= NTDDI_WINXP */

RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcGetBuffer(RPC_MESSAGE *);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcGetBufferWithObject(RPC_MESSAGE *, UUID *);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcSendReceive(RPC_MESSAGE *);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcFreeBuffer(RPC_MESSAGE *);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcSend(PRPC_MESSAGE);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcReceive(PRPC_MESSAGE, unsigned int);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcFreePipeBuffer(RPC_MESSAGE *);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcReallocPipeBuffer(PRPC_MESSAGE, unsigned int);

typedef void * I_RPC_MUTEX;

RPCRTAPI void RPC_ENTRY I_RpcRequestMutex(I_RPC_MUTEX *);
RPCRTAPI void RPC_ENTRY I_RpcClearMutex(I_RPC_MUTEX);
RPCRTAPI void RPC_ENTRY I_RpcDeleteMutex(I_RPC_MUTEX);
RPCRTAPI void *RPC_ENTRY I_RpcAllocate(unsigned int);
RPCRTAPI void RPC_ENTRY I_RpcFree(void *);
RPCRTAPI void RPC_ENTRY I_RpcPauseExecution(unsigned long);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcGetExtendedError(void);

typedef void (__RPC_USER * PRPC_RUNDOWN)(void *);

RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcMonitorAssociation(RPC_BINDING_HANDLE, PRPC_RUNDOWN, void *);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcStopMonitorAssociation(RPC_BINDING_HANDLE);
RPCRTAPI RPC_BINDING_HANDLE RPC_ENTRY I_RpcGetCurrentCallHandle(void);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcGetAssociationContext(RPC_BINDING_HANDLE, void **);
RPCRTAPI void *RPC_ENTRY I_RpcGetServerContextList(RPC_BINDING_HANDLE);
RPCRTAPI void RPC_ENTRY I_RpcSetServerContextList(RPC_BINDING_HANDLE, void *);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcNsInterfaceExported(unsigned long, unsigned short *, RPC_SERVER_INTERFACE *);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcNsInterfaceUnexported(unsigned long, unsigned short *, RPC_SERVER_INTERFACE *);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcBindingToStaticStringBindingW(RPC_BINDING_HANDLE, unsigned short **);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcBindingInqSecurityContext(RPC_BINDING_HANDLE, void **);

#if (NTDDI_VERSION >= NTDDI_VISTA)
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcBindingInqSecurityContextKeyInfo(RPC_BINDING_HANDLE, void *);
#endif /* NTDDI_VERSION >= NTDDI_VISTA */

RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcBindingInqWireIdForSnego(RPC_BINDING_HANDLE, RPC_CSTR);

#if (NTDDI_VERSION >= NTDDI_WS03)
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcBindingInqMarshalledTargetInfo(RPC_BINDING_HANDLE, unsigned long *, RPC_CSTR *);
#endif /* NTDDI_VERSION >= NTDDI_WS03 */

#if (NTDDI_VERSION >= WINXP)
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcBindingInqLocalClientPID(RPC_BINDING_HANDLE, unsigned long *);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcBindingHandleToAsyncHandle(RPC_BINDING_HANDLE, void **);
#endif /* NTDDI_VERSION >= WINXP */

RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcNsBindingSetEntryNameW(RPC_BINDING_HANDLE, unsigned long, RPC_WSTR);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcNsBindingSetEntryNameA(RPC_BINDING_HANDLE, unsigned long, RPC_CSTR);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcServerUseProtseqEp2A(RPC_CSTR, RPC_CSTR, unsigned int, RPC_CSTR, void *, void *);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcServerUseProtseqEp2W(RPC_WSTR, RPC_WSTR, unsigned int, RPC_WSTR, void *, void *);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcServerUseProtseq2W(RPC_WSTR, RPC_WSTR, unsigned int, void *, void *);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcServerUseProtseq2A(RPC_CSTR, RPC_CSTR, unsigned int, void *, void *);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcServerStartService(RPC_WSTR, RPC_WSTR, RPC_IF_HANDLE);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcBindingInqDynamicEndpointW(RPC_BINDING_HANDLE, RPC_WSTR *);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcBindingInqDynamicEndpointA(RPC_BINDING_HANDLE, RPC_CSTR *);

#if (NTDDI_VERSION >= NTDDI_WINXP)
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcServerCheckClientRestriction(RPC_BINDING_HANDLE);
#endif /* NTDDI_VERSION >= NTDDI_WINXP */

#define TRANSPORT_TYPE_CN  0x01
#define TRANSPORT_TYPE_DG  0x02
#define TRANSPORT_TYPE_LPC  0x04
#define TRANSPORT_TYPE_WMSG  0x08

RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcBindingInqTransportType(RPC_BINDING_HANDLE, unsigned int *);

typedef struct _RPC_TRANSFER_SYNTAX {
    UUID Uuid;
    unsigned short VersMajor;
    unsigned short VersMinor;
} RPC_TRANSFER_SYNTAX;

RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcIfInqTransferSyntaxes(RPC_IF_HANDLE, RPC_TRANSFER_SYNTAX *, unsigned int, unsigned int *);
RPCRTAPI RPC_STATUS RPC_ENTRY I_UuidCreate(UUID *);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcBindingCopy(RPC_BINDING_HANDLE, RPC_BINDING_HANDLE *);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcBindingIsClientLocal(RPC_BINDING_HANDLE, unsigned int *);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcBindingInqConnId(RPC_BINDING_HANDLE, void **, int *);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcBindingCreateNP(RPC_WSTR, RPC_WSTR, RPC_WSTR, RPC_BINDING_HANDLE *);
RPCRTAPI void RPC_ENTRY I_RpcSsDontSerializeContext(void);

RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcLaunchDatagramReceiveThread(void *);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcServerRegisterForwardFunction(RPC_FORWARD_FUNCTION *);
RPC_ADDRESS_CHANGE_FN *RPC_ENTRY I_RpcServerInqAddressChangeFn(void);
RPC_STATUS RPC_ENTRY I_RpcServerSetAddressChangeFn(RPC_ADDRESS_CHANGE_FN *);

#if (NTDDI_VERSION >= WINXP)
#define RPC_P_ADDR_FORMAT_TCP_IPV4  1
#define RPC_P_ADDR_FORMAT_TCP_IPV6  2

RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcServerInqLocalConnAddress(RPC_BINDING_HANDLE, void *, unsigned long *, unsigned long *);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcServerInqRemoteConnAddress(RPC_BINDING_HANDLE, void *, unsigned long *, unsigned long *);
RPCRTAPI void RPC_ENTRY I_RpcSessionStrictContextHandle(void);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcTurnOnEEInfoPropagation(void);
#endif /* NTDDI_VERSION >= NTDDI_WINXP */

RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcConnectionInqSockBuffSize(unsigned long *, unsigned long *);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcConnectionSetSockBuffSize(unsigned long, unsigned long);

typedef void (*RPCLT_PDU_FILTER_FUNC)(void *, unsigned int, int);
typedef void (__cdecl * RPC_SETFILTER_FUNC)(RPCLT_PDU_FILTER_FUNC);

#ifndef WINNT
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcServerStartListening(void *);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcServerStopListening(void);

typedef RPC_STATUS(*RPC_BLOCKING_FN) (void *, void *, void *);

RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcBindingSetAsync(RPC_BINDING_HANDLE, RPC_BLOCKING_FN, unsigned long);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcSetThreadParams(int, void *, void *);
RPCRTAPI unsigned int RPC_ENTRY I_RpcWindowProc(void *, unsigned int, unsigned int, unsigned long);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcServerUnregisterEndpointA(RPC_CSTR, RPC_CSTR);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcServerUnregisterEndpointW(RPC_WSTR, RPC_WSTR);

#ifdef UNICODE
#define I_RpcServerUnregisterEndpoint I_RpcServerUnregisterEndpointW
#else
#define I_RpcServerUnregisterEndpoint I_RpcServerUnregisterEndpointA
#endif
#endif /* WINNT */

RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcServerInqTransportType(unsigned int *);
RPCRTAPI long RPC_ENTRY I_RpcMapWin32Status(RPC_STATUS);

#if (NTDDI_VERSION >= NTDDI_WS03)

#define RPC_C_OPT_SESSION_ID  (6)
#define RPC_C_OPT_COOKIE_AUTH  (7)
#define RPC_C_OPT_RESOURCE_TYPE_UUID  (8)

typedef struct _RPC_C_OPT_COOKIE_AUTH_DESCRIPTOR {
    unsigned long BufferSize;
    char *Buffer;
} RPC_C_OPT_COOKIE_AUTH_DESCRIPTOR;

typedef struct _RDR_CALLOUT_STATE {
    RPC_STATUS LastError;
    void *LastEEInfo;
    RPC_HTTP_REDIRECTOR_STAGE LastCalledStage;
    unsigned short *ServerName;
    unsigned short *ServerPort;
    unsigned short *RemoteUser;
    unsigned short *AuthType;
    unsigned char ResourceTypePresent;
    unsigned char SessionIdPresent;
    unsigned char InterfacePresent;
    UUID ResourceType;
    UUID SessionId;
    RPC_SYNTAX_IDENTIFIER Interface;
    void *CertContext;
} RDR_CALLOUT_STATE;

#endif /* NTDDI_VERSION >= NTDDI_WS03 */

#if (NTDDI_VERSION >= NTDDI_WINXP)
typedef RPC_STATUS(RPC_ENTRY * I_RpcProxyIsValidMachineFn) (RPC_WSTR, RPC_WSTR, unsigned long);
typedef RPC_STATUS(RPC_ENTRY * I_RpcProxyGetClientAddressFn) (void *, char *, unsigned long *);
typedef RPC_STATUS(RPC_ENTRY * I_RpcProxyGetConnectionTimeoutFn) (unsigned long *);

#if (NTDDI_VERSION >= NTDDI_WS03)
typedef RPC_STATUS(RPC_ENTRY * I_RpcPerformCalloutFn) (void *, RDR_CALLOUT_STATE *, RPC_HTTP_REDIRECTOR_STAGE);
typedef void (RPC_ENTRY * I_RpcFreeCalloutStateFn) (RDR_CALLOUT_STATE *);
typedef RPC_STATUS(RPC_ENTRY * I_RpcProxyGetClientSessionAndResourceUUID) (void *, int *, UUID *, int *, UUID *);
#endif /* NTDDI_VERSION >= NTDDI_WS03 */

#if (NTDDI_VERSION >= NTDDI_VISTA)
typedef RPC_STATUS(RPC_ENTRY * I_RpcProxyFilterIfFn)(void *, UUID *, unsigned short, int *);

typedef enum RpcProxyPerfCounters {
    RpcCurrentUniqueUser = 1,
    RpcBackEndConnectionAttempts,
    RpcBackEndConnectionFailed,
    RpcRequestsPerSecond,
    RpcIncomingConnections,
    RpcIncomingBandwidth,
    RpcOutgoingBandwidth,
    RpcAttemptedLbsDecisions,
    RpcFailedLbsDecisions,
    RpcAttemptedLbsMessages,
    RpcFailedLbsMessages,
    RpcLastCounter
} RpcPerfCounters;

typedef void (RPC_ENTRY *I_RpcProxyUpdatePerfCounterFn)(RpcPerfCounters, int, unsigned long);
typedef void (RPC_ENTRY *I_RpcProxyUpdatePerfCounterBackendServerFn)(unsigned short *, int);

#endif /* NTDDI_VERSION >= NTDDI_VISTA */

#define RPC_PROXY_CONNECTION_TYPE_IN_PROXY  0
#define RPC_PROXY_CONNECTION_TYPE_OUT_PROXY  1

#if (NTDDI_VERSION >= NTDDI_WS03)
typedef struct tagI_RpcProxyCallbackInterface {
    I_RpcProxyIsValidMachineFn IsValidMachineFn;
    I_RpcProxyGetClientAddressFn GetClientAddressFn;
    I_RpcProxyGetConnectionTimeoutFn GetConnectionTimeoutFn;
    I_RpcPerformCalloutFn PerformCalloutFn;
    I_RpcFreeCalloutStateFn FreeCalloutStateFn;
    I_RpcProxyGetClientSessionAndResourceUUID GetClientSessionAndResourceUUIDFn;
#if (NTDDI_VERSION >= NTDDI_VISTA)
    I_RpcProxyFilterIfFn ProxyFilterIfFn;
    I_RpcProxyUpdatePerfCounterFn RpcProxyUpdatePerfCounterFn;
    I_RpcProxyUpdatePerfCounterBackendServerFn RpcProxyUpdatePerfCounterBackendServerFn;
#endif /* NTDDI_VERSION >= NTDDI_VISTA */
} I_RpcProxyCallbackInterface;

RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcProxyNewConnection(unsigned long, unsigned short *, unsigned short *, unsigned short *, void *, RDR_CALLOUT_STATE *, I_RpcProxyCallbackInterface *);
#else /* NTDDI_VERSION < NTDDI_WS03 */
typedef struct tagI_RpcProxyCallbackInterface {
    I_RpcProxyIsValidMachineFn IsValidMachineFn;
    I_RpcProxyGetClientAddressFn GetClientAddressFn;
    I_RpcProxyGetConnectionTimeoutFn GetConnectionTimeoutFn;
} I_RpcProxyCallbackInterface;

RPCRTAPI RPC_STATUS RPC_ENTRY  I_RpcProxyNewConnection(unsigned long, unsigned short *, unsigned short *, void *, I_RpcProxyCallbackInterface *);
#endif /* NTDDI_VERSION < NTDDI_WS03 */

#endif /* NTDDI_VERSION >= NTDDI_WINXP */

#if (NTDDI_VERSION >= NTDDI_WS03)
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcReplyToClientWithStatus(void *, RPC_STATUS);
RPCRTAPI void RPC_ENTRY I_RpcRecordCalloutFailure(RPC_STATUS, RDR_CALLOUT_STATE *, unsigned short *);
#endif /* NTDDI_VERSION >= NTDDI_WS03 */

#if (NTDDI_VERSION >= NTDDI_VISTA)
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcOpenClientProcess(RPC_BINDING_HANDLE,unsigned long,void**);
#endif /* NTDDI_VERSION >= NTDDI_VISTA */

#if (NTDDI_VERSION >= NTDDI_WIN7)
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcMgmtEnableDedicatedThreadPool(void);
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcGetDefaultSD(void**);
#endif /* NTDDI_VERSION >= NTDDI_WIN7 */

#if (NTDDI_VERSION >= NTDDI_WIN8)
RPCRTAPI RPC_STATUS RPC_ENTRY I_RpcBindingIsServerLocal(RPC_BINDING_HANDLE,unsigned int*);
RPC_STATUS RPC_ENTRY I_RpcBindingSetPrivateOption(RPC_BINDING_HANDLE,unsigned long,ULONG_PTR);
#define RPC_C_OPT_PRIVATE_SUPPRESS_WAKE  1
#endif /* NTDDI_VERSION >= NTDDI_WIN8 */

#if (NTDDI_VERSION >= NTDDI_WINBLUE)
RPC_STATUS RPC_ENTRY I_RpcServerSubscribeForDisconnectNotification(RPC_BINDING_HANDLE,void*);
RPC_STATUS RPC_ENTRY I_RpcServerGetAssociationID(RPC_BINDING_HANDLE,unsigned long*);
RPCRTAPI long RPC_ENTRY I_RpcServerDisableExceptionFilter(void);
#endif /* NTDDI_VERSION >= NTDDI_WINBLUE */

#ifdef UNICODE
#define I_RpcNsBindingSetEntryName I_RpcNsBindingSetEntryNameW
#define I_RpcServerUseProtseqEp2 I_RpcServerUseProtseqEp2W
#define I_RpcServerUseProtseq2 I_RpcServerUseProtseq2W
#define I_RpcBindingInqDynamicEndpoint I_RpcBindingInqDynamicEndpointW
#else /* !UNICODE */
#define I_RpcNsBindingSetEntryName I_RpcNsBindingSetEntryNameA
#define I_RpcServerUseProtseqEp2 I_RpcServerUseProtseqEp2A
#define I_RpcServerUseProtseq2 I_RpcServerUseProtseq2A
#define I_RpcBindingInqDynamicEndpoint I_RpcBindingInqDynamicEndpointA
#endif /* !UNICODE */

#ifdef __cplusplus
}
#endif

#endif /* _RPCDCEP_H */
