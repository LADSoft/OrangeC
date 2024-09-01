#ifndef _SFC_H
#define _SFC_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows File Protection definitions */

#ifdef __cplusplus
extern "C" {
#endif

#define SFC_DISABLE_NORMAL  0
#define SFC_DISABLE_ASK  1
#define SFC_DISABLE_ONCE  2
#define SFC_DISABLE_SETUP  3
#define SFC_DISABLE_NOPOPUPS  4

#define SFC_SCAN_NORMAL  0
#define SFC_SCAN_ALWAYS  1
#define SFC_SCAN_ONCE  2
#define SFC_SCAN_IMMEDIATE  3

#define SFC_QUOTA_DEFAULT  50
#define SFC_QUOTA_ALL_FILES  ((ULONG)-1)

#define SFC_IDLE_TRIGGER  L"WFP_IDLE_TRIGGER"

typedef struct _PROTECTED_FILE_DATA {
    WCHAR FileName[MAX_PATH];
    DWORD FileNumber;
} PROTECTED_FILE_DATA, *PPROTECTED_FILE_DATA;

BOOL WINAPI SfcGetNextProtectedFile(HANDLE,PPROTECTED_FILE_DATA);
BOOL WINAPI SfcIsFileProtected(HANDLE,LPCWSTR);
BOOL WINAPI SfcIsKeyProtected(HKEY,LPCWSTR,REGSAM); 
BOOL SfpVerifyFile(LPCTSTR,LPTSTR,DWORD);

#ifdef __cplusplus
}
#endif

#endif /* _SFC_H */
