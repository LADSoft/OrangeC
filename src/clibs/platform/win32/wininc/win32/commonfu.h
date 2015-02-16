/* 
   CommonFunctions.h

   Declarations for all the Windows32 API Functions that do not have
   separate ANSI and UNICODE versions.

   Copyright (C) 1996 Free Software Foundation, Inc.

   Author: Colin Peters <colin@fu.is.saga-u.ac.jp>

   Most of this file was extracted from Functions.h, by Scott Christley

   This file is part of the Windows32 API Library.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; see the file COPYING.LIB.
   If not, write to the Free Software Foundation, 
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   -----------
   DAL 2003 - this file modified extensively for my compiler.  New
   definitionswnwn added as well.
*/ 

#ifndef _GNU_H_WINDOWS32_COMMONFUNCTIONS
#define _GNU_H_WINDOWS32_COMMONFUNCTIONS


/* Skip if invoked by resource compiler */
#ifndef RC_INVOKED

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern WINBOOL  PASCAL WINBASEAPI AbnormalTermination(VOID);
extern WINBOOL  PASCAL WINBASEAPI AbortPrinter(HANDLE);
extern WINBOOL  PASCAL WINBASEAPI AbortProc(HDC, int);
/*
extern int  PASCAL WINBASEAPI AbortDoc(HDC);
extern WINBOOL  PASCAL WINBASEAPI AbortPath(HDC);
extern WINBOOL  PASCAL WINBASEAPI AccessCheck(
		    PSECURITY_DESCRIPTOR  pSecurityDescriptor,
		    HANDLE  ClientToken,
		    DWORD  DesiredAccess,
		    PGENERIC_MAPPING  GenericMapping,
		    PPRIVILEGE_SET  PrivilegeSet,
		    LPDWORD  PrivilegeSetLength,
		    LPDWORD  GrantedAccess,
		    LPBOOL  AccessStatus
		    );
*/
extern LONG PASCAL InterlockedCompareExchange (
    PLONG Destination,
    LONG ExChange,
    LONG Comperand
    );

extern LONG
 PASCAL WINBASEAPI InterlockedIncrement(
		     LPLONG lpAddend
		     );

extern LONG
 PASCAL WINBASEAPI InterlockedDecrement(
		     LPLONG lpAddend
		     );

extern LONG
 PASCAL WINBASEAPI InterlockedExchange(
		    LPLONG Target,
		    LONG Value
		    );

extern LONG PASCAL WINBASEAPI InterlockedExchangeAdd(LPLONG,LONG);

extern int  PASCAL WINBASEAPI AddFontMemResourceEx(PVOID, DWORD, PVOID , DWORD*);
extern int  PASCAL WINBASEAPI RemoveFontMemResourceEx( HANDLE);

extern WINBOOL
 PASCAL WINBASEAPI FreeResource(
	     HGLOBAL hResData
	     );

extern LPVOID
 PASCAL WINBASEAPI LockResource(
	     HGLOBAL hResData
	     );

#define UnlockResource(hResData) ((hResData), 0)

extern int
 PASCAL WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd
	);

extern int
 PASCAL DllMain(
 	HINSTANCE hinstDLL,
	DWORD fdwReason,
	LPVOID lpvReserved
	);

#define DllEntryPoint DllMain

extern WINBOOL
 PASCAL WINBASEAPI FreeLibrary(
	    HINSTANCE hLibModule
	    );


extern VOID
 PASCAL WINBASEAPI FreeLibraryAndExitThread(
			 HMODULE hLibModule,
			 DWORD dwExitCode
			 );

extern WINBOOL
 PASCAL WINBASEAPI DisableThreadLibraryCalls(
			  HMODULE hLibModule
			  );

extern FARPROC
 PASCAL WINBASEAPI GetProcAddress(
	       HINSTANCE hModule,
	       LPCSTR lpProcName
	       );

extern DWORD
 PASCAL WINBASEAPI GetVersion( VOID );

extern HGLOBAL
 PASCAL WINBASEAPI GlobalAlloc(
	    UINT uFlags,
	    DWORD dwBytes
	    );

extern HGLOBAL
GlobalDiscard(
	      HGLOBAL hglbMem
	      );

extern HGLOBAL
 PASCAL WINBASEAPI GlobalReAlloc(
	      HGLOBAL hMem,
	      DWORD dwBytes,
	      UINT uFlags
	      );

extern DWORD
 PASCAL WINBASEAPI GlobalSize(
	   HGLOBAL hMem
	   );

extern UINT
 PASCAL WINBASEAPI GlobalFlags(
	    HGLOBAL hMem
	    );


extern LPVOID
 PASCAL WINBASEAPI GlobalLock(
	   HGLOBAL hMem
	   );

extern HGLOBAL
 PASCAL WINBASEAPI GlobalHandle(
	     LPCVOID pMem
	     );


extern WINBOOL
 PASCAL WINBASEAPI GlobalUnlock(
	     HGLOBAL hMem
	     );


extern HGLOBAL
 PASCAL WINBASEAPI GlobalFree(
	   HGLOBAL hMem
	   );

extern UINT
 PASCAL WINBASEAPI GlobalCompact(
	      DWORD dwMinFree
	      );


extern VOID
 PASCAL WINBASEAPI GlobalFix(
	  HGLOBAL hMem
	  );


extern VOID
 PASCAL WINBASEAPI GlobalUnfix(
	    HGLOBAL hMem
	    );


extern LPVOID
 PASCAL WINBASEAPI GlobalWire(
	   HGLOBAL hMem
	   );


extern WINBOOL
 PASCAL WINBASEAPI GlobalUnWire(
	     HGLOBAL hMem
	     );


extern VOID
 PASCAL WINBASEAPI GlobalMemoryStatus(
		   LPMEMORYSTATUS lpBuffer
		   );

extern WINBOOL 
 PASCAL WINBASEAPI IsDebuggerPresent(
         VOID
         ) ;

extern HLOCAL
 PASCAL WINBASEAPI LocalAlloc(
	   UINT uFlags,
	   UINT uBytes
	   );

extern HLOCAL
LocalDiscard(
	     HLOCAL hlocMem
	     );

extern HLOCAL
 PASCAL WINBASEAPI LocalReAlloc(
	     HLOCAL hMem,
	     UINT uBytes,
	     UINT uFlags
	     );


extern LPVOID
 PASCAL WINBASEAPI LocalLock(
	  HLOCAL hMem
	  );


extern HLOCAL
 PASCAL WINBASEAPI LocalHandle(
	    LPCVOID pMem
	    );


extern WINBOOL
 PASCAL WINBASEAPI LocalUnlock(
	    HLOCAL hMem
	    );


extern UINT
 PASCAL WINBASEAPI LocalSize(
	  HLOCAL hMem
	  );


extern UINT
 PASCAL WINBASEAPI LocalFlags(
	   HLOCAL hMem
	   );


extern HLOCAL
 PASCAL WINBASEAPI LocalFree(
	  HLOCAL hMem
	  );


extern UINT
 PASCAL WINBASEAPI LocalShrink(
	    HLOCAL hMem,
	    UINT cbNewSize
	    );


extern UINT
 PASCAL WINBASEAPI LocalCompact(
	     UINT uMinFree
	     );


extern WINBOOL
 PASCAL WINBASEAPI FlushInstructionCache(
		      HANDLE hProcess,
		      LPCVOID lpBaseAddress,
		      DWORD dwSize
		      );


extern LPVOID
 PASCAL WINBASEAPI VirtualAlloc(
	     LPVOID lpAddress,
	     DWORD dwSize,
	     DWORD flAllocationType,
	     DWORD flProtect
	     );


extern WINBOOL
 PASCAL WINBASEAPI VirtualFree(
	    LPVOID lpAddress,
	    DWORD dwSize,
	    DWORD dwFreeType
	    );


extern WINBOOL
 PASCAL WINBASEAPI VirtualProtect(
	       LPVOID lpAddress,
	       DWORD dwSize,
	       DWORD flNewProtect,
	       PDWORD lpflOldProtect
	       );


extern DWORD
 PASCAL WINBASEAPI VirtualQuery(
	     LPCVOID lpAddress,
	     PMEMORY_BASIC_INFORMATION lpBuffer,
	     DWORD dwLength
	     );


extern WINBOOL
 PASCAL WINBASEAPI VirtualProtectEx(
		 HANDLE hProcess,
		 LPVOID lpAddress,
		 DWORD dwSize,
		 DWORD flNewProtect,
		 PDWORD lpflOldProtect
		 );


extern DWORD
 PASCAL WINBASEAPI VirtualQueryEx(
	       HANDLE hProcess,
	       LPCVOID lpAddress,
	       PMEMORY_BASIC_INFORMATION lpBuffer,
	       DWORD dwLength
	       );


extern HANDLE
 PASCAL WINBASEAPI HeapCreate(
	   DWORD flOptions,
	   DWORD dwInitialSize,
	   DWORD dwMaximumSize
	   );

extern WINBOOL
 PASCAL WINBASEAPI HeapDestroy(
	    HANDLE hHeap
	    );

extern LPVOID
 PASCAL WINBASEAPI HeapAlloc(
	  HANDLE hHeap,
	  DWORD dwFlags,
	  DWORD dwBytes
	  );

extern LPVOID
 PASCAL WINBASEAPI HeapReAlloc(
	    HANDLE hHeap,
	    DWORD dwFlags,
	    LPVOID lpMem,
	    DWORD dwBytes
	    );

extern WINBOOL
 PASCAL WINBASEAPI HeapFree(
	 HANDLE hHeap,
	 DWORD dwFlags,
	 LPVOID lpMem
	 );

extern DWORD
 PASCAL WINBASEAPI HeapSize(
	 HANDLE hHeap,
	 DWORD dwFlags,
	 LPCVOID lpMem
	 );

extern WINBOOL
 PASCAL WINBASEAPI HeapValidate(
	     HANDLE hHeap,
	     DWORD dwFlags,
	     LPCVOID lpMem
	     );

extern UINT
 PASCAL WINBASEAPI HeapCompact(
	    HANDLE hHeap,
	    DWORD dwFlags
	    );

extern HANDLE
 PASCAL WINBASEAPI GetProcessHeap( VOID );

extern DWORD
 PASCAL WINBASEAPI GetProcessHeaps(
		DWORD NumberOfHeaps,
		PHANDLE ProcessHeaps
		);

extern WINBOOL
 PASCAL WINBASEAPI HeapLock(
	 HANDLE hHeap
	 );

extern WINBOOL
 PASCAL WINBASEAPI HeapUnlock(
	   HANDLE hHeap
	   );

extern WINBOOL
 PASCAL WINBASEAPI HeapWalk(
	 HANDLE hHeap,
	 LPPROCESS_HEAP_ENTRY lpEntry
	 );

extern WINBOOL
 PASCAL WINBASEAPI GetProcessAffinityMask(
		       HANDLE hProcess,
		       LPDWORD lpProcessAffinityMask,
		       LPDWORD lpSystemAffinityMask
		       );

extern WINBOOL
 PASCAL WINBASEAPI GetProcessTimes(
		HANDLE hProcess,
		LPFILETIME lpCreationTime,
		LPFILETIME lpExitTime,
		LPFILETIME lpKernelTime,
		LPFILETIME lpUserTime
		);

extern WINBOOL
 PASCAL WINBASEAPI GetProcessWorkingSetSize(
			 HANDLE hProcess,
			 LPDWORD lpMinimumWorkingSetSize,
			 LPDWORD lpMaximumWorkingSetSize
			 );

extern WINBOOL
 PASCAL WINBASEAPI SetProcessWorkingSetSize(
			 HANDLE hProcess,
			 DWORD dwMinimumWorkingSetSize,
			 DWORD dwMaximumWorkingSetSize
			 );

extern HANDLE
 PASCAL WINBASEAPI OpenProcess(
	    DWORD dwDesiredAccess,
	    WINBOOL bInheritHandle,
	    DWORD dwProcessId
	    );

extern HANDLE
 PASCAL WINBASEAPI GetCurrentProcess(
		  VOID
		  );

extern DWORD
 PASCAL WINBASEAPI GetCurrentProcessId(
		    VOID
		    );

extern VOID
 PASCAL WINBASEAPI ExitProcess(
	    UINT uExitCode
	    );

extern WINBOOL
 PASCAL WINBASEAPI TerminateProcess(
		 HANDLE hProcess,
		 UINT uExitCode
		 );

extern WINBOOL
 PASCAL WINBASEAPI GetExitCodeProcess(
		   HANDLE hProcess,
		   LPDWORD lpExitCode
		   );

extern VOID
 PASCAL WINBASEAPI FatalExit(
	  int ExitCode
	  );

extern VOID
 PASCAL WINBASEAPI RaiseException(
	       DWORD dwExceptionCode,
	       DWORD dwExceptionFlags,
	       DWORD nNumberOfArguments,
	       CONST DWORD *lpArguments
	       );
 
extern VOID
 PASCAL WINBASEAPI RtlUnwind(
           PVOID TargetFrame,
           PVOID TargetIp,
           PEXCEPTION_RECORD ExceptionRecord,
           PVOID ReturnValue);

extern LONG
 PASCAL WINBASEAPI UnhandledExceptionFilter(
			 struct _EXCEPTION_POINTERS *ExceptionInfo
			 );

/* TODO: Should this go somewhere else? Colin Peters 1997/10/20 */
typedef LONG (*LPTOP_LEVEL_EXCEPTION_FILTER) (struct _EXCEPTION_POINTERS *);

extern LPTOP_LEVEL_EXCEPTION_FILTER
 PASCAL WINBASEAPI SetUnhandledExceptionFilter(
    LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter
    );


extern HANDLE
 PASCAL WINBASEAPI CreateThread(
	     LPSECURITY_ATTRIBUTES lpThreadAttributes,
	     DWORD dwStackSize,
	     LPTHREAD_START_ROUTINE lpStartAddress,
	     LPVOID lpParameter,
	     DWORD dwCreationFlags,
	     LPDWORD lpThreadId
	     );


extern HANDLE
 PASCAL WINBASEAPI CreateRemoteThread(
		   HANDLE hProcess,
		   LPSECURITY_ATTRIBUTES lpThreadAttributes,
		   DWORD dwStackSize,
		   LPTHREAD_START_ROUTINE lpStartAddress,
		   LPVOID lpParameter,
		   DWORD dwCreationFlags,
		   LPDWORD lpThreadId
		   );


extern HANDLE
 PASCAL WINBASEAPI GetCurrentThread(
		 VOID
		 );


extern DWORD
 PASCAL WINBASEAPI GetCurrentThreadId(
		   VOID
		   );


extern DWORD
 PASCAL WINBASEAPI SetThreadAffinityMask(
		      HANDLE hThread,
		      DWORD dwThreadAffinityMask
		      );


extern WINBOOL
 PASCAL WINBASEAPI SetThreadPriority(
		  HANDLE hThread,
		  int nPriority
		  );


extern int
 PASCAL WINBASEAPI GetThreadPriority(
		  HANDLE hThread
		  );


extern WINBOOL
 PASCAL WINBASEAPI GetThreadTimes(
	       HANDLE hThread,
	       LPFILETIME lpCreationTime,
	       LPFILETIME lpExitTime,
	       LPFILETIME lpKernelTime,
	       LPFILETIME lpUserTime
	       );


extern VOID
 PASCAL WINBASEAPI ExitThread(
	   DWORD dwExitCode
	   );


extern WINBOOL
 PASCAL WINBASEAPI TerminateThread(
		HANDLE hThread,
		DWORD dwExitCode
		);


extern WINBOOL
 PASCAL WINBASEAPI GetExitCodeThread(
		  HANDLE hThread,
		  LPDWORD lpExitCode
		  );

extern WINBOOL
 PASCAL WINBASEAPI GetThreadSelectorEntry(
		       HANDLE hThread,
		       DWORD dwSelector,
		       LPLDT_ENTRY lpSelectorEntry
		       );


extern DWORD
 PASCAL WINBASEAPI GetLastError(
	     VOID
	     );


extern VOID
 PASCAL WINBASEAPI SetLastError(
	     DWORD dwErrCode
	     );


extern WINBOOL
 PASCAL WINBASEAPI GetOverlappedResult(
		    HANDLE hFile,
		    LPOVERLAPPED lpOverlapped,
		    LPDWORD lpNumberOfBytesTransferred,
		    WINBOOL bWait
		    );


extern HANDLE
 PASCAL WINBASEAPI CreateIoCompletionPort(
		       HANDLE FileHandle,
		       HANDLE ExistingCompletionPort,
		       DWORD CompletionKey,
		       DWORD NumberOfConcurrentThreads
		       );


extern WINBOOL
 PASCAL WINBASEAPI GetQueuedCompletionStatus(
			  HANDLE CompletionPort,
			  LPDWORD lpNumberOfBytesTransferred,
			  LPDWORD lpCompletionKey,
			  LPOVERLAPPED *lpOverlapped,
			  DWORD dwMilliseconds
			  );

extern UINT
 PASCAL WINBASEAPI SetErrorMode(
	     UINT uMode
	     );


extern WINBOOL
 PASCAL WINBASEAPI ReadProcessMemory(
		  HANDLE hProcess,
		  LPCVOID lpBaseAddress,
		  LPVOID lpBuffer,
		  DWORD nSize,
		  LPDWORD lpNumberOfBytesRead
		  );


extern WINBOOL
 PASCAL WINBASEAPI WriteProcessMemory(
		   HANDLE hProcess,
		   LPVOID lpBaseAddress,
		   LPVOID lpBuffer,
		   DWORD nSize,
		   LPDWORD lpNumberOfBytesWritten
		   );


extern WINBOOL
 PASCAL WINBASEAPI GetThreadContext(
		 HANDLE hThread,
		 LPCONTEXT lpContext
		 );


extern WINBOOL
 PASCAL WINBASEAPI SetThreadContext(
		 HANDLE hThread,
		 CONST CONTEXT *lpContext
		 );


extern DWORD
 PASCAL WINBASEAPI SuspendThread(
	      HANDLE hThread
	      );


