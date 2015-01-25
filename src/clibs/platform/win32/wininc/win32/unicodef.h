/* 
   UnicodeFunctions.h

   Declarations for all the Windows32 API Unicode Functions

   Copyright (C) 1996 Free Software Foundation, Inc.

   Author:  Scott Christley <scottc@net-community.com>
   Date: 1996
   
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

#ifndef _GNU_H_WINDOWS32_UNICODEFUNCTIONS
#define _GNU_H_WINDOWS32_UNICODEFUNCTIONS


#ifndef	RC_INVOKED

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern WINBOOL
 PASCAL WINBASEAPI GetBinaryTypeW(
    LPCWSTR lpApplicationName,
    LPDWORD lpBinaryType
    );

extern DWORD
 PASCAL WINBASEAPI GetShortPathNameW(
    LPCWSTR lpszLongPath,
    LPWSTR  lpszShortPath,
    DWORD    cchBuffer
    );

extern LPWSTR
 PASCAL WINBASEAPI GetEnvironmentStringsW(
    VOID
    );

extern WINBOOL
 PASCAL WINBASEAPI FreeEnvironmentStringsW(
    LPWSTR
    );

extern DWORD
 PASCAL WINBASEAPI FormatMessageW(
    DWORD dwFlags,
    LPCVOID lpSource,
    DWORD dwMessageId,
    DWORD dwLanguageId,
    LPWSTR lpBuffer,
    DWORD nSize,
    va_list *Arguments
    );

extern HANDLE
 PASCAL WINBASEAPI CreateMailslotW(
    LPCWSTR lpName,
    DWORD nMaxMessageSize,
    DWORD lReadTimeout,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );

extern int
 PASCAL WINBASEAPI lstrcmpW(
    LPCWSTR lpString1,
    LPCWSTR lpString2
    );

extern int
 PASCAL WINBASEAPI lstrcmpiW(
    LPCWSTR lpString1,
    LPCWSTR lpString2
    );

extern LPWSTR
 PASCAL WINBASEAPI lstrcpynW(
    LPWSTR lpString1,
    LPCWSTR lpString2,
    int iMaxLength
    );

extern LPWSTR
 PASCAL WINBASEAPI lstrcpyW(
    LPWSTR lpString1,
    LPCWSTR lpString2
    );

extern LPWSTR
 PASCAL WINBASEAPI lstrcatW(
    LPWSTR lpString1,
    LPCWSTR lpString2
    );

extern int
 PASCAL WINBASEAPI lstrlenW(
    LPCWSTR lpString
    );

extern HANDLE
 PASCAL WINBASEAPI CreateMutexW(
    LPSECURITY_ATTRIBUTES lpMutexAttributes,
    WINBOOL bInitialOwner,
    LPCWSTR lpName
    );

extern HANDLE
 PASCAL WINBASEAPI OpenMutexW(
    DWORD dwDesiredAccess,
    WINBOOL bInheritHandle,
    LPCWSTR lpName
    );

extern HANDLE
 PASCAL WINBASEAPI CreateEventW(
    LPSECURITY_ATTRIBUTES lpEventAttributes,
    WINBOOL bManualReset,
    WINBOOL bInitialState,
    LPCWSTR lpName
    );

extern HANDLE
 PASCAL WINBASEAPI OpenEventW(
    DWORD dwDesiredAccess,
    WINBOOL bInheritHandle,
    LPCWSTR lpName
    );

extern HANDLE
 PASCAL WINBASEAPI CreateSemaphoreW(
         LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
         LONG lInitialCount,
         LONG lMaximumCount,
         LPCWSTR lpName
         );

extern HANDLE
 PASCAL WINBASEAPI OpenSemaphoreW(
    DWORD dwDesiredAccess,
    WINBOOL bInheritHandle,
    LPCWSTR lpName
    );

extern HANDLE
 PASCAL WINBASEAPI CreateFileMappingW(
    HANDLE hFile,
    LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
    DWORD flProtect,
    DWORD dwMaximumSizeHigh,
    DWORD dwMaximumSizeLow,
    LPCWSTR lpName
    );

extern HANDLE
 PASCAL WINBASEAPI OpenFileMappingW(
    DWORD dwDesiredAccess,
    WINBOOL bInheritHandle,
    LPCWSTR lpName
    );

extern DWORD
 PASCAL WINBASEAPI GetLogicalDriveStringsW(
    DWORD nBufferLength,
    LPWSTR lpBuffer
    );

extern HINSTANCE
 PASCAL WINBASEAPI LoadLibraryW(
    LPCWSTR lpLibFileName
    );

extern HINSTANCE
 PASCAL WINBASEAPI LoadLibraryExW(
    LPCWSTR lpLibFileName,
    HANDLE hFile,
    DWORD dwFlags
    );

extern DWORD
 PASCAL WINBASEAPI GetModuleFileNameW(
    HINSTANCE hModule,
    LPWSTR lpFilename,
    DWORD nSize
    );

extern HMODULE
 PASCAL WINBASEAPI GetModuleHandleW(
    LPCWSTR lpModuleName
    );

extern VOID
 PASCAL WINBASEAPI FatalAppExitW(
    UINT uAction,
    LPCWSTR lpMessageText
    );

extern LPWSTR
 PASCAL WINBASEAPI GetCommandLineW(
    VOID
    );

extern DWORD
 PASCAL WINBASEAPI GetEnvironmentVariableW(
    LPCWSTR lpName,
    LPWSTR lpBuffer,
    DWORD nSize
    );

extern WINBOOL
 PASCAL WINBASEAPI SetEnvironmentVariableW(
    LPCWSTR lpName,
    LPCWSTR lpValue
    );

extern DWORD
 PASCAL WINBASEAPI ExpandEnvironmentStringsW(
    LPCWSTR lpSrc,
    LPWSTR lpDst,
    DWORD nSize
    );

extern VOID
 PASCAL WINBASEAPI OutputDebugStringW(
    LPCWSTR lpOutputString
    );

extern HRSRC
 PASCAL WINBASEAPI FindResourceW(
    HINSTANCE hModule,
    LPCWSTR lpName,
    LPCWSTR lpType
    );

extern HRSRC
 PASCAL WINBASEAPI FindResourceExW(
    HINSTANCE hModule,
    LPCWSTR lpType,
    LPCWSTR lpName,
    WORD    wLanguage
    );

extern WINBOOL
 PASCAL WINBASEAPI EnumResourceTypesW(
    HINSTANCE hModule,
    ENUMRESTYPEPROC lpEnumFunc,
    LONG lParam
    );

extern WINBOOL
 PASCAL WINBASEAPI EnumResourceNamesW(
    HINSTANCE hModule,
    LPCWSTR lpType,
    ENUMRESNAMEPROC lpEnumFunc,
    LONG lParam
    );

extern WINBOOL
 PASCAL WINBASEAPI EnumResourceLanguagesW(
    HINSTANCE hModule,
    LPCWSTR lpType,
    LPCWSTR lpName,
    ENUMRESLANGPROC lpEnumFunc,
    LONG lParam
    );

extern HANDLE
 PASCAL WINBASEAPI BeginUpdateResourceW(
    LPCWSTR pFileName,
    WINBOOL bDeleteExistingResources
    );

extern WINBOOL
 PASCAL WINBASEAPI UpdateResourceW(
    HANDLE      hUpdate,
    LPCWSTR     lpType,
    LPCWSTR     lpName,
    WORD        wLanguage,
    LPVOID      lpData,
    DWORD       cbData
    );

extern WINBOOL
 PASCAL WINBASEAPI EndUpdateResourceW(
    HANDLE      hUpdate,
    WINBOOL        fDiscard
    );

extern ATOM
 PASCAL WINBASEAPI GlobalAddAtomW(
    LPCWSTR lpString
    );

extern ATOM
 PASCAL WINBASEAPI GlobalFindAtomW(
    LPCWSTR lpString
    );

extern UINT
 PASCAL WINBASEAPI GlobalGetAtomNameW(
    ATOM nAtom,
    LPWSTR lpBuffer,
    int nSize
    );

extern ATOM
 PASCAL WINBASEAPI AddAtomW(
    LPCWSTR lpString
    );

extern ATOM
 PASCAL WINBASEAPI FindAtomW(
    LPCWSTR lpString
    );

extern UINT
 PASCAL WINBASEAPI GetAtomNameW(
    ATOM nAtom,
    LPWSTR lpBuffer,
    int nSize
    );

extern UINT
 PASCAL WINBASEAPI GetProfileIntW(
    LPCWSTR lpAppName,
    LPCWSTR lpKeyName,
    INT nDefault
    );

extern DWORD
 PASCAL WINBASEAPI GetProfileStringW(
    LPCWSTR lpAppName,
    LPCWSTR lpKeyName,
    LPCWSTR lpDefault,
    LPWSTR lpReturnedString,
    DWORD nSize
    );

extern WINBOOL
 PASCAL WINBASEAPI WriteProfileStringW(
    LPCWSTR lpAppName,
    LPCWSTR lpKeyName,
    LPCWSTR lpString
    );

extern DWORD
 PASCAL WINBASEAPI GetProfileSectionW(
    LPCWSTR lpAppName,
    LPWSTR lpReturnedString,
    DWORD nSize
    );

extern WINBOOL
 PASCAL WINBASEAPI WriteProfileSectionW(
    LPCWSTR lpAppName,
    LPCWSTR lpString
    );

extern UINT
 PASCAL WINBASEAPI GetPrivateProfileIntW(
    LPCWSTR lpAppName,
    LPCWSTR lpKeyName,
    INT nDefault,
    LPCWSTR lpFileName
    );

extern DWORD
 PASCAL WINBASEAPI GetPrivateProfileStringW(
    LPCWSTR lpAppName,
    LPCWSTR lpKeyName,
    LPCWSTR lpDefault,
    LPWSTR lpReturnedString,
    DWORD nSize,
    LPCWSTR lpFileName
    );

extern WINBOOL
 PASCAL WINBASEAPI WritePrivateProfileStringW(
    LPCWSTR lpAppName,
    LPCWSTR lpKeyName,
    LPCWSTR lpString,
    LPCWSTR lpFileName
    );

extern DWORD
 PASCAL WINBASEAPI GetPrivateProfileSectionW(
    LPCWSTR lpAppName,
    LPWSTR lpReturnedString,
    DWORD nSize,
    LPCWSTR lpFileName
    );

extern DWORD PASCAL WINBASEAPI GetPrivateProfileSectionNamesW(LPWSTR,DWORD,LPCWSTR);

extern WINBOOL
 PASCAL WINBASEAPI WritePrivateProfileSectionW(
    LPCWSTR lpAppName,
    LPCWSTR lpString,
    LPCWSTR lpFileName
    );

extern UINT
 PASCAL WINBASEAPI GetDriveTypeW(
    LPCWSTR lpRootPathName
    );

extern UINT
 PASCAL WINBASEAPI GetSystemDirectoryW(
    LPWSTR lpBuffer,
    UINT uSize
    );

extern DWORD
 PASCAL WINBASEAPI GetTempPathW(
    DWORD nBufferLength,
    LPWSTR lpBuffer
    );

extern UINT
 PASCAL WINBASEAPI GetTempFileNameW(
    LPCWSTR lpPathName,
    LPCWSTR lpPrefixString,
    UINT uUnique,
    LPWSTR lpTempFileName
    );

extern UINT
 PASCAL WINBASEAPI GetWindowsDirectoryW(
    LPWSTR lpBuffer,
    UINT uSize
    );

extern WINBOOL
 PASCAL WINBASEAPI SetCurrentDirectoryW(
    LPCWSTR lpPathName
    );

extern DWORD
 PASCAL WINBASEAPI GetCurrentDirectoryW(
    DWORD nBufferLength,
    LPWSTR lpBuffer
    );

extern WINBOOL
 PASCAL WINBASEAPI GetDiskFreeSpaceW(
    LPCWSTR lpRootPathName,
    LPDWORD lpSectorsPerCluster,
    LPDWORD lpBytesPerSector,
    LPDWORD lpNumberOfFreeClusters,
    LPDWORD lpTotalNumberOfClusters
    );

extern WINBOOL
 PASCAL WINBASEAPI CreateDirectoryW(
    LPCWSTR lpPathName,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );

extern WINBOOL
 PASCAL WINBASEAPI CreateDirectoryExW(
    LPCWSTR lpTemplateDirectory,
    LPCWSTR lpNewDirectory,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );

extern WINBOOL
 PASCAL WINBASEAPI RemoveDirectoryW(
    LPCWSTR lpPathName
    );

extern DWORD
 PASCAL WINBASEAPI GetFullPathNameW(
    LPCWSTR lpFileName,
    DWORD nBufferLength,
    LPWSTR lpBuffer,
    LPWSTR *lpFilePart
    );

extern WINBOOL
 PASCAL WINBASEAPI DefineDosDeviceW(
    DWORD dwFlags,
    LPCWSTR lpDeviceName,
    LPCWSTR lpTargetPath
    );

extern DWORD
 PASCAL WINBASEAPI QueryDosDeviceW(
    LPCWSTR lpDeviceName,
    LPWSTR lpTargetPath,
    DWORD ucchMax
    );

extern HANDLE
 PASCAL WINBASEAPI CreateFileW(
    LPCWSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
    );

extern WINBOOL
 PASCAL WINBASEAPI SetFileAttributesW(
    LPCWSTR lpFileName,
    DWORD dwFileAttributes
    );

extern DWORD
 PASCAL WINBASEAPI GetFileAttributesW(
    LPCWSTR lpFileName
    );

extern DWORD
 PASCAL WINBASEAPI GetCompressedFileSizeW(
    LPCWSTR lpFileName,
    LPDWORD lpFileSizeHigh
    );

extern WINBOOL
 PASCAL WINBASEAPI DeleteFileW(
    LPCWSTR lpFileName
    );

extern DWORD
 PASCAL WINBASEAPI SearchPathW(
    LPCWSTR lpPath,
    LPCWSTR lpFileName,
    LPCWSTR lpExtension,
    DWORD nBufferLength,
    LPWSTR lpBuffer,
    LPWSTR *lpFilePart
    );

extern WINBOOL
 PASCAL WINBASEAPI CopyFileW(
    LPCWSTR lpExistingFileName,
    LPCWSTR lpNewFileName,
    WINBOOL bFailIfExists
    );

extern WINBOOL
 PASCAL WINBASEAPI MoveFileW(
    LPCWSTR lpExistingFileName,
    LPCWSTR lpNewFileName
    );

extern WINBOOL
 PASCAL WINBASEAPI MoveFileExW(
    LPCWSTR lpExistingFileName,
    LPCWSTR lpNewFileName,
    DWORD dwFlags
    );

extern HANDLE
 PASCAL WINBASEAPI CreateNamedPipeW(
    LPCWSTR lpName,
    DWORD dwOpenMode,
    DWORD dwPipeMode,
    DWORD nMaxInstances,
    DWORD nOutBufferSize,
    DWORD nInBufferSize,
    DWORD nDefaultTimeOut,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );

extern WINBOOL
 PASCAL WINBASEAPI GetNamedPipeHandleStateW(
    HANDLE hNamedPipe,
    LPDWORD lpState,
    LPDWORD lpCurInstances,
    LPDWORD lpMaxCollectionCount,
    LPDWORD lpCollectDataTimeout,
    LPWSTR lpUserName,
    DWORD nMaxUserNameSize
    );

extern WINBOOL
 PASCAL WINBASEAPI CallNamedPipeW(
    LPCWSTR lpNamedPipeName,
    LPVOID lpInBuffer,
    DWORD nInBufferSize,
    LPVOID lpOutBuffer,
    DWORD nOutBufferSize,
    LPDWORD lpBytesRead,
    DWORD nTimeOut
    );

extern WINBOOL
 PASCAL WINBASEAPI WaitNamedPipeW(
    LPCWSTR lpNamedPipeName,
    DWORD nTimeOut
    );

extern WINBOOL
 PASCAL WINBASEAPI SetVolumeLabelW(
    LPCWSTR lpRootPathName,
    LPCWSTR lpVolumeName
    );

extern WINBOOL
 PASCAL WINBASEAPI GetVolumeInformationW(
    LPCWSTR lpRootPathName,
    LPWSTR lpVolumeNameBuffer,
    DWORD nVolumeNameSize,
    LPDWORD lpVolumeSerialNumber,
    LPDWORD lpMaximumComponentLength,
    LPDWORD lpFileSystemFlags,
    LPWSTR lpFileSystemNameBuffer,
    DWORD nFileSystemNameSize
    );

extern WINBOOL
 PASCAL WINBASEAPI ClearEventLogW(
    HANDLE hEventLog,
    LPCWSTR lpBackupFileName
    );

extern WINBOOL
 PASCAL WINBASEAPI BackupEventLogW(
    HANDLE hEventLog,
    LPCWSTR lpBackupFileName
    );

extern HANDLE
 PASCAL WINBASEAPI OpenEventLogW(
    LPCWSTR lpUNCServerName,
    LPCWSTR lpSourceName
    );

extern HANDLE
 PASCAL WINBASEAPI RegisterEventSourceW(
    LPCWSTR lpUNCServerName,
    LPCWSTR lpSourceName
    );

extern HANDLE
 PASCAL WINBASEAPI OpenBackupEventLogW(
    LPCWSTR lpUNCServerName,
    LPCWSTR lpFileName
    );

extern WINBOOL
 PASCAL WINBASEAPI ReadEventLogW(
     HANDLE     hEventLog,
     DWORD      dwReadFlags,
     DWORD      dwRecordOffset,
     LPVOID     lpBuffer,
     DWORD      nNumberOfBytesToRead,
     DWORD      *pnBytesRead,
     DWORD      *pnMinNumberOfBytesNeeded
    );

extern WINBOOL
 PASCAL WINBASEAPI ReportEventW(
     HANDLE     hEventLog,
     WORD       wType,
     WORD       wCategory,
     DWORD      dwEventID,
     PSID       lpUserSid,
     WORD       wNumStrings,
     DWORD      dwDataSize,
     LPCWSTR   *lpStrings,
     LPVOID     lpRawData
    );

extern WINBOOL
 PASCAL WINBASEAPI AccessCheckAndAuditAlarmW(
    LPCWSTR SubsystemName,
    LPVOID HandleId,
    LPWSTR ObjectTypeName,
    LPWSTR ObjectName,
    PSECURITY_DESCRIPTOR SecurityDescriptor,
    DWORD DesiredAccess,
    PGENERIC_MAPPING GenericMapping,
    WINBOOL ObjectCreation,
    LPDWORD GrantedAccess,
    LPBOOL AccessStatus,
    LPBOOL pfGenerateOnClose
    );

extern WINBOOL
 PASCAL WINBASEAPI ObjectOpenAuditAlarmW(
    LPCWSTR SubsystemName,
    LPVOID HandleId,
    LPWSTR ObjectTypeName,
    LPWSTR ObjectName,
    PSECURITY_DESCRIPTOR pSecurityDescriptor,
    HANDLE ClientToken,
    DWORD DesiredAccess,
    DWORD GrantedAccess,
    PPRIVILEGE_SET Privileges,
    WINBOOL ObjectCreation,
    WINBOOL AccessGranted,
    LPBOOL GenerateOnClose
    );

extern WINBOOL
 PASCAL WINBASEAPI ObjectPrivilegeAuditAlarmW(
    LPCWSTR SubsystemName,
    LPVOID HandleId,
    HANDLE ClientToken,
    DWORD DesiredAccess,
    PPRIVILEGE_SET Privileges,
    WINBOOL AccessGranted
    );

extern WINBOOL
 PASCAL WINBASEAPI ObjectCloseAuditAlarmW(
    LPCWSTR SubsystemName,
    LPVOID HandleId,
    WINBOOL GenerateOnClose
    );

extern WINBOOL
 PASCAL WINBASEAPI PrivilegedServiceAuditAlarmW(
    LPCWSTR SubsystemName,
    LPCWSTR ServiceName,
    HANDLE ClientToken,
    PPRIVILEGE_SET Privileges,
    WINBOOL AccessGranted
    );

extern WINBOOL
 PASCAL WINBASEAPI SetFileSecurityW(
    LPCWSTR lpFileName,
    SECURITY_INFORMATION SecurityInformation,
    PSECURITY_DESCRIPTOR pSecurityDescriptor
    );

extern WINBOOL
 PASCAL WINBASEAPI GetFileSecurityW(
    LPCWSTR lpFileName,
    SECURITY_INFORMATION RequestedInformation,
    PSECURITY_DESCRIPTOR pSecurityDescriptor,
    DWORD nLength,
    LPDWORD lpnLengthNeeded
    );

extern HANDLE
 PASCAL WINBASEAPI FindFirstChangeNotificationW(
    LPCWSTR lpPathName,
    WINBOOL bWatchSubtree,
    DWORD dwNotifyFilter
    );

extern WINBOOL
 PASCAL WINBASEAPI IsBadStringPtrW(
    LPCWSTR lpsz,
    UINT ucchMax
    );

extern WINBOOL
 PASCAL WINBASEAPI LookupAccountSidW(
    LPCWSTR lpSystemName,
    PSID Sid,
    LPWSTR Name,
    LPDWORD cbName,
    LPWSTR ReferencedDomainName,
    LPDWORD cbReferencedDomainName,
    PSID_NAME_USE peUse
    );

extern WINBOOL
 PASCAL WINBASEAPI LookupAccountNameW(
    LPCWSTR lpSystemName,
    LPCWSTR lpAccountName,
    PSID Sid,
    LPDWORD cbSid,
    LPWSTR ReferencedDomainName,
    LPDWORD cbReferencedDomainName,
    PSID_NAME_USE peUse
    );

extern WINBOOL
 PASCAL WINBASEAPI LookupPrivilegeValueW(
    LPCWSTR lpSystemName,
    LPCWSTR lpName,
    PLUID   lpLuid
    );

extern WINBOOL
 PASCAL WINBASEAPI LookupPrivilegeNameW(
    LPCWSTR lpSystemName,
    PLUID   lpLuid,
    LPWSTR lpName,
    LPDWORD cbName
    );

extern WINBOOL
 PASCAL WINBASEAPI LookupPrivilegeDisplayNameW(
    LPCWSTR lpSystemName,
    LPCWSTR lpName,
    LPWSTR lpDisplayName,
    LPDWORD cbDisplayName,
    LPDWORD lpLanguageId
    );

extern WINBOOL
 PASCAL WINBASEAPI BuildCommDCBW(
    LPCWSTR lpDef,
    LPDCB lpDCB
    );

extern WINBOOL
 PASCAL WINBASEAPI BuildCommDCBAndTimeoutsW(
    LPCWSTR lpDef,
    LPDCB lpDCB,
    LPCOMMTIMEOUTS lpCommTimeouts
    );

extern WINBOOL
 PASCAL WINBASEAPI CommConfigDialogW(
    LPCWSTR lpszName,
    HWND hWnd,
    LPCOMMCONFIG lpCC
    );

extern WINBOOL
 PASCAL WINBASEAPI GetDefaultCommConfigW(
    LPCWSTR lpszName,
    LPCOMMCONFIG lpCC,
    LPDWORD lpdwSize
    );

extern WINBOOL
 PASCAL WINBASEAPI SetDefaultCommConfigW(
    LPCWSTR lpszName,
    LPCOMMCONFIG lpCC,
    DWORD dwSize
    );

extern WINBOOL
 PASCAL WINBASEAPI GetComputerNameW(
    LPWSTR lpBuffer,
    LPDWORD nSize
    );

extern WINBOOL
 PASCAL WINBASEAPI SetComputerNameW(
    LPCWSTR lpComputerName
    );

extern WINBOOL
 PASCAL WINBASEAPI GetUserNameW(
    LPWSTR lpBuffer,
    LPDWORD nSize
    );

extern int
 PASCAL WINBASEAPI wvsprintfW(
    LPWSTR,
    LPCWSTR,
    va_list arglist);

extern int
 WINBASEAPI wsprintfW(LPWSTR, LPCWSTR, ...);

extern HKL
 PASCAL WINBASEAPI LoadKeyboardLayoutW(
    LPCWSTR pwszKLID,
    UINT Flags);

extern WINBOOL
 PASCAL WINBASEAPI GetKeyboardLayoutNameW(
    LPWSTR pwszKLID);

extern HDESK
 PASCAL WINBASEAPI CreateDesktopW(
    LPWSTR lpszDesktop,
    LPWSTR lpszDevice,
    LPDEVMODE pDevmode,
    DWORD dwFlags,
    DWORD dwDesiredAccess,
    LPSECURITY_ATTRIBUTES lpsa);

extern HDESK
 PASCAL WINBASEAPI OpenDesktopW(
    LPWSTR lpszDesktop,
    DWORD dwFlags,
    WINBOOL fInherit,
    DWORD dwDesiredAccess);

extern WINBOOL
 PASCAL WINBASEAPI EnumDesktopsW(
    HWINSTA hwinsta,
    DESKTOPENUMPROC lpEnumFunc,
    LPARAM lParam);

extern HWINSTA
 PASCAL WINBASEAPI CreateWindowStationW(
    LPWSTR lpwinsta,
    DWORD dwReserved,
    DWORD dwDesiredAccess,
    LPSECURITY_ATTRIBUTES lpsa);

extern HWINSTA
 PASCAL WINBASEAPI OpenWindowStationW(
    LPWSTR lpszWinSta,
    WINBOOL fInherit,
    DWORD dwDesiredAccess);

extern WINBOOL
 PASCAL WINBASEAPI EnumWindowStationsW(
    ENUMWINDOWSTATIONPROC lpEnumFunc,
    LPARAM lParam);

extern WINBOOL
 PASCAL WINBASEAPI GetUserObjectInformationW(
    HANDLE hObj,
    int nIndex,
    PVOID pvInfo,
    DWORD nLength,
    LPDWORD lpnLengthNeeded);

extern WINBOOL
 PASCAL WINBASEAPI SetUserObjectInformationW(
    HANDLE hObj,
    int nIndex,
    PVOID pvInfo,
    DWORD nLength);

extern UINT
 PASCAL WINBASEAPI RegisterWindowMessageW(
    LPCWSTR lpString);

extern WINBOOL
 PASCAL WINBASEAPI GetMessageW(
    LPMSG lpMsg,
    HWND hWnd ,
    UINT wMsgFilterMin,
    UINT wMsgFilterMax);

extern LONG
 PASCAL WINBASEAPI DispatchMessageW(
    CONST MSG *lpMsg);

extern WINBOOL
 PASCAL WINBASEAPI PeekMessageW(
    LPMSG lpMsg,
    HWND hWnd ,
    UINT wMsgFilterMin,
    UINT wMsgFilterMax,
    UINT wRemoveMsg);

extern LRESULT
 PASCAL WINBASEAPI SendMessageW(
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam);

extern LRESULT
 PASCAL WINBASEAPI SendMessageTimeoutW(
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam,
    UINT fuFlags,
    UINT uTimeout,
    LPDWORD lpdwResult);

extern WINBOOL
 PASCAL WINBASEAPI SendNotifyMessageW(
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam);

extern WINBOOL
 PASCAL WINBASEAPI SendMessageCallbackW(
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam,
    SENDASYNCPROC lpResultCallBack,
    DWORD dwData);

extern WINBOOL
 PASCAL WINBASEAPI PostMessageW(
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam);

extern WINBOOL
 PASCAL WINBASEAPI PostThreadMessageW(
    DWORD idThread,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam);

extern LRESULT
 PASCAL WINBASEAPI DefWindowProcW(
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam);

extern LRESULT
 PASCAL WINBASEAPI CallWindowProcW(
    WNDPROC lpPrevWndFunc,
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam);

extern ATOM
 PASCAL WINBASEAPI RegisterClassW(
    CONST WNDCLASS *lpWndClass);

extern WINBOOL
 PASCAL WINBASEAPI UnregisterClassW(
    LPCWSTR lpClassName,
    HINSTANCE hInstance);

extern WINBOOL
 PASCAL WINBASEAPI GetClassInfoW(
    HINSTANCE hInstance ,
    LPCWSTR lpClassName,
    LPWNDCLASS lpWndClass);

extern ATOM
 PASCAL WINBASEAPI RegisterClassExW(CONST WNDCLASSEX *);

extern WINBOOL
 PASCAL WINBASEAPI GetClassInfoExW(HINSTANCE, LPCWSTR, LPWNDCLASSEX);

extern HWND
 PASCAL WINBASEAPI CreateWindowExW(
    DWORD dwExStyle,
    LPCWSTR lpClassName,
    LPCWSTR lpWindowName,
    DWORD dwStyle,
    int X,
    int Y,
    int nWidth,
    int nHeight,
    HWND hWndParent ,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam);

extern HWND
 PASCAL WINBASEAPI CreateDialogParamW(
    HINSTANCE hInstance,
    LPCWSTR lpTemplateName,
    HWND hWndParent ,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam);

extern HWND
 PASCAL WINBASEAPI CreateDialogIndirectParamW(
    HINSTANCE hInstance,
    LPCDLGTEMPLATE lpTemplate,
    HWND hWndParent,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam);

extern int
 PASCAL WINBASEAPI DialogBoxParamW(
    HINSTANCE hInstance,
    LPCWSTR lpTemplateName,
    HWND hWndParent ,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam);

extern int
 PASCAL WINBASEAPI DialogBoxIndirectParamW(
    HINSTANCE hInstance,
    LPCDLGTEMPLATE hDialogTemplate,
    HWND hWndParent ,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam);

extern WINBOOL
 PASCAL WINBASEAPI SetDlgItemTextW(
    HWND hDlg,
    int nIDDlgItem,
    LPCWSTR lpString);

extern UINT
 PASCAL WINBASEAPI GetDlgItemTextW(
    HWND hDlg,
    int nIDDlgItem,
    LPWSTR lpString,
    int nMaxCount);

extern LONG
 PASCAL WINBASEAPI SendDlgItemMessageW(
    HWND hDlg,
    int nIDDlgItem,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam);

extern LRESULT
 PASCAL WINBASEAPI DefDlgProcW(
    HWND hDlg,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam);

extern WINBOOL
 PASCAL WINBASEAPI CallMsgFilterW(
    LPMSG lpMsg,
    int nCode);

extern UINT
 PASCAL WINBASEAPI RegisterClipboardFormatW(
    LPCWSTR lpszFormat);

extern int
 PASCAL WINBASEAPI GetClipboardFormatNameW(
    UINT format,
    LPWSTR lpszFormatName,
    int cchMaxCount);

extern WINBOOL
 PASCAL WINBASEAPI CharToOemW(
    LPCWSTR lpszSrc,
    LPSTR lpszDst);

extern WINBOOL
 PASCAL WINBASEAPI OemToCharW(
    LPCSTR lpszSrc,
    LPWSTR lpszDst);

extern WINBOOL
 PASCAL WINBASEAPI CharToOemBuffW(
    LPCWSTR lpszSrc,
    LPSTR lpszDst,
    DWORD cchDstLength);

extern WINBOOL
 PASCAL WINBASEAPI OemToCharBuffW(
    LPCSTR lpszSrc,
    LPWSTR lpszDst,
    DWORD cchDstLength);

extern LPWSTR
 PASCAL WINBASEAPI CharUpperW(
    LPWSTR lpsz);

extern DWORD
 PASCAL WINBASEAPI CharUpperBuffW(
    LPWSTR lpsz,
    DWORD cchLength);

extern LPWSTR
 PASCAL WINBASEAPI CharLowerW(
    LPWSTR lpsz);

extern DWORD
 PASCAL WINBASEAPI CharLowerBuffW(
    LPWSTR lpsz,
    DWORD cchLength);

extern LPWSTR
 PASCAL WINBASEAPI CharNextW(
    LPCWSTR lpsz);

extern LPWSTR
 PASCAL WINBASEAPI CharPrevW(
    LPCWSTR lpszStart,
    LPCWSTR lpszCurrent);

extern WINBOOL
 PASCAL WINBASEAPI IsCharAlphaW(
    WCHAR ch);

extern WINBOOL
 PASCAL WINBASEAPI IsCharAlphaNumericW(
    WCHAR ch);

extern WINBOOL
 PASCAL WINBASEAPI IsCharUpperW(
    WCHAR ch);

extern WINBOOL
 PASCAL WINBASEAPI IsCharLowerW(
    WCHAR ch);

extern int
 PASCAL WINBASEAPI GetKeyNameTextW(
    LONG lParam,
    LPWSTR lpString,
    int nSize
    );

extern SHORT
 PASCAL WINBASEAPI VkKeyScanW(
    WCHAR ch);

extern SHORT
 PASCAL WINBASEAPI VkKeyScanExW(
    WCHAR  ch,
    HKL   dwhkl);

extern UINT
 PASCAL WINBASEAPI MapVirtualKeyW(
    UINT uCode,
    UINT uMapType);

extern UINT
 PASCAL WINBASEAPI MapVirtualKeyExW(
    UINT uCode,
    UINT uMapType,
    HKL dwhkl);

extern HACCEL
 PASCAL WINBASEAPI LoadAcceleratorsW(
    HINSTANCE hInstance,
    LPCWSTR lpTableName);

extern HACCEL
 PASCAL WINBASEAPI CreateAcceleratorTableW(
    LPACCEL, int);

extern int
 PASCAL WINBASEAPI CopyAcceleratorTableW(
    HACCEL hAccelSrc,
    LPACCEL lpAccelDst,
    int cAccelEntries);

extern int
 PASCAL WINBASEAPI TranslateAcceleratorW(
    HWND hWnd,
    HACCEL hAccTable,
    LPMSG lpMsg);

extern HMENU
 PASCAL WINBASEAPI LoadMenuW(
    HINSTANCE hInstance,
    LPCWSTR lpMenuName);

extern HMENU
 PASCAL WINBASEAPI LoadMenuIndirectW(
    CONST MENUTEMPLATE *lpMenuTemplate);

extern WINBOOL
 PASCAL WINBASEAPI ChangeMenuW(
    HMENU hMenu,
    UINT cmd,
    LPCWSTR lpszNewItem,
    UINT cmdInsert,
    UINT flags);

extern int
 PASCAL WINBASEAPI GetMenuStringW(
    HMENU hMenu,
    UINT uIDItem,
    LPWSTR lpString,
    int nMaxCount,
    UINT uFlag);

extern WINBOOL
 PASCAL WINBASEAPI InsertMenuW(
    HMENU hMenu,
    UINT uPosition,
    UINT uFlags,
    UINT uIDNewItem,
    LPCWSTR lpNewItem
    );

extern WINBOOL
 PASCAL WINBASEAPI AppendMenuW(
    HMENU hMenu,
    UINT uFlags,
    UINT uIDNewItem,
    LPCWSTR lpNewItem
    );

extern WINBOOL
 PASCAL WINBASEAPI ModifyMenuW(
    HMENU hMnu,
    UINT uPosition,
    UINT uFlags,
    UINT uIDNewItem,
    LPCWSTR lpNewItem
    );

extern WINBOOL
 PASCAL WINBASEAPI InsertMenuItemW(
    HMENU,
    UINT,
    WINBOOL,
    LPCMENUITEMINFO
    );

extern WINBOOL
 PASCAL WINBASEAPI GetMenuItemInfoW(
    HMENU,
    UINT,
    WINBOOL,
    LPMENUITEMINFO
    );

extern WINBOOL
 PASCAL WINBASEAPI SetMenuItemInfoW(
    HMENU,
    UINT,
    WINBOOL,
    LPCMENUITEMINFO
    );

extern int
 PASCAL WINBASEAPI DrawTextW(
    HDC hDC,
    LPCWSTR lpString,
    int nCount,
    LPRECT lpRect,
    UINT uFormat);

extern int
 PASCAL WINBASEAPI DrawTextExW(HDC, LPWSTR, int, LPRECT, UINT, LPDRAWTEXTPARAMS);

extern WINBOOL
 PASCAL WINBASEAPI GrayStringW(
    HDC hDC,
    HBRUSH hBrush,
    GRAYSTRINGPROC lpOutputFunc,
    LPARAM lpData,
    int nCount,
    int X,
    int Y,
    int nWidth,
    int nHeight);

extern WINBOOL  PASCAL WINBASEAPI DrawStateW(HDC, HBRUSH, DRAWSTATEPROC, LPARAM, WPARAM, int, int, int, int, UINT);

extern LONG
 PASCAL WINBASEAPI TabbedTextOutW(
    HDC hDC,
    int X,
    int Y,
    LPCWSTR lpString,
    int nCount,
    int nTabPositions,
    LPINT lpnTabStopPositions,
    int nTabOrigin);

extern DWORD
 PASCAL WINBASEAPI GetTabbedTextExtentW(
    HDC hDC,
    LPCWSTR lpString,
    int nCount,
    int nTabPositions,
    LPINT lpnTabStopPositions);

extern WINBOOL
 PASCAL WINBASEAPI SetPropW(
    HWND hWnd,
    LPCWSTR lpString,
    HANDLE hData);

extern HANDLE
 PASCAL WINBASEAPI GetPropW(
    HWND hWnd,
    LPCWSTR lpString);

extern HANDLE
 PASCAL WINBASEAPI RemovePropW(
    HWND hWnd,
    LPCWSTR lpString);

extern int
 PASCAL WINBASEAPI EnumPropsExW(
    HWND hWnd,
    PROPENUMPROCEX lpEnumFunc,
    LPARAM lParam);

extern int
 PASCAL WINBASEAPI EnumPropsW(
    HWND hWnd,
    PROPENUMPROC lpEnumFunc);

extern WINBOOL
 PASCAL WINBASEAPI SetWindowTextW(
    HWND hWnd,
    LPCWSTR lpString);

extern int
 PASCAL WINBASEAPI GetWindowTextW(
    HWND hWnd,
    LPWSTR lpString,
    int nMaxCount);

extern int
 PASCAL WINBASEAPI GetWindowTextLengthW(
    HWND hWnd);

extern int
 PASCAL WINBASEAPI MessageBoxW(
    HWND hWnd ,
    LPCWSTR lpText,
    LPCWSTR lpCaption,
    UINT uType);

extern int
 PASCAL WINBASEAPI MessageBoxExW(
    HWND hWnd ,
    LPCWSTR lpText,
    LPCWSTR lpCaption,
    UINT uType,
    WORD wLanguageId);

extern int
 PASCAL WINBASEAPI MessageBoxIndirectW(LPMSGBOXPARAMS);

extern LONG
 PASCAL WINBASEAPI GetWindowLongW(
    HWND hWnd,
    int nIndex);

extern LONG
 PASCAL WINBASEAPI SetWindowLongW(
    HWND hWnd,
    int nIndex,
    LONG dwNewLong);

extern DWORD
 PASCAL WINBASEAPI GetClassLongW(
    HWND hWnd,
    int nIndex);

extern DWORD
 PASCAL WINBASEAPI SetClassLongW(
    HWND hWnd,
    int nIndex,
    LONG dwNewLong);

extern HWND
 PASCAL WINBASEAPI FindWindowW(
    LPCWSTR lpClassName ,
    LPCWSTR lpWindowName);

extern HWND
 PASCAL WINBASEAPI FindWindowExW(HWND, HWND, LPCWSTR, LPCWSTR);

extern int
 PASCAL WINBASEAPI GetClassNameW(
    HWND hWnd,
    LPWSTR lpClassName,
    int nMaxCount);

extern int
 PASCAL WINBASEAPI SetWindowsHookW(
    int nFilterType,
    HOOKPROC pfnFilterProc);

extern HHOOK
 PASCAL WINBASEAPI SetWindowsHookExW(
    int idHook,
    HOOKPROC lpfn,
    HINSTANCE hmod,
    DWORD dwThreadId);

extern HBITMAP
 PASCAL WINBASEAPI LoadBitmapW(
    HINSTANCE hInstance,
    LPCWSTR lpBitmapName);

extern HCURSOR
 PASCAL WINBASEAPI LoadCursorW(
    HINSTANCE hInstance,
    LPCWSTR lpCursorName);

extern HCURSOR
 PASCAL WINBASEAPI LoadCursorFromFileW(
    LPCWSTR    lpFileName);

extern HICON
 PASCAL WINBASEAPI LoadIconW(
    HINSTANCE hInstance,
    LPCWSTR lpIconName);

extern HANDLE
 PASCAL WINBASEAPI LoadImageW(
    HINSTANCE,
    LPCWSTR,
    UINT,
    int,
    int,
    UINT);

extern int
 PASCAL WINBASEAPI LoadStringW(
    HINSTANCE hInstance,
    UINT uID,
    LPWSTR lpBuffer,
    int nBufferMax);

extern WINBOOL
 PASCAL WINBASEAPI IsDialogMessageW(
    HWND hDlg,
    LPMSG lpMsg);

extern int
 PASCAL WINBASEAPI DlgDirListW(
    HWND hDlg,
    LPWSTR lpPathSpec,
    int nIDListBox,
    int nIDStaticPath,
    UINT uFileType);

extern WINBOOL
 PASCAL WINBASEAPI DlgDirSelectExW(
    HWND hDlg,
    LPWSTR lpString,
    int nCount,
    int nIDListBox);

extern int
 PASCAL WINBASEAPI DlgDirListComboBoxW(
    HWND hDlg,
    LPWSTR lpPathSpec,
    int nIDComboBox,
    int nIDStaticPath,
    UINT uFiletype);

extern WINBOOL
 PASCAL WINBASEAPI DlgDirSelectComboBoxExW(
    HWND hDlg,
    LPWSTR lpString,
    int nCount,
    int nIDComboBox);

extern LRESULT
 PASCAL WINBASEAPI DefFrameProcW(
    HWND hWnd,
    HWND hWndMDIClient ,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam);

extern LRESULT
 PASCAL WINBASEAPI DefMDIChildProcW(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam);

extern HWND
 PASCAL WINBASEAPI CreateMDIWindowW(
    LPWSTR lpClassName,
    LPWSTR lpWindowName,
    DWORD dwStyle,
    int X,
    int Y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HINSTANCE hInstance,
    LPARAM lParam
    );

extern WINBOOL
 PASCAL WINBASEAPI WinHelpW(
    HWND hWndMain,
    LPCWSTR lpszHelp,
    UINT uCommand,
    DWORD dwData
    );

extern LONG
 PASCAL WINBASEAPI ChangeDisplaySettingsW(
    LPDEVMODE lpDevMode,
    DWORD dwFlags);

extern WINBOOL
 PASCAL WINBASEAPI EnumDisplaySettingsW(
    LPCWSTR lpszDeviceName,
    DWORD iModeNum,
    LPDEVMODE lpDevMode);

extern WINBOOL
 PASCAL WINBASEAPI SystemParametersInfoW(
    UINT uiAction,
    UINT uiParam,
    PVOID pvParam,
    UINT fWinIni);

extern int
 PASCAL WINBASEAPI AddFontResourceW(LPCWSTR);

extern int
 PASCAL WINBASEAPI AddFontResourceExW(LPCWSTR, DWORD, PVOID);

extern HMETAFILE
 PASCAL WINBASEAPI CopyMetaFileW(HMETAFILE, LPCWSTR);

extern HFONT
 PASCAL WINBASEAPI CreateFontIndirectW(CONST LOGFONT *);

extern HFONT
 PASCAL WINBASEAPI CreateFontW(int, int, int, int, int, DWORD,
                             DWORD, DWORD, DWORD, DWORD, DWORD,
                             DWORD, DWORD, LPCWSTR);

extern HDC
 PASCAL WINBASEAPI CreateICW(LPCWSTR, LPCWSTR , LPCWSTR , CONST DEVMODE *);

extern HDC
 PASCAL WINBASEAPI CreateMetaFileW(LPCWSTR);

extern WINBOOL
 PASCAL WINBASEAPI CreateScalableFontResourceW(DWORD, LPCWSTR, LPCWSTR, LPCWSTR);

extern int
 PASCAL WINBASEAPI DeviceCapabilitiesW(LPCWSTR, LPCWSTR, WORD,
                                LPWSTR, CONST DEVMODE *);

extern int
 PASCAL WINBASEAPI EnumFontFamiliesExW(HDC, LPLOGFONT, FONTENUMEXPROC, LPARAM, DWORD);

extern int
 PASCAL WINBASEAPI EnumFontFamiliesW(HDC, LPCWSTR, FONTENUMPROC, LPARAM);

extern int
 PASCAL WINBASEAPI EnumFontsW(HDC, LPCWSTR,  OLDFONTENUMPROC, LPARAM);

extern WINBOOL
 PASCAL WINBASEAPI GetCharWidthW(HDC, UINT, UINT, LPINT);

extern WINBOOL
  PASCAL WINBASEAPI GetCharWidth32W(HDC, UINT, UINT, LPINT);

extern WINBOOL
 PASCAL WINBASEAPI GetCharWidthFloatW(HDC, UINT, UINT, PFLOAT);

extern WINBOOL
 PASCAL WINBASEAPI GetCharABCWidthsW(HDC, UINT, UINT, LPABC);

extern WINBOOL
 PASCAL WINBASEAPI GetCharABCWidthsFloatW(HDC, UINT, UINT, LPABCFLOAT);

extern DWORD
 PASCAL WINBASEAPI GetGlyphOutlineW(HDC, UINT, UINT, LPGLYPHMETRICS, DWORD, LPVOID, CONST MAT2 *);

extern HMETAFILE
 PASCAL WINBASEAPI GetMetaFileW(LPCWSTR);

extern UINT
 PASCAL WINBASEAPI GetOutlineTextMetricsW(HDC, UINT, LPOUTLINETEXTMETRIC);

extern WINBOOL
 PASCAL WINBASEAPI GetTextExtentPointW(
                    HDC,
                    LPCWSTR,
                    int,
                    LPSIZE
                    );

extern WINBOOL
 PASCAL WINBASEAPI GetTextExtentPoint32W(
                    HDC,
                    LPCWSTR,
                    int,
                    LPSIZE
                    );

extern WINBOOL
 PASCAL WINBASEAPI GetTextExtentExPointW(
                    HDC,
                    LPCWSTR,
                    int,
                    int,
                    LPINT,
                    LPINT,
                    LPSIZE
                    );

extern DWORD
 PASCAL WINBASEAPI GetCharacterPlacementW(HDC, LPCWSTR, int, int, LPGCP_RESULTS, DWORD);

extern HDC
 PASCAL WINBASEAPI ResetDCW(HDC, CONST DEVMODE *);

extern WINBOOL
 PASCAL WINBASEAPI RemoveFontResourceW(LPCWSTR);

extern BOOL 
 PASCAL WINBASEAPI RemoveFontResourceExW(LPCWSTR, DWORD, PVOID);

extern HENHMETAFILE
 PASCAL WINBASEAPI CopyEnhMetaFileW(HENHMETAFILE, LPCWSTR);

extern HDC
 PASCAL WINBASEAPI CreateEnhMetaFileW(HDC, LPCWSTR, CONST RECT *, LPCWSTR);

extern HENHMETAFILE
 PASCAL WINBASEAPI GetEnhMetaFileW(LPCWSTR);

extern UINT
 PASCAL WINBASEAPI GetEnhMetaFileDescriptionW(HENHMETAFILE, UINT, LPWSTR );

extern WINBOOL
 PASCAL WINBASEAPI GetTextMetricsW(HDC, LPTEXTMETRIC);

extern int
 PASCAL WINBASEAPI StartDocW(HDC, CONST DOCINFO *);

extern int
 PASCAL WINBASEAPI GetObjectW(HGDIOBJ, int, LPVOID);

extern WINBOOL
 PASCAL WINBASEAPI TextOutW(HDC, int, int, LPCWSTR, int);

extern WINBOOL
 PASCAL WINBASEAPI ExtTextOutW(HDC, int, int, UINT, CONST RECT *,LPCWSTR, UINT, CONST INT *);

extern WINBOOL
 PASCAL WINBASEAPI PolyTextOutW(HDC, CONST POLYTEXT *, int);

extern int
 PASCAL WINBASEAPI GetTextFaceW(HDC, int, LPWSTR);

extern DWORD
 PASCAL WINBASEAPI GetKerningPairsW(HDC, DWORD, LPKERNINGPAIR);

extern WINBOOL
 PASCAL WINBASEAPI GetLogColorSpaceW(HCOLORSPACE,LPLOGCOLORSPACE,DWORD);

extern HCOLORSPACE
 PASCAL WINBASEAPI CreateColorSpaceW(LPLOGCOLORSPACE);

extern WINBOOL
 PASCAL WINBASEAPI GetICMProfileW(HDC,DWORD,LPWSTR);

extern WINBOOL
 PASCAL WINBASEAPI SetICMProfileW(HDC,LPWSTR);

extern WINBOOL
 PASCAL WINBASEAPI UpdateICMRegKeyW(DWORD, DWORD, LPWSTR, UINT);

extern int
 PASCAL WINBASEAPI EnumICMProfilesW(HDC,ICMENUMPROC,LPARAM);

extern WINBOOL
 PASCAL WINBASEAPI CreateProcessW(
    LPCWSTR lpApplicationName,
    LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    WINBOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFO lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
    );

extern VOID
 PASCAL WINBASEAPI GetStartupInfoW(
    LPSTARTUPINFO lpStartupInfo
    );

extern HANDLE
 PASCAL WINBASEAPI FindFirstFileW(
    LPCWSTR lpFileName,
    LPWIN32_FIND_DATA lpFindFileData
    );

extern WINBOOL
 PASCAL WINBASEAPI FindNextFileW(
    HANDLE hFindFile,
    LPWIN32_FIND_DATA lpFindFileData
    );

extern WINBOOL
 PASCAL WINBASEAPI GetVersionExW(
    LPOSVERSIONINFO lpVersionInformation
    );

#define CreateWindowW(lpClassName, lpWindowName, dwStyle, x, y,\
nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)\
CreateWindowExW(0L, lpClassName, lpWindowName, dwStyle, x, y,\
nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)

#define CreateDialogW(hInstance, lpName, hWndParent, lpDialogFunc) \
CreateDialogParamW(hInstance, lpName, hWndParent, lpDialogFunc, 0L)

#define CreateDialogIndirectW(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
CreateDialogIndirectParamW(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0L)

#define DialogBoxW(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
DialogBoxParamW(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0L)

#define DialogBoxIndirectW(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
DialogBoxIndirectParamW(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0L)

extern HDC
 PASCAL WINBASEAPI CreateDCW(LPCWSTR, LPCWSTR , LPCWSTR , CONST DEVMODE *);

extern HFONT
 PASCAL WINBASEAPI CreateFontA(int, int, int, int, int, DWORD,
                             DWORD, DWORD, DWORD, DWORD, DWORD,
                             DWORD, DWORD, LPCSTR);

extern DWORD
 PASCAL WINBASEAPI VerInstallFileW(
        DWORD uFlags,
        LPWSTR szSrcFileName,
        LPWSTR szDestFileName,
        LPWSTR szSrcDir,
        LPWSTR szDestDir,
        LPWSTR szCurDir,
        LPWSTR szTmpFile,
        PUINT lpuTmpFileLen
        );

extern DWORD
 PASCAL WINBASEAPI GetFileVersionInfoSizeW(
        LPWSTR lptstrFilename,
        LPDWORD lpdwHandle
        );

extern WINBOOL
 PASCAL WINBASEAPI GetFileVersionInfoW(
        LPWSTR lptstrFilename,
        DWORD dwHandle,
        DWORD dwLen,
        LPVOID lpData
        );

extern DWORD
 PASCAL WINBASEAPI VerLanguageNameW(
        DWORD wLang,
        LPWSTR szLang,
        DWORD nSize
        );

extern WINBOOL
 PASCAL WINBASEAPI VerQueryValueW(
        const LPVOID pBlock,
        LPWSTR lpSubBlock,
        LPVOID * lplpBuffer,
        PUINT puLen
        );

extern DWORD
 PASCAL WINBASEAPI VerFindFileW(
        DWORD uFlags,
        LPWSTR szFileName,
        LPWSTR szWinDir,
        LPWSTR szAppDir,
        LPWSTR szCurDir,
        PUINT lpuCurDirLen,
        LPWSTR szDestDir,
        PUINT lpuDestDirLen
        );

extern LONG
 PASCAL WINBASEAPI RegSetValueExW(
    HKEY hKey,
    LPCWSTR lpValueName,
    DWORD Reserved,
    DWORD dwType,
    CONST BYTE* lpData,
    DWORD cbData
    );

extern LONG
 PASCAL WINBASEAPI RegUnLoadKeyW(
    HKEY    hKey,
    LPCWSTR lpSubKey
    );

extern WINBOOL
 PASCAL WINBASEAPI InitiateSystemShutdownW(
    LPWSTR lpMachineName,
    LPWSTR lpMessage,
    DWORD dwTimeout,
    WINBOOL bForceAppsClosed,
    WINBOOL bRebootAfterShutdown
    );

extern WINBOOL
 PASCAL WINBASEAPI AbortSystemShutdownW(
    LPWSTR lpMachineName
    );

extern LONG
 PASCAL WINBASEAPI RegRestoreKeyW(
    HKEY hKey,
    LPCWSTR lpFile,
    DWORD   dwFlags
    );

extern LONG
 PASCAL WINBASEAPI RegSaveKeyW(
    HKEY hKey,
    LPCWSTR lpFile,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );

extern LONG
 PASCAL WINBASEAPI RegSetValueW(
    HKEY hKey,
    LPCWSTR lpSubKey,
    DWORD dwType,
    LPCWSTR lpData,
    DWORD cbData
    );

extern LONG
 PASCAL WINBASEAPI RegQueryValueW(
    HKEY hKey,
    LPCWSTR lpSubKey,
    LPWSTR lpValue,
    PLONG   lpcbValue
    );

extern LONG
 PASCAL WINBASEAPI RegQueryMultipleValuesW(
    HKEY hKey,
    PVALENT val_list,
    DWORD num_vals,
    LPWSTR lpValueBuf,
    LPDWORD ldwTotsize
    );

extern LONG
 PASCAL WINBASEAPI RegQueryValueExW(
    HKEY hKey,
    LPCWSTR lpValueName,
    LPDWORD lpReserved,
    LPDWORD lpType,
    LPBYTE lpData,
    LPDWORD lpcbData
    );

extern LONG
 PASCAL WINBASEAPI RegReplaceKeyW(
    HKEY     hKey,
    LPCWSTR  lpSubKey,
    LPCWSTR  lpNewFile,
    LPCWSTR  lpOldFile
    );

extern LONG
 PASCAL WINBASEAPI RegConnectRegistryW(
    LPWSTR lpMachineName,
    HKEY hKey,
    PHKEY phkResult
    );

extern LONG
 PASCAL WINBASEAPI RegCreateKeyW(
    HKEY hKey,
    LPCWSTR lpSubKey,
    PHKEY phkResult
    );

extern LONG
 PASCAL WINBASEAPI RegCreateKeyExW(
    HKEY hKey,
    LPCWSTR lpSubKey,
    DWORD Reserved,
    LPWSTR lpClass,
    DWORD dwOptions,
    REGSAM samDesired,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    PHKEY phkResult,
    LPDWORD lpdwDisposition
    );

extern LONG
 PASCAL WINBASEAPI RegDeleteKeyW(
    HKEY hKey,
    LPCWSTR lpSubKey
    );

extern LONG
 PASCAL WINBASEAPI RegDeleteValueW(
    HKEY hKey,
    LPCWSTR lpValueName
    );

extern LONG
 PASCAL WINBASEAPI RegEnumKeyW(
    HKEY hKey,
    DWORD dwIndex,
    LPWSTR lpName,
    DWORD cbName
    );

extern LONG
 PASCAL WINBASEAPI RegEnumKeyExW(
    HKEY hKey,
    DWORD dwIndex,
    LPWSTR lpName,
    LPDWORD lpcbName,
    LPDWORD lpReserved,
    LPWSTR lpClass,
    LPDWORD lpcbClass,
    PFILETIME lpftLastWriteTime
    );

extern LONG
 PASCAL WINBASEAPI RegEnumValueW(
    HKEY hKey,
    DWORD dwIndex,
    LPWSTR lpValueName,
    LPDWORD lpcbValueName,
    LPDWORD lpReserved,
    LPDWORD lpType,
    LPBYTE lpData,
    LPDWORD lpcbData
    );

extern LONG
 PASCAL WINBASEAPI RegLoadKeyW(
    HKEY    hKey,
    LPCWSTR  lpSubKey,
    LPCWSTR  lpFile
    );

extern LONG
 PASCAL WINBASEAPI RegOpenKeyW(
    HKEY hKey,
    LPCWSTR lpSubKey,
    PHKEY phkResult
    );

extern LONG
 PASCAL WINBASEAPI RegOpenKeyExW(
    HKEY hKey,
    LPCWSTR lpSubKey,
    DWORD ulOptions,
    REGSAM samDesired,
    PHKEY phkResult
    );

extern LONG
 PASCAL WINBASEAPI RegQueryInfoKeyW(
    HKEY hKey,
    LPWSTR lpClass,
    LPDWORD lpcbClass,
    LPDWORD lpReserved,
    LPDWORD lpcSubKeys,
    LPDWORD lpcbMaxSubKeyLen,
    LPDWORD lpcbMaxClassLen,
    LPDWORD lpcValues,
    LPDWORD lpcbMaxValueNameLen,
    LPDWORD lpcbMaxValueLen,
    LPDWORD lpcbSecurityDescriptor,
    PFILETIME lpftLastWriteTime
    );

extern int
 PASCAL WINBASEAPI CompareStringW(
    LCID     Locale,
    DWORD    dwCmpFlags,
    LPCWSTR lpString1,
    int      cchCount1,
    LPCWSTR lpString2,
    int      cchCount2);

extern int
 PASCAL WINBASEAPI LCMapStringW(
    LCID     Locale,
    DWORD    dwMapFlags,
    LPCWSTR lpSrcStr,
    int      cchSrc,
    LPWSTR  lpDestStr,
    int      cchDest);


extern int
 PASCAL WINBASEAPI GetLocaleInfoW(
    LCID     Locale,
    LCTYPE   LCType,
    LPWSTR  lpLCData,
    int      cchData);

extern WINBOOL
 PASCAL WINBASEAPI SetLocaleInfoW(
    LCID     Locale,
    LCTYPE   LCType,
    LPCWSTR lpLCData);

extern int
 PASCAL WINBASEAPI GetTimeFormatW(
    LCID     Locale,
    DWORD    dwFlags,
    CONST SYSTEMTIME *lpTime,
    LPCWSTR lpFormat,
    LPWSTR  lpTimeStr,
    int      cchTime);

extern int
 PASCAL WINBASEAPI GetDateFormatW(
    LCID     Locale,
    DWORD    dwFlags,
    CONST SYSTEMTIME *lpDate,
    LPCWSTR lpFormat,
    LPWSTR  lpDateStr,
    int      cchDate);

extern int
 PASCAL WINBASEAPI GetNumberFormatW(
    LCID     Locale,
    DWORD    dwFlags,
    LPCWSTR lpValue,
    CONST NUMBERFMT *lpFormat,
    LPWSTR  lpNumberStr,
    int      cchNumber);

extern int
 PASCAL WINBASEAPI GetCurrencyFormatW(
    LCID     Locale,
    DWORD    dwFlags,
    LPCWSTR lpValue,
    CONST CURRENCYFMT *lpFormat,
    LPWSTR  lpCurrencyStr,
    int      cchCurrency);

extern WINBOOL
 PASCAL WINBASEAPI EnumCalendarInfoW(
    CALINFO_ENUMPROC lpCalInfoEnumProc,
    LCID              Locale,
    CALID             Calendar,
    CALTYPE           CalType);

extern WINBOOL
 PASCAL WINBASEAPI EnumTimeFormatsW(
    TIMEFMT_ENUMPROC lpTimeFmtEnumProc,
    LCID              Locale,
    DWORD             dwFlags);

extern WINBOOL
 PASCAL WINBASEAPI EnumDateFormatsW(
    DATEFMT_ENUMPROC lpDateFmtEnumProc,
    LCID              Locale,
    DWORD             dwFlags);

extern WINBOOL
 PASCAL WINBASEAPI GetStringTypeExW(
    LCID     Locale,
    DWORD    dwInfoType,
    LPCWSTR lpSrcStr,
    int      cchSrc,
    LPWORD   lpCharType);

extern WINBOOL
 PASCAL WINBASEAPI GetStringTypeW(
    DWORD    dwInfoType,
    LPCWSTR  lpSrcStr,
    int      cchSrc,
    LPWORD   lpCharType);

extern int
 PASCAL WINBASEAPI FoldStringW(
    DWORD    dwMapFlags,
    LPCWSTR lpSrcStr,
    int      cchSrc,
    LPWSTR  lpDestStr,
    int      cchDest);

extern WINBOOL
 PASCAL WINBASEAPI EnumSystemLocalesW(
    LOCALE_ENUMPROC lpLocaleEnumProc,
    DWORD            dwFlags);

extern WINBOOL
 PASCAL WINBASEAPI EnumSystemCodePagesW(
    CODEPAGE_ENUMPROC lpCodePageEnumProc,
    DWORD              dwFlags);

extern WINBOOL
 PASCAL WINBASEAPI PeekConsoleInputW(
    HANDLE hConsoleInput,
    PINPUT_RECORD lpBuffer,
    DWORD nLength,
    LPDWORD lpNumberOfEventsRead
    );

extern WINBOOL
 PASCAL WINBASEAPI ReadConsoleInputW(
    HANDLE hConsoleInput,
    PINPUT_RECORD lpBuffer,
    DWORD nLength,
    LPDWORD lpNumberOfEventsRead
    );

extern WINBOOL
 PASCAL WINBASEAPI WriteConsoleInputW(
    HANDLE hConsoleInput,
    CONST INPUT_RECORD *lpBuffer,
    DWORD nLength,
    LPDWORD lpNumberOfEventsWritten
    );

extern WINBOOL
 PASCAL WINBASEAPI ReadConsoleOutputW(
    HANDLE hConsoleOutput,
    PCHAR_INFO lpBuffer,
    COORD dwBufferSize,
    COORD dwBufferCoord,
    PSMALL_RECT lpReadRegion
    );

extern WINBOOL
 PASCAL WINBASEAPI WriteConsoleOutputW(
    HANDLE hConsoleOutput,
    CONST CHAR_INFO *lpBuffer,
    COORD dwBufferSize,
    COORD dwBufferCoord,
    PSMALL_RECT lpWriteRegion
    );

extern WINBOOL
 PASCAL WINBASEAPI ReadConsoleOutputCharacterW(
    HANDLE hConsoleOutput,
    LPWSTR lpCharacter,
    DWORD nLength,
    COORD dwReadCoord,
    LPDWORD lpNumberOfCharsRead
    );

extern WINBOOL
 PASCAL WINBASEAPI WriteConsoleOutputCharacterW(
    HANDLE hConsoleOutput,
    LPCWSTR lpCharacter,
    DWORD nLength,
    COORD dwWriteCoord,
    LPDWORD lpNumberOfCharsWritten
    );

extern WINBOOL
 PASCAL WINBASEAPI FillConsoleOutputCharacterW(
    HANDLE hConsoleOutput,
    WCHAR  cCharacter,
    DWORD  nLength,
    COORD  dwWriteCoord,
    LPDWORD lpNumberOfCharsWritten
    );

extern WINBOOL
 PASCAL WINBASEAPI ScrollConsoleScreenBufferW(
    HANDLE hConsoleOutput,
    CONST SMALL_RECT *lpScrollRectangle,
    CONST SMALL_RECT *lpClipRectangle,
    COORD dwDestinationOrigin,
    CONST CHAR_INFO *lpFill
    );

extern DWORD
 PASCAL WINBASEAPI GetConsoleTitleW(
    LPWSTR lpConsoleTitle,
    DWORD nSize
    );

extern WINBOOL
 PASCAL WINBASEAPI SetConsoleTitleW(
    LPCWSTR lpConsoleTitle
    );

extern WINBOOL
 PASCAL WINBASEAPI ReadConsoleW(
    HANDLE hConsoleInput,
    LPVOID lpBuffer,
    DWORD nNumberOfCharsToRead,
    LPDWORD lpNumberOfCharsRead,
    LPVOID lpReserved
    );

extern WINBOOL
 PASCAL WINBASEAPI WriteConsoleW(
    HANDLE hConsoleOutput,
    CONST VOID *lpBuffer,
    DWORD nNumberOfCharsToWrite,
    LPDWORD lpNumberOfCharsWritten,
    LPVOID lpReserved
    );

extern DWORD  PASCAL WINBASEAPI WNetAddConnectionW(
     LPCWSTR   lpRemoteName,
     LPCWSTR   lpPassword,
     LPCWSTR   lpLocalName
    );

extern DWORD  PASCAL WINBASEAPI WNetAddConnection2W(
     LPNETRESOURCE lpNetResource,
     LPCWSTR       lpPassword,
     LPCWSTR       lpUserName,
     DWORD          dwFlags
    );

extern DWORD  PASCAL WINBASEAPI WNetAddConnection3W(
     HWND           hwndOwner,
     LPNETRESOURCE lpNetResource,
     LPCWSTR       lpPassword,
     LPCWSTR       lpUserName,
     DWORD          dwFlags
    );

extern DWORD  PASCAL WINBASEAPI WNetCancelConnectionW(
     LPCWSTR lpName,
     WINBOOL     fForce
    );

extern DWORD  PASCAL WINBASEAPI WNetCancelConnection2W(
     LPCWSTR lpName,
     DWORD    dwFlags,
     WINBOOL     fForce
    );

extern DWORD  PASCAL WINBASEAPI WNetGetConnectionW(
     LPCWSTR lpLocalName,
     LPWSTR  lpRemoteName,
     LPDWORD  lpnLength
    );

extern DWORD  PASCAL WINBASEAPI WNetUseConnectionW(
    HWND            hwndOwner,
    LPNETRESOURCE  lpNetResource,
    LPCWSTR        lpUserID,
    LPCWSTR        lpPassword,
    DWORD           dwFlags,
    LPWSTR         lpAccessName,
    LPDWORD         lpBufferSize,
    LPDWORD         lpResult
    );

extern DWORD  PASCAL WINBASEAPI WNetSetConnectionW(
    LPCWSTR    lpName,
    DWORD       dwProperties,
    LPVOID      pvValues
    );

extern DWORD  PASCAL WINBASEAPI WNetConnectionDialog1W(
    LPCONNECTDLGSTRUCT lpConnDlgStruct
    );

extern DWORD  PASCAL WINBASEAPI WNetDisconnectDialog1W(
    LPDISCDLGSTRUCT lpConnDlgStruct
    );

extern DWORD  PASCAL WINBASEAPI WNetOpenEnumW(
     DWORD          dwScope,
     DWORD          dwType,
     DWORD          dwUsage,
     LPNETRESOURCE lpNetResource,
     LPHANDLE       lphEnum
    );

extern DWORD  PASCAL WINBASEAPI WNetEnumResourceW(
     HANDLE  hEnum,
     LPDWORD lpcCount,
     LPVOID  lpBuffer,
     LPDWORD lpBufferSize
    );

extern DWORD  PASCAL WINBASEAPI WNetGetUniversalNameW(
     LPCWSTR lpLocalPath,
     DWORD    dwInfoLevel,
     LPVOID   lpBuffer,
     LPDWORD  lpBufferSize
     );

extern DWORD  PASCAL WINBASEAPI WNetGetUserW(
     LPCWSTR  lpName,
     LPWSTR   lpUserName,
     LPDWORD   lpnLength
    );

extern DWORD  PASCAL WINBASEAPI WNetGetProviderNameW(
    DWORD   dwNetType,
    LPWSTR lpProviderName,
    LPDWORD lpBufferSize
    );

extern DWORD  PASCAL WINBASEAPI WNetGetNetworkInformationW(
    LPCWSTR          lpProvider,
    LPNETINFOSTRUCT   lpNetInfoStruct
    );

extern DWORD  PASCAL WINBASEAPI WNetGetLastErrorW(
     LPDWORD    lpError,
     LPWSTR    lpErrorBuf,
     DWORD      nErrorBufSize,
     LPWSTR    lpNameBuf,
     DWORD      nNameBufSize
    );

extern DWORD  PASCAL WINBASEAPI MultinetGetConnectionPerformanceW(
        LPNETRESOURCE lpNetResource,
        LPNETCONNECTINFOSTRUCT lpNetConnectInfoStruct
        );

extern WINBOOL
 PASCAL WINBASEAPI ChangeServiceConfigW(
    SC_HANDLE    hService,
    DWORD        dwServiceType,
    DWORD        dwStartType,
    DWORD        dwErrorControl,
    LPCWSTR     lpBinaryPathName,
    LPCWSTR     lpLoadOrderGroup,
    LPDWORD      lpdwTagId,
    LPCWSTR     lpDependencies,
    LPCWSTR     lpServiceStartName,
    LPCWSTR     lpPassword,
    LPCWSTR     lpDisplayName
    );

extern SC_HANDLE
 PASCAL WINBASEAPI CreateServiceW(
    SC_HANDLE    hSCManager,
    LPCWSTR     lpServiceName,
    LPCWSTR     lpDisplayName,
    DWORD        dwDesiredAccess,
    DWORD        dwServiceType,
    DWORD        dwStartType,
    DWORD        dwErrorControl,
    LPCWSTR     lpBinaryPathName,
    LPCWSTR     lpLoadOrderGroup,
    LPDWORD      lpdwTagId,
    LPCWSTR     lpDependencies,
    LPCWSTR     lpServiceStartName,
    LPCWSTR     lpPassword
    );

extern WINBOOL
 PASCAL WINBASEAPI EnumDependentServicesW(
    SC_HANDLE               hService,
    DWORD                   dwServiceState,
    LPENUM_SERVICE_STATUS  lpServices,
    DWORD                   cbBufSize,
    LPDWORD                 pcbBytesNeeded,
    LPDWORD                 lpServicesReturned
    );

extern WINBOOL
 PASCAL WINBASEAPI EnumServicesStatusW(
    SC_HANDLE               hSCManager,
    DWORD                   dwServiceType,
    DWORD                   dwServiceState,
    LPENUM_SERVICE_STATUS  lpServices,
    DWORD                   cbBufSize,
    LPDWORD                 pcbBytesNeeded,
    LPDWORD                 lpServicesReturned,
    LPDWORD                 lpResumeHandle
    );

extern WINBOOL
 PASCAL WINBASEAPI GetServiceKeyNameW(
    SC_HANDLE               hSCManager,
    LPCWSTR                lpDisplayName,
    LPWSTR                 lpServiceName,
    LPDWORD                 lpcchBuffer
    );

extern WINBOOL
 PASCAL WINBASEAPI GetServiceDisplayNameW(
    SC_HANDLE               hSCManager,
    LPCWSTR                lpServiceName,
    LPWSTR                 lpDisplayName,
    LPDWORD                 lpcchBuffer
    );

extern SC_HANDLE
 PASCAL WINBASEAPI OpenSCManagerW(
    LPCWSTR lpMachineName,
    LPCWSTR lpDatabaseName,
    DWORD   dwDesiredAccess
    );

extern SC_HANDLE
 PASCAL WINBASEAPI OpenServiceW(
    SC_HANDLE   hSCManager,
    LPCWSTR    lpServiceName,
    DWORD       dwDesiredAccess
    );

extern WINBOOL
 PASCAL WINBASEAPI QueryServiceConfigW(
    SC_HANDLE               hService,
    LPQUERY_SERVICE_CONFIG lpServiceConfig,
    DWORD                   cbBufSize,
    LPDWORD                 pcbBytesNeeded
    );

extern WINBOOL
 PASCAL WINBASEAPI QueryServiceLockStatusW(
    SC_HANDLE                       hSCManager,
    LPQUERY_SERVICE_LOCK_STATUS    lpLockStatus,
    DWORD                           cbBufSize,
    LPDWORD                         pcbBytesNeeded
    );

extern SERVICE_STATUS_HANDLE
 PASCAL WINBASEAPI RegisterServiceCtrlHandlerW(
    LPCWSTR             lpServiceName,
    LPHANDLER_FUNCTION   lpHandlerProc
    );

extern WINBOOL
 PASCAL WINBASEAPI StartServiceCtrlDispatcherW(
    LPSERVICE_TABLE_ENTRY    lpServiceStartTable
    );

extern WINBOOL
 PASCAL WINBASEAPI StartServiceW(
    SC_HANDLE            hService,
    DWORD                dwNumServiceArgs,
    LPCWSTR             *lpServiceArgVectors
    );

/* Extensions to OpenGL */

