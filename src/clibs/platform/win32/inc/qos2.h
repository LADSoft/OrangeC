#ifndef _QOS2_H
#define _QOS2_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#include <ws2tcpip.h>
#include <mstcpip.h>

#if defined(__cplusplus)
#define ExternC extern "C"
#else
#define ExternC
#endif

#define QOS_OUTGOING_DEFAULT_MINIMUM_BANDWIDTH  0xFFFFFFFF

#define QOS_QUERYFLOW_FRESH  0x00000001
#define QOS_NON_ADAPTIVE_FLOW  0x00000002

typedef ULONG QOS_FLOWID, *PQOS_FLOWID;

typedef enum _QOS_TRAFFIC_TYPE {
    QOSTrafficTypeBestEffort = 0,
    QOSTrafficTypeBackground = 1,
    QOSTrafficTypeExcellentEffort = 2,
    QOSTrafficTypeAudioVideo = 3,
    QOSTrafficTypeVoice = 4,
    QOSTrafficTypeControl = 5
} QOS_TRAFFIC_TYPE, *PQOS_TRAFFIC_TYPE;

typedef enum _QOS_SET_FLOW {
    QOSSetTrafficType = 0,
    QOSSetOutgoingRate = 1,
    QOSSetOutgoingDSCPValue = 2
} QOS_SET_FLOW, *PQOS_SET_FLOW;

typedef struct _QOS_PACKET_PRIORITY {
    ULONG ConformantDSCPValue;
    ULONG NonConformantDSCPValue;
    ULONG ConformantL2Value;
    ULONG NonConformantL2Value;
} QOS_PACKET_PRIORITY, *PQOS_PACKET_PRIORITY;

typedef struct _QOS_FLOW_FUNDAMENTALS {
    BOOL BottleneckBandwidthSet;
    UINT64 BottleneckBandwidth;
    BOOL AvailableBandwidthSet;
    UINT64 AvailableBandwidth;
    BOOL RTTSet;
    UINT32 RTT;
} QOS_FLOW_FUNDAMENTALS, *PQOS_FLOW_FUNDAMENTALS;

typedef enum _QOS_FLOWRATE_REASON {
    QOSFlowRateNotApplicable = 0,
    QOSFlowRateContentChange = 1,
    QOSFlowRateCongestion = 2,
    QOSFlowRateHigherContentEncoding = 3,
    QOSFlowRateUserCaused = 4
} QOS_FLOWRATE_REASON, *PQOS_FLOWRATE_REASON;

typedef enum _QOS_SHAPING {
    QOSShapeOnly = 0,
    QOSShapeAndMark = 1,
    QOSUseNonConformantMarkings = 2
} QOS_SHAPING, *PQOS_SHAPING;

typedef struct _QOS_FLOWRATE_OUTGOING {
    UINT64 Bandwidth;
    QOS_SHAPING ShapingBehavior;
    QOS_FLOWRATE_REASON Reason;
} QOS_FLOWRATE_OUTGOING, *PQOS_FLOWRATE_OUTGOING;

typedef enum _QOS_QUERY_FLOW {
    QOSQueryFlowFundamentals = 0,
    QOSQueryPacketPriority = 1,
    QOSQueryOutgoingRate = 2
} QOS_QUERY_FLOW, *PQOS_QUERY_FLOW;

typedef enum _QOS_NOTIFY_FLOW {
    QOSNotifyCongested = 0,
    QOSNotifyUncongested = 1,
    QOSNotifyAvailable = 2
} QOS_NOTIFY_FLOW, *PQOS_NOTIFY_FLOW;

typedef struct _QOS_VERSION {
    USHORT MajorVersion;
    USHORT MinorVersion;
} QOS_VERSION, *PQOS_VERSION;

inline INT QOS_HEADER_OVERHEAD(INT af, INT protocol) {
    UINT32 overhead;

    if (af == AF_INET)
        overhead = 20;
    else
        overhead = 40;

    if (protocol == IPPROTO_TCP)
        overhead += 20;
    else
        overhead += 8;

    return overhead;
}

inline UINT64 QOS_ADD_OVERHEAD(INT af, INT protocol, UINT32 targetDataPacketSize, UINT64 dataRate) {
    UINT32 overhead;
    double d;
    UINT64 r;

    overhead = QOS_HEADER_OVERHEAD(af, protocol);

    overhead *= 8;
    targetDataPacketSize *= 8;

    d = (double)overhead;
    d /= (double)targetDataPacketSize;
    d *= (double)dataRate;

    r = dataRate;
    r += (UINT64)d;

    return r;
}

inline UINT64 QOS_SUBTRACT_OVERHEAD(INT af, INT protocol, UINT32 targetDataPacketSize, UINT64 dataRate) {
    UINT32 overhead;
    double d;
    UINT64 r;

    overhead = QOS_HEADER_OVERHEAD(af, protocol);

    overhead *= 8;
    targetDataPacketSize *= 8;

    d = (double)overhead;
    d /= (double)targetDataPacketSize + overhead;
    d *= (double)dataRate;

    r = dataRate;
    r -= (UINT64)d;

    return r;
}

ExternC BOOL WINAPI QOSCreateHandle(PQOS_VERSION, PHANDLE);
ExternC BOOL WINAPI QOSCloseHandle(HANDLE);
ExternC BOOL WINAPI QOSStartTrackingClient(HANDLE, PSOCKADDR, DWORD);
ExternC BOOL WINAPI QOSStopTrackingClient(HANDLE, PSOCKADDR, DWORD);
ExternC BOOL WINAPI QOSEnumerateFlows(HANDLE, PULONG, PVOID);
ExternC BOOL WINAPI QOSAddSocketToFlow(HANDLE, SOCKET, PSOCKADDR, QOS_TRAFFIC_TYPE, DWORD, PQOS_FLOWID);
ExternC BOOL WINAPI QOSRemoveSocketFromFlow(HANDLE, SOCKET, QOS_FLOWID, DWORD);
ExternC BOOL WINAPI QOSSetFlow(HANDLE, QOS_FLOWID, QOS_SET_FLOW, ULONG, PVOID, DWORD, LPOVERLAPPED);
ExternC BOOL WINAPI QOSQueryFlow(HANDLE, QOS_FLOWID, QOS_QUERY_FLOW, PULONG, PVOID, DWORD, LPOVERLAPPED);
ExternC BOOL WINAPI QOSNotifyFlow(HANDLE, QOS_FLOWID, QOS_NOTIFY_FLOW, PULONG, PVOID, DWORD, LPOVERLAPPED);
ExternC BOOL WINAPI QOSCancel(HANDLE QOSHandle, LPOVERLAPPED Overlapped);

#endif /* _QOS2_H */