extern DWORD
 PASCAL WINBASEAPI ResumeThread(
	     HANDLE hThread
	     );


extern VOID
 PASCAL WINBASEAPI DebugBreak(
	   VOID
	   );


extern WINBOOL
 PASCAL WINBASEAPI WaitForDebugEvent(
		  LPDEBUG_EVENT lpDebugEvent,
		  DWORD dwMilliseconds
		  );


extern WINBOOL
 PASCAL WINBASEAPI ContinueDebugEvent(
		   DWORD dwProcessId,
		   DWORD dwThreadId,
		   DWORD dwContinueStatus
		   );


extern WINBOOL
 PASCAL WINBASEAPI DebugActiveProcess(
		   DWORD dwProcessId
		   );


extern VOID
 PASCAL WINBASEAPI InitializeCriticalSection(
			  LPCRITICAL_SECTION lpCriticalSection
			  );


extern VOID
 PASCAL WINBASEAPI EnterCriticalSection(
		     LPCRITICAL_SECTION lpCriticalSection
		     );

extern VOID
 PASCAL WINBASEAPI TryEnterCriticalSection(
		     LPCRITICAL_SECTION lpCriticalSection
		     );

extern VOID
 PASCAL WINBASEAPI LeaveCriticalSection(
		     LPCRITICAL_SECTION lpCriticalSection
		     );


extern VOID
 PASCAL WINBASEAPI DeleteCriticalSection(
		      LPCRITICAL_SECTION lpCriticalSection
		      );

extern BOOL
 PASCAL WINBASEAPI InitializeCriticalSectionAndSpinCount(
    LPCRITICAL_SECTION lpCriticalSection,
    DWORD dwSpinCount
    );

extern DWORD
 PASCAL WINBASEAPI SetCriticalSectionSpinCount(
    LPCRITICAL_SECTION lpCriticalSection,
    DWORD dwSpinCount
    );

extern WINBOOL
 PASCAL WINBASEAPI SetEvent(
	 HANDLE hEvent
	 );


extern WINBOOL
 PASCAL WINBASEAPI ResetEvent(
	   HANDLE hEvent
	   );


extern WINBOOL
 PASCAL WINBASEAPI PulseEvent(
	   HANDLE hEvent
	   );


extern WINBOOL
 PASCAL WINBASEAPI ReleaseSemaphore(
		 HANDLE hSemaphore,
		 LONG lReleaseCount,
		 LPLONG lpPreviousCount
		 );


extern WINBOOL
 PASCAL WINBASEAPI ReleaseMutex(
	     HANDLE hMutex
	     );


extern DWORD
 PASCAL WINBASEAPI WaitForSingleObject(
		    HANDLE hHandle,
		    DWORD dwMilliseconds
		    );


extern DWORD
 PASCAL WINBASEAPI WaitForMultipleObjects(
		       DWORD nCount,
		       CONST HANDLE *lpHandles,
		       WINBOOL bWaitAll,
		       DWORD dwMilliseconds
		       );


extern VOID
 PASCAL WINBASEAPI Sleep(
      DWORD dwMilliseconds
      );


extern HGLOBAL
 PASCAL WINBASEAPI LoadResource(
	     HINSTANCE hModule,
	     HRSRC hResInfo
	     );


extern DWORD
 PASCAL WINBASEAPI SizeofResource(
	       HINSTANCE hModule,
	       HRSRC hResInfo
	       );



extern ATOM
 PASCAL WINBASEAPI GlobalDeleteAtom(
		 ATOM nAtom
		 );


extern WINBOOL
 PASCAL WINBASEAPI InitAtomTable(
	      DWORD nSize
	      );


extern ATOM
 PASCAL WINBASEAPI DeleteAtom(
	   ATOM nAtom
	   );


extern UINT
 PASCAL WINBASEAPI SetHandleCount(
	       UINT uNumber
	       );


extern DWORD
 PASCAL WINBASEAPI GetLogicalDrives(
		 VOID
		 );


extern WINBOOL
 PASCAL WINBASEAPI LockFile(
	 HANDLE hFile,
	 DWORD dwFileOffsetLow,
	 DWORD dwFileOffsetHigh,
	 DWORD nNumberOfBytesToLockLow,
	 DWORD nNumberOfBytesToLockHigh
	 );


extern WINBOOL
 PASCAL WINBASEAPI UnlockFile(
	   HANDLE hFile,
	   DWORD dwFileOffsetLow,
	   DWORD dwFileOffsetHigh,
	   DWORD nNumberOfBytesToUnlockLow,
	   DWORD nNumberOfBytesToUnlockHigh
	   );


extern WINBOOL
 PASCAL WINBASEAPI LockFileEx(
	   HANDLE hFile,
	   DWORD dwFlags,
	   DWORD dwReserved,
	   DWORD nNumberOfBytesToLockLow,
	   DWORD nNumberOfBytesToLockHigh,
	   LPOVERLAPPED lpOverlapped
	   );

extern WINBOOL
 PASCAL WINBASEAPI UnlockFileEx(
	     HANDLE hFile,
	     DWORD dwReserved,
	     DWORD nNumberOfBytesToUnlockLow,
	     DWORD nNumberOfBytesToUnlockHigh,
	     LPOVERLAPPED lpOverlapped
	     );

extern WINBOOL
 PASCAL WINBASEAPI GetFileInformationByHandle(
			   HANDLE hFile,
			   LPBY_HANDLE_FILE_INFORMATION lpFileInformation
			   );


extern DWORD
 PASCAL WINBASEAPI GetFileType(
	    HANDLE hFile
	    );


extern DWORD
 PASCAL WINBASEAPI GetFileSize(
	    HANDLE hFile,
	    LPDWORD lpFileSizeHigh
	    );


extern HANDLE
 PASCAL WINBASEAPI GetStdHandle(
	     DWORD nStdHandle
	     );


extern WINBOOL
 PASCAL WINBASEAPI SetStdHandle(
	     DWORD nStdHandle,
	     HANDLE hHandle
	     );


extern WINBOOL
 PASCAL WINBASEAPI WriteFile(
	  HANDLE hFile,
	  LPCVOID lpBuffer,
	  DWORD nNumberOfBytesToWrite,
	  LPDWORD lpNumberOfBytesWritten,
	  LPOVERLAPPED lpOverlapped
	  );


extern WINBOOL
 PASCAL WINBASEAPI ReadFile(
	 HANDLE hFile,
	 LPVOID lpBuffer,
	 DWORD nNumberOfBytesToRead,
	 LPDWORD lpNumberOfBytesRead,
	 LPOVERLAPPED lpOverlapped
	 );


extern WINBOOL
 PASCAL WINBASEAPI FlushFileBuffers(
		 HANDLE hFile
		 );


extern WINBOOL
 PASCAL WINBASEAPI DeviceIoControl(
		HANDLE hDevice,
		DWORD dwIoControlCode,
		LPVOID lpInBuffer,
		DWORD nInBufferSize,
		LPVOID lpOutBuffer,
		DWORD nOutBufferSize,
		LPDWORD lpBytesReturned,
		LPOVERLAPPED lpOverlapped
		);


extern WINBOOL
 PASCAL WINBASEAPI SetEndOfFile(
	     HANDLE hFile
	     );


extern DWORD
 PASCAL WINBASEAPI SetFilePointer(
	       HANDLE hFile,
	       LONG lDistanceToMove,
	       PLONG lpDistanceToMoveHigh,
	       DWORD dwMoveMethod
	       );


extern WINBOOL
 PASCAL WINBASEAPI FindClose(
	  HANDLE hFindFile
	  );


extern WINBOOL
 PASCAL WINBASEAPI GetFileTime(
	    HANDLE hFile,
	    LPFILETIME lpCreationTime,
	    LPFILETIME lpLastAccessTime,
	    LPFILETIME lpLastWriteTime
	    );


extern WINBOOL
 PASCAL WINBASEAPI SetFileTime(
	    HANDLE hFile,
	    CONST FILETIME *lpCreationTime,
	    CONST FILETIME *lpLastAccessTime,
	    CONST FILETIME *lpLastWriteTime
	    );


extern WINBOOL
 PASCAL WINBASEAPI CloseHandle(
	    HANDLE hObject
	    );


extern WINBOOL
 PASCAL WINBASEAPI DuplicateHandle(
		HANDLE hSourceProcessHandle,
		HANDLE hSourceHandle,
		HANDLE hTargetProcessHandle,
		LPHANDLE lpTargetHandle,
		DWORD dwDesiredAccess,
		WINBOOL bInheritHandle,
		DWORD dwOptions
		);


extern WINBOOL
 PASCAL WINBASEAPI GetHandleInformation(
		     HANDLE hObject,
		     LPDWORD lpdwFlags
		     );


extern WINBOOL
 PASCAL WINBASEAPI SetHandleInformation(
		     HANDLE hObject,
		     DWORD dwMask,
		     DWORD dwFlags
		     );

extern DWORD
 PASCAL WINBASEAPI LoadModule(
	   LPCSTR lpModuleName,
	   LPVOID lpParameterBlock
	   );


extern UINT
 PASCAL WINBASEAPI WinExec(
	LPCSTR lpCmdLine,
	UINT uCmdShow
	);


extern WINBOOL
 PASCAL WINBASEAPI ClearCommBreak(
	       HANDLE hFile
	       );


extern WINBOOL
 PASCAL WINBASEAPI ClearCommError(
	       HANDLE hFile,
	       LPDWORD lpErrors,
	       LPCOMSTAT lpStat
	       );


extern WINBOOL
 PASCAL WINBASEAPI SetupComm(
	  HANDLE hFile,
	  DWORD dwInQueue,
	  DWORD dwOutQueue
	  );


extern WINBOOL
 PASCAL WINBASEAPI EscapeCommFunction(
		   HANDLE hFile,
		   DWORD dwFunc
		   );


extern WINBOOL
 PASCAL WINBASEAPI GetCommConfig(
	      HANDLE hCommDev,
	      LPCOMMCONFIG lpCC,
	      LPDWORD lpdwSize
	      );


extern WINBOOL
 PASCAL WINBASEAPI GetCommMask(
	    HANDLE hFile,
	    LPDWORD lpEvtMask
	    );


extern WINBOOL
 PASCAL WINBASEAPI GetCommProperties(
		  HANDLE hFile,
		  LPCOMMPROP lpCommProp
		  );


extern WINBOOL
 PASCAL WINBASEAPI GetCommModemStatus(
		   HANDLE hFile,
		   LPDWORD lpModemStat
		   );


extern WINBOOL
 PASCAL WINBASEAPI GetCommState(
	     HANDLE hFile,
	     LPDCB lpDCB
	     );


extern WINBOOL
 PASCAL WINBASEAPI GetCommTimeouts(
		HANDLE hFile,
		LPCOMMTIMEOUTS lpCommTimeouts
		);


extern WINBOOL
 PASCAL WINBASEAPI PurgeComm(
	  HANDLE hFile,
	  DWORD dwFlags
	  );


extern WINBOOL
 PASCAL WINBASEAPI SetCommBreak(
	     HANDLE hFile
	     );


extern WINBOOL
 PASCAL WINBASEAPI SetCommConfig(
	      HANDLE hCommDev,
	      LPCOMMCONFIG lpCC,
	      DWORD dwSize
	      );


extern WINBOOL
 PASCAL WINBASEAPI SetCommMask(
	    HANDLE hFile,
	    DWORD dwEvtMask
	    );


extern WINBOOL
 PASCAL WINBASEAPI SetCommState(
	     HANDLE hFile,
	     LPDCB lpDCB
	     );


extern WINBOOL
 PASCAL WINBASEAPI SetCommTimeouts(
		HANDLE hFile,
		LPCOMMTIMEOUTS lpCommTimeouts
		);


extern WINBOOL
 PASCAL WINBASEAPI TransmitCommChar(
		 HANDLE hFile,
		 char cChar
		 );


extern WINBOOL
 PASCAL WINBASEAPI WaitCommEvent(
	      HANDLE hFile,
	      LPDWORD lpEvtMask,
	      LPOVERLAPPED lpOverlapped
	      );



extern DWORD
 PASCAL WINBASEAPI SetTapePosition(
		HANDLE hDevice,
		DWORD dwPositionMethod,
		DWORD dwPartition,
		DWORD dwOffsetLow,
		DWORD dwOffsetHigh,
		WINBOOL bImmediate
		);


extern DWORD
 PASCAL WINBASEAPI GetTapePosition(
		HANDLE hDevice,
		DWORD dwPositionType,
		LPDWORD lpdwPartition,
		LPDWORD lpdwOffsetLow,
		LPDWORD lpdwOffsetHigh
		);


extern DWORD
 PASCAL WINBASEAPI PrepareTape(
	    HANDLE hDevice,
	    DWORD dwOperation,
	    WINBOOL bImmediate
	    );


extern DWORD
 PASCAL WINBASEAPI EraseTape(
	  HANDLE hDevice,
	  DWORD dwEraseType,
	  WINBOOL bImmediate
	  );


extern DWORD
 PASCAL WINBASEAPI CreateTapePartition(
		    HANDLE hDevice,
		    DWORD dwPartitionMethod,
		    DWORD dwCount,
		    DWORD dwSize
		    );


extern DWORD
 PASCAL WINBASEAPI WriteTapemark(
	      HANDLE hDevice,
	      DWORD dwTapemarkType,
	      DWORD dwTapemarkCount,
	      WINBOOL bImmediate
	      );


extern DWORD
 PASCAL WINBASEAPI GetTapeStatus(
	      HANDLE hDevice
	      );


extern DWORD
 PASCAL WINBASEAPI GetTapeParameters(
		  HANDLE hDevice,
		  DWORD dwOperation,
		  LPDWORD lpdwSize,
		  LPVOID lpTapeInformation
		  );

extern DWORD
 PASCAL WINBASEAPI SetTapeParameters(
		  HANDLE hDevice,
		  DWORD dwOperation,
		  LPVOID lpTapeInformation
		  );

extern WINBOOL
 PASCAL WINBASEAPI Beep(
     DWORD dwFreq,
     DWORD dwDuration
     );


extern VOID
 PASCAL WINBASEAPI OpenSound(
	  VOID
	  );


extern VOID
 PASCAL WINBASEAPI CloseSound(
	   VOID
	   );


extern VOID
 PASCAL WINBASEAPI StartSound(
	   VOID
	   );


extern VOID
 PASCAL WINBASEAPI StopSound(
	  VOID
	  );


extern DWORD
 PASCAL WINBASEAPI WaitSoundState(
	       DWORD nState
	       );


extern DWORD
 PASCAL WINBASEAPI SyncAllVoices(
	      VOID
	      );


extern DWORD
 PASCAL WINBASEAPI CountVoiceNotes(
		DWORD nVoice
		);


extern LPDWORD
 PASCAL WINBASEAPI GetThresholdEvent(
		  VOID
		  );


extern DWORD
 PASCAL WINBASEAPI GetThresholdStatus(
		   VOID
		   );


extern DWORD
 PASCAL WINBASEAPI SetSoundNoise(
	      DWORD nSource,
	      DWORD nDuration
	      );


extern DWORD
 PASCAL WINBASEAPI SetVoiceAccent(
	       DWORD nVoice,
	       DWORD nTempo,
	       DWORD nVolume,
	       DWORD nMode,
	       DWORD nPitch
	       );


extern DWORD
 PASCAL WINBASEAPI SetVoiceEnvelope(
		 DWORD nVoice,
		 DWORD nShape,
		 DWORD nRepeat
		 );


extern DWORD
 PASCAL WINBASEAPI SetVoiceNote(
	     DWORD nVoice,
	     DWORD nValue,
	     DWORD nLength,
	     DWORD nCdots
	     );


extern DWORD
 PASCAL WINBASEAPI SetVoiceQueueSize(
		  DWORD nVoice,
		  DWORD nBytes
		  );


extern DWORD
 PASCAL WINBASEAPI SetVoiceSound(
	      DWORD nVoice,
	      DWORD Frequency,
	      DWORD nDuration
	      );


extern DWORD
 PASCAL WINBASEAPI SetVoiceThreshold(
		  DWORD nVoice,
		  DWORD nNotes
		  );


extern int
 PASCAL WINBASEAPI MulDiv(
       int nNumber,
       int nNumerator,
       int nDenominator
       );

extern WINBOOL PASCAL WINBASEAPI SetSystemPowerState (WINBOOL, WINBOOL);

extern VOID
 PASCAL WINBASEAPI GetSystemTime(
	      LPSYSTEMTIME lpSystemTime
	      );


extern WINBOOL
 PASCAL WINBASEAPI SetSystemTime(
	      CONST SYSTEMTIME *lpSystemTime
	      );


extern VOID
 PASCAL WINBASEAPI GetSystemTimeAsFileTime(
          LPFILETIME lpSystemTimeAsFileTime
	      );


extern VOID
 PASCAL WINBASEAPI GetLocalTime(
	     LPSYSTEMTIME lpSystemTime
	     );


extern WINBOOL
 PASCAL WINBASEAPI SetLocalTime(
	     CONST SYSTEMTIME *lpSystemTime
	     );


extern VOID
 PASCAL WINBASEAPI GetSystemInfo(
	      LPSYSTEM_INFO lpSystemInfo
	      );

extern WINBOOL
 PASCAL WINBASEAPI SystemTimeToTzSpecificLocalTime(
				LPTIME_ZONE_INFORMATION lpTimeZoneInformation,
				LPSYSTEMTIME lpUniversalTime,
				LPSYSTEMTIME lpLocalTime
				);


extern DWORD
 PASCAL WINBASEAPI GetTimeZoneInformation(
		       LPTIME_ZONE_INFORMATION lpTimeZoneInformation
		       );


extern WINBOOL
 PASCAL WINBASEAPI SetTimeZoneInformation(
		       CONST TIME_ZONE_INFORMATION *lpTimeZoneInformation
		       );

extern WINBOOL
 PASCAL WINBASEAPI SystemTimeToFileTime(
		     CONST SYSTEMTIME *lpSystemTime,
		     LPFILETIME lpFileTime
		     );


