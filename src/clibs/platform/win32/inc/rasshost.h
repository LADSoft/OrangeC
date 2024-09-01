#ifndef _RASSHOST_H
#define _RASSHOST_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Remote Access Server and third party security DLL definitions */

#include <mprapi.h>

#define SECURITYMSG_SUCCESS  1
#define SECURITYMSG_FAILURE  2
#define SECURITYMSG_ERROR  3

typedef DWORD HPORT;

typedef struct _SECURITY_MESSAGE {
    DWORD dwMsgId;
    HPORT hPort;
    DWORD dwError;
    CHAR UserName[UNLEN+1];
    CHAR Domain[DNLEN+1];

} SECURITY_MESSAGE, *PSECURITY_MESSAGE;

typedef struct _RAS_SECURITY_INFO {
    DWORD LastError;
    DWORD BytesReceived;
    CHAR DeviceName[MAX_DEVICE_NAME+1];
} RAS_SECURITY_INFO, *PRAS_SECURITY_INFO;

typedef DWORD (WINAPI *RASSECURITYPROC)();

VOID WINAPI RasSecurityDialogComplete(SECURITY_MESSAGE*);
DWORD WINAPI RasSecurityDialogBegin(HPORT,PBYTE,DWORD,PBYTE,DWORD,VOID (WINAPI *)(SECURITY_MESSAGE*));
DWORD WINAPI RasSecurityDialogEnd(HPORT);
DWORD WINAPI RasSecurityDialogSend(HPORT,PBYTE,WORD);
DWORD WINAPI RasSecurityDialogReceive(HPORT,PBYTE,PWORD,DWORD,HANDLE);
DWORD WINAPI RasSecurityDialogGetInfo(HPORT,RAS_SECURITY_INFO*);

#endif /* _RASSHOST_H */
