#ifndef _SYSINFOAPI_H
#define _SYSINFOAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-core-sysinfo-l1 */

/* #include <apiset.h> */
#include <apisetcconv.h>
#include <minwindef.h>
#include <minwinbase.h>

#define NOT_BUILD_WINDOWS_DEPRECATE  __declspec(deprecated)


typedef struct _SYSTEM_INFO {
    union {
        DWORD dwOemId;
        struct {
            WORD wProcessorArchitecture;
            WORD wReserved;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
    DWORD dwPageSize;
    LPVOID lpMinimumApplicationAddress;
    LPVOID lpMaximumApplicationAddress;
    DWORD_PTR dwActiveProcessorMask;
    DWORD dwNumberOfProcessors;
    DWORD dwProcessorType;
    DWORD dwAllocationGranularity;
    WORD wProcessorLevel;
    WORD wProcessorRevision;
} SYSTEM_INFO, *LPSYSTEM_INFO;

typedef struct _MEMORYSTATUSEX {
    DWORD dwLength;
    DWORD dwMemoryLoad;
    DWORDLONG ullTotalPhys;
    DWORDLONG ullAvailPhys;
    DWORDLONG ullTotalPageFile;
    DWORDLONG ullAvailPageFile;
    DWORDLONG ullTotalVirtual;
    DWORDLONG ullAvailVirtual;
    DWORDLONG ullAvailExtendedVirtual;
} MEMORYSTATUSEX, *LPMEMORYSTATUSEX;

typedef enum _COMPUTER_NAME_FORMAT {
    ComputerNameNetBIOS,
    ComputerNameDnsHostname,
    ComputerNameDnsDomain,
    ComputerNameDnsFullyQualified,
    ComputerNamePhysicalNetBIOS,
    ComputerNamePhysicalDnsHostname,
    ComputerNamePhysicalDnsDomain,
    ComputerNamePhysicalDnsFullyQualified,
    ComputerNameMax
} COMPUTER_NAME_FORMAT;

#define SCEX2_ALT_NETBIOS_NAME 0x00000001

WINBASEAPI BOOL WINAPI GlobalMemoryStatusEx(LPMEMORYSTATUSEX);
WINBASEAPI VOID WINAPI GetSystemInfo(LPSYSTEM_INFO);
WINBASEAPI VOID WINAPI GetSystemTime(LPSYSTEMTIME);
WINBASEAPI VOID WINAPI GetSystemTimeAsFileTime(LPFILETIME);
WINBASEAPI VOID WINAPI GetLocalTime(LPSYSTEMTIME);
NOT_BUILD_WINDOWS_DEPRECATE WINBASEAPI DWORD WINAPI GetVersion(VOID);
WINBASEAPI BOOL WINAPI SetLocalTime(CONST SYSTEMTIME *);
WINBASEAPI DWORD WINAPI GetTickCount(VOID);
#if (_WIN32_WINNT >= 0x0600)
WINBASEAPI ULONGLONG WINAPI GetTickCount64(VOID);
#endif /* _WIN32_WINNT >= 0x0600 */
WINBASEAPI BOOL WINAPI GetSystemTimeAdjustment(PDWORD, PDWORD, PBOOL);
WINBASEAPI UINT WINAPI GetSystemDirectoryA(LPSTR, UINT);
WINBASEAPI UINT WINAPI GetSystemDirectoryW(LPWSTR, UINT);
WINBASEAPI UINT WINAPI GetWindowsDirectoryA(LPSTR, UINT);
WINBASEAPI UINT WINAPI GetWindowsDirectoryW(LPWSTR, UINT);
WINBASEAPI UINT WINAPI GetSystemWindowsDirectoryA(LPSTR, UINT);
WINBASEAPI UINT WINAPI GetSystemWindowsDirectoryW(LPWSTR, UINT);
WINBASEAPI BOOL WINAPI GetComputerNameExA(COMPUTER_NAME_FORMAT, LPSTR, LPDWORD);
WINBASEAPI BOOL WINAPI GetComputerNameExW(COMPUTER_NAME_FORMAT, LPWSTR, LPDWORD);
WINBASEAPI BOOL WINAPI SetComputerNameExW(COMPUTER_NAME_FORMAT, LPCWSTR);
WINBASEAPI BOOL WINAPI SetSystemTime(CONST SYSTEMTIME *);
NOT_BUILD_WINDOWS_DEPRECATE WINBASEAPI BOOL WINAPI GetVersionExA(LPOSVERSIONINFOA);
NOT_BUILD_WINDOWS_DEPRECATE WINBASEAPI BOOL WINAPI GetVersionExW(LPOSVERSIONINFOW);
WINBASEAPI BOOL WINAPI GetLogicalProcessorInformation(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD);
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN7)
WINBASEAPI BOOL WINAPI GetLogicalProcessorInformationEx(LOGICAL_PROCESSOR_RELATIONSHIP, PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, PDWORD);
#endif /* _WIN32_WINNT >= _WIN32_WINNT_WIN7 */
#if (_WIN32_WINNT >= _WIN32_WINNT_WINXP)
WINBASEAPI VOID WINAPI GetNativeSystemInfo(LPSYSTEM_INFO);
#endif /* _WIN32_WINNT >= _WIN32_WINNT_WINXP */
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
WINBASEAPI VOID WINAPI GetSystemTimePreciseAsFileTime(LPFILETIME);
#endif /* _WIN32_WINNT >= _WIN32_WINNT_WIN8 */
#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
WINBASEAPI BOOL WINAPI GetProductInfo(DWORD, DWORD, DWORD, DWORD, PDWORD);
#endif /* _WIN32_WINNT >= _WIN32_WINNT_VISTA */
#if (NTDDI_VERSION >= NTDDI_WIN2K)
NTSYSAPI ULONGLONG NTAPI VerSetConditionMask(ULONGLONG, ULONG, UCHAR);
#endif /* NTDDI_VERSION >= NTDDI_WIN2K */
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
WINBASEAPI BOOL WINAPI GetOsSafeBootMode(PDWORD);
#endif /* _WIN32_WINNT >= _WIN32_WINNT_WIN8 */
WINBASEAPI UINT WINAPI EnumSystemFirmwareTables(DWORD, PVOID, DWORD);
WINBASEAPI UINT WINAPI GetSystemFirmwareTable(DWORD, DWORD, PVOID, DWORD);
WINBASEAPI BOOL WINAPI DnsHostnameToComputerNameExW(LPCWSTR, LPWSTR, LPDWORD);
WINBASEAPI BOOL WINAPI GetPhysicallyInstalledSystemMemory(PULONGLONG);
WINBASEAPI BOOL WINAPI SetComputerNameEx2W(COMPUTER_NAME_FORMAT, DWORD, LPCWSTR);
WINBASEAPI BOOL WINAPI SetSystemTimeAdjustment(DWORD, BOOL);
WINBASEAPI BOOL WINAPI InstallELAMCertificateInfo(HANDLE);
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN7)
WINBASEAPI BOOL WINAPI GetProcessorSystemCycleTime(USHORT Group, PSYSTEM_PROCESSOR_CYCLE_TIME_INFORMATION, PDWORD);
#endif /* _WIN32_WINNT >= _WIN32_WINNT_WIN7 */
#if (_WIN32_WINNT >= _WIN32_WINNT_WINTHRESHOLD)
WINBASEAPI BOOL WINAPI GetOsManufacturingMode(PBOOL);
#endif /* _WIN32_WINNT >= _WIN32_WINNT_WINTHRESHOLD */
#if (_WIN32_WINNT >= _WIN32_WINNT_WINTHRESHOLD)
WINBASEAPI HRESULT WINAPI GetIntegratedDisplaySize(double *);
#endif /* _WIN32_WINNT >= _WIN32_WINNT_WINTHRESHOLD */
WINBASEAPI BOOL WINAPI SetComputerNameA(LPCSTR);
WINBASEAPI BOOL WINAPI SetComputerNameW(LPCWSTR);
WINBASEAPI BOOL WINAPI SetComputerNameExA(COMPUTER_NAME_FORMAT, LPCSTR);

#ifdef UNICODE
#define GetSystemDirectory  GetSystemDirectoryW
#define GetWindowsDirectory  GetWindowsDirectoryW
#define GetSystemWindowsDirectory  GetSystemWindowsDirectoryW
#define GetComputerNameEx  GetComputerNameExW
#define SetComputerNameEx SetComputerNameExW
#define GetVersionEx  GetVersionExW
#define SetComputerNameEx2 SetComputerNameEx2W
#define SetComputerName  SetComputerNameW
#else
#define GetSystemDirectory  GetSystemDirectoryA
#define GetWindowsDirectory  GetWindowsDirectoryA
#define GetSystemWindowsDirectory  GetSystemWindowsDirectoryA
#define GetComputerNameEx  GetComputerNameExA
#define GetVersionEx  GetVersionExA
#define SetComputerName  SetComputerNameA
#define SetComputerNameEx SetComputerNameExA
#endif // !UNICODE


#endif /* _SYSINFOAPI_H */
