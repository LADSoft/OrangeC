/* 
   shafunc.h

   shellapi functions

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

*/ 

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */



DECLARE_HANDLE(HDROP);

SHSTDAPI_(UINT) DragQueryFileA(HDROP,UINT,LPSTR,UINT);
SHSTDAPI_(UINT) DragQueryFileW(HDROP,UINT,LPWSTR,UINT);
SHSTDAPI_(BOOL) DragQueryPoint(HDROP,LPPOINT);
SHSTDAPI_(void) DragFinish(HDROP);
SHSTDAPI_(void) DragAcceptFiles(HWND,BOOL);
SHSTDAPI_(HINSTANCE) ShellExecuteA(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd);
SHSTDAPI_(HINSTANCE) ShellExecuteW(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd);
SHSTDAPI_(HINSTANCE) FindExecutableA(LPCSTR lpFile, LPCSTR lpDirectory, LPSTR lpResult);
SHSTDAPI_(HINSTANCE) FindExecutableW(LPCWSTR lpFile, LPCWSTR lpDirectory, LPWSTR lpResult);
SHSTDAPI_(LPWSTR *)  CommandLineToArgvW(LPCWSTR lpCmdLine, int*pNumArgs);
SHSTDAPI_(INT) ShellAboutA(HWND hWnd, LPCSTR szApp, LPCSTR szOtherStuff, HICON hIcon);
SHSTDAPI_(INT) ShellAboutW(HWND hWnd, LPCWSTR szApp, LPCWSTR szOtherStuff, HICON hIcon);
SHSTDAPI_(HICON) DuplicateIcon(HINSTANCE hInst, HICON hIcon);
SHSTDAPI_(HICON) ExtractAssociatedIconA(HINSTANCE hInst, LPSTR lpIconPath, LPWORD lpiIcon);
SHSTDAPI_(HICON) ExtractAssociatedIconW(HINSTANCE hInst, LPWSTR lpIconPath, LPWORD lpiIcon);
SHSTDAPI_(HICON) ExtractIconA(HINSTANCE hInst, LPCSTR lpszExeFileName, UINT nIconIndex);
SHSTDAPI_(HICON) ExtractIconW(HINSTANCE hInst, LPCWSTR lpszExeFileName, UINT nIconIndex);
SHSTDAPI_(UINT) SHAppBarMessage(DWORD dwMessage, PAPPBARDATA pData);
SHSTDAPI_(DWORD)   DoEnvironmentSubstA(LPSTR szString, UINT cchString);
SHSTDAPI_(DWORD)   DoEnvironmentSubstW(LPWSTR szString, UINT cchString);
SHSTDAPI_(UINT) ExtractIconExA(LPCTSTR lpszFile, int nIconIndex, HICON *phiconLarge, HICON *phiconSmall, UINT nIcons);
SHSTDAPI_(UINT) ExtractIconExW(LPCTSTR lpszFile, int nIconIndex, HICON *phiconLarge, HICON *phiconSmall, UINT nIcons);
SHSTDAPI_(int) SHFileOperationA(LPSHFILEOPSTRUCT lpFileOp);
SHSTDAPI_(int) SHFileOperationW(LPSHFILEOPSTRUCT lpFileOp);
SHSTDAPI_(void) SHFreeNameMappings(HANDLE hNameMappings);
SHSTDAPI_(BOOL) ShellExecuteExA(LPSHELLEXECUTEINFO lpExecInfo);
SHSTDAPI_(BOOL) ShellExecuteExW(LPSHELLEXECUTEINFO lpExecInfo);
SHSTDAPI_(void) WinExecErrorA(HWND hwnd, int error, LPCSTR lpstrFileName, LPCSTR lpstrTitle);
SHSTDAPI_(void) WinExecErrorW(HWND hwnd, int error, LPCWSTR lpstrFileName, LPCWSTR lpstrTitle);
SHSTDAPI_(BOOL) SHCreateProcessAsUserW(PSHCREATEPROCESSINFO pscpi);
SHSTDAPI SHQueryRecycleBinA(LPCSTR pszRootPath, LPSHQUERYRBINFO pSHQueryRBInfo);
SHSTDAPI SHQueryRecycleBinW(LPCWSTR pszRootPath, LPSHQUERYRBINFO pSHQueryRBInfo);
SHSTDAPI SHEmptyRecycleBinA(HWND hwnd, LPCSTR pszRootPath, DWORD dwFlags);
SHSTDAPI SHEmptyRecycleBinW(HWND hwnd, LPCWSTR pszRootPath, DWORD dwFlags);
SHSTDAPI_(BOOL) Shell_NotifyIconA(DWORD dwMessage, PNOTIFYICONDATA lpData);
SHSTDAPI_(BOOL) Shell_NotifyIconW(DWORD dwMessage, PNOTIFYICONDATA lpData);
SHSTDAPI_(DWORD_PTR) SHGetFileInfoA(LPCSTR pszPath, DWORD dwFileAttributes, SHFILEINFO *psfi, UINT cbFileInfo, UINT uFlags);
SHSTDAPI_(DWORD_PTR) SHGetFileInfoW(LPCWSTR pszPath, DWORD dwFileAttributes, SHFILEINFO *psfi, UINT cbFileInfo, UINT uFlags);
SHSTDAPI_(BOOL) SHGetDiskFreeSpaceExA(LPCSTR pszDirectoryName, ULARGE_INTEGER* pulFreeBytesAvailableToCaller, ULARGE_INTEGER* pulTotalNumberOfBytes, ULARGE_INTEGER* pulTotalNumberOfFreeBytes);
SHSTDAPI_(BOOL) SHGetDiskFreeSpaceExW(LPCWSTR pszDirectoryName, ULARGE_INTEGER* pulFreeBytesAvailableToCaller, ULARGE_INTEGER* pulTotalNumberOfBytes, ULARGE_INTEGER* pulTotalNumberOfFreeBytes);
SHSTDAPI_(BOOL) SHGetNewLinkInfoA(LPCSTR pszLinkTo, LPCSTR pszDir, LPSTR pszName, BOOL *pfMustCopy, UINT uFlags);
SHSTDAPI_(BOOL) SHGetNewLinkInfoW(LPCWSTR pszLinkTo, LPCWSTR pszDir, LPWSTR pszName, BOOL *pfMustCopy, UINT uFlags);
SHSTDAPI_(BOOL) SHInvokePrinterCommandA(HWND hwnd, UINT uAction, LPCSTR lpBuf1, LPCSTR lpBuf2, BOOL fModal);
SHSTDAPI_(BOOL) SHInvokePrinterCommandW(HWND hwnd, UINT uAction, LPCWSTR lpBuf1, LPCWSTR lpBuf2, BOOL fModal);
SHSTDAPI SHLoadNonloadedIconOverlayIdentifiers(void);

