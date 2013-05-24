/* 
   Base.h

   Base definitions

   Copyright (C) 1996 Free Software Foundation, Inc.

   Author: Scott Christley <scottc@net-community.com>

   This file is part of the Windows32 API Library.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   If you are interested in a warranty or support for this source code,
   contact Scott Christley <scottc@net-community.com> for more information.
   
   You should have received a copy of the GNU Library General Public
   License along with this library; see the file COPYING.LIB.
   If not, write to the Free Software Foundation, 
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   -----------
   DAL 2003 - this file modified extensively for my compiler.  New
   definitionswnwn added as well.
*/ 

#ifndef _GNU_H_WINDOWS32_BASE
#define _GNU_H_WINDOWS32_BASE

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef SNDMSG
#ifdef __cplusplus
#define SNDMSG ::SendMessage
#else
#define SNDMSG SendMessage
#endif
#endif

#ifndef NULL
#define NULL  ((void *)0)
#endif /* !NULL */

#define FALSE 0
#define TRUE 1

#define CONST const
#define VOID void

/* Skip if invoked by resource compiler */
#ifndef	RC_INVOKED

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x501 /*win xp*/
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x400
#endif

/* There is a conflict with BOOL between Objective-C and Win32,
   so the Windows32 API Library defines and uses WINBOOL.
   However, if we are not using Objective-C then define the normal
   windows BOOL so Win32 programs compile normally.  If you are
   using Objective-C then you must use WINBOOL for Win32 operations.
*/
typedef int WINBOOL;

#ifndef FAR
#define FAR
#endif

#ifndef __OBJC__
typedef WINBOOL BOOL;
#endif /* !__OBJC__ */

/* typedef ACMDRIVERENUMCB;
typedef ACMDRIVERPROC;
typedef ACMFILERCHOOSEHOOKPROC;
typedef ACMFILTERENUMCB;
typedef ACMFILTERTAGENUMCB;
typedef ACMFORMATCHOOSEHOOKPROC;
typedef ACMFORMATENUMCB;
typedef ACMFORMATTAGENUMCB;
typedef APPLET_PROC;
*/
typedef unsigned short ATOM;

typedef unsigned char BOOLEAN;
typedef unsigned char BYTE;
typedef unsigned long CALTYPE;
typedef unsigned long CALID;
typedef char CCHAR;
typedef char CHAR;

#ifndef __cplusplus
typedef unsigned short WCHAR;
#else
typedef wchar_t WCHAR;
#endif

typedef unsigned long COLORREF;

/*
typedef CTRYID;
typedef DLGPROC;
*/

typedef unsigned int DWORD; /* was unsigned long */
typedef double DWORDLONG, *PDWORDLONG;


typedef int INT_PTR, *PINT_PTR;
typedef unsigned  UINT_PTR, *PUINT_PTR;

typedef int LONG_PTR, *PLONG_PTR;
typedef unsigned ULONG_PTR, *PULONG_PTR;

typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR ;

typedef __int64 LONGLONG, *PLONGLONG;
typedef __int64 LONG64, *PLONG64;
typedef unsigned __int64 ULONGLONG, *PULONGLONG;
typedef unsigned __int64 ULONG64, *PULONG64;
typedef unsigned __int64 DWORD64, *PDWORD64;
typedef unsigned __int64 UINT64, *PUINT64;

/* for headers abstracted from MINGW */
#define _ANONYMOUS_UNION
#define _ANONYMOUS_STRUCT
#define _STRUCT_NAME(x) x
#define DECLSPEC_NORETURN
#define DECLARE_STDCALL_P( type ) type __stdcall

/*
typedef EDITWORDBREAKPROC;
typedef ENHMFENUMPROC;
typedef ENUMRESLANGPROC;
typedef ENUMRESNAMEPROC;
typedef ENUMRESTYPEPROC;
*/

