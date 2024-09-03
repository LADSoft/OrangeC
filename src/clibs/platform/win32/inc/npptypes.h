#ifndef _NPPTYPES_H
#define _NPPTYPES_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Microsoft Network Monitor type definitions */

#include <unknwn.h>

#ifndef LPBYTE
typedef BYTE *LPBYTE;
#endif

typedef const void *HBLOB;

#include <pshpack1.h>


#define MAC_TYPE_UNKNOWN  0
#define MAC_TYPE_ETHERNET  1
#define MAC_TYPE_TOKENRING  2
#define MAC_TYPE_FDDI  3

#define MACHINE_NAME_LENGTH  16
#define USER_NAME_LENGTH  32
#define ADAPTER_COMMENT_LENGTH  32

#define CONNECTION_FLAGS_WANT_CONVERSATION_STATS  0x00000001

#define MAX_NAME_SIZE  32
#define IP_ADDRESS_SIZE  4
#define MAC_ADDRESS_SIZE  6

#define MAX_ADDRESS_SIZE  10

#define ADDRESS_TYPE_ETHERNET  0
#define ADDRESS_TYPE_IP  1
#define ADDRESS_TYPE_IPX  2
#define ADDRESS_TYPE_TOKENRING  3
#define ADDRESS_TYPE_FDDI  4

#define ADDRESS_TYPE_XNS  5
#define ADDRESS_TYPE_ANY  6
#define ADDRESS_TYPE_ANY_GROUP  7
#define ADDRESS_TYPE_FIND_HIGHEST  8
#define ADDRESS_TYPE_VINES_IP  9
#define ADDRESS_TYPE_LOCAL_ONLY  10

#define ADDRESSTYPE_FLAGS_NORMALIZE  0x0001
#define ADDRESSTYPE_FLAGS_BIT_REVERSE  0x0002

#define ADDRESS_FLAGS_MATCH_DST  0x0001
#define ADDRESS_FLAGS_MATCH_SRC  0x0002
#define ADDRESS_FLAGS_EXCLUDE  0x0004
#define ADDRESS_FLAGS_DST_GROUP_ADDR  0x0008
#define ADDRESS_FLAGS_MATCH_BOTH  0x0003

#define MAX_ADDRESS_PAIRS  8

#define NETWORKINFO_FLAGS_PMODE_NOT_SUPPORTED  0x00000001
#define NETWORKINFO_FLAGS_REMOTE_NAL  0x00000004
#define NETWORKINFO_FLAGS_REMOTE_NAL_CONNECTED  0x00000008
#define NETWORKINFO_FLAGS_REMOTE_CARD  0x00000010
#define NETWORKINFO_FLAGS_RAS  0x00000020

#define MINIMUM_FRAME_SIZE  32

#define MAX_PATTERN_LENGTH  16

#define PATTERN_MATCH_FLAGS_NOT  0x00000001
#define PATTERN_MATCH_FLAGS_RESERVED_1  0x00000002
#define PATTERN_MATCH_FLAGS_PORT_SPECIFIED  0x00000008

#define OFFSET_BASIS_RELATIVE_TO_FRAME  0
#define OFFSET_BASIS_RELATIVE_TO_EFFECTIVE_PROTOCOL  1
#define OFFSET_BASIS_RELATIVE_TO_IPX  2
#define OFFSET_BASIS_RELATIVE_TO_IP  3

#define MAX_PATTERNS  4

#define TRIGGER_TYPE_PATTERN_MATCH  1
#define TRIGGER_TYPE_BUFFER_CONTENT  2
#define TRIGGER_TYPE_PATTERN_MATCH_THEN_BUFFER_CONTENT  3
#define TRIGGER_TYPE_BUFFER_CONTENT_THEN_PATTERN_MATCH  4

#define TRIGGER_FLAGS_FRAME_RELATIVE  0x00000000
#define TRIGGER_FLAGS_DATA_RELATIVE  0x00000001

#define TRIGGER_ACTION_NOTIFY  0x00
#define TRIGGER_ACTION_STOP  0x02
#define TRIGGER_ACTION_PAUSE  0x03

#define TRIGGER_BUFFER_FULL_25_PERCENT  0
#define TRIGGER_BUFFER_FULL_50_PERCENT  1
#define TRIGGER_BUFFER_FULL_75_PERCENT  2
#define TRIGGER_BUFFER_FULL_100_PERCENT  3

