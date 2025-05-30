#ifndef _WFEXT_H
#define _WFEXT_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows File Manager Extensions definitions */

#ifdef __cplusplus
extern "C" {
#endif

#define MENU_TEXT_LEN  40

#define FMMENU_FIRST  1
#define FMMENU_LAST  99

#define FMEVENT_LOAD  100
#define FMEVENT_UNLOAD  101
#define FMEVENT_INITMENU  102
#define FMEVENT_USER_REFRESH  103
#define FMEVENT_SELCHANGE  104
#define FMEVENT_TOOLBARLOAD  105
#define FMEVENT_HELPSTRING  106
#define FMEVENT_HELPMENUITEM  107

#define FMFOCUS_DIR  1
#define FMFOCUS_TREE  2
#define FMFOCUS_DRIVES  3
#define FMFOCUS_SEARCH  4

#define FM_GETFOCUS  (WM_USER+0x0200)
#define FM_GETSELCOUNT  (WM_USER+0x0202)
#define FM_GETSELCOUNTLFN  (WM_USER+0x0203)
#define FM_REFRESH_WINDOWS  (WM_USER+0x0206)
#define FM_RELOAD_EXTENSIONS  (WM_USER+0x0207)
#define FM_GETDRIVEINFOA  (WM_USER+0x0201)
#define FM_GETFILESELA  (WM_USER+0x0204)
#define FM_GETFILESELLFNA  (WM_USER+0x0205)
#define FM_GETDRIVEINFOW  (WM_USER+0x0211)
#define FM_GETFILESELW  (WM_USER+0x0214)
#define FM_GETFILESELLFNW  (WM_USER+0x0215)

typedef struct _FMS_GETFILESELA {
    FILETIME ftTime;
    DWORD dwSize;
    BYTE bAttr;
    CHAR szName[260];
} FMS_GETFILESELA, *LPFMS_GETFILESELA;

typedef struct _FMS_GETFILESELW {
    FILETIME ftTime;
    DWORD dwSize;
    BYTE bAttr;
    WCHAR szName[260];
} FMS_GETFILESELW, *LPFMS_GETFILESELW;

typedef struct _FMS_GETDRIVEINFOA {
    DWORD dwTotalSpace;
    DWORD dwFreeSpace;
    CHAR szPath[260];
    CHAR szVolume[14];
    CHAR szShare[128];
} FMS_GETDRIVEINFOA, *LPFMS_GETDRIVEINFOA;

typedef struct _FMS_GETDRIVEINFOW {
    DWORD dwTotalSpace;
    DWORD dwFreeSpace;
    WCHAR szPath[260];
    WCHAR szVolume[14];
    WCHAR szShare[128];
} FMS_GETDRIVEINFOW, *LPFMS_GETDRIVEINFOW;

typedef struct _FMS_LOADA {
    DWORD dwSize;
    CHAR szMenuName[MENU_TEXT_LEN];
    HMENU hMenu;
    UINT wMenuDelta;
} FMS_LOADA, *LPFMS_LOADA;

typedef struct _FMS_LOADW {
    DWORD dwSize;
    WCHAR szMenuName[MENU_TEXT_LEN];
    HMENU hMenu;
    UINT wMenuDelta;
} FMS_LOADW, *LPFMS_LOADW;

typedef struct tagEXT_BUTTON {
    WORD idCommand;
    WORD idsHelp;
    WORD fsStyle;
} EXT_BUTTON, *LPEXT_BUTTON;

typedef struct tagFMS_TOOLBARLOAD {
    DWORD dwSize;
    LPEXT_BUTTON lpButtons;
    WORD cButtons;
    WORD cBitmaps;
    WORD idBitmap;
    HBITMAP hBitmap;
} FMS_TOOLBARLOAD, *LPFMS_TOOLBARLOAD;

typedef struct tagFMS_HELPSTRINGA {
    INT idCommand;
    HMENU hMenu;
    CHAR szHelp[128];
} FMS_HELPSTRINGA, *LPFMS_HELPSTRINGA;

typedef struct tagFMS_HELPSTRINGW {
    INT idCommand;
    HMENU hMenu;
    WCHAR szHelp[128];
} FMS_HELPSTRINGW, *LPFMS_HELPSTRINGW;

typedef DWORD (APIENTRY *FM_EXT_PROC)(HWND,WORD,LONG);
typedef DWORD (APIENTRY *FM_UNDELETE_PROC)(HWND,LPTSTR);

#ifdef UNICODE
#define FM_GETDRIVEINFO FM_GETDRIVEINFOW
#define FM_GETFILESEL FM_GETFILESELW
#define FM_GETFILESELLFN FM_GETFILESELLFNW
#define FMS_GETFILESEL FMS_GETFILESELW
#define LPFMS_GETFILESEL LPFMS_GETFILESELW
#define FMS_GETDRIVEINFO FMS_GETDRIVEINFOW
#define LPFMS_GETDRIVEINFO LPFMS_GETDRIVEINFOW
#define FMS_LOAD FMS_LOADW
#define LPFMS_LOAD LPFMS_LOADW
#define FMS_HELPSTRING FMS_HELPSTRINGW
#define LPFMS_HELPSTRING LPFMS_HELPSTRINGW
LONG WINAPI FMExtensionProcW(HWND,WORD,LONG);
#else
#define FM_GETDRIVEINFO FM_GETDRIVEINFOA
#define FM_GETFILESEL FM_GETFILESELA
#define FM_GETFILESELLFN FM_GETFILESELLFNA
#define FMS_GETFILESEL FMS_GETFILESELA
#define LPFMS_GETFILESEL LPFMS_GETFILESELA
#define FMS_GETDRIVEINFO FMS_GETDRIVEINFOA
#define LPFMS_GETDRIVEINFO LPFMS_GETDRIVEINFOA
#define FMS_LOAD FMS_LOADA
#define LPFMS_LOAD LPFMS_LOADA
#define FMS_HELPSTRING FMS_HELPSTRINGA
#define LPFMS_HELPSTRING LPFMS_HELPSTRINGA
LONG WINAPI FMExtensionProc(HWND,WORD,LONG);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _WFEXT_H */

