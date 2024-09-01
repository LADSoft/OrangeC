#ifndef _WPTYPES_H
#define _WPTYPES_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Microsoft Web Publishing API data type definitions */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagWPSITEINFOA {
    DWORD dwSize;
    DWORD dwFlags;
    LPSTR lpszSiteName;
    LPSTR lpszSiteURL;
} WPSITEINFOA, *LPWPSITEINFOA;

typedef struct tagWPSITEINFOW {
    DWORD dwSize;
    DWORD dwFlags;
    LPWSTR lpszSiteName;
    LPWSTR lpszSiteURL;
} WPSITEINFOW, *LPWPSITEINFOW;

typedef struct tagWPPROVINFOA {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwPriority;
    LPSTR lpszProviderName;
    LPSTR lpszProviderCLSID;
    LPSTR lpszDllPath;
} WPPROVINFOA, *LPWPPROVINFOA;

typedef struct tagWPPROVINFOW {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwPriority;
    LPWSTR lpszProviderName;
    LPWSTR lpszProviderCLSID;
    LPWSTR lpszDllPath;
} WPPROVINFOW, *LPWPPROVINFOW;

#ifdef UNICODE
#define WPSITEINFO WPSITEINFOW
#define LPWPSITEINFO LPWPSITEINFOW
#define WPPROVINFO WPPROVINFOW
#define LPWPPROVINFO LPWPPROVINFOW
#else
#define WPSITEINFO WPSITEINFOA
#define LPWPSITEINFO LPWPSITEINFOA
#define WPPROVINFO WPPROVINFOA
#define LPWPPROVINFO LPWPPROVINFOA
#endif

#ifdef __cplusplus
}
#endif

#endif /* _WPTYPES_H */

