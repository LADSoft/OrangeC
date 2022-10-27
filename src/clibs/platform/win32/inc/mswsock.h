#ifndef _MSWSOCK_H
#define _MSWSOCK_H

#ifdef __ORANGEC__ 
#pragma once
#endif

/* Microsoft extensions to Windows Sockets API */

#ifdef __cplusplus
extern "C" {
#endif

#if __POCC__ >= 290
#pragma warn(push)
#pragma warn(disable:2198)  /* Nameless field is not standard */
#endif

#include <mswsockdef.h>

#define SO_CONNDATA  0x7000
#define SO_CONNOPT  0x7001
#define SO_DISCDATA  0x7002
#define SO_DISCOPT  0x7003
#define SO_CONNDATALEN  0x7004
#define SO_CONNOPTLEN  0x7005
#define SO_DISCDATALEN  0x7006
#define SO_DISCOPTLEN  0x7007
#define SO_OPENTYPE  0x7008
#define SO_MAXDG  0x7009
#define SO_MAXPATHDG  0x700A
#define SO_UPDATE_ACCEPT_CONTEXT  0x700B
#define SO_CONNECT_TIME  0x700C
#if (_WIN32_WINNT >= 0x0501)
#define SO_UPDATE_CONNECT_CONTEXT  0x7010
#endif /* _WIN32_WINNT >= 0x0501 */

#define SO_SYNCHRONOUS_ALERT  0x10
#define SO_SYNCHRONOUS_NONALERT  0x20

#define TCP_BSDURGENT  0x7000

#define SIO_UDP_CONNRESET  _WSAIOW(IOC_VENDOR,12)

#if (_WIN32_WINNT < 0x0600 && _WIN32_WINNT >= 0x0501)
#define SIO_SOCKET_CLOSE_NOTIFY  _WSAIOW(IOC_VENDOR,13)
#endif /* _WIN32_WINNT < 0x0600 && _WIN32_WINNT >= 0x0501 */

#define SIO_UDP_NETRESET  _WSAIOW(IOC_VENDOR,15)

#define TF_DISCONNECT  0x01
#define TF_REUSE_SOCKET  0x02
#define TF_WRITE_BEHIND  0x04
#define TF_USE_DEFAULT_WORKER  0x00
#define TF_USE_SYSTEM_THREAD  0x10
#define TF_USE_KERNEL_APC  0x20

#define WSAID_TRANSMITFILE  {0xb5367df0,0xcbac,0x11cf,{0x95,0xca,0x00,0x80,0x5f,0x48,0xa1,0x92}}
#define WSAID_ACCEPTEX  {0xb5367df1,0xcbac,0x11cf,{0x95,0xca,0x00,0x80,0x5f,0x48,0xa1,0x92}}
#define WSAID_GETACCEPTEXSOCKADDRS  {0xb5367df2,0xcbac,0x11cf,{0x95,0xca,0x00,0x80,0x5f,0x48,0xa1,0x92}}

#if (_WIN32_WINNT < 0x0600)
int PASCAL WSARecvEx(SOCKET, char *, int, int *);
#else /* _WIN32_WINNT >= 0x0600 */
INT PASCAL WSARecvEx(SOCKET, CHAR *, INT, INT *);
#endif /* _WIN32_WINNT >= 0x0600 */

typedef struct _TRANSMIT_FILE_BUFFERS {
    LPVOID Head;
    DWORD HeadLength;
    LPVOID Tail;
    DWORD TailLength;
} TRANSMIT_FILE_BUFFERS, *PTRANSMIT_FILE_BUFFERS, *LPTRANSMIT_FILE_BUFFERS;

typedef BOOL (PASCAL *LPFN_TRANSMITFILE)(SOCKET, HANDLE, DWORD, DWORD, LPOVERLAPPED, LPTRANSMIT_FILE_BUFFERS, DWORD);
typedef BOOL (PASCAL *LPFN_ACCEPTEX)(SOCKET, SOCKET, PVOID, DWORD, DWORD, DWORD, LPDWORD, LPOVERLAPPED);
typedef VOID (PASCAL *LPFN_GETACCEPTEXSOCKADDRS)(PVOID, DWORD, DWORD, DWORD, struct sockaddr **, LPINT, struct sockaddr **, LPINT);

BOOL PASCAL TransmitFile(SOCKET, HANDLE, DWORD, DWORD, LPOVERLAPPED, LPTRANSMIT_FILE_BUFFERS, DWORD);
BOOL PASCAL AcceptEx(SOCKET, SOCKET, PVOID, DWORD, DWORD, DWORD, LPDWORD, LPOVERLAPPED);
VOID PASCAL GetAcceptExSockaddrs(PVOID, DWORD, DWORD, DWORD, struct sockaddr **, LPINT, struct sockaddr **, LPINT);

#if (_WIN32_WINNT >= 0x0501)

#define TP_ELEMENT_MEMORY  1
#define TP_ELEMENT_FILE  2
#define TP_ELEMENT_EOP  4

#define TP_DISCONNECT  TF_DISCONNECT
#define TP_REUSE_SOCKET  TF_REUSE_SOCKET
#define TP_USE_DEFAULT_WORKER  TF_USE_DEFAULT_WORKER
#define TP_USE_SYSTEM_THREAD  TF_USE_SYSTEM_THREAD
#define TP_USE_KERNEL_APC  TF_USE_KERNEL_APC

#define WSAID_TRANSMITPACKETS  {0xd9689da0,0x1f90,0x11d3,{0x99,0x71,0x00,0xc0,0x4f,0x68,0xc8,0x76}}
#define WSAID_CONNECTEX  {0x25a207b9,0xddf3,0x4660,{0x8e,0xe9,0x76,0xe5,0x8c,0x74,0x06,0x3e}}
#define WSAID_DISCONNECTEX  {0x7fda2e11,0x8630,0x436f,{0xa0, 0x31, 0xf5, 0x36,0xa6, 0xee, 0xc1,0x57}}
#define WSAID_WSARECVMSG  {0xf689d7c8,0x6f1f,0x436b,{0x8a,0x53,0xe5,0x4f,0xe3,0x51,0xc3,0x22}}

#define NLA_NAMESPACE_GUID  {0x6642243a,0x3ba8,0x4aa6,{0xba,0xa5,0x2e,0xb,0xd7,0x1f,0xdd,0x83}}
#define NLA_SERVICE_CLASS_GUID  {0x37e515,0xb5c9,0x4a43,{0xba,0xda,0x8b,0x48,0xa8,0x7a,0xd2,0x39}}

#define DE_REUSE_SOCKET  TF_REUSE_SOCKET

#define NLA_ALLUSERS_NETWORK  0x00000001
#define NLA_FRIENDLY_NAME  0x00000002

typedef struct _TRANSMIT_PACKETS_ELEMENT {
    ULONG dwElFlags;
    ULONG cLength;
    union {
        struct {
            LARGE_INTEGER nFileOffset;
            HANDLE hFile;
        };
        PVOID pBuffer;
    };
} TRANSMIT_PACKETS_ELEMENT, *PTRANSMIT_PACKETS_ELEMENT, *LPTRANSMIT_PACKETS_ELEMENT;

typedef enum _NLA_BLOB_DATA_TYPE {
    NLA_RAW_DATA = 0,
    NLA_INTERFACE = 1,
    NLA_802_1X_LOCATION = 2,
    NLA_CONNECTIVITY = 3,
    NLA_ICS = 4,
} NLA_BLOB_DATA_TYPE, *PNLA_BLOB_DATA_TYPE;

typedef enum _NLA_CONNECTIVITY_TYPE {
    NLA_NETWORK_AD_HOC = 0,
    NLA_NETWORK_MANAGED = 1,
    NLA_NETWORK_UNMANAGED = 2,
    NLA_NETWORK_UNKNOWN = 3,
} NLA_CONNECTIVITY_TYPE, *PNLA_CONNECTIVITY_TYPE;

typedef enum _NLA_INTERNET {
    NLA_INTERNET_UNKNOWN = 0,
    NLA_INTERNET_NO = 1,
    NLA_INTERNET_YES = 2,
} NLA_INTERNET, *PNLA_INTERNET;

typedef struct _NLA_BLOB {
    struct {
        NLA_BLOB_DATA_TYPE type;
        DWORD dwSize;
        DWORD nextOffset;
    } header;
    union {
        CHAR rawData[1];
        struct {
            DWORD dwType;
            DWORD dwSpeed;
            CHAR adapterName[1];
        } interfaceData;
        struct {
            CHAR information[1];
        } locationData;
        struct {
            NLA_CONNECTIVITY_TYPE type;
            NLA_INTERNET internet;
        } connectivity;
        struct {
            struct {
                DWORD speed;
                DWORD type;
                DWORD state;
                WCHAR machineName[256];
                WCHAR sharedAdapterName[256];
            } remote;
        } ICS;
    } data;
} NLA_BLOB, *PNLA_BLOB, *LPNLA_BLOB;

typedef BOOL (PASCAL *LPFN_TRANSMITPACKETS)(SOCKET, LPTRANSMIT_PACKETS_ELEMENT, DWORD, DWORD, LPOVERLAPPED, DWORD);
typedef BOOL (PASCAL *LPFN_CONNECTEX)(SOCKET, const struct sockaddr *, int, PVOID, DWORD, LPDWORD, LPOVERLAPPED);
typedef BOOL (PASCAL *LPFN_DISCONNECTEX)(SOCKET, LPOVERLAPPED, DWORD, DWORD);
typedef INT (PASCAL *LPFN_WSARECVMSG)(SOCKET, LPWSAMSG, LPDWORD, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE);

#endif /* _WIN32_WINNT >= 0x0501 */

#if (_WIN32_WINNT >= 0x0600)

#define SIO_BSP_HANDLE  _WSAIOR(IOC_WS2,27)
#define SIO_BSP_HANDLE_SELECT  _WSAIOR(IOC_WS2,28)
#define SIO_BSP_HANDLE_POLL  _WSAIOR(IOC_WS2,29)

#define SIO_BASE_HANDLE  _WSAIOR(IOC_WS2,34)

#define SIO_EXT_SELECT  _WSAIORW(IOC_WS2,30)
#define SIO_EXT_POLL  _WSAIORW(IOC_WS2,31)
#define SIO_EXT_SENDMSG  _WSAIORW(IOC_WS2,32)

#define WSAID_WSASENDMSG  {0xa441e712,0x754f,0x43ca,{0x84,0xa7,0x0d,0xee,0x44,0xcf,0x60,0x6d}}
#define WSAID_WSAPOLL  {0x18C76F85,0xDC66,0x4964,{0x97,0x2E,0x23,0xC2,0x72,0x38,0x31,0x2B}}

typedef struct {
    int result;
    ULONG fds;
    INT timeout;
    WSAPOLLFD fdArray[0];
} WSAPOLLDATA, *LPWSAPOLLDATA;

typedef struct {
    LPWSAMSG lpMsg;
    DWORD dwFlags;
    LPDWORD lpNumberOfBytesSent;
    LPWSAOVERLAPPED lpOverlapped;
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine;
} WSASENDMSG, *LPWSASENDMSG;

typedef INT (PASCAL *LPFN_WSASENDMSG)(SOCKET, LPWSAMSG, DWORD, LPDWORD, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE);
typedef INT (WSAAPI *LPFN_WSAPOLL)(LPWSAPOLLFD, ULONG, INT);

#endif /* _WIN32_WINNT >= 0x0600 */

#if __POCC__ >= 290
#pragma warn(pop)
#endif

#ifdef __cplusplus
}
#endif

#endif  /* _MSWSOCK_H */
