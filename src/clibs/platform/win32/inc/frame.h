#ifndef _FRAME_H
#define _FRAME_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Microsoft Network Monitor definitions */

#ifdef __cplusplus
extern "C" {
#endif


#include <pshpack1.h>

#define MAX_ADDR_LENGTH  6

#define ETHERNET_HEADER_LENGTH  14

#define ETHERNET_DATA_LENGTH  0x05DC
#define ETHERNET_FRAME_LENGTH  0x05EA
#define ETHERNET_FRAME_TYPE  0x0600

#define TOKENRING_HEADER_LENGTH  14

#define TOKENRING_SA_ROUTING_INFO  0x0080
#define TOKENRING_SA_LOCAL  0x0040

#define TOKENRING_DA_LOCAL  0x0040
#define TOKENRING_DA_GROUP  0x0080

#define TOKENRING_RC_LENGTHMASK  0x001F
#define TOKENRING_BC_MASK  0x00E0

#define TOKENRING_TYPE_MAC  0x0000
#define TOKENRING_TYPE_LLC  0x0040

#define FDDI_HEADER_LENGTH  13

#define FDDI_TYPE_MAC  0x00
#define FDDI_TYPE_LLC  0x10
#define FDDI_TYPE_LONG_ADDRESS  0x40

#define IsRoutingInfoPresent(f)  ((((ULPTOKENRING)(f))->SrcAddr[0] & TOKENRING_SA_ROUTING_INFO) ? TRUE : FALSE)
#define GetRoutingInfoLength(f)  (IsRoutingInfoPresent(f) ? (((ULPTOKENRING)(f))->RoutingInfo[0] & TOKENRING_RC_LENGTHMASK) : 0)

typedef struct _ETHERNET {
    BYTE DstAddr[MAX_ADDR_LENGTH];
    BYTE SrcAddr[MAX_ADDR_LENGTH];
    union {
        WORD Length;
        WORD Type;
    };
    BYTE Info[];
} ETHERNET, *LPETHERNET, UNALIGNED *ULPETHERNET;
#define ETHERNET_SIZE  sizeof(ETHERNET)

typedef struct _TOKENRING {
    BYTE AccessCtrl;
    BYTE FrameCtrl;
    BYTE DstAddr[MAX_ADDR_LENGTH];
    BYTE SrcAddr[MAX_ADDR_LENGTH];
    union {
        BYTE Info[];
        WORD RoutingInfo[];
    };
} TOKENRING, *LPTOKENRING, UNALIGNED *ULPTOKENRING;
#define TOKENRING_SIZE  sizeof(TOKENRING)

typedef struct _FDDI {
    BYTE FrameCtrl;
    BYTE DstAddr[MAX_ADDR_LENGTH];
    BYTE SrcAddr[MAX_ADDR_LENGTH];
    BYTE Info[];
} FDDI, *LPFDDI, UNALIGNED *ULPFDDI;
#define FDDI_SIZE  sizeof(FDDI)

typedef struct _LLC {
    BYTE dsap;
    BYTE ssap;
    struct {
        union {
            BYTE Command;
            BYTE NextSend;
        };
        union {
            BYTE NextRecv;
            BYTE Data[1];
        };
    } ControlField;
} LLC, *LPLLC, UNALIGNED *ULPLLC;
#define LLC_SIZE  sizeof(LLC)

#include <poppack.h>


#ifdef __cplusplus
}
#endif

#endif /* _FRAME_H */