typedef float FLOAT;
/* typedef GLOBALHANDLE; */
typedef void *HANDLE;
typedef HANDLE HTASK ;
typedef HANDLE LOCALHANDLE;
typedef int HFILE;
typedef long HRESULT;
typedef int INT;
typedef unsigned short LANGID;
/* typedef LOCALHANDLE */
typedef long LONG;
typedef LONGLONG USN ;
typedef unsigned short *LP;
typedef long LPARAM;
/* typedef LPFRHOOKPROC; */
/* typedef LPHANDLER_FUNCTION; */
typedef long LRESULT;
/* typedef NPSTR; */
/* typedef PHKEY; */
/* typedef LCID *PLCID; */
/* typedef PSID; */
/* typedef REGSAM; */
typedef short RETCODE;
typedef short SHORT;
/* typedef SPHANDLE; */
typedef unsigned char UCHAR;
typedef unsigned int UINT;
typedef unsigned long ULONG;
typedef unsigned short USHORT;
typedef unsigned short WORD;
typedef unsigned int WPARAM;

#define UNALIGNED

/* typedef YIELDPROC; */

/*
  Types defined in terms of other defined types
 */

typedef HANDLE HACCEL;
typedef HANDLE HBITMAP;
typedef HANDLE HBRUSH;
typedef HANDLE HCOLORSPACE;
typedef HANDLE HCONV;
typedef HANDLE HCONVLIST;
typedef HANDLE HCURSOR;
typedef HANDLE HDBC;
typedef HANDLE HDC;
typedef HANDLE HDDEDATA;
typedef HANDLE HDESK;
typedef HANDLE HDROP;
typedef HANDLE HDWP;
typedef HANDLE HENHMETAFILE;
typedef HANDLE HENV;
typedef HANDLE HEVENT;
typedef HANDLE HFONT;
typedef HANDLE HGDIOBJ;
typedef HANDLE HGLOBAL;
typedef HANDLE HGLRC;
typedef HANDLE HHOOK;
typedef HANDLE HICON;
typedef HANDLE HINSTANCE;
typedef HANDLE HKEY, *PHKEY;
typedef HANDLE HKL;
typedef HANDLE HLOCAL;
typedef HANDLE HMENU;
typedef HANDLE HMETAFILE;
typedef HANDLE HMODULE;
typedef HANDLE HPALETTE;
typedef HANDLE HPEN;
typedef HANDLE HRASCONN;
typedef HANDLE HRGN;
typedef HANDLE HRSRC;
typedef HANDLE HSTMT;
typedef HANDLE HSZ;
typedef HANDLE HWINSTA;
typedef HANDLE HWND;

typedef DWORD LCID;
typedef DWORD LCTYPE;

typedef WCHAR *NWPSTR;

typedef WINBOOL *LPBOOL;

typedef COLORREF *LPCOLORREF;
typedef CONST VOID *LPCVOID;


typedef BYTE *PBOOLEAN;
typedef BYTE *PBYTE, *LPBYTE;

typedef CONST CHAR *PCCH, *LPCCH;
typedef CHAR *PCH, *LPCH, *PCHAR, *LPCHAR;
typedef CONST CHAR *PCSTR, *LPCSTR;
typedef CONST WCHAR *PCWCH, *LPCWCH;
typedef CONST WCHAR *PCWSTR, *LPCWSTR;

typedef DWORD *PDWORD, *LPDWORD;

typedef FLOAT *PFLOAT;

typedef HANDLE *PHANDLE, *LPHANDLE;

typedef INT *PINT, *LPINT;
typedef LONG *PLONG, *LPLONG;
typedef SHORT *PSHORT;

typedef CHAR *PSTR, *LPSTR;
typedef CHAR *PSZ;

typedef UCHAR *PUCHAR;
typedef UINT *PUINT;
typedef ULONG *PULONG;
typedef USHORT *PUSHORT;

typedef VOID *PVOID, *LPVOID;
typedef CONST VOID *PCVOID, *LPCVOID;

typedef WCHAR *PWCH, *LPWCH, *PWCHAR, *LPWCHAR;

typedef WORD *PWORD, *LPWORD;

typedef WINBOOL *PWINBOOL;

typedef WCHAR *PWSTR, *LPWSTR;

typedef HANDLE SC_HANDLE, *PSC_HANDLE, *LPSC_HANDLE;
typedef LPVOID  SC_LOCK;
typedef WINBOOL SECURITY_CONTEXT_TRACKING_MODE;
typedef DWORD SERVICE_STATUS_HANDLE;

#ifdef UNICODE
typedef WCHAR TBYTE;
typedef WCHAR TCHAR;
typedef WCHAR BCHAR;
#else
typedef BYTE TBYTE;
typedef CHAR TCHAR;
typedef BYTE BCHAR;
#endif /* UNICODE */

