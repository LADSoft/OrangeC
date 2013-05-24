/* 
   shastruc.h

   shell api definitions

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

#pragma pack(1)
typedef struct _DRAGINFO {
    UINT uSize;
    POINT pt;
    BOOL fNC;
    LPTSTR   lpFileList;
    DWORD grfKeyState;
} DRAGINFOA, *LPDRAGINFO;

typedef struct _AppBarData
{
    DWORD cbSize;
    HWND hWnd;
    UINT uCallbackMessage;
    UINT uEdge;
    RECT rc;
    LPARAM lParam;
} APPBARDATA, *PAPPBARDATA;

typedef struct _SHFILEOPSTRUCT
{
        HWND            hwnd;
        UINT            wFunc;
        LPCTSTR         pFrom;
        LPCTSTR         pTo;
        FILEOP_FLAGS    fFlags;
        BOOL            fAnyOperationsAborted;
        LPVOID          hNameMappings;
        LPCTSTR         lpszProgressTitle;
} SHFILEOPSTRUCT, *LPSHFILEOPSTRUCT;

typedef struct _SHNAMEMAPPING
{
    LPTSTR  pszOldPath;
    LPTSTR  pszNewPath;
    int   cchOldPath;
    int   cchNewPath;
} SHNAMEMAPPING, *LPSHNAMEMAPPING;

typedef struct _SHELLEXECUTEINFO
{
        DWORD cbSize;
        ULONG fMask;
        HWND hwnd;
        LPCTSTR   lpVerb;
        LPCTSTR   lpFile;
        LPCTSTR   lpParameters;
        LPCTSTR   lpDirectory;
        int nShow;
        HINSTANCE hInstApp;
        // Optional fields
        LPVOID lpIDList;
        LPCTSTR   lpClass;
        HKEY hkeyClass;
        DWORD dwHotKey;
        union {
            HANDLE hIcon;
            HANDLE hMonitor;
        };
        HANDLE hProcess;
} SHELLEXECUTEINFO, *LPSHELLEXECUTEINFO;

typedef struct _SHCREATEPROCESSINFO
{
        DWORD cbSize;
        ULONG fMask;
        HWND hwnd;
        LPCTSTR  pszFile;
        LPCTSTR  pszParameters;
        LPCTSTR  pszCurrentDirectory;
        HANDLE hUserToken;
        LPSECURITY_ATTRIBUTES lpProcessAttributes;
        LPSECURITY_ATTRIBUTES lpThreadAttributes;
        BOOL bInheritHandles;
        DWORD dwCreationFlags;
        LPSTARTUPINFO lpStartupInfo;
        LPPROCESS_INFORMATION lpProcessInformation;
} SHCREATEPROCESSINFO, *PSHCREATEPROCESSINFO;

// struct for query recycle bin info
typedef struct _SHQUERYRBINFO {
    DWORD   cbSize;
    LONGLONG i64Size;
    LONGLONG i64NumItems;
} SHQUERYRBINFO, *LPSHQUERYRBINFO;

typedef struct _NOTIFYICONDATA {
        DWORD cbSize;
        HWND hWnd;
        UINT uID;
        UINT uFlags;
        UINT uCallbackMessage;
        HICON hIcon;
        BCHAR   szTip[128]; // only 64 befire IE 5
        /* the rest of this is IE 5 */
        DWORD dwState;
        DWORD dwStateMask;
        BCHAR   szInfo[256];
        union {
            UINT  uTimeout;
            UINT  uVersion;
        };
        BCHAR   szInfoTitle[64];
        DWORD dwInfoFlags;
} NOTIFYICONDATA, *PNOTIFYICONDATA;

typedef struct _SHELLEXECUTEINFOA {
    DWORD cbSize;
    ULONG fMask;
    HWND hwnd;
    LPCSTR lpVerb;
    LPCSTR lpFile;
    LPCSTR lpParameters;
    LPCSTR lpDirectory;
    int nShow;
    HINSTANCE hInstApp;
    PVOID lpIDList;
    LPCSTR lpClass;
    HKEY hkeyClass;
    DWORD dwHotKey;
    HANDLE hIcon;
    HANDLE hProcess;
} SHELLEXECUTEINFOA,*LPSHELLEXECUTEINFOA;
typedef struct _SHELLEXECUTEINFOW {
    DWORD cbSize;
    ULONG fMask;
    HWND hwnd;
    LPCWSTR lpVerb;
    LPCWSTR lpFile;
    LPCWSTR lpParameters;
    LPCWSTR lpDirectory;
    int nShow;
    HINSTANCE hInstApp;
    PVOID lpIDList;
    LPCWSTR lpClass;
    HKEY hkeyClass;
    DWORD dwHotKey;
    HANDLE hIcon;
    HANDLE hProcess;
} SHELLEXECUTEINFOW,*LPSHELLEXECUTEINFOW;

#ifndef SHFILEINFO_DEFINED
#define SHFILEINFO_DEFINED
typedef struct _SHFILEINFO
{
        HICON       hIcon;
        int         iIcon;
        DWORD       dwAttributes;
        BCHAR        szDisplayName[MAX_PATH];
        BCHAR        szTypeName[80];
} SHFILEINFO;
#endif // !SHFILEINFO_DEFINED
#pragma pack()