#ifndef MAX_PATH
#define MAX_PATH  260
#endif

#define CAPTUREFILTER_FLAGS_INCLUDE_ALL_SAPS  0x0001
#define CAPTUREFILTER_FLAGS_INCLUDE_ALL_ETYPES  0x0002
#define CAPTUREFILTER_FLAGS_TRIGGER  0x0004
#define CAPTUREFILTER_FLAGS_LOCAL_ONLY  0x0008
#define CAPTUREFILTER_FLAGS_DISCARD_COMMENTS  0x0010
#define CAPTUREFILTER_FLAGS_KEEP_RAW  0x0020
#define CAPTUREFILTER_FLAGS_INCLUDE_ALL  0x0003

#define BUFFER_FULL_25_PERCENT  0
#define BUFFER_FULL_50_PERCENT  1
#define BUFFER_FULL_75_PERCENT  2
#define BUFFER_FULL_100_PERCENT  3

#define LOW_PROTOCOL_IPX  OFFSET_BASIS_RELATIVE_TO_IPX
#define LOW_PROTOCOL_IP  OFFSET_BASIS_RELATIVE_TO_IP
#define LOW_PROTOCOL_UNKNOWN ((BYTE)-1)

#define STATIONSTATS_FLAGS_INITIALIZED  0x0001
#define STATIONSTATS_FLAGS_EVENTPOSTED  0x0002

#define STATIONSTATS_POOL_SIZE  100

#define SESSION_FLAGS_INITIALIZED  0x0001
#define SESSION_FLAGS_EVENTPOSTED  0x0002

#define SESSION_POOL_SIZE  100

#define STATIONQUERY_FLAGS_LOADED  0x0001
#define STATIONQUERY_FLAGS_RUNNING  0x0002
#define STATIONQUERY_FLAGS_CAPTURING  0x0004
#define STATIONQUERY_FLAGS_TRANSMITTING 0x0008

#define STATIONQUERY_VERSION_MINOR  0x01
#define STATIONQUERY_VERSION_MAJOR  0x02

#define MAX_SECURITY_BREACH_REASON_SIZE  100
#define MAX_SIGNATURE_LENGTH  128
#define MAX_USER_NAME_LENGTH  256

#define UPDATE_EVENT_TERMINATE_THREAD  0x00000000
#define UPDATE_EVENT_NETWORK_STATUS  0x00000001

#define UPDATE_EVENT_RTC_INTERVAL_ELAPSED  0x00000002
#define UPDATE_EVENT_RTC_FRAME_TABLE_FULL  0x00000003
#define UPDATE_EVENT_RTC_BUFFER_FULL  0x00000004

#define UPDATE_EVENT_TRIGGER_BUFFER_CONTENT  0x00000005
#define UPDATE_EVENT_TRIGGER_PATTERN_MATCH  0x00000006
#define UPDATE_EVENT_TRIGGER_BUFFER_PATTERN  0x00000007
#define UPDATE_EVENT_TRIGGER_PATTERN_BUFFER  0x00000008

#define UPDATE_EVENT_TRANSMIT_STATUS  0x00000009

#define UPDATE_EVENT_SECURITY_BREACH  0x0000000A

#define UPDATE_EVENT_REMOTE_FAILURE  0x0000000B

#define UPDATE_ACTION_TERMINATE_THREAD  0x00000000
#define UPDATE_ACTION_NOTIFY  0x00000001
#define UPDATE_ACTION_STOP_CAPTURE  0x00000002
#define UPDATE_ACTION_PAUSE_CAPTURE  0x00000003
#define UPDATE_ACTION_RTC_BUFFER_SWITCH  0x00000004

#define NETWORKSTATUS_STATE_VOID  0
#define NETWORKSTATUS_STATE_INIT  1
#define NETWORKSTATUS_STATE_CAPTURING  2
#define NETWORKSTATUS_STATE_PAUSED  3

#define NETWORKSTATUS_FLAGS_TRIGGER_PENDING  0x0001

#define ALERT_CODE_BEGIN_TRANSMIT  0

