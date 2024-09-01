#ifndef _WINNLS32_H
#define _WINNLS32_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows NT 3.x compatible FarEast IMM component definitions */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _tagDATETIME {
    WORD year;
    WORD month;
    WORD day;
    WORD hour;
    WORD min;
    WORD sec;
} DATETIME;

typedef struct _tagIMEPROA {
    HWND hWnd;
    DATETIME InstDate;
    UINT wVersion;
    BYTE szDescription[50];
    BYTE szName[80];
    BYTE szOptions[30];
} IMEPROA, *PIMEPROA, *NPIMEPROA, *LPIMEPROA;

typedef struct _tagIMEPROW {
    HWND hWnd;
    DATETIME InstDate;
    UINT wVersion;
    WCHAR szDescription[50];
    WCHAR szName[80];
    WCHAR szOptions[30];
} IMEPROW, *PIMEPROW, *NPIMEPROW, *LPIMEPROW;

BOOL WINAPI IMPGetIMEA(HWND,LPIMEPROA);
BOOL WINAPI IMPGetIMEW(HWND,LPIMEPROW);
BOOL WINAPI IMPQueryIMEA(LPIMEPROA);
BOOL WINAPI IMPQueryIMEW(LPIMEPROW);
BOOL WINAPI IMPSetIMEA(HWND,LPIMEPROA);
BOOL WINAPI IMPSetIMEW(HWND,LPIMEPROW);
UINT WINAPI WINNLSGetIMEHotkey(HWND);
BOOL WINAPI WINNLSEnableIME(HWND,BOOL);
BOOL WINAPI WINNLSGetEnableStatus(HWND);

#ifdef UNICODE
typedef IMEPROW IMEPRO;
typedef PIMEPROW PIMEPRO;
typedef NPIMEPROW NPIMEPRO;
typedef LPIMEPROW LPIMEPRO;
#define IMPGetIME IMPGetIMEW
#define IMPQueryIME IMPQueryIMEW
#define IMPSetIME IMPSetIMEW
#else
typedef IMEPROA IMEPRO;
typedef PIMEPROA PIMEPRO;
typedef NPIMEPROA NPIMEPRO;
typedef LPIMEPROA LPIMEPRO;
#define IMPGetIME IMPGetIMEA
#define IMPQueryIME IMPQueryIMEA
#define IMPSetIME IMPSetIMEA
#endif

#ifdef __cplusplus
}
#endif

#endif /* _WINNLS32_H */

