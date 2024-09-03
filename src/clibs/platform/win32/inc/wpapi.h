#ifndef _WPAPI_H
#define _WPAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Microsoft Web Publishing API definitions */

#include <windows.h>
#include "wptypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <pshpack4.h>

#define WPF_NO_RECURSIVE_POST  0x00000001
#define WPF_NO_WIZARD  0x00000002
#define WPF_MINIMAL_UI  0x00000004
#define WPF_FIRST_FILE_AS_DEFAULT  0x00000008
#define WPF_NO_DIRECT_LOOKUP  0x00000010
#define WPF_NO_URL_LOOKUP  0x00000020
#define WPF_USE_PROVIDER_PASSED  0x00000040
#define WPF_NO_VERIFY_POST  0x00000080
#define WPF_NO_PROGRESS_DLGS  0x00000200
#define WPF_SHOWPAGE_WELCOME  0x00010000
#define WPF_SHOWPAGE_SRCFILE  0x00020000
#define WPF_SHOWPAGE_DESTSITE  0x00040000
#define WPF_SHOWPAGE_PROVIDER  0x00080000
#define WPF_ENABLE_LOCAL_BASE_DIR  0x00100000

#define WPF_NO_UI  (WPF_NO_WIZARD|WPF_NO_PROGRESS_DLGS)
#define WPF_SHOWPAGE_ALL  (WPF_SHOWPAGE_WELCOME|WPF_SHOWPAGE_SRCFILE|WPF_SHOWPAGE_DESTSITE|WPF_SHOWPAGE_PROVIDER)

#define MAX_SITENAME_LEN  128

DWORD WINAPI WpPostW(HWND,DWORD,LPWSTR*,LPDWORD,LPWSTR,LPDWORD,LPWSTR,DWORD);
DWORD WINAPI WpListSitesW(LPDWORD,LPWPSITEINFOW,LPDWORD);
DWORD WINAPI WpDoesSiteExistW(LPCWSTR,BOOL*);
DWORD WINAPI WpDeleteSiteW(LPCWSTR);
DWORD WINAPI WpBindToSiteW(HWND,LPCWSTR,LPCWSTR,DWORD,DWORD,PVOID*);
DWORD WINAPI WpCreateSiteW(LPCWSTR,LPCWSTR,LPCWSTR,LPCWSTR,DWORD);
DWORD WINAPI WpEnumProvidersW(LPDWORD,LPWPPROVINFOW,LPDWORD);
DWORD WINAPI WpGetErrorStringW(UINT,LPWSTR,LPDWORD);

DWORD WINAPI WpPostA(HWND,DWORD,LPSTR*,LPDWORD,LPSTR,LPDWORD,LPSTR,DWORD);
DWORD WINAPI WpListSitesA(LPDWORD,LPWPSITEINFOA,LPDWORD);
DWORD WINAPI WpDoesSiteExistA(LPCSTR,BOOL*);
DWORD WINAPI WpDeleteSiteA(LPCSTR);
DWORD WINAPI WpBindToSiteA(HWND,LPCSTR,LPCSTR,DWORD,DWORD,PVOID*);
DWORD WINAPI WpCreateSiteA(LPCSTR,LPCSTR,LPCSTR,LPCSTR,DWORD);
DWORD WINAPI WpEnumProvidersA(LPDWORD,LPWPPROVINFOA,LPDWORD);
DWORD WINAPI WpGetErrorStringA(UINT,LPSTR,LPDWORD);

#ifdef UNICODE
#define WpPost WpPostW
#define WpListSites WpListSitesW
#define WpDoesSiteExist WpDoesSiteExistW
#define WpDeleteSite WpDeleteSiteW
#define WpBindToSite WpBindToSiteW
#define WpCreateSite WpCreateSiteW
#define WpEnumProviders WpEnumProvidersW
#define WpGetErrorString WpGetErrorStringW
#else
#define WpPost WpPostA
#define WpListSites WpListSitesA
#define WpDoesSiteExist WpDoesSiteExistA
#define WpDeleteSite WpDeleteSiteA
#define WpBindToSite WpBindToSiteA
#define WpCreateSite WpCreateSiteA
#define WpEnumProviders WpEnumProvidersA
#define WpGetErrorString WpGetErrorStringA
#endif /* UNICODE */

#include <poppack.h>

#ifdef __cplusplus
}
#endif

#endif /* _WPAPI_H */
