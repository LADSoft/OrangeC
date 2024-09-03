#ifndef _WLCLIENT_H
#define _WLCLIENT_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows wireless API definitions */

#include <windot11.h>

#if (NTDDI_VERSION >= NTDDI_VISTA)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _DOT11_ADAPTER {
    GUID gAdapterId;
    LPWSTR pszDescription;
    DOT11_CURRENT_OPERATION_MODE Dot11CurrentOpMode;
} DOT11_ADAPTER, *PDOT11_ADAPTER;

typedef struct _DOT11_BSS_LIST {
    ULONG uNumOfBytes;
    PUCHAR pucBuffer;
} DOT11_BSS_LIST, *PDOT11_BSS_LIST;

typedef struct _DOT11_PORT_STATE {
    DOT11_MAC_ADDRESS PeerMacAddress;
    ULONG uSessionId;
    BOOL bPortControlled;
    BOOL bPortAuthorized;
} DOT11_PORT_STATE, *PDOT11_PORT_STATE;

#include <pshpack1.h>
typedef struct _DOT11_SECURITY_PACKET_HEADER {
    DOT11_MAC_ADDRESS PeerMac;
    USHORT usEtherType;
    UCHAR Data[1];
} DOT11_SECURITY_PACKET_HEADER, *PDOT11_SECURITY_PACKET_HEADER;
#include <poppack.h>

#ifdef __cplusplus
}
#endif

#endif /* NTDDI_VERSION > NTDDI_VISTA */

#endif /* _WLCLIENT_H */