#define MAKE_WORD(l,h)  (((WORD)(l))|(((WORD)(h))<<8))
#define MAKE_LONG(l,h)  (((DWORD)(l))|(((DWORD)(h))<<16L))
#define MAKE_SIG(a,b,c,d)  MAKE_LONG(MAKE_WORD(a,b),MAKE_WORD(c,d))
#define BONE_PACKET_SIGNATURE  MAKE_SIG('R','T','S','S')

#define BONE_COMMAND_STATION_QUERY_REQUEST  0
#define BONE_COMMAND_STATION_QUERY_RESPONSE  1
#define BONE_COMMAND_ALERT  2
#define BONE_COMMAND_PERMISSION_CHECK  3
#define BONE_COMMAND_PERMISSION_RESPONSE  4
#define BONE_COMMAND_SECURITY_MONITOR_EVENT  5

#define MAX_SESSIONS  100
#define MAX_STATIONS  100

#define CAPTUREFILE_VERSION_MAJOR  2
#define CAPTUREFILE_VERSION_MINOR  0

#define MakeVersion(Major,Minor)  ((DWORD)MAKEWORD(Minor,Major))

#define GetCurrentVersion()  MakeVersion(CAPTUREFILE_VERSION_MAJOR,CAPTUREFILE_VERSION_MINOR)

#define NETMON_1_0_CAPTUREFILE_SIGNATURE  MAKE_IDENTIFIER('R','T','S','S')
#define NETMON_2_0_CAPTUREFILE_SIGNATURE  MAKE_IDENTIFIER('G','M','B','U')

#define DEFAULT_TR_AC  0x00
#define DEFAULT_TR_FC  0x40
#define DEFAULT_SAP  0xAA
#define DEFAULT_CONTROL  0x03
#define DEFAULT_ETHERTYPE  0x8419
#define DEFAULT_FDDI_FC  0x10

#define STATISTICS_VERSION_1_0  0x00000000
#define STATISTICS_VERSION_2_0  0x00000020

#define MAX_STATSFRAME_SIZE  sizeof(TRSTATFRAME)

#define STATS_FRAME_TYPE  103

typedef DWORD HNETWORK;

typedef struct _STATISTICS {
    __int64 TimeElapsed;
    DWORD TotalFramesCaptured;
    DWORD TotalBytesCaptured;
    DWORD TotalFramesFiltered;
    DWORD TotalBytesFiltered;
    DWORD TotalMulticastsFiltered;
    DWORD TotalBroadcastsFiltered;
    DWORD TotalFramesSeen;
    DWORD TotalBytesSeen;
    DWORD TotalMulticastsReceived;
    DWORD TotalBroadcastsReceived;
    DWORD TotalFramesDropped;
    DWORD TotalFramesDroppedFromBuffer;
    DWORD MacFramesReceived;
    DWORD MacCRCErrors;
    __int64 MacBytesReceivedEx;
    DWORD MacFramesDropped_NoBuffers;
    DWORD MacMulticastsReceived;
    DWORD MacBroadcastsReceived;
    DWORD MacFramesDropped_HwError;
} STATISTICS, *LPSTATISTICS;
#define STATISTICS_SIZE  sizeof(STATISTICS)

typedef struct _VINES_IP_ADDRESS {
    DWORD NetID;
    WORD SubnetID;
} VINES_IP_ADDRESS;
typedef VINES_IP_ADDRESS *LPVINES_IP_ADDRESS;
#define VINES_IP_ADDRESS_SIZE sizeof(VINES_IP_ADDRESS)

typedef struct _IPX_ADDRESS {
    BYTE Subnet[4];
    BYTE Address[6];
} IPX_ADDRESS;
typedef IPX_ADDRESS *LPIPX_ADDRESS;
#define IPX_ADDRESS_SIZE  sizeof(IPX_ADDRESS)

typedef IPX_ADDRESS XNS_ADDRESS, *LPXNS_ADDRESS;

typedef struct _ETHERNET_SRC_ADDRESS {
    BYTE RoutingBit:1;
    BYTE LocalBit:1;
    BYTE Byte0:6;
    BYTE Reserved[5];
} ETHERNET_SRC_ADDRESS, *LPETHERNET_SRC_ADDRESS;

typedef struct _ETHERNET_DST_ADDRESS {
    BYTE GroupBit:1;
    BYTE AdminBit:1;
    BYTE Byte0:6;
    BYTE Reserved[5];
} ETHERNET_DST_ADDRESS, *LPETHERNET_DST_ADDRESS;

