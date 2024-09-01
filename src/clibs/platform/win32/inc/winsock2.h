#ifndef _WINSOCK2_H
#define _WINSOCK2_H
#define _WINSOCK_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#if (!defined(_WIN64) && !defined(WIN32))
#include <pshpack4.h>
#define _NEED_POPPACK
#endif

#ifndef INCL_WINSOCK_API_PROTOTYPES
#define INCL_WINSOCK_API_PROTOTYPES  1
#endif

#ifndef INCL_WINSOCK_API_TYPEDEFS
#define INCL_WINSOCK_API_TYPEDEFS  0
#endif

#ifndef _WINDOWS_H
#include <windows.h>
#endif /* _WINDOWS_H */

#if !defined(MAKEWORD)
#define MAKEWORD(low,high)  ((WORD)(((BYTE)(low))|((WORD)((BYTE)(high)))<<8))
#endif

#ifndef WINSOCK_VERSION
#define WINSOCK_VERSION  MAKEWORD(2,2)
#endif

#ifndef WINSOCK_API_LINKAGE
#ifdef DECLSPEC_IMPORT
#define WINSOCK_API_LINKAGE DECLSPEC_IMPORT
#else
#define WINSOCK_API_LINKAGE
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;

#if(_WIN32_WINNT >= 0x0501)
typedef unsigned __int64 u_int64;
#endif

#include <ws2def.h>

typedef UINT_PTR SOCKET;

#ifndef FD_SETSIZE
#define FD_SETSIZE  64
#endif /* FD_SETSIZE */

typedef struct fd_set {
    u_int fd_count;
    SOCKET  fd_array[FD_SETSIZE];
} fd_set;

extern int PASCAL __WSAFDIsSet(SOCKET,fd_set*);

#define FD_CLR(fd, set) do { \
    u_int __i; \
    for (__i = 0; __i < ((fd_set *)(set))->fd_count ; __i++) { \
        if (((fd_set *)(set))->fd_array[__i] == fd) { \
            while (__i < ((fd_set *)(set))->fd_count-1) { \
                ((fd_set *)(set))->fd_array[__i] = \
                    ((fd_set *)(set))->fd_array[__i+1]; \
                __i++; \
            } \
            ((fd_set *)(set))->fd_count--; \
            break; \
        } \
    } \
} while(0)

#define FD_SET(fd, set) do { \
    u_int __i; \
    for (__i = 0; __i < ((fd_set *)(set))->fd_count; __i++) { \
        if (((fd_set *)(set))->fd_array[__i] == (fd)) { \
            break; \
        } \
    } \
    if (__i == ((fd_set *)(set))->fd_count) { \
        if (((fd_set *)(set))->fd_count < FD_SETSIZE) { \
            ((fd_set *)(set))->fd_array[__i] = (fd); \
            ((fd_set *)(set))->fd_count++; \
        } \
    } \
} while(0)

#define FD_ZERO(set) (((fd_set *)(set))->fd_count=0)
#define FD_ISSET(fd,set) __WSAFDIsSet((SOCKET)(fd),(fd_set*)(set))

#if !defined(_TIMEVAL_DEFINED)
#define _WINSOCK_H_TIMEVAL
struct timeval {
    long tv_sec;
    long tv_usec;
};
#endif

#define timerisset(tvp)  ((tvp)->tv_sec || (tvp)->tv_usec)
#define timercmp(tvp,uvp,cmp)  ((tvp)->tv_sec cmp (uvp)->tv_sec || (tvp)->tv_sec == (uvp)->tv_sec && (tvp)->tv_usec cmp (uvp)->tv_usec)
#define timerclear(tvp)  (tvp)->tv_sec = (tvp)->tv_usec = 0

#define IOCPARM_MASK  0x7f
#define IOC_VOID  0x20000000
#define IOC_OUT  0x40000000
#define IOC_IN  0x80000000
#define IOC_INOUT  (IOC_IN|IOC_OUT)
#define _IO(x,y)  (IOC_VOID|((x)<<8)|(y))
#define _IOR(x,y,t)  (IOC_OUT|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))
#define _IOW(x,y,t)  (IOC_IN|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))

#define FIONREAD  _IOR('f',127,u_long)
#define FIONBIO   _IOW('f',126,u_long)
#define FIOASYNC  _IOW('f',125,u_long)

#define SIOCSHIWAT  _IOW('s',0,u_long)
#define SIOCGHIWAT  _IOR('s',1,u_long)
#define SIOCSLOWAT  _IOW('s',2,u_long)
#define SIOCGLOWAT  _IOR('s',3,u_long)
#define SIOCATMARK  _IOR('s',7,u_long)

struct  hostent {
    char *h_name;
    char **h_aliases;
    short h_addrtype;
    short h_length;
    char **h_addr_list;
#define h_addr  h_addr_list[0]
};

struct  netent {
    char *n_name;
    char **n_aliases;
    short n_addrtype;
    u_long n_net;
};

struct  servent {
    char *s_name;
    char **s_aliases;
#ifdef _WIN64
    char *s_proto;
    short s_port;
#else
    short s_port;
    char *s_proto;
#endif /* !WIN64 */
};

struct  protoent {
    char *p_name;
    char **p_aliases;
    short p_proto;
};

#define IPPORT_ECHO  7
#define IPPORT_DISCARD  9
#define IPPORT_SYSTAT  11
#define IPPORT_DAYTIME  13
#define IPPORT_NETSTAT  15
#define IPPORT_FTP  21
#define IPPORT_TELNET  23
#define IPPORT_SMTP  25
#define IPPORT_TIMESERVER  37
#define IPPORT_NAMESERVER  42
#define IPPORT_WHOIS  43
#define IPPORT_MTP  57

#define IPPORT_TFTP  69
#define IPPORT_RJE  77
#define IPPORT_FINGER  79
#define IPPORT_TTYLINK  87
#define IPPORT_SUPDUP  95

#define IPPORT_EXECSERVER  512
#define IPPORT_LOGINSERVER  513
#define IPPORT_CMDSERVER  514
#define IPPORT_EFSSERVER  520

#define IPPORT_BIFFUDP  512
#define IPPORT_WHOSERVER  513
#define IPPORT_ROUTESERVER  520

#define IPPORT_RESERVED  1024

#define IMPLINK_IP  155
#define IMPLINK_LOWEXPER  156
#define IMPLINK_HIGHEXPER  158

#ifndef s_addr
struct in_addr {
    union {
        struct { u_char s_b1,s_b2,s_b3,s_b4; } S_un_b;
        struct { u_short s_w1,s_w2; } S_un_w;
        u_long S_addr;
    } S_un;
#define s_addr  S_un.S_addr
#define s_host  S_un.S_un_b.s_b2
#define s_net   S_un.S_un_b.s_b1
#define s_imp   S_un.S_un_w.s_w2
#define s_impno S_un.S_un_b.s_b4
#define s_lh    S_un.S_un_b.s_b3
};
#endif /* s_addr */

#define ADDR_ANY  INADDR_ANY

#define WSADESCRIPTION_LEN  256
#define WSASYS_STATUS_LEN  128

typedef struct WSAData {
    WORD wVersion;
    WORD wHighVersion;
#ifdef _WIN64
    unsigned short iMaxSockets;
    unsigned short iMaxUdpDg;
    char *lpVendorInfo;
    char szDescription[WSADESCRIPTION_LEN+1];
    char szSystemStatus[WSASYS_STATUS_LEN+1];
#else
    char szDescription[WSADESCRIPTION_LEN+1];
    char szSystemStatus[WSASYS_STATUS_LEN+1];
    unsigned short iMaxSockets;
    unsigned short iMaxUdpDg;
    char *lpVendorInfo;
#endif /* _WIN64 */
} WSADATA,* LPWSADATA;

#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR  (-1)

#define FROM_PROTOCOL_INFO  (-1)

#define SOCK_STREAM  1
#define SOCK_DGRAM  2
#define SOCK_RAW  3
#define SOCK_RDM  4
#define SOCK_SEQPACKET  5

#define SO_DEBUG  0x0001
#define SO_ACCEPTCONN  0x0002
#define SO_REUSEADDR  0x0004
#define SO_KEEPALIVE  0x0008
#define SO_DONTROUTE  0x0010
#define SO_BROADCAST  0x0020
#define SO_USELOOPBACK  0x0040
#define SO_LINGER  0x0080
#define SO_OOBINLINE  0x0100

#define SO_DONTLINGER  (int)(~SO_LINGER)
#define SO_EXCLUSIVEADDRUSE  ((int)(~SO_REUSEADDR))

#define SO_SNDBUF  0x1001
#define SO_RCVBUF  0x1002
#define SO_SNDLOWAT  0x1003
#define SO_RCVLOWAT  0x1004
#define SO_SNDTIMEO  0x1005
#define SO_RCVTIMEO  0x1006
#define SO_ERROR  0x1007
#define SO_TYPE  0x1008

#define SO_GROUP_ID  0x2001
#define SO_GROUP_PRIORITY  0x2002
#define SO_MAX_MSG_SIZE  0x2003
#define SO_PROTOCOL_INFOA  0x2004
#define SO_PROTOCOL_INFOW  0x2005
#ifdef UNICODE
#define SO_PROTOCOL_INFO  SO_PROTOCOL_INFOW
#else
#define SO_PROTOCOL_INFO  SO_PROTOCOL_INFOA
#endif /* !UNICODE */
#define PVD_CONFIG  0x3001
#define SO_CONDITIONAL_ACCEPT  0x3002

struct sockproto {
    u_short sp_family;
    u_short sp_protocol;
};

