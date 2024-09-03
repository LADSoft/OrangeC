#ifndef _STRINGAPISET_H
#define _STRINGAPISET_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-core-string-l1 */

/* #include <apiset.h> */
#include <apisetcconv.h>
#include <minwindef.h>
#include <winnls.h>

#if (WINVER >= 0x0600)
WINBASEAPI int WINAPI CompareStringEx(LPCWSTR, DWORD, LPCWCH, int, LPCWCH, int, LPNLSVERSIONINFO, LPVOID, LPARAM);
WINBASEAPI int WINAPI CompareStringOrdinal(LPCWCH, int, LPCWCH, int, BOOL);
#endif /* WINVER >= 0x0600 */
WINBASEAPI int WINAPI CompareStringW(LCID, DWORD, PCNZWCH, int, PCNZWCH, int);
WINBASEAPI int WINAPI FoldStringW(DWORD, LPCWCH, int, LPWSTR, int);
WINBASEAPI BOOL WINAPI GetStringTypeExW(LCID, DWORD, LPCWCH, int, LPWORD);
WINBASEAPI BOOL WINAPI GetStringTypeW(DWORD, LPCWCH, int, LPWORD);
WINBASEAPI int WINAPI MultiByteToWideChar(UINT, DWORD, LPCCH, int, LPWSTR, int);
WINBASEAPI int WINAPI WideCharToMultiByte(UINT, DWORD, LPCWCH, int, LPSTR, int, LPCCH, LPBOOL);

#ifdef UNICODE
#define CompareString  CompareStringW
#define FoldString  FoldStringW
#define GetStringTypeEx  GetStringTypeExW
#endif /* UNICODE */

#endif /* STRINGAPISET_H */
