#ifndef _POWERSETTING_H
#define _POWERSETTING_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-power-setting-l1-1-0 */

/* #include <apiset.h> */
#include <apisetcconv.h>

#ifndef _HPOWERNOTIFY_DEF_
#define _HPOWERNOTIFY_DEF_
typedef PVOID HPOWERNOTIFY, *PHPOWERNOTIFY;  /* see winuser.h */
#endif // _HPOWERNOTIFY_DEF_

#if (NTDDI_VERSION >= NTDDI_VISTA)
DWORD WINAPI PowerReadACValue(HKEY, CONST GUID *, CONST GUID *, CONST GUID *, PULONG, LPBYTE, LPDWORD);
DWORD WINAPI PowerReadDCValue(HKEY RootPowerKey, CONST GUID *, CONST GUID *, CONST GUID *, PULONG, PUCHAR, LPDWORD);
DWORD WINAPI PowerWriteACValueIndex(HKEY, CONST GUID *, CONST GUID *, CONST GUID *, DWORD);
DWORD WINAPI PowerWriteDCValueIndex(HKEY, CONST GUID *, CONST GUID *, CONST GUID *, DWORD);
DWORD WINAPI PowerGetActiveScheme(HKEY, GUID **);
DWORD WINAPI PowerSetActiveScheme(HKEY, CONST GUID *);
#endif /* NTDDI_VERSION >= NTDDI_VISTA */

#if (NTDDI_VERSION >= NTDDI_WIN7)
DWORD WINAPI PowerSettingRegisterNotification(LPCGUID, DWORD, HANDLE, PHPOWERNOTIFY);
DWORD WINAPI PowerSettingUnregisterNotification(HPOWERNOTIFY);
#endif /* NTDDI_VERSION >= NTDDI_WIN7 */

#endif /* _POWERSETTING_H */