extern WINBOOL
 PASCAL WINBASEAPI FileTimeToLocalFileTime(
			CONST FILETIME *lpFileTime,
			LPFILETIME lpLocalFileTime
			);


extern WINBOOL
 PASCAL WINBASEAPI LocalFileTimeToFileTime(
			CONST FILETIME *lpLocalFileTime,
			LPFILETIME lpFileTime
			);


extern WINBOOL
 PASCAL WINBASEAPI FileTimeToSystemTime(
		     CONST FILETIME *lpFileTime,
		     LPSYSTEMTIME lpSystemTime
		     );


extern LONG
 PASCAL WINBASEAPI CompareFileTime(
		CONST FILETIME *lpFileTime1,
		CONST FILETIME *lpFileTime2
		);


extern WINBOOL
 PASCAL WINBASEAPI FileTimeToDosDateTime(
		      CONST FILETIME *lpFileTime,
		      LPWORD lpFatDate,
		      LPWORD lpFatTime
		      );


extern WINBOOL
 PASCAL WINBASEAPI DosDateTimeToFileTime(
		      WORD wFatDate,
		      WORD wFatTime,
		      LPFILETIME lpFileTime
		      );


extern DWORD
 PASCAL WINBASEAPI GetTickCount(
	     VOID
	     );


extern WINBOOL
 PASCAL WINBASEAPI SetSystemTimeAdjustment(
			DWORD dwTimeAdjustment,
			WINBOOL  bTimeAdjustmentDisabled
			);


extern WINBOOL
 PASCAL WINBASEAPI GetSystemTimeAdjustment(
			PDWORD lpTimeAdjustment,
			PDWORD lpTimeIncrement,
			PWINBOOL  lpTimeAdjustmentDisabled
			);


extern WINBOOL
 PASCAL WINBASEAPI CreatePipe(
	   PHANDLE hReadPipe,
	   PHANDLE hWritePipe,
	   LPSECURITY_ATTRIBUTES lpPipeAttributes,
	   DWORD nSize
	   );


extern WINBOOL
 PASCAL WINBASEAPI ConnectNamedPipe(
		 HANDLE hNamedPipe,
		 LPOVERLAPPED lpOverlapped
		 );


extern WINBOOL
 PASCAL WINBASEAPI DisconnectNamedPipe(
		    HANDLE hNamedPipe
		    );


extern WINBOOL
 PASCAL WINBASEAPI SetNamedPipeHandleState(
			HANDLE hNamedPipe,
			LPDWORD lpMode,
			LPDWORD lpMaxCollectionCount,
			LPDWORD lpCollectDataTimeout
			);


extern WINBOOL
 PASCAL WINBASEAPI GetNamedPipeInfo(
		 HANDLE hNamedPipe,
		 LPDWORD lpFlags,
		 LPDWORD lpOutBufferSize,
		 LPDWORD lpInBufferSize,
		 LPDWORD lpMaxInstances
		 );


extern WINBOOL
 PASCAL WINBASEAPI PeekNamedPipe(
	      HANDLE hNamedPipe,
	      LPVOID lpBuffer,
	      DWORD nBufferSize,
	      LPDWORD lpBytesRead,
	      LPDWORD lpTotalBytesAvail,
	      LPDWORD lpBytesLeftThisMessage
	      );


extern WINBOOL
 PASCAL WINBASEAPI TransactNamedPipe(
		  HANDLE hNamedPipe,
		  LPVOID lpInBuffer,
		  DWORD nInBufferSize,
		  LPVOID lpOutBuffer,
		  DWORD nOutBufferSize,
		  LPDWORD lpBytesRead,
		  LPOVERLAPPED lpOverlapped
		  );



extern WINBOOL
 PASCAL WINBASEAPI GetMailslotInfo(
		HANDLE hMailslot,
		LPDWORD lpMaxMessageSize,
		LPDWORD lpNextSize,
		LPDWORD lpMessageCount,
		LPDWORD lpReadTimeout
		);


extern WINBOOL
 PASCAL WINBASEAPI SetMailslotInfo(
		HANDLE hMailslot,
		DWORD lReadTimeout
		);


extern LPVOID
 PASCAL WINBASEAPI MapViewOfFile(
	      HANDLE hFileMappingObject,
	      DWORD dwDesiredAccess,
	      DWORD dwFileOffsetHigh,
	      DWORD dwFileOffsetLow,
	      DWORD dwNumberOfBytesToMap
	      );


extern WINBOOL
 PASCAL WINBASEAPI FlushViewOfFile(
		LPCVOID lpBaseAddress,
		DWORD dwNumberOfBytesToFlush
		);


extern WINBOOL
 PASCAL WINBASEAPI UnmapViewOfFile(
		LPVOID lpBaseAddress
		);

extern HFILE
 PASCAL WINBASEAPI OpenFile(
	 LPCSTR lpFileName,
	 LPOFSTRUCT lpReOpenBuff,
	 UINT uStyle
	 );


extern HFILE
 PASCAL WINBASEAPI _lopen(
       LPCSTR lpPathName,
       int iReadWrite
       );


extern HFILE
 PASCAL WINBASEAPI _lcreat(
	LPCSTR lpPathName,
	int  iAttribute
	);


extern UINT
 PASCAL WINBASEAPI _lread(
       HFILE hFile,
       LPVOID lpBuffer,
       UINT uBytes
       );


extern UINT
 PASCAL WINBASEAPI _lwrite(
	HFILE hFile,
	LPCSTR lpBuffer,
	UINT uBytes
	);


extern long
 PASCAL WINBASEAPI _hread(
       HFILE hFile,
       LPVOID lpBuffer,
       long lBytes
       );


extern long
 PASCAL WINBASEAPI _hwrite(
	HFILE hFile,
	LPCSTR lpBuffer,
	long lBytes
	);


extern HFILE
 PASCAL WINBASEAPI _lclose(
	HFILE hFile
	);


extern LONG
 PASCAL WINBASEAPI _llseek(
	HFILE hFile,
	LONG lOffset,
	int iOrigin
	);


extern WINBOOL
 PASCAL WINBASEAPI IsTextUnicode(
	      CONST LPVOID lpBuffer,
	      int cb,
	      LPINT lpi
	      );


extern DWORD
 PASCAL WINBASEAPI TlsAlloc(
	 VOID
	 );

extern LPVOID
 PASCAL WINBASEAPI TlsGetValue(
	    DWORD dwTlsIndex
	    );


extern WINBOOL
 PASCAL WINBASEAPI TlsSetValue(
	    DWORD dwTlsIndex,
	    LPVOID lpTlsValue
	    );


extern WINBOOL
 PASCAL WINBASEAPI TlsFree(
	DWORD dwTlsIndex
	);

extern DWORD
 PASCAL WINBASEAPI SleepEx(
	DWORD dwMilliseconds,
	WINBOOL bAlertable
	);


extern DWORD
 PASCAL WINBASEAPI WaitForSingleObjectEx(
		      HANDLE hHandle,
		      DWORD dwMilliseconds,
		      WINBOOL bAlertable
		      );


extern DWORD
 PASCAL WINBASEAPI WaitForMultipleObjectsEx(
			 DWORD nCount,
			 CONST HANDLE *lpHandles,
			 WINBOOL bWaitAll,
			 DWORD dwMilliseconds,
			 WINBOOL bAlertable
			 );


extern WINBOOL
 PASCAL WINBASEAPI ReadFileEx(
	   HANDLE hFile,
	   LPVOID lpBuffer,
	   DWORD nNumberOfBytesToRead,
	   LPOVERLAPPED lpOverlapped,
	   LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
	   );


extern WINBOOL
 PASCAL WINBASEAPI WriteFileEx(
	    HANDLE hFile,
	    LPCVOID lpBuffer,
	    DWORD nNumberOfBytesToWrite,
	    LPOVERLAPPED lpOverlapped,
	    LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
	    );


extern WINBOOL
 PASCAL WINBASEAPI BackupRead(
	   HANDLE hFile,
	   LPBYTE lpBuffer,
	   DWORD nNumberOfBytesToRead,
	   LPDWORD lpNumberOfBytesRead,
	   WINBOOL bAbort,
	   WINBOOL bProcessSecurity,
	   LPVOID *lpContext
	   );


extern WINBOOL
 PASCAL WINBASEAPI BackupSeek(
	   HANDLE hFile,
	   DWORD  dwLowBytesToSeek,
	   DWORD  dwHighBytesToSeek,
	   LPDWORD lpdwLowByteSeeked,
	   LPDWORD lpdwHighByteSeeked,
	   LPVOID *lpContext
	   );


extern WINBOOL
 PASCAL WINBASEAPI BackupWrite(
	    HANDLE hFile,
	    LPBYTE lpBuffer,
	    DWORD nNumberOfBytesToWrite,
	    LPDWORD lpNumberOfBytesWritten,
	    WINBOOL bAbort,
	    WINBOOL bProcessSecurity,
	    LPVOID *lpContext
	    );

extern WINBOOL
 PASCAL WINBASEAPI SetProcessShutdownParameters(
			     DWORD dwLevel,
			     DWORD dwFlags
			     );


extern WINBOOL
 PASCAL WINBASEAPI GetProcessShutdownParameters(
			     LPDWORD lpdwLevel,
			     LPDWORD lpdwFlags
			     );


extern VOID
 PASCAL WINBASEAPI SetFileApisToOEM( VOID );


extern VOID
 PASCAL WINBASEAPI SetFileApisToANSI( VOID );


extern WINBOOL
 PASCAL WINBASEAPI AreFileApisANSI( VOID );

extern WINBOOL
 PASCAL WINBASEAPI CloseEventLog(
	       HANDLE hEventLog
		);


extern WINBOOL
 PASCAL WINBASEAPI DeregisterEventSource(
		       HANDLE hEventLog
			);


extern WINBOOL
 PASCAL WINBASEAPI NotifyChangeEventLog(
		      HANDLE hEventLog,
		      HANDLE hEvent
		       );


extern WINBOOL
 PASCAL WINBASEAPI GetNumberOfEventLogRecords(
			    HANDLE hEventLog,
			    PDWORD NumberOfRecords
			     );


extern WINBOOL
 PASCAL WINBASEAPI GetOldestEventLogRecord(
			 HANDLE hEventLog,
			 PDWORD OldestRecord
			  );

extern WINBOOL
 PASCAL WINBASEAPI DuplicateToken(
	       HANDLE ExistingTokenHandle,
	       SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,
	       PHANDLE DuplicateTokenHandle
	       );


extern WINBOOL
 PASCAL WINBASEAPI GetKernelObjectSecurity(
			 HANDLE Handle,
			 SECURITY_INFORMATION RequestedInformation,
			 PSECURITY_DESCRIPTOR pSecurityDescriptor,
			 DWORD nLength,
			 LPDWORD lpnLengthNeeded
			  );


extern WINBOOL
 PASCAL WINBASEAPI ImpersonateNamedPipeClient(
			   HANDLE hNamedPipe
			   );


extern WINBOOL
 PASCAL WINBASEAPI ImpersonateSelf(
		SECURITY_IMPERSONATION_LEVEL ImpersonationLevel
		);



extern WINBOOL
 PASCAL WINBASEAPI RevertToSelf(
	      VOID
	       );


extern WINBOOL
 PASCAL WINBASEAPI SetThreadToken(
		PHANDLE Thread,
		HANDLE Token
		 );


extern WINBOOL
 PASCAL WINBASEAPI AccessCheck(
	     PSECURITY_DESCRIPTOR pSecurityDescriptor,
	     HANDLE ClientToken,
	     DWORD DesiredAccess,
	     PGENERIC_MAPPING GenericMapping,
	     PPRIVILEGE_SET PrivilegeSet,
	     LPDWORD PrivilegeSetLength,
	     LPDWORD GrantedAccess,
	     LPBOOL AccessStatus
	      );



extern WINBOOL
 PASCAL WINBASEAPI OpenProcessToken(
		  HANDLE ProcessHandle,
		  DWORD DesiredAccess,
		  PHANDLE TokenHandle
		   );



extern WINBOOL
 PASCAL WINBASEAPI OpenThreadToken(
		 HANDLE ThreadHandle,
		 DWORD DesiredAccess,
		 WINBOOL OpenAsSelf,
		 PHANDLE TokenHandle
		  );



extern WINBOOL
 PASCAL WINBASEAPI GetTokenInformation(
		     HANDLE TokenHandle,
		     TOKEN_INFORMATION_CLASS TokenInformationClass,
		     LPVOID TokenInformation,
		     DWORD TokenInformationLength,
		     PDWORD ReturnLength
		      );



extern WINBOOL
 PASCAL WINBASEAPI SetTokenInformation(
		     HANDLE TokenHandle,
		     TOKEN_INFORMATION_CLASS TokenInformationClass,
		     LPVOID TokenInformation,
		     DWORD TokenInformationLength
		      );



extern WINBOOL
 PASCAL WINBASEAPI AdjustTokenPrivileges(
		       HANDLE TokenHandle,
		       WINBOOL DisableAllPrivileges,
		       PTOKEN_PRIVILEGES NewState,
		       DWORD BufferLength,
		       PTOKEN_PRIVILEGES PreviousState,
		       PDWORD ReturnLength
			);



extern WINBOOL
 PASCAL WINBASEAPI AdjustTokenGroups(
		   HANDLE TokenHandle,
		   WINBOOL ResetToDefault,
		   PTOKEN_GROUPS NewState,
		   DWORD BufferLength,
		   PTOKEN_GROUPS PreviousState,
		   PDWORD ReturnLength
		    );



extern WINBOOL
 PASCAL WINBASEAPI PrivilegeCheck(
		HANDLE ClientToken,
		PPRIVILEGE_SET RequiredPrivileges,
		LPBOOL pfResult
		 );



extern WINBOOL
 PASCAL WINBASEAPI IsValidSid(
	    PSID pSid
	     );



extern WINBOOL
 PASCAL WINBASEAPI EqualSid(
	  PSID pSid1,
	  PSID pSid2
	   );



extern WINBOOL
 PASCAL WINBASEAPI EqualPrefixSid(
		PSID pSid1,
		PSID pSid2
		 );



extern DWORD
 PASCAL WINBASEAPI GetSidLengthRequired(
		      UCHAR nSubAuthorityCount
		       );



extern WINBOOL
 PASCAL WINBASEAPI AllocateAndInitializeSid(
			  PSID_IDENTIFIER_AUTHORITY pIdentifierAuthority,
			  BYTE nSubAuthorityCount,
			  DWORD nSubAuthority0,
			  DWORD nSubAuthority1,
			  DWORD nSubAuthority2,
			  DWORD nSubAuthority3,
			  DWORD nSubAuthority4,
			  DWORD nSubAuthority5,
			  DWORD nSubAuthority6,
			  DWORD nSubAuthority7,
			  PSID *pSid
			   );


extern PVOID
 PASCAL WINBASEAPI FreeSid(
	PSID pSid
	);


extern WINBOOL
 PASCAL WINBASEAPI InitializeSid(
	       PSID Sid,
	       PSID_IDENTIFIER_AUTHORITY pIdentifierAuthority,
	       BYTE nSubAuthorityCount
		);



extern PSID_IDENTIFIER_AUTHORITY
 PASCAL WINBASEAPI GetSidIdentifierAuthority(
			   PSID pSid
			    );



extern PDWORD
 PASCAL WINBASEAPI GetSidSubAuthority(
		    PSID pSid,
		    DWORD nSubAuthority
		     );



extern PUCHAR
 PASCAL WINBASEAPI GetSidSubAuthorityCount(
			 PSID pSid
			  );



extern DWORD
 PASCAL WINBASEAPI GetLengthSid(
	      PSID pSid
	       );



extern WINBOOL
 PASCAL WINBASEAPI CopySid(
	 DWORD nDestinationSidLength,
	 PSID pDestinationSid,
	 PSID pSourceSid
	  );



extern WINBOOL
 PASCAL WINBASEAPI AreAllAccessesGranted(
		       DWORD GrantedAccess,
		       DWORD DesiredAccess
			);



extern WINBOOL
 PASCAL WINBASEAPI AreAnyAccessesGranted(
		       DWORD GrantedAccess,
		       DWORD DesiredAccess
			);



extern VOID
 PASCAL WINBASEAPI MapGenericMask(
		PDWORD AccessMask,
		PGENERIC_MAPPING GenericMapping
		 );



extern WINBOOL
 PASCAL WINBASEAPI IsValidAcl(
	    PACL pAcl
	     );



extern WINBOOL
 PASCAL WINBASEAPI InitializeAcl(
	       PACL pAcl,
	       DWORD nAclLength,
	       DWORD dwAclRevision
		);



extern WINBOOL
 PASCAL WINBASEAPI GetAclInformation(
		   PACL pAcl,
		   LPVOID pAclInformation,
		   DWORD nAclInformationLength,
		   ACL_INFORMATION_CLASS dwAclInformationClass
		    );



extern WINBOOL
 PASCAL WINBASEAPI SetAclInformation(
		   PACL pAcl,
		   LPVOID pAclInformation,
		   DWORD nAclInformationLength,
		   ACL_INFORMATION_CLASS dwAclInformationClass
		    );



extern WINBOOL
 PASCAL WINBASEAPI AddAce(
	PACL pAcl,
	DWORD dwAceRevision,
	DWORD dwStartingAceIndex,
	LPVOID pAceList,
	DWORD nAceListLength
	 );



extern WINBOOL
 PASCAL WINBASEAPI DeleteAce(
	   PACL pAcl,
	   DWORD dwAceIndex
	    );



extern WINBOOL
 PASCAL WINBASEAPI GetAce(
	PACL pAcl,
	DWORD dwAceIndex,
	LPVOID *pAce
	 );



extern WINBOOL
 PASCAL WINBASEAPI AddAccessAllowedAce(
		     PACL pAcl,
		     DWORD dwAceRevision,
		     DWORD AccessMask,
		     PSID pSid
		      );



