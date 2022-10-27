#ifndef _SRRESTOREPTAPI_H
#define _SRRESTOREPTAPI_H

/* System restore point API definitions */

#define MIN_EVENT  100
#define BEGIN_SYSTEM_CHANGE  100
#define END_SYSTEM_CHANGE  101
#define BEGIN_NESTED_SYSTEM_CHANGE  102
#define END_NESTED_SYSTEM_CHANGE  103
#define BEGIN_NESTED_SYSTEM_CHANGE_NORP  104
#define END_NESTED_SYSTEM_CHANGE_NORP  END_NESTED_SYSTEM_CHANGE
#define MAX_EVENT  104

#define MIN_RPT  0
#define APPLICATION_INSTALL  0
#define APPLICATION_UNINSTALL  1
#define DESKTOP_SETTING  2
#define ACCESSIBILITY_SETTING  3
#define OE_SETTING  4
#define APPLICATION_RUN  5
#define RESTORE  6
#define CHECKPOINT  7
#define WINDOWS_SHUTDOWN  8
#define WINDOWS_BOOT  9
#define DEVICE_DRIVER_INSTALL  10
#define FIRSTRUN  11
#define MODIFY_SETTINGS  12
#define CANCELLED_OPERATION  13
#define BACKUP_RECOVERY  14
#define BACKUP  15
#define MANUAL_CHECKPOINT  16
#define WINDOWS_UPDATE  17
#define CRITICAL_UPDATE  18
#define MAX_RPT  18

#define MAX_DESC  64
#define MAX_DESC_W  256

#if __POCC__ >= 290
#pragma warn(push)
#pragma warn(disable:2185)  /* Alignment of field 'x' can be less than the natural alignment */
#endif

#include <pshpack1.h>

typedef struct _RESTOREPTINFOA {
    DWORD dwEventType;
    DWORD dwRestorePtType;
    INT64 llSequenceNumber;
    CHAR szDescription[MAX_DESC];
} RESTOREPOINTINFOA, *PRESTOREPOINTINFOA;

typedef struct _RESTOREPTINFOW {
    DWORD dwEventType;
    DWORD dwRestorePtType;
    INT64 llSequenceNumber;
    WCHAR szDescription[MAX_DESC_W];
} RESTOREPOINTINFOW, *PRESTOREPOINTINFOW;

typedef struct _RESTOREPTINFOEX {
    FILETIME ftCreation;
    DWORD dwEventType;
    DWORD dwRestorePtType;
    DWORD dwRPNum;
    WCHAR szDescription[MAX_DESC_W];
} RESTOREPOINTINFOEX, *PRESTOREPOINTINFOEX;

typedef struct _SMGRSTATUS {
    DWORD nStatus;
    INT64 llSequenceNumber;
} STATEMGRSTATUS, *PSTATEMGRSTATUS;

#include <poppack.h>

#if __POCC__ >= 290
#pragma warn(pop)
#endif

#ifdef __cplusplus
extern "C" {
#endif

BOOL __stdcall SRSetRestorePointA(PRESTOREPOINTINFOA, PSTATEMGRSTATUS);
BOOL __stdcall SRSetRestorePointW(PRESTOREPOINTINFOW, PSTATEMGRSTATUS);
BOOL __stdcall SRSetRestorePointInternal(PRESTOREPOINTINFOW, PSTATEMGRSTATUS, BOOL);
DWORD __stdcall SRRemoveRestorePoint(DWORD);

#ifdef __cplusplus
}
#endif

#ifdef UNICODE
#define RESTOREPOINTINFO  RESTOREPOINTINFOW
#define PRESTOREPOINTINFO  PRESTOREPOINTINFOW
#define SRSetRestorePoint  SRSetRestorePointW
#else /* !UNICODE */
#define RESTOREPOINTINFO  RESTOREPOINTINFOA
#define PRESTOREPOINTINFO  PRESTOREPOINTINFOA
#define SRSetRestorePoint  SRSetRestorePointA
#endif /* !UNICODE */

#endif /* _SRRESTOREPTAPI_H */
