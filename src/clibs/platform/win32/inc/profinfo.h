#ifndef _PROFINFO_H
#define _PROFINFO_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Profile info definitions */

typedef struct _PROFILEINFOA {
    DWORD dwSize;
    DWORD dwFlags;
    LPSTR lpUserName;
    LPSTR lpProfilePath;
    LPSTR lpDefaultPath;
    LPSTR lpServerName;
    LPSTR lpPolicyPath;
    HANDLE hProfile;
} PROFILEINFOA, *LPPROFILEINFOA;

typedef struct _PROFILEINFOW {
    DWORD dwSize;
    DWORD dwFlags;
    LPWSTR lpUserName;
    LPWSTR lpProfilePath;
    LPWSTR lpDefaultPath;
    LPWSTR lpServerName;
    LPWSTR lpPolicyPath;
    HANDLE hProfile;
} PROFILEINFOW, *LPPROFILEINFOW;

#ifdef UNICODE
typedef PROFILEINFOW PROFILEINFO;
typedef LPPROFILEINFOW LPPROFILEINFO;
#else
typedef PROFILEINFOA PROFILEINFO;
typedef LPPROFILEINFOA LPPROFILEINFO;
#endif /* UNICODE */

#endif /* _PROFINFO_H */