extern WINBOOL
 PASCAL WINBASEAPI AddAccessDeniedAce(
		    PACL pAcl,
		    DWORD dwAceRevision,
		    DWORD AccessMask,
		    PSID pSid
		     );



extern WINBOOL
 PASCAL WINBASEAPI AddAuditAccessAce(
		  PACL pAcl,
		  DWORD dwAceRevision,
		  DWORD dwAccessMask,
		  PSID pSid,
		  WINBOOL bAuditSuccess,
		  WINBOOL bAuditFailure
		  );



extern WINBOOL
 PASCAL WINBASEAPI FindFirstFreeAce(
		  PACL pAcl,
		  LPVOID *pAce
		   );



extern WINBOOL
 PASCAL WINBASEAPI InitializeSecurityDescriptor(
			      PSECURITY_DESCRIPTOR pSecurityDescriptor,
			      DWORD dwRevision
			       );



extern WINBOOL
 PASCAL WINBASEAPI IsValidSecurityDescriptor(
			   PSECURITY_DESCRIPTOR pSecurityDescriptor
			    );



extern DWORD
 PASCAL WINBASEAPI GetSecurityDescriptorLength(
			     PSECURITY_DESCRIPTOR pSecurityDescriptor
			      );



extern WINBOOL
 PASCAL WINBASEAPI GetSecurityDescriptorControl(
			      PSECURITY_DESCRIPTOR pSecurityDescriptor,
			      PSECURITY_DESCRIPTOR_CONTROL pControl,
			      LPDWORD lpdwRevision
			       );



extern WINBOOL
 PASCAL WINBASEAPI SetSecurityDescriptorDacl(
			   PSECURITY_DESCRIPTOR pSecurityDescriptor,
			   WINBOOL bDaclPresent,
			   PACL pDacl,
			   WINBOOL bDaclDefaulted
			    );



extern WINBOOL
 PASCAL WINBASEAPI GetSecurityDescriptorDacl(
			   PSECURITY_DESCRIPTOR pSecurityDescriptor,
			   LPBOOL lpbDaclPresent,
			   PACL *pDacl,
			   LPBOOL lpbDaclDefaulted
			    );



extern WINBOOL
 PASCAL WINBASEAPI SetSecurityDescriptorSacl(
			   PSECURITY_DESCRIPTOR pSecurityDescriptor,
			   WINBOOL bSaclPresent,
			   PACL pSacl,
			   WINBOOL bSaclDefaulted
			    );



extern WINBOOL
 PASCAL WINBASEAPI GetSecurityDescriptorSacl(
			   PSECURITY_DESCRIPTOR pSecurityDescriptor,
			   LPBOOL lpbSaclPresent,
			   PACL *pSacl,
			   LPBOOL lpbSaclDefaulted
			    );



extern WINBOOL
 PASCAL WINBASEAPI SetSecurityDescriptorOwner(
			    PSECURITY_DESCRIPTOR pSecurityDescriptor,
			    PSID pOwner,
			    WINBOOL bOwnerDefaulted
			     );



extern WINBOOL
 PASCAL WINBASEAPI GetSecurityDescriptorOwner(
			    PSECURITY_DESCRIPTOR pSecurityDescriptor,
			    PSID *pOwner,
			    LPBOOL lpbOwnerDefaulted
			     );



extern WINBOOL
 PASCAL WINBASEAPI SetSecurityDescriptorGroup(
			    PSECURITY_DESCRIPTOR pSecurityDescriptor,
			    PSID pGroup,
			    WINBOOL bGroupDefaulted
			     );



extern WINBOOL
 PASCAL WINBASEAPI GetSecurityDescriptorGroup(
			    PSECURITY_DESCRIPTOR pSecurityDescriptor,
			    PSID *pGroup,
			    LPBOOL lpbGroupDefaulted
			     );



extern WINBOOL
 PASCAL WINBASEAPI CreatePrivateObjectSecurity(
			     PSECURITY_DESCRIPTOR ParentDescriptor,
			     PSECURITY_DESCRIPTOR CreatorDescriptor,
			     PSECURITY_DESCRIPTOR * NewDescriptor,
			     WINBOOL IsDirectoryObject,
			     HANDLE Token,
			     PGENERIC_MAPPING GenericMapping
			      );



extern WINBOOL
 PASCAL WINBASEAPI SetPrivateObjectSecurity(
			  SECURITY_INFORMATION SecurityInformation,
			  PSECURITY_DESCRIPTOR ModificationDescriptor,
			  PSECURITY_DESCRIPTOR *ObjectsSecurityDescriptor,
			  PGENERIC_MAPPING GenericMapping,
			  HANDLE Token
			   );



extern WINBOOL
 PASCAL WINBASEAPI GetPrivateObjectSecurity(
			  PSECURITY_DESCRIPTOR ObjectDescriptor,
			  SECURITY_INFORMATION SecurityInformation,
			  PSECURITY_DESCRIPTOR ResultantDescriptor,
			  DWORD DescriptorLength,
			  PDWORD ReturnLength
			   );



extern WINBOOL
 PASCAL WINBASEAPI DestroyPrivateObjectSecurity(
			      PSECURITY_DESCRIPTOR * ObjectDescriptor
			       );



extern WINBOOL
 PASCAL WINBASEAPI MakeSelfRelativeSD(
		    PSECURITY_DESCRIPTOR pAbsoluteSecurityDescriptor,
		    PSECURITY_DESCRIPTOR pSelfRelativeSecurityDescriptor,
		    LPDWORD lpdwBufferLength
		     );



extern WINBOOL
 PASCAL WINBASEAPI MakeAbsoluteSD(
		PSECURITY_DESCRIPTOR pSelfRelativeSecurityDescriptor,
		PSECURITY_DESCRIPTOR pAbsoluteSecurityDescriptor,
		LPDWORD lpdwAbsoluteSecurityDescriptorSize,
		PACL pDacl,
		LPDWORD lpdwDaclSize,
		PACL pSacl,
		LPDWORD lpdwSaclSize,
		PSID pOwner,
		LPDWORD lpdwOwnerSize,
		PSID pPrimaryGroup,
		LPDWORD lpdwPrimaryGroupSize
		 );



extern WINBOOL
 PASCAL WINBASEAPI SetKernelObjectSecurity(
			 HANDLE Handle,
			 SECURITY_INFORMATION SecurityInformation,
			 PSECURITY_DESCRIPTOR SecurityDescriptor
			  );


extern WINBOOL
 PASCAL WINBASEAPI FindNextChangeNotification(
			   HANDLE hChangeHandle
			   );


extern WINBOOL
 PASCAL WINBASEAPI FindCloseChangeNotification(
			    HANDLE hChangeHandle
			    );


extern WINBOOL
 PASCAL WINBASEAPI VirtualLock(
	    LPVOID lpAddress,
	    DWORD dwSize
	    );


extern WINBOOL
 PASCAL WINBASEAPI VirtualUnlock(
	      LPVOID lpAddress,
	      DWORD dwSize
	      );


extern LPVOID
 PASCAL WINBASEAPI MapViewOfFileEx(
		HANDLE hFileMappingObject,
		DWORD dwDesiredAccess,
		DWORD dwFileOffsetHigh,
		DWORD dwFileOffsetLow,
		DWORD dwNumberOfBytesToMap,
		LPVOID lpBaseAddress
		);


extern WINBOOL
 PASCAL WINBASEAPI SetPriorityClass(
		 HANDLE hProcess,
		 DWORD dwPriorityClass
		 );


extern DWORD
 PASCAL WINBASEAPI GetPriorityClass(
		 HANDLE hProcess
		 );


extern WINBOOL
 PASCAL WINBASEAPI IsBadReadPtr(
	     CONST VOID *lp,
	     UINT ucb
	     );


extern WINBOOL
 PASCAL WINBASEAPI IsBadWritePtr(
	      LPVOID lp,
	      UINT ucb
	      );


extern WINBOOL
 PASCAL WINBASEAPI IsBadHugeReadPtr(
		 CONST VOID *lp,
		 UINT ucb
		 );


extern WINBOOL
 PASCAL WINBASEAPI IsBadHugeWritePtr(
		  LPVOID lp,
		  UINT ucb
		  );


extern WINBOOL
 PASCAL WINBASEAPI IsBadCodePtr(
	     FARPROC lpfn
	     );

extern WINBOOL
 PASCAL WINBASEAPI AllocateLocallyUniqueId(
			PLUID Luid
			);


extern WINBOOL
 PASCAL WINBASEAPI QueryPerformanceCounter(
			LARGE_INTEGER *lpPerformanceCount
			);


extern WINBOOL
 PASCAL WINBASEAPI QueryPerformanceFrequency(
			  LARGE_INTEGER *lpFrequency
			  );

#if 0
extern VOID
 PASCAL WINBASEAPI MoveMemory(
	    PVOID Destination,
	    CONST VOID *Source,
	    DWORD Length
	     );

extern VOID
 PASCAL WINBASEAPI FillMemory(
	    PVOID Destination,
	    DWORD Length,
	    BYTE  Fill
	     );

extern VOID
 PASCAL WINBASEAPI ZeroMemory(
	    PVOID Destination,
	    DWORD Length
	     );
#endif

#ifdef WIN95
extern HKL
 PASCAL WINBASEAPI ActivateKeyboardLayout(
		       HKL hkl,
		       UINT Flags);
#else
extern WINBOOL
 PASCAL WINBASEAPI ActivateKeyboardLayout(
		       HKL hkl,
		       UINT Flags);
#endif /* WIN95 */

 
extern int
 PASCAL WINBASEAPI ToUnicodeEx(
	    UINT wVirtKey,
	    UINT wScanCode,
	    PBYTE lpKeyState,
	    LPWSTR pwszBuff,
	    int cchBuff,
	    UINT wFlags,
	    HKL dwhkl);

 
extern WINBOOL
 PASCAL WINBASEAPI UnloadKeyboardLayout(
		     HKL hkl);

 
extern int
 PASCAL WINBASEAPI GetKeyboardLayoutList(
		      int nBuff,
		      HKL *lpList);

 
extern HKL
 PASCAL WINBASEAPI GetKeyboardLayout(
		  DWORD dwLayout
		  );

 
extern HDESK
 PASCAL WINBASEAPI OpenInputDesktop(
		 DWORD dwFlags,
		 WINBOOL fInherit,
		 DWORD dwDesiredAccess);

extern WINBOOL
 PASCAL WINBASEAPI EnumDesktopWindows(
		   HDESK hDesktop,
		   WNDENUMPROC lpfn,
		   LPARAM lParam);

 
extern WINBOOL
 PASCAL WINBASEAPI SwitchDesktop(
	      HDESK hDesktop);

 
extern WINBOOL
 PASCAL WINBASEAPI SetThreadDesktop(
		 HDESK hDesktop);

 
extern WINBOOL
 PASCAL WINBASEAPI CloseDesktop(
	     HDESK hDesktop);

 
extern HDESK
 PASCAL WINBASEAPI GetThreadDesktop(
		 DWORD dwThreadId);

 
extern WINBOOL
 PASCAL WINBASEAPI CloseWindowStation(
		   HWINSTA hWinSta);

 
extern WINBOOL
 PASCAL WINBASEAPI SetProcessWindowStation(
			HWINSTA hWinSta);

 
extern HWINSTA
 PASCAL WINBASEAPI GetProcessWindowStation(
			VOID);

 
extern WINBOOL
 PASCAL WINBASEAPI SetUserObjectSecurity(
		      HANDLE hObj,
		      PSECURITY_INFORMATION pSIRequested,
		      PSECURITY_DESCRIPTOR pSID);

 
extern WINBOOL
 PASCAL WINBASEAPI GetUserObjectSecurity(
		      HANDLE hObj,
		      PSECURITY_INFORMATION pSIRequested,
		      PSECURITY_DESCRIPTOR pSID,
		      DWORD nLength,
		      LPDWORD lpnLengthNeeded);

 
extern WINBOOL
 PASCAL WINBASEAPI TranslateMessage(
		 CONST MSG *lpMsg);

extern WINBOOL
 PASCAL WINBASEAPI SetMessageQueue(
		int cMessagesMax);

extern WINBOOL
 PASCAL WINBASEAPI RegisterHotKey(
	       HWND hWnd ,
	       int anID,
	       UINT fsModifiers,
	       UINT vk);

 
extern WINBOOL
 PASCAL WINBASEAPI UnregisterHotKey(
		 HWND hWnd,
		 int anID);

 
extern WINBOOL
 PASCAL WINBASEAPI ExitWindowsEx(
	      UINT uFlags,
	      DWORD dwReserved);

 
extern WINBOOL
 PASCAL WINBASEAPI SwapMouseButton(
		WINBOOL fSwap);

 
extern DWORD
 PASCAL WINBASEAPI GetMessagePos(
	      VOID);

 
extern LONG
 PASCAL WINBASEAPI GetMessageTime(
	       VOID);

 
extern LONG
 PASCAL WINBASEAPI GetMessageExtraInfo(
		    VOID);

 
extern LPARAM
 PASCAL WINBASEAPI SetMessageExtraInfo(
		    LPARAM lParam);

 
extern long  
   PASCAL WINBASEAPI BroadcastSystemMessage(
		       DWORD, 
		       LPDWORD, 
		       UINT, 
		       WPARAM, 
		       LPARAM);

extern WINBOOL
 PASCAL WINBASEAPI AttachThreadInput(
		  DWORD idAttach,
		  DWORD idAttachTo,
		  WINBOOL fAttach);

 
extern WINBOOL
 PASCAL WINBASEAPI ReplyMessage(
	     LRESULT lResult);

 
extern WINBOOL
 PASCAL WINBASEAPI WaitMessage(
	    VOID);

 
extern DWORD
 PASCAL WINBASEAPI WaitForInputIdle(
		 HANDLE hProcess,
		 DWORD dwMilliseconds);

 
extern VOID
 PASCAL WINBASEAPI PostQuitMessage(
		int nExitCode);

extern WINBOOL
 PASCAL WINBASEAPI InSendMessage(
	      VOID);

 
extern UINT
 PASCAL WINBASEAPI GetDoubleClickTime(
		   VOID);

 
extern WINBOOL
 PASCAL WINBASEAPI SetDoubleClickTime(
		   UINT);

 
extern WINBOOL
 PASCAL WINBASEAPI IsWindow(
	 HWND hWnd);

 
extern WINBOOL
 PASCAL WINBASEAPI IsMenu(
       HMENU hMenu);

 
extern WINBOOL
 PASCAL WINBASEAPI IsChild(
	HWND hWndParent,
	HWND hWnd);

 
extern WINBOOL
 PASCAL WINBASEAPI DestroyWindow(
	      HWND hWnd);

 
extern WINBOOL
 PASCAL WINBASEAPI ShowWindow(
	   HWND hWnd,
	   int nCmdShow);

 
extern WINBOOL
 PASCAL WINBASEAPI ShowWindowAsync(
		HWND hWnd,
		int nCmdShow);

 
extern WINBOOL
 PASCAL WINBASEAPI FlashWindow(
	    HWND hWnd,
	    WINBOOL bInvert);

 
extern WINBOOL
 PASCAL WINBASEAPI ShowOwnedPopups(
		HWND hWnd,
		WINBOOL fShow);

 
extern WINBOOL
 PASCAL WINBASEAPI OpenIcon(
	 HWND hWnd);

 
extern WINBOOL
 PASCAL WINBASEAPI CloseWindow(
	    HWND hWnd);

 
extern WINBOOL
 PASCAL WINBASEAPI MoveWindow(
	   HWND hWnd,
	   int X,
	   int Y,
	   int nWidth,
	   int nHeight,
	   WINBOOL bRepaint);

 
extern WINBOOL
 PASCAL WINBASEAPI SetWindowPos(
	     HWND hWnd,
	     HWND hWndInsertAfter ,
	     int X,
	     int Y,
	     int cx,
	     int cy,
	     UINT uFlags);

 
extern WINBOOL
 PASCAL WINBASEAPI GetWindowPlacement(
		   HWND hWnd,
		   WINDOWPLACEMENT *lpwndpl);

 
extern WINBOOL
 PASCAL WINBASEAPI SetWindowPlacement(
		   HWND hWnd,
		   CONST WINDOWPLACEMENT *lpwndpl);

 
extern HDWP
 PASCAL WINBASEAPI BeginDeferWindowPos(
		    int nNumWindows);

 
extern HDWP
 PASCAL WINBASEAPI DeferWindowPos(
	       HDWP hWinPosInfo,
	       HWND hWnd,
	       HWND hWndInsertAfter,
	       int x,
	       int y,
	       int cx,
	       int cy,
	       UINT uFlags);

 
extern WINBOOL
 PASCAL WINBASEAPI EndDeferWindowPos(
		  HDWP hWinPosInfo);

 
extern WINBOOL
 PASCAL WINBASEAPI IsWindowVisible(
		HWND hWnd);

 
extern WINBOOL
 PASCAL WINBASEAPI IsIconic(
	 HWND hWnd);

 
extern WINBOOL
 PASCAL WINBASEAPI AnyPopup(
	 VOID);

 
extern WINBOOL
 PASCAL WINBASEAPI BringWindowToTop(
		 HWND hWnd);

 
extern WINBOOL
 PASCAL WINBASEAPI IsZoomed(
	 HWND hWnd);

 
extern WINBOOL
 PASCAL WINBASEAPI EndDialog(
	  HWND hDlg,
	  int nResult);

 
extern HWND
 PASCAL WINBASEAPI GetDlgItem(
	   HWND hDlg,
	   int nIDDlgItem);

 
extern WINBOOL
 PASCAL WINBASEAPI SetDlgItemInt(
	      HWND hDlg,
	      int nIDDlgItem,
	      UINT uValue,
	      WINBOOL bSigned);

 
extern UINT
 PASCAL WINBASEAPI GetDlgItemInt(
	      HWND hDlg,
	      int nIDDlgItem,
	      WINBOOL *lpTranslated,
	      WINBOOL bSigned);

 
