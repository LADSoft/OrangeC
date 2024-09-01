#ifndef _ENCLAVEAPI_H
#define _ENCLAVEAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-core-enclave-l1-1-0 */

/* #include <apiset.h> */
#include <apisetcconv.h>
#include <minwindef.h>
#include <minwinbase.h>

WINBASEAPI BOOL WINAPI IsEnclaveTypeSupported(DWORD);
WINBASEAPI LPVOID WINAPI CreateEnclave(HANDLE, LPVOID, SIZE_T, SIZE_T, DWORD, LPCVOID, DWORD, LPDWORD);
WINBASEAPI BOOL WINAPI LoadEnclaveData(HANDLE, LPVOID, LPCVOID, SIZE_T, DWORD, LPCVOID, DWORD, PSIZE_T, LPDWORD);
WINBASEAPI BOOL WINAPI InitializeEnclave(HANDLE, LPVOID, LPCVOID, DWORD, LPDWORD);

#endif /* _ENCLAVEAPI_H */
