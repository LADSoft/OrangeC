#ifndef _WS2TCPIP_H
#define _WS2TCPIP_H

/* WinSock2 Extension for TCP/IP protocols */

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#include <winsock2.h>
#include <ws2ipdef.h>
#include <limits.h>

#define UDP_NOCHECKSUM  1
#define UDP_CHECKSUM_COVERAGE  20

#define WS2TCPIP_INLINE  inline

#define EAI_AGAIN  WSATRY_AGAIN
#define EAI_BADFLAGS  WSAEINVAL
#define EAI_FAIL  WSANO_RECOVERY
#define EAI_FAMILY  WSAEAFNOSUPPORT
#define EAI_MEMORY  WSA_NOT_ENOUGH_MEMORY
#define EAI_NONAME  WSAHOST_NOT_FOUND
#define EAI_SERVICE  WSATYPE_NOT_FOUND
#define EAI_SOCKTYPE  WSAESOCKTNOSUPPORT

#define EAI_NODATA  EAI_NONAME

#define AI_PASSIVE  0x00000001
#define AI_CANONNAME  0x00000002
#define AI_NUMERICHOST  0x00000004
#define AI_NUMERICSERV  0x00000008

#define AI_ALL  0x00000100
#define AI_ADDRCONFIG  0x00000400
#define AI_V4MAPPED  0x00000800

#define AI_NON_AUTHORITATIVE  LUP_NON_AUTHORITATIVE
#define AI_SECURE  LUP_SECURE
#define AI_RETURN_PREFERRED_NAMES  LUP_RETURN_PREFERRED_NAMES

typedef struct addrinfo {
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    size_t ai_addrlen;
    char *ai_canonname;
    struct sockaddr *ai_addr;
    struct addrinfo *ai_next;
} ADDRINFOA, *PADDRINFOA;

typedef struct addrinfoW {
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    size_t ai_addrlen;
    PWSTR ai_canonname;
    struct sockaddr *ai_addr;
    struct addrinfoW *ai_next;
} ADDRINFOW, *PADDRINFOW;

#ifdef UNICODE
typedef ADDRINFOW ADDRINFOT, *PADDRINFOT;
#else
typedef ADDRINFOA ADDRINFOT, *PADDRINFOT;
#endif /* UNICODE */

typedef ADDRINFOA ADDRINFO, *LPADDRINFO;

#if (_WIN32_WINNT >= 0x0600)

typedef struct addrinfoexA {
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    size_t ai_addrlen;
    char *ai_canonname;
    struct sockaddr *ai_addr;
    void *ai_blob;
    size_t ai_bloblen;
    LPGUID ai_provider;
    struct addrinfoexA *ai_next;
} ADDRINFOEXA, *PADDRINFOEXA, *LPADDRINFOEXA;

typedef struct addrinfoexW {
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    size_t ai_addrlen;
    PWSTR ai_canonname;
    struct sockaddr *ai_addr;
    void *ai_blob;
    size_t ai_bloblen;
    LPGUID ai_provider;
    struct addrinfoexW *ai_next;
} ADDRINFOEXW, *PADDRINFOEXW, *LPADDRINFOEXW;

#ifdef UNICODE
typedef ADDRINFOEXW ADDRINFOEX, *PADDRINFOEX;
#else
typedef ADDRINFOEXA ADDRINFOEX, *PADDRINFOEX;
#endif /* UNICODE */

#endif /* _WIN32_WINNT >= 0x0600 */

