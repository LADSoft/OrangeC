#ifndef _POWERBASE_H
#define _POWERBASE_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-power-base-l1-1-0 */

/* #include <apiset.h> */
#include <apisetcconv.h>

#ifndef _HPOWERNOTIFY_DEF_
#define _HPOWERNOTIFY_DEF_
typedef PVOID HPOWERNOTIFY, *PHPOWERNOTIFY;  /* see winuser.h */
#endif /* _HPOWERNOTIFY_DEF_ */

#ifndef NT_SUCCESS
#define NTSTATUS LONG
#define _OVERRIDE_NTSTATUS_
#endif
NTSTATUS WINAPI CallNtPowerInformation(POWER_INFORMATION_LEVEL, PVOID, ULONG, PVOID, ULONG);
#ifdef _OVERRIDE_NTSTATUS_
#undef NTSTATUS
#endif

BOOLEAN WINAPI GetPwrCapabilities(PSYSTEM_POWER_CAPABILITIES);

#if (NTDDI_VERSION >= NTDDI_WIN8)
POWER_PLATFORM_ROLE WINAPI PowerDeterminePlatformRoleEx(ULONG);
DWORD WINAPI PowerRegisterSuspendResumeNotification(DWORD, HANDLE, PHPOWERNOTIFY);
DWORD WINAPI PowerUnregisterSuspendResumeNotification(HPOWERNOTIFY);
#endif /* NTDDI_VERSION >= NTDDI_WIN8 */

#endif /* _POWERBASE_H */