#define PF_UNSPEC  AF_UNSPEC
#define PF_UNIX  AF_UNIX
#define PF_INET  AF_INET
#define PF_IMPLINK  AF_IMPLINK
#define PF_PUP  AF_PUP
#define PF_CHAOS  AF_CHAOS
#define PF_NS  AF_NS
#define PF_IPX  AF_IPX
#define PF_ISO  AF_ISO
#define PF_OSI  AF_OSI
#define PF_ECMA  AF_ECMA
#define PF_DATAKIT  AF_DATAKIT
#define PF_CCITT  AF_CCITT
#define PF_SNA  AF_SNA
#define PF_DECnet  AF_DECnet
#define PF_DLI  AF_DLI
#define PF_LAT  AF_LAT
#define PF_HYLINK  AF_HYLINK
#define PF_APPLETALK  AF_APPLETALK
#define PF_VOICEVIEW  AF_VOICEVIEW
#define PF_FIREFOX  AF_FIREFOX
#define PF_UNKNOWN1  AF_UNKNOWN1
#define PF_BAN  AF_BAN
#define PF_ATM  AF_ATM
#define PF_INET6  AF_INET6
#if(_WIN32_WINNT >= 0x0600)
#define PF_BTH  AF_BTH
#endif  /* _WIN32_WINNT >= 0x0600 */
#define PF_MAX  AF_MAX

struct  linger {
    u_short l_onoff;
    u_short l_linger;
};

#define SOL_SOCKET  0xffff

#define SOMAXCONN  0x7fffffff

#define MSG_OOB  0x1
#define MSG_PEEK  0x2
#define MSG_DONTROUTE  0x4

#if(_WIN32_WINNT >= 0x0502)
#define MSG_WAITALL  0x8
#endif /* _WIN32_WINNT >= 0x0502 */

#define MSG_PARTIAL  0x8000

#define MSG_INTERRUPT  0x10

#define MSG_MAXIOVLEN  16

#define MAXGETHOSTSTRUCT  1024

#define FD_READ_BIT  0
#define FD_READ  (1<<FD_READ_BIT)

#define FD_WRITE_BIT  1
#define FD_WRITE  (1<<FD_WRITE_BIT)

#define FD_OOB_BIT  2
#define FD_OOB  (1<<FD_OOB_BIT)

#define FD_ACCEPT_BIT  3
#define FD_ACCEPT  (1<<FD_ACCEPT_BIT)

#define FD_CONNECT_BIT  4
#define FD_CONNECT  (1<<FD_CONNECT_BIT)

#define FD_CLOSE_BIT  5
#define FD_CLOSE  (1<<FD_CLOSE_BIT)

#define FD_QOS_BIT  6
#define FD_QOS  (1<<FD_QOS_BIT)

#define FD_GROUP_QOS_BIT  7
#define FD_GROUP_QOS  (1<<FD_GROUP_QOS_BIT)

#define FD_ROUTING_INTERFACE_CHANGE_BIT  8
#define FD_ROUTING_INTERFACE_CHANGE  (1<<FD_ROUTING_INTERFACE_CHANGE_BIT)

#define FD_ADDRESS_LIST_CHANGE_BIT  9
#define FD_ADDRESS_LIST_CHANGE  (1<<FD_ADDRESS_LIST_CHANGE_BIT)

#define FD_MAX_EVENTS  10
#define FD_ALL_EVENTS  ((1<<FD_MAX_EVENTS)-1)

#ifndef WSABASEERR
#define WSABASEERR  10000
#define WSAEINTR  (WSABASEERR+4)
#define WSAEBADF  (WSABASEERR+9)
#define WSAEACCES  (WSABASEERR+13)
#define WSAEFAULT  (WSABASEERR+14)
#define WSAEINVAL  (WSABASEERR+22)
#define WSAEMFILE  (WSABASEERR+24)
#define WSAEWOULDBLOCK  (WSABASEERR+35)
#define WSAEINPROGRESS  (WSABASEERR+36)
#define WSAEALREADY  (WSABASEERR+37)
#define WSAENOTSOCK  (WSABASEERR+38)
#define WSAEDESTADDRREQ  (WSABASEERR+39)
#define WSAEMSGSIZE  (WSABASEERR+40)
#define WSAEPROTOTYPE  (WSABASEERR+41)
#define WSAENOPROTOOPT  (WSABASEERR+42)
#define WSAEPROTONOSUPPORT  (WSABASEERR+43)
#define WSAESOCKTNOSUPPORT  (WSABASEERR+44)
#define WSAEOPNOTSUPP  (WSABASEERR+45)
#define WSAEPFNOSUPPORT  (WSABASEERR+46)
#define WSAEAFNOSUPPORT  (WSABASEERR+47)
#define WSAEADDRINUSE  (WSABASEERR+48)
#define WSAEADDRNOTAVAIL  (WSABASEERR+49)
#define WSAENETDOWN  (WSABASEERR+50)
#define WSAENETUNREACH  (WSABASEERR+51)
#define WSAENETRESET  (WSABASEERR+52)
#define WSAECONNABORTED  (WSABASEERR+53)
#define WSAECONNRESET  (WSABASEERR+54)
#define WSAENOBUFS  (WSABASEERR+55)
#define WSAEISCONN  (WSABASEERR+56)
#define WSAENOTCONN  (WSABASEERR+57)
#define WSAESHUTDOWN  (WSABASEERR+58)
#define WSAETOOMANYREFS  (WSABASEERR+59)
#define WSAETIMEDOUT  (WSABASEERR+60)
#define WSAECONNREFUSED  (WSABASEERR+61)
#define WSAELOOP  (WSABASEERR+62)
#define WSAENAMETOOLONG  (WSABASEERR+63)
#define WSAEHOSTDOWN  (WSABASEERR+64)
#define WSAEHOSTUNREACH  (WSABASEERR+65)
#define WSAENOTEMPTY  (WSABASEERR+66)
#define WSAEPROCLIM  (WSABASEERR+67)
#define WSAEUSERS  (WSABASEERR+68)
#define WSAEDQUOT  (WSABASEERR+69)
#define WSAESTALE  (WSABASEERR+70)
#define WSAEREMOTE  (WSABASEERR+71)
#define WSASYSNOTREADY  (WSABASEERR+91)
#define WSAVERNOTSUPPORTED  (WSABASEERR+92)
#define WSANOTINITIALISED  (WSABASEERR+93)
#define WSAEDISCON  (WSABASEERR+101)
#define WSAENOMORE  (WSABASEERR+102)
#define WSAECANCELLED  (WSABASEERR+103)
#define WSAEINVALIDPROCTABLE  (WSABASEERR+104)
#define WSAEINVALIDPROVIDER  (WSABASEERR+105)
#define WSAEPROVIDERFAILEDINIT  (WSABASEERR+106)
#define WSASYSCALLFAILURE  (WSABASEERR+107)
#define WSASERVICE_NOT_FOUND  (WSABASEERR+108)
#define WSATYPE_NOT_FOUND  (WSABASEERR+109)
#define WSA_E_NO_MORE  (WSABASEERR+110)
#define WSA_E_CANCELLED  (WSABASEERR+111)
#define WSAEREFUSED  (WSABASEERR+112)
#define WSAHOST_NOT_FOUND  (WSABASEERR+1001)
#define WSATRY_AGAIN  (WSABASEERR+1002)
#define WSANO_RECOVERY  (WSABASEERR+1003)
#define WSANO_DATA  (WSABASEERR+1004)
#define WSA_QOS_RECEIVERS  (WSABASEERR+1005)
#define WSA_QOS_SENDERS  (WSABASEERR+1006)
#define WSA_QOS_NO_SENDERS  (WSABASEERR+1007)
#define WSA_QOS_NO_RECEIVERS  (WSABASEERR+1008)
#define WSA_QOS_REQUEST_CONFIRMED  (WSABASEERR+1009)
#define WSA_QOS_ADMISSION_FAILURE  (WSABASEERR+1010)
#define WSA_QOS_POLICY_FAILURE  (WSABASEERR+1011)
#define WSA_QOS_BAD_STYLE  (WSABASEERR+1012)
#define WSA_QOS_BAD_OBJECT  (WSABASEERR+1013)
#define WSA_QOS_TRAFFIC_CTRL_ERROR  (WSABASEERR+1014)
#define WSA_QOS_GENERIC_ERROR  (WSABASEERR+1015)
#define WSA_QOS_ESERVICETYPE  (WSABASEERR+1016)
#define WSA_QOS_EFLOWSPEC  (WSABASEERR+1017)
#define WSA_QOS_EPROVSPECBUF  (WSABASEERR+1018)
#define WSA_QOS_EFILTERSTYLE  (WSABASEERR+1019)
#define WSA_QOS_EFILTERTYPE  (WSABASEERR+1020)
#define WSA_QOS_EFILTERCOUNT  (WSABASEERR+1021)
#define WSA_QOS_EOBJLENGTH  (WSABASEERR+1022)
#define WSA_QOS_EFLOWCOUNT  (WSABASEERR+1023)
#define WSA_QOS_EUNKOWNPSOBJ  (WSABASEERR+1024)
#define WSA_QOS_EPOLICYOBJ  (WSABASEERR+1025)
#define WSA_QOS_EFLOWDESC  (WSABASEERR+1026)
#define WSA_QOS_EPSFLOWSPEC  (WSABASEERR+1027)
#define WSA_QOS_EPSFILTERSPEC  (WSABASEERR+1028)
#define WSA_QOS_ESDMODEOBJ  (WSABASEERR+1029)
#define WSA_QOS_ESHAPERATEOBJ  (WSABASEERR+1030)
#define WSA_QOS_RESERVED_PETYPE  (WSABASEERR+1031)
#endif /* WSABASEERR */

#define h_errno  WSAGetLastError()
#define HOST_NOT_FOUND  WSAHOST_NOT_FOUND
#define TRY_AGAIN  WSATRY_AGAIN
#define NO_RECOVERY  WSANO_RECOVERY
#define NO_DATA  WSANO_DATA
#define WSANO_ADDRESS  WSANO_DATA
#define NO_ADDRESS  WSANO_ADDRESS


/* WinSock 2 extension */

#define WSAAPI  PASCAL
#define WSAEVENT  HANDLE
#define LPWSAEVENT  LPHANDLE
#define WSAOVERLAPPED  OVERLAPPED
typedef struct _OVERLAPPED *LPWSAOVERLAPPED;

