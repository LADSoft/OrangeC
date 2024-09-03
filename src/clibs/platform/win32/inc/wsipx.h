#ifndef _WSIPX_H
#define _WSIPX_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows Sockets definitions for IPX/SPX */

#define NSPROTO_IPX  1000
#define NSPROTO_SPX  1256
#define NSPROTO_SPXII  1257

typedef struct sockaddr_ipx {
    short sa_family;
    char sa_netnum[4];
    char sa_nodenum[6];
    unsigned short sa_socket;
} SOCKADDR_IPX, *PSOCKADDR_IPX, *LPSOCKADDR_IPX;

#endif /* _WSIPX_H */

