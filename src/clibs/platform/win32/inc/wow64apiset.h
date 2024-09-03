#ifndef _WOW64APISET_H
#define _WOW64APISET_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-core-wow64-l1 */

/* #include <apiset.h> */
#include <apisetcconv.h>
#include <minwindef.h>
#include <minwinbase.h>

#ifdef __cplusplus
extern "C" {
#endif

#if _WIN32_WINNT >= 0x0501

WINBASEAPI BOOL WINAPI Wow64DisableWow64FsRedirection(PVOID *);
WINBASEAPI BOOL WINAPI Wow64RevertWow64FsRedirection(PVOID);
WINBASEAPI BOOL WINAPI IsWow64Process(HANDLE, PBOOL);
WINBASEAPI UINT WINAPI GetSystemWow64DirectoryA(LPSTR, UINT);
WINBASEAPI UINT WINAPI GetSystemWow64DirectoryW(LPWSTR, UINT);
WINBASEAPI UINT WINAPI GetSystemWow64Directory2A(LPSTR, UINT, WORD);
WINBASEAPI UINT WINAPI GetSystemWow64Directory2W(LPWSTR, UINT, WORD);

#ifdef UNICODE
#define GetSystemWow64Directory  GetSystemWow64DirectoryW
#define GetSystemWow64Directory2  GetSystemWow64Directory2W
#else /* !UNICODE */
#define GetSystemWow64Directory  GetSystemWow64DirectoryA
#define GetSystemWow64Directory2  GetSystemWow64Directory2A
#endif /* !UNICODE */

#endif /* _WIN32_WINNT >= 0x0501 */

#if (_WIN32_WINNT >= 0x0603)
WINBASEAPI USHORT WINAPI Wow64SetThreadDefaultGuestMachine(USHORT);
WINBASEAPI BOOL WINAPI IsWow64Process2(HANDLE, USHORT *, USHORT *);
#endif /* _WIN32_WINNT >= 0x0603 */

#ifdef __cplusplus
}
#endif

#endif /* _WOW64APISET_H */