#define WSA_IO_PENDING  (ERROR_IO_PENDING)
#define WSA_IO_INCOMPLETE  (ERROR_IO_INCOMPLETE)
#define WSA_INVALID_HANDLE  (ERROR_INVALID_HANDLE)
#define WSA_INVALID_PARAMETER  (ERROR_INVALID_PARAMETER)
#define WSA_NOT_ENOUGH_MEMORY  (ERROR_NOT_ENOUGH_MEMORY)
#define WSA_OPERATION_ABORTED  (ERROR_OPERATION_ABORTED)

#define WSA_INVALID_EVENT  ((WSAEVENT)NULL)
#define WSA_MAXIMUM_WAIT_EVENTS  (MAXIMUM_WAIT_OBJECTS)
#define WSA_WAIT_FAILED  (WAIT_FAILED)
#define WSA_WAIT_EVENT_0  (WAIT_OBJECT_0)
#define WSA_WAIT_IO_COMPLETION  (WAIT_IO_COMPLETION)
#define WSA_WAIT_TIMEOUT  (WAIT_TIMEOUT)
#define WSA_INFINITE  (INFINITE)

#include <qos.h>

typedef struct _QualityOfService {
    FLOWSPEC SendingFlowspec;
    FLOWSPEC ReceivingFlowspec;
    WSABUF ProviderSpecific;
} QOS, *LPQOS;

#define CF_ACCEPT  0x0000
#define CF_REJECT  0x0001
#define CF_DEFER  0x0002

#define SD_RECEIVE  0x00
#define SD_SEND  0x01
#define SD_BOTH  0x02

typedef unsigned int GROUP;

#define SG_UNCONSTRAINED_GROUP  0x01
#define SG_CONSTRAINED_GROUP  0x02

typedef struct _WSANETWORKEVENTS {
   long lNetworkEvents;
   int iErrorCode[FD_MAX_EVENTS];
} WSANETWORKEVENTS, *LPWSANETWORKEVENTS;

#ifndef GUID_DEFINED
#include <guiddef.h>
#endif /* GUID_DEFINED */

#define MAX_PROTOCOL_CHAIN  7

#define BASE_PROTOCOL  1
#define LAYERED_PROTOCOL  0

typedef struct _WSAPROTOCOLCHAIN {
    int ChainLen;
    DWORD ChainEntries[MAX_PROTOCOL_CHAIN];
} WSAPROTOCOLCHAIN, *LPWSAPROTOCOLCHAIN;

#define WSAPROTOCOL_LEN  255

typedef struct _WSAPROTOCOL_INFOA {
    DWORD dwServiceFlags1;
    DWORD dwServiceFlags2;
    DWORD dwServiceFlags3;
    DWORD dwServiceFlags4;
    DWORD dwProviderFlags;
    GUID ProviderId;
    DWORD dwCatalogEntryId;
    WSAPROTOCOLCHAIN ProtocolChain;
    int iVersion;
    int iAddressFamily;
    int iMaxSockAddr;
    int iMinSockAddr;
    int iSocketType;
    int iProtocol;
    int iProtocolMaxOffset;
    int iNetworkByteOrder;
    int iSecurityScheme;
    DWORD dwMessageSize;
    DWORD dwProviderReserved;
    CHAR   szProtocol[WSAPROTOCOL_LEN+1];
} WSAPROTOCOL_INFOA, *LPWSAPROTOCOL_INFOA;

typedef struct _WSAPROTOCOL_INFOW {
    DWORD dwServiceFlags1;
    DWORD dwServiceFlags2;
    DWORD dwServiceFlags3;
    DWORD dwServiceFlags4;
    DWORD dwProviderFlags;
    GUID ProviderId;
    DWORD dwCatalogEntryId;
    WSAPROTOCOLCHAIN ProtocolChain;
    int iVersion;
    int iAddressFamily;
    int iMaxSockAddr;
    int iMinSockAddr;
    int iSocketType;
    int iProtocol;
    int iProtocolMaxOffset;
    int iNetworkByteOrder;
    int iSecurityScheme;
    DWORD dwMessageSize;
    DWORD dwProviderReserved;
    WCHAR  szProtocol[WSAPROTOCOL_LEN+1];
} WSAPROTOCOL_INFOW, *LPWSAPROTOCOL_INFOW;

#ifdef UNICODE
typedef WSAPROTOCOL_INFOW WSAPROTOCOL_INFO;
typedef LPWSAPROTOCOL_INFOW LPWSAPROTOCOL_INFO;
#else
typedef WSAPROTOCOL_INFOA WSAPROTOCOL_INFO;
typedef LPWSAPROTOCOL_INFOA LPWSAPROTOCOL_INFO;
#endif /* UNICODE */

#define PFL_MULTIPLE_PROTO_ENTRIES  0x00000001
#define PFL_RECOMMENDED_PROTO_ENTRY  0x00000002
#define PFL_HIDDEN  0x00000004
#define PFL_MATCHES_PROTOCOL_ZERO  0x00000008

#define XP1_CONNECTIONLESS  0x00000001
#define XP1_GUARANTEED_DELIVERY  0x00000002
#define XP1_GUARANTEED_ORDER  0x00000004
#define XP1_MESSAGE_ORIENTED  0x00000008
#define XP1_PSEUDO_STREAM  0x00000010
#define XP1_GRACEFUL_CLOSE  0x00000020
#define XP1_EXPEDITED_DATA  0x00000040
#define XP1_CONNECT_DATA  0x00000080
#define XP1_DISCONNECT_DATA  0x00000100
#define XP1_SUPPORT_BROADCAST  0x00000200
#define XP1_SUPPORT_MULTIPOINT  0x00000400
#define XP1_MULTIPOINT_CONTROL_PLANE  0x00000800
#define XP1_MULTIPOINT_DATA_PLANE  0x00001000
#define XP1_QOS_SUPPORTED  0x00002000
#define XP1_INTERRUPT  0x00004000
#define XP1_UNI_SEND  0x00008000
#define XP1_UNI_RECV  0x00010000
#define XP1_IFS_HANDLES  0x00020000
#define XP1_PARTIAL_MESSAGE  0x00040000
#define XP1_SAN_SUPPORT_SDP  0x00080000

#define BIGENDIAN  0x0000
#define LITTLEENDIAN  0x0001

#define SECURITY_PROTOCOL_NONE  0x0000

#define JL_SENDER_ONLY  0x01
#define JL_RECEIVER_ONLY  0x02
#define JL_BOTH  0x04

#define WSA_FLAG_OVERLAPPED  0x01
#define WSA_FLAG_MULTIPOINT_C_ROOT  0x02
#define WSA_FLAG_MULTIPOINT_C_LEAF  0x04
#define WSA_FLAG_MULTIPOINT_D_ROOT  0x08
#define WSA_FLAG_MULTIPOINT_D_LEAF  0x10
#define WSA_FLAG_ACCESS_SYSTEM_SECURITY  0x40

typedef int (CALLBACK *LPCONDITIONPROC)(LPWSABUF,LPWSABUF,LPQOS,LPQOS,LPWSABUF,LPWSABUF,GROUP*,DWORD_PTR);
typedef void (CALLBACK *LPWSAOVERLAPPED_COMPLETION_ROUTINE)(DWORD,DWORD,LPWSAOVERLAPPED,DWORD);

#if (_WIN32_WINNT >= 0x0501)
#define SIO_NSP_NOTIFY_CHANGE  _WSAIOW(IOC_WS2,25)

typedef enum _WSACOMPLETIONTYPE {
    NSP_NOTIFY_IMMEDIATELY = 0,
    NSP_NOTIFY_HWND,
    NSP_NOTIFY_EVENT,
    NSP_NOTIFY_PORT,
    NSP_NOTIFY_APC,
} WSACOMPLETIONTYPE, *PWSACOMPLETIONTYPE, *LPWSACOMPLETIONTYPE;

typedef struct _WSACOMPLETION {
    WSACOMPLETIONTYPE Type;
    union {
        struct {
            HWND hWnd;
            UINT uMsg;
            WPARAM context;
        } WindowMessage;
        struct {
            LPWSAOVERLAPPED lpOverlapped;
        } Event;
        struct {
            LPWSAOVERLAPPED lpOverlapped;
            LPWSAOVERLAPPED_COMPLETION_ROUTINE lpfnCompletionProc;
        } Apc;
        struct {
            LPWSAOVERLAPPED lpOverlapped;
            HANDLE hPort;
            ULONG_PTR Key;
        } Port;
    } Parameters;
} WSACOMPLETION, *PWSACOMPLETION, *LPWSACOMPLETION;

#endif /* _WIN32_WINNT >= 0x0501 */

#define TH_NETDEV  0x00000001
#define TH_TAPI  0x00000002

#ifndef _tagBLOB_DEFINED
#define _tagBLOB_DEFINED
#define _BLOB_DEFINED
#define _LPBLOB_DEFINED
typedef struct _BLOB {
    ULONG cbSize;
    BYTE *pBlobData;
} BLOB, *LPBLOB;
#endif /* Bill Gates for president */

#define SERVICE_MULTIPLE  0x00000001

#define NS_ALL  0

#define NS_SAP  1
#define NS_NDS  2
#define NS_PEER_BROWSE  3
#define NS_SLP  5
#define NS_DHCP  6
#define NS_TCPIP_LOCAL  10
#define NS_TCPIP_HOSTS  11
#define NS_DNS  12
#define NS_NETBT  13
#define NS_WINS  14
#if(_WIN32_WINNT >= 0x0501)
#define NS_NLA  15
#endif /* _WIN32_WINNT >= 0x0501 */
#if(_WIN32_WINNT >= 0x0600)
#define NS_BTH  16
#endif /* _WIN32_WINNT >= 0x0600 */
#define NS_NBP  20
#define NS_MS  30
#define NS_STDA  31
#define NS_NTDS  32
#if(_WIN32_WINNT >= 0x0600)
#define NS_EMAIL  37
#define NS_PNRPNAME  38
#define NS_PNRPCLOUD  39
#endif /* (_WIN32_WINNT >= 0x0600) */
#define NS_X500  40
#define NS_NIS  41
#define NS_NISPLUS  42
#define NS_WRQ  50
#define NS_NETDES  60