extern WINBOOL
 PASCAL WINBASEAPI CheckDlgButton(
	       HWND hDlg,
	       int nIDButton,
	       UINT uCheck);

 
extern WINBOOL
 PASCAL WINBASEAPI CheckRadioButton(
		 HWND hDlg,
		 int nIDFirstButton,
		 int nIDLastButton,
		 int nIDCheckButton);

 
extern UINT
 PASCAL WINBASEAPI IsDlgButtonChecked(
		   HWND hDlg,
		   int nIDButton);

 
extern HWND
 PASCAL WINBASEAPI GetNextDlgGroupItem(
		    HWND hDlg,
		    HWND hCtl,
		    WINBOOL bPrevious);

 
extern HWND
 PASCAL WINBASEAPI GetNextDlgTabItem(
		  HWND hDlg,
		  HWND hCtl,
		  WINBOOL bPrevious);

 
extern int
 PASCAL WINBASEAPI GetDlgCtrlID(
	     HWND hWnd);

 
extern long
 PASCAL WINBASEAPI GetDialogBaseUnits(VOID);

extern WINBOOL
 PASCAL WINBASEAPI OpenClipboard(
	      HWND hWndNewOwner);

 
extern WINBOOL
 PASCAL WINBASEAPI CloseClipboard(
	       VOID);

 
extern HWND
 PASCAL WINBASEAPI GetClipboardOwner(
		  VOID);

 
extern HWND
 PASCAL WINBASEAPI SetClipboardViewer(
		   HWND hWndNewViewer);

 
extern HWND
 PASCAL WINBASEAPI GetClipboardViewer(
		   VOID);

 
extern WINBOOL
 PASCAL WINBASEAPI ChangeClipboardChain(
		     HWND hWndRemove,
		     HWND hWndNewNext);

 
extern HANDLE
 PASCAL WINBASEAPI SetClipboardData(
		 UINT uFormat,
		 HANDLE hMem);

 
extern HANDLE
 PASCAL WINBASEAPI GetClipboardData(
		 UINT uFormat);

 
 
extern int
 PASCAL WINBASEAPI CountClipboardFormats(
		      VOID);

 
extern UINT
 PASCAL WINBASEAPI EnumClipboardFormats(
		     UINT format);

 
extern WINBOOL
 PASCAL WINBASEAPI EmptyClipboard(
	       VOID);

 
extern WINBOOL
 PASCAL WINBASEAPI IsClipboardFormatAvailable(
			   UINT format);

 
extern int
 PASCAL WINBASEAPI GetPriorityClipboardFormat(
			   UINT *paFormatPriorityList,
			   int cFormats);

 
extern HWND
 PASCAL WINBASEAPI GetOpenClipboardWindow(
		       VOID);

 
/* Despite the A these are ASCII functions! */
extern LPSTR
 PASCAL WINBASEAPI CharNextExA(
	    WORD CodePage,
	    LPCSTR lpCurrentChar,
	    DWORD dwFlags);

 
extern LPSTR
 PASCAL WINBASEAPI CharPrevExA(
	    WORD CodePage,
	    LPCSTR lpStart,
	    LPCSTR lpCurrentChar,
	    DWORD dwFlags);

 
extern HWND
 PASCAL WINBASEAPI SetFocus(
	 HWND hWnd);

 
extern HWND
 PASCAL WINBASEAPI GetActiveWindow(
		VOID);

 
extern HWND
 PASCAL WINBASEAPI GetFocus(
	 VOID);

 
extern UINT
 PASCAL WINBASEAPI GetKBCodePage(
	      VOID);

 
extern SHORT
 PASCAL WINBASEAPI GetKeyState(
	    int nVirtKey);

 
extern SHORT
 PASCAL WINBASEAPI GetAsyncKeyState(
		 int vKey);

 
extern WINBOOL
 PASCAL WINBASEAPI GetKeyboardState(
		 PBYTE lpKeyState);

 
extern WINBOOL
 PASCAL WINBASEAPI SetKeyboardState(
		 LPBYTE lpKeyState);

 
extern int
 PASCAL WINBASEAPI GetKeyboardType(
		int nTypeFlag);

 
extern int
 PASCAL WINBASEAPI ToAscii(
	UINT uVirtKey,
	UINT uScanCode,
	PBYTE lpKeyState,
	LPWORD lpChar,
	UINT uFlags);

 
extern int
 PASCAL WINBASEAPI ToAsciiEx(
	  UINT uVirtKey,
	  UINT uScanCode,
	  PBYTE lpKeyState,
	  LPWORD lpChar,
	  UINT uFlags,
	  HKL dwhkl);

 
extern int
 PASCAL WINBASEAPI ToUnicode(
	  UINT wVirtKey,
	  UINT wScanCode,
	  PBYTE lpKeyState,
	  LPWSTR pwszBuff,
	  int cchBuff,
	  UINT wFlags);

 
extern DWORD
 PASCAL WINBASEAPI OemKeyScan(
	   WORD wOemChar);

 
extern VOID
 PASCAL WINBASEAPI keybd_event(
	    BYTE bVk,
	    BYTE bScan,
	    DWORD dwFlags,
	    DWORD dwExtraInfo);

 
extern VOID
 PASCAL WINBASEAPI mouse_event(
	    DWORD dwFlags,
	    DWORD dx,
	    DWORD dy,
	    DWORD cButtons,
	    DWORD dwExtraInfo);

extern WINBOOL
 PASCAL WINBASEAPI GetInputState(
	      VOID);

 
extern DWORD
 PASCAL WINBASEAPI GetQueueStatus(
	       UINT flags);

 
extern HWND
 PASCAL WINBASEAPI GetCapture(
	   VOID);

 
extern HWND
 PASCAL WINBASEAPI SetCapture(
	   HWND hWnd);

 
extern WINBOOL
 PASCAL WINBASEAPI ReleaseCapture(
	       VOID);

 
extern DWORD
 PASCAL WINBASEAPI MsgWaitForMultipleObjects(
			  DWORD nCount,
			  LPHANDLE pHandles,
			  WINBOOL fWaitAll,
			  DWORD dwMilliseconds,
			  DWORD dwWakeMask);

 
extern UINT
 PASCAL WINBASEAPI SetTimer(
	 HWND hWnd ,
	 UINT nIDEvent,
	 UINT uElapse,
	 TIMERPROC lpTimerFunc);

 
extern WINBOOL
 PASCAL WINBASEAPI KillTimer(
	  HWND hWnd,
	  UINT uIDEvent);

 
extern WINBOOL
 PASCAL WINBASEAPI IsWindowUnicode(
		HWND hWnd);

 
extern WINBOOL
 PASCAL WINBASEAPI EnableWindow(
	     HWND hWnd,
	     WINBOOL bEnable);

 
extern WINBOOL
 PASCAL WINBASEAPI IsWindowEnabled(
		HWND hWnd);

 
extern WINBOOL
 PASCAL WINBASEAPI DestroyAcceleratorTable(
			HACCEL hAccel);

extern int
 PASCAL WINBASEAPI GetSystemMetrics(
		 int nIndex);

extern HMENU
 PASCAL WINBASEAPI GetMenu(
	HWND hWnd);

 
extern WINBOOL
 PASCAL WINBASEAPI SetMenu(
	HWND hWnd,
	HMENU hMenu);

 
extern WINBOOL
 PASCAL WINBASEAPI HiliteMenuItem(
	       HWND hWnd,
	       HMENU hMenu,
	       UINT uIDHiliteItem,
	       UINT uHilite);

 
extern UINT
 PASCAL WINBASEAPI GetMenuState(
	     HMENU hMenu,
	     UINT uId,
	     UINT uFlags);

 
extern WINBOOL
 PASCAL WINBASEAPI DrawMenuBar(
	    HWND hWnd);

 
extern HMENU
 PASCAL WINBASEAPI GetSystemMenu(
	      HWND hWnd,
	      WINBOOL bRevert);

 
extern HMENU
 PASCAL WINBASEAPI CreateMenu(
	   VOID);

 
extern HMENU
 PASCAL WINBASEAPI CreatePopupMenu(
		VOID);

 
extern WINBOOL
 PASCAL WINBASEAPI DestroyMenu(
	    HMENU hMenu);

 
extern DWORD
 PASCAL WINBASEAPI CheckMenuItem(
	      HMENU hMenu,
	      UINT uIDCheckItem,
	      UINT uCheck);

 
extern WINBOOL
 PASCAL WINBASEAPI EnableMenuItem(
	       HMENU hMenu,
	       UINT uIDEnableItem,
	       UINT uEnable);

 
extern HMENU
 PASCAL WINBASEAPI GetSubMenu(
	   HMENU hMenu,
	   int nPos);

 
extern UINT
 PASCAL WINBASEAPI GetMenuItemID(
	      HMENU hMenu,
	      int nPos);

 
extern int
 PASCAL WINBASEAPI GetMenuItemCount(
		 HMENU hMenu);

extern WINBOOL
 PASCAL WINBASEAPI RemoveMenu(
		   HMENU hMenu,
		   UINT uPosition,
		   UINT uFlags);

 
extern WINBOOL
 PASCAL WINBASEAPI DeleteMenu(
	   HMENU hMenu,
	   UINT uPosition,
	   UINT uFlags);

 
extern WINBOOL
 PASCAL WINBASEAPI SetMenuItemBitmaps(
		   HMENU hMenu,
		   UINT uPosition,
		   UINT uFlags,
		   HBITMAP hBitmapUnchecked,
		   HBITMAP hBitmapChecked);

 
extern LONG
 PASCAL WINBASEAPI GetMenuCheckMarkDimensions(
			   VOID);

 
extern WINBOOL
 PASCAL WINBASEAPI TrackPopupMenu(
	       HMENU hMenu,
	       UINT uFlags,
	       int x,
	       int y,
	       int nReserved,
	       HWND hWnd,
	       CONST RECT *prcRect);

extern UINT
 PASCAL WINBASEAPI GetMenuDefaultItem(
		   HMENU hMenu, 
		   UINT fByPos, 
		   UINT gmdiFlags);

extern WINBOOL
 PASCAL WINBASEAPI SetMenuDefaultItem(
		   HMENU hMenu, 
		   UINT uItem, 
		   UINT fByPos);

extern WINBOOL
 PASCAL WINBASEAPI GetMenuItemRect(HWND hWnd, 
		HMENU hMenu, 
		UINT uItem, 
		LPRECT lprcItem);

extern int
 PASCAL WINBASEAPI MenuItemFromPoint(HWND hWnd, 
		  HMENU hMenu, 
		  POINT ptScreen);

 
extern DWORD
 PASCAL WINBASEAPI DragObject(HWND, HWND, UINT, DWORD, HCURSOR);

 
extern WINBOOL
 PASCAL WINBASEAPI DragDetect(HWND hwnd, 
	   POINT pt);

 
extern WINBOOL
 PASCAL WINBASEAPI DrawIcon(
	 HDC hDC,
	 int X,
	 int Y,
	 HICON hIcon);

extern WINBOOL
 PASCAL WINBASEAPI UpdateWindow(
	     HWND hWnd);

 
extern HWND
 PASCAL WINBASEAPI SetActiveWindow(
		HWND hWnd);

 
extern HWND
 PASCAL WINBASEAPI GetForegroundWindow(
		    VOID);

extern WINBOOL
 PASCAL WINBASEAPI PaintDesktop(HDC hdc);

 
extern WINBOOL
 PASCAL WINBASEAPI SetForegroundWindow(
		    HWND hWnd);

extern WINBOOL
 PASCAL WINBASEAPI AllowSetForegroundWindow(
			DWORD dwProcessId);
  
extern HWND
 PASCAL WINBASEAPI WindowFromDC(
	     HDC hDC);

 
extern HDC
 PASCAL WINBASEAPI GetDC(
      HWND hWnd);

 
extern HDC
 PASCAL WINBASEAPI GetDCEx(
	HWND hWnd ,
	HRGN hrgnClip,
	DWORD flags);

 
extern HDC
 PASCAL WINBASEAPI GetWindowDC(
	    HWND hWnd);

 
extern int
 PASCAL WINBASEAPI ReleaseDC(
	  HWND hWnd,
	  HDC hDC);

 
extern HDC
 PASCAL WINBASEAPI BeginPaint(
	   HWND hWnd,
	   LPPAINTSTRUCT lpPaint);

 
extern WINBOOL
 PASCAL WINBASEAPI EndPaint(
	 HWND hWnd,
	 CONST PAINTSTRUCT *lpPaint);

 
extern WINBOOL
 PASCAL WINBASEAPI GetUpdateRect(
	      HWND hWnd,
	      LPRECT lpRect,
	      WINBOOL bErase);

 
extern int
 PASCAL WINBASEAPI GetUpdateRgn(
	     HWND hWnd,
	     HRGN hRgn,
	     WINBOOL bErase);

 
extern int
 PASCAL WINBASEAPI SetWindowRgn(
	     HWND hWnd,
	     HRGN hRgn,
	     WINBOOL bRedraw);

 
extern int
 PASCAL WINBASEAPI GetWindowRgn(
	     HWND hWnd,
	     HRGN hRgn);

 
extern int
 PASCAL WINBASEAPI ExcludeUpdateRgn(
		 HDC hDC,
		 HWND hWnd);

 
extern WINBOOL
 PASCAL WINBASEAPI InvalidateRect(
	       HWND hWnd ,
	       CONST RECT *lpRect,
	       WINBOOL bErase);

 
extern WINBOOL
 PASCAL WINBASEAPI ValidateRect(
	     HWND hWnd ,
	     CONST RECT *lpRect);

 
extern WINBOOL
 PASCAL WINBASEAPI InvalidateRgn(
	      HWND hWnd,
	      HRGN hRgn,
	      WINBOOL bErase);

 
extern WINBOOL
 PASCAL WINBASEAPI ValidateRgn(
	    HWND hWnd,
	    HRGN hRgn);

 
extern WINBOOL
 PASCAL WINBASEAPI RedrawWindow(
	     HWND hWnd,
	     CONST RECT *lprcUpdate,
	     HRGN hrgnUpdate,
	     UINT flags);

 
extern WINBOOL
 PASCAL WINBASEAPI LockWindowUpdate(
		 HWND hWndLock);

 
extern WINBOOL
 PASCAL WINBASEAPI ScrollWindow(
	     HWND hWnd,
	     int XAmount,
	     int YAmount,
	     CONST RECT *lpRect,
	     CONST RECT *lpClipRect);

 
extern WINBOOL
 PASCAL WINBASEAPI ScrollDC(
	 HDC hDC,
	 int dx,
	 int dy,
	 CONST RECT *lprcScroll,
	 CONST RECT *lprcClip ,
	 HRGN hrgnUpdate,
	 LPRECT lprcUpdate);

 
extern int
 PASCAL WINBASEAPI ScrollWindowEx(
	       HWND hWnd,
	       int dx,
	       int dy,
	       CONST RECT *prcScroll,
	       CONST RECT *prcClip ,
	       HRGN hrgnUpdate,
	       LPRECT prcUpdate,
	       UINT flags);

 
extern int
 PASCAL WINBASEAPI SetScrollPos(
	     HWND hWnd,
	     int nBar,
	     int nPos,
	     WINBOOL bRedraw);

 
extern int
 PASCAL WINBASEAPI GetScrollPos(
	     HWND hWnd,
	     int nBar);

 
extern WINBOOL
 PASCAL WINBASEAPI SetScrollRange(
	       HWND hWnd,
	       int nBar,
	       int nMinPos,
	       int nMaxPos,
	       WINBOOL bRedraw);

 
extern WINBOOL
 PASCAL WINBASEAPI GetScrollRange(
	       HWND hWnd,
	       int nBar,
	       LPINT lpMinPos,
	       LPINT lpMaxPos);

 
extern WINBOOL
 PASCAL WINBASEAPI ShowScrollBar(
	      HWND hWnd,
	      int wBar,
	      WINBOOL bShow);

 
extern WINBOOL
 PASCAL WINBASEAPI EnableScrollBar(
		HWND hWnd,
		UINT wSBflags,
		UINT wArrows);

 
extern WINBOOL
 PASCAL WINBASEAPI GetClientRect(
	      HWND hWnd,
	      LPRECT lpRect);

 
extern WINBOOL
 PASCAL WINBASEAPI GetWindowRect(
	      HWND hWnd,
	      LPRECT lpRect);

 
extern WINBOOL
 PASCAL WINBASEAPI AdjustWindowRect(
		 LPRECT lpRect,
		 DWORD dwStyle,
		 WINBOOL bMenu);

 
extern WINBOOL
 PASCAL WINBASEAPI AdjustWindowRectEx(
		   LPRECT lpRect,
		   DWORD dwStyle,
		   WINBOOL bMenu,
		   DWORD dwExStyle);

extern WINBOOL
 PASCAL WINBASEAPI SetWindowContextHelpId(HWND, DWORD);

extern DWORD
 PASCAL WINBASEAPI GetWindowContextHelpId(HWND);

extern WINBOOL
 PASCAL WINBASEAPI SetMenuContextHelpId(HMENU, DWORD);

extern DWORD
 PASCAL WINBASEAPI GetMenuContextHelpId(HMENU);

 
extern WINBOOL
 PASCAL WINBASEAPI MessageBeep(
	    UINT uType);

 
extern int
 PASCAL WINBASEAPI ShowCursor(
	   WINBOOL bShow);

 
extern WINBOOL
 PASCAL WINBASEAPI SetCursorPos(
	     int X,
	     int Y);

 
extern HCURSOR
 PASCAL WINBASEAPI SetCursor(
	  HCURSOR hCursor);

 
extern WINBOOL
 PASCAL WINBASEAPI GetCursorPos(
	     LPPOINT lpPoint);

 
extern WINBOOL
 PASCAL WINBASEAPI ClipCursor(
	   CONST RECT *lpRect);

 
extern WINBOOL
 PASCAL WINBASEAPI GetClipCursor(
	      LPRECT lpRect);

 
extern HCURSOR
 PASCAL WINBASEAPI GetCursor(
	  VOID);

 
