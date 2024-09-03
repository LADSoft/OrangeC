#ifndef _TRAFFIC_H
#define _TRAFFIC_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Traffic control interface API definitions */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CALLBACK
#define CALLBACK __stdcall
#endif

#ifndef APIENTRY
#define APIENTRY __stdcall
#endif

#define CURRENT_TCI_VERSION  0x0002

#define TC_NOTIFY_IFC_UP  1
#define TC_NOTIFY_IFC_CLOSE  2
#define TC_NOTIFY_IFC_CHANGE  3
#define TC_NOTIFY_PARAM_CHANGED 4
#define TC_NOTIFY_FLOW_CLOSE  5

#define TC_INVALID_HANDLE  ((HANDLE)0)

#define MAX_STRING_LENGTH  256

#define QOS_TRAFFIC_GENERAL_ID_BASE  4000

#define QOS_OBJECT_DS_CLASS  (0x00000001+QOS_TRAFFIC_GENERAL_ID_BASE)
#define QOS_OBJECT_TRAFFIC_CLASS  (0x00000002+QOS_TRAFFIC_GENERAL_ID_BASE)
#define QOS_OBJECT_DIFFSERV  (0x00000003+QOS_TRAFFIC_GENERAL_ID_BASE)
#define QOS_OBJECT_TCP_TRAFFIC  (0x00000004+QOS_TRAFFIC_GENERAL_ID_BASE)

typedef VOID (CALLBACK *TCI_NOTIFY_HANDLER)(HANDLE,HANDLE,ULONG,ULONG,ULONG,PVOID);
typedef VOID (CALLBACK *TCI_ADD_FLOW_COMPLETE_HANDLER)(HANDLE,ULONG);
typedef VOID (CALLBACK *TCI_MOD_FLOW_COMPLETE_HANDLER)(HANDLE,ULONG);
typedef VOID (CALLBACK *TCI_DEL_FLOW_COMPLETE_HANDLER)(HANDLE,ULONG);

typedef struct _TCI_CLIENT_FUNC_LIST {
    TCI_NOTIFY_HANDLER ClNotifyHandler;
    TCI_ADD_FLOW_COMPLETE_HANDLER ClAddFlowCompleteHandler;
    TCI_MOD_FLOW_COMPLETE_HANDLER ClModifyFlowCompleteHandler;
    TCI_DEL_FLOW_COMPLETE_HANDLER ClDeleteFlowCompleteHandler;
} TCI_CLIENT_FUNC_LIST, *PTCI_CLIENT_FUNC_LIST;

typedef struct _ADDRESS_LIST_DESCRIPTOR {
    ULONG MediaType;
    NETWORK_ADDRESS_LIST AddressList;
} ADDRESS_LIST_DESCRIPTOR, *PADDRESS_LIST_DESCRIPTOR;

typedef struct _TC_IFC_DESCRIPTOR {
    ULONG Length;
    LPWSTR pInterfaceName;
    LPWSTR pInterfaceID;
    ADDRESS_LIST_DESCRIPTOR AddressListDesc;
} TC_IFC_DESCRIPTOR, *PTC_IFC_DESCRIPTOR;

typedef struct _TC_SUPPORTED_INFO_BUFFER {
    USHORT InstanceIDLength;
    WCHAR InstanceID[MAX_STRING_LENGTH];
    ADDRESS_LIST_DESCRIPTOR AddrListDesc;
} TC_SUPPORTED_INFO_BUFFER, *PTC_SUPPORTED_INFO_BUFFER;

typedef struct _TC_GEN_FILTER {
    USHORT AddressType;
    ULONG PatternSize;
    PVOID Pattern;
    PVOID Mask;
} TC_GEN_FILTER, *PTC_GEN_FILTER;

typedef struct _TC_GEN_FLOW {
    FLOWSPEC SendingFlowspec;
    FLOWSPEC ReceivingFlowspec;
    ULONG TcObjectsLength;
    QOS_OBJECT_HDR TcObjects[1];
} TC_GEN_FLOW, *PTC_GEN_FLOW;

typedef struct _IP_PATTERN {
    ULONG Reserved1;
    ULONG Reserved2;
    ULONG SrcAddr;
    ULONG DstAddr;
    union {
        struct {
            USHORT s_srcport, s_dstport;
        } S_un_ports;
        struct {
            UCHAR s_type, s_code;
            USHORT filler;
        } S_un_icmp;
        ULONG S_Spi;
    } S_un;
    UCHAR ProtocolId;
    UCHAR Reserved3[3];
} IP_PATTERN, *PIP_PATTERN;

#define tcSrcPort S_un.S_un_ports.s_srcport
#define tcDstPort S_un.S_un_ports.s_dstport
#define tcIcmpType  S_un.S_un_icmp.s_type
#define tcIcmpCode  S_un.S_un_icmp.s_code
#define tcSpi  S_un.S_Spi