#define RES_UNUSED_1  0x00000001
#define RES_FLUSH_CACHE  0x00000002
#ifndef RES_SERVICE
#define RES_SERVICE  0x00000004
#endif /* RES_SERVICE */

#define SERVICE_TYPE_VALUE_IPXPORTA  "IpxSocket"
#define SERVICE_TYPE_VALUE_IPXPORTW  L"IpxSocket"
#define SERVICE_TYPE_VALUE_SAPIDA  "SapId"
#define SERVICE_TYPE_VALUE_SAPIDW  L"SapId"

#define SERVICE_TYPE_VALUE_TCPPORTA  "TcpPort"
#define SERVICE_TYPE_VALUE_TCPPORTW  L"TcpPort"

#define SERVICE_TYPE_VALUE_UDPPORTA  "UdpPort"
#define SERVICE_TYPE_VALUE_UDPPORTW  L"UdpPort"

#define SERVICE_TYPE_VALUE_OBJECTIDA  "ObjectId"
#define SERVICE_TYPE_VALUE_OBJECTIDW  L"ObjectId"

#ifdef UNICODE
#define SERVICE_TYPE_VALUE_SAPID        SERVICE_TYPE_VALUE_SAPIDW
#define SERVICE_TYPE_VALUE_TCPPORT      SERVICE_TYPE_VALUE_TCPPORTW
#define SERVICE_TYPE_VALUE_UDPPORT      SERVICE_TYPE_VALUE_UDPPORTW
#define SERVICE_TYPE_VALUE_OBJECTID     SERVICE_TYPE_VALUE_OBJECTIDW
#else /* !UNICODE */
#define SERVICE_TYPE_VALUE_SAPID        SERVICE_TYPE_VALUE_SAPIDA
#define SERVICE_TYPE_VALUE_TCPPORT      SERVICE_TYPE_VALUE_TCPPORTA
#define SERVICE_TYPE_VALUE_UDPPORT      SERVICE_TYPE_VALUE_UDPPORTA
#define SERVICE_TYPE_VALUE_OBJECTID     SERVICE_TYPE_VALUE_OBJECTIDA
#endif /* !UNICODE */

typedef struct _AFPROTOCOLS {
    INT iAddressFamily;
    INT iProtocol;
} AFPROTOCOLS, *PAFPROTOCOLS, *LPAFPROTOCOLS;

/*
 * Client Query API Typedefs
 */

typedef enum _WSAEcomparator {
    COMP_EQUAL = 0,
    COMP_NOTLESS
} WSAECOMPARATOR, *PWSAECOMPARATOR, *LPWSAECOMPARATOR;

typedef struct _WSAVersion {
    DWORD dwVersion;
    WSAECOMPARATOR ecHow;
}WSAVERSION, *PWSAVERSION, *LPWSAVERSION;

typedef struct _WSAQuerySetA {
    DWORD dwSize;
    LPSTR lpszServiceInstanceName;
    LPGUID lpServiceClassId;
    LPWSAVERSION lpVersion;
    LPSTR lpszComment;
    DWORD dwNameSpace;
    LPGUID lpNSProviderId;
    LPSTR lpszContext;
    DWORD dwNumberOfProtocols;
    LPAFPROTOCOLS lpafpProtocols;
    LPSTR lpszQueryString;
    DWORD dwNumberOfCsAddrs;
    LPCSADDR_INFO lpcsaBuffer;
    DWORD dwOutputFlags;
    LPBLOB lpBlob;
} WSAQUERYSETA, *PWSAQUERYSETA, *LPWSAQUERYSETA;

typedef struct _WSAQuerySetW {
    DWORD dwSize;
    LPWSTR lpszServiceInstanceName;
    LPGUID lpServiceClassId;
    LPWSAVERSION lpVersion;
    LPWSTR lpszComment;
    DWORD dwNameSpace;
    LPGUID lpNSProviderId;
    LPWSTR lpszContext;
    DWORD dwNumberOfProtocols;
    LPAFPROTOCOLS lpafpProtocols;
    LPWSTR lpszQueryString;
    DWORD dwNumberOfCsAddrs;
    LPCSADDR_INFO lpcsaBuffer;
    DWORD dwOutputFlags;
    LPBLOB lpBlob;
} WSAQUERYSETW, *PWSAQUERYSETW, *LPWSAQUERYSETW;

typedef struct _WSAQuerySet2A {
    DWORD dwSize;
    LPSTR lpszServiceInstanceName;
    LPWSAVERSION lpVersion;
    LPSTR lpszComment;
    DWORD dwNameSpace;
    LPGUID lpNSProviderId;
    LPSTR lpszContext;
    DWORD dwNumberOfProtocols;
    LPAFPROTOCOLS lpafpProtocols;
    LPSTR lpszQueryString;
    DWORD dwNumberOfCsAddrs;
    LPCSADDR_INFO lpcsaBuffer;
    DWORD dwOutputFlags;
    LPBLOB lpBlob;
} WSAQUERYSET2A, *PWSAQUERYSET2A, *LPWSAQUERYSET2A;

typedef struct _WSAQuerySet2W {
    DWORD dwSize;
    LPWSTR lpszServiceInstanceName;
    LPWSAVERSION lpVersion;
    LPWSTR lpszComment;
    DWORD dwNameSpace;
    LPGUID lpNSProviderId;
    LPWSTR lpszContext;
    DWORD dwNumberOfProtocols;
    LPAFPROTOCOLS lpafpProtocols;
    LPWSTR lpszQueryString;
    DWORD dwNumberOfCsAddrs;
    LPCSADDR_INFO lpcsaBuffer;
    DWORD dwOutputFlags;
    LPBLOB lpBlob;
} WSAQUERYSET2W, *PWSAQUERYSET2W, *LPWSAQUERYSET2W;

#ifdef UNICODE
typedef WSAQUERYSETW WSAQUERYSET;
typedef PWSAQUERYSETW PWSAQUERYSET;
typedef LPWSAQUERYSETW LPWSAQUERYSET;
typedef WSAQUERYSET2W WSAQUERYSET2;
typedef PWSAQUERYSET2W PWSAQUERYSET2;
typedef LPWSAQUERYSET2W LPWSAQUERYSET2;
#else
typedef WSAQUERYSETA WSAQUERYSET;
typedef PWSAQUERYSETA PWSAQUERYSET;
typedef LPWSAQUERYSETA LPWSAQUERYSET;
typedef WSAQUERYSET2A WSAQUERYSET2;
typedef PWSAQUERYSET2A PWSAQUERYSET2;
typedef LPWSAQUERYSET2A LPWSAQUERYSET2;
#endif /* UNICODE */

#define LUP_DEEP  0x0001
#define LUP_CONTAINERS  0x0002
#define LUP_NOCONTAINERS  0x0004
#define LUP_NEAREST  0x0008
#define LUP_RETURN_NAME  0x0010
#define LUP_RETURN_TYPE  0x0020
#define LUP_RETURN_VERSION  0x0040
#define LUP_RETURN_COMMENT  0x0080
#define LUP_RETURN_ADDR  0x0100
#define LUP_RETURN_BLOB  0x0200
#define LUP_RETURN_ALIASES  0x0400
#define LUP_RETURN_QUERY_STRING  0x0800
#define LUP_RETURN_ALL  0x0FF0
#define LUP_RES_SERVICE  0x8000
#define LUP_FLUSHCACHE  0x1000
#define LUP_FLUSHPREVIOUS  0x2000
#define LUP_NON_AUTHORITATIVE  0x4000
#define LUP_SECURE  0x8000
#define LUP_RETURN_PREFERRED_NAMES  0x10000
#define LUP_ADDRCONFIG  0x00100000
#define LUP_DUAL_ADDR  0x00200000

#define RESULT_IS_ALIAS  0x0001
#if(_WIN32_WINNT >= 0x0501)
#define RESULT_IS_ADDED  0x0010
#define RESULT_IS_CHANGED  0x0020
#define RESULT_IS_DELETED  0x0040
#endif /* _WIN32_WINNT >= 0x0501 */

typedef enum _WSAESETSERVICEOP {
    RNRSERVICE_REGISTER=0,
    RNRSERVICE_DEREGISTER,
    RNRSERVICE_DELETE
} WSAESETSERVICEOP, *PWSAESETSERVICEOP, *LPWSAESETSERVICEOP;

typedef struct _WSANSClassInfoA {
    LPSTR lpszName;
    DWORD dwNameSpace;
    DWORD dwValueType;
    DWORD dwValueSize;
    LPVOID lpValue;
} WSANSCLASSINFOA, *PWSANSCLASSINFOA, *LPWSANSCLASSINFOA;

typedef struct _WSANSClassInfoW {
    LPWSTR lpszName;
    DWORD dwNameSpace;
    DWORD dwValueType;
    DWORD dwValueSize;
    LPVOID lpValue;
} WSANSCLASSINFOW, *PWSANSCLASSINFOW, *LPWSANSCLASSINFOW;

typedef struct _WSAServiceClassInfoA {
    LPGUID lpServiceClassId;
    LPSTR lpszServiceClassName;
    DWORD dwCount;
    LPWSANSCLASSINFOA lpClassInfos;
} WSASERVICECLASSINFOA, *PWSASERVICECLASSINFOA, *LPWSASERVICECLASSINFOA;

typedef struct _WSAServiceClassInfoW {
    LPGUID lpServiceClassId;
    LPWSTR lpszServiceClassName;
    DWORD dwCount;
    LPWSANSCLASSINFOW lpClassInfos;
} WSASERVICECLASSINFOW, *PWSASERVICECLASSINFOW, *LPWSASERVICECLASSINFOW;

typedef struct _WSANAMESPACE_INFOA {
    GUID NSProviderId;
    DWORD dwNameSpace;
    BOOL fActive;
    DWORD dwVersion;
    LPSTR lpszIdentifier;
} WSANAMESPACE_INFOA, *PWSANAMESPACE_INFOA, *LPWSANAMESPACE_INFOA;

