#ifndef _RASSAPI_H
#define _RASSAPI_H

#ifdef __ORANGEC__ 
#pragma once
#endif

/* Remote Access admin API definitions */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UNLEN
#include <lmcons.h>
#endif

#define RASSAPI_MAX_PHONENUMBER_SIZE  128
#define RASSAPI_MAX_MEDIA_NAME  16
#define RASSAPI_MAX_PORT_NAME  16
#define RASSAPI_MAX_DEVICE_NAME  128
#define RASSAPI_MAX_DEVICETYPE_NAME  16
#define RASSAPI_MAX_PARAM_KEY_SIZE  32

#define RASPRIV_NoCallback  0x01
#define RASPRIV_AdminSetCallback  0x02
#define RASPRIV_CallerSetCallback 0x04
#define RASPRIV_DialinPrivilege  0x08

#define RASPRIV_CallbackType  (RASPRIV_AdminSetCallback|RASPRIV_CallerSetCallback|RASPRIV_NoCallback)

#define RAS_MODEM_OPERATIONAL  1
#define RAS_MODEM_NOT_RESPONDING  2
#define RAS_MODEM_HARDWARE_FAILURE  3
#define RAS_MODEM_INCORRECT_RESPONSE 4
#define RAS_MODEM_UNKNOWN  5

#define RAS_PORT_NON_OPERATIONAL 1
#define RAS_PORT_DISCONNECTED  2
#define RAS_PORT_CALLING_BACK  3
#define RAS_PORT_LISTENING  4
#define RAS_PORT_AUTHENTICATING  5
#define RAS_PORT_AUTHENTICATED  6
#define RAS_PORT_INITIALIZING  7

#define MEDIA_UNKNOWN  0
#define MEDIA_SERIAL  1
#define MEDIA_RAS10_SERIAL  2
#define MEDIA_X25  3
#define MEDIA_ISDN  4

#define USER_AUTHENTICATED  0x0001
#define MESSENGER_PRESENT  0x0002
#define PPP_CLIENT  0x0004
#define GATEWAY_ACTIVE  0x0008
#define REMOTE_LISTEN  0x0010
#define PORT_MULTILINKED  0x0020

#define RAS_IPADDRESSLEN  15
#define RAS_IPXADDRESSLEN  22
#define RAS_ATADDRESSLEN  32

#define RASDOWNLEVEL  10
#define RASADMIN_35  35
#define RASADMIN_CURRENT  40

enum RAS_PARAMS_FORMAT {
    ParamNumber = 0,
    ParamString = 1
};
typedef enum RAS_PARAMS_FORMAT RAS_PARAMS_FORMAT;

union RAS_PARAMS_VALUE {
    DWORD Number;
    struct {
        DWORD Length;
        PCHAR Data;
    } String;
};
typedef union RAS_PARAMS_VALUE RAS_PARAMS_VALUE;

struct RAS_PARAMETERS {
    CHAR P_Key[RASSAPI_MAX_PARAM_KEY_SIZE];
    RAS_PARAMS_FORMAT P_Type;
    BYTE P_Attributes;
    RAS_PARAMS_VALUE P_Value;
};
typedef struct RAS_PARAMETERS RAS_PARAMETERS;

#ifndef __PELLES_C_IMPDEF__
typedef struct _RAS_USER_0 {
    BYTE bfPrivilege;
    WCHAR szPhoneNumber[RASSAPI_MAX_PHONENUMBER_SIZE+1];
} RAS_USER_0, *PRAS_USER_0;
#endif

#ifndef __PELLES_C_IMPDEF__
typedef struct _RAS_PORT_0 {
    WCHAR wszPortName[RASSAPI_MAX_PORT_NAME];
    WCHAR wszDeviceType[RASSAPI_MAX_DEVICETYPE_NAME];
    WCHAR wszDeviceName[RASSAPI_MAX_DEVICE_NAME];
    WCHAR wszMediaName[RASSAPI_MAX_MEDIA_NAME];
    DWORD reserved;
    DWORD Flags;
    WCHAR wszUserName[UNLEN+1];
    WCHAR wszComputer[NETBIOS_NAME_LEN];
    DWORD dwStartSessionTime;
    WCHAR wszLogonDomain[DNLEN+1];
    BOOL fAdvancedServer;
} RAS_PORT_0, *PRAS_PORT_0;
#endif

typedef ULONG IPADDR;

typedef struct _RAS_PPP_NBFCP_RESULT {
    DWORD dwError;
    DWORD dwNetBiosError;
    CHAR szName[NETBIOS_NAME_LEN+1];
    WCHAR wszWksta[NETBIOS_NAME_LEN+1];
} RAS_PPP_NBFCP_RESULT;

typedef struct _RAS_PPP_IPCP_RESULT {
    DWORD dwError;
    WCHAR wszAddress[RAS_IPADDRESSLEN+1];
} RAS_PPP_IPCP_RESULT;

