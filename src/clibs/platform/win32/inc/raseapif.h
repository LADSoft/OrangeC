#ifndef _RASEAPIF_H
#define _RASEAPIF_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Remote Access Service PPP engine and third party authentication module interface definitions */

#ifdef __cplusplus
extern "C" {
#endif


#if (WINVER >= 0x0500)

#define RAS_EAP_REGISTRY_LOCATION  TEXT("System\\CurrentControlSet\\Services\\Rasman\\PPP\\EAP")

#define RAS_EAP_VALUENAME_PATH  TEXT("Path")
#define RAS_EAP_VALUENAME_CONFIGUI  TEXT("ConfigUIPath")
#define RAS_EAP_VALUENAME_INTERACTIVEUI  TEXT("InteractiveUIPath")
#define RAS_EAP_VALUENAME_IDENTITY  TEXT("IdentityPath")
#define RAS_EAP_VALUENAME_FRIENDLY_NAME  TEXT("FriendlyName")
#define RAS_EAP_VALUENAME_DEFAULT_DATA  TEXT("ConfigData")
#define RAS_EAP_VALUENAME_REQUIRE_CONFIGUI  TEXT("RequireConfigUI")
#define RAS_EAP_VALUENAME_ENCRYPTION  TEXT("MPPEEncryptionSupported")
#define RAS_EAP_VALUENAME_INVOKE_NAMEDLG  TEXT("InvokeUsernameDialog")
#define RAS_EAP_VALUENAME_INVOKE_PWDDLG  TEXT("InvokePasswordDialog")
#define RAS_EAP_VALUENAME_CONFIG_CLSID  TEXT("ConfigCLSID")
#define RAS_EAP_VALUENAME_STANDALONE_SUPPORTED  TEXT("StandaloneSupported")

#define raatARAPChallenge  33
#define raatARAPOldPassword  19
#define raatARAPNewPassword  20
#define raatARAPPasswordChangeReason  21

#define EAPCODE_Request  1
#define EAPCODE_Response  2
#define EAPCODE_Success  3
#define EAPCODE_Failure  4

#define MAXEAPCODE  4

#define RAS_EAP_FLAG_ROUTER  0x00000001
#define RAS_EAP_FLAG_NON_INTERACTIVE  0x00000002
#define RAS_EAP_FLAG_LOGON  0x00000004
#define RAS_EAP_FLAG_PREVIEW  0x00000008
#define RAS_EAP_FLAG_FIRST_LINK  0x00000010

#define PPP_EAP_PACKET_HDR_LEN  (sizeof(PPP_EAP_PACKET)-1)

typedef enum _RAS_AUTH_ATTRIBUTE_TYPE_ {
    raatMinimum = 0,
    raatUserName,
    raatUserPassword,
    raatMD5CHAPPassword,
    raatNASIPAddress,
    raatNASPort,
    raatServiceType,
    raatFramedProtocol,
    raatFramedIPAddress,
    raatFramedIPNetmask,
    raatFramedRouting = 10,
    raatFilterId,
    raatFramedMTU,
    raatFramedCompression,
    raatLoginIPHost,
    raatLoginService,
    raatLoginTCPPort,
    raatUnassigned17,
    raatReplyMessage,
    raatCallbackNumber,
    raatCallbackId = 20,
    raatUnassigned21,
    raatFramedRoute,
    raatFramedIPXNetwork,
    raatState,
    raatClass,
    raatVendorSpecific,
    raatSessionTimeout,
    raatIdleTimeout,
    raatTerminationAction,
    raatCalledStationId = 30,
    raatCallingStationId,
    raatNASIdentifier,
    raatProxyState,
    raatLoginLATService,
    raatLoginLATNode,
    raatLoginLATGroup,
    raatFramedAppleTalkLink,
    raatFramedAppleTalkNetwork,
    raatFramedAppleTalkZone,
    raatAcctStatusType = 40,
    raatAcctDelayTime,
    raatAcctInputOctets,
    raatAcctOutputOctets,
    raatAcctSessionId,
    raatAcctAuthentic,
    raatAcctSessionTime,
    raatAcctInputPackets,
    raatAcctOutputPackets,
    raatAcctTerminateCause,
    raatAcctMultiSessionId = 50,
    raatAcctLinkCount,
    raatAcctEventTimeStamp = 55,
    raatMD5CHAPChallenge = 60,
    raatNASPortType,
    raatPortLimit,
    raatLoginLATPort,
    raatTunnelType,
    raatTunnelMediumType,
    raatTunnelClientEndpoint,
    raatTunnelServerEndpoint,
    raatARAPPassword = 70,
    raatARAPFeatures,
    raatARAPZoneAccess,
    raatARAPSecurity,
    raatARAPSecurityData,
    raatPasswordRetry,
    raatPrompt,
    raatConnectInfo,
    raatConfigurationToken,
    raatEAPMessage,
    raatSignature = 80,
    raatARAPChallengeResponse = 84,
    raatAcctInterimInterval = 85,
    raatARAPGuestLogon = 8096,
    raatReserved = 0xFFFFFFFF
} RAS_AUTH_ATTRIBUTE_TYPE;

typedef struct _RAS_AUTH_ATTRIBUTE {
    RAS_AUTH_ATTRIBUTE_TYPE raaType;
    DWORD dwLength;
    PVOID Value;
} RAS_AUTH_ATTRIBUTE, *PRAS_AUTH_ATTRIBUTE;

typedef struct _PPP_EAP_PACKET {
    BYTE Code;
    BYTE Id;
    BYTE Length[2];
    BYTE Data[1];
} PPP_EAP_PACKET, *PPPP_EAP_PACKET;

typedef struct _PPP_EAP_INPUT {
    DWORD dwSizeInBytes;
    DWORD fFlags;
    BOOL fAuthenticator;
    WCHAR *pwszIdentity;
    WCHAR *pwszPassword;
    BYTE bInitialId;
    RAS_AUTH_ATTRIBUTE *pUserAttributes;
    BOOL fAuthenticationComplete;
    DWORD dwAuthResultCode;
    HANDLE hTokenImpersonateUser;
    BOOL fSuccessPacketReceived;
    BOOL fDataReceivedFromInteractiveUI;
    PBYTE pDataFromInteractiveUI;
    DWORD dwSizeOfDataFromInteractiveUI;
    PBYTE pConnectionData;
    DWORD dwSizeOfConnectionData;
    PBYTE pUserData;
    DWORD dwSizeOfUserData;
    HANDLE hReserved;
} PPP_EAP_INPUT, *PPPP_EAP_INPUT;

typedef enum _PPP_EAP_ACTION {
    EAPACTION_NoAction,
    EAPACTION_Authenticate,
    EAPACTION_Done,
    EAPACTION_SendAndDone,
    EAPACTION_Send,
    EAPACTION_SendWithTimeout,
    EAPACTION_SendWithTimeoutInteractive
} PPP_EAP_ACTION;

typedef struct _PPP_EAP_OUTPUT {
    DWORD dwSizeInBytes;
    PPP_EAP_ACTION Action;
    DWORD dwAuthResultCode;
    RAS_AUTH_ATTRIBUTE *pUserAttributes;
    BOOL fInvokeInteractiveUI;
    PBYTE pUIContextData;
    DWORD dwSizeOfUIContextData;
    BOOL fSaveConnectionData;
    PBYTE pConnectionData;
    DWORD dwSizeOfConnectionData;
    BOOL fSaveUserData;
    PBYTE pUserData;
    DWORD dwSizeOfUserData;
} PPP_EAP_OUTPUT, *PPPP_EAP_OUTPUT;

typedef struct _PPP_EAP_INFO {
    DWORD dwSizeInBytes;
    DWORD dwEapTypeId;
    DWORD(APIENTRY *RasEapInitialize)(BOOL);
    DWORD(APIENTRY *RasEapBegin)(VOID**,PPP_EAP_INPUT*);
    DWORD(APIENTRY *RasEapEnd)(VOID*);
    DWORD(APIENTRY *RasEapMakeMessage)(VOID*,PPP_EAP_PACKET*,PPP_EAP_PACKET*,DWORD,PPP_EAP_OUTPUT*,PPP_EAP_INPUT*);
} PPP_EAP_INFO, *PPPP_EAP_INFO;

DWORD APIENTRY RasEapGetInfo(DWORD,PPP_EAP_INFO*);
DWORD APIENTRY RasEapFreeMemory(BYTE*);
DWORD APIENTRY RasEapInvokeInteractiveUI(DWORD,HWND,BYTE*,DWORD,BYTE**,DWORD*);
DWORD APIENTRY RasEapInvokeConfigUI(DWORD,HWND,DWORD,BYTE*,DWORD,BYTE**,DWORD*);
DWORD APIENTRY RasEapGetIdentity(DWORD,HWND,DWORD,const WCHAR*,const WCHAR*,BYTE*,DWORD,BYTE*,DWORD,BYTE**,DWORD*,WCHAR**);

#endif /* WINVER >= 0x0500 */


#ifdef __cplusplus
}
#endif

#endif /* _RASEAPIF_H */