extern WINBOOL
 PASCAL WINBASEAPI CreateCaret(
	    HWND hWnd,
	    HBITMAP hBitmap ,
	    int nWidth,
	    int nHeight);

 
extern UINT
 PASCAL WINBASEAPI GetCaretBlinkTime(
		  VOID);

 
extern WINBOOL
 PASCAL WINBASEAPI SetCaretBlinkTime(
		  UINT uMSeconds);

 
extern WINBOOL
 PASCAL WINBASEAPI DestroyCaret(
	     VOID);

 
extern WINBOOL
 PASCAL WINBASEAPI HideCaret(
	  HWND hWnd);

 
extern WINBOOL
 PASCAL WINBASEAPI ShowCaret(
	  HWND hWnd);

 
extern WINBOOL
 PASCAL WINBASEAPI SetCaretPos(
	    int X,
	    int Y);

 
extern WINBOOL
 PASCAL WINBASEAPI GetCaretPos(
	    LPPOINT lpPoint);

 
extern WINBOOL
 PASCAL WINBASEAPI ClientToScreen(
	       HWND hWnd,
	       LPPOINT lpPoint);

 
extern WINBOOL
 PASCAL WINBASEAPI ScreenToClient(
	       HWND hWnd,
	       LPPOINT lpPoint);

 
extern int
 PASCAL WINBASEAPI MapWindowPoints(
		HWND hWndFrom,
		HWND hWndTo,
		LPPOINT lpPoints,
		UINT cPoints);

 
extern HWND
 PASCAL WINBASEAPI WindowFromPoint(
		POINT Point);

 
extern HWND
 PASCAL WINBASEAPI ChildWindowFromPoint(
		     HWND hWndParent,
		     POINT Point);

 
extern DWORD
 PASCAL WINBASEAPI GetSysColor(
	    int nIndex);

 
extern HBRUSH
 PASCAL WINBASEAPI GetSysColorBrush(
		 int nIndex);

 
extern WINBOOL
 PASCAL WINBASEAPI SetSysColors(
	     int cElements,
	     CONST INT * lpaElements,
	     CONST COLORREF * lpaRgbValues);

 
extern WINBOOL
 PASCAL WINBASEAPI DrawFocusRect(
	      HDC hDC,
	      CONST RECT * lprc);

 
extern int
 PASCAL WINBASEAPI FillRect(
	 HDC hDC,
	 CONST RECT *lprc,
	 HBRUSH hbr);

 
extern int
 PASCAL WINBASEAPI FrameRect(
	  HDC hDC,
	  CONST RECT *lprc,
	  HBRUSH hbr);

 
extern WINBOOL
 PASCAL WINBASEAPI InvertRect(
	   HDC hDC,
	   CONST RECT *lprc);

 
extern WINBOOL
 PASCAL WINBASEAPI SetRect(
	LPRECT lprc,
	int xLeft,
	int yTop,
	int xRight,
	int yBottom);

 
extern WINBOOL
 PASCAL WINBASEAPI SetRectEmpty(
	     LPRECT lprc);

 
extern WINBOOL
 PASCAL WINBASEAPI CopyRect(
	 LPRECT lprcDst,
	 CONST RECT *lprcSrc);

 
extern WINBOOL
 PASCAL WINBASEAPI InflateRect(
	    LPRECT lprc,
	    int dx,
	    int dy);

 
extern WINBOOL
 PASCAL WINBASEAPI IntersectRect(
	      LPRECT lprcDst,
	      CONST RECT *lprcSrc1,
	      CONST RECT *lprcSrc2);

 
extern WINBOOL
 PASCAL WINBASEAPI UnionRect(
	  LPRECT lprcDst,
	  CONST RECT *lprcSrc1,
	  CONST RECT *lprcSrc2);

 
extern WINBOOL
 PASCAL WINBASEAPI SubtractRect(
	     LPRECT lprcDst,
	     CONST RECT *lprcSrc1,
	     CONST RECT *lprcSrc2);

 
extern WINBOOL
 PASCAL WINBASEAPI OffsetRect(
	   LPRECT lprc,
	   int dx,
	   int dy);

 
extern WINBOOL
 PASCAL WINBASEAPI IsRectEmpty(
	    CONST RECT *lprc);

 
extern WINBOOL
 PASCAL WINBASEAPI EqualRect(
	  CONST RECT *lprc1,
	  CONST RECT *lprc2);

 
extern WINBOOL
 PASCAL WINBASEAPI PtInRect(
	 CONST RECT *lprc,
	 POINT pt);

 
extern WORD
 PASCAL WINBASEAPI GetWindowWord(
	      HWND hWnd,
	      int nIndex);

 
extern WORD
 PASCAL WINBASEAPI SetWindowWord(
	      HWND hWnd,
	      int nIndex,
	      WORD wNewWord);

 
extern WORD
 PASCAL WINBASEAPI GetClassWord(
	     HWND hWnd,
	     int nIndex);

 
extern WORD
 PASCAL WINBASEAPI SetClassWord(
	     HWND hWnd,
	     int nIndex,
	     WORD wNewWord);

extern HWND
 PASCAL WINBASEAPI GetDesktopWindow(
		 VOID);

 
extern HWND
 PASCAL WINBASEAPI GetParent(
	  HWND hWnd);

 
extern HWND
 PASCAL WINBASEAPI SetParent(
	  HWND hWndChild,
	  HWND hWndNewParent);

 
extern WINBOOL
 PASCAL WINBASEAPI EnumChildWindows(
		 HWND hWndParent,
		 WNDENUMPROC lpEnumFunc,
		 LPARAM lParam);

 
extern WINBOOL
 PASCAL WINBASEAPI EnumWindows(
	    WNDENUMPROC lpEnumFunc,
	    LPARAM lParam);

 
extern WINBOOL
 PASCAL WINBASEAPI EnumThreadWindows(
		  DWORD dwThreadId,
		  WNDENUMPROC lpfn,
		  LPARAM lParam);

extern HWND
 PASCAL WINBASEAPI GetTopWindow(
	     HWND hWnd);

#define GetNextWindow(hWnd, wCmd) GetWindow(hWnd, wCmd)
#define GetSysModalWindow() (NULL)
#define SetSysModalWindow(hWnd) (NULL)
 
extern DWORD
 PASCAL WINBASEAPI GetWindowThreadProcessId(
			 HWND hWnd,
			 LPDWORD lpdwProcessId);

#define GetWindowTask(hWnd) \
	((HANDLE)GetWindowThreadProcessId(hWnd, NULL))
 
extern HWND
 PASCAL WINBASEAPI GetLastActivePopup(
		   HWND hWnd);

 
extern HWND
 PASCAL WINBASEAPI GetWindow(
	  HWND hWnd,
	  UINT uCmd);

extern WINBOOL
 PASCAL WINBASEAPI UnhookWindowsHook(
		  int nCode,
		  HOOKPROC pfnFilterProc);

extern WINBOOL
 PASCAL WINBASEAPI UnhookWindowsHookEx(
		    HHOOK hhk);

 
extern LRESULT
 PASCAL WINBASEAPI CallNextHookEx(
	       HHOOK hhk,
	       int nCode,
	       WPARAM wParam,
	       LPARAM lParam);

#define DefHookProc(nCode, wParam, lParam, phhk)\
        CallNextHookEx((HHOOK)*phhk, nCode, wParam, lParam)
 
extern WINBOOL
 PASCAL WINBASEAPI CheckMenuRadioItem(HMENU, UINT, UINT, UINT, UINT);

extern HCURSOR
 PASCAL WINBASEAPI CreateCursor(
	     HINSTANCE hInst,
	     int xHotSpot,
	     int yHotSpot,
	     int nWidth,
	     int nHeight,
	     CONST VOID *pvANDPlane,
	     CONST VOID *pvXORPlane);

 
extern WINBOOL
 PASCAL WINBASEAPI DestroyCursor(
	      HCURSOR hCursor);

 
extern WINBOOL
 PASCAL WINBASEAPI SetSystemCursor(
		HCURSOR hcur,
		DWORD   anID);

 
extern HICON
 PASCAL WINBASEAPI CreateIcon(
	   HINSTANCE hInstance,
	   int nWidth,
	   int nHeight,
	   BYTE cPlanes,
	   BYTE cBitsPixel,
	   CONST BYTE *lpbANDbits,
	   CONST BYTE *lpbXORbits);

 
extern WINBOOL
 PASCAL WINBASEAPI DestroyIcon(
	    HICON hIcon);

 
extern int
 PASCAL WINBASEAPI LookupIconIdFromDirectory(
			  PBYTE presbits,
			  WINBOOL fIcon);

 
extern int
 PASCAL WINBASEAPI LookupIconIdFromDirectoryEx(
			    PBYTE presbits,
			    WINBOOL  fIcon,
			    int   cxDesired,
			    int   cyDesired,
			    UINT  Flags);

 
extern HICON
 PASCAL WINBASEAPI CreateIconFromResource(
		       PBYTE presbits,
		       DWORD dwResSize,
		       WINBOOL fIcon,
		       DWORD dwVer);

 
extern HICON
 PASCAL WINBASEAPI CreateIconFromResourceEx(
			 PBYTE presbits,
			 DWORD dwResSize,
			 WINBOOL  fIcon,
			 DWORD dwVer,
			 int   cxDesired,
			 int   cyDesired,
			 UINT  Flags);

 
extern HICON
 PASCAL WINBASEAPI CopyImage(
	  HANDLE,
	  UINT,
	  int,
	  int,
	  UINT);

 
extern HICON
 PASCAL WINBASEAPI CreateIconIndirect(
		   PICONINFO piconinfo);

 
extern HICON
 PASCAL WINBASEAPI CopyIcon(
	 HICON hIcon);

 
extern WINBOOL
 PASCAL WINBASEAPI GetIconInfo(
	    HICON hIcon,
	    PICONINFO piconinfo);

 
extern WINBOOL
 PASCAL WINBASEAPI MapDialogRect(
	      HWND hDlg,
	      LPRECT lpRect);

extern int
 PASCAL WINBASEAPI SetScrollInfo(HWND, int, LPCSCROLLINFO, WINBOOL);

extern WINBOOL
 PASCAL WINBASEAPI GetScrollInfo(HWND, int, LPSCROLLINFO);

extern WINBOOL
 PASCAL WINBASEAPI TranslateMDISysAccel(
		     HWND hWndClient,
		     LPMSG lpMsg);

 
extern UINT
 PASCAL WINBASEAPI ArrangeIconicWindows(
		     HWND hWnd);

extern WORD
 PASCAL WINBASEAPI TileWindows(HWND hwndParent, UINT wHow, CONST RECT * lpRect, UINT cKids, const HWND *lpKids);

extern WORD
 PASCAL WINBASEAPI CascadeWindows(HWND hwndParent, UINT wHow, CONST RECT * lpRect, UINT cKids,  const HWND *lpKids);

 
extern VOID
 PASCAL WINBASEAPI SetLastErrorEx(
	       DWORD dwErrCode,
	       DWORD dwType
	       );

 
extern VOID
 PASCAL WINBASEAPI SetDebugErrorLevel(
		   DWORD dwLevel
		   );

extern WINBOOL
 PASCAL WINBASEAPI DrawEdge(HDC hdc, LPRECT qrc, UINT edge, UINT grfFlags);

extern WINBOOL
 PASCAL WINBASEAPI DrawFrameControl(HDC, LPRECT, UINT, UINT);

extern WINBOOL
 PASCAL WINBASEAPI DrawCaption(HWND, HDC, CONST RECT *, UINT);

extern WINBOOL
 PASCAL WINBASEAPI DrawAnimatedRects(HWND hwnd, int idAni, CONST RECT * lprcFrom, CONST RECT * lprcTo);

extern WINBOOL
 PASCAL WINBASEAPI TrackPopupMenuEx(HMENU, UINT, int, int, HWND, LPTPMPARAMS);

extern HWND
 PASCAL WINBASEAPI ChildWindowFromPointEx(HWND, POINT, UINT);

extern WINBOOL
 PASCAL WINBASEAPI DrawIconEx(HDC hdc, int xLeft, int yTop,
	   HICON hIcon, int cxWidth, int cyWidth,
	   UINT istepIfAniCur, HBRUSH hbrFlickerFreeDraw, UINT diFlags);

extern WINBOOL
 PASCAL WINBASEAPI AnimatePalette(HPALETTE, UINT, UINT, CONST PALETTEENTRY *);

extern WINBOOL
 PASCAL WINBASEAPI Arc(HDC, int, int, int, int, int, int, int, int);

extern WINBOOL
 PASCAL WINBASEAPI BitBlt(HDC, int, int, int, int, HDC, int, int, DWORD);

extern WINBOOL
 PASCAL WINBASEAPI CancelDC(HDC);

extern WINBOOL
 PASCAL WINBASEAPI Chord(HDC, int, int, int, int, int, int, int, int);

extern HMETAFILE
 PASCAL WINBASEAPI CloseMetaFile(HDC);

extern int
 PASCAL WINBASEAPI CombineRgn(HRGN, HRGN, HRGN, int);

extern HBITMAP
 PASCAL WINBASEAPI CreateBitmap(int, int, UINT, UINT, CONST VOID *);

extern HBITMAP
 PASCAL WINBASEAPI CreateBitmapIndirect(CONST BITMAP *);

extern HBRUSH
 PASCAL WINBASEAPI CreateBrushIndirect(CONST LOGBRUSH *);

extern HBITMAP
 PASCAL WINBASEAPI CreateCompatibleBitmap(HDC, int, int);

extern HBITMAP
 PASCAL WINBASEAPI CreateDiscardableBitmap(HDC, int, int);

extern HDC
 PASCAL WINBASEAPI CreateCompatibleDC(HDC);

extern HBITMAP
 PASCAL WINBASEAPI CreateDIBitmap(HDC, CONST BITMAPINFOHEADER *, DWORD, CONST VOID *, CONST BITMAPINFO *, UINT);

extern HBRUSH
 PASCAL WINBASEAPI CreateDIBPatternBrush(HGLOBAL, UINT);

extern HBRUSH
 PASCAL WINBASEAPI CreateDIBPatternBrushPt(CONST VOID *, UINT);

extern HRGN
 PASCAL WINBASEAPI CreateEllipticRgn(int, int, int, int);

extern HRGN
 PASCAL WINBASEAPI CreateEllipticRgnIndirect(CONST RECT *);

extern HBRUSH
 PASCAL WINBASEAPI CreateHatchBrush(int, COLORREF);

extern HPALETTE
 PASCAL WINBASEAPI CreatePalette(CONST LOGPALETTE *);

extern HPEN
 PASCAL WINBASEAPI CreatePen(int, int, COLORREF);

extern HPEN
 PASCAL WINBASEAPI CreatePenIndirect(CONST LOGPEN *);

extern HRGN
 PASCAL WINBASEAPI CreatePolyPolygonRgn(CONST POINT *, CONST INT *, int, int);

extern HBRUSH
 PASCAL WINBASEAPI CreatePatternBrush(HBITMAP);

extern HRGN
 PASCAL WINBASEAPI CreateRectRgn(int, int, int, int);

extern HRGN
 PASCAL WINBASEAPI CreateRectRgnIndirect(CONST RECT *);

extern HRGN
 PASCAL WINBASEAPI CreateRoundRectRgn(int, int, int, int, int, int);

extern HBRUSH
 PASCAL WINBASEAPI CreateSolidBrush(COLORREF);

extern WINBOOL
 PASCAL WINBASEAPI DeleteDC(HDC);

extern WINBOOL
 PASCAL WINBASEAPI DeleteMetaFile(HMETAFILE);

extern WINBOOL
 PASCAL WINBASEAPI DeleteObject(HGDIOBJ);

extern int
 PASCAL WINBASEAPI DrawEscape(HDC, int, int, LPCSTR);

extern WINBOOL
 PASCAL WINBASEAPI Ellipse(HDC, int, int, int, int);

extern int
 PASCAL WINBASEAPI EnumObjects(HDC, int, ENUMOBJECTSPROC, LPARAM);

extern WINBOOL
 PASCAL WINBASEAPI EqualRgn(HRGN, HRGN);

extern int
 PASCAL WINBASEAPI Escape(HDC, int, int, LPCSTR, LPVOID);

extern int
 PASCAL WINBASEAPI ExtEscape(HDC, int, int, LPCSTR, int, LPSTR);

extern int
 PASCAL WINBASEAPI ExcludeClipRect(HDC, int, int, int, int);

extern HRGN
 PASCAL WINBASEAPI ExtCreateRegion(CONST XFORM *, DWORD, CONST RGNDATA *);

extern WINBOOL
 PASCAL WINBASEAPI ExtFloodFill(HDC, int, int, COLORREF, UINT);

extern WINBOOL
 PASCAL WINBASEAPI FillRgn(HDC, HRGN, HBRUSH);

extern WINBOOL
 PASCAL WINBASEAPI FloodFill(HDC, int, int, COLORREF);

extern WINBOOL
 PASCAL WINBASEAPI FrameRgn(HDC, HRGN, HBRUSH, int, int);

extern int
 PASCAL WINBASEAPI GetROP2(HDC);

extern WINBOOL
 PASCAL WINBASEAPI GetAspectRatioFilterEx(HDC, LPSIZE);

extern COLORREF
 PASCAL WINBASEAPI GetBkColor(HDC);

extern int
 PASCAL WINBASEAPI GetBkMode(HDC);

extern LONG
 PASCAL WINBASEAPI GetBitmapBits(HBITMAP, LONG, LPVOID);

extern WINBOOL
 PASCAL WINBASEAPI GetBitmapDimensionEx(HBITMAP, LPSIZE);

extern UINT
 PASCAL WINBASEAPI GetBoundsRect(HDC, LPRECT, UINT);

extern WINBOOL
 PASCAL WINBASEAPI GetBrushOrgEx(HDC, LPPOINT);

extern int
 PASCAL WINBASEAPI GetClipBox(HDC, LPRECT);

extern int
 PASCAL WINBASEAPI GetClipRgn(HDC, HRGN);

