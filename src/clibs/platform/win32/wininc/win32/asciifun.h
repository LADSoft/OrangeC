/* 
   ASCIIFunctions.h

   Declarations for all the Win32 ASCII Functions

   Copyright (C) 1996 Free Software Foundation, Inc.

   Author:  Scott Christley <scottc@net-community.com>

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

#ifndef _GNU_H_WINDOWS32_ASCIIFUNCTIONS
#define _GNU_H_WINDOWS32_ASCIIFUNCTIONS


/* This keeps all this stuff from bugging the resource compiler. */
#ifndef	RC_INVOKED

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern WINBOOL
 PASCAL WINBASEAPI GetBinaryTypeA(
	       LPCSTR lpApplicationName,
	       LPDWORD lpBinaryType
	       );

extern DWORD
 PASCAL WINBASEAPI GetShortPathNameA(
		  LPCSTR lpszLongPath,
		  LPSTR  lpszShortPath,
		  DWORD    cchBuffer
		  );

extern LPSTR
 PASCAL WINBASEAPI GetEnvironmentStringsA(
		       VOID
		       );

extern WINBOOL
 PASCAL WINBASEAPI FreeEnvironmentStringsA(
			LPSTR
			);

extern DWORD
 PASCAL WINBASEAPI FormatMessageA(
	       DWORD dwFlags,
	       LPCVOID lpSource,
	       DWORD dwMessageId,
	       DWORD dwLanguageId,
	       LPSTR lpBuffer,
	       DWORD nSize,
	       va_list *Arguments
	       );

extern HANDLE
 PASCAL WINBASEAPI CreateMailslotA(
		LPCSTR lpName,
		DWORD nMaxMessageSize,
		DWORD lReadTimeout,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes
		);

extern int
 PASCAL WINBASEAPI lstrcmpA(
	 LPCSTR lpString1,
	 LPCSTR lpString2
	 );

extern int
 PASCAL WINBASEAPI lstrcmpiA(
	  LPCSTR lpString1,
	  LPCSTR lpString2
	  );

extern LPSTR
 PASCAL WINBASEAPI lstrcpynA(
	  LPSTR lpString1,
	  LPCSTR lpString2,
	  int iMaxLength
	  );

extern LPSTR
 PASCAL WINBASEAPI lstrcpyA(
	 LPSTR lpString1,
	 LPCSTR lpString2
	 );

extern LPSTR
 PASCAL WINBASEAPI lstrcatA(
	 LPSTR lpString1,
	 LPCSTR lpString2
	 );

extern int
 PASCAL WINBASEAPI lstrlenA(
	 LPCSTR lpString
	 );

extern HANDLE
 PASCAL WINBASEAPI CreateMutexA(
	     LPSECURITY_ATTRIBUTES lpMutexAttributes,
	     WINBOOL bInitialOwner,
	     LPCSTR lpName
	     );

extern HANDLE
 PASCAL WINBASEAPI OpenMutexA(
	   DWORD dwDesiredAccess,
	   WINBOOL bInheritHandle,
	   LPCSTR lpName
	   );

extern HANDLE
 PASCAL WINBASEAPI CreateEventA(
	     LPSECURITY_ATTRIBUTES lpEventAttributes,
	     WINBOOL bManualReset,
	     WINBOOL bInitialState,
	     LPCSTR lpName
	     );

extern HANDLE
 PASCAL WINBASEAPI OpenEventA(
	   DWORD dwDesiredAccess,
	   WINBOOL bInheritHandle,
	   LPCSTR lpName
	   );

extern HANDLE
 PASCAL WINBASEAPI CreateSemaphoreA(
		 LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
		 LONG lInitialCount,
		 LONG lMaximumCount,
		 LPCSTR lpName
		 );

extern HANDLE
 PASCAL WINBASEAPI OpenSemaphoreA(
	       DWORD dwDesiredAccess,
	       WINBOOL bInheritHandle,
	       LPCSTR lpName
	       );

extern HANDLE
 PASCAL WINBASEAPI CreateFileMappingA(
		   HANDLE hFile,
		   LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
		   DWORD flProtect,
		   DWORD dwMaximumSizeHigh,
		   DWORD dwMaximumSizeLow,
		   LPCSTR lpName
		   );

extern HANDLE
 PASCAL WINBASEAPI OpenFileMappingA(
		 DWORD dwDesiredAccess,
		 WINBOOL bInheritHandle,
		 LPCSTR lpName
		 );

extern DWORD
 PASCAL WINBASEAPI GetLogicalDriveStringsA(
			DWORD nBufferLength,
			LPSTR lpBuffer
			);

extern HINSTANCE
 PASCAL WINBASEAPI LoadLibraryA(
	     LPCSTR lpLibFileName
	     );

extern HINSTANCE
 PASCAL WINBASEAPI LoadLibraryExA(
	       LPCSTR lpLibFileName,
	       HANDLE hFile,
	       DWORD dwFlags
	       );

extern DWORD
 PASCAL WINBASEAPI GetModuleFileNameA(
		   HINSTANCE hModule,
		   LPSTR lpFilename,
		   DWORD nSize
		   );

extern HMODULE
 PASCAL WINBASEAPI GetModuleHandleA(
		 LPCSTR lpModuleName
		 );

extern VOID
 PASCAL WINBASEAPI FatalAppExitA(
	      UINT uAction,
	      LPCSTR lpMessageText
	      );

extern LPSTR
 PASCAL WINBASEAPI GetCommandLineA(
		VOID
		);

extern DWORD
 PASCAL WINBASEAPI GetEnvironmentVariableA(
			LPCSTR lpName,
			LPSTR lpBuffer,
			DWORD nSize
			);

extern WINBOOL
 PASCAL WINBASEAPI SetEnvironmentVariableA(
			LPCSTR lpName,
			LPCSTR lpValue
			);

extern DWORD
 PASCAL WINBASEAPI ExpandEnvironmentStringsA(
			  LPCSTR lpSrc,
			  LPSTR lpDst,
			  DWORD nSize
			  );

extern VOID
 PASCAL WINBASEAPI OutputDebugStringA(
		   LPCSTR lpOutputString
		   );

extern HRSRC
 PASCAL WINBASEAPI FindResourceA(
	      HINSTANCE hModule,
	      LPCSTR lpName,
	      LPCSTR lpType
	      );

extern HRSRC
 PASCAL WINBASEAPI FindResourceExA(
		HINSTANCE hModule,
		LPCSTR lpType,
		LPCSTR lpName,
		WORD    wLanguage
		);

extern WINBOOL
 PASCAL WINBASEAPI EnumResourceTypesA(
		   HINSTANCE hModule,
		   ENUMRESTYPEPROC lpEnumFunc,
		   LONG lParam
		   );

extern WINBOOL
 PASCAL WINBASEAPI EnumResourceNamesA(
		   HINSTANCE hModule,
		   LPCSTR lpType,
		   ENUMRESNAMEPROC lpEnumFunc,
		   LONG lParam
		   );

extern WINBOOL
 PASCAL WINBASEAPI EnumResourceLanguagesA(
		       HINSTANCE hModule,
		       LPCSTR lpType,
		       LPCSTR lpName,
		       ENUMRESLANGPROC lpEnumFunc,
		       LONG lParam
		       );

extern HANDLE
 PASCAL WINBASEAPI BeginUpdateResourceA(
		     LPCSTR pFileName,
		     WINBOOL bDeleteExistingResources
		     );

extern WINBOOL
 PASCAL WINBASEAPI UpdateResourceA(
		HANDLE      hUpdate,
		LPCSTR     lpType,
		LPCSTR     lpName,
		WORD        wLanguage,
		LPVOID      lpData,
		DWORD       cbData
		);

extern WINBOOL
 PASCAL WINBASEAPI EndUpdateResourceA(
		   HANDLE      hUpdate,
		   WINBOOL        fDiscard
		   );

extern ATOM
 PASCAL WINBASEAPI GlobalAddAtomA(
	       LPCSTR lpString
	       );

extern ATOM
 PASCAL WINBASEAPI GlobalFindAtomA(
		LPCSTR lpString
		);

extern UINT
 PASCAL WINBASEAPI GlobalGetAtomNameA(
		   ATOM nAtom,
		   LPSTR lpBuffer,
		   int nSize
		   );

extern ATOM
 PASCAL WINBASEAPI AddAtomA(
	 LPCSTR lpString
	 );

extern ATOM
 PASCAL WINBASEAPI FindAtomA(
	  LPCSTR lpString
	  );

extern UINT
 PASCAL WINBASEAPI GetAtomNameA(
	     ATOM nAtom,
	     LPSTR lpBuffer,
	     int nSize
	     );

extern UINT
 PASCAL WINBASEAPI GetProfileIntA(
	       LPCSTR lpAppName,
	       LPCSTR lpKeyName,
	       INT nDefault
	       );

extern DWORD
 PASCAL WINBASEAPI GetProfileStringA(
		  LPCSTR lpAppName,
		  LPCSTR lpKeyName,
		  LPCSTR lpDefault,
		  LPSTR lpReturnedString,
		  DWORD nSize
		  );

extern WINBOOL
 PASCAL WINBASEAPI WriteProfileStringA(
    LPCSTR lpAppName,
    LPCSTR lpKeyName,
    LPCSTR lpString
    );