#ifdef __cplusplus
extern "C" {
#endif

WINSOCK_API_LINKAGE INT WSAAPI getaddrinfo(PCSTR, PCSTR, const ADDRINFOA *, PADDRINFOA *);

#if (_WIN32_WINNT >= 0x0502)
WINSOCK_API_LINKAGE INT WSAAPI GetAddrInfoW(PCWSTR, PCWSTR, const ADDRINFOW *, PADDRINFOW *);

#define GetAddrInfoA  getaddrinfo

#ifdef UNICODE
#define GetAddrInfo  GetAddrInfoW
#else
#define GetAddrInfo  GetAddrInfoA
#endif /* UNICODE */
#endif /* _WIN32_WINNT >= 0x0502 */

#if INCL_WINSOCK_API_TYPEDEFS
typedef INT (WSAAPI *LPFN_GETADDRINFO)(PCSTR, PCSTR, const ADDRINFOA *, PADDRINFOA *);
typedef INT (WSAAPI *LPFN_GETADDRINFOW)(PCWSTR, PCWSTR, const ADDRINFOW *, PADDRINFOW *);

#define LPFN_GETADDRINFOA  LPFN_GETADDRINFO

#ifdef UNICODE
#define LPFN_GETADDRINFOT  LPFN_GETADDRINFOW
#else
#define LPFN_GETADDRINFOT  LPFN_GETADDRINFOA
#endif /* UNICODE */
#endif /* INCL_WINSOCK_API_TYPEDEFS */

#if (_WIN32_WINNT >= 0x0600)

typedef void (CALLBACK *LPLOOKUPSERVICE_COMPLETION_ROUTINE)(DWORD, DWORD, LPWSAOVERLAPPED);

WINSOCK_API_LINKAGE INT WSAAPI GetAddrInfoExA(PCSTR, PCSTR, DWORD, LPGUID, const ADDRINFOEXA *, PADDRINFOEXA *, struct timeval *, LPOVERLAPPED, LPLOOKUPSERVICE_COMPLETION_ROUTINE, LPHANDLE);
WINSOCK_API_LINKAGE INT WSAAPI GetAddrInfoExW(PCWSTR, PCWSTR, DWORD, LPGUID, const ADDRINFOEXW *, PADDRINFOEXW *, struct timeval *, LPOVERLAPPED, LPLOOKUPSERVICE_COMPLETION_ROUTINE, LPHANDLE);

#ifdef UNICODE
#define GetAddrInfoEx  GetAddrInfoExW
#else
#define GetAddrInfoEx  GetAddrInfoExA
#endif /* UNICODE */

#if INCL_WINSOCK_API_TYPEDEFS

typedef INT (WSAAPI *LPFN_GETADDRINFOEXA)(PCSTR, PCSTR, DWORD, LPGUID, const ADDRINFOEXA *, PADDRINFOEXA *, struct timeval *, LPOVERLAPPED, LPLOOKUPSERVICE_COMPLETION_ROUTINE, LPHANDLE);
typedef INT (WSAAPI *LPFN_GETADDRINFOEXW)(PCWSTR, PCWSTR, DWORD, LPGUID, const ADDRINFOEXW *, PADDRINFOEXW *, struct timeval *, LPOVERLAPPED, LPLOOKUPSERVICE_COMPLETION_ROUTINE, LPHANDLE);

#ifdef UNICODE
#define LPFN_GETADDRINFOEX  LPFN_GETADDRINFOEXW
#else
#define LPFN_GETADDRINFOEX  LPFN_GETADDRINFOEXA
#endif /* UNICODE */
#endif /* INCL_WINSOCK_API_TYPEDEFS */

#endif /* _WIN32_WINNT >= 0x0600 */

#if (_WIN32_WINNT >= 0x0600)

WINSOCK_API_LINKAGE INT WSAAPI SetAddrInfoExA(PCSTR, PCSTR, SOCKET_ADDRESS *, DWORD, LPBLOB, DWORD, DWORD, LPGUID, struct timeval *, LPOVERLAPPED, LPLOOKUPSERVICE_COMPLETION_ROUTINE, LPHANDLE);
WINSOCK_API_LINKAGE INT WSAAPI SetAddrInfoExW(PCWSTR, PCWSTR, SOCKET_ADDRESS *, DWORD, LPBLOB, DWORD, DWORD, LPGUID, struct timeval *, LPOVERLAPPED, LPLOOKUPSERVICE_COMPLETION_ROUTINE, LPHANDLE);

#ifdef UNICODE
#define SetAddrInfoEx  SetAddrInfoExW
#else
#define SetAddrInfoEx  SetAddrInfoExA
#endif

#if INCL_WINSOCK_API_TYPEDEFS
typedef INT (WSAAPI *LPFN_SETADDRINFOEXA)(PCSTR, PCSTR, SOCKET_ADDRESS *, DWORD, LPBLOB, DWORD, DWORD, LPGUID, struct timeval *, LPOVERLAPPED, LPLOOKUPSERVICE_COMPLETION_ROUTINE, LPHANDLE);
typedef INT (WSAAPI *LPFN_SETADDRINFOEXW)(PCWSTR, PCWSTR, SOCKET_ADDRESS *, DWORD, LPBLOB, DWORD, DWORD, LPGUID, struct timeval *, LPOVERLAPPED, LPLOOKUPSERVICE_COMPLETION_ROUTINE, LPHANDLE);

#ifdef UNICODE
#define LPFN_SETADDRINFOEX  LPFN_SETADDRINFOEXW
#else
#define LPFN_SETADDRINFOEX  LPFN_SETADDRINFOEXA
#endif /* UNICODE */
#endif /* INCL_WINSOCK_API_TYPEDEFS */

#endif /* _WIN32_WINNT >= 0x0600 */

WINSOCK_API_LINKAGE VOID WSAAPI freeaddrinfo(PADDRINFOA);

#if (_WIN32_WINNT >= 0x0502)
WINSOCK_API_LINKAGE VOID WSAAPI FreeAddrInfoW(PADDRINFOW);

#define FreeAddrInfoA  freeaddrinfo

#ifdef UNICODE
#define FreeAddrInfo  FreeAddrInfoW
#else
#define FreeAddrInfo  FreeAddrInfoA
#endif /* UNICODE */
#endif /* _WIN32_WINNT >= 0x0502 */

#if INCL_WINSOCK_API_TYPEDEFS
typedef VOID (WSAAPI *LPFN_FREEADDRINFO)(PADDRINFOA);
typedef VOID (WSAAPI *LPFN_FREEADDRINFOW)(PADDRINFOW);

#define LPFN_FREEADDRINFOA  LPFN_FREEADDRINFO

#ifdef UNICODE
#define LPFN_FREEADDRINFOT  LPFN_FREEADDRINFOW
#else
#define LPFN_FREEADDRINFOT  LPFN_FREEADDRINFOA
#endif
#endif /* INCL_WINSOCK_API_TYPEDEFS */

#if (_WIN32_WINNT >= 0x0600)

WINSOCK_API_LINKAGE void WSAAPI FreeAddrInfoEx(PADDRINFOEXA);
WINSOCK_API_LINKAGE void WSAAPI FreeAddrInfoExW(PADDRINFOEXW);

#define FreeAddrInfoExA  FreeAddrInfoEx

#ifdef UNICODE
#define FreeAddrInfoEx  FreeAddrInfoExW
#endif

#ifdef INCL_WINSOCK_API_TYPEDEFS
typedef void (WSAAPI *LPFN_FREEADDRINFOEXA)(PADDRINFOEXA);
typedef void (WSAAPI *LPFN_FREEADDRINFOEXW)(PADDRINFOEXW);

#ifdef UNICODE
#define LPFN_FREEADDRINFOEX  LPFN_FREEADDRINFOEXW
#else
#define LPFN_FREEADDRINFOEX  LPFN_FREEADDRINFOEXA
#endif /* UNICODE */

#endif /* INCL_WINSOCK_API_TYPEDEFS */
#endif /* _WIN32_WINNT >= 0x0600 */

typedef int socklen_t;

WINSOCK_API_LINKAGE INT WSAAPI getnameinfo(const SOCKADDR *, socklen_t, PCHAR, DWORD, PCHAR, DWORD, INT);

#if (_WIN32_WINNT >= 0x0502)
WINSOCK_API_LINKAGE INT WSAAPI GetNameInfoW(const SOCKADDR *, socklen_t, PWCHAR, DWORD, PWCHAR, DWORD, INT);

#define GetNameInfoA  getnameinfo

#ifdef UNICODE
#define GetNameInfo  GetNameInfoW
#else
#define GetNameInfo  GetNameInfoA
#endif /* UNICODE */
#endif /* _WIN32_WINNT >= 0x0502 */

#if INCL_WINSOCK_API_TYPEDEFS
typedef int (WSAAPI *LPFN_GETNAMEINFO)(const SOCKADDR *, socklen_t, PCHAR, DWORD, PCHAR, DWORD, INT);
typedef INT (WSAAPI *LPFN_GETNAMEINFOW)(const SOCKADDR *, socklen_t, PWCHAR, DWORD, PWCHAR, DWORD, INT);

#define LPFN_GETNAMEINFOA  LPFN_GETNAMEINFO

#ifdef UNICODE
#define LPFN_GETNAMEINFOT  LPFN_GETNAMEINFOW
#else
#define LPFN_GETNAMEINFOT  LPFN_GETNAMEINFOA
#endif /* UNICODE */
#endif /* INCL_WINSOCK_API_TYPEDEFS */

#if (NTDDI_VERSION >= NTDDI_VISTA)
WINSOCK_API_LINKAGE INT WSAAPI inet_pton(INT, PCSTR, PVOID);
INT WSAAPI InetPtonW(INT, PCWSTR, PVOID);
PCSTR WSAAPI inet_ntop(INT, PVOID, PSTR, size_t);
PCWSTR WSAAPI InetNtopW(INT, PVOID, PWSTR, size_t);

#define InetPtonA  inet_pton
#define InetNtopA  inet_ntop

#ifdef UNICODE
#define InetPton  InetPtonW
#define InetNtop  InetNtopW
#else
#define InetPton  InetPtonA
#define InetNtop  InetNtopA
#endif /* UNICODE */

#if INCL_WINSOCK_API_TYPEDEFS
typedef INT (WSAAPI *LPFN_INET_PTONA)(INT, PCSTR, PVOID);
typedef INT (WSAAPI *LPFN_INET_PTONW)(INT, PCWSTR, PVOID);
typedef PCSTR (WSAAPI *LPFN_INET_NTOPA)(INT, PVOID, PSTR, size_t);
typedef PCWSTR (WSAAPI *LPFN_INET_NTOPW)(INT, PVOID, PWSTR, size_t);

#ifdef UNICODE
#define LPFN_INET_PTON  LPFN_INET_PTONW
#define LPFN_INET_NTOP  LPFN_INET_NTOPW
#else
#define LPFN_INET_PTON  LPFN_INET_PTONA
#define LPFN_INET_NTOP  LPFN_INET_NTOPA
#endif /* UNICODE */

#endif /* INCL_WINSOCK_API_TYPEDEFS */
#endif /* NTDDI_VERSION >= NTDDI_VISTA */

#if INCL_WINSOCK_API_PROTOTYPES
#ifdef UNICODE
#define gai_strerror  gai_strerrorW
#else
#define gai_strerror  gai_strerrorA
#endif  /* UNICODE */

#define GAI_STRERROR_BUFFER_SIZE  1024

WS2TCPIP_INLINE char *gai_strerrorA(int ecode)
{
    DWORD dwMsgLen;
    static char buff[GAI_STRERROR_BUFFER_SIZE + 1];

    dwMsgLen = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_MAX_WIDTH_MASK,
        NULL, ecode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)buff, GAI_STRERROR_BUFFER_SIZE, NULL);

    return buff;
}