extern int
 PASCAL WINBASEAPI GetMetaRgn(HDC, HRGN);

extern HGDIOBJ
 PASCAL WINBASEAPI GetCurrentObject(HDC, UINT);

extern WINBOOL
 PASCAL WINBASEAPI GetCurrentPositionEx(HDC, LPPOINT);

extern int
 PASCAL WINBASEAPI GetDeviceCaps(HDC, int);

extern int
 PASCAL WINBASEAPI GetDIBits(HDC, HBITMAP, UINT, UINT, LPVOID, LPBITMAPINFO, UINT);

extern DWORD
 PASCAL WINBASEAPI GetFontData(HDC, DWORD, DWORD, LPVOID, DWORD);

extern int
 PASCAL WINBASEAPI GetGraphicsMode(HDC);

extern int
 PASCAL WINBASEAPI GetMapMode(HDC);

extern UINT
 PASCAL WINBASEAPI GetMetaFileBitsEx(HMETAFILE, UINT, LPVOID);

extern COLORREF
 PASCAL WINBASEAPI GetNearestColor(HDC, COLORREF);

extern UINT
 PASCAL WINBASEAPI GetNearestPaletteIndex(HPALETTE, COLORREF);

extern DWORD
 PASCAL WINBASEAPI GetObjectType(HGDIOBJ h);

extern UINT
 PASCAL WINBASEAPI GetPaletteEntries(HPALETTE, UINT, UINT, LPPALETTEENTRY);

extern COLORREF
 PASCAL WINBASEAPI GetPixel(HDC, int, int);

extern int
 PASCAL WINBASEAPI GetPixelFormat(HDC);

extern int
 PASCAL WINBASEAPI GetPolyFillMode(HDC);

extern WINBOOL
 PASCAL WINBASEAPI GetRasterizerCaps(LPRASTERIZER_STATUS, UINT);

extern DWORD
 PASCAL WINBASEAPI GetRegionData(HRGN, DWORD, LPRGNDATA);

extern int
 PASCAL WINBASEAPI GetRgnBox(HRGN, LPRECT);

extern HGDIOBJ
 PASCAL WINBASEAPI GetStockObject(int);

extern int
 PASCAL WINBASEAPI GetStretchBltMode(HDC);

extern UINT
 PASCAL WINBASEAPI GetSystemPaletteEntries(HDC, UINT, UINT, LPPALETTEENTRY);

extern UINT
 PASCAL WINBASEAPI GetSystemPaletteUse(HDC);

extern int
 PASCAL WINBASEAPI GetTextCharacterExtra(HDC);

extern UINT
 PASCAL WINBASEAPI GetTextAlign(HDC);

extern COLORREF
 PASCAL WINBASEAPI GetTextColor(HDC);

extern int
 PASCAL WINBASEAPI GetTextCharset(HDC hdc);

extern int
 PASCAL WINBASEAPI GetTextCharsetInfo(HDC hdc, LPFONTSIGNATURE lpSig, DWORD dwFlags);

extern WINBOOL
 PASCAL WINBASEAPI TranslateCharsetInfo( DWORD *lpSrc, LPCHARSETINFO lpCs, DWORD dwFlags);

extern DWORD
 PASCAL WINBASEAPI GetFontLanguageInfo( HDC );

extern WINBOOL
 PASCAL WINBASEAPI GetViewportExtEx(HDC, LPSIZE);

extern WINBOOL
 PASCAL WINBASEAPI GetViewportOrgEx(HDC, LPPOINT);

extern WINBOOL
 PASCAL WINBASEAPI GetWindowExtEx(HDC, LPSIZE);

extern WINBOOL
 PASCAL WINBASEAPI GetWindowOrgEx(HDC, LPPOINT);

extern int
 PASCAL WINBASEAPI IntersectClipRect(HDC, int, int, int, int);

extern WINBOOL
 PASCAL WINBASEAPI InvertRgn(HDC, HRGN);

extern WINBOOL
 PASCAL WINBASEAPI LineDDA(int, int, int, int, LINEDDAPROC, LPARAM);

extern WINBOOL
 PASCAL WINBASEAPI LineTo(HDC, int, int);

extern WINBOOL
 PASCAL WINBASEAPI MaskBlt(HDC, int, int, int, int,
	HDC, int, int, HBITMAP, int, int, DWORD);

extern WINBOOL
 PASCAL WINBASEAPI PlgBlt(HDC, CONST POINT *, HDC, int, int, int,
       int, HBITMAP, int, int);

extern int
 PASCAL WINBASEAPI OffsetClipRgn(HDC, int, int);

extern int
 PASCAL WINBASEAPI OffsetRgn(HRGN, int, int);

extern WINBOOL
  PASCAL WINBASEAPI PatBlt(HDC, int, int, int, int, DWORD);

extern WINBOOL
 PASCAL WINBASEAPI Pie(HDC, int, int, int, int, int, int, int, int);

extern WINBOOL
 PASCAL WINBASEAPI PlayMetaFile(HDC, HMETAFILE);

extern WINBOOL
 PASCAL WINBASEAPI PaintRgn(HDC, HRGN);

extern WINBOOL
 PASCAL WINBASEAPI PolyPolygon(HDC, CONST POINT *, CONST INT *, int);

extern WINBOOL
 PASCAL WINBASEAPI PtInRegion(HRGN, int, int);

extern WINBOOL
 PASCAL WINBASEAPI PtVisible(HDC, int, int);

extern WINBOOL
 PASCAL WINBASEAPI RectInRegion(HRGN, CONST RECT *);

extern WINBOOL
 PASCAL WINBASEAPI RectVisible(HDC, CONST RECT *);

extern WINBOOL
 PASCAL WINBASEAPI Rectangle(HDC, int, int, int, int);

extern WINBOOL
 PASCAL WINBASEAPI RestoreDC(HDC, int);

extern UINT
 PASCAL WINBASEAPI RealizePalette(HDC);

extern WINBOOL
 PASCAL WINBASEAPI RoundRect(HDC, int, int, int, int, int, int);

extern WINBOOL
 PASCAL WINBASEAPI ResizePalette(HPALETTE, UINT);

extern int
 PASCAL WINBASEAPI SaveDC(HDC);

extern int
 PASCAL WINBASEAPI SelectClipRgn(HDC, HRGN);

extern int
 PASCAL WINBASEAPI ExtSelectClipRgn(HDC, HRGN, int);

extern int
 PASCAL WINBASEAPI SetMetaRgn(HDC);

extern HGDIOBJ
 PASCAL WINBASEAPI SelectObject(HDC, HGDIOBJ);

extern HPALETTE
 PASCAL WINBASEAPI SelectPalette(HDC, HPALETTE, WINBOOL);

extern COLORREF
 PASCAL WINBASEAPI SetBkColor(HDC, COLORREF);

extern int
 PASCAL WINBASEAPI SetBkMode(HDC, int);

extern LONG
 PASCAL WINBASEAPI SetBitmapBits(HBITMAP, DWORD, CONST VOID *);

extern UINT
 PASCAL WINBASEAPI SetBoundsRect(HDC, CONST RECT *, UINT);

extern int
 PASCAL WINBASEAPI SetDIBits(HDC, HBITMAP, UINT, UINT, CONST VOID *, CONST BITMAPINFO *, UINT);

extern int
 PASCAL WINBASEAPI SetDIBitsToDevice(HDC, int, int, DWORD, DWORD, int,
		  int, UINT, UINT, CONST VOID *, CONST BITMAPINFO *, UINT);

extern DWORD
 PASCAL WINBASEAPI SetMapperFlags(HDC, DWORD);

extern int
 PASCAL WINBASEAPI SetGraphicsMode(HDC hdc, int iMode);

extern int
 PASCAL WINBASEAPI SetMapMode(HDC, int);

extern HMETAFILE
 PASCAL WINBASEAPI SetMetaFileBitsEx(UINT, CONST BYTE *);

extern UINT
 PASCAL WINBASEAPI SetPaletteEntries(HPALETTE, UINT, UINT, CONST PALETTEENTRY *);

extern COLORREF
 PASCAL WINBASEAPI SetPixel(HDC, int, int, COLORREF);

extern WINBOOL
 PASCAL WINBASEAPI SetPixelV(HDC, int, int, COLORREF);

extern int
 PASCAL WINBASEAPI SetPolyFillMode(HDC, int);

extern WINBOOL
 PASCAL WINBASEAPI StretchBlt(HDC, int, int, int, int, HDC, int, int, int, int, DWORD);

extern WINBOOL
 PASCAL WINBASEAPI SetRectRgn(HRGN, int, int, int, int);
extern int
 PASCAL WINBASEAPI StretchDIBits(HDC, int, int, int, int, int, int, int, int, CONST
	      VOID *, CONST BITMAPINFO *, UINT, DWORD);

extern int
 PASCAL WINBASEAPI SetROP2(HDC, int);

extern int
 PASCAL WINBASEAPI SetStretchBltMode(HDC, int);

extern UINT
 PASCAL WINBASEAPI SetSystemPaletteUse(HDC, UINT);

extern int
 PASCAL WINBASEAPI SetTextCharacterExtra(HDC, int);

extern COLORREF
 PASCAL WINBASEAPI SetTextColor(HDC, COLORREF);

extern UINT
 PASCAL WINBASEAPI SetTextAlign(HDC, UINT);

extern WINBOOL
 PASCAL WINBASEAPI SetTextJustification(HDC, int, int);

extern WINBOOL
 PASCAL WINBASEAPI UpdateColors(HDC);

extern WINBOOL
 PASCAL WINBASEAPI PlayMetaFileRecord(HDC, LPHANDLETABLE, LPMETARECORD, UINT);

extern WINBOOL
 PASCAL WINBASEAPI EnumMetaFile(HDC, HMETAFILE, ENUMMETAFILEPROC, LPARAM);

extern HENHMETAFILE
 PASCAL WINBASEAPI CloseEnhMetaFile(HDC);

extern WINBOOL
 PASCAL WINBASEAPI DeleteEnhMetaFile(HENHMETAFILE);

extern WINBOOL
 PASCAL WINBASEAPI EnumEnhMetaFile(HDC, HENHMETAFILE, ENHMETAFILEPROC,
		LPVOID, CONST RECT *);

extern UINT
 PASCAL WINBASEAPI GetEnhMetaFileHeader(HENHMETAFILE, UINT, LPENHMETAHEADER );

extern UINT
 PASCAL WINBASEAPI GetEnhMetaFilePaletteEntries(HENHMETAFILE, UINT, LPPALETTEENTRY );

extern UINT
 PASCAL WINBASEAPI GetWinMetaFileBits(HENHMETAFILE, UINT, LPBYTE, INT, HDC);

extern WINBOOL
 PASCAL WINBASEAPI PlayEnhMetaFile(HDC, HENHMETAFILE, CONST RECT *);

extern WINBOOL
 PASCAL WINBASEAPI PlayEnhMetaFileRecord(HDC, LPHANDLETABLE, CONST ENHMETARECORD *, UINT);

extern HENHMETAFILE
 PASCAL WINBASEAPI SetEnhMetaFileBits(UINT, CONST BYTE *);

extern HENHMETAFILE
 PASCAL WINBASEAPI SetWinMetaFileBits(UINT, CONST BYTE *, HDC, CONST METAFILEPICT *);

extern WINBOOL
 PASCAL WINBASEAPI GdiComment(HDC, UINT, CONST BYTE *);

extern WINBOOL
 PASCAL WINBASEAPI AlphaBlend(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION);

extern WINBOOL
 PASCAL WINBASEAPI TransparentBlt(HDC, int, int, int, int, HDC, int, int, int, int, UINT);
extern WINBOOL 
 PASCAL WINBASEAPI GradientFill(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);

extern WINBOOL
 PASCAL WINBASEAPI AngleArc(HDC, int, int, DWORD, FLOAT, FLOAT);

extern WINBOOL
 PASCAL WINBASEAPI PolyPolyline(HDC, CONST POINT *, CONST DWORD *, DWORD);

extern WINBOOL
 PASCAL WINBASEAPI GetWorldTransform(HDC, LPXFORM);

extern WINBOOL
 PASCAL WINBASEAPI SetWorldTransform(HDC, CONST XFORM *);

extern WINBOOL
 PASCAL WINBASEAPI ModifyWorldTransform(HDC, CONST XFORM *, DWORD);

extern WINBOOL
 PASCAL WINBASEAPI CombineTransform(LPXFORM, CONST XFORM *, CONST XFORM *);

extern HBITMAP
 PASCAL WINBASEAPI CreateDIBSection(HDC, CONST BITMAPINFO *, UINT, VOID **, HANDLE, DWORD);

extern UINT
 PASCAL WINBASEAPI GetDIBColorTable(HDC, UINT, UINT, RGBQUAD *);

extern UINT
 PASCAL WINBASEAPI SetDIBColorTable(HDC, UINT, UINT, CONST RGBQUAD *);

extern WINBOOL
 PASCAL WINBASEAPI SetColorAdjustment(HDC, CONST COLORADJUSTMENT *);

extern WINBOOL
 PASCAL WINBASEAPI GetColorAdjustment(HDC, LPCOLORADJUSTMENT);

extern HPALETTE
  PASCAL WINBASEAPI CreateHalftonePalette(HDC);

extern int
 PASCAL WINBASEAPI EndDoc(HDC);

extern int
 PASCAL WINBASEAPI StartPage(HDC);

extern int
 PASCAL WINBASEAPI EndPage(HDC);

extern int
 PASCAL WINBASEAPI AbortDoc(HDC);

extern int
 PASCAL WINBASEAPI SetAbortProc(HDC, ABORTPROC);

extern WINBOOL
 PASCAL WINBASEAPI AbortPath(HDC);

extern WINBOOL
 PASCAL WINBASEAPI ArcTo(HDC, int, int, int, int, int, int,int, int);

extern WINBOOL
 PASCAL WINBASEAPI BeginPath(HDC);

extern WINBOOL
 PASCAL WINBASEAPI CloseFigure(HDC);

extern WINBOOL
 PASCAL WINBASEAPI EndPath(HDC);

extern WINBOOL
 PASCAL WINBASEAPI FillPath(HDC);

extern WINBOOL
 PASCAL WINBASEAPI FlattenPath(HDC);

extern int
 PASCAL WINBASEAPI GetPath(HDC, LPPOINT, LPBYTE, int);

extern HRGN
 PASCAL WINBASEAPI PathToRegion(HDC);

extern WINBOOL
 PASCAL WINBASEAPI PolyDraw(HDC, CONST POINT *, CONST BYTE *, int);

extern WINBOOL
 PASCAL WINBASEAPI SelectClipPath(HDC, int);

extern int
 PASCAL WINBASEAPI SetArcDirection(HDC, int);

extern WINBOOL
 PASCAL WINBASEAPI SetMiterLimit(HDC, FLOAT, PFLOAT);

extern WINBOOL
 PASCAL WINBASEAPI StrokeAndFillPath(HDC);

extern WINBOOL
 PASCAL WINBASEAPI StrokePath(HDC);

extern WINBOOL
 PASCAL WINBASEAPI WidenPath(HDC);

extern HPEN
 PASCAL WINBASEAPI ExtCreatePen(DWORD, DWORD, CONST LOGBRUSH *, DWORD, CONST DWORD *);

extern WINBOOL
 PASCAL WINBASEAPI GetMiterLimit(HDC, PFLOAT);

extern int
 PASCAL WINBASEAPI GetArcDirection(HDC);

extern WINBOOL
 PASCAL WINBASEAPI MoveToEx(HDC, int, int, LPPOINT);

extern HRGN
 PASCAL WINBASEAPI CreatePolygonRgn(CONST POINT *, int, int);

extern WINBOOL
 PASCAL WINBASEAPI DPtoLP(HDC, LPPOINT, int);

extern WINBOOL
 PASCAL WINBASEAPI LPtoDP(HDC, LPPOINT, int);
 
extern WINBOOL
 PASCAL WINBASEAPI Polygon(HDC, CONST POINT *, int);

extern WINBOOL
 PASCAL WINBASEAPI Polyline(HDC, CONST POINT *, int);

extern WINBOOL
 PASCAL WINBASEAPI PolyBezier(HDC, CONST POINT *, DWORD);

extern WINBOOL
 PASCAL WINBASEAPI PolyBezierTo(HDC, CONST POINT *, DWORD);

extern WINBOOL
 PASCAL WINBASEAPI PolylineTo(HDC, CONST POINT *, DWORD);

extern WINBOOL
 PASCAL WINBASEAPI SetViewportExtEx(HDC, int, int, LPSIZE);

extern WINBOOL
 PASCAL WINBASEAPI SetViewportOrgEx(HDC, int, int, LPPOINT);

extern WINBOOL
 PASCAL WINBASEAPI SetWindowExtEx(HDC, int, int, LPSIZE);

extern WINBOOL
 PASCAL WINBASEAPI SetWindowOrgEx(HDC, int, int, LPPOINT);

extern WINBOOL
 PASCAL WINBASEAPI OffsetViewportOrgEx(HDC, int, int, LPPOINT);

extern WINBOOL
 PASCAL WINBASEAPI OffsetWindowOrgEx(HDC, int, int, LPPOINT);

extern WINBOOL
 PASCAL WINBASEAPI ScaleViewportExtEx(HDC, int, int, int, int, LPSIZE);

extern WINBOOL
 PASCAL WINBASEAPI ScaleWindowExtEx(HDC, int, int, int, int, LPSIZE);

extern WINBOOL
 PASCAL WINBASEAPI SetBitmapDimensionEx(HBITMAP, int, int, LPSIZE);

extern WINBOOL
 PASCAL WINBASEAPI SetBrushOrgEx(HDC, int, int, LPPOINT);

extern WINBOOL
 PASCAL WINBASEAPI GetDCOrgEx(HDC,LPPOINT);

extern WINBOOL
 PASCAL WINBASEAPI FixBrushOrgEx(HDC,int,int,LPPOINT);

extern WINBOOL
 PASCAL WINBASEAPI UnrealizeObject(HGDIOBJ);