typedef struct _RAS_PPP_IPXCP_RESULT {
    DWORD dwError;
    WCHAR wszAddress[RAS_IPXADDRESSLEN+1];
} RAS_PPP_IPXCP_RESULT;

typedef struct _RAS_PPP_ATCP_RESULT {
    DWORD dwError;
    WCHAR wszAddress[RAS_ATADDRESSLEN+1];
} RAS_PPP_ATCP_RESULT;

typedef struct _RAS_PPP_PROJECTION_RESULT {
    RAS_PPP_NBFCP_RESULT nbf;
    RAS_PPP_IPCP_RESULT ip;
    RAS_PPP_IPXCP_RESULT ipx;
    RAS_PPP_ATCP_RESULT at;
} RAS_PPP_PROJECTION_RESULT;

#ifndef __PELLES_C_IMPDEF__
typedef struct _RAS_PORT_1 {
    RAS_PORT_0 rasport0;
    DWORD LineCondition;
    DWORD HardwareCondition;
    DWORD LineSpeed;
    WORD NumStatistics;
    WORD NumMediaParms;
    DWORD SizeMediaParms;
    RAS_PPP_PROJECTION_RESULT ProjResult;
} RAS_PORT_1, *PRAS_PORT_1;
#endif

typedef struct _RAS_PORT_STATISTICS {
    DWORD dwBytesXmited;
    DWORD dwBytesRcved;
    DWORD dwFramesXmited;
    DWORD dwFramesRcved;
    DWORD dwCrcErr;
    DWORD dwTimeoutErr;
    DWORD dwAlignmentErr;
    DWORD dwHardwareOverrunErr;
    DWORD dwFramingErr;
    DWORD dwBufferOverrunErr;
    DWORD dwBytesXmitedUncompressed;
    DWORD dwBytesRcvedUncompressed;
    DWORD dwBytesXmitedCompressed;
    DWORD dwBytesRcvedCompressed;
    DWORD dwPortBytesXmited;
    DWORD dwPortBytesRcved;
    DWORD dwPortFramesXmited;
    DWORD dwPortFramesRcved;
    DWORD dwPortCrcErr;
    DWORD dwPortTimeoutErr;
    DWORD dwPortAlignmentErr;
    DWORD dwPortHardwareOverrunErr;
    DWORD dwPortFramingErr;
    DWORD dwPortBufferOverrunErr;
    DWORD dwPortBytesXmitedUncompressed;
    DWORD dwPortBytesRcvedUncompressed;
    DWORD dwPortBytesXmitedCompressed;
    DWORD dwPortBytesRcvedCompressed;
} RAS_PORT_STATISTICS, *PRAS_PORT_STATISTICS;

typedef struct _RAS_SERVER_0 {
    WORD TotalPorts;
    WORD PortsInUse;
    DWORD RasVersion;
} RAS_SERVER_0, *PRAS_SERVER_0;

DWORD APIENTRY RasAdminServerGetInfo(const WCHAR*,PRAS_SERVER_0);
DWORD APIENTRY RasAdminGetUserAccountServer(const WCHAR*,const WCHAR*,LPWSTR);
DWORD APIENTRY RasAdminUserGetInfo(const WCHAR*,const WCHAR*,PRAS_USER_0);
DWORD APIENTRY RasAdminUserSetInfo(const WCHAR*,const WCHAR*,const PRAS_USER_0);
DWORD APIENTRY RasAdminPortEnum(const WCHAR*,PRAS_PORT_0*,WORD*);
DWORD APIENTRY RasAdminPortGetInfo(const WCHAR*,const WCHAR*,RAS_PORT_1*,RAS_PORT_STATISTICS*,RAS_PARAMETERS**);
DWORD APIENTRY RasAdminPortClearStatistics(const WCHAR*,const WCHAR*);
DWORD APIENTRY RasAdminPortDisconnect(const WCHAR*,const WCHAR*);
DWORD APIENTRY RasAdminFreeBuffer(PVOID);
DWORD APIENTRY RasAdminGetErrorString(UINT,WCHAR*,DWORD);
BOOL APIENTRY RasAdminAcceptNewConnection(RAS_PORT_1*,RAS_PORT_STATISTICS*,RAS_PARAMETERS*);
VOID APIENTRY RasAdminConnectionHangupNotification(RAS_PORT_1*,RAS_PORT_STATISTICS*,RAS_PARAMETERS*);
DWORD APIENTRY RasAdminGetIpAddressForUser(WCHAR*,WCHAR*,IPADDR*,BOOL*);
VOID APIENTRY RasAdminReleaseIpAddress(WCHAR*,WCHAR*,IPADDR*);
DWORD APIENTRY RasAdminGetUserParms(WCHAR*,PRAS_USER_0);
DWORD APIENTRY RasAdminSetUserParms(WCHAR*,DWORD,PRAS_USER_0);

#ifdef __cplusplus
}
#endif

#endif /* _RASSAPI_H */