typedef ETHERNET_SRC_ADDRESS FDDI_SRC_ADDRESS;
typedef ETHERNET_DST_ADDRESS FDDI_DST_ADDRESS;

typedef FDDI_SRC_ADDRESS *LPFDDI_SRC_ADDRESS;
typedef FDDI_DST_ADDRESS *LPFDDI_DST_ADDRESS;

typedef struct _TOKENRING_SRC_ADDRESS {
    BYTE Byte0:6;
    BYTE LocalBit:1;
    BYTE RoutingBit:1;
    BYTE Byte1;
    BYTE Byte2:7;
    BYTE Functional:1;
    BYTE Reserved[3];
} TOKENRING_SRC_ADDRESS, *LPTOKENRING_SRC_ADDRESS;

typedef struct _TOKENRING_DST_ADDRESS {
    BYTE Byte0:6;
    BYTE AdminBit:1;
    BYTE GroupBit:1;
    BYTE Reserved[5];
} TOKENRING_DST_ADDRESS, *LPTOKENRING_DST_ADDRESS;

typedef struct _ADDRESS {
    DWORD Type;
    union {
        BYTE MACAddress[MAC_ADDRESS_SIZE];
        BYTE IPAddress[IP_ADDRESS_SIZE];
        BYTE IPXRawAddress[IPX_ADDRESS_SIZE];
        IPX_ADDRESS IPXAddress;
        BYTE VinesIPRawAddress[VINES_IP_ADDRESS_SIZE];
        VINES_IP_ADDRESS VinesIPAddress;
        ETHERNET_SRC_ADDRESS EthernetSrcAddress;
        ETHERNET_DST_ADDRESS EthernetDstAddress;
        TOKENRING_SRC_ADDRESS TokenringSrcAddress;
        TOKENRING_DST_ADDRESS TokenringDstAddress;
        FDDI_SRC_ADDRESS FddiSrcAddress;
        FDDI_DST_ADDRESS FddiDstAddress;
    };
    WORD Flags;
} ADDRESS, *LPADDRESS;
#define ADDRESS_SIZE  sizeof(ADDRESS)

typedef struct _ADDRESSPAIR {
    WORD AddressFlags;
    WORD NalReserved;
    ADDRESS DstAddress;
    ADDRESS SrcAddress;
} ADDRESSPAIR, *LPADDRESSPAIR;
#define ADDRESSPAIR_SIZE  sizeof(ADDRESSPAIR)

typedef struct _ADDRESSTABLE {
    DWORD nAddressPairs;
    DWORD nNonMacAddressPairs;
    ADDRESSPAIR AddressPair[MAX_ADDRESS_PAIRS];
} ADDRESSTABLE, *LPADDRESSTABLE;
#define ADDRESSTABLE_SIZE  sizeof(ADDRESSTABLE)

typedef struct _NETWORKINFO {
    BYTE PermanentAddr[6];
    BYTE CurrentAddr[6];
    ADDRESS OtherAddress;
    DWORD LinkSpeed;
    DWORD MacType;
    DWORD MaxFrameSize;
    DWORD Flags;
    DWORD TimestampScaleFactor;
    BYTE NodeName[32];
    BOOL PModeSupported;
    BYTE Comment[ADAPTER_COMMENT_LENGTH];
} NETWORKINFO, *LPNETWORKINFO;
#define NETWORKINFO_SIZE  sizeof(NETWORKINFO)

typedef union {
    BYTE IPPort;
    WORD ByteSwappedIPXPort;
} GENERIC_PORT;

typedef struct _PATTERNMATCH {
    DWORD Flags;
    BYTE OffsetBasis;
    GENERIC_PORT Port;
    WORD Offset;
    WORD Length;
    BYTE PatternToMatch[MAX_PATTERN_LENGTH];
} PATTERNMATCH, *LPPATTERNMATCH;
#define PATTERNMATCH_SIZE sizeof(PATTERNMATCH)

typedef struct _ANDEXP {
    DWORD nPatternMatches;
    PATTERNMATCH PatternMatch[MAX_PATTERNS];
} ANDEXP, *LPANDEXP;
#define ANDEXP_SIZE  sizeof(ANDEXP)