extern DWORD
 PASCAL WINBASEAPI GetProfileSectionA(
    LPCSTR lpAppName,
    LPSTR lpReturnedString,
    DWORD nSize
    );

extern WINBOOL
 PASCAL WINBASEAPI WriteProfileSectionA(
    LPCSTR lpAppName,
    LPCSTR lpString
    );

extern UINT
 PASCAL WINBASEAPI GetPrivateProfileIntA(
    LPCSTR lpAppName,
    LPCSTR lpKeyName,
    INT nDefault,
    LPCSTR lpFileName
    );

extern DWORD
 PASCAL WINBASEAPI GetPrivateProfileStringA(
    LPCSTR lpAppName,
    LPCSTR lpKeyName,
    LPCSTR lpDefault,
    LPSTR lpReturnedString,
    DWORD nSize,
    LPCSTR lpFileName
    );

extern WINBOOL
 PASCAL WINBASEAPI WritePrivateProfileStringA(
    LPCSTR lpAppName,
    LPCSTR lpKeyName,
    LPCSTR lpString,
    LPCSTR lpFileName
    );

extern DWORD
 PASCAL WINBASEAPI GetPrivateProfileSectionA(
    LPCSTR lpAppName,
    LPSTR lpReturnedString,
    DWORD nSize,
    LPCSTR lpFileName
    );

extern DWORD PASCAL WINBASEAPI GetPrivateProfileSectionNamesA(LPSTR,DWORD,LPCSTR);

extern WINBOOL
 PASCAL WINBASEAPI WritePrivateProfileSectionA(
    LPCSTR lpAppName,
    LPCSTR lpString,
    LPCSTR lpFileName
    );

extern UINT
 PASCAL WINBASEAPI GetDriveTypeA(
    LPCSTR lpRootPathName
    );

extern UINT
 PASCAL WINBASEAPI GetSystemDirectoryA(
    LPSTR lpBuffer,
    UINT uSize
    );

extern DWORD
 PASCAL WINBASEAPI GetTempPathA(
    DWORD nBufferLength,
    LPSTR lpBuffer
    );

extern UINT
 PASCAL WINBASEAPI GetTempFileNameA(
    LPCSTR lpPathName,
    LPCSTR lpPrefixString,
    UINT uUnique,
    LPSTR lpTempFileName
    );

extern UINT
 PASCAL WINBASEAPI GetWindowsDirectoryA(
    LPSTR lpBuffer,
    UINT uSize
    );

extern WINBOOL
 PASCAL WINBASEAPI SetCurrentDirectoryA(
    LPCSTR lpPathName
    );

extern DWORD
 PASCAL WINBASEAPI GetCurrentDirectoryA(
    DWORD nBufferLength,
    LPSTR lpBuffer
    );

extern WINBOOL
 PASCAL WINBASEAPI GetDiskFreeSpaceA(
    LPCSTR lpRootPathName,
    LPDWORD lpSectorsPerCluster,
    LPDWORD lpBytesPerSector,
    LPDWORD lpNumberOfFreeClusters,
    LPDWORD lpTotalNumberOfClusters
    );

extern WINBOOL
 PASCAL WINBASEAPI CreateDirectoryA(
    LPCSTR lpPathName,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );

extern WINBOOL
 PASCAL WINBASEAPI CreateDirectoryExA(
    LPCSTR lpTemplateDirectory,
    LPCSTR lpNewDirectory,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );

extern WINBOOL
 PASCAL WINBASEAPI RemoveDirectoryA(
    LPCSTR lpPathName
    );

extern DWORD
 PASCAL WINBASEAPI GetFullPathNameA(
    LPCSTR lpFileName,
    DWORD nBufferLength,
    LPSTR lpBuffer,
    LPSTR *lpFilePart
    );

extern WINBOOL
 PASCAL WINBASEAPI DefineDosDeviceA(
    DWORD dwFlags,
    LPCSTR lpDeviceName,
    LPCSTR lpTargetPath
    );

extern DWORD
 PASCAL WINBASEAPI QueryDosDeviceA(
    LPCSTR lpDeviceName,
    LPSTR lpTargetPath,
    DWORD ucchMax
    );

extern HANDLE
 PASCAL WINBASEAPI CreateFileA(
    LPCSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
    );

extern WINBOOL
 PASCAL WINBASEAPI SetFileAttributesA(
    LPCSTR lpFileName,
    DWORD dwFileAttributes
    );

extern DWORD
 PASCAL WINBASEAPI GetFileAttributesA(
    LPCSTR lpFileName
    );

extern DWORD
 PASCAL WINBASEAPI GetCompressedFileSizeA(
    LPCSTR lpFileName,
    LPDWORD lpFileSizeHigh
    );

extern WINBOOL
 PASCAL WINBASEAPI DeleteFileA(
    LPCSTR lpFileName
    );

extern DWORD
 PASCAL WINBASEAPI SearchPathA(
	    LPCSTR lpPath,
    LPCSTR lpFileName,
    LPCSTR lpExtension,
    DWORD nBufferLength,
    LPSTR lpBuffer,
    LPSTR *lpFilePart
    );

extern WINBOOL
 PASCAL WINBASEAPI CopyFileA(
    LPCSTR lpExistingFileName,
    LPCSTR lpNewFileName,
    WINBOOL bFailIfExists
    );

extern WINBOOL
 PASCAL WINBASEAPI MoveFileA(
    LPCSTR lpExistingFileName,
    LPCSTR lpNewFileName
    );

extern WINBOOL
 PASCAL WINBASEAPI MoveFileExA(
    LPCSTR lpExistingFileName,
    LPCSTR lpNewFileName,
    DWORD dwFlags
    );

extern HANDLE
 PASCAL WINBASEAPI CreateNamedPipeA(
    LPCSTR lpName,
    DWORD dwOpenMode,
    DWORD dwPipeMode,
    DWORD nMaxInstances,
    DWORD nOutBufferSize,
    DWORD nInBufferSize,
    DWORD nDefaultTimeOut,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );

extern WINBOOL
 PASCAL WINBASEAPI GetNamedPipeHandleStateA(
    HANDLE hNamedPipe,
    LPDWORD lpState,
    LPDWORD lpCurInstances,
    LPDWORD lpMaxCollectionCount,
    LPDWORD lpCollectDataTimeout,
    LPSTR lpUserName,
    DWORD nMaxUserNameSize
    );

extern WINBOOL
 PASCAL WINBASEAPI CallNamedPipeA(
    LPCSTR lpNamedPipeName,
    LPVOID lpInBuffer,
    DWORD nInBufferSize,
    LPVOID lpOutBuffer,
    DWORD nOutBufferSize,
    LPDWORD lpBytesRead,
    DWORD nTimeOut
    );

extern WINBOOL
 PASCAL WINBASEAPI WaitNamedPipeA(
    LPCSTR lpNamedPipeName,
    DWORD nTimeOut
    );

extern WINBOOL
 PASCAL WINBASEAPI SetVolumeLabelA(
    LPCSTR lpRootPathName,
    LPCSTR lpVolumeName
    );

extern WINBOOL
 PASCAL WINBASEAPI GetVolumeInformationA(
    LPCSTR lpRootPathName,
    LPSTR lpVolumeNameBuffer,
    DWORD nVolumeNameSize,
    LPDWORD lpVolumeSerialNumber,
    LPDWORD lpMaximumComponentLength,
    LPDWORD lpFileSystemFlags,
    LPSTR lpFileSystemNameBuffer,
    DWORD nFileSystemNameSize
    );

extern WINBOOL
 PASCAL WINBASEAPI ClearEventLogA(
    HANDLE hEventLog,
    LPCSTR lpBackupFileName
    );

extern WINBOOL
 PASCAL WINBASEAPI BackupEventLogA(
    HANDLE hEventLog,
    LPCSTR lpBackupFileName
    );

extern HANDLE
 PASCAL WINBASEAPI OpenEventLogA(
    LPCSTR lpUNCServerName,
    LPCSTR lpSourceName
    );

extern HANDLE
 PASCAL WINBASEAPI RegisterEventSourceA(
    LPCSTR lpUNCServerName,
    LPCSTR lpSourceName
    );

extern HANDLE
 PASCAL WINBASEAPI OpenBackupEventLogA(
    LPCSTR lpUNCServerName,
    LPCSTR lpFileName
    );

extern WINBOOL
 PASCAL WINBASEAPI ReadEventLogA(
     HANDLE     hEventLog,
     DWORD      dwReadFlags,
     DWORD      dwRecordOffset,
     LPVOID     lpBuffer,
     DWORD      nNumberOfBytesToRead,
     DWORD      *pnBytesRead,
     DWORD      *pnMinNumberOfBytesNeeded
    );

extern WINBOOL
 PASCAL WINBASEAPI ReportEventA(
     HANDLE     hEventLog,
     WORD       wType,
     WORD       wCategory,
     DWORD      dwEventID,
     PSID       lpUserSid,
     WORD       wNumStrings,
     DWORD      dwDataSize,
     LPCSTR   *lpStrings,
     LPVOID     lpRawData
    );

