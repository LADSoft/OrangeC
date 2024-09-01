#ifndef _DATETIMEAPI_H
#define _DATETIMEAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-core-datetime-l1 */

/* #include <apiset.h> */
#include <apisetcconv.h>
#include <minwindef.h>
#include <minwinbase.h>

WINBASEAPI int WINAPI GetDateFormatA(LCID, DWORD, CONST SYSTEMTIME *, LPCSTR, LPSTR, int);
WINBASEAPI int WINAPI GetDateFormatW(LCID, DWORD, CONST SYSTEMTIME *, LPCWSTR, LPWSTR, int);
WINBASEAPI int WINAPI GetTimeFormatA(LCID, DWORD, CONST SYSTEMTIME *, LPCSTR, LPSTR, int);
WINBASEAPI int WINAPI GetTimeFormatW(LCID, DWORD, CONST SYSTEMTIME *, LPCWSTR, LPWSTR, int);
WINBASEAPI int WINAPI GetTimeFormatEx(LPCWSTR, DWORD, CONST SYSTEMTIME *, LPCWSTR, LPWSTR, int);
WINBASEAPI int WINAPI GetDateFormatEx(LPCWSTR, DWORD, CONST SYSTEMTIME *, LPCWSTR, LPWSTR, int, LPCWSTR);
WINBASEAPI int WINAPI GetDurationFormatEx(LPCWSTR, DWORD, CONST SYSTEMTIME *, ULONGLONG, LPCWSTR, LPWSTR, int);

#ifdef UNICODE
#define GetDateFormat  GetDateFormatW
#define GetTimeFormat  GetTimeFormatW
#else /* !UNICODE */
#define GetDateFormat  GetDateFormatA
#define GetTimeFormat  GetTimeFormatA
#endif /* !UNICODE */

#define GetDurationFormatEx_DEFINED

#endif /* _DATETIMEAPI_H */
