#ifndef _NETIOAPI_H
#define _NETIOAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Version agnostic IP helper API definitions */

#ifdef __cplusplus
extern "C" {
#endif


#ifndef ANY_SIZE
#define ANY_SIZE  1
#endif

#ifdef _IPHLPAPI_H

#define NETIO_STATUS  DWORD
#define NETIO_SUCCESS(x)  ((x) == NO_ERROR)
#define NETIOAPI_API_  WINAPI

#else /* !_IPHLPAPI_H */

#include <ws2def.h>
#include <ws2ipdef.h>
#include <ifdef.h>
#include <nldef.h>

#define NETIO_STATUS  NTSTATUS
#define NETIO_SUCCESS(x)  NT_SUCCESS(x)
#define NETIOAPI_API_  NTAPI

#endif /* _IPHLPAPI_H */

#define NETIOAPI_API  NETIO_STATUS NETIOAPI_API_

typedef enum _MIB_NOTIFICATION_TYPE {
    MibParameterNotification,
    MibAddInstance,
    MibDeleteInstance,
    MibInitialNotification,
} MIB_NOTIFICATION_TYPE, *PMIB_NOTIFICATION_TYPE;

#ifdef _WS2IPDEF_H
#include <ntddndis.h>

#define MIB_INVALID_TEREDO_PORT_NUMBER  0

typedef struct _MIB_IF_ROW2 {
    NET_LUID InterfaceLuid;
    NET_IFINDEX InterfaceIndex; 
    GUID InterfaceGuid;
    WCHAR Alias[IF_MAX_STRING_SIZE + 1]; 
    WCHAR Description[IF_MAX_STRING_SIZE + 1];
    ULONG PhysicalAddressLength;
    UCHAR PhysicalAddress[IF_MAX_PHYS_ADDRESS_LENGTH];
    UCHAR PermanentPhysicalAddress[IF_MAX_PHYS_ADDRESS_LENGTH];
    ULONG Mtu;
    IFTYPE Type;
    TUNNEL_TYPE TunnelType;
    NDIS_MEDIUM MediaType; 
    NDIS_PHYSICAL_MEDIUM PhysicalMediumType; 
    NET_IF_ACCESS_TYPE AccessType;
    NET_IF_DIRECTION_TYPE DirectionType;
    struct {
        BOOLEAN HardwareInterface : 1;
        BOOLEAN FilterInterface : 1;
        BOOLEAN ConnectorPresent : 1;
        BOOLEAN NotAuthenticated : 1;
        BOOLEAN NotMediaConnected : 1;
        BOOLEAN Paused : 1;
        BOOLEAN LowPower : 1;
        BOOLEAN EndPointInterface : 1;
    } InterfaceAndOperStatusFlags;
    IF_OPER_STATUS OperStatus;  
    NET_IF_ADMIN_STATUS AdminStatus;
    NET_IF_MEDIA_CONNECT_STATE MediaConnectState;
    NET_IF_NETWORK_GUID NetworkGuid;
    NET_IF_CONNECTION_TYPE ConnectionType; 
    ULONG64 TransmitLinkSpeed;
    ULONG64 ReceiveLinkSpeed;
    ULONG64 InOctets;
    ULONG64 InUcastPkts;
    ULONG64 InNUcastPkts;
    ULONG64 InDiscards;
    ULONG64 InErrors;
    ULONG64 InUnknownProtos;
    ULONG64 InUcastOctets;
    ULONG64 InMulticastOctets;
    ULONG64 InBroadcastOctets;
    ULONG64 OutOctets;
    ULONG64 OutUcastPkts;
    ULONG64 OutNUcastPkts;
    ULONG64 OutDiscards;
    ULONG64 OutErrors;
    ULONG64 OutUcastOctets;
    ULONG64 OutMulticastOctets;
    ULONG64 OutBroadcastOctets;
    ULONG64 OutQLen;
} MIB_IF_ROW2, *PMIB_IF_ROW2;

typedef struct _MIB_IF_TABLE2 {
    ULONG NumEntries;
    MIB_IF_ROW2 Table[ANY_SIZE];
} MIB_IF_TABLE2, *PMIB_IF_TABLE2;

typedef enum _MIB_IF_TABLE_LEVEL {
    MibIfTableNormal,
    MibIfTableRaw
} MIB_IF_TABLE_LEVEL, *PMIB_IF_TABLE_LEVEL;

typedef struct _MIB_IPINTERFACE_ROW {
    ADDRESS_FAMILY Family;
    NET_LUID InterfaceLuid;
    NET_IFINDEX InterfaceIndex;
    ULONG MaxReassemblySize;
    ULONG64 InterfaceIdentifier;
    ULONG MinRouterAdvertisementInterval;
    ULONG MaxRouterAdvertisementInterval;
    BOOLEAN AdvertisingEnabled;
    BOOLEAN ForwardingEnabled;
    BOOLEAN WeakHostSend;
    BOOLEAN WeakHostReceive;
    BOOLEAN UseAutomaticMetric;
    BOOLEAN UseNeighborUnreachabilityDetection;   
    BOOLEAN ManagedAddressConfigurationSupported;
    BOOLEAN OtherStatefulConfigurationSupported;
    BOOLEAN AdvertiseDefaultRoute;
    NL_ROUTER_DISCOVERY_BEHAVIOR RouterDiscoveryBehavior;
    ULONG DadTransmits;
    ULONG BaseReachableTime;
    ULONG RetransmitTime;
    ULONG PathMtuDiscoveryTimeout;
    NL_LINK_LOCAL_ADDRESS_BEHAVIOR LinkLocalAddressBehavior;
    ULONG LinkLocalAddressTimeout;
    ULONG ZoneIndices[ScopeLevelCount];
    ULONG SitePrefixLength;
    ULONG Metric;
    ULONG NlMtu;
    BOOLEAN Connected;
    BOOLEAN SupportsWakeUpPatterns;
    BOOLEAN SupportsNeighborDiscovery;
    BOOLEAN SupportsRouterDiscovery;
    ULONG ReachableTime;
    NL_INTERFACE_OFFLOAD_ROD TransmitOffload;
    NL_INTERFACE_OFFLOAD_ROD ReceiveOffload;
    BOOLEAN DisableDefaultRoutes;
} MIB_IPINTERFACE_ROW, *PMIB_IPINTERFACE_ROW;

typedef struct _MIB_IPINTERFACE_TABLE {
    ULONG NumEntries;
    MIB_IPINTERFACE_ROW Table[ANY_SIZE];
} MIB_IPINTERFACE_TABLE, *PMIB_IPINTERFACE_TABLE;

typedef struct _MIB_IFSTACK_ROW {
    NET_IFINDEX HigherLayerInterfaceIndex;
    NET_IFINDEX LowerLayerInterfaceIndex;
} MIB_IFSTACK_ROW, *PMIB_IFSTACK_ROW;

typedef struct _MIB_INVERTEDIFSTACK_ROW {
    NET_IFINDEX LowerLayerInterfaceIndex;
    NET_IFINDEX HigherLayerInterfaceIndex;
} MIB_INVERTEDIFSTACK_ROW, *PMIB_INVERTEDIFSTACK_ROW;

typedef struct _MIB_IFSTACK_TABLE {
    ULONG NumEntries;
    MIB_IFSTACK_ROW Table[ANY_SIZE];
} MIB_IFSTACK_TABLE, *PMIB_IFSTACK_TABLE;

typedef struct _MIB_INVERTEDIFSTACK_TABLE {
    ULONG NumEntries;
    MIB_INVERTEDIFSTACK_ROW Table[ANY_SIZE];
} MIB_INVERTEDIFSTACK_TABLE, *PMIB_INVERTEDIFSTACK_TABLE;

typedef VOID (*PIPINTERFACE_CHANGE_CALLBACK)(PVOID,PMIB_IPINTERFACE_ROW,MIB_NOTIFICATION_TYPE);

typedef struct _MIB_UNICASTIPADDRESS_ROW {
    SOCKADDR_INET Address;
    NET_LUID InterfaceLuid;
    NET_IFINDEX InterfaceIndex;
    NL_PREFIX_ORIGIN PrefixOrigin;
    NL_SUFFIX_ORIGIN SuffixOrigin;
    ULONG ValidLifetime;
    ULONG PreferredLifetime;
    UINT8 OnLinkPrefixLength;
    BOOLEAN SkipAsSource;
    NL_DAD_STATE DadState;
    SCOPE_ID ScopeId;
    LARGE_INTEGER CreationTimeStamp;
} MIB_UNICASTIPADDRESS_ROW, *PMIB_UNICASTIPADDRESS_ROW;

typedef struct _MIB_UNICASTIPADDRESS_TABLE {
    ULONG NumEntries;
    MIB_UNICASTIPADDRESS_ROW Table[ANY_SIZE];
} MIB_UNICASTIPADDRESS_TABLE, *PMIB_UNICASTIPADDRESS_TABLE;

typedef VOID (*PUNICAST_IPADDRESS_CHANGE_CALLBACK)(PVOID,PMIB_UNICASTIPADDRESS_ROW,MIB_NOTIFICATION_TYPE);

typedef VOID (*PSTABLE_UNICAST_IPADDRESS_TABLE_CALLBACK)(PVOID,PMIB_UNICASTIPADDRESS_TABLE);

typedef struct _MIB_ANYCASTIPADDRESS_ROW {
    SOCKADDR_INET Address;
    NET_LUID InterfaceLuid;
    NET_IFINDEX InterfaceIndex;
    SCOPE_ID ScopeId;
} MIB_ANYCASTIPADDRESS_ROW, *PMIB_ANYCASTIPADDRESS_ROW; 

typedef struct _MIB_ANYCASTIPADDRESS_TABLE {
    ULONG NumEntries;
    MIB_ANYCASTIPADDRESS_ROW Table[ANY_SIZE];
} MIB_ANYCASTIPADDRESS_TABLE, *PMIB_ANYCASTIPADDRESS_TABLE;

typedef struct _MIB_MULTICASTIPADDRESS_ROW {
    SOCKADDR_INET Address;
    NET_IFINDEX InterfaceIndex;
    NET_LUID InterfaceLuid;
    SCOPE_ID ScopeId;
} MIB_MULTICASTIPADDRESS_ROW, *PMIB_MULTICASTIPADDRESS_ROW;

typedef struct _MIB_MULTICASTIPADDRESS_TABLE {
    ULONG NumEntries;
    MIB_MULTICASTIPADDRESS_ROW Table[ANY_SIZE];
} MIB_MULTICASTIPADDRESS_TABLE, *PMIB_MULTICASTIPADDRESS_TABLE;

typedef struct _IP_ADDRESS_PREFIX {
    SOCKADDR_INET Prefix;
    UINT8 PrefixLength;
} IP_ADDRESS_PREFIX, *PIP_ADDRESS_PREFIX;

typedef struct _MIB_IPFORWARD_ROW2 {
    NET_LUID InterfaceLuid;
    NET_IFINDEX InterfaceIndex;
    IP_ADDRESS_PREFIX DestinationPrefix;
    SOCKADDR_INET NextHop;
    UCHAR SitePrefixLength;
    ULONG ValidLifetime;
    ULONG PreferredLifetime;
    ULONG Metric;
    NL_ROUTE_PROTOCOL Protocol;
    BOOLEAN Loopback;
    BOOLEAN AutoconfigureAddress;
    BOOLEAN Publish;
    BOOLEAN Immortal;
    ULONG Age;
    NL_ROUTE_ORIGIN Origin;
} MIB_IPFORWARD_ROW2, *PMIB_IPFORWARD_ROW2;

typedef struct _MIB_IPFORWARD_TABLE2 {
    ULONG NumEntries;
    MIB_IPFORWARD_ROW2 Table[ANY_SIZE];
} MIB_IPFORWARD_TABLE2, *PMIB_IPFORWARD_TABLE2;

typedef VOID (*PIPFORWARD_CHANGE_CALLBACK)(PVOID,PMIB_IPFORWARD_ROW2,MIB_NOTIFICATION_TYPE);

typedef struct _MIB_IPPATH_ROW {
    SOCKADDR_INET Source;
    SOCKADDR_INET Destination;
    NET_LUID InterfaceLuid;
    NET_IFINDEX InterfaceIndex;
    SOCKADDR_INET CurrentNextHop;
    ULONG PathMtu;
    ULONG RttMean;
    ULONG RttDeviation;
    union {
        ULONG LastReachable;
        ULONG LastUnreachable;
    };
    BOOLEAN IsReachable;
    ULONG64 LinkTransmitSpeed;
    ULONG64 LinkReceiveSpeed;
} MIB_IPPATH_ROW, *PMIB_IPPATH_ROW;

typedef struct _MIB_IPPATH_TABLE {
    ULONG NumEntries;
    MIB_IPPATH_ROW Table[ANY_SIZE];
} MIB_IPPATH_TABLE, *PMIB_IPPATH_TABLE;

typedef struct _MIB_IPNET_ROW2 {
    SOCKADDR_INET Address;
    NET_IFINDEX InterfaceIndex;
    NET_LUID InterfaceLuid;
    UCHAR PhysicalAddress[IF_MAX_PHYS_ADDRESS_LENGTH];
    ULONG PhysicalAddressLength;
    NL_NEIGHBOR_STATE State;
    union {
        struct {
            BOOLEAN IsRouter : 1;
            BOOLEAN IsUnreachable : 1;
        };
        UCHAR Flags;
    };
    union {
        ULONG LastReachable;
        ULONG LastUnreachable;
    } ReachabilityTime;
} MIB_IPNET_ROW2, *PMIB_IPNET_ROW2;

typedef struct _MIB_IPNET_TABLE2 {
    ULONG NumEntries;
    MIB_IPNET_ROW2 Table[ANY_SIZE];
} MIB_IPNET_TABLE2, *PMIB_IPNET_TABLE2;

typedef VOID (*PTEREDO_PORT_CHANGE_CALLBACK)(PVOID,USHORT,MIB_NOTIFICATION_TYPE);

NETIOAPI_API GetIfEntry2(PMIB_IF_ROW2);
NETIOAPI_API GetIfTable2(PMIB_IF_TABLE2*);
NETIOAPI_API GetIfTable2Ex(MIB_IF_TABLE_LEVEL,PMIB_IF_TABLE2*);
NETIOAPI_API GetIfStackTable(PMIB_IFSTACK_TABLE*);
NETIOAPI_API GetInvertedIfStackTable(PMIB_INVERTEDIFSTACK_TABLE*);
NETIOAPI_API GetIpInterfaceEntry(PMIB_IPINTERFACE_ROW);
NETIOAPI_API GetIpInterfaceTable(ADDRESS_FAMILY,PMIB_IPINTERFACE_TABLE*);
VOID NETIOAPI_API_ InitializeIpInterfaceEntry(PMIB_IPINTERFACE_ROW);
NETIOAPI_API NotifyIpInterfaceChange(ADDRESS_FAMILY,PIPINTERFACE_CHANGE_CALLBACK,PVOID,BOOLEAN,HANDLE*);
NETIOAPI_API SetIpInterfaceEntry(PMIB_IPINTERFACE_ROW);
NETIOAPI_API CreateUnicastIpAddressEntry(CONST MIB_UNICASTIPADDRESS_ROW*);
NETIOAPI_API DeleteUnicastIpAddressEntry(CONST MIB_UNICASTIPADDRESS_ROW*);
NETIOAPI_API GetUnicastIpAddressEntry(PMIB_UNICASTIPADDRESS_ROW);
NETIOAPI_API GetUnicastIpAddressTable(ADDRESS_FAMILY,PMIB_UNICASTIPADDRESS_TABLE*);
VOID NETIOAPI_API_ InitializeUnicastIpAddressEntry(PMIB_UNICASTIPADDRESS_ROW);
NETIOAPI_API NotifyUnicastIpAddressChange(ADDRESS_FAMILY,PUNICAST_IPADDRESS_CHANGE_CALLBACK,PVOID,BOOLEAN,HANDLE*);
NETIOAPI_API NotifyStableUnicastIpAddressTable(ADDRESS_FAMILY,PMIB_UNICASTIPADDRESS_TABLE*,PSTABLE_UNICAST_IPADDRESS_TABLE_CALLBACK,PVOID,HANDLE*);
NETIOAPI_API SetUnicastIpAddressEntry(CONST MIB_UNICASTIPADDRESS_ROW*);
NETIOAPI_API CreateAnycastIpAddressEntry(CONST MIB_ANYCASTIPADDRESS_ROW*);
NETIOAPI_API DeleteAnycastIpAddressEntry(CONST MIB_ANYCASTIPADDRESS_ROW*);
NETIOAPI_API GetAnycastIpAddressEntry(PMIB_ANYCASTIPADDRESS_ROW);
NETIOAPI_API GetAnycastIpAddressTable(ADDRESS_FAMILY,PMIB_ANYCASTIPADDRESS_TABLE*);
NETIOAPI_API GetMulticastIpAddressEntry(PMIB_MULTICASTIPADDRESS_ROW);
NETIOAPI_API GetMulticastIpAddressTable(ADDRESS_FAMILY,PMIB_MULTICASTIPADDRESS_TABLE*);
NETIOAPI_API CreateIpForwardEntry2(CONST MIB_IPFORWARD_ROW2*);
NETIOAPI_API DeleteIpForwardEntry2(CONST MIB_IPFORWARD_ROW2*);
NETIOAPI_API GetBestRoute2(NET_LUID*,NET_IFINDEX,CONST SOCKADDR_INET*,CONST SOCKADDR_INET*,ULONG,PMIB_IPFORWARD_ROW2,SOCKADDR_INET*);
NETIOAPI_API GetIpForwardEntry2(PMIB_IPFORWARD_ROW2);
NETIOAPI_API GetIpForwardTable2(ADDRESS_FAMILY,PMIB_IPFORWARD_TABLE2*);
VOID NETIOAPI_API_ InitializeIpForwardEntry(PMIB_IPFORWARD_ROW2);
NETIOAPI_API NotifyRouteChange2(ADDRESS_FAMILY,PIPFORWARD_CHANGE_CALLBACK,PVOID,BOOLEAN,HANDLE*);
NETIOAPI_API SetIpForwardEntry2(CONST MIB_IPFORWARD_ROW2*);
NETIOAPI_API FlushIpPathTable(ADDRESS_FAMILY);
NETIOAPI_API GetIpPathEntry(PMIB_IPPATH_ROW);
NETIOAPI_API GetIpPathTable(ADDRESS_FAMILY,PMIB_IPPATH_TABLE*);
NETIOAPI_API CreateIpNetEntry2(CONST MIB_IPNET_ROW2*);
NETIOAPI_API DeleteIpNetEntry2(CONST MIB_IPNET_ROW2*);
NETIOAPI_API FlushIpNetTable2(ADDRESS_FAMILY,NET_IFINDEX);
NETIOAPI_API GetIpNetEntry2(PMIB_IPNET_ROW2);
NETIOAPI_API GetIpNetTable2(ADDRESS_FAMILY,PMIB_IPNET_TABLE2*);
NETIOAPI_API ResolveIpNetEntry2(PMIB_IPNET_ROW2,CONST SOCKADDR_INET*);
NETIOAPI_API SetIpNetEntry2(PMIB_IPNET_ROW2);
NETIOAPI_API NotifyTeredoPortChange(PTEREDO_PORT_CHANGE_CALLBACK,PVOID,BOOLEAN,HANDLE*);
NETIOAPI_API GetTeredoPort(USHORT*);
NETIOAPI_API CancelMibChangeNotify2(HANDLE);
VOID NETIOAPI_API_ FreeMibTable(PVOID);
NETIOAPI_API CreateSortedAddressPairs(const PSOCKADDR_IN6,ULONG,const PSOCKADDR_IN6,ULONG,ULONG,PSOCKADDR_IN6_PAIR*,ULONG*);

#endif /* _WS2IPDEF_H */

#define IF_NAMESIZE  NDIS_IF_MAX_STRING_SIZE

NETIOAPI_API ConvertInterfaceNameToLuidA(CONST CHAR*,NET_LUID*);
NETIOAPI_API ConvertInterfaceNameToLuidW(CONST WCHAR*,NET_LUID*);
NETIOAPI_API ConvertInterfaceLuidToNameA(CONST NET_LUID*,PSTR,SIZE_T);
NETIOAPI_API ConvertInterfaceLuidToNameW(CONST NET_LUID*,PWSTR,SIZE_T);
NETIOAPI_API ConvertInterfaceLuidToIndex(CONST NET_LUID*,PNET_IFINDEX);
NETIOAPI_API ConvertInterfaceIndexToLuid(NET_IFINDEX,PNET_LUID);
NETIOAPI_API ConvertInterfaceLuidToAlias(CONST NET_LUID*,PWSTR,SIZE_T);
NETIOAPI_API ConvertInterfaceAliasToLuid(CONST WCHAR*,PNET_LUID);
NETIOAPI_API ConvertInterfaceLuidToGuid(CONST NET_LUID*,GUID*);
NETIOAPI_API ConvertInterfaceGuidToLuid(CONST GUID*,PNET_LUID);
NET_IFINDEX NETIOAPI_API_ if_nametoindex(PCSTR);
PCHAR NETIOAPI_API_ if_indextoname(NET_IFINDEX,PCHAR);
NET_IF_COMPARTMENT_ID NETIOAPI_API_ GetCurrentThreadCompartmentId(VOID);
NETIOAPI_API SetCurrentThreadCompartmentId(NET_IF_COMPARTMENT_ID);
NET_IF_COMPARTMENT_ID NETIOAPI_API_ GetSessionCompartmentId(ULONG);
NETIOAPI_API SetSessionCompartmentId(ULONG,NET_IF_COMPARTMENT_ID);
NETIOAPI_API GetNetworkInformation(CONST NET_IF_NETWORK_GUID*,PNET_IF_COMPARTMENT_ID,PULONG,PWCHAR,ULONG);
NETIOAPI_API SetNetworkInformation(CONST NET_IF_NETWORK_GUID*,NET_IF_COMPARTMENT_ID,CONST WCHAR*);
NETIOAPI_API ConvertLengthToIpv4Mask(ULONG,PULONG);
NETIOAPI_API ConvertIpv4MaskToLength(ULONG,PUINT8);


#ifdef __cplusplus
}
#endif

#endif /* _NETIOAPI_H_ */