WS2TCPIP_INLINE WCHAR * gai_strerrorW(int ecode)
{
    DWORD dwMsgLen;
    static WCHAR buff[GAI_STRERROR_BUFFER_SIZE + 1];

    dwMsgLen = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_MAX_WIDTH_MASK,
        NULL, ecode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)buff, GAI_STRERROR_BUFFER_SIZE, NULL);

    return buff;
}
#endif /* INCL_WINSOCK_API_PROTOTYPES */

#define NI_MAXHOST  1025
#define NI_MAXSERV 32

#define NI_NOFQDN  0x01
#define NI_NUMERICHOST  0x02
#define NI_NAMEREQD  0x04
#define NI_NUMERICSERV  0x08
#define NI_DGRAM  0x10

WS2TCPIP_INLINE int setipv4sourcefilter(SOCKET Socket, IN_ADDR Interface, IN_ADDR Group, MULTICAST_MODE_TYPE FilterMode, ULONG SourceCount, CONST IN_ADDR *SourceList)
{
    int Error;
    DWORD Size, Returned;
    PIP_MSFILTER Filter;

    if (SourceCount > (((ULONG) (ULONG_MAX - sizeof(*Filter))) / sizeof(*SourceList)))
    {
        WSASetLastError(WSAENOBUFS);
        return SOCKET_ERROR;
    }

    Size = IP_MSFILTER_SIZE(SourceCount);
    Filter = (PIP_MSFILTER)HeapAlloc(GetProcessHeap(), 0, Size);
    if (Filter == NULL)
    {
        WSASetLastError(WSAENOBUFS);
        return SOCKET_ERROR;
    }

    Filter->imsf_multiaddr = Group;
    Filter->imsf_interface = Interface;
    Filter->imsf_fmode = FilterMode;
    Filter->imsf_numsrc = SourceCount;
    if (SourceCount > 0)
    {
        (void)CopyMemory(Filter->imsf_slist, SourceList, SourceCount * sizeof(*SourceList));
    }

    Error = WSAIoctl(Socket, SIOCSIPMSFILTER, Filter, Size, NULL, 0, &Returned, NULL, NULL);

    (void)HeapFree(GetProcessHeap(), 0, Filter);

    return Error;
}