/* this is actually out of shlobj.h... */
SHSTDAPI SHAddToRecentDocs(UINT uFlags, LPCVOID pv);

#ifdef UNICODE
#define DragQueryFile  DragQueryFileW
#define ShellExecute  ShellExecuteW
#define FindExecutable  FindExecutableW
#define ShellAbout  ShellAboutW
#define ExtractAssociatedIcon  ExtractAssociatedIconW
#define ExtractIcon  ExtractIconW
#define DoEnvironmentSubst  DoEnvironmentSubstW
#define ExtractIconEx  ExtractIconExW
#define SHFileOperation  SHFileOperationW
#define ShellExecuteEx  ShellExecuteExW
#define WinExecError  WinExecErrorW
#define SHQueryRecycleBin  SHQueryRecycleBinW
#define SHEmptyRecycleBin  SHEmptyRecycleBinW
#define Shell_NotifyIcon  Shell_NotifyIconW
#define SHGetFileInfo  SHGetFileInfoW
#define SHGetDiskFreeSpaceEx  SHGetDiskFreeSpaceExW
#define SHGetNewLinkInfo  SHGetNewLinkInfoW
#define SHInvokePrinterCommand  SHInvokePrinterCommandW
#else
#define DragQueryFile  DragQueryFileA
#define ShellExecute  ShellExecuteA
#define FindExecutable  FindExecutableA
#define ShellAbout  ShellAboutA
#define ExtractAssociatedIcon  ExtractAssociatedIconA
#define ExtractIcon  ExtractIconA
#define DoEnvironmentSubst  DoEnvironmentSubstA
#define ExtractIconEx  ExtractIconExA
#define SHFileOperation  SHFileOperationA
#define ShellExecuteEx  ShellExecuteExA
#define WinExecError  WinExecErrorA
#define SHQueryRecycleBin  SHQueryRecycleBinA
#define SHEmptyRecycleBin  SHEmptyRecycleBinA
#define Shell_NotifyIcon  Shell_NotifyIconA
#define SHGetFileInfo  SHGetFileInfoA
#define SHGetDiskFreeSpaceEx  SHGetDiskFreeSpaceExA
#define SHGetNewLinkInfo  SHGetNewLinkInfoA
#define SHInvokePrinterCommand  SHInvokePrinterCommandA
#endif

#define SHGetDiskFreeSpace SHGetDiskFreeSpaceEx

#ifdef __cplusplus
}
#endif  /* __cplusplus */