typedef TBYTE *PTBYTE;
typedef TCHAR *PTCH, *LPTCH;
typedef TCHAR *PTCHAR, *LPTCHAR;
typedef TCHAR *PTSTR, *LPTSTR;
typedef CONST TCHAR *PCTSTR, *LPCTSTR;

#endif	/* RC_INVOKED */

/*
  Calling Conventions
 */

/* Only use __stdcall under WIN32 compiler */
#ifdef __i386__
#ifndef _WIN32
#define _WIN32
#endif
#endif

#ifdef _WIN32
#define STDCALL     __stdcall
#define CDECL       __cdecl
#define PASCAL      WINAPI
#define WINBASEAPI  __import
# ifdef __cplusplus
#  define EXTERN_C extern "C"
# else
#  define EXTERN_C extern
# endif  /* __cplusplus */ 
#define STDAPICALLTYPE STDCALL
#define STDAPI EXTERN_C HRESULT STDAPICALLTYPE
#else
#define STDCALL
#define CDECL
#define PASCAL
#define WINBASEAPI
#define STDAPICALLTYPE
#define STDAPI
#endif
#define WINAPI      STDCALL
#define APIENTRY    STDCALL
#define CALLBACK    WINAPI
#define WINGDIAPI

#define EXPORT _export
#define IMPORT _import
#define INDIRECT _indirect

#define DECLSPEC_IMPORT IMPORT


/* Skip if called by resource compiler */
#ifndef	RC_INVOKED

/*
 * Compatibility macros
 */

#define DefineHandleTable(w)            ((w),TRUE)
#define LimitEmsPages(dw)
#define SetSwapAreaSize(w)              (w)
#define LockSegment(w)                  GlobalFix((HANDLE)(w))
#define UnlockSegment(w)                GlobalUnfix((HANDLE)(w))
#define GetCurrentTime()                GetTickCount()

#define Yield()


/*
  Enumerations
*/

typedef enum _ACL_INFORMATION_CLASS {
  AclRevisionInformation = 1,   
  AclSizeInformation            
} ACL_INFORMATION_CLASS; 
  
#endif	/* RC_INVOKED */

#define RASCS_DONE 0x2000
#define RASCS_PAUSED 0x1000

#ifndef	RC_INVOKED

typedef enum _RASCONNSTATE { 
    RASCS_OpenPort = 0, 
    RASCS_PortOpened, 
    RASCS_ConnectDevice, 
    RASCS_DeviceConnected, 
    RASCS_AllDevicesConnected, 
    RASCS_Authenticate, 
    RASCS_AuthNotify, 
    RASCS_AuthRetry, 
    RASCS_AuthCallback, 
    RASCS_AuthChangePassword, 
    RASCS_AuthProject, 
    RASCS_AuthLinkSpeed, 
    RASCS_AuthAck, 
    RASCS_ReAuthenticate, 
    RASCS_Authenticated, 
    RASCS_PrepareForCallback, 
    RASCS_WaitForModemReset, 
    RASCS_WaitForCallback,
    RASCS_Projected, 
 
    RASCS_StartAuthentication,  
    RASCS_CallbackComplete,     
    RASCS_LogonNetwork,         
 
    RASCS_Interactive = RASCS_PAUSED, 
    RASCS_RetryAuthentication, 
    RASCS_CallbackSetByCaller, 
    RASCS_PasswordExpired, 
 
    RASCS_Connected = RASCS_DONE, 
    RASCS_Disconnected 
} RASCONNSTATE ; 
 
typedef enum _RASPROJECTION {  
    RASP_Amb = 0x10000, 
    RASP_PppNbf = 0x803F, 
    RASP_PppIpx = 0x802B, 
    RASP_PppIp = 0x8021 
} RASPROJECTION ; 
 
typedef enum _SECURITY_IMPERSONATION_LEVEL {
    SecurityAnonymous, 
    SecurityIdentification, 
    SecurityImpersonation, 
    SecurityDelegation 
} SECURITY_IMPERSONATION_LEVEL; 
 
typedef enum _SID_NAME_USE { 
    SidTypeUser = 1, 
    SidTypeGroup, 
    SidTypeDomain, 
    SidTypeAlias, 
    SidTypeWellKnownGroup, 
    SidTypeDeletedAccount, 
    SidTypeInvalid, 
    SidTypeUnknown 
} SID_NAME_USE, *PSID_NAME_USE; 
 