WS2TCPIP_INLINE int getipv4sourcefilter(SOCKET Socket, IN_ADDR Interface, IN_ADDR Group, MULTICAST_MODE_TYPE *FilterMode, ULONG *SourceCount, IN_ADDR *SourceList)
{
    int Error;
    DWORD Size, Returned;
    PIP_MSFILTER Filter;

    if (*SourceCount > (((ULONG) (ULONG_MAX - sizeof(*Filter))) / sizeof(*SourceList)))
    {
        WSASetLastError(WSAENOBUFS);
        return SOCKET_ERROR;
    }

    Size = IP_MSFILTER_SIZE(*SourceCount);
    Filter = (PIP_MSFILTER)HeapAlloc(GetProcessHeap(), 0, Size);
    if (Filter == NULL)
    {
        WSASetLastError(WSAENOBUFS);
        return SOCKET_ERROR;
    }

    Filter->imsf_multiaddr = Group;
    Filter->imsf_interface = Interface;
    Filter->imsf_numsrc = *SourceCount;

    Error = WSAIoctl(Socket, SIOCGIPMSFILTER, Filter, Size, Filter, Size, &Returned, NULL, NULL);

    if (Error == 0)
    {
        if (*SourceCount > 0)
        {
            (void)CopyMemory(SourceList, Filter->imsf_slist, *SourceCount * sizeof(*SourceList));
            *SourceCount = Filter->imsf_numsrc;
        }
        *FilterMode = Filter->imsf_fmode;
    }

    (void)HeapFree(GetProcessHeap(), 0, Filter);

    return Error;
}

