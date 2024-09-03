#ifndef _NAMESPACEAPI_H
#define _NAMESPACEAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-core-namespace-l1 */

/* #include <apiset.h> */
#include <apisetcconv.h>
#include <minwindef.h>
#include <minwinbase.h>

#define PRIVATE_NAMESPACE_FLAG_DESTROY  0x00000001

WINBASEAPI HANDLE WINAPI CreatePrivateNamespaceW(LPSECURITY_ATTRIBUTES, LPVOID, LPCWSTR);
WINBASEAPI HANDLE WINAPI OpenPrivateNamespaceW(LPVOID, LPCWSTR);
WINBASEAPI BOOLEAN WINAPI ClosePrivateNamespace(HANDLE, ULONG);
WINBASEAPI HANDLE WINAPI CreateBoundaryDescriptorW(LPCWSTR, ULONG);
WINBASEAPI BOOL WINAPI AddSIDToBoundaryDescriptor(HANDLE *, PSID);
WINBASEAPI VOID WINAPI DeleteBoundaryDescriptor(HANDLE);

#endif /* _NAMESPACEAPI_H */
