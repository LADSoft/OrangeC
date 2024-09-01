#ifndef _SECURITYAPPCONTAINER_H
#define _SECURITYAPPCONTAINER_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-security-appcontainer-l1 */

/* #include <apiset.h> */
#include <apisetcconv.h>

#include <minwindef.h>
#include <minwinbase.h>

#if (NTDDI_VERSION >= NTDDI_WIN8)
BOOL WINAPI GetAppContainerNamedObjectPath(HANDLE, PSID, ULONG, LPWSTR, PULONG);
#endif /* NTDDI_VERSION >= NTDDI_WIN8 */

#endif /* _SECURITYAPPCONTAINER_H */
