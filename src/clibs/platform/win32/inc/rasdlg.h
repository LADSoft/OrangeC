#ifndef _RASDLG_H
#define _RASDLG_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Remote Access Common Dialog API definitions */

#include <pshpack4.h>

#ifdef __cplusplus
extern "C" {
#endif


#include <ras.h>

#define RASPBDEVENT_AddEntry  1
#define RASPBDEVENT_EditEntry  2
#define RASPBDEVENT_RemoveEntry 3
#define RASPBDEVENT_DialEntry  4
#define RASPBDEVENT_EditGlobals  5
#define RASPBDEVENT_NoUser  6
#define RASPBDEVENT_NoUserEdit  7

#define RASNOUSER_SmartCard  0x00000001

#define RASPBDFLAG_PositionDlg  0x00000001
#define RASPBDFLAG_ForceCloseOnDial  0x00000002
#define RASPBDFLAG_NoUser  0x00000010
#define RASPBDFLAG_UpdateDefaults  0x80000000

#define RASEDFLAG_PositionDlg  0x00000001
#define RASEDFLAG_NewEntry  0x00000002
#if (WINVER < 0x0600)
#define RASEDFLAG_CloneEntry  0x00000004
#endif /* WINVER < 0x0600 */
#define RASEDFLAG_NoRename  0x00000008
#define RASEDFLAG_ShellOwned  0x40000000
#define RASEDFLAG_NewPhoneEntry  0x00000010
#define RASEDFLAG_NewTunnelEntry  0x00000020
#if (WINVER < 0x0600)
#define RASEDFLAG_NewDirectEntry  0x00000040
#endif /* WINVER < 0x0600 */
#define RASEDFLAG_NewBroadbandEntry  0x00000080
#define RASEDFLAG_InternetEntry  0x00000100
#define RASEDFLAG_NAT  0x00000200
#if (WINVER >= 0x0600)
#define RASEDFLAG_IncomingConnection  0x00000400
#endif /* WINVER >= 0x0600 */

#define RASDDFLAG_PositionDlg  0x00000001
#define RASDDFLAG_NoPrompt  0x00000002
#define RASDDFLAG_LinkFailure  0x80000000

#define RASMDPAGE_Status  0
#define RASMDPAGE_Summary  1
#define RASMDPAGE_Preferences 2

#define RASMDFLAG_PositionDlg  0x00000001
#define RASMDFLAG_UpdateDefaults  0x80000000

typedef VOID (WINAPI* RASPBDLGFUNCW)(ULONG_PTR,DWORD,LPWSTR,LPVOID);
typedef VOID (WINAPI* RASPBDLGFUNCA)(ULONG_PTR,DWORD,LPSTR,LPVOID);

typedef struct tagRASNOUSERW {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwTimeoutMs;
    WCHAR szUserName[UNLEN+1];
    WCHAR szPassword[PWLEN+1];
    WCHAR szDomain[DNLEN+1];
} RASNOUSERW, *LPRASNOUSERW;

typedef struct tagRASNOUSERA {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwTimeoutMs;
    CHAR szUserName[UNLEN+1];
    CHAR szPassword[PWLEN+1];
    CHAR szDomain[DNLEN+1];
} RASNOUSERA, *LPRASNOUSERA;

typedef struct tagRASPBDLGW {
    DWORD dwSize;
    HWND hwndOwner;
    DWORD dwFlags;
    LONG xDlg;
    LONG yDlg;
    ULONG_PTR dwCallbackId;
    RASPBDLGFUNCW pCallback;
    DWORD dwError;
    ULONG_PTR reserved;
    ULONG_PTR reserved2;
} RASPBDLGW, *LPRASPBDLGW;

typedef struct tagRASPBDLGA {
    DWORD dwSize;
    HWND hwndOwner;
    DWORD dwFlags;
    LONG xDlg;
    LONG yDlg;
    ULONG_PTR dwCallbackId;
    RASPBDLGFUNCA pCallback;
    DWORD dwError;
    ULONG_PTR reserved;
    ULONG_PTR reserved2;
} RASPBDLGA, *LPRASPBDLGA;

typedef struct tagRASENTRYDLGW {
    DWORD dwSize;
    HWND hwndOwner;
    DWORD dwFlags;
    LONG xDlg;
    LONG yDlg;
    WCHAR szEntry[RAS_MaxEntryName+1];
    DWORD dwError;
    ULONG_PTR reserved;
    ULONG_PTR reserved2;
} RASENTRYDLGW, *LPRASENTRYDLGW;

typedef struct tagRASENTRYDLGA {
    DWORD dwSize;
    HWND hwndOwner;
    DWORD dwFlags;
    LONG xDlg;
    LONG yDlg;
    CHAR szEntry[RAS_MaxEntryName+1];
    DWORD dwError;
    ULONG_PTR reserved;
    ULONG_PTR reserved2;
} RASENTRYDLGA, *LPRASENTRYDLGA;

typedef struct tagRASDIALDLG {
    DWORD dwSize;
    HWND hwndOwner;
    DWORD dwFlags;
    LONG xDlg;
    LONG yDlg;
    DWORD dwSubEntry;
    DWORD dwError;
    ULONG_PTR reserved;
    ULONG_PTR reserved2;
} RASDIALDLG, *LPRASDIALDLG;

typedef struct tagRASMONITORDLG {
    DWORD dwSize;
    HWND hwndOwner;
    DWORD dwFlags;
    DWORD dwStartPage;
    LONG xDlg;
    LONG yDlg;
    DWORD dwError;
    ULONG_PTR reserved;
    ULONG_PTR reserved2;
} RASMONITORDLG, *LPRASMONITORDLG;

#if (WINVER >= 0x500)
typedef BOOL (WINAPI *RasCustomDialDlgFn)(HINSTANCE,DWORD,LPWSTR,LPWSTR,LPWSTR,LPRASDIALDLG,PVOID);
#ifdef UNICODE
typedef BOOL (WINAPI *RasCustomEntryDlgFn)(HINSTANCE,LPWSTR,LPWSTR,LPRASENTRYDLGW,DWORD);
#else
typedef BOOL (WINAPI *RasCustomEntryDlgFn)(HINSTANCE,LPWSTR,LPWSTR,LPRASENTRYDLGA,DWORD);
#endif /* UNICODE */
typedef DWORD (WINAPI *PFNRASGETBUFFER)(PBYTE*,PDWORD);
typedef DWORD (WINAPI *PFNRASFREEBUFFER)(PBYTE);
typedef DWORD (WINAPI *PFNRASSENDBUFFER)(HANDLE,PBYTE,DWORD);
typedef DWORD (WINAPI *PFNRASRECEIVEBUFFER)(HANDLE,PBYTE,PDWORD,DWORD,HANDLE);
typedef DWORD (WINAPI *PFNRASRETRIEVEBUFFER)(HANDLE,PBYTE,PDWORD);
typedef  DWORD (WINAPI *RasCustomScriptExecuteFn)(HANDLE,LPCWSTR,LPCWSTR,PFNRASGETBUFFER,PFNRASFREEBUFFER,PFNRASSENDBUFFER,PFNRASRECEIVEBUFFER,PFNRASRETRIEVEBUFFER,HWND,RASDIALPARAMS*,PVOID);
#endif /* WINVER >= 0x500 */

BOOL APIENTRY RasPhonebookDlgA(LPSTR,LPSTR,LPRASPBDLGA);
BOOL APIENTRY RasPhonebookDlgW(LPWSTR,LPWSTR,LPRASPBDLGW);
BOOL APIENTRY RasEntryDlgA(LPSTR,LPSTR,LPRASENTRYDLGA);
BOOL APIENTRY RasEntryDlgW(LPWSTR,LPWSTR,LPRASENTRYDLGW);
BOOL APIENTRY RasDialDlgA(LPSTR,LPSTR,LPSTR,LPRASDIALDLG);
BOOL APIENTRY RasDialDlgW(LPWSTR,LPWSTR,LPWSTR,LPRASDIALDLG);
BOOL APIENTRY RasMonitorDlgA(LPSTR,LPRASMONITORDLG);
BOOL APIENTRY RasMonitorDlgW(LPWSTR,LPRASMONITORDLG);

#ifdef UNICODE
#define RASNOUSER RASNOUSERW
#define LPRASNOUSER LPRASNOUSERW
#define RASPBDLG RASPBDLGW
#define RASPBDLGFUNC RASPBDLGFUNCW
#define LPRASPBDLG LPRASPBDLGW
#define RASENTRYDLG RASENTRYDLGW
#define LPRASENTRYDLG LPRASENTRYDLGW
#define RasPhonebookDlg RasPhonebookDlgW
#define RasEntryDlg RasEntryDlgW
#define RasDialDlg RasDialDlgW
#define RasMonitorDlg RasMonitorDlgW
#else /* !UNICODE */
#define RASNOUSER RASNOUSERA
#define LPRASNOUSER LPRASNOUSERA
#define RASPBDLG RASPBDLGA
#define RASPBDLGFUNC RASPBDLGFUNCA
#define LPRASPBDLG LPRASPBDLGA
#define RASENTRYDLG RASENTRYDLGA
#define LPRASENTRYDLG LPRASENTRYDLGA
#define RasPhonebookDlg RasPhonebookDlgA
#define RasEntryDlg RasEntryDlgA
#define RasDialDlg RasDialDlgA
#define RasMonitorDlg RasMonitorDlgA
#endif /* !UNICODE */


#ifdef __cplusplus
}
#endif

#include <poppack.h>

#endif /* _RASDLG_H */