#if (NTDDI_VERSION >= NTDDI_WINXP)
WS2TCPIP_INLINE int setsourcefilter(SOCKET Socket, ULONG Interface, CONST SOCKADDR *Group, int GroupLength, MULTICAST_MODE_TYPE FilterMode, ULONG SourceCount, CONST SOCKADDR_STORAGE *SourceList)
{
    int Error;
    DWORD Size, Returned;
    PGROUP_FILTER Filter;

    if (SourceCount >= (((ULONG) (ULONG_MAX - sizeof(*Filter))) / sizeof(*SourceList)))
    {
        WSASetLastError(WSAENOBUFS);
        return SOCKET_ERROR;
    }

    Size = GROUP_FILTER_SIZE(SourceCount);
    Filter = (PGROUP_FILTER)HeapAlloc(GetProcessHeap(), 0, Size);
    if (Filter == NULL)
    {
        WSASetLastError(WSAENOBUFS);
        return SOCKET_ERROR;
    }

    Filter->gf_interface = Interface;
    (void)ZeroMemory(&Filter->gf_group, sizeof(Filter->gf_group));
    (void)CopyMemory(&Filter->gf_group, Group, GroupLength);
    Filter->gf_fmode = FilterMode;
    Filter->gf_numsrc = SourceCount;
    if (SourceCount > 0)
    {
        (void)CopyMemory(Filter->gf_slist, SourceList, SourceCount * sizeof(*SourceList));
    }

    Error = WSAIoctl(Socket, SIOCSMSFILTER, Filter, Size, NULL, 0, &Returned, NULL, NULL);

    (void)HeapFree(GetProcessHeap(), 0, Filter);

    return Error;
}