typedef struct _WSANAMESPACE_INFOW {
    GUID NSProviderId;
    DWORD dwNameSpace;
    BOOL fActive;
    DWORD dwVersion;
    LPWSTR lpszIdentifier;
} WSANAMESPACE_INFOW, *PWSANAMESPACE_INFOW, *LPWSANAMESPACE_INFOW;

typedef struct _WSANAMESPACE_INFOEXA {
    GUID NSProviderId;
    DWORD dwNameSpace;
    BOOL fActive;
    DWORD dwVersion;
    LPSTR lpszIdentifier;
    BLOB ProviderSpecific;
} WSANAMESPACE_INFOEXA, *PWSANAMESPACE_INFOEXA, *LPWSANAMESPACE_INFOEXA;

typedef struct _WSANAMESPACE_INFOEXW {
    GUID NSProviderId;
    DWORD dwNameSpace;
    BOOL fActive;
    DWORD dwVersion;
    LPWSTR lpszIdentifier;
    BLOB ProviderSpecific;
} WSANAMESPACE_INFOEXW, *PWSANAMESPACE_INFOEXW, *LPWSANAMESPACE_INFOEXW;

#ifdef UNICODE
typedef WSANSCLASSINFOW WSANSCLASSINFO;
typedef PWSANSCLASSINFOW PWSANSCLASSINFO;
typedef LPWSANSCLASSINFOW LPWSANSCLASSINFO;
typedef WSASERVICECLASSINFOW WSASERVICECLASSINFO;
typedef PWSASERVICECLASSINFOW PWSASERVICECLASSINFO;
typedef LPWSASERVICECLASSINFOW LPWSASERVICECLASSINFO;
typedef WSANAMESPACE_INFOW WSANAMESPACE_INFO;
typedef PWSANAMESPACE_INFOW PWSANAMESPACE_INFO;
typedef LPWSANAMESPACE_INFOW LPWSANAMESPACE_INFO;
typedef WSANAMESPACE_INFOEXW WSANAMESPACE_INFOEX;
typedef PWSANAMESPACE_INFOEXW PWSANAMESPACE_INFOEX;
typedef LPWSANAMESPACE_INFOEXW LPWSANAMESPACE_INFOEX;
#else
typedef WSANSCLASSINFOA WSANSCLASSINFO;
typedef PWSANSCLASSINFOA PWSANSCLASSINFO;
typedef LPWSANSCLASSINFOA LPWSANSCLASSINFO;
typedef WSASERVICECLASSINFOA WSASERVICECLASSINFO;
typedef PWSASERVICECLASSINFOA PWSASERVICECLASSINFO;
typedef LPWSASERVICECLASSINFOA LPWSASERVICECLASSINFO;
typedef WSANAMESPACE_INFOA WSANAMESPACE_INFO;
typedef PWSANAMESPACE_INFOA PWSANAMESPACE_INFO;
typedef LPWSANAMESPACE_INFOA LPWSANAMESPACE_INFO;
typedef WSANAMESPACE_INFOEXA WSANAMESPACE_INFOEX;
typedef PWSANAMESPACE_INFOEXA PWSANAMESPACE_INFOEX;
typedef LPWSANAMESPACE_INFOEXA LPWSANAMESPACE_INFOEX;
#endif /* UNICODE */

#if(_WIN32_WINNT >= 0x0600)

#define POLLRDNORM 0x0100
#define POLLRDBAND 0x0200
#define POLLIN  (POLLRDNORM|POLLRDBAND)
#define POLLPRI  0x0400

#define POLLWRNORM  0x0010
#define POLLOUT  (POLLWRNORM)
#define POLLWRBAND  0x0020

#define POLLERR  0x0001
#define POLLHUP  0x0002
#define POLLNVAL  0x0004

typedef struct pollfd {
    SOCKET fd;
    SHORT events;
    SHORT revents;
} WSAPOLLFD, *PWSAPOLLFD, *LPWSAPOLLFD;

#endif /* _WIN32_WINNT >= 0x0600 */