typedef struct _EXPRESSION {
    DWORD nAndExps;
    ANDEXP AndExp[MAX_PATTERNS];
} EXPRESSION, *LPEXPRESSION;
#define EXPRESSION_SIZE  sizeof(EXPRESSION)

typedef struct _TRIGGER {
    BOOL TriggerActive;
    BYTE TriggerType;
    BYTE TriggerAction;
    DWORD TriggerFlags;
    PATTERNMATCH TriggerPatternMatch;
    DWORD TriggerBufferSize;
    DWORD TriggerReserved;
    char TriggerCommandLine[MAX_PATH];
} TRIGGER, *LPTRIGGER;
#define TRIGGER_SIZE  sizeof(TRIGGER)

typedef struct _CAPTUREFILTER {
    DWORD FilterFlags;
    LPBYTE lpSapTable;
    LPWORD lpEtypeTable;
    WORD nSaps;
    WORD nEtypes;
    LPADDRESSTABLE AddressTable;
    EXPRESSION FilterExpression;
    TRIGGER Trigger;
    DWORD nFrameBytesToCopy;
    DWORD Reserved;
} CAPTUREFILTER, *LPCAPTUREFILTER;
#define CAPTUREFILTER_SIZE  sizeof(CAPTUREFILTER)

typedef struct _FRAME {
    __int64 TimeStamp;
    DWORD FrameLength;
    DWORD nBytesAvail;
    BYTE MacFrame[];
} FRAME, *LPFRAME, UNALIGNED *ULPFRAME;
#define FRAME_SIZE  sizeof(FRAME)

typedef struct _FRAME_DESCRIPTOR {
    LPBYTE FramePointer;
    __int64 TimeStamp;
    DWORD FrameLength;
    DWORD nBytesAvail;
    WORD Etype;
    BYTE Sap;
    BYTE LowProtocol;
    WORD LowProtocolOffset;
    GENERIC_PORT HighPort;
    WORD HighProtocolOffset;
} FRAME_DESCRIPTOR, *LPFRAME_DESCRIPTOR;
#define FRAME_DESCRIPTOR_SIZE sizeof(FRAME_DESCRIPTOR)

typedef struct _FRAMETABLE {
    DWORD FrameTableLength;
    DWORD StartIndex;
    DWORD EndIndex;
    DWORD FrameCount;
    FRAME_DESCRIPTOR Frames[1];
} FRAMETABLE, *LPFRAMETABLE;

typedef struct _STATIONSTATS {
    DWORD NextStationStats;
    DWORD SessionPartnerList;
    DWORD Flags;
    BYTE StationAddress[6];
    WORD Pad;
    DWORD TotalPacketsReceived;
    DWORD TotalDirectedPacketsSent;
    DWORD TotalBroadcastPacketsSent;
    DWORD TotalMulticastPacketsSent;
    DWORD TotalBytesReceived;
    DWORD TotalBytesSent;
} STATIONSTATS, *LPSTATIONSTATS;
#define STATIONSTATS_SIZE  sizeof(STATIONSTATS)

typedef struct _SESSIONSTATS {
    DWORD NextSession;
    DWORD StationOwner;
    DWORD StationPartner;
    DWORD Flags;
    DWORD TotalPacketsSent;
} SESSIONSTATS, *LPSESSIONSTATS;
#define SESSIONSTATS_SIZE  sizeof(SESSIONSTATS)

typedef struct _OLDSTATIONQUERY {
    DWORD Flags;
    BYTE BCDVerMinor;
    BYTE BCDVerMajor;
    DWORD LicenseNumber;
    BYTE MachineName[MACHINE_NAME_LENGTH];
    BYTE UserName[USER_NAME_LENGTH];
    BYTE Reserved[32];
    BYTE AdapterAddress[6];
} OLDSTATIONQUERY, *LPOLDSTATIONQUERY;
#define OLDSTATIONQUERY_SIZE  sizeof(OLDSTATIONQUERY)

typedef struct _STATIONQUERY {
    DWORD Flags;
    BYTE BCDVerMinor;
    BYTE BCDVerMajor;
    DWORD LicenseNumber;
    BYTE MachineName[MACHINE_NAME_LENGTH];
    BYTE UserName[USER_NAME_LENGTH];
    BYTE Reserved[32];
    BYTE AdapterAddress[6];
    WCHAR WMachineName[MACHINE_NAME_LENGTH];
    WCHAR WUserName[USER_NAME_LENGTH];
} STATIONQUERY, *LPSTATIONQUERY;
#define STATIONQUERY_SIZE  sizeof(STATIONQUERY)