extern WINBOOL
 PASCAL WINBASEAPI GdiFlush();

extern DWORD
 PASCAL WINBASEAPI GdiSetBatchLimit(DWORD);

extern DWORD
 PASCAL WINBASEAPI GdiGetBatchLimit();

extern int
 PASCAL WINBASEAPI SetICMMode(HDC, int);

extern WINBOOL
 PASCAL WINBASEAPI CheckColorsInGamut(HDC,LPVOID,LPVOID,DWORD);

extern HANDLE
 PASCAL WINBASEAPI GetColorSpace(HDC);

extern WINBOOL
 PASCAL WINBASEAPI SetColorSpace(HDC,HCOLORSPACE);

extern WINBOOL
 PASCAL WINBASEAPI DeleteColorSpace(HCOLORSPACE);

extern WINBOOL
 PASCAL WINBASEAPI GetDeviceGammaRamp(HDC,LPVOID);

extern WINBOOL
 PASCAL WINBASEAPI SetDeviceGammaRamp(HDC,LPVOID);

extern WINBOOL
 PASCAL WINBASEAPI ColorMatchToTarget(HDC,HDC,DWORD);

extern void
 PASCAL WINBASEAPI GetEffectiveClientRect(HWND hWnd, LPRECT lprc, LPINT lpInfo);

extern LONG
 PASCAL WINBASEAPI RegCloseKey(
    HKEY hKey
    );

extern LONG
 PASCAL WINBASEAPI RegSetKeySecurity(
    HKEY hKey,
    SECURITY_INFORMATION SecurityInformation,
    PSECURITY_DESCRIPTOR pSecurityDescriptor
    );

extern LONG
 PASCAL WINBASEAPI RegFlushKey(
    HKEY hKey
    );

extern LONG
 PASCAL WINBASEAPI RegGetKeySecurity(
    HKEY hKey,
    SECURITY_INFORMATION SecurityInformation,
    PSECURITY_DESCRIPTOR pSecurityDescriptor,
    LPDWORD lpcbSecurityDescriptor
    );

extern LONG
 PASCAL WINBASEAPI RegNotifyChangeKeyValue(
    HKEY hKey,
    WINBOOL bWatchSubtree,
    DWORD dwNotifyFilter,
    HANDLE hEvent,
    WINBOOL fAsynchronus
    );

extern WINBOOL
 PASCAL WINBASEAPI IsValidCodePage(
    UINT  CodePage);


extern UINT
 PASCAL WINBASEAPI GetACP(void);


extern UINT
 PASCAL WINBASEAPI GetOEMCP(void);


extern WINBOOL
 PASCAL WINBASEAPI GetCPInfo(UINT, LPCPINFO);


extern WINBOOL
 PASCAL WINBASEAPI IsDBCSLeadByte(
    BYTE  TestChar);


extern WINBOOL
 PASCAL WINBASEAPI IsDBCSLeadByteEx(
    UINT  CodePage,
    BYTE  TestChar);


extern int
 PASCAL WINBASEAPI MultiByteToWideChar(
    UINT     CodePage,
    DWORD    dwFlags,
    LPCSTR   lpMultiByteStr,
    int      cchMultiByte,
    LPWSTR   lpWideCharStr,
    int      cchWideChar);


extern int
 PASCAL WINBASEAPI WideCharToMultiByte(
    UINT     CodePage,
    DWORD    dwFlags,
    LPCWSTR  lpWideCharStr,
    int      cchWideChar,
    LPSTR    lpMultiByteStr,
    int      cchMultiByte,
    LPCSTR   lpDefaultChar,
    LPBOOL   lpUsedDefaultChar);

extern WINBOOL
 PASCAL WINBASEAPI IsValidLocale(
    LCID   Locale,
    DWORD  dwFlags);


extern LCID
 PASCAL WINBASEAPI ConvertDefaultLocale(
    LCID   Locale);


extern LCID
 PASCAL WINBASEAPI GetThreadLocale(void);


extern WINBOOL
 PASCAL WINBASEAPI SetThreadLocale(
    LCID  Locale
    );


extern LANGID
 PASCAL WINBASEAPI GetSystemDefaultLangID(void);


extern LANGID
 PASCAL WINBASEAPI GetUserDefaultLangID(void);


extern LCID
 PASCAL WINBASEAPI GetSystemDefaultLCID(void);


extern LCID
 PASCAL WINBASEAPI GetUserDefaultLCID(void);


extern WINBOOL
 PASCAL WINBASEAPI ReadConsoleOutputAttribute(
    HANDLE hConsoleOutput,
    LPWORD lpAttribute,
    DWORD nLength,
    COORD dwReadCoord,
    LPDWORD lpNumberOfAttrsRead
    );


extern WINBOOL
 PASCAL WINBASEAPI WriteConsoleOutputAttribute(
    HANDLE hConsoleOutput,
    CONST WORD *lpAttribute,
    DWORD nLength,
    COORD dwWriteCoord,
    LPDWORD lpNumberOfAttrsWritten
    );


extern WINBOOL
 PASCAL WINBASEAPI FillConsoleOutputAttribute(
    HANDLE hConsoleOutput,
    WORD   wAttribute,
    DWORD  nLength,
    COORD  dwWriteCoord,
    LPDWORD lpNumberOfAttrsWritten
    );


extern WINBOOL
 PASCAL WINBASEAPI GetConsoleMode(
    HANDLE hConsoleHandle,
    LPDWORD lpMode
    );


extern WINBOOL
 PASCAL WINBASEAPI GetNumberOfConsoleInputEvents(
    HANDLE hConsoleInput,
    LPDWORD lpNumberOfEvents
    );


extern WINBOOL
 PASCAL WINBASEAPI GetConsoleScreenBufferInfo(
    HANDLE hConsoleOutput,
    PCONSOLE_SCREEN_BUFFER_INFO lpConsoleScreenBufferInfo
    );


extern COORD
 PASCAL WINBASEAPI GetLargestConsoleWindowSize(
    HANDLE hConsoleOutput
    );


extern WINBOOL
 PASCAL WINBASEAPI GetConsoleCursorInfo(
    HANDLE hConsoleOutput,
    PCONSOLE_CURSOR_INFO lpConsoleCursorInfo
    );


extern WINBOOL
 PASCAL WINBASEAPI GetNumberOfConsoleMouseButtons(
    LPDWORD lpNumberOfMouseButtons
    );


extern WINBOOL
 PASCAL WINBASEAPI SetConsoleMode(
    HANDLE hConsoleHandle,
    DWORD dwMode
    );


extern WINBOOL
 PASCAL WINBASEAPI SetConsoleActiveScreenBuffer(
    HANDLE hConsoleOutput
    );


extern WINBOOL
 PASCAL WINBASEAPI FlushConsoleInputBuffer(
    HANDLE hConsoleInput
    );


extern WINBOOL
 PASCAL WINBASEAPI SetConsoleScreenBufferSize(
    HANDLE hConsoleOutput,
    COORD dwSize
    );


extern WINBOOL
 PASCAL WINBASEAPI SetConsoleCursorPosition(
    HANDLE hConsoleOutput,
    COORD dwCursorPosition
    );


extern WINBOOL
 PASCAL WINBASEAPI SetConsoleCursorInfo(
    HANDLE hConsoleOutput,
    CONST CONSOLE_CURSOR_INFO *lpConsoleCursorInfo
    );

extern WINBOOL
 PASCAL WINBASEAPI SetConsoleWindowInfo(
    HANDLE hConsoleOutput,
    WINBOOL bAbsolute,
    CONST SMALL_RECT *lpConsoleWindow
    );


extern WINBOOL
 PASCAL WINBASEAPI SetConsoleTextAttribute(
    HANDLE hConsoleOutput,
    WORD wAttributes
    );


extern WINBOOL
 PASCAL WINBASEAPI SetConsoleCtrlHandler(
    PHANDLER_ROUTINE HandlerRoutine,
    WINBOOL Add
    );


extern WINBOOL
 PASCAL WINBASEAPI GenerateConsoleCtrlEvent(
    DWORD dwCtrlEvent,
    DWORD dwProcessGroupId
    );


extern WINBOOL
 PASCAL WINBASEAPI AllocConsole( VOID );


extern WINBOOL
 PASCAL WINBASEAPI FreeConsole( VOID );



extern HANDLE
 PASCAL WINBASEAPI CreateConsoleScreenBuffer(
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    CONST SECURITY_ATTRIBUTES *lpSecurityAttributes,
    DWORD dwFlags,
    LPVOID lpScreenBufferData
    );


extern UINT
 PASCAL WINBASEAPI GetConsoleCP( VOID );


extern WINBOOL
 PASCAL WINBASEAPI SetConsoleCP(
    UINT wCodePageID
    );


extern UINT
 PASCAL WINBASEAPI GetConsoleOutputCP( VOID );


extern WINBOOL
 PASCAL WINBASEAPI SetConsoleOutputCP(
    UINT wCodePageID
    );

WINBOOL
PASCAL WINBASEAPI SetLayeredWindowAttributes(
    HWND hwnd,
    COLORREF crKey,
    BYTE bAlpha,
    DWORD dwFlags);

extern DWORD  PASCAL WINBASEAPI WNetConnectionDialog(
    HWND  hwnd,
    DWORD dwType
    );

extern DWORD  PASCAL WINBASEAPI WNetDisconnectDialog(
    HWND  hwnd,
    DWORD dwType
    );

extern DWORD  PASCAL WINBASEAPI WNetCloseEnum(
    HANDLE   hEnum
    );

extern WINBOOL
 PASCAL WINBASEAPI CloseServiceHandle(
    SC_HANDLE   hSCObject
    );


extern WINBOOL
 PASCAL WINBASEAPI ControlService(
    SC_HANDLE           hService,
    DWORD               dwControl,
    LPSERVICE_STATUS    lpServiceStatus
    );

extern WINBOOL
 PASCAL WINBASEAPI DeleteService(
    SC_HANDLE   hService
    );

extern SC_LOCK
 PASCAL WINBASEAPI LockServiceDatabase(
    SC_HANDLE   hSCManager
    );


extern WINBOOL
 PASCAL WINBASEAPI NotifyBootConfigStatus(
    WINBOOL     BootAcceptable
    );

extern WINBOOL
 PASCAL WINBASEAPI QueryServiceObjectSecurity(
    SC_HANDLE               hService,
    SECURITY_INFORMATION    dwSecurityInformation,
    PSECURITY_DESCRIPTOR    lpSecurityDescriptor,
    DWORD                   cbBufSize,
    LPDWORD                 pcbBytesNeeded
    );


extern WINBOOL
 PASCAL WINBASEAPI QueryServiceStatus(
    SC_HANDLE           hService,
    LPSERVICE_STATUS    lpServiceStatus
    );

extern WINBOOL
 PASCAL WINBASEAPI SetServiceObjectSecurity(
    SC_HANDLE               hService,
    SECURITY_INFORMATION    dwSecurityInformation,
    PSECURITY_DESCRIPTOR    lpSecurityDescriptor
    );


extern WINBOOL
 PASCAL WINBASEAPI SetServiceStatus(
    SERVICE_STATUS_HANDLE   hServiceStatus,
    LPSERVICE_STATUS        lpServiceStatus
    );

extern WINBOOL
 PASCAL WINBASEAPI UnlockServiceDatabase(
    SC_LOCK     ScLock
    );


/* shellapi.h */

extern void
 PASCAL WINBASEAPI DragAcceptFiles(
	HWND,
	WINBOOL
	);


extern void
 PASCAL WINBASEAPI DragFinish(
	HDROP
	);


extern WINBOOL
 PASCAL WINBASEAPI DragQueryPoint(
	HDROP,
	LPPOINT
	);


extern HICON
 PASCAL WINBASEAPI DuplicateIcon(
	HINSTANCE,
	HICON
	);


/* ddeml.h */

extern HCONV  PASCAL WINBASEAPI DdeConnect(DWORD, HSZ, HSZ, CONVCONTEXT *);
extern WINBOOL  PASCAL WINBASEAPI DdeDisconnect(HCONV);
extern WINBOOL  PASCAL WINBASEAPI DdeFreeDataHandle(HDDEDATA);
extern DWORD  PASCAL WINBASEAPI DdeGetData(HDDEDATA, BYTE *, DWORD, DWORD);
extern UINT  PASCAL WINBASEAPI DdeGetLastError(DWORD);
extern HDDEDATA  PASCAL WINBASEAPI DdeNameService(DWORD, HSZ, HSZ, UINT);
extern WINBOOL  PASCAL WINBASEAPI DdePostAdvise(DWORD, HSZ, HSZ);
extern HCONV  PASCAL WINBASEAPI DdeReconnect(HCONV);
extern WINBOOL  PASCAL WINBASEAPI DdeUninitialize(DWORD);

extern DWORD  PASCAL WINBASEAPI NetUserEnum(LPWSTR, DWORD, DWORD, LPBYTE*, DWORD, LPDWORD,
				LPDWORD, LPDWORD);
extern DWORD  PASCAL WINBASEAPI NetApiBufferFree(LPVOID);
extern DWORD  PASCAL WINBASEAPI NetUserGetInfo(LPWSTR, LPWSTR, DWORD, LPBYTE);
extern DWORD  PASCAL WINBASEAPI NetGetDCName(LPWSTR, LPWSTR, LPBYTE*);
extern DWORD  PASCAL WINBASEAPI NetGroupEnum(LPWSTR, DWORD, LPBYTE*, DWORD, LPDWORD,
				LPDWORD, LPDWORD);
extern DWORD  PASCAL WINBASEAPI NetLocalGroupEnum(LPWSTR, DWORD, LPBYTE*, DWORD, LPDWORD,
				LPDWORD, LPDWORD);


/* Extensions to OpenGL */

extern int  PASCAL WINBASEAPI ChoosePixelFormat(HDC, CONST PIXELFORMATDESCRIPTOR *);	

extern int  PASCAL WINBASEAPI DescribePixelFormat(HDC, int, UINT, LPPIXELFORMATDESCRIPTOR);

extern UINT  PASCAL WINBASEAPI GetEnhMetaFilePixelFormat(HENHMETAFILE, DWORD,
			  CONST PIXELFORMATDESCRIPTOR *);

/* extern int  PASCAL WINBASEAPI GetPixelFormat(HDC); */

extern WINBOOL  PASCAL WINBASEAPI SetPixelFormat(HDC, int, CONST PIXELFORMATDESCRIPTOR *);

extern WINBOOL  PASCAL WINBASEAPI SwapBuffers(HDC);

extern HGLRC  PASCAL WINBASEAPI wglCreateContext(HDC);

extern HGLRC  PASCAL WINBASEAPI wglCreateLayerContext(HDC, int);

extern WINBOOL  PASCAL WINBASEAPI wglCopyContext(HGLRC, HGLRC, UINT);

extern WINBOOL  PASCAL WINBASEAPI wglDeleteContext(HGLRC);

extern WINBOOL  PASCAL WINBASEAPI wglDescribeLayerPlane(HDC, int, int, UINT, LPLAYERPLANEDESCRIPTOR);

extern HGLRC  PASCAL WINBASEAPI wglGetCurrentContext(VOID);

extern HDC  PASCAL WINBASEAPI wglGetCurrentDC(VOID);

extern int  PASCAL WINBASEAPI wglGetLayerPaletteEntries(HDC, int, int, int, CONST COLORREF *);

extern PROC  PASCAL WINBASEAPI wglGetProcAddress(LPCSTR);

extern WINBOOL  PASCAL WINBASEAPI wglMakeCurrent(HDC, HGLRC);

extern WINBOOL  PASCAL WINBASEAPI wglRealizeLayerPalette(HDC, int, WINBOOL);

extern int  PASCAL WINBASEAPI wglSetLayerPaletteEntries(HDC, int, int, int, CONST COLORREF *);

extern WINBOOL  PASCAL WINBASEAPI wglShareLists(HGLRC, HGLRC);

extern WINBOOL  PASCAL WINBASEAPI wglSwapLayerBuffers(HDC, UINT);

extern WINBOOL  PASCAL WINBASEAPI IsValidCodePage(UINT CodePage);

extern UINT  PASCAL WINBASEAPI GetACP(void);

extern UINT  PASCAL WINBASEAPI GetOEMCP(void);

extern WINBOOL  PASCAL WINBASEAPI GetCPInfo(UINT CodePage, LPCPINFO lpCPInfo);

extern WINBOOL  PASCAL WINBASEAPI IsDBCSLeadByte(BYTE TestChar);

extern WINBOOL  PASCAL WINBASEAPI IsDBCSLeadByteEx(UINT CodePage, BYTE TestChar);

extern int  PASCAL WINBASEAPI MultiByteToWideChar(UINT CodePage,DWORD dwFlags,LPCSTR lpMultiByteStr,
						int cbMultiByte,LPWSTR lpWideCharStr,int cchWideChar);

extern int  PASCAL WINBASEAPI WideCharToMultiByte(UINT CodePage,DWORD dwFlags,LPCWSTR lpWideCharStr,
						int cchWideChar, LPSTR lpMultiByteStr,int cbMultiByte,
						LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar);


/* ansi compatibility for win 3.1 */
#define AnsiToOem CharToOemA
#define OemToAnsi OemToCharA
#define AnsiToOemBuff CharToOemBuffA
#define OemToAnsiBuff OemToCharBuffA
#define AnsiUpper CharUpperA
#define AnsiUpperBuff CharUpperBuffA
#define AnsiLower CharLowerA
#define AnsiLowerBuff CharLowerBuffA
#define AnsiNext CharNextA
#define AnsiPrev CharPrevA

/* objbase */
#define WINOLEAPI        EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
#define WINOLEAPI_(type) EXTERN_C DECLSPEC_IMPORT type STDAPICALLTYPE
WINOLEAPI CoInitialize(PVOID);
WINOLEAPI CoInitializeEx(LPVOID,DWORD);
WINOLEAPI_(void) CoUninitialize(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* RC_INVOKED */

#endif /* _GNU_H_WINDOWS32_COMMONFUNCTIONS */