#if INCL_WINSOCK_API_PROTOTYPES
WINSOCK_API_LINKAGE SOCKET WSAAPI accept(SOCKET,struct sockaddr*,int*);
WINSOCK_API_LINKAGE int WSAAPI bind(SOCKET,const struct sockaddr*,int);
WINSOCK_API_LINKAGE int WSAAPI closesocket(SOCKET);
WINSOCK_API_LINKAGE int WSAAPI connect(SOCKET,const struct sockaddr*,int);
WINSOCK_API_LINKAGE int WSAAPI ioctlsocket(SOCKET,long,u_long*);
WINSOCK_API_LINKAGE int WSAAPI getpeername(SOCKET,struct sockaddr*,int*);
WINSOCK_API_LINKAGE int WSAAPI getsockname(SOCKET,struct sockaddr*,int*);
WINSOCK_API_LINKAGE int WSAAPI getsockopt(SOCKET,int,int,char*,int*);
WINSOCK_API_LINKAGE u_long WSAAPI htonl(u_long);
WINSOCK_API_LINKAGE u_short WSAAPI htons(u_short);
WINSOCK_API_LINKAGE unsigned long WSAAPI inet_addr(const char*);
WINSOCK_API_LINKAGE char * WSAAPI inet_ntoa(struct in_addr);
WINSOCK_API_LINKAGE int WSAAPI listen(SOCKET,int);
WINSOCK_API_LINKAGE u_long WSAAPI ntohl(u_long);
WINSOCK_API_LINKAGE u_short WSAAPI ntohs(u_short);
WINSOCK_API_LINKAGE int WSAAPI recv(SOCKET,char*,int,int);
WINSOCK_API_LINKAGE int WSAAPI recvfrom(SOCKET,char*,int,int,struct sockaddr*,int*);
WINSOCK_API_LINKAGE int WSAAPI select(int,fd_set*,fd_set*,fd_set*,const struct timeval*);
WINSOCK_API_LINKAGE int WSAAPI send(SOCKET,const char*,int,int);
WINSOCK_API_LINKAGE int WSAAPI sendto(SOCKET,const char*,int,int,const struct sockaddr*,int);
WINSOCK_API_LINKAGE int WSAAPI setsockopt(SOCKET,int,int,const char*,int);
WINSOCK_API_LINKAGE int WSAAPI shutdown(SOCKET,int);
WINSOCK_API_LINKAGE SOCKET WSAAPI socket(int,int,int);
WINSOCK_API_LINKAGE struct hostent * WSAAPI gethostbyaddr(const char*,int,int);
WINSOCK_API_LINKAGE struct hostent * WSAAPI gethostbyname(const char*);
WINSOCK_API_LINKAGE int WSAAPI gethostname(char*,int);
WINSOCK_API_LINKAGE struct servent * WSAAPI getservbyport(int,const char*);
WINSOCK_API_LINKAGE struct servent * WSAAPI getservbyname(const char*,const char*);
WINSOCK_API_LINKAGE struct protoent * WSAAPI getprotobynumber(int);
WINSOCK_API_LINKAGE struct protoent * WSAAPI getprotobyname(const char*);
/* extensions */
WINSOCK_API_LINKAGE int WSAAPI WSAStartup(WORD,LPWSADATA);
WINSOCK_API_LINKAGE int WSAAPI WSACleanup(void);
WINSOCK_API_LINKAGE void WSAAPI WSASetLastError(int);
WINSOCK_API_LINKAGE int WSAAPI WSAGetLastError(void);
WINSOCK_API_LINKAGE BOOL WSAAPI WSAIsBlocking(void);
WINSOCK_API_LINKAGE int WSAAPI WSAUnhookBlockingHook(void);
WINSOCK_API_LINKAGE FARPROC WSAAPI WSASetBlockingHook(FARPROC);
WINSOCK_API_LINKAGE int WSAAPI WSACancelBlockingCall(void);
WINSOCK_API_LINKAGE HANDLE WSAAPI WSAAsyncGetServByName(HWND,u_int,const char*,const char*,char*,int);
WINSOCK_API_LINKAGE HANDLE WSAAPI WSAAsyncGetServByPort(HWND,u_int,int,const char*,char*,int);
WINSOCK_API_LINKAGE HANDLE WSAAPI WSAAsyncGetProtoByName(HWND,u_int,const char*,char*,int);
WINSOCK_API_LINKAGE HANDLE WSAAPI WSAAsyncGetProtoByNumber(HWND,u_int,int,char*,int);
WINSOCK_API_LINKAGE HANDLE WSAAPI WSAAsyncGetHostByName(HWND,u_int,const char*,char*,int);
WINSOCK_API_LINKAGE HANDLE WSAAPI WSAAsyncGetHostByAddr(HWND,u_int,const char*,int,int,char*,int);
WINSOCK_API_LINKAGE int WSAAPI WSACancelAsyncRequest(HANDLE);
WINSOCK_API_LINKAGE int WSAAPI WSAAsyncSelect(SOCKET,HWND,u_int,long);
WINSOCK_API_LINKAGE SOCKET WSAAPI WSAAccept(SOCKET,struct sockaddr*,LPINT,LPCONDITIONPROC,DWORD_PTR);
WINSOCK_API_LINKAGE BOOL WSAAPI WSACloseEvent(WSAEVENT);
WINSOCK_API_LINKAGE int WSAAPI WSAConnect(SOCKET,const struct sockaddr*,int,LPWSABUF,LPWSABUF,LPQOS,LPQOS);
BOOL PASCAL WSAConnectByNameW(SOCKET,LPWSTR,LPWSTR,LPDWORD,LPSOCKADDR,LPDWORD,LPSOCKADDR,const struct timeval*,LPWSAOVERLAPPED);
BOOL PASCAL WSAConnectByNameA(SOCKET,LPCSTR,LPCSTR,LPDWORD,LPSOCKADDR,LPDWORD,LPSOCKADDR,const struct timeval*,LPWSAOVERLAPPED);
BOOL PASCAL WSAConnectByList(SOCKET,PSOCKET_ADDRESS_LIST,LPDWORD,LPSOCKADDR,LPDWORD,LPSOCKADDR,const struct timeval*,LPWSAOVERLAPPED);
WINSOCK_API_LINKAGE WSAEVENT WSAAPI WSACreateEvent(void);
WINSOCK_API_LINKAGE int WSAAPI WSADuplicateSocketA(SOCKET,DWORD,LPWSAPROTOCOL_INFOA);
WINSOCK_API_LINKAGE int WSAAPI WSADuplicateSocketW(SOCKET,DWORD,LPWSAPROTOCOL_INFOW);
WINSOCK_API_LINKAGE int WSAAPI WSAEnumNetworkEvents(SOCKET,WSAEVENT,LPWSANETWORKEVENTS);
WINSOCK_API_LINKAGE int WSAAPI WSAEnumProtocolsA(LPINT,LPWSAPROTOCOL_INFOA,LPDWORD);
WINSOCK_API_LINKAGE int WSAAPI WSAEnumProtocolsW(LPINT,LPWSAPROTOCOL_INFOW,LPDWORD);
WINSOCK_API_LINKAGE int WSAAPI WSAEventSelect(SOCKET,WSAEVENT,long);
WINSOCK_API_LINKAGE BOOL WSAAPI WSAGetOverlappedResult(SOCKET,LPWSAOVERLAPPED,LPDWORD,BOOL,LPDWORD);
WINSOCK_API_LINKAGE BOOL WSAAPI WSAGetQOSByName(SOCKET,LPWSABUF,LPQOS);
WINSOCK_API_LINKAGE int WSAAPI WSAHtonl(SOCKET,u_long,u_long*);
WINSOCK_API_LINKAGE int WSAAPI WSAHtons(SOCKET,u_short,u_short*);
WINSOCK_API_LINKAGE int WSAAPI WSAIoctl(SOCKET,DWORD,LPVOID,DWORD,LPVOID,DWORD,LPDWORD,LPWSAOVERLAPPED,LPWSAOVERLAPPED_COMPLETION_ROUTINE);
WINSOCK_API_LINKAGE SOCKET WSAAPI WSAJoinLeaf(SOCKET,const struct sockaddr*,int,LPWSABUF,LPWSABUF,LPQOS,LPQOS,DWORD);
WINSOCK_API_LINKAGE int WSAAPI WSANtohl(SOCKET,u_long,u_long*);
WINSOCK_API_LINKAGE int WSAAPI WSANtohs(SOCKET,u_short,u_short*);
WINSOCK_API_LINKAGE int WSAAPI WSARecv(SOCKET,LPWSABUF,DWORD,LPDWORD,LPDWORD,LPWSAOVERLAPPED,LPWSAOVERLAPPED_COMPLETION_ROUTINE);
WINSOCK_API_LINKAGE int WSAAPI WSARecvDisconnect(SOCKET,LPWSABUF);
WINSOCK_API_LINKAGE int WSAAPI WSARecvFrom(SOCKET,LPWSABUF,DWORD,LPDWORD,LPDWORD,struct sockaddr*,LPINT,LPWSAOVERLAPPED,LPWSAOVERLAPPED_COMPLETION_ROUTINE);
WINSOCK_API_LINKAGE BOOL WSAAPI WSAResetEvent(WSAEVENT);
WINSOCK_API_LINKAGE int WSAAPI WSASend(SOCKET,LPWSABUF,DWORD,LPDWORD,DWORD,LPWSAOVERLAPPED,LPWSAOVERLAPPED_COMPLETION_ROUTINE);
WINSOCK_API_LINKAGE int WSAAPI WSASendDisconnect(SOCKET,LPWSABUF);
WINSOCK_API_LINKAGE int WSAAPI WSASendTo(SOCKET,LPWSABUF,DWORD,LPDWORD,DWORD,const struct sockaddr*,int,LPWSAOVERLAPPED,LPWSAOVERLAPPED_COMPLETION_ROUTINE);
WINSOCK_API_LINKAGE BOOL WSAAPI WSASetEvent(WSAEVENT);
WINSOCK_API_LINKAGE SOCKET WSAAPI WSASocketA(int,int,int,LPWSAPROTOCOL_INFOA,GROUP,DWORD);
WINSOCK_API_LINKAGE SOCKET WSAAPI WSASocketW(int,int,int,LPWSAPROTOCOL_INFOW,GROUP,DWORD);
WINSOCK_API_LINKAGE DWORD WSAAPI WSAWaitForMultipleEvents(DWORD,const WSAEVENT*,BOOL,DWORD,BOOL);
WINSOCK_API_LINKAGE INT WSAAPI WSAAddressToStringA(LPSOCKADDR,DWORD,LPWSAPROTOCOL_INFOA,LPSTR,LPDWORD);
WINSOCK_API_LINKAGE INT WSAAPI WSAAddressToStringW(LPSOCKADDR,DWORD,LPWSAPROTOCOL_INFOW,LPWSTR,LPDWORD);
WINSOCK_API_LINKAGE INT WSAAPI WSAStringToAddressA(LPSTR,INT,LPWSAPROTOCOL_INFOA,LPSOCKADDR,LPINT);
WINSOCK_API_LINKAGE INT WSAAPI WSAStringToAddressW(LPWSTR,INT,LPWSAPROTOCOL_INFOW,LPSOCKADDR,LPINT);
WINSOCK_API_LINKAGE INT WSAAPI WSALookupServiceBeginA(LPWSAQUERYSETA,DWORD,LPHANDLE);
WINSOCK_API_LINKAGE INT WSAAPI WSALookupServiceBeginW(LPWSAQUERYSETW,DWORD,LPHANDLE);
WINSOCK_API_LINKAGE INT WSAAPI WSALookupServiceNextA(HANDLE,DWORD,LPDWORD,LPWSAQUERYSETA);
WINSOCK_API_LINKAGE INT WSAAPI WSALookupServiceNextW(HANDLE,DWORD,LPDWORD,LPWSAQUERYSETW);
WINSOCK_API_LINKAGE INT WSAAPI WSALookupServiceEnd(HANDLE);
WINSOCK_API_LINKAGE INT WSAAPI WSAInstallServiceClassA(LPWSASERVICECLASSINFOA);
WINSOCK_API_LINKAGE INT WSAAPI WSAInstallServiceClassW(LPWSASERVICECLASSINFOW);
WINSOCK_API_LINKAGE INT WSAAPI WSARemoveServiceClass(LPGUID);
WINSOCK_API_LINKAGE INT WSAAPI WSAGetServiceClassInfoA(LPGUID,LPGUID,LPDWORD,LPWSASERVICECLASSINFOA);
WINSOCK_API_LINKAGE INT WSAAPI WSAGetServiceClassInfoW(LPGUID,LPGUID,LPDWORD,LPWSASERVICECLASSINFOW);
WINSOCK_API_LINKAGE INT WSAAPI WSAEnumNameSpaceProvidersA(LPDWORD,LPWSANAMESPACE_INFOA);
WINSOCK_API_LINKAGE INT WSAAPI WSAEnumNameSpaceProvidersW(LPDWORD,LPWSANAMESPACE_INFOW);
WINSOCK_API_LINKAGE INT WSAAPI WSAGetServiceClassNameByClassIdA(LPGUID,LPSTR,LPDWORD);
WINSOCK_API_LINKAGE INT WSAAPI WSAGetServiceClassNameByClassIdW(LPGUID,LPWSTR,LPDWORD);
WINSOCK_API_LINKAGE INT WSAAPI WSASetServiceA(LPWSAQUERYSETA,WSAESETSERVICEOP,DWORD);
WINSOCK_API_LINKAGE INT WSAAPI WSASetServiceW(LPWSAQUERYSETW,WSAESETSERVICEOP,DWORD);
WINSOCK_API_LINKAGE INT WSAAPI WSAProviderConfigChange(LPHANDLE,LPWSAOVERLAPPED,LPWSAOVERLAPPED_COMPLETION_ROUTINE);
#if (_WIN32_WINNT >= 0x0501)
WINSOCK_API_LINKAGE INT WSAAPI WSANSPIoctl(HANDLE,DWORD,LPVOID,DWORD,LPVOID,DWORD,LPDWORD,LPWSACOMPLETION);
#endif /* _WIN32_WINNT >= 0x0501 */
#if (_WIN32_WINNT >= 0x0600)
WINSOCK_API_LINKAGE int WSAAPI WSASendMsg(SOCKET,LPWSAMSG,DWORD,LPDWORD,LPWSAOVERLAPPED,LPWSAOVERLAPPED_COMPLETION_ROUTINE);
WINSOCK_API_LINKAGE INT WSAAPI WSAEnumNameSpaceProvidersExA(LPDWORD,LPWSANAMESPACE_INFOEXA);
WINSOCK_API_LINKAGE INT WSAAPI WSAEnumNameSpaceProvidersExW(LPDWORD,LPWSANAMESPACE_INFOEXW);
WINSOCK_API_LINKAGE int WSAAPI WSAPoll(LPWSAPOLLFD,ULONG,INT);
#endif /* _WIN32_WINNT >= 0x0600 */
#endif /* INCL_WINSOCK_API_PROTOTYPES */