typedef enum _TOKEN_INFORMATION_CLASS {
    TokenUser = 1, 
    TokenGroups, 
    TokenPrivileges, 
    TokenOwner, 
    TokenPrimaryGroup, 
    TokenDefaultDacl, 
    TokenSource, 
    TokenType, 
    TokenImpersonationLevel, 
    TokenStatistics 
} TOKEN_INFORMATION_CLASS; 
 
typedef enum tagTOKEN_TYPE {
    TokenPrimary = 1, 
    TokenImpersonation 
} TOKEN_TYPE; 
 
#endif /* RC_INVOKED */

/*
  Macros
*/

#define DECLARE_HANDLE(s) typedef HANDLE s

#define GetBValue(rgb)   ((BYTE) ((rgb) >> 16)) 
#define GetGValue(rgb)   ((BYTE) (((WORD) (rgb)) >> 8)) 
#define GetRValue(rgb)   ((BYTE) (rgb)) 
#define RGB(r, g ,b)  ((DWORD) (((BYTE) (r) | \
    ((WORD) (g) << 8)) | \
    (((DWORD) (BYTE) (b)) << 16))) 

#define HIBYTE(w)   ((BYTE) (((WORD) (w) >> 8) & 0xFF)) 
#define HIWORD(l)   ((WORD) (((DWORD) (l) >> 16) & 0xFFFF)) 
#define LOBYTE(w)   ((BYTE) (w)) 
#define LOWORD(l)   ((WORD) (l)) 
#define MAKELONG(a, b) \
    ((LONG) (((WORD) (a)) | ((DWORD) ((WORD) (b))) << 16)) 
#define MAKEWORD(a, b) \
    ((WORD) (((BYTE) (a)) | ((WORD) ((BYTE) (b))) << 8)) 

/* original Cygnus headers also had the following defined: */
#define SEXT_HIWORD(l)     ((((int)l) >> 16))
#define ZEXT_HIWORD(l)     ((((unsigned int)l) >> 16))
#define SEXT_LOWORD(l)     ((int)(short)l)

#define MAKEINTATOM(i)   (LPTSTR) ((DWORD) ((WORD) (i))) 
#define MAKEINTRESOURCE(i)  (LPTSTR) ((DWORD) ((WORD) (i)))

#define MAKELANGID(p, s) ((((WORD) (s)) << 10) | (WORD) (p)) 
#define PRIMARYLANGID(lgid)    ((WORD  )(lgid) & 0x3ff) 
#define SUBLANGID(lgid)        ((WORD  )(lgid) >> 10) 

#define LANGIDFROMLCID(lcid)   ((WORD) (lcid)) 
#define SORTIDFROMLCID(lcid) \
          ((WORD  )((((DWORD)(lcid)) & 0x000FFFFF) >> 16)) 
#define MAKELCID(lgid, srtid)  \
    ((DWORD)((((DWORD)((WORD)(srtid))) << 16) |  \
            ((DWORD)((WORD)(lgid))))) 
#define MAKELPARAM(l, h)   ((LPARAM) MAKELONG(l, h)) 
#define MAKELRESULT(l, h)   ((LRESULT) MAKELONG(l, h)) 
#define MAKEPOINTS(l)   (*((POINTS FAR *) & (l))) 
#define MAKEROP4(fore,back) \
          (DWORD)((((back) << 8) & 0xFF000000) | (fore)) 
#define MAKEWPARAM(l, h)   ((WPARAM) MAKELONG(l, h)) 

#ifndef max
#define max(a, b)  (((a) > (b)) ? (a) : (b)) 
#endif

#ifndef min
#define min(a, b)  (((a) < (b)) ? (a) : (b)) 
#endif

#define PALETTEINDEX(i) \
    ((COLORREF) (0x01000000 | (DWORD) (WORD) (i))) 
#define PALETTERGB(r, g, b)  (0x02000000 | RGB(r, g, b)) 
#define POINTSTOPOINT(pt, pts) {(pt).x = (SHORT) LOWORD(pts); \
      (pt).y = (SHORT) HIWORD(pts);} 
#define POINTTOPOINTS(pt) \
    (MAKELONG((short) ((pt).x), (short) ((pt).y))) 

#ifdef UNICODE
#define TEXT(quote) L##quote 
#else
#define TEXT(quote) quote
#endif

/* Skip if invoked by resource compiler */
#ifndef	RC_INVOKED

