#ifndef _CPL_H
#define _CPL_H

#ifdef __ORANGEC__ 
#pragma once
#endif

/* Windows Control panel extension DLL definitions */

#ifdef __cplusplus
extern "C" {
#endif

#include <pshpack1.h>

#define WM_CPL_LAUNCH  (WM_USER+1000)
#define WM_CPL_LAUNCHED  (WM_USER+1001)

#define CPL_DYNAMIC_RES  0
#define CPL_INIT  1
#define CPL_GETCOUNT  2
#define CPL_INQUIRE  3
#define CPL_SELECT  4
#define CPL_DBLCLK  5
#define CPL_STOP  6
#define CPL_EXIT  7
#define CPL_NEWINQUIRE  8
#define CPL_STARTWPARMSA  9
#define CPL_STARTWPARMSW  10
#define CPL_SETUP  200

typedef LONG (APIENTRY *APPLET_PROC)(HWND,UINT,LPARAM,LPARAM);

typedef struct tagCPLINFO {
    int idIcon;
    int idName;
    int idInfo;
    LONG_PTR lData;
} CPLINFO, *LPCPLINFO;

typedef struct tagNEWCPLINFOA {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwHelpContext;
    LONG_PTR lData;
    HICON hIcon;
    CHAR szName[32];
    CHAR szInfo[64];
    CHAR szHelpFile[128];
} NEWCPLINFOA, *LPNEWCPLINFOA;

typedef struct tagNEWCPLINFOW {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwHelpContext;
    LONG_PTR lData;
    HICON hIcon;
    WCHAR szName[32];
    WCHAR szInfo[64];
    WCHAR szHelpFile[128];
} NEWCPLINFOW, *LPNEWCPLINFOW;

#ifdef UNICODE
typedef NEWCPLINFOW NEWCPLINFO, *LPNEWCPLINFO;
#define CPL_STARTWPARMS CPL_STARTWPARMSW
#else
typedef NEWCPLINFOA NEWCPLINFO, *LPNEWCPLINFO;
#define CPL_STARTWPARMS CPL_STARTWPARMSA
#endif

#ifdef __cplusplus
}
#endif

#include <poppack.h>

#endif /* _CPL_H */