extern WINBOOL
 PASCAL WINBASEAPI AccessCheckAndAuditAlarmA(
    LPCSTR SubsystemName,
    LPVOID HandleId,
    LPSTR ObjectTypeName,
    LPSTR ObjectName,
    PSECURITY_DESCRIPTOR SecurityDescriptor,
    DWORD DesiredAccess,
    PGENERIC_MAPPING GenericMapping,
    WINBOOL ObjectCreation,
    LPDWORD GrantedAccess,
    LPBOOL AccessStatus,
    LPBOOL pfGenerateOnClose
    );

extern WINBOOL
 PASCAL WINBASEAPI ObjectOpenAuditAlarmA(
    LPCSTR SubsystemName,
    LPVOID HandleId,
    LPSTR ObjectTypeName,
    LPSTR ObjectName,
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
 PASCAL WINBASEAPI ObjectPrivilegeAuditAlarmA(
    LPCSTR SubsystemName,
    LPVOID HandleId,
    HANDLE ClientToken,
    DWORD DesiredAccess,
    PPRIVILEGE_SET Privileges,
    WINBOOL AccessGranted
    );

extern WINBOOL
 PASCAL WINBASEAPI ObjectCloseAuditAlarmA(
    LPCSTR SubsystemName,
    LPVOID HandleId,
    WINBOOL GenerateOnClose
    );

extern WINBOOL
 PASCAL WINBASEAPI PrivilegedServiceAuditAlarmA(
    LPCSTR SubsystemName,
    LPCSTR ServiceName,
    HANDLE ClientToken,
    PPRIVILEGE_SET Privileges,
    WINBOOL AccessGranted
    );

extern WINBOOL
 PASCAL WINBASEAPI SetFileSecurityA(
    LPCSTR lpFileName,
    SECURITY_INFORMATION SecurityInformation,
    PSECURITY_DESCRIPTOR pSecurityDescriptor
    );

extern WINBOOL
 PASCAL WINBASEAPI GetFileSecurityA(
    LPCSTR lpFileName,
    SECURITY_INFORMATION RequestedInformation,
    PSECURITY_DESCRIPTOR pSecurityDescriptor,
    DWORD nLength,
    LPDWORD lpnLengthNeeded
    );

extern HANDLE
 PASCAL WINBASEAPI FindFirstChangeNotificationA(
    LPCSTR lpPathName,
    WINBOOL bWatchSubtree,
    DWORD dwNotifyFilter
    );

extern WINBOOL
 PASCAL WINBASEAPI IsBadStringPtrA(
    LPCSTR lpsz,
    UINT ucchMax
    );

extern WINBOOL
 PASCAL WINBASEAPI LookupAccountSidA(
    LPCSTR lpSystemName,
    PSID Sid,
    LPSTR Name,
    LPDWORD cbName,
    LPSTR ReferencedDomainName,
    LPDWORD cbReferencedDomainName,
    PSID_NAME_USE peUse
    );

extern WINBOOL
 PASCAL WINBASEAPI LookupAccountNameA(
    LPCSTR lpSystemName,
    LPCSTR lpAccountName,
    PSID Sid,
    LPDWORD cbSid,
    LPSTR ReferencedDomainName,
    LPDWORD cbReferencedDomainName,
    PSID_NAME_USE peUse
    );

extern WINBOOL
 PASCAL WINBASEAPI LookupPrivilegeValueA(
    LPCSTR lpSystemName,
    LPCSTR lpName,
    PLUID   lpLuid
    );

extern WINBOOL
 PASCAL WINBASEAPI LookupPrivilegeNameA(
    LPCSTR lpSystemName,
    PLUID   lpLuid,
    LPSTR lpName,
    LPDWORD cbName
    );

extern WINBOOL
 PASCAL WINBASEAPI LookupPrivilegeDisplayNameA(
    LPCSTR lpSystemName,
    LPCSTR lpName,
    LPSTR lpDisplayName,
    LPDWORD cbDisplayName,
    LPDWORD lpLanguageId
    );

extern WINBOOL
 PASCAL WINBASEAPI BuildCommDCBA(
    LPCSTR lpDef,
    LPDCB lpDCB
    );

extern WINBOOL
 PASCAL WINBASEAPI BuildCommDCBAndTimeoutsA(
    LPCSTR lpDef,
    LPDCB lpDCB,
    LPCOMMTIMEOUTS lpCommTimeouts
    );

extern WINBOOL
 PASCAL WINBASEAPI CommConfigDialogA(
    LPCSTR lpszName,
    HWND hWnd,
    LPCOMMCONFIG lpCC
    );

extern WINBOOL
 PASCAL WINBASEAPI GetDefaultCommConfigA(
    LPCSTR lpszName,
    LPCOMMCONFIG lpCC,
    LPDWORD lpdwSize
    );

extern WINBOOL
 PASCAL WINBASEAPI SetDefaultCommConfigA(
    LPCSTR lpszName,
    LPCOMMCONFIG lpCC,
    DWORD dwSize
    );

extern WINBOOL
 PASCAL WINBASEAPI GetComputerNameA(
    LPSTR lpBuffer,
    LPDWORD nSize
    );

extern WINBOOL
 PASCAL WINBASEAPI SetComputerNameA(
    LPCSTR lpComputerName
    );

extern WINBOOL
 PASCAL WINBASEAPI GetUserNameA(
    LPSTR lpBuffer,
    LPDWORD nSize
    );

extern int
 PASCAL WINBASEAPI wvsprintfA(
    LPSTR,
    LPCSTR,
    va_list arglist);
 
extern int
 WINBASEAPI wsprintfA(LPSTR, LPCSTR, ...);

extern HKL
 PASCAL WINBASEAPI LoadKeyboardLayoutA(
    LPCSTR pwszKLID,
    UINT Flags);
 
extern WINBOOL
 PASCAL WINBASEAPI GetKeyboardLayoutNameA(
    LPSTR pwszKLID);
 
extern HDESK
 PASCAL WINBASEAPI CreateDesktopA(
    LPSTR lpszDesktop,
    LPSTR lpszDevice,
    LPDEVMODE pDevmode,
    DWORD dwFlags,
    DWORD dwDesiredAccess,
    LPSECURITY_ATTRIBUTES lpsa);
 
extern HDESK
 PASCAL WINBASEAPI OpenDesktopA(
    LPSTR lpszDesktop,
    DWORD dwFlags,
    WINBOOL fInherit,
    DWORD dwDesiredAccess);
 
extern WINBOOL
 PASCAL WINBASEAPI EnumDesktopsA(
    HWINSTA hwinsta,
    DESKTOPENUMPROC lpEnumFunc,
    LPARAM lParam);
 
extern HWINSTA
 PASCAL WINBASEAPI CreateWindowStationA(
    LPSTR lpwinsta,
    DWORD dwReserved,
    DWORD dwDesiredAccess,
    LPSECURITY_ATTRIBUTES lpsa);
 
extern HWINSTA
 PASCAL WINBASEAPI OpenWindowStationA(
    LPSTR lpszWinSta,
    WINBOOL fInherit,
    DWORD dwDesiredAccess);
 
extern WINBOOL
 PASCAL WINBASEAPI EnumWindowStationsA(
    ENUMWINDOWSTATIONPROC lpEnumFunc,
    LPARAM lParam);
 
extern WINBOOL
 PASCAL WINBASEAPI GetUserObjectInformationA(
    HANDLE hObj,
    int nIndex,
    PVOID pvInfo,
    DWORD nLength,
    LPDWORD lpnLengthNeeded);
 
extern WINBOOL
 PASCAL WINBASEAPI SetUserObjectInformationA(
    HANDLE hObj,
    int nIndex,
    PVOID pvInfo,
    DWORD nLength);
 
extern UINT
 PASCAL WINBASEAPI RegisterWindowMessageA(
    LPCSTR lpString);
 
extern WINBOOL
 PASCAL WINBASEAPI GetMessageA(
    LPMSG lpMsg,
    HWND hWnd ,
    UINT wMsgFilterMin,
    UINT wMsgFilterMax);
 
extern LONG
 PASCAL WINBASEAPI DispatchMessageA(
    CONST MSG *lpMsg);
 
extern WINBOOL
 PASCAL WINBASEAPI PeekMessageA(
    LPMSG lpMsg,
    HWND hWnd ,
    UINT wMsgFilterMin,
    UINT wMsgFilterMax,
    UINT wRemoveMsg);
 
extern LRESULT
 PASCAL WINBASEAPI SendMessageA(
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam);
 
extern LRESULT
 PASCAL WINBASEAPI SendMessageTimeoutA(
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam,
    UINT fuFlags,
    UINT uTimeout,
    LPDWORD lpdwResult);
 
extern WINBOOL
 PASCAL WINBASEAPI SendNotifyMessageA(
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam);
 
extern WINBOOL
 PASCAL WINBASEAPI SendMessageCallbackA(
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam,
    SENDASYNCPROC lpResultCallBack,
    DWORD dwData);
 
extern WINBOOL
 PASCAL WINBASEAPI PostMessageA(
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam);
 
extern WINBOOL
 PASCAL WINBASEAPI PostThreadMessageA(
    DWORD idThread,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam);
 
extern LRESULT
 PASCAL WINBASEAPI DefWindowProcA(
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam);
 
extern LRESULT
 PASCAL WINBASEAPI CallWindowProcA(
    WNDPROC lpPrevWndFunc,
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam);
 
extern ATOM
 PASCAL WINBASEAPI RegisterClassA(
    CONST WNDCLASS *lpWndClass);
 
extern WINBOOL
 PASCAL WINBASEAPI UnregisterClassA(
    LPCSTR lpClassName,
    HINSTANCE hInstance);
 
extern WINBOOL
 PASCAL WINBASEAPI GetClassInfoA(
    HINSTANCE hInstance ,
    LPCSTR lpClassName,
    LPWNDCLASS lpWndClass);
 
extern ATOM
 PASCAL WINBASEAPI RegisterClassExA(CONST WNDCLASSEX *);
 
extern WINBOOL
 PASCAL WINBASEAPI GetClassInfoExA(HINSTANCE, LPCSTR, LPWNDCLASSEX);
 
extern HWND
 PASCAL WINBASEAPI CreateWindowExA(
    DWORD dwExStyle,
    LPCSTR lpClassName,
    LPCSTR lpWindowName,
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
 PASCAL WINBASEAPI CreateDialogParamA(
    HINSTANCE hInstance,
    LPCSTR lpTemplateName,
    HWND hWndParent ,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam);
 
extern HWND
 PASCAL WINBASEAPI CreateDialogIndirectParamA(
    HINSTANCE hInstance,
    LPCDLGTEMPLATE lpTemplate,
    HWND hWndParent,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam);
 
extern int
 PASCAL WINBASEAPI DialogBoxParamA(
    HINSTANCE hInstance,
    LPCSTR lpTemplateName,
    HWND hWndParent ,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam);
 
extern int
 PASCAL WINBASEAPI DialogBoxIndirectParamA(
    HINSTANCE hInstance,
    LPCDLGTEMPLATE hDialogTemplate,
    HWND hWndParent ,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam);
 
extern WINBOOL
 PASCAL WINBASEAPI SetDlgItemTextA(
    HWND hDlg,
    int nIDDlgItem,
    LPCSTR lpString);
 
extern UINT
 PASCAL WINBASEAPI GetDlgItemTextA(
    HWND hDlg,
    int nIDDlgItem,
    LPSTR lpString,
    int nMaxCount);
 
extern LONG
 PASCAL WINBASEAPI SendDlgItemMessageA(
    HWND hDlg,
    int nIDDlgItem,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam);
 
extern LRESULT
 PASCAL WINBASEAPI DefDlgProcA(
    HWND hDlg,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam);
 
extern WINBOOL
 PASCAL WINBASEAPI CallMsgFilterA(
    LPMSG lpMsg,
    int nCode);
 
extern UINT
 PASCAL WINBASEAPI RegisterClipboardFormatA(
    LPCSTR lpszFormat);
 
extern int
 PASCAL WINBASEAPI GetClipboardFormatNameA(
    UINT format,
    LPSTR lpszFormatName,
    int cchMaxCount);
 
extern WINBOOL
 PASCAL WINBASEAPI CharToOemA(
    LPCSTR lpszSrc,
    LPSTR lpszDst);
 
extern WINBOOL
 PASCAL WINBASEAPI OemToCharA(
    LPCSTR lpszSrc,
    LPSTR lpszDst);
 
extern WINBOOL
 PASCAL WINBASEAPI CharToOemBuffA(
    LPCSTR lpszSrc,
    LPSTR lpszDst,
    DWORD cchDstLength);
 
extern WINBOOL
 PASCAL WINBASEAPI OemToCharBuffA(
    LPCSTR lpszSrc,
    LPSTR lpszDst,
    DWORD cchDstLength);
 
extern LPSTR
 PASCAL WINBASEAPI CharUpperA(
    LPSTR lpsz);
 
extern DWORD
 PASCAL WINBASEAPI CharUpperBuffA(
    LPSTR lpsz,
    DWORD cchLength);
 
extern LPSTR
 PASCAL WINBASEAPI CharLowerA(
    LPSTR lpsz);
 
extern DWORD
 PASCAL WINBASEAPI CharLowerBuffA(
    LPSTR lpsz,
    DWORD cchLength);
 
extern LPSTR
 PASCAL WINBASEAPI CharNextA(
    LPCSTR lpsz);
 
extern LPSTR
 PASCAL WINBASEAPI CharPrevA(
    LPCSTR lpszStart,
    LPCSTR lpszCurrent);
 
extern WINBOOL
 PASCAL WINBASEAPI IsCharAlphaA(
    CHAR ch);
 
extern WINBOOL
 PASCAL WINBASEAPI IsCharAlphaNumericA(
    CHAR ch);
 
extern WINBOOL
 PASCAL WINBASEAPI IsCharUpperA(
    CHAR ch);
 
extern WINBOOL
 PASCAL WINBASEAPI IsCharLowerA(
    CHAR ch);
 
extern int
 PASCAL WINBASEAPI GetKeyNameTextA(
    LONG lParam,
    LPSTR lpString,
    int nSize
    );
 
extern SHORT
 PASCAL WINBASEAPI VkKeyScanA(
    CHAR ch);
 
extern SHORT
 PASCAL WINBASEAPI VkKeyScanExA(
    CHAR  ch,
    HKL   dwhkl);
 
extern UINT
 PASCAL WINBASEAPI MapVirtualKeyA(
    UINT uCode,
    UINT uMapType);
 
extern UINT
 PASCAL WINBASEAPI MapVirtualKeyExA(
    UINT uCode,
    UINT uMapType,
    HKL dwhkl);
 
extern HACCEL
 PASCAL WINBASEAPI LoadAcceleratorsA(
    HINSTANCE hInstance,
    LPCSTR lpTableName);
 
extern HACCEL
 PASCAL WINBASEAPI CreateAcceleratorTableA(
    LPACCEL, int);
 
extern int
 PASCAL WINBASEAPI CopyAcceleratorTableA(
    HACCEL hAccelSrc,
    LPACCEL lpAccelDst,
    int cAccelEntries);
 
extern int
 PASCAL WINBASEAPI TranslateAcceleratorA(
    HWND hWnd,
    HACCEL hAccTable,
    LPMSG lpMsg);
 
extern HMENU
 PASCAL WINBASEAPI LoadMenuA(
    HINSTANCE hInstance,
    LPCSTR lpMenuName);
 
extern HMENU
 PASCAL WINBASEAPI LoadMenuIndirectA(
    CONST MENUTEMPLATE *lpMenuTemplate);
 
extern WINBOOL
 PASCAL WINBASEAPI ChangeMenuA(
    HMENU hMenu,
    UINT cmd,
    LPCSTR lpszNewItem,
    UINT cmdInsert,
    UINT flags);
 
extern int
 PASCAL WINBASEAPI GetMenuStringA(
    HMENU hMenu,
    UINT uIDItem,
    LPSTR lpString,
    int nMaxCount,
    UINT uFlag);
 
extern WINBOOL
 PASCAL WINBASEAPI InsertMenuA(
    HMENU hMenu,
    UINT uPosition,
    UINT uFlags,
    UINT uIDNewItem,
    LPCSTR lpNewItem
    );
 
extern WINBOOL
 PASCAL WINBASEAPI AppendMenuA(
    HMENU hMenu,
    UINT uFlags,
    UINT uIDNewItem,
    LPCSTR lpNewItem
    );
 
extern WINBOOL
 PASCAL WINBASEAPI ModifyMenuA(
    HMENU hMnu,
    UINT uPosition,
    UINT uFlags,
    UINT uIDNewItem,
    LPCSTR lpNewItem
    );
 
extern WINBOOL
 PASCAL WINBASEAPI InsertMenuItemA(
    HMENU,
    UINT,
    WINBOOL,
    LPCMENUITEMINFO
    );
 
extern WINBOOL
 PASCAL WINBASEAPI GetMenuItemInfoA(
    HMENU,
    UINT,
    WINBOOL,
    LPMENUITEMINFO
    );
 
extern WINBOOL
 PASCAL WINBASEAPI SetMenuItemInfoA(
    HMENU,
    UINT,
    WINBOOL,
    LPCMENUITEMINFO
    );
 
extern int
 PASCAL WINBASEAPI DrawTextA(
    HDC hDC,
    LPCSTR lpString,
    int nCount,
    LPRECT lpRect,
    UINT uFormat);
 
extern int
 PASCAL WINBASEAPI DrawTextExA(HDC, LPSTR, int, LPRECT, UINT, LPDRAWTEXTPARAMS);
 
extern WINBOOL
 PASCAL WINBASEAPI GrayStringA(
    HDC hDC,
    HBRUSH hBrush,
    GRAYSTRINGPROC lpOutputFunc,
    LPARAM lpData,
    int nCount,
    int X,
    int Y,
    int nWidth,
    int nHeight);
 
extern WINBOOL
 PASCAL WINBASEAPI DrawStateA(HDC, HBRUSH, DRAWSTATEPROC, LPARAM, WPARAM, int, int, int, int, UINT);

extern LONG
 PASCAL WINBASEAPI TabbedTextOutA(
    HDC hDC,
    int X,
    int Y,
    LPCSTR lpString,
    int nCount,
    int nTabPositions,
    LPINT lpnTabStopPositions,
    int nTabOrigin);
 
extern DWORD
 PASCAL WINBASEAPI GetTabbedTextExtentA(
    HDC hDC,
    LPCSTR lpString,
    int nCount,
    int nTabPositions,
    LPINT lpnTabStopPositions);
 
extern WINBOOL
 PASCAL WINBASEAPI SetPropA(
    HWND hWnd,
    LPCSTR lpString,
    HANDLE hData);
 
extern HANDLE
 PASCAL WINBASEAPI GetPropA(
    HWND hWnd,
    LPCSTR lpString);
 
extern HANDLE
 PASCAL WINBASEAPI RemovePropA(
    HWND hWnd,
    LPCSTR lpString);
 
extern int
 PASCAL WINBASEAPI EnumPropsExA(
    HWND hWnd,
    PROPENUMPROCEX lpEnumFunc,
    LPARAM lParam);
 
extern int
 PASCAL WINBASEAPI EnumPropsA(
    HWND hWnd,
    PROPENUMPROC lpEnumFunc);
 
extern WINBOOL
 PASCAL WINBASEAPI SetWindowTextA(
    HWND hWnd,
    LPCSTR lpString);
 
extern int
 PASCAL WINBASEAPI GetWindowTextA(
    HWND hWnd,
    LPSTR lpString,
    int nMaxCount);
 
extern int
 PASCAL WINBASEAPI GetWindowTextLengthA(
    HWND hWnd);
 
extern int
PASCAL WINBASEAPI MessageBoxA(
    HWND hWnd ,
    LPCSTR lpText,
    LPCSTR lpCaption,
    UINT uType);
 
extern int
 PASCAL WINBASEAPI MessageBoxExA(
    HWND hWnd ,
    LPCSTR lpText,
    LPCSTR lpCaption,
    UINT uType,
    WORD wLanguageId);
 
extern int
 PASCAL WINBASEAPI MessageBoxIndirectA(LPMSGBOXPARAMS);

extern LONG
 PASCAL WINBASEAPI GetWindowLongA(
    HWND hWnd,
    int nIndex);
 
extern LONG
 PASCAL WINBASEAPI SetWindowLongA(
    HWND hWnd,
    int nIndex,
    LONG dwNewLong);
 
extern DWORD
 PASCAL WINBASEAPI GetClassLongA(
    HWND hWnd,
    int nIndex);
 
extern DWORD
 PASCAL WINBASEAPI SetClassLongA(
    HWND hWnd,
    int nIndex,
    LONG dwNewLong);
 
extern HWND
 PASCAL WINBASEAPI FindWindowA(
    LPCSTR lpClassName ,
    LPCSTR lpWindowName);
 
extern HWND
 PASCAL WINBASEAPI FindWindowExA(HWND, HWND, LPCSTR, LPCSTR);

extern int
 PASCAL WINBASEAPI GetClassNameA(
    HWND hWnd,
    LPSTR lpClassName,
    int nMaxCount);

extern HOOKPROC 
 PASCAL WINBASEAPI SetWindowsHookA(
    int nFilterType,
    HOOKPROC pfnFilterProc);

extern HHOOK
 PASCAL WINBASEAPI SetWindowsHookExA(
    int idHook,
    HOOKPROC lpfn,
    HINSTANCE hmod,
    DWORD dwThreadId);
 
extern HBITMAP
 PASCAL WINBASEAPI LoadBitmapA(
    HINSTANCE hInstance,
    LPCSTR lpBitmapName);
 
extern HCURSOR
 PASCAL WINBASEAPI LoadCursorA(
    HINSTANCE hInstance,
    LPCSTR lpCursorName);
 
extern HCURSOR
 PASCAL WINBASEAPI LoadCursorFromFileA(
    LPCSTR    lpFileName);
 
extern HICON
 PASCAL WINBASEAPI LoadIconA(
    HINSTANCE hInstance,
    LPCSTR lpIconName);
 
extern HANDLE
 PASCAL WINBASEAPI LoadImageA(
    HINSTANCE,
    LPCSTR,
    UINT,
    int,
    int,
    UINT);
 
extern int
 PASCAL WINBASEAPI LoadStringA(
    HINSTANCE hInstance,
    UINT uID,
    LPSTR lpBuffer,
    int nBufferMax);
 
extern WINBOOL
 PASCAL WINBASEAPI IsDialogMessageA(
    HWND hDlg,
    LPMSG lpMsg);
 
extern int
 PASCAL WINBASEAPI DlgDirListA(
    HWND hDlg,
    LPSTR lpPathSpec,
    int nIDListBox,
    int nIDStaticPath,
    UINT uFileType);
 
extern WINBOOL
 PASCAL WINBASEAPI DlgDirSelectExA(
    HWND hDlg,
    LPSTR lpString,
    int nCount,
    int nIDListBox);
 
extern int
 PASCAL WINBASEAPI DlgDirListComboBoxA(
    HWND hDlg,
    LPSTR lpPathSpec,
    int nIDComboBox,
    int nIDStaticPath,
    UINT uFiletype);
 
extern WINBOOL
 PASCAL WINBASEAPI DlgDirSelectComboBoxExA(
    HWND hDlg,
    LPSTR lpString,
    int nCount,
    int nIDComboBox);
 
extern LRESULT
 PASCAL WINBASEAPI DefFrameProcA(
    HWND hWnd,
    HWND hWndMDIClient ,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam);
 
extern LRESULT
 PASCAL WINBASEAPI DefMDIChildProcA(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam);
 
extern HWND
 PASCAL WINBASEAPI CreateMDIWindowA(
    LPSTR lpClassName,
    LPSTR lpWindowName,
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
 PASCAL WINBASEAPI WinHelpA(
    HWND hWndMain,
    LPCSTR lpszHelp,
    UINT uCommand,
    DWORD dwData
    );
 
extern LONG
 PASCAL WINBASEAPI ChangeDisplaySettingsA(
    LPDEVMODE lpDevMode,
    DWORD dwFlags);
 
extern WINBOOL
 PASCAL WINBASEAPI EnumDisplaySettingsA(
    LPCSTR lpszDeviceName,
    DWORD iModeNum,
    LPDEVMODE lpDevMode);
 
extern WINBOOL
 PASCAL WINBASEAPI SystemParametersInfoA(
    UINT uiAction,
    UINT uiParam,
    PVOID pvParam,
    UINT fWinIni);
 
extern int
 PASCAL WINBASEAPI AddFontResourceA(LPCSTR);

extern int 
 PASCAL WINBASEAPI AddFontResourceExA(LPCSTR, DWORD, PVOID);

extern HMETAFILE
 PASCAL WINBASEAPI CopyMetaFileA(HMETAFILE, LPCSTR);

extern HFONT
 PASCAL WINBASEAPI CreateFontIndirectA(CONST LOGFONT *);

extern HDC
 PASCAL WINBASEAPI CreateICA(LPCSTR, LPCSTR , LPCSTR , CONST DEVMODE *);

extern HDC
 PASCAL WINBASEAPI CreateMetaFileA(LPCSTR);

extern WINBOOL
 PASCAL WINBASEAPI CreateScalableFontResourceA(DWORD, LPCSTR, LPCSTR, LPCSTR);

extern int
 PASCAL WINBASEAPI DeviceCapabilitiesA(LPCSTR, LPCSTR, WORD,
                                LPSTR, CONST DEVMODE *);

extern int
 PASCAL WINBASEAPI EnumFontFamiliesExA(HDC, LPLOGFONT, FONTENUMEXPROC, LPARAM,DWORD);

extern int
 PASCAL WINBASEAPI EnumFontFamiliesA(HDC, LPCSTR, FONTENUMPROC, LPARAM);

extern int
 PASCAL WINBASEAPI EnumFontsA(HDC, LPCSTR,  OLDFONTENUMPROC, LPARAM);

extern WINBOOL
 PASCAL WINBASEAPI GetCharWidthA(HDC, UINT, UINT, LPINT);

extern WINBOOL
 PASCAL WINBASEAPI GetCharWidth32A(HDC, UINT, UINT, LPINT);

extern WINBOOL
 PASCAL WINBASEAPI GetCharWidthFloatA(HDC, UINT, UINT, PFLOAT);

extern WINBOOL
 PASCAL WINBASEAPI GetCharABCWidthsA(HDC, UINT, UINT, LPABC);

extern WINBOOL
 PASCAL WINBASEAPI GetCharABCWidthsFloatA(HDC, UINT, UINT, LPABCFLOAT);
extern DWORD
 PASCAL WINBASEAPI GetGlyphOutlineA(HDC, UINT, UINT, LPGLYPHMETRICS, DWORD, LPVOID, CONST MAT2 *);

extern HMETAFILE
 PASCAL WINBASEAPI GetMetaFileA(LPCSTR);

extern UINT
 PASCAL WINBASEAPI GetOutlineTextMetricsA(HDC, UINT, LPOUTLINETEXTMETRIC);

extern WINBOOL
 PASCAL WINBASEAPI GetTextExtentPointA(
                    HDC,
                    LPCSTR,
                    int,
                    LPSIZE
                    );

extern WINBOOL
 PASCAL WINBASEAPI GetTextExtentPoint32A(
                    HDC,
                    LPCSTR,
                    int,
                    LPSIZE
                    );

extern WINBOOL
 PASCAL WINBASEAPI GetTextExtentExPointA(
                    HDC,
                    LPCSTR,
                    int,
                    int,
                    LPINT,
                    LPINT,
                    LPSIZE
                    );

extern DWORD
 PASCAL WINBASEAPI GetCharacterPlacementA(HDC, LPCSTR, int, int, LPGCP_RESULTSA, DWORD);

extern HDC
 PASCAL WINBASEAPI ResetDCA(HDC, CONST DEVMODE *);

extern WINBOOL
 PASCAL WINBASEAPI RemoveFontResourceA(LPCSTR);

extern BOOL 
 PASCAL WINBASEAPI RemoveFontResourceExA(LPCSTR, DWORD, PVOID);

extern HENHMETAFILE
 PASCAL WINBASEAPI CopyEnhMetaFileA(HENHMETAFILE, LPCSTR);

extern HDC
 PASCAL WINBASEAPI CreateEnhMetaFileA(HDC, LPCSTR, CONST RECT *, LPCSTR);

extern HENHMETAFILE
 PASCAL WINBASEAPI GetEnhMetaFileA(LPCSTR);

extern UINT
 PASCAL WINBASEAPI GetEnhMetaFileDescriptionA(HENHMETAFILE, UINT, LPSTR );

extern WINBOOL
 PASCAL WINBASEAPI GetTextMetricsA(HDC, LPTEXTMETRIC);

extern int
 PASCAL WINBASEAPI StartDocA(HDC, CONST DOCINFO *);

extern int
 PASCAL WINBASEAPI GetObjectA(HGDIOBJ, int, LPVOID);

extern WINBOOL
 PASCAL WINBASEAPI TextOutA(HDC, int, int, LPCSTR, int);

extern WINBOOL
 PASCAL WINBASEAPI ExtTextOutA(HDC, int, int, UINT, CONST RECT *,LPCSTR, UINT, CONST INT *);

extern WINBOOL
 PASCAL WINBASEAPI PolyTextOutA(HDC, CONST POLYTEXT *, int); 

extern int
 PASCAL WINBASEAPI GetTextFaceA(HDC, int, LPSTR);

extern DWORD
 PASCAL WINBASEAPI GetKerningPairsA(HDC, DWORD, LPKERNINGPAIR);

extern HCOLORSPACE
 PASCAL WINBASEAPI CreateColorSpaceA(LPLOGCOLORSPACE);

extern WINBOOL
 PASCAL WINBASEAPI GetLogColorSpaceA(HCOLORSPACE,LPLOGCOLORSPACE,DWORD);

extern WINBOOL
 PASCAL WINBASEAPI GetICMProfileA(HDC,DWORD,LPSTR);

extern WINBOOL
 PASCAL WINBASEAPI SetICMProfileA(HDC,LPSTR);

extern WINBOOL
 PASCAL WINBASEAPI UpdateICMRegKeyA(DWORD, DWORD, LPSTR, UINT);

extern int
 PASCAL WINBASEAPI EnumICMProfilesA(HDC,ICMENUMPROC,LPARAM);

extern WINBOOL
 PASCAL WINBASEAPI CreateProcessA(
    LPCSTR lpApplicationName,
    LPSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    WINBOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCSTR lpCurrentDirectory,
    LPSTARTUPINFO lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
    );

extern VOID
 PASCAL WINBASEAPI GetStartupInfoA(
    LPSTARTUPINFO lpStartupInfo
    );

extern HANDLE
 PASCAL WINBASEAPI FindFirstFileA(
    LPCSTR lpFileName,
    LPWIN32_FIND_DATA lpFindFileData
    );

extern WINBOOL
 PASCAL WINBASEAPI FindNextFileA(
    HANDLE hFindFile,
    LPWIN32_FIND_DATA lpFindFileData
    );

extern WINBOOL
 PASCAL WINBASEAPI GetVersionExA(
    LPOSVERSIONINFO lpVersionInformation
    );

#define CreateWindowA(lpClassName, lpWindowName, dwStyle, x, y,\
nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)\
CreateWindowExA(0L, lpClassName, lpWindowName, dwStyle, x, y,\
nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)

#define CreateDialogA(hInstance, lpName, hWndParent, lpDialogFunc) \
CreateDialogParamA(hInstance, lpName, hWndParent, lpDialogFunc, 0L)

#define CreateDialogIndirectA(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
CreateDialogIndirectParamA(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0L)

#define DialogBoxA(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
DialogBoxParamA(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0L)

#define DialogBoxIndirectA(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
DialogBoxIndirectParamA(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0L)

extern HDC
 PASCAL WINBASEAPI CreateDCA(LPCSTR, LPCSTR , LPCSTR , CONST DEVMODE *);

extern DWORD
 PASCAL WINBASEAPI VerInstallFileA(
        DWORD uFlags,
        LPSTR szSrcFileName,
        LPSTR szDestFileName,
        LPSTR szSrcDir,
        LPSTR szDestDir,
        LPSTR szCurDir,
        LPSTR szTmpFile,
        PUINT lpuTmpFileLen
        );

extern DWORD
 PASCAL WINBASEAPI GetFileVersionInfoSizeA(
        LPSTR lptstrFilename,
        LPDWORD lpdwHandle
        );

extern WINBOOL
 PASCAL WINBASEAPI GetFileVersionInfoA(
        LPSTR lptstrFilename,
        DWORD dwHandle,
        DWORD dwLen,
        LPVOID lpData
        );

extern DWORD
 PASCAL WINBASEAPI VerLanguageNameA(
        DWORD wLang,
        LPSTR szLang,
        DWORD nSize
        );

extern WINBOOL
 PASCAL WINBASEAPI VerQueryValueA(
        const LPVOID pBlock,
        LPSTR lpSubBlock,
        LPVOID * lplpBuffer,
        PUINT puLen
        );

extern DWORD
 PASCAL WINBASEAPI VerFindFileA(
        DWORD uFlags,
        LPSTR szFileName,
        LPSTR szWinDir,
        LPSTR szAppDir,
        LPSTR szCurDir,
        PUINT lpuCurDirLen,
        LPSTR szDestDir,
        PUINT lpuDestDirLen
        );

extern LONG
 PASCAL WINBASEAPI RegConnectRegistryA(
    LPSTR lpMachineName,
    HKEY hKey,
    PHKEY phkResult
    );

extern LONG
 PASCAL WINBASEAPI RegCreateKeyA(
    HKEY hKey,
    LPCSTR lpSubKey,
    PHKEY phkResult
    );

extern LONG
 PASCAL WINBASEAPI RegCreateKeyExA(
    HKEY hKey,
    LPCSTR lpSubKey,
    DWORD Reserved,
    LPSTR lpClass,
    DWORD dwOptions,
    REGSAM samDesired,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    PHKEY phkResult,
    LPDWORD lpdwDisposition
    );

extern LONG
 PASCAL WINBASEAPI RegDeleteKeyA(
    HKEY hKey,
    LPCSTR lpSubKey
    );

extern LONG
 PASCAL WINBASEAPI RegDeleteValueA(
    HKEY hKey,
    LPCSTR lpValueName
    );

extern LONG
 PASCAL WINBASEAPI RegEnumKeyA(
    HKEY hKey,
    DWORD dwIndex,
    LPSTR lpName,
    DWORD cbName
    );

extern LONG
 PASCAL WINBASEAPI RegEnumKeyExA(
    HKEY hKey,
    DWORD dwIndex,
    LPSTR lpName,
    LPDWORD lpcbName,
    LPDWORD lpReserved,
    LPSTR lpClass,
    LPDWORD lpcbClass,
    PFILETIME lpftLastWriteTime
    );

extern LONG
 PASCAL WINBASEAPI RegEnumValueA(
    HKEY hKey,
    DWORD dwIndex,
    LPSTR lpValueName,
    LPDWORD lpcbValueName,
    LPDWORD lpReserved,
    LPDWORD lpType,
    LPBYTE lpData,
    LPDWORD lpcbData
    );

extern LONG
 PASCAL WINBASEAPI RegLoadKeyA(
    HKEY    hKey,
    LPCSTR  lpSubKey,
    LPCSTR  lpFile
    );

extern LONG
 PASCAL WINBASEAPI RegOpenKeyA(
    HKEY hKey,
    LPCSTR lpSubKey,
    PHKEY phkResult
    );

extern LONG
 PASCAL WINBASEAPI RegOpenKeyExA(
    HKEY hKey,
    LPCSTR lpSubKey,
    DWORD ulOptions,
    REGSAM samDesired,
    PHKEY phkResult
    );

extern LONG
 PASCAL WINBASEAPI RegQueryInfoKeyA(
    HKEY hKey,
    LPSTR lpClass,
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

extern LONG
 PASCAL WINBASEAPI RegQueryValueA(
    HKEY hKey,
    LPCSTR lpSubKey,
    LPSTR lpValue,
    PLONG   lpcbValue
    );

extern LONG
 PASCAL WINBASEAPI RegQueryMultipleValuesA(
    HKEY hKey,
    PVALENT val_list,
    DWORD num_vals,
    LPSTR lpValueBuf,
    LPDWORD ldwTotsize
    );

extern LONG
 PASCAL WINBASEAPI RegQueryValueExA(
    HKEY hKey,
    LPCSTR lpValueName,
    LPDWORD lpReserved,
    LPDWORD lpType,
    LPBYTE lpData,
    LPDWORD lpcbData
    );

extern LONG
 PASCAL WINBASEAPI RegReplaceKeyA(
    HKEY     hKey,
    LPCSTR  lpSubKey,
    LPCSTR  lpNewFile,
    LPCSTR  lpOldFile
    );

extern LONG
 PASCAL WINBASEAPI RegRestoreKeyA(
    HKEY hKey,
    LPCSTR lpFile,
    DWORD   dwFlags
    );

extern LONG
 PASCAL WINBASEAPI RegSaveKeyA(
    HKEY hKey,
    LPCSTR lpFile,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );

extern LONG
 PASCAL WINBASEAPI RegSetValueA(
    HKEY hKey,
    LPCSTR lpSubKey,
    DWORD dwType,
    LPCSTR lpData,
    DWORD cbData
    );

extern LONG
 PASCAL WINBASEAPI RegSetValueExA(
    HKEY hKey,
    LPCSTR lpValueName,
    DWORD Reserved,
    DWORD dwType,
    CONST BYTE* lpData,
    DWORD cbData
    );

extern LONG
 PASCAL WINBASEAPI RegUnLoadKeyA(
    HKEY    hKey,
    LPCSTR lpSubKey
    );

extern WINBOOL
 PASCAL WINBASEAPI InitiateSystemShutdownA(
    LPSTR lpMachineName,
    LPSTR lpMessage,
    DWORD dwTimeout,
    WINBOOL bForceAppsClosed,
    WINBOOL bRebootAfterShutdown
    );

extern WINBOOL
 PASCAL WINBASEAPI AbortSystemShutdownA(
    LPSTR lpMachineName
    );

extern int
 PASCAL WINBASEAPI CompareStringA(
    LCID     Locale,
    DWORD    dwCmpFlags,
    LPCSTR lpString1,
    int      cchCount1,
    LPCSTR lpString2,
    int      cchCount2);

extern int
 PASCAL WINBASEAPI LCMapStringA(
    LCID     Locale,
    DWORD    dwMapFlags,
    LPCSTR lpSrcStr,
    int      cchSrc,
    LPSTR  lpDestStr,
    int      cchDest);

extern int
 PASCAL WINBASEAPI GetLocaleInfoA(
    LCID     Locale,
    LCTYPE   LCType,
    LPSTR  lpLCData,
    int      cchData);

extern WINBOOL
 PASCAL WINBASEAPI SetLocaleInfoA(
    LCID     Locale,
    LCTYPE   LCType,
    LPCSTR lpLCData);

extern int
 PASCAL WINBASEAPI GetTimeFormatA(
    LCID     Locale,
    DWORD    dwFlags,
    CONST SYSTEMTIME *lpTime,
    LPCSTR lpFormat,
    LPSTR  lpTimeStr,
    int      cchTime);

extern int
 PASCAL WINBASEAPI GetDateFormatA(
    LCID     Locale,
    DWORD    dwFlags,
    CONST SYSTEMTIME *lpDate,
    LPCSTR lpFormat,
    LPSTR  lpDateStr,
    int      cchDate);

extern int
 PASCAL WINBASEAPI GetNumberFormatA(
    LCID     Locale,
    DWORD    dwFlags,
    LPCSTR lpValue,
    CONST NUMBERFMT *lpFormat,
    LPSTR  lpNumberStr,
    int      cchNumber);

extern int
 PASCAL WINBASEAPI GetCurrencyFormatA(
    LCID     Locale,
    DWORD    dwFlags,
    LPCSTR lpValue,
    CONST CURRENCYFMT *lpFormat,
    LPSTR  lpCurrencyStr,
    int      cchCurrency);

extern WINBOOL
 PASCAL WINBASEAPI EnumCalendarInfoA(
    CALINFO_ENUMPROC lpCalInfoEnumProc,
    LCID              Locale,
    CALID             Calendar,
    CALTYPE           CalType);

extern WINBOOL
 PASCAL WINBASEAPI EnumTimeFormatsA(
    TIMEFMT_ENUMPROC lpTimeFmtEnumProc,
    LCID              Locale,
    DWORD             dwFlags);

extern WINBOOL
 PASCAL WINBASEAPI EnumDateFormatsA(
    DATEFMT_ENUMPROC lpDateFmtEnumProc,
    LCID              Locale,
    DWORD             dwFlags);

extern WINBOOL
 PASCAL WINBASEAPI GetStringTypeExA(
    LCID     Locale,
    DWORD    dwInfoType,
    LPCSTR lpSrcStr,
    int      cchSrc,
    LPWORD   lpCharType);

extern WINBOOL
 PASCAL WINBASEAPI GetStringTypeA(
    LCID     Locale,
    DWORD    dwInfoType,
    LPCSTR   lpSrcStr,
    int      cchSrc,
    LPWORD   lpCharType);


extern int
 PASCAL WINBASEAPI FoldStringA(
    DWORD    dwMapFlags,
    LPCSTR lpSrcStr,
    int      cchSrc,
    LPSTR  lpDestStr,
    int      cchDest);

extern WINBOOL
 PASCAL WINBASEAPI EnumSystemLocalesA(
    LOCALE_ENUMPROC lpLocaleEnumProc,
    DWORD            dwFlags);

extern WINBOOL
 PASCAL WINBASEAPI EnumSystemCodePagesA(
    CODEPAGE_ENUMPROC lpCodePageEnumProc,
    DWORD              dwFlags);

extern WINBOOL
 PASCAL WINBASEAPI PeekConsoleInputA(
    HANDLE hConsoleInput,
    PINPUT_RECORD lpBuffer,
    DWORD nLength,
    LPDWORD lpNumberOfEventsRead
    );

extern WINBOOL
 PASCAL WINBASEAPI ReadConsoleInputA(
    HANDLE hConsoleInput,
    PINPUT_RECORD lpBuffer,
    DWORD nLength,
    LPDWORD lpNumberOfEventsRead
    );

extern WINBOOL
 PASCAL WINBASEAPI WriteConsoleInputA(
    HANDLE hConsoleInput,
    CONST INPUT_RECORD *lpBuffer,
    DWORD nLength,
    LPDWORD lpNumberOfEventsWritten
    );

extern WINBOOL
 PASCAL WINBASEAPI ReadConsoleOutputA(
    HANDLE hConsoleOutput,
    PCHAR_INFO lpBuffer,
    COORD dwBufferSize,
    COORD dwBufferCoord,
    PSMALL_RECT lpReadRegion
    );

extern WINBOOL
 PASCAL WINBASEAPI WriteConsoleOutputA(
    HANDLE hConsoleOutput,
    CONST CHAR_INFO *lpBuffer,
    COORD dwBufferSize,
    COORD dwBufferCoord,
    PSMALL_RECT lpWriteRegion
    );

extern WINBOOL
 PASCAL WINBASEAPI ReadConsoleOutputCharacterA(
    HANDLE hConsoleOutput,
    LPSTR lpCharacter,
    DWORD nLength,
    COORD dwReadCoord,
    LPDWORD lpNumberOfCharsRead
    );

extern WINBOOL
 PASCAL WINBASEAPI WriteConsoleOutputCharacterA(
    HANDLE hConsoleOutput,
    LPCSTR lpCharacter,
    DWORD nLength,
    COORD dwWriteCoord,
    LPDWORD lpNumberOfCharsWritten
    );

extern WINBOOL
 PASCAL WINBASEAPI FillConsoleOutputCharacterA(
    HANDLE hConsoleOutput,
    CHAR  cCharacter,
    DWORD  nLength,
    COORD  dwWriteCoord,
    LPDWORD lpNumberOfCharsWritten
    );

extern WINBOOL
 PASCAL WINBASEAPI ScrollConsoleScreenBufferA(
    HANDLE hConsoleOutput,
    CONST SMALL_RECT *lpScrollRectangle,
    CONST SMALL_RECT *lpClipRectangle,
    COORD dwDestinationOrigin,
    CONST CHAR_INFO *lpFill
    );

extern DWORD
 PASCAL WINBASEAPI GetConsoleTitleA(
    LPSTR lpConsoleTitle,
    DWORD nSize
    );

extern WINBOOL
 PASCAL WINBASEAPI SetConsoleTitleA(
    LPCSTR lpConsoleTitle
    );

extern WINBOOL
 PASCAL WINBASEAPI ReadConsoleA(
    HANDLE hConsoleInput,
    LPVOID lpBuffer,
    DWORD nNumberOfCharsToRead,
    LPDWORD lpNumberOfCharsRead,
    LPVOID lpReserved
    );

extern WINBOOL
 PASCAL WINBASEAPI WriteConsoleA(
    HANDLE hConsoleOutput,
    CONST VOID *lpBuffer,
    DWORD nNumberOfCharsToWrite,
    LPDWORD lpNumberOfCharsWritten,
    LPVOID lpReserved
    );

extern DWORD  PASCAL WINBASEAPI WNetAddConnectionA(
     LPCSTR   lpRemoteName,
     LPCSTR   lpPassword,
     LPCSTR   lpLocalName
    );

extern DWORD  PASCAL WINBASEAPI WNetAddConnection2A(
     LPNETRESOURCE lpNetResource,
     LPCSTR       lpPassword,
     LPCSTR       lpUserName,
     DWORD          dwFlags
    );

extern DWORD  PASCAL WINBASEAPI WNetAddConnection3A(
     HWND           hwndOwner,
     LPNETRESOURCE lpNetResource,
     LPCSTR       lpPassword,
     LPCSTR       lpUserName,
     DWORD          dwFlags
    );

extern DWORD  PASCAL WINBASEAPI WNetCancelConnectionA(
     LPCSTR lpName,
     WINBOOL     fForce
    );

extern DWORD  PASCAL WINBASEAPI WNetCancelConnection2A(
     LPCSTR lpName,
     DWORD    dwFlags,
     WINBOOL     fForce
    );

extern DWORD  PASCAL WINBASEAPI WNetGetConnectionA(
     LPCSTR lpLocalName,
     LPSTR  lpRemoteName,
     LPDWORD  lpnLength
    );

extern DWORD  PASCAL WINBASEAPI WNetUseConnectionA(
    HWND            hwndOwner,
    LPNETRESOURCE  lpNetResource,
    LPCSTR        lpUserID,
    LPCSTR        lpPassword,
    DWORD           dwFlags,
    LPSTR         lpAccessName,
    LPDWORD         lpBufferSize,
    LPDWORD         lpResult
    );

extern DWORD  PASCAL WINBASEAPI WNetSetConnectionA(
    LPCSTR    lpName,
    DWORD       dwProperties,
    LPVOID      pvValues
    );

extern DWORD  PASCAL WINBASEAPI WNetConnectionDialog1A(
    LPCONNECTDLGSTRUCT lpConnDlgStruct
    );

extern DWORD  PASCAL WINBASEAPI WNetDisconnectDialog1A(
    LPDISCDLGSTRUCT lpConnDlgStruct
    );

extern DWORD  PASCAL WINBASEAPI WNetOpenEnumA(
     DWORD          dwScope,
     DWORD          dwType,
     DWORD          dwUsage,
     LPNETRESOURCE lpNetResource,
     LPHANDLE       lphEnum
    );

extern DWORD  PASCAL WINBASEAPI WNetEnumResourceA(
     HANDLE  hEnum,
     LPDWORD lpcCount,
     LPVOID  lpBuffer,
     LPDWORD lpBufferSize
    );

extern DWORD  PASCAL WINBASEAPI WNetGetUniversalNameA(
     LPCSTR lpLocalPath,
     DWORD    dwInfoLevel,
     LPVOID   lpBuffer,
     LPDWORD  lpBufferSize
     );

extern DWORD  PASCAL WINBASEAPI WNetGetUserA(
     LPCSTR  lpName,
     LPSTR   lpUserName,
     LPDWORD   lpnLength
    );

extern DWORD  PASCAL WINBASEAPI WNetGetProviderNameA(
    DWORD   dwNetType,
    LPSTR lpProviderName,
    LPDWORD lpBufferSize
    );

extern DWORD  PASCAL WINBASEAPI WNetGetNetworkInformationA(
    LPCSTR          lpProvider,
    LPNETINFOSTRUCT   lpNetInfoStruct
    );

extern DWORD  PASCAL WINBASEAPI WNetGetLastErrorA(
     LPDWORD    lpError,
     LPSTR    lpErrorBuf,
     DWORD      nErrorBufSize,
     LPSTR    lpNameBuf,
     DWORD      nNameBufSize
    );

extern DWORD  PASCAL WINBASEAPI MultinetGetConnectionPerformanceA(
        LPNETRESOURCE lpNetResource,
        LPNETCONNECTINFOSTRUCT lpNetConnectInfoStruct
        );

extern WINBOOL
 PASCAL WINBASEAPI ChangeServiceConfigA(
    SC_HANDLE    hService,
    DWORD        dwServiceType,
    DWORD        dwStartType,
    DWORD        dwErrorControl,
    LPCSTR     lpBinaryPathName,
    LPCSTR     lpLoadOrderGroup,
    LPDWORD      lpdwTagId,
    LPCSTR     lpDependencies,
    LPCSTR     lpServiceStartName,
    LPCSTR     lpPassword,
    LPCSTR     lpDisplayName
    );

extern SC_HANDLE
 PASCAL WINBASEAPI CreateServiceA(
    SC_HANDLE    hSCManager,
    LPCSTR     lpServiceName,
    LPCSTR     lpDisplayName,
    DWORD        dwDesiredAccess,
    DWORD        dwServiceType,
    DWORD        dwStartType,
    DWORD        dwErrorControl,
    LPCSTR     lpBinaryPathName,
    LPCSTR     lpLoadOrderGroup,
    LPDWORD      lpdwTagId,
    LPCSTR     lpDependencies,
    LPCSTR     lpServiceStartName,
    LPCSTR     lpPassword
    );

extern WINBOOL
 PASCAL WINBASEAPI EnumDependentServicesA(
    SC_HANDLE               hService,
    DWORD                   dwServiceState,
    LPENUM_SERVICE_STATUS  lpServices,
    DWORD                   cbBufSize,
    LPDWORD                 pcbBytesNeeded,
    LPDWORD                 lpServicesReturned
    );

extern WINBOOL
 PASCAL WINBASEAPI EnumServicesStatusA(
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
 PASCAL WINBASEAPI GetServiceKeyNameA(
    SC_HANDLE               hSCManager,
    LPCSTR                lpDisplayName,
    LPSTR                 lpServiceName,
    LPDWORD                 lpcchBuffer
    );

extern WINBOOL
 PASCAL WINBASEAPI GetServiceDisplayNameA(
    SC_HANDLE               hSCManager,
    LPCSTR                lpServiceName,
    LPSTR                 lpDisplayName,
    LPDWORD                 lpcchBuffer
    );

extern SC_HANDLE
 PASCAL WINBASEAPI OpenSCManagerA(
    LPCSTR lpMachineName,
    LPCSTR lpDatabaseName,
    DWORD   dwDesiredAccess
    );

extern SC_HANDLE
 PASCAL WINBASEAPI OpenServiceA(
    SC_HANDLE   hSCManager,
    LPCSTR    lpServiceName,
    DWORD       dwDesiredAccess
    );

extern WINBOOL
 PASCAL WINBASEAPI QueryServiceConfigA(
    SC_HANDLE               hService,
    LPQUERY_SERVICE_CONFIG lpServiceConfig,
    DWORD                   cbBufSize,
    LPDWORD                 pcbBytesNeeded
    );

extern WINBOOL
 PASCAL WINBASEAPI QueryServiceLockStatusA(
    SC_HANDLE                       hSCManager,
    LPQUERY_SERVICE_LOCK_STATUS    lpLockStatus,
    DWORD                           cbBufSize,
    LPDWORD                         pcbBytesNeeded
    );

extern SERVICE_STATUS_HANDLE
 PASCAL WINBASEAPI RegisterServiceCtrlHandlerA(
    LPCSTR             lpServiceName,
    LPHANDLER_FUNCTION   lpHandlerProc
    );

extern WINBOOL
 PASCAL WINBASEAPI StartServiceCtrlDispatcherA(
			    LPSERVICE_TABLE_ENTRY    lpServiceStartTable
			    );

extern WINBOOL
 PASCAL WINBASEAPI StartServiceA(
	      SC_HANDLE            hService,
	      DWORD                dwNumServiceArgs,
	      LPCSTR             *lpServiceArgVectors
	      );

/* Extensions to OpenGL */

extern WINBOOL  PASCAL WINBASEAPI wglUseFontBitmapsA(HDC, DWORD, DWORD, DWORD);

extern WINBOOL  PASCAL WINBASEAPI wglUseFontOutlinesA(HDC, DWORD, DWORD, DWORD, FLOAT,
		    FLOAT, int, LPGLYPHMETRICSFLOAT);

/* -------------------------------------------------- */
/* From ddeml.h in old Cygnus headers */

extern HSZ  PASCAL WINBASEAPI DdeCreateStringHandleA(DWORD, char *, int);

extern UINT  PASCAL WINBASEAPI DdeInitializeA(DWORD *, CALLB, DWORD, DWORD);

extern DWORD  PASCAL WINBASEAPI DdeQueryStringA(DWORD, HSZ, char *, DWORD, int);

/* end of stuff from ddeml.h in old Cygnus headers */
/* ----------------------------------------------- */

extern WINBOOL  PASCAL WINBASEAPI LogonUserA(LPSTR, LPSTR, LPSTR, DWORD, DWORD, HANDLE *);
extern WINBOOL  PASCAL WINBASEAPI CreateProcessAsUserA(HANDLE, LPCTSTR, LPTSTR,
	SECURITY_ATTRIBUTES*, SECURITY_ATTRIBUTES*, WINBOOL, DWORD, LPVOID,
	LPCTSTR, STARTUPINFO*, PROCESS_INFORMATION*);

extern WINBOOL  PASCAL WINBASEAPI PlaySoundA(LPCSTR pszSound, HMODULE hmod, DWORD fdwSound);

extern WINBOOL  PASCAL WINBASEAPI GetCPInfoExA(UINT CodePage,DWORD dwFlags, LPCPINFOEX lpCPInfoEx);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* RC_INVOKED */

#endif /* _GNU_H_WINDOWS32_ASCIIFUNCTIONS */
