#ifndef _PSAPI_H
#define _PSAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows PSAPI.DLL definitions */

#ifdef __cplusplus
extern "C" {
#endif


#define LIST_MODULES_DEFAULT  0x00
#define LIST_MODULES_32BIT  0x01
#define LIST_MODULES_64BIT  0x02
#define LIST_MODULES_ALL  (LIST_MODULES_32BIT|LIST_MODULES_64BIT)

#ifndef PSAPI_VERSION
#if (NTDDI_VERSION >= NTDDI_WIN7)
#define PSAPI_VERSION  2
#else /* NTDDI_VERSION < NTDDI_WIN7 */
#define PSAPI_VERSION  1
#endif /* NTDDI_VERSION < NTDDI_WIN7 */
#endif /* PSAPI_VERSION */

#if (PSAPI_VERSION > 1)
#define EnumProcesses  K32EnumProcesses
#define EnumProcessModules  K32EnumProcessModules
#define EnumProcessModulesEx  K32EnumProcessModulesEx
#define GetModuleBaseNameA  K32GetModuleBaseNameA
#define GetModuleBaseNameW  K32GetModuleBaseNameW
#define GetModuleFileNameExA  K32GetModuleFileNameExA
#define GetModuleFileNameExW  K32GetModuleFileNameExW
#define GetModuleInformation  K32GetModuleInformation
#define EmptyWorkingSet  K32EmptyWorkingSet
#define QueryWorkingSet  K32QueryWorkingSet
#define QueryWorkingSetEx  K32QueryWorkingSetEx
#define InitializeProcessForWsWatch  K32InitializeProcessForWsWatch
#define GetWsChanges  K32GetWsChanges
#define GetWsChangesEx  K32GetWsChangesEx
#define GetMappedFileNameW  K32GetMappedFileNameW
#define GetMappedFileNameA  K32GetMappedFileNameA
#define EnumDeviceDrivers  K32EnumDeviceDrivers
#define GetDeviceDriverBaseNameA  K32GetDeviceDriverBaseNameA
#define GetDeviceDriverBaseNameW  K32GetDeviceDriverBaseNameW
#define GetDeviceDriverFileNameA  K32GetDeviceDriverFileNameA
#define GetDeviceDriverFileNameW  K32GetDeviceDriverFileNameW
#define GetProcessMemoryInfo  K32GetProcessMemoryInfo
#define GetPerformanceInfo  K32GetPerformanceInfo
#define EnumPageFilesW  K32EnumPageFilesW
#define EnumPageFilesA  K32EnumPageFilesA
#define GetProcessImageFileNameA  K32GetProcessImageFileNameA
#define GetProcessImageFileNameW  K32GetProcessImageFileNameW
#endif /* PSAPI_VERSION > 1 */

typedef struct _MODULEINFO {
    LPVOID lpBaseOfDll;
    DWORD SizeOfImage;
    LPVOID EntryPoint;
} MODULEINFO, *LPMODULEINFO;

typedef union _PSAPI_WORKING_SET_BLOCK {
    ULONG_PTR Flags;
    struct {
        ULONG_PTR Protection:5;
        ULONG_PTR ShareCount:3;
        ULONG_PTR Shared:1;
        ULONG_PTR Reserved:3;
#ifdef _WIN64
        ULONG_PTR VirtualPage:52;
#else /* !_WIN64 */
        ULONG_PTR VirtualPage:20;
#endif /* !_WIN64 */
    };
} PSAPI_WORKING_SET_BLOCK, *PPSAPI_WORKING_SET_BLOCK;

typedef struct _PSAPI_WORKING_SET_INFORMATION {
    ULONG_PTR NumberOfEntries;
    PSAPI_WORKING_SET_BLOCK WorkingSetInfo[1];
} PSAPI_WORKING_SET_INFORMATION, *PPSAPI_WORKING_SET_INFORMATION;

typedef union _PSAPI_WORKING_SET_EX_BLOCK {
    ULONG_PTR Flags;
    struct {
        ULONG_PTR Valid:1;
        ULONG_PTR ShareCount:3;
        ULONG_PTR Win32Protection:11;
        ULONG_PTR Shared:1;
        ULONG_PTR Node:6;
        ULONG_PTR Locked:1;
        ULONG_PTR LargePage:1;
    };
} PSAPI_WORKING_SET_EX_BLOCK, *PPSAPI_WORKING_SET_EX_BLOCK;

typedef struct _PSAPI_WORKING_SET_EX_INFORMATION {
    PVOID VirtualAddress;
    PSAPI_WORKING_SET_EX_BLOCK VirtualAttributes;
} PSAPI_WORKING_SET_EX_INFORMATION, *PPSAPI_WORKING_SET_EX_INFORMATION;

typedef struct _PSAPI_WS_WATCH_INFORMATION {
    LPVOID FaultingPc;
    LPVOID FaultingVa;
} PSAPI_WS_WATCH_INFORMATION, *PPSAPI_WS_WATCH_INFORMATION;

typedef struct _PSAPI_WS_WATCH_INFORMATION_EX {
    PSAPI_WS_WATCH_INFORMATION BasicInfo;
    ULONG_PTR FaultingThreadId;
    ULONG_PTR Flags;
} PSAPI_WS_WATCH_INFORMATION_EX, *PPSAPI_WS_WATCH_INFORMATION_EX;

typedef struct _PROCESS_MEMORY_COUNTERS {
    DWORD cb;
    DWORD PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
} PROCESS_MEMORY_COUNTERS;
typedef PROCESS_MEMORY_COUNTERS *PPROCESS_MEMORY_COUNTERS;

#if _WIN32_WINNT >= 0x0501
typedef struct _PROCESS_MEMORY_COUNTERS_EX {
    DWORD cb;
    DWORD PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivateUsage;
} PROCESS_MEMORY_COUNTERS_EX;
typedef PROCESS_MEMORY_COUNTERS_EX *PPROCESS_MEMORY_COUNTERS_EX;
#endif /* _WIN32_WINNT >= 0x0501 */

typedef struct _PERFORMANCE_INFORMATION {
    DWORD cb;
    SIZE_T CommitTotal;
    SIZE_T CommitLimit;
    SIZE_T CommitPeak;
    SIZE_T PhysicalTotal;
    SIZE_T PhysicalAvailable;
    SIZE_T SystemCache;
    SIZE_T KernelTotal;
    SIZE_T KernelPaged;
    SIZE_T KernelNonpaged;
    SIZE_T PageSize;
    DWORD HandleCount;
    DWORD ProcessCount;
    DWORD ThreadCount;
} PERFORMANCE_INFORMATION, *PPERFORMANCE_INFORMATION, PERFORMACE_INFORMATION, *PPERFORMACE_INFORMATION;

typedef struct _ENUM_PAGE_FILE_INFORMATION {
    DWORD cb;
    DWORD Reserved;
    SIZE_T TotalSize;
    SIZE_T TotalInUse;
    SIZE_T PeakUsage;
} ENUM_PAGE_FILE_INFORMATION, *PENUM_PAGE_FILE_INFORMATION;

typedef BOOL (*PENUM_PAGE_FILE_CALLBACKW)(LPVOID,PENUM_PAGE_FILE_INFORMATION,LPCWSTR);
typedef BOOL (*PENUM_PAGE_FILE_CALLBACKA)(LPVOID,PENUM_PAGE_FILE_INFORMATION,LPCSTR);

BOOL WINAPI EnumProcesses(DWORD*,DWORD,DWORD*);
BOOL WINAPI EnumProcessModules(HANDLE,HMODULE*,DWORD,LPDWORD);
BOOL WINAPI EnumProcessModulesEx(HANDLE,HMODULE*,DWORD,LPDWORD,DWORD);
DWORD WINAPI GetModuleBaseNameA(HANDLE,HMODULE,LPSTR,DWORD);
DWORD WINAPI GetModuleBaseNameW(HANDLE,HMODULE,LPWSTR,DWORD);
DWORD WINAPI GetModuleFileNameExA(HANDLE,HMODULE,LPSTR,DWORD);
DWORD WINAPI GetModuleFileNameExW(HANDLE,HMODULE,LPWSTR,DWORD);
BOOL WINAPI GetModuleInformation(HANDLE,HMODULE,LPMODULEINFO,DWORD);
BOOL WINAPI EmptyWorkingSet(HANDLE);
BOOL WINAPI QueryWorkingSet(HANDLE,PVOID,DWORD);
BOOL WINAPI QueryWorkingSetEx(HANDLE,PVOID,DWORD);
BOOL WINAPI InitializeProcessForWsWatch(HANDLE);
BOOL WINAPI GetWsChanges(HANDLE,PPSAPI_WS_WATCH_INFORMATION,DWORD);
BOOL WINAPI GetWsChangesEx(HANDLE,PPSAPI_WS_WATCH_INFORMATION_EX,PDWORD);
DWORD WINAPI GetMappedFileNameW(HANDLE,LPVOID,LPWSTR,DWORD);
DWORD WINAPI GetMappedFileNameA(HANDLE,LPVOID,LPSTR,DWORD);
BOOL WINAPI EnumDeviceDrivers(LPVOID*,DWORD,LPDWORD);
DWORD WINAPI GetDeviceDriverBaseNameA(LPVOID,LPSTR,DWORD);
DWORD WINAPI GetDeviceDriverBaseNameW(LPVOID,LPWSTR,DWORD);
DWORD WINAPI GetDeviceDriverFileNameA(LPVOID,LPSTR,DWORD);
DWORD WINAPI GetDeviceDriverFileNameW(LPVOID,LPWSTR,DWORD);
BOOL WINAPI GetProcessMemoryInfo(HANDLE,PPROCESS_MEMORY_COUNTERS,DWORD);
BOOL WINAPI GetPerformanceInfo(PPERFORMACE_INFORMATION,DWORD);
BOOL WINAPI EnumPageFilesW(PENUM_PAGE_FILE_CALLBACKW,LPVOID);
BOOL WINAPI EnumPageFilesA(PENUM_PAGE_FILE_CALLBACKA,LPVOID);
DWORD WINAPI GetProcessImageFileNameA(HANDLE,LPSTR,DWORD);
DWORD WINAPI GetProcessImageFileNameW(HANDLE,LPWSTR,DWORD);

#ifdef UNICODE
#define GetModuleBaseName GetModuleBaseNameW
#define GetModuleFileNameEx GetModuleFileNameExW
#define GetMappedFileName GetMappedFileNameW
#define GetDeviceDriverBaseName GetDeviceDriverBaseNameW
#define GetDeviceDriverFileName GetDeviceDriverFileNameW
#define PENUM_PAGE_FILE_CALLBACK PENUM_PAGE_FILE_CALLBACKW
#define EnumPageFiles EnumPageFilesW
#define GetProcessImageFileName  GetProcessImageFileNameW
#else
#define GetModuleBaseName GetModuleBaseNameA
#define GetModuleFileNameEx GetModuleFileNameExA
#define GetMappedFileName GetMappedFileNameA
#define GetDeviceDriverBaseName GetDeviceDriverBaseNameA
#define GetDeviceDriverFileName GetDeviceDriverFileNameA
#define PENUM_PAGE_FILE_CALLBACK PENUM_PAGE_FILE_CALLBACKA
#define EnumPageFiles EnumPageFilesA
#define GetProcessImageFileName  GetProcessImageFileNameA
#endif


#ifdef __cplusplus
}
#endif

#endif /* _PSAPI_H */