WS2TCPIP_INLINE int getsourcefilter(SOCKET Socket, ULONG Interface, CONST SOCKADDR *Group, int GroupLength, MULTICAST_MODE_TYPE *FilterMode, ULONG *SourceCount, SOCKADDR_STORAGE *SourceList)
{
    int Error;
    DWORD Size, Returned;
    PGROUP_FILTER Filter;

    if (*SourceCount > (((ULONG) (ULONG_MAX - sizeof(*Filter))) / sizeof(*SourceList)))
    {
        WSASetLastError(WSAENOBUFS);
        return SOCKET_ERROR;
    }

    Size = GROUP_FILTER_SIZE(*SourceCount);
    Filter = (PGROUP_FILTER)HeapAlloc(GetProcessHeap(), 0, Size);
    if (Filter == NULL)
    {
        WSASetLastError(WSAENOBUFS);
        return SOCKET_ERROR;
    }

    Filter->gf_interface = Interface;
    (void)ZeroMemory(&Filter->gf_group, sizeof(Filter->gf_group));
    (void)CopyMemory(&Filter->gf_group, Group, GroupLength);
    Filter->gf_numsrc = *SourceCount;

    Error = WSAIoctl(Socket, SIOCGMSFILTER, Filter, Size, Filter, Size, &Returned, NULL, NULL);

    if (Error == 0)
    {
        if (*SourceCount > 0)
        {
            (void)CopyMemory(SourceList, Filter->gf_slist, *SourceCount * sizeof(*SourceList));
            *SourceCount = Filter->gf_numsrc;
        }
        *FilterMode = Filter->gf_fmode;
    }

    (void)HeapFree(GetProcessHeap(), 0, Filter);

    return Error;
}
#endif /* NTDDI_VERSION >= NTDDI_WINXP */

#ifdef IDEAL_SEND_BACKLOG_IOCTLS
WS2TCPIP_INLINE int idealsendbacklogquery(SOCKET s, ULONG *pISB)
{
    DWORD bytes;

    return WSAIoctl(s, SIO_IDEAL_SEND_BACKLOG_QUERY, NULL, 0, pISB, sizeof(*pISB), &bytes, NULL, NULL);
}

WS2TCPIP_INLINE int idealsendbacklognotify(SOCKET s, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    DWORD bytes;

    return WSAIoctl(s, SIO_IDEAL_SEND_BACKLOG_CHANGE, NULL, 0, NULL, 0, &bytes, lpOverlapped, lpCompletionRoutine);
}
#endif /* IDEAL_SEND_BACKLOG_IOCTLS */

#if (_WIN32_WINNT >= 0x0600)
#ifdef _SECURE_SOCKET_TYPES_DEFINED_

WINSOCK_API_LINKAGE INT WSAAPI WSASetSocketSecurity(SOCKET Socket, const SOCKET_SECURITY_SETTINGS *, ULONG, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE);
WINSOCK_API_LINKAGE INT WSAAPI WSAQuerySocketSecurity(SOCKET, const SOCKET_SECURITY_QUERY_TEMPLATE *, ULONG, SOCKET_SECURITY_QUERY_INFO *, ULONG *, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE);
WINSOCK_API_LINKAGE INT WSAAPI WSASetSocketPeerTargetName(SOCKET, const SOCKET_PEER_TARGET_NAME *, ULONG, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE);
WINSOCK_API_LINKAGE INT WSAAPI WSADeleteSocketPeerTargetName(SOCKET, const struct sockaddr *, ULONG, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE);
WINSOCK_API_LINKAGE INT WSAAPI WSAImpersonateSocketPeer(SOCKET, const struct sockaddr *, ULONG);
WINSOCK_API_LINKAGE INT WSAAPI WSARevertImpersonation(void);

#endif /* _SECURE_SOCKET_TYPES_DEFINED_ */
#endif /* _WIN32_WINNT >= 0x0600 */

#ifdef __cplusplus
}
#endif

#if !defined(_WIN32_WINNT) || (_WIN32_WINNT <= 0x0500)
#include <wspiapi.h>
#endif

#endif /* _WS2TCPIP_H */
