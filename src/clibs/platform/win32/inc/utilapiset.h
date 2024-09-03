#ifndef _UTILAPISET_H
#define _UTILAPISET_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-core-util-l1-1-0 */

/* #include <apiset.h> */
#include <apisetcconv.h>
#include <minwindef.h>

WINBASEAPI PVOID WINAPI EncodePointer(PVOID);
WINBASEAPI PVOID WINAPI DecodePointer(PVOID);
WINBASEAPI PVOID WINAPI EncodeSystemPointer(PVOID);
WINBASEAPI PVOID WINAPI DecodeSystemPointer(PVOID);
WINBASEAPI HRESULT WINAPI EncodeRemotePointer(HANDLE, PVOID, PVOID *);
WINBASEAPI HRESULT WINAPI DecodeRemotePointer(HANDLE, PVOID, PVOID *);
WINBASEAPI BOOL WINAPI Beep(DWORD, DWORD);

#endif /* _UTILAPISET_H */