typedef struct _QUERYTABLE {
    DWORD nStationQueries;
    STATIONQUERY StationQuery[1];
} QUERYTABLE, *LPQUERYTABLE;
#define QUERYTABLE_SIZE  sizeof(QUERYTABLE)

#ifndef _LINK_
#define _LINK_
typedef struct _LINK *LPLINK;
typedef struct _LINK {
    LPLINK PrevLink;
    LPLINK NextLink;
} LINK;
#endif /* _LINK_ */

typedef struct _SECURITY_PERMISSION_CHECK {
    UINT Version;
    DWORD RandomNumber;
    BYTE MachineName[MACHINE_NAME_LENGTH];
    BYTE UserName[USER_NAME_LENGTH];
    UINT MacType;
    BYTE PermanentAdapterAddress[MAC_ADDRESS_SIZE];
    BYTE CurrentAdapterAddress[MAC_ADDRESS_SIZE];
    WCHAR WMachineName[MACHINE_NAME_LENGTH];
    WCHAR WUserName[USER_NAME_LENGTH];
} SECURITY_PERMISSION_CHECK, *LPSECURITY_PERMISSION_CHECK, UNALIGNED *ULPSECURITY_PERMISSION_CHECK;
#define SECURITY_PERMISSION_CHECK_SIZE  sizeof(SECURITY_PERMISSION_CHECK)

typedef struct _SECURITY_PERMISSION_RESPONSE {
    UINT Version;
    DWORD RandomNumber;
    BYTE MachineName[MACHINE_NAME_LENGTH];
    BYTE Address[MAC_ADDRESS_SIZE];
    BYTE UserName[MAX_USER_NAME_LENGTH];
    BYTE Reason[MAX_SECURITY_BREACH_REASON_SIZE];
    DWORD SignatureLength;
    BYTE Signature[MAX_SIGNATURE_LENGTH];
} SECURITY_PERMISSION_RESPONSE, *LPSECURITY_PERMISSION_RESPONSE, UNALIGNED *ULPSECURITY_PERMISSION_RESPONSE;
#define SECURITY_PERMISSION_RESPONSE_SIZE  sizeof(SECURITY_PERMISSION_RESPONSE)

typedef struct _UPDATE_EVENT {
    USHORT Event;
    DWORD Action;
    DWORD Status;
    DWORD Value;
    __int64 TimeStamp;
    LPVOID lpUserContext;
    LPVOID lpReserved;
    UINT FramesDropped;
    union {
        DWORD Reserved;
        LPFRAMETABLE lpFrameTable;
        SECURITY_PERMISSION_RESPONSE SecurityResponse;
    };
    LPSTATISTICS lpFinalStats;
} UPDATE_EVENT, *PUPDATE_EVENT;

typedef DWORD (WINAPI * LPNETWORKCALLBACKPROC)(UPDATE_EVENT);

typedef struct _NETWORKSTATUS {
    DWORD State;
    DWORD Flags;
} NETWORKSTATUS, *LPNETWORKSTATUS;
#define NETWORKSTATUS_SIZE  sizeof(NETWORKSTATUS)

typedef struct _BONEPACKET {
    DWORD Signature;
    BYTE Command;
    BYTE Flags;
    DWORD Reserved;
    WORD Length;
} BONEPACKET, *LPBONEPACKET;
#define BONEPACKET_SIZE  sizeof(BONEPACKET)

typedef struct _ALERT {
    DWORD AlertCode;
    WCHAR WMachineName[MACHINE_NAME_LENGTH];
    WCHAR WUserName[USER_NAME_LENGTH];
    union {
        BYTE Pad[32];
        DWORD nFramesToSend;
    };
} ALERT, *LPALERT;
#define ALERT_SIZE sizeof(ALERT)

typedef struct _STATISTICSPARAM {
    DWORD StatisticsSize;
    STATISTICS Statistics;
    DWORD StatisticsTableEntries;
    STATIONSTATS StatisticsTable[MAX_STATIONS];
    DWORD SessionTableEntries;
    SESSIONSTATS SessionTable[MAX_SESSIONS];
} STATISTICSPARAM, *LPSTATISTICSPARAM;
#define STATISTICSPARAM_SIZE  sizeof(STATISTICSPARAM)