extern WINBOOL  PASCAL WINBASEAPI wglUseFontBitmapsW(HDC, DWORD, DWORD, DWORD);

extern WINBOOL  PASCAL WINBASEAPI wglUseFontOutlinesW(HDC, DWORD, DWORD, DWORD, FLOAT,
            FLOAT, int, LPGLYPHMETRICSFLOAT);

/* ------------------------------------- */
/* From shellapi.h in old Cygnus headers */

extern unsigned int  PASCAL WINBASEAPI DragQueryFileW(HDROP, unsigned int, LPCWSTR, unsigned int);

extern HICON  PASCAL WINBASEAPI ExtractAssociatedIconW(HINSTANCE, LPCWSTR, WORD *);

/* end of stuff from shellapi.h in old Cygnus headers */
/* -------------------------------------------------- */
/* From ddeml.h in old Cygnus headers */

extern HSZ  PASCAL WINBASEAPI DdeCreateStringHandleW(DWORD, LPCWSTR, int);

extern UINT  PASCAL WINBASEAPI DdeInitializeW(DWORD *, CALLB, DWORD, DWORD);

extern DWORD  PASCAL WINBASEAPI DdeQueryStringW(DWORD, HSZ, LPCWSTR, DWORD, int);

/* end of stuff from ddeml.h in old Cygnus headers */
/* ----------------------------------------------- */

extern WINBOOL  PASCAL WINBASEAPI LogonUserW(LPWSTR, LPWSTR, LPWSTR, DWORD, DWORD, HANDLE *);
extern WINBOOL  PASCAL WINBASEAPI CreateProcessAsUserW(HANDLE, LPCWSTR, LPWSTR,
            SECURITY_ATTRIBUTES*, SECURITY_ATTRIBUTES*, WINBOOL,
            DWORD, LPVOID, LPCWSTR, STARTUPINFO*,
            PROCESS_INFORMATION*);

extern WINBOOL  PASCAL WINBASEAPI PlaySoundW(LPCWSTR pszSound, HMODULE hmod, DWORD fdwSound);

extern WINBOOL  PASCAL WINBASEAPI GetCPInfoExW(UINT CodePage,DWORD dwFlags, LPCPINFOEX lpCPInfoEx);

extern WINBOOL PASCAL WINBASEAPI GetFileAttributesExW(
    LPWSTR lpFileName,
    GET_FILEEX_INFO_LEVELS fInfoLevelId,
    LPVOID lpFileInformation
    );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RC_INVOKED */

#endif /* _GNU_H_WINDOWS32_UNICODEFUNCTIONS */