/*
   Definitions for callback procedures
*/

typedef void (*CALLB) ();
typedef int (CALLBACK *BFFCALLBACK) (HWND, UINT, LPARAM, LPARAM);
typedef DWORD (CALLBACK *PTHREAD_START_ROUTINE) (LPVOID);
typedef DWORD (CALLBACK *EDITSTREAMCALLBACK) (DWORD, LPBYTE, LONG, LONG);
typedef WINBOOL (CALLBACK *DLGPROC) (HWND, UINT, WPARAM, LPARAM);
typedef VOID (CALLBACK *LPSERVICE_MAIN_FUNCTION) (DWORD, LPTSTR);
typedef int (CALLBACK *PFNTVCOMPARE) (LPARAM, LPARAM, LPARAM);
typedef LRESULT (CALLBACK *WNDPROC) (HWND, UINT, WPARAM, LPARAM);
typedef int (CALLBACK *FARPROC)();
typedef WINBOOL (CALLBACK *ENUMRESTYPEPROC) (HANDLE, LPTSTR, LONG);
typedef WINBOOL (CALLBACK *ENUMRESNAMEPROC) (HANDLE, LPCTSTR, LPTSTR, LONG);
typedef WINBOOL (CALLBACK *ENUMRESLANGPROC) (HANDLE, LPCTSTR, LPCTSTR, WORD, LONG);
typedef WINBOOL (CALLBACK *WNDENUMPROC) (HWND, LPARAM);
typedef WINBOOL (CALLBACK *ENUMWINDOWSTATIONPROC) (LPTSTR, LPARAM);
typedef VOID (CALLBACK *SENDASYNCPROC) (HWND, UINT, DWORD, LRESULT);
typedef VOID (CALLBACK *TIMERPROC) (HWND, UINT, UINT, DWORD);
typedef WINBOOL (CALLBACK *DRAWSTATEPROC) (HDC, LPARAM, WPARAM, int, int);
typedef WINBOOL (CALLBACK *PROPENUMPROCEX) (HWND, LPCTSTR, HANDLE, DWORD);
typedef WINBOOL (CALLBACK *PROPENUMPROC) (HWND, LPCTSTR, HANDLE);
typedef LRESULT (CALLBACK *HOOKPROC) (int, WPARAM, LPARAM);
typedef VOID (CALLBACK *ENUMOBJECTSPROC) (LPVOID, LPARAM);
typedef VOID (CALLBACK *LINEDDAPROC) (int, int, LPARAM);
typedef WINBOOL (CALLBACK *ABORTPROC) (HDC, int);
typedef int (CALLBACK *ICMENUMPROC) (LPTSTR, LPARAM);
typedef LONG (*EDITWORDBREAKPROCEX) (char *, LONG, BYTE, INT);
typedef int (CALLBACK *PFNLVCOMPARE) (LPARAM, LPARAM, LPARAM);
typedef WINBOOL (CALLBACK *LOCALE_ENUMPROC) (LPTSTR);
typedef WINBOOL (CALLBACK *CODEPAGE_ENUMPROC) (LPTSTR);
typedef WINBOOL (CALLBACK *DATEFMT_ENUMPROC) (LPTSTR);
typedef WINBOOL (CALLBACK *TIMEFMT_ENUMPROC) (LPTSTR);
typedef WINBOOL (CALLBACK *CALINFO_ENUMPROC) (LPTSTR);
typedef WINBOOL (CALLBACK *PHANDLER_ROUTINE) (DWORD);
typedef WINBOOL (CALLBACK *LPHANDLER_FUNCTION) (DWORD);
typedef UINT (CALLBACK *PFNGETPROFILEPATH) (LPCTSTR, LPSTR, UINT);
typedef UINT (CALLBACK *PFNRECONCILEPROFILE) (LPCTSTR, LPCTSTR, DWORD);
typedef WINBOOL (CALLBACK *PFNPROCESSPOLICIES) (HWND, LPCTSTR, LPCTSTR, LPCTSTR, DWORD);

/*
  Callback types defined in terms of other callback types
 */

typedef FARPROC DESKTOPENUMPROC;
typedef FARPROC ENHMFENUMPROC;
typedef FARPROC GRAYSTRINGPROC;
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;
typedef CALLB PFNCALLBACK;
typedef FARPROC PROC;

#endif	/* RC_INVOKED */

