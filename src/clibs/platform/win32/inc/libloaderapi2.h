#ifndef _LIBLOADERAPI2_H
#define _LIBLOADERAPI2_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-core-libraryloader-l2 */

/* #include <apiset.h> */
#include <apisetcconv.h>
#include <minwindef.h>
#include <minwinbase.h>

#if (_WIN32_WINNT >= 0x0602)

WINBASEAPI HMODULE WINAPI LoadPackagedLibrary(LPCWSTR, DWORD);
WINBASEAPI BOOL WINAPI QueryOptionalDelayLoadedAPI(HMODULE, LPCSTR, LPCSTR, DWORD);

#endif // _WIN32_WINNT >= 0x0602

#endif /* _LIBLOADERAPI2_H */
