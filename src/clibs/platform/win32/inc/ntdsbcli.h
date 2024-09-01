#ifndef _NTDSBCLI_H
#define _NTDSBCLI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows NT Directory Service Backup Client API definitions */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

#define xRPC_STRING
typedef wchar_t WCHAR;

#ifndef _NTDSBCLI_DEFINED
#define NTDSBCLI_API __declspec(dllimport) _stdcall
#else
#define NTDSBCLI_API
#endif

#ifndef _HRESULT_DEFINED
#define _HRESULT_DEFINED
typedef LONG HRESULT;
#endif

#ifdef UNICODE
typedef WCHAR BFT;
#else
typedef CHAR BFT;
#endif

#define g_wszBackupAnnotation  L"NTDS Backup Interface"
#define g_aszBackupAnnotation  "NTDS Backup Interface"
#define g_wszRestoreAnnotation  L"NTDS Restore Interface"
#define g_aszRestoreAnnotation  "NTDS Restore Interface"

#define BACKUP_TYPE_FULL  0x01
#define BACKUP_TYPE_LOGS_ONLY  0x02
#define BACKUP_TYPE_INCREMENTAL  0x04

#define RESTORE_TYPE_AUTHORATATIVE  0x01
#define RESTORE_TYPE_ONLINE  0x02
#define RESTORE_TYPE_CATCHUP  0x04

#define BACKUP_DISABLE_INCREMENTAL  0xffffffff

#define BFT_DIRECTORY  0x80
#define BFT_DATABASE_DIRECTORY  0x40
#define BFT_LOG_DIRECTORY  0x20

#define BFT_LOG  (BFT)(TEXT('\x01')|BFT_LOG_DIRECTORY)
#define BFT_LOG_DIR  (BFT)(TEXT('\x02')|BFT_DIRECTORY)
#define BFT_CHECKPOINT_DIR  (BFT)(TEXT('\x03')|BFT_DIRECTORY)
#define BFT_NTDS_DATABASE  (BFT)(TEXT('\x04')|BFT_DATABASE_DIRECTORY)
#define BFT_PATCH_FILE  (BFT)(TEXT('\x05')|BFT_LOG_DIRECTORY)
#define BFT_UNKNOWN  (BFT)(TEXT('\x0f'))

#include <ntdsbmsg.h>

typedef void *HBC;

typedef struct tagEDB_RSTMAPA {
    xRPC_STRING char *szDatabaseName;
    xRPC_STRING char *szNewDatabaseName;
} EDB_RSTMAPA, *PEDB_RSTMAPA;

#define UNICODE_RSTMAP

typedef struct tagEDB_RSTMAPW {
    xRPC_STRING WCHAR *wszDatabaseName;
    xRPC_STRING WCHAR *wszNewDatabaseName;
} EDB_RSTMAPW, *PEDB_RSTMAPW;