typedef struct _CAPTUREFILE_HEADER_VALUES {
    DWORD Signature;
    BYTE BCDVerMinor;
    BYTE BCDVerMajor;
    WORD MacType;
    SYSTEMTIME TimeStamp;
    DWORD FrameTableOffset;
    DWORD FrameTableLength;
    DWORD UserDataOffset;
    DWORD UserDataLength;
    DWORD CommentDataOffset;
    DWORD CommentDataLength;
    DWORD StatisticsOffset;
    DWORD StatisticsLength;
    DWORD NetworkInfoOffset;
    DWORD NetworkInfoLength;
    DWORD ConversationStatsOffset;
    DWORD ConversationStatsLength;
} CAPTUREFILE_HEADER_VALUES, *LPCAPTUREFILE_HEADER_VALUES;
#define CAPTUREFILE_HEADER_VALUES_SIZE  sizeof(CAPTUREFILE_HEADER_VALUES)

typedef struct _CAPTUREFILE_HEADER {
    union {
        CAPTUREFILE_HEADER_VALUES ActualHeader;
        BYTE Buffer[CAPTUREFILE_HEADER_VALUES_SIZE];
    };
    BYTE Reserved[128 - CAPTUREFILE_HEADER_VALUES_SIZE];
} CAPTUREFILE_HEADER, *LPCAPTUREFILE_HEADER;
#define CAPTUREFILE_HEADER_SIZE  sizeof(CAPTUREFILE_HEADER)

typedef struct _EFRAMEHDR {
    BYTE SrcAddress[6];
    BYTE DstAddress[6];
    WORD Length;
    BYTE DSAP;
    BYTE SSAP;
    BYTE Control;
    BYTE ProtocolID[3];
    WORD EtherType;
} EFRAMEHDR;

typedef struct _TRFRAMEHDR {
    BYTE AC;
    BYTE FC;
    BYTE SrcAddress[6];
    BYTE DstAddress[6];
    BYTE DSAP;
    BYTE SSAP;
    BYTE Control;
    BYTE ProtocolID[3];
    WORD EtherType;
} TRFRAMEHDR;

typedef struct _FDDIFRAMEHDR {
    BYTE FC;
    BYTE SrcAddress[6];
    BYTE DstAddress[6];
    BYTE DSAP;
    BYTE SSAP;
    BYTE Control;
    BYTE ProtocolID[3];
    WORD EtherType;
} FDDIFRAMEHDR;

typedef struct _FDDISTATFRAME {
    __int64 TimeStamp;
    DWORD FrameLength;
    DWORD nBytesAvail;
    FDDIFRAMEHDR FrameHeader;
    BYTE FrameID[4];
    DWORD Flags;
    DWORD FrameType;
    WORD StatsDataLen;
    DWORD StatsVersion;
    STATISTICS Statistics;
} FDDISTATFRAME, *LPFDDISTATFRAME, UNALIGNED *ULPFDDISTATFRAME;
#define FDDISTATFRAME_SIZE  sizeof(FDDISTATFRAME)

typedef struct _TRSTATFRAME {
    __int64 TimeStamp;
    DWORD FrameLength;
    DWORD nBytesAvail;
    TRFRAMEHDR FrameHeader;
    BYTE FrameID[4];
    DWORD Flags;
    DWORD FrameType;
    WORD StatsDataLen;
    DWORD StatsVersion;
    STATISTICS Statistics;
} TRSTATFRAME, *LPTRSTATFRAME, UNALIGNED *ULPTRSTATFRAME;
#define TRSTATFRAME_SIZE  sizeof(TRSTATFRAME)

typedef struct _ESTATFRAME {
    __int64 TimeStamp;
    DWORD FrameLength;
    DWORD nBytesAvail;
    EFRAMEHDR FrameHeader;
    BYTE FrameID[4];
    DWORD Flags;
    DWORD FrameType;
    WORD StatsDataLen;
    DWORD StatsVersion;
    STATISTICS Statistics;
} ESTATFRAME, *LPESTATFRAME, UNALIGNED *ULPESTATFRAME;
#define ESTATFRAME_SIZE  sizeof(ESTATFRAME)


#include <poppack.h>

#endif /* _NPPTYPES_H */
