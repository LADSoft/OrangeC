#ifndef _LOADPERF_H
#define _LOADPERF_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Performance Monitor counter string definitions */

#ifdef __cplusplus
extern "C" {
#endif

#define LOADPERF_FUNCTION  __declspec(dllimport) DWORD __stdcall

#define LOADPERF_FLAGS_DELETE_MOF_ON_EXIT  ((ULONG_PTR)1)
#define LOADPERF_FLAGS_LOAD_REGISTRY_ONLY  ((ULONG_PTR)2)
#define LOADPERF_FLAGS_CREATE_MOF_ONLY  ((ULONG_PTR)4)
#define LOADPERF_FLAGS_DISPLAY_USER_MSGS  ((ULONG_PTR)8)

LOADPERF_FUNCTION InstallPerfDllW(LPCWSTR szComputerName, LPCWSTR, ULONG_PTR);
LOADPERF_FUNCTION InstallPerfDllA(LPCSTR szComputerName, LPCSTR, ULONG_PTR);
LOADPERF_FUNCTION LoadPerfCounterTextStringsA(LPSTR, BOOL);
LOADPERF_FUNCTION LoadPerfCounterTextStringsW(LPWSTR, BOOL);
LOADPERF_FUNCTION UnloadPerfCounterTextStringsW(LPWSTR, BOOL);
LOADPERF_FUNCTION UnloadPerfCounterTextStringsA(LPSTR, BOOL);
LOADPERF_FUNCTION UpdatePerfNameFilesA(LPCSTR, LPCSTR, LPSTR, ULONG_PTR);
LOADPERF_FUNCTION UpdatePerfNameFilesW(LPCWSTR, LPCWSTR, LPWSTR, ULONG_PTR);
LOADPERF_FUNCTION SetServiceAsTrustedA(LPCSTR, LPCSTR);
LOADPERF_FUNCTION SetServiceAsTrustedW(LPCWSTR, LPCWSTR);
DWORD BackupPerfRegistryToFileW(LPCWSTR, LPCWSTR);
DWORD RestorePerfRegistryFromFileW(LPCWSTR, LPCWSTR);

#ifdef UNICODE
#define InstallPerfDll  InstallPerfDllW
#define LoadPerfCounterTextStrings  LoadPerfCounterTextStringsW
#define UnloadPerfCounterTextStrings  UnloadPerfCounterTextStringsW
#define UpdatePerfNameFiles  UpdatePerfNameFilesW 
#define SetServiceAsTrusted  SetServiceAsTrustedW
#else /* !UNICODE */
#define InstallPerfDll  InstallPerfDllA
#define LoadPerfCounterTextStrings  LoadPerfCounterTextStringsA
#define UnloadPerfCounterTextStrings  UnloadPerfCounterTextStringsA
#define UpdatePerfNameFiles  UpdatePerfNameFilesA
#define SetServiceAsTrusted  SetServiceAsTrustedA
#endif /* !UNICODE */

#ifdef __cplusplus
}
#endif

#endif /* _LOADPERF_H */