#if INCL_WINSOCK_API_TYPEDEFS
typedef SOCKET (WSAAPI *LPFN_ACCEPT)(SOCKET,struct sockaddr*,int*);
typedef int (WSAAPI *LPFN_BIND)(SOCKET,const struct sockaddr*,int);
typedef int (WSAAPI *LPFN_CLOSESOCKET)(SOCKET);
typedef int (WSAAPI *LPFN_CONNECT)(SOCKET,const struct sockaddr*,int);
typedef int (WSAAPI *LPFN_IOCTLSOCKET)(SOCKET,long,u_long*);
typedef int (WSAAPI *LPFN_GETPEERNAME)(SOCKET,struct sockaddr*,int*);
typedef int (WSAAPI *LPFN_GETSOCKNAME)(SOCKET,struct sockaddr*,int*);
typedef int (WSAAPI *LPFN_GETSOCKOPT)(SOCKET,int,int,char*,int*);
typedef u_long (WSAAPI *LPFN_HTONL)(u_long);
typedef u_short (WSAAPI *LPFN_HTONS)(u_short);
typedef unsigned long (WSAAPI *LPFN_INET_ADDR)(const char*);
typedef char * (WSAAPI *LPFN_INET_NTOA)(struct in_addr);
typedef int (WSAAPI *LPFN_LISTEN)(SOCKET,int);
typedef u_long (WSAAPI *LPFN_NTOHL)(u_long);
typedef u_short (WSAAPI *LPFN_NTOHS)(u_short);
typedef int (WSAAPI *LPFN_RECV)(SOCKET,char*,int,int);
typedef int (WSAAPI *LPFN_RECVFROM)(SOCKET,char*,int,int,struct sockaddr*,int*);
typedef int (WSAAPI *LPFN_SELECT)(int,fd_set*,fd_set*,fd_set*,const struct timeval*);
typedef int (WSAAPI *LPFN_SEND)(SOCKET,const char*,int,int);
typedef int (WSAAPI *LPFN_SENDTO)(SOCKET,const char*,int,int,const struct sockaddr*,int);
typedef int (WSAAPI *LPFN_SETSOCKOPT)(SOCKET,int,int,const char*,int);
typedef int (WSAAPI *LPFN_SHUTDOWN)(SOCKET,int);
typedef SOCKET (WSAAPI *LPFN_SOCKET)(int,int,int);
typedef struct hostent * (WSAAPI *LPFN_GETHOSTBYADDR)(const char*,int,int);
typedef struct hostent * (WSAAPI *LPFN_GETHOSTBYNAME)(const char*);
typedef int (WSAAPI *LPFN_GETHOSTNAME)(char*,int);
typedef struct servent * (WSAAPI *LPFN_GETSERVBYPORT)(int,const char*);
typedef struct servent * (WSAAPI *LPFN_GETSERVBYNAME)(const char*,const char*);
typedef struct protoent * (WSAAPI *LPFN_GETPROTOBYNUMBER)(int);
typedef struct protoent * (WSAAPI *LPFN_GETPROTOBYNAME)(const char*);
/* extensions */
typedef int (WSAAPI *LPFN_WSASTARTUP)(WORD,LPWSADATA);
typedef int (WSAAPI *LPFN_WSACLEANUP)(void);
typedef void (WSAAPI *LPFN_WSASETLASTERROR)(int);
typedef int (WSAAPI *LPFN_WSAGETLASTERROR)(void);
typedef BOOL (WSAAPI *LPFN_WSAISBLOCKING)(void);
typedef int (WSAAPI *LPFN_WSAUNHOOKBLOCKINGHOOK)(void);
typedef FARPROC (WSAAPI *LPFN_WSASETBLOCKINGHOOK)(FARPROC);
typedef int (WSAAPI *LPFN_WSACANCELBLOCKINGCALL)(void);
typedef HANDLE (WSAAPI *LPFN_WSAASYNCGETSERVBYNAME)(HWND,u_int,const char*,const char*,char*,int);
typedef HANDLE (WSAAPI *LPFN_WSAASYNCGETSERVBYPORT)(HWND,u_int,int,const char*,char*,int);
typedef HANDLE (WSAAPI *LPFN_WSAASYNCGETPROTOBYNAME)(HWND,u_int,const char*,char*,int);
typedef HANDLE (WSAAPI *LPFN_WSAASYNCGETPROTOBYNUMBER)(HWND,u_int,int,char*,int);
typedef HANDLE (WSAAPI *LPFN_WSAASYNCGETHOSTBYNAME)(HWND,u_int,const char*,char*,int);
typedef HANDLE (WSAAPI *LPFN_WSAASYNCGETHOSTBYADDR)(HWND,u_int,const char*,int,int,char*,int);
typedef int (WSAAPI *LPFN_WSACANCELASYNCREQUEST)(HANDLE);
typedef int (WSAAPI *LPFN_WSAASYNCSELECT)(SOCKET,HWND,u_int,long);
typedef SOCKET (WSAAPI *LPFN_WSAACCEPT)(SOCKET,struct sockaddr*,LPINT,LPCONDITIONPROC,DWORD_PTR);
typedef BOOL (WSAAPI *LPFN_WSACLOSEEVENT)(WSAEVENT);
typedef int (WSAAPI *LPFN_WSACONNECT)(SOCKET,const struct sockaddr*,int,LPWSABUF,LPWSABUF,LPQOS,LPQOS);
typedef WSAEVENT (WSAAPI * LPFN_WSACREATEEVENT)(void);
typedef int (WSAAPI *LPFN_WSADUPLICATESOCKETA)(SOCKET,DWORD,LPWSAPROTOCOL_INFOA);
typedef int (WSAAPI *LPFN_WSADUPLICATESOCKETW)(SOCKET,DWORD,LPWSAPROTOCOL_INFOW);
typedef int (WSAAPI *LPFN_WSAENUMNETWORKEVENTS)(SOCKET,WSAEVENT,LPWSANETWORKEVENTS);
typedef int (WSAAPI *LPFN_WSAENUMPROTOCOLSA)(LPINT,LPWSAPROTOCOL_INFOA,LPDWORD);
typedef int (WSAAPI *LPFN_WSAENUMPROTOCOLSW)(LPINT,LPWSAPROTOCOL_INFOW,LPDWORD);
typedef int (WSAAPI *LPFN_WSAEVENTSELECT)(SOCKET,WSAEVENT,long);
typedef BOOL (WSAAPI *LPFN_WSAGETOVERLAPPEDRESULT)(SOCKET,LPWSAOVERLAPPED,LPDWORD,BOOL,LPDWORD);
typedef BOOL (WSAAPI *LPFN_WSAGETQOSBYNAME)(SOCKET,LPWSABUF,LPQOS);
typedef int (WSAAPI *LPFN_WSAHTONL)(SOCKET,u_long,u_long*);
typedef int (WSAAPI *LPFN_WSAHTONS)(SOCKET,u_short,u_short*);
typedef int (WSAAPI *LPFN_WSAIOCTL)(SOCKET,DWORD,LPVOID,DWORD,LPVOID,DWORD,LPDWORD,LPWSAOVERLAPPED,LPWSAOVERLAPPED_COMPLETION_ROUTINE);
typedef SOCKET (WSAAPI *LPFN_WSAJOINLEAF)(SOCKET,const struct sockaddr*,int,LPWSABUF,LPWSABUF,LPQOS,LPQOS,DWORD);
typedef int (WSAAPI *LPFN_WSANTOHL)(SOCKET,u_long,u_long*);
typedef int (WSAAPI *LPFN_WSANTOHS)(SOCKET,u_short,u_short*);
typedef int (WSAAPI *LPFN_WSARECV)(SOCKET,LPWSABUF,DWORD,LPDWORD,LPDWORD,LPWSAOVERLAPPED,LPWSAOVERLAPPED_COMPLETION_ROUTINE);
typedef int (WSAAPI *LPFN_WSARECVDISCONNECT)(SOCKET,LPWSABUF);
typedef int (WSAAPI *LPFN_WSARECVFROM)(SOCKET,LPWSABUF,DWORD,LPDWORD,LPDWORD,struct sockaddr*,LPINT,LPWSAOVERLAPPED,LPWSAOVERLAPPED_COMPLETION_ROUTINE);
typedef BOOL (WSAAPI *LPFN_WSARESETEVENT)(WSAEVENT);
typedef int (WSAAPI *LPFN_WSASEND)(SOCKET,LPWSABUF,DWORD,LPDWORD,DWORD,LPWSAOVERLAPPED,LPWSAOVERLAPPED_COMPLETION_ROUTINE);
typedef int (WSAAPI *LPFN_WSASENDDISCONNECT)(SOCKET,LPWSABUF);
typedef int (WSAAPI *LPFN_WSASENDTO)(SOCKET,LPWSABUF,DWORD,LPDWORD,DWORD,const struct sockaddr*,int,LPWSAOVERLAPPED,LPWSAOVERLAPPED_COMPLETION_ROUTINE);
typedef BOOL (WSAAPI *LPFN_WSASETEVENT)(WSAEVENT);
typedef SOCKET (WSAAPI *LPFN_WSASOCKETA)(int,int,int,LPWSAPROTOCOL_INFOA,GROUP,DWORD);
typedef SOCKET (WSAAPI *LPFN_WSASOCKETW)(int,int,int,LPWSAPROTOCOL_INFOW,GROUP,DWORD);
typedef DWORD (WSAAPI *LPFN_WSAWAITFORMULTIPLEEVENTS)(DWORD,const WSAEVENT*,BOOL,DWORD,BOOL);
typedef INT (WSAAPI *LPFN_WSAADDRESSTOSTRINGA)(LPSOCKADDR,DWORD,LPWSAPROTOCOL_INFOA,LPSTR,LPDWORD);
typedef INT (WSAAPI *LPFN_WSAADDRESSTOSTRINGW)(LPSOCKADDR,DWORD,LPWSAPROTOCOL_INFOW,LPWSTR,LPDWORD);
typedef INT (WSAAPI *LPFN_WSASTRINGTOADDRESSA)(LPSTR,INT,LPWSAPROTOCOL_INFOA,LPSOCKADDR,LPINT);
typedef INT (WSAAPI *LPFN_WSASTRINGTOADDRESSW)(LPWSTR,INT,LPWSAPROTOCOL_INFOW,LPSOCKADDR,LPINT);
typedef INT (WSAAPI *LPFN_WSALOOKUPSERVICEBEGINA)(LPWSAQUERYSETA,DWORD,LPHANDLE);
typedef INT (WSAAPI *LPFN_WSALOOKUPSERVICEBEGINW)(LPWSAQUERYSETW,DWORD,LPHANDLE);
typedef INT (WSAAPI *LPFN_WSALOOKUPSERVICENEXTA)(HANDLE,DWORD,LPDWORD,LPWSAQUERYSETA);
typedef INT (WSAAPI *LPFN_WSALOOKUPSERVICENEXTW)(HANDLE,DWORD,LPDWORD,LPWSAQUERYSETW);
typedef INT (WSAAPI *LPFN_WSALOOKUPSERVICEEND)(HANDLE);
typedef INT (WSAAPI *LPFN_WSAINSTALLSERVICECLASSA)(LPWSASERVICECLASSINFOA);
typedef INT (WSAAPI *LPFN_WSAINSTALLSERVICECLASSW)(LPWSASERVICECLASSINFOW);
typedef INT (WSAAPI *LPFN_WSAREMOVESERVICECLASS)(LPGUID);
typedef INT (WSAAPI *LPFN_WSAGETSERVICECLASSINFOA)(LPGUID,LPGUID,LPDWORD,LPWSASERVICECLASSINFOA);
typedef INT (WSAAPI *LPFN_WSAGETSERVICECLASSINFOW)(LPGUID,LPGUID,LPDWORD,LPWSASERVICECLASSINFOW);
typedef INT (WSAAPI *LPFN_WSAENUMNAMESPACEPROVIDERSA)(LPDWORD,LPWSANAMESPACE_INFOA);
typedef INT (WSAAPI *LPFN_WSAENUMNAMESPACEPROVIDERSW)(LPDWORD,LPWSANAMESPACE_INFOW);
typedef INT (WSAAPI *LPFN_WSAGETSERVICECLASSNAMEBYCLASSIDA)(LPGUID,LPSTR,LPDWORD);
typedef INT (WSAAPI *LPFN_WSAGETSERVICECLASSNAMEBYCLASSIDW)(LPGUID,LPWSTR,LPDWORD);
typedef INT (WSAAPI *LPFN_WSASETSERVICEA)(LPWSAQUERYSETA,WSAESETSERVICEOP,DWORD);
typedef INT (WSAAPI *LPFN_WSASETSERVICEW)(LPWSAQUERYSETW,WSAESETSERVICEOP,DWORD);
typedef INT (WSAAPI *LPFN_WSAPROVIDERCONFIGCHANGE)(LPHANDLE,LPWSAOVERLAPPED,LPWSAOVERLAPPED_COMPLETION_ROUTINE);
#if (_WIN32_WINNT >= 0x0501)
typedef INT (WSAAPI *LPFN_WSANSPIOCTL)(HANDLE,DWORD,LPVOID,DWORD,LPVOID,DWORD,LPDWORD,LPWSACOMPLETION);
#endif /* _WIN32_WINNT >= 0x0501 */
#if (_WIN32_WINNT >= 0x0600)
typedef INT (WSAAPI *LPFN_WSAENUMNAMESPACEPROVIDERSEXA)(LPDWORD,LPWSANAMESPACE_INFOEXA);
typedef INT (WSAAPI *LPFN_WSAENUMNAMESPACEPROVIDERSEXW)(LPDWORD,LPWSANAMESPACE_INFOEXW);
#endif /* _WIN32_WINNT >= 0x0600 */
#endif /* INCL_WINSOCK_API_TYPEDEFS */