#define SECURITY_NULL_SID_AUTHORITY     {0,0,0,0,0,0}
#define SECURITY_WORLD_SID_AUTHORITY    {0,0,0,0,0,1}
#define SECURITY_LOCAL_SID_AUTHORITY    {0,0,0,0,0,2}
#define SECURITY_CREATOR_SID_AUTHORITY  {0,0,0,0,0,3}
#define SECURITY_NON_UNIQUE_AUTHORITY   {0,0,0,0,0,4}
#define SECURITY_NT_AUTHORITY           {0,0,0,0,0,5}

#define SE_CREATE_TOKEN_NAME              TEXT("SeCreateTokenPrivilege")
#define SE_ASSIGNPRIMARYTOKEN_NAME        TEXT("SeAssignPrimaryTokenPrivilege")
#define SE_LOCK_MEMORY_NAME               TEXT("SeLockMemoryPrivilege")
#define SE_INCREASE_QUOTA_NAME            TEXT("SeIncreaseQuotaPrivilege")
#define SE_UNSOLICITED_INPUT_NAME         TEXT("SeUnsolicitedInputPrivilege")
#define SE_MACHINE_ACCOUNT_NAME           TEXT("SeMachineAccountPrivilege")
#define SE_TCB_NAME                       TEXT("SeTcbPrivilege")
#define SE_SECURITY_NAME                  TEXT("SeSecurityPrivilege")
#define SE_TAKE_OWNERSHIP_NAME            TEXT("SeTakeOwnershipPrivilege")
#define SE_LOAD_DRIVER_NAME               TEXT("SeLoadDriverPrivilege")
#define SE_SYSTEM_PROFILE_NAME            TEXT("SeSystemProfilePrivilege")
#define SE_SYSTEMTIME_NAME                TEXT("SeSystemtimePrivilege")
#define SE_PROF_SINGLE_PROCESS_NAME       TEXT("SeProfileSingleProcessPrivilege")
#define SE_INC_BASE_PRIORITY_NAME         TEXT("SeIncreaseBasePriorityPrivilege")
#define SE_CREATE_PAGEFILE_NAME           TEXT("SeCreatePagefilePrivilege")
#define SE_CREATE_PERMANENT_NAME          TEXT("SeCreatePermanentPrivilege")
#define SE_BACKUP_NAME                    TEXT("SeBackupPrivilege")
#define SE_RESTORE_NAME                   TEXT("SeRestorePrivilege")
#define SE_SHUTDOWN_NAME                  TEXT("SeShutdownPrivilege")
#define SE_DEBUG_NAME                     TEXT("SeDebugPrivilege")
#define SE_AUDIT_NAME                     TEXT("SeAuditPrivilege")
#define SE_SYSTEM_ENVIRONMENT_NAME        TEXT("SeSystemEnvironmentPrivilege")
#define SE_CHANGE_NOTIFY_NAME             TEXT("SeChangeNotifyPrivilege")
#define SE_REMOTE_SHUTDOWN_NAME           TEXT("SeRemoteShutdownPrivilege")

#define SERVICES_ACTIVE_DATABASEW      L"ServicesActive"
#define SERVICES_FAILED_DATABASEW      L"ServicesFailed"
#define SERVICES_ACTIVE_DATABASEA      "ServicesActive"
#define SERVICES_FAILED_DATABASEA      "ServicesFailed"
#define SC_GROUP_IDENTIFIERW           L'+'
#define SC_GROUP_IDENTIFIERA           '+'

#ifdef UNICODE
#define SERVICES_ACTIVE_DATABASE       SERVICES_ACTIVE_DATABASEW
#define SERVICES_FAILED_DATABASE       SERVICES_FAILED_DATABASEW
#define SC_GROUP_IDENTIFIER            SC_GROUP_IDENTIFIERW
#else 
#define SERVICES_ACTIVE_DATABASE       SERVICES_ACTIVE_DATABASEA
#define SERVICES_FAILED_DATABASE       SERVICES_FAILED_DATABASEA
#define SC_GROUP_IDENTIFIER            SC_GROUP_IDENTIFIERA
#endif /* UNICODE */


/* Skip if invoked by resource compiler */
#ifndef	RC_INVOKED


/* End of stuff from ddeml.h in old Cygnus headers */
/* ----------------------------------------------- */

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _GNU_H_WINDOWS32_BASE */