typedef struct _IPX_PATTERN {
    struct {
        ULONG NetworkAddress;
        UCHAR NodeAddress[6];
        USHORT Socket;
    } Src, Dest;
} IPX_PATTERN, *PIPX_PATTERN;

typedef struct _ENUMERATION_BUFFER {
    ULONG Length;
    ULONG OwnerProcessId;
    USHORT FlowNameLength;
    WCHAR FlowName[MAX_STRING_LENGTH];
    PTC_GEN_FLOW pFlow;
    ULONG NumberOfFilters;
    TC_GEN_FILTER GenericFilter[1];
} ENUMERATION_BUFFER, *PENUMERATION_BUFFER;

typedef struct _QOS_TRAFFIC_CLASS {
    QOS_OBJECT_HDR ObjectHdr;
    ULONG TrafficClass;
} QOS_TRAFFIC_CLASS, *LPQOS_TRAFFIC_CLASS;

typedef struct _QOS_DS_CLASS {
    QOS_OBJECT_HDR ObjectHdr;
    ULONG DSField;
} QOS_DS_CLASS, *LPQOS_DS_CLASS;

typedef struct _QOS_DIFFSERV {
    QOS_OBJECT_HDR ObjectHdr;
    ULONG DSFieldCount;
    UCHAR DiffservRule[1];
} QOS_DIFFSERV, *LPQOS_DIFFSERV;

typedef struct _QOS_DIFFSERV_RULE {
    UCHAR InboundDSField;
    UCHAR ConformingOutboundDSField;
    UCHAR NonConformingOutboundDSField;
    UCHAR ConformingUserPriority;
    UCHAR NonConformingUserPriority;
} QOS_DIFFSERV_RULE, *LPQOS_DIFFSERV_RULE;

typedef struct _QOS_TCP_TRAFFIC {
    QOS_OBJECT_HDR ObjectHdr;
} QOS_TCP_TRAFFIC, *LPQOS_TCP_TRAFFIC;

ULONG APIENTRY TcRegisterClient(ULONG,HANDLE,PTCI_CLIENT_FUNC_LIST,PHANDLE);
ULONG APIENTRY TcEnumerateInterfaces(HANDLE,PULONG,PTC_IFC_DESCRIPTOR);
ULONG APIENTRY TcOpenInterfaceA(LPSTR,HANDLE,HANDLE,PHANDLE);
ULONG APIENTRY TcOpenInterfaceW(LPWSTR,HANDLE,HANDLE,PHANDLE);
ULONG APIENTRY TcCloseInterface(HANDLE);
ULONG APIENTRY TcQueryInterface(HANDLE,LPGUID,BOOLEAN,PULONG,PVOID);
ULONG APIENTRY TcSetInterface(HANDLE,LPGUID,ULONG,PVOID);
ULONG APIENTRY TcQueryFlowA(LPSTR,LPGUID,PULONG,PVOID);
ULONG APIENTRY TcQueryFlowW(LPWSTR,LPGUID,PULONG,PVOID);
ULONG APIENTRY TcSetFlowA(LPSTR,LPGUID,ULONG,PVOID);
ULONG APIENTRY TcSetFlowW(LPWSTR,LPGUID,ULONG,PVOID);
ULONG APIENTRY TcAddFlow(HANDLE,HANDLE,ULONG,PTC_GEN_FLOW,PHANDLE);
ULONG APIENTRY TcGetFlowNameA(HANDLE,ULONG,LPSTR);
ULONG APIENTRY TcGetFlowNameW(HANDLE,ULONG,LPWSTR);
ULONG APIENTRY TcModifyFlow(HANDLE,PTC_GEN_FLOW);
ULONG APIENTRY TcAddFilter(HANDLE,PTC_GEN_FILTER,PHANDLE);
ULONG APIENTRY TcDeregisterClient(HANDLE);
ULONG APIENTRY TcDeleteFlow(HANDLE);
ULONG APIENTRY TcDeleteFilter(HANDLE);
ULONG APIENTRY TcEnumerateFlows(HANDLE,PHANDLE,PULONG,PULONG,PENUMERATION_BUFFER);

#ifdef UNICODE
#define TcOpenInterface  TcOpenInterfaceW
#define TcQueryFlow  TcQueryFlowW
#define TcSetFlow  TcSetFlowW
#define TcGetFlowName  TcGetFlowNameW
#else
#define TcOpenInterface  TcOpenInterfaceA
#define TcQueryFlow  TcQueryFlowA
#define TcSetFlow  TcSetFlowA
#define TcGetFlowName  TcGetFlowNameA
#endif /* UNICODE */

#ifdef __cplusplus
}
#endif

#endif /* _TRAFFIC_H */