HRESULT NTDSBCLI_API DsIsNTDSOnlineA(LPCSTR,BOOL*);
HRESULT NTDSBCLI_API DsIsNTDSOnlineW(LPCWSTR,BOOL*);
HRESULT NTDSBCLI_API DsBackupPrepareA(LPCSTR,ULONG,ULONG,PVOID*,LPDWORD,HBC*);
HRESULT NTDSBCLI_API DsBackupPrepareW(LPCWSTR,ULONG,ULONG,PVOID*,LPDWORD,HBC*);
HRESULT NTDSBCLI_API DsBackupGetDatabaseNamesA(HBC,LPSTR*,LPDWORD);
HRESULT NTDSBCLI_API DsBackupGetDatabaseNamesW(HBC,LPWSTR*,LPDWORD);
HRESULT NTDSBCLI_API DsBackupOpenFileA(HBC,LPCSTR,DWORD,LARGE_INTEGER*);
HRESULT NTDSBCLI_API DsBackupOpenFileW(HBC,LPCWSTR,DWORD,LARGE_INTEGER*);
HRESULT NTDSBCLI_API DsBackupRead(HBC,PVOID,DWORD,PDWORD);
HRESULT NTDSBCLI_API DsBackupClose(HBC);
HRESULT NTDSBCLI_API DsBackupGetBackupLogsA(HBC,LPSTR*,LPDWORD);
HRESULT NTDSBCLI_API DsBackupGetBackupLogsW(HBC,LPWSTR*,LPDWORD);
HRESULT NTDSBCLI_API DsBackupTruncateLogs(HBC);
HRESULT NTDSBCLI_API DsBackupEnd(HBC);
VOID NTDSBCLI_API DsBackupFree(PVOID);
HRESULT NTDSBCLI_API DsRestoreGetDatabaseLocationsA(HBC,LPSTR*,LPDWORD);
HRESULT NTDSBCLI_API DsRestoreGetDatabaseLocationsW(HBC,LPWSTR*,LPDWORD);
HRESULT NTDSBCLI_API DsRestorePrepareA(LPCSTR,ULONG,PVOID,DWORD,HBC*);
HRESULT NTDSBCLI_API DsRestorePrepareW(LPCWSTR,ULONG,PVOID,DWORD,HBC*);
HRESULT NTDSBCLI_API DsRestoreRegisterA(HBC,LPCSTR,LPCSTR,EDB_RSTMAPA[],LONG,LPCSTR,ULONG,ULONG);
HRESULT NTDSBCLI_API DsRestoreRegisterW(HBC,LPCWSTR,LPCWSTR,EDB_RSTMAPW[],LONG,LPCWSTR,ULONG,ULONG);
HRESULT NTDSBCLI_API DsRestoreRegisterComplete(HBC,HRESULT);
HRESULT NTDSBCLI_API DsRestoreEnd(HBC);
HRESULT NTDSBCLI_API DsSetCurrentBackupLogA(LPCSTR,DWORD);
HRESULT NTDSBCLI_API DsSetCurrentBackupLogW(LPCWSTR,DWORD);
HRESULT NTDSBCLI_API DsSetAuthIdentityA(LPCSTR,LPCSTR,LPCSTR);
HRESULT NTDSBCLI_API DsSetAuthIdentityW(LPCWSTR,LPCWSTR,LPCWSTR);

#ifdef UNICODE
#define g_szBackupAnnotation  g_wszBackupAnnotation
#define g_szRestoreAnnotation g_wszRestoreAnnotation
#define EDB_RSTMAP EDB_RSTMAPW
#define PEDB_RSTMAP PEDB_RSTMAPW
#define DsIsNTDSOnline  DsIsNTDSOnlineW
#define DsBackupPrepare  DsBackupPrepareW
#define DsBackupGetDatabaseNames  DsBackupGetDatabaseNamesW
#define DsBackupOpenFile  DsBackupOpenFileW
#define DsBackupGetBackupLogs  DsBackupGetBackupLogsW
#define DsRestoreGetDatabaseLocations  DsRestoreGetDatabaseLocationsW
#define DsRestorePrepare  DsRestorePrepareW
#define DsRestoreRegister  DsRestoreRegisterW
#define DsSetCurrentBackupLog  DsSetCurrentBackupLogW
#define DsSetAuthIdentity  DsSetAuthIdentityW
#else
#define g_szBackupAnnotation  g_aszBackupAnnotation
#define g_szRestoreAnnotation g_aszRestoreAnnotation
#define EDB_RSTMAP EDB_RSTMAPA
#define PEDB_RSTMAP PEDB_RSTMAPA
#define DsIsNTDSOnline  DsIsNTDSOnlineA
#define DsBackupPrepare  DsBackupPrepareA
#define DsBackupGetDatabaseNames  DsBackupGetDatabaseNamesA
#define DsBackupOpenFile  DsBackupOpenFileA
#define DsBackupGetBackupLogs  DsBackupGetBackupLogsA
#define DsRestoreGetDatabaseLocations  DsRestoreGetDatabaseLocationsA
#define DsRestorePrepare  DsRestorePrepareA
#define DsRestoreRegister  DsRestoreRegisterA
#define DsSetCurrentBackupLog  DsSetCurrentBackupLogA
#define DsSetAuthIdentity  DsSetAuthIdentityA
#endif /* UNICODE */

#ifdef __cplusplus
}
#endif

#endif /* _NTDSBCLI_H */