#if INCL_WINSOCK_API_PROTOTYPES
#ifdef UNICODE
#define WSAConnectByName  WSAConnectByNameW
#define WSADuplicateSocket  WSADuplicateSocketW
#define WSAEnumProtocols  WSAEnumProtocolsW
#define WSASocket  WSASocketW
#define WSAAddressToString  WSAAddressToStringW
#define WSAStringToAddress  WSAStringToAddressW
#define WSALookupServiceBegin  WSALookupServiceBeginW
#define WSALookupServiceNext  WSALookupServiceNextW
#define WSAInstallServiceClass  WSAInstallServiceClassW
#define WSAGetServiceClassInfo  WSAGetServiceClassInfoW
#define WSAEnumNameSpaceProviders   WSAEnumNameSpaceProvidersW
#define WSAEnumNameSpaceProvidersEx WSAEnumNameSpaceProvidersExW
#define WSAGetServiceClassNameByClassId  WSAGetServiceClassNameByClassIdW
#define WSASetService  WSASetServiceW
#else /* !UNICODE */
#define WSAConnectByName  WSAConnectByNameA
#define WSADuplicateSocket  WSADuplicateSocketA
#define WSAEnumProtocols  WSAEnumProtocolsA
#define WSASocket  WSASocketA
#define WSAAddressToString  WSAAddressToStringA
#define WSAStringToAddress  WSAStringToAddressA
#define WSALookupServiceBegin  WSALookupServiceBeginA
#define WSALookupServiceNext  WSALookupServiceNextA
#define WSAInstallServiceClass  WSAInstallServiceClassA
#define WSAGetServiceClassInfo  WSAGetServiceClassInfoA
#define WSAEnumNameSpaceProviders   WSAEnumNameSpaceProvidersA
#define WSAEnumNameSpaceProvidersEx WSAEnumNameSpaceProvidersExA
#define WSAGetServiceClassNameByClassId  WSAGetServiceClassNameByClassIdA
#define WSASetService  WSASetServiceA
#endif /* !UNICODE */
#endif /* INCL_WINSOCK_API_PROTOTYPES */

#if INCL_WINSOCK_API_TYPEDEFS
#ifdef UNICODE
#define LPFN_WSADUPLICATESOCKET  LPFN_WSADUPLICATESOCKETW
#define LPFN_WSAENUMPROTOCOLS  LPFN_WSAENUMPROTOCOLSW
#define LPFN_WSASOCKET  LPFN_WSASOCKETW
#define LPFN_WSAADDRESSTOSTRING  LPFN_WSAADDRESSTOSTRINGW
#define LPFN_WSASTRINGTOADDRESS  LPFN_WSASTRINGTOADDRESSW
#define LPFN_WSALOOKUPSERVICEBEGIN  LPFN_WSALOOKUPSERVICEBEGINW
#define LPFN_WSALOOKUPSERVICENEXT  LPFN_WSALOOKUPSERVICENEXTW
#define LPFN_WSAINSTALLSERVICECLASS  LPFN_WSAINSTALLSERVICECLASSW
#define LPFN_WSAGETSERVICECLASSINFO  LPFN_WSAGETSERVICECLASSINFOW
#define LPFN_WSAENUMNAMESPACEPROVIDERS  LPFN_WSAENUMNAMESPACEPROVIDERSW
#define LPFN_WSAENUMNAMESPACEPROVIDERSEX  LPFN_WSAENUMNAMESPACEPROVIDERSEXW
#define LPFN_WSAGETSERVICECLASSNAMEBYCLASSID  LPFN_WSAGETSERVICECLASSNAMEBYCLASSIDW
#define LPFN_WSASETSERVICE  LPFN_WSASETSERVICEW
#else /* !UNICODE */
#define LPFN_WSADUPLICATESOCKET  LPFN_WSADUPLICATESOCKETA
#define LPFN_WSAENUMPROTOCOLS  LPFN_WSAENUMPROTOCOLSA
#define LPFN_WSASOCKET  LPFN_WSASOCKETA
#define LPFN_WSAADDRESSTOSTRING  LPFN_WSAADDRESSTOSTRINGA
#define LPFN_WSASTRINGTOADDRESS  LPFN_WSASTRINGTOADDRESSA
#define LPFN_WSALOOKUPSERVICEBEGIN  LPFN_WSALOOKUPSERVICEBEGINA
#define LPFN_WSALOOKUPSERVICENEXT  LPFN_WSALOOKUPSERVICENEXTA
#define LPFN_WSAINSTALLSERVICECLASS  LPFN_WSAINSTALLSERVICECLASSA
#define LPFN_WSAGETSERVICECLASSINFO  LPFN_WSAGETSERVICECLASSINFOA
#define LPFN_WSAENUMNAMESPACEPROVIDERS  LPFN_WSAENUMNAMESPACEPROVIDERSA
#define LPFN_WSAENUMNAMESPACEPROVIDERSEX  LPFN_WSAENUMNAMESPACEPROVIDERSEXA
#define LPFN_WSAGETSERVICECLASSNAMEBYCLASSID  LPFN_WSAGETSERVICECLASSNAMEBYCLASSIDA
#define LPFN_WSASETSERVICE  LPFN_WSASETSERVICEA
#endif /* !UNICODE */
#endif /* INCL_WINSOCK_API_TYPEDEFS */

typedef struct sockaddr_in *LPSOCKADDR_IN;

typedef struct linger LINGER;
typedef struct linger *PLINGER;
typedef struct linger *LPLINGER;

typedef struct fd_set FD_SET;
typedef struct fd_set *PFD_SET;
typedef struct fd_set *LPFD_SET;

typedef struct hostent HOSTENT;
typedef struct hostent *PHOSTENT;
typedef struct hostent *LPHOSTENT;

typedef struct servent SERVENT;
typedef struct servent *PSERVENT;
typedef struct servent *LPSERVENT;

typedef struct protoent PROTOENT;
typedef struct protoent *PPROTOENT;
typedef struct protoent *LPPROTOENT;

typedef struct timeval TIMEVAL;
typedef struct timeval *PTIMEVAL;
typedef struct timeval *LPTIMEVAL;

#define WSAMAKEASYNCREPLY(buflen,error)  MAKELONG(buflen,error)
#define WSAMAKESELECTREPLY(event,error)  MAKELONG(event,error)
#define WSAGETASYNCBUFLEN(lParam)  LOWORD(lParam)
#define WSAGETASYNCERROR(lParam)   HIWORD(lParam)
#define WSAGETSELECTEVENT(lParam)  LOWORD(lParam)
#define WSAGETSELECTERROR(lParam)  HIWORD(lParam)

#ifdef __cplusplus
}
#endif

#ifdef _NEED_POPPACK
#include <poppack.h>
#endif

#if (_WIN32_WINNT >= 0x0501)
#ifdef IPV6STRICT
#include <wsipv6ok.h>
#endif  /* IPV6STRICT */
#endif /* _WIN32_WINNT >= 0x0501 */

#endif  /* _WINSOCK2_H */
