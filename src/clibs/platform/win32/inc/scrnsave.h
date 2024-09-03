#ifndef _SCRNSAVE_H
#define _SCRNSAVE_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows screensaver definitions */

#ifdef __cplusplus
extern "C" {
#endif

#define IDS_DESCRIPTION  1

#define ID_APP  100
#define DLG_SCRNSAVECONFIGURE  2003

#define idsIsPassword  1000
#define idsIniFile  1001
#define idsScreenSaver  1002
#define idsPassword  1003
#define idsDifferentPW  1004
#define idsChangePW  1005
#define idsBadOldPW  1006
#define idsAppName  1007
#define idsNoHelpMemory  1008
#define idsHelpFile  1009
#define idsDefKeyword  1010

#define WS_GT  (WS_GROUP|WS_TABSTOP)

#define MAXFILELEN  13
#define TITLEBARNAMELEN  40
#define APPNAMEBUFFERLEN  40
#define BUFFLEN  255

#define SCRM_VERIFYPW  WM_APP

#ifdef UNICODE
LRESULT WINAPI ScreenSaverProcW(HWND,UINT,WPARAM,LPARAM);
#define ScreenSaverProc ScreenSaverProcW
#else
LRESULT WINAPI ScreenSaverProc(HWND,UINT,WPARAM,LPARAM);
#endif

LRESULT WINAPI DefScreenSaverProc(HWND,UINT,WPARAM,LPARAM);
BOOL WINAPI ScreenSaverConfigureDialog(HWND,UINT,WPARAM,LPARAM);
BOOL WINAPI RegisterDialogClasses(HANDLE);
void WINAPI ScreenSaverChangePassword(HWND);

extern HINSTANCE hMainInstance;
extern HWND hMainWindow;
extern BOOL fChildPreview;
extern TCHAR szName[TITLEBARNAMELEN];
extern TCHAR szAppName[APPNAMEBUFFERLEN];
extern TCHAR szIniFile[MAXFILELEN];
extern TCHAR szScreenSaver[22];
extern TCHAR szHelpFile[MAXFILELEN];
extern TCHAR szNoHelpMemory[BUFFLEN];
extern UINT MyHelpMessage;

#ifdef __cplusplus
}
#endif

#endif /* _SCRNSAVE_H */
