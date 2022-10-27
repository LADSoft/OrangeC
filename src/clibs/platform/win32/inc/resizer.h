/****************************************************************************
 *                                                                          *
 * File    : resizer.h                                                      *
 *                                                                          *
 * Purpose : Definitions for Pelles C Resizable Dialog Manager.             *
 *                                                                          *
 * History : Date      Reason                                               *
 *           09-02-22  Created                                              *
 *                                                                          *
 ****************************************************************************/

#ifndef _RESIZER_H
#define _RESIZER_H

#if defined(_WIN64)
#pragma comment(lib, "resizer64.lib")
#else /* !_WIN64 */
#pragma comment(lib, "resizer.lib")
#endif /* !_WIN64 */

/****** Function prototypes ************************************************/

HWND WINAPI CreateResizableDialogParamA(HINSTANCE, PCSTR, HWND, DLGPROC, LPARAM);                       /* Like CreateDialogParamA() */
HWND WINAPI CreateResizableDialogParamW(HINSTANCE, PCWSTR, HWND, DLGPROC, LPARAM);                      /* Like CreateDialogParamW() */
HWND WINAPI CreateResizableDialogIndirectParamA(HINSTANCE, LPCDLGTEMPLATEA, HWND, DLGPROC, LPARAM);     /* Like CreateDialogIndirectParamA() */
HWND WINAPI CreateResizableDialogIndirectParamW(HINSTANCE, LPCDLGTEMPLATEW, HWND, DLGPROC, LPARAM);     /* Like CreateDialogIndirectParamW() */
INT_PTR WINAPI ResizableDialogBoxParamA(HINSTANCE, PCSTR, HWND, DLGPROC, LPARAM);                       /* Like DialogBoxParamA() */
INT_PTR WINAPI ResizableDialogBoxParamW(HINSTANCE, PCWSTR, HWND, DLGPROC, LPARAM);                      /* Like DialogBoxParamW() */
INT_PTR WINAPI ResizableDialogBoxIndirectParamA(HINSTANCE, LPCDLGTEMPLATEA, HWND, DLGPROC, LPARAM);     /* Like DialogBoxIndirectParamA() */
INT_PTR WINAPI ResizableDialogBoxIndirectParamW(HINSTANCE, LPCDLGTEMPLATEW, HWND, DLGPROC, LPARAM);     /* Like DialogBoxIndirectParamW() */
BOOL WINAPI AdjustResizableDialog(HWND, int /*delta x*/, int /*delta y*/);                              /* Resize dialog without moving controls */

#define CreateResizableDialogA(hInst,pName,hwndParent,pfnDialog)  CreateResizableDialogParamA(hInst,pName,hwndParent,pfnDialog,0L)
#define CreateResizableDialogW(hInst,pName,hwndParent,pfnDialog)  CreateResizableDialogParamW(hInst,pName,hwndParent,pfnDialog,0L)
#define CreateResizableDialogIndirectA(hInst,pTemplate,hwndParent,pfnDialog)  CreateResizableDialogIndirectParamA(hInst,pTemplate,hwndParent,pfnDialog,0L)
#define CreateResizableDialogIndirectW(hInst,pTemplate,hwndParent,pfnDialog)  CreateResizableDialogIndirectParamW(hInst,pTemplate,hwndParent,pfnDialog,0L)
#define ResizableDialogBoxA(hInst,pTemplate,hwndParent,pfnDialog)  ResizableDialogBoxParamA(hInst,pTemplate,hwndParent,pfnDialog,0L)
#define ResizableDialogBoxW(hInst,pTemplate,hwndParent,pfnDialog)  ResizableDialogBoxParamW(hInst,pTemplate,hwndParent,pfnDialog,0L)
#define ResizableDialogBoxIndirectA(hInst,pTemplate,hwndParent,pfnDialog)  ResizableDialogBoxIndirectParamA(hInst,pTemplate,hwndParent,pfnDialog,0L)
#define ResizableDialogBoxIndirectW(hInst,pTemplate,hwndParent,pfnDialog)  ResizableDialogBoxIndirectParamW(hInst,pTemplate,hwndParent,pfnDialog,0L)

#ifdef UNICODE
#define CreateResizableDialog  CreateResizableDialogW
#define CreateResizableDialogParam  CreateResizableDialogParamW
#define CreateResizableDialogIndirect  CreateResizableDialogIndirectW
#define CreateResizableDialogIndirectParam  CreateResizableDialogIndirectParamW
#define ResizableDialogBox  ResizableDialogBoxW
#define ResizableDialogBoxParam  ResizableDialogBoxParamW
#define ResizableDialogBoxIndirect  ResizableDialogBoxIndirectW
#define ResizableDialogBoxIndirectParam  ResizableDialogBoxIndirectParamW
#else /* !UNICODE */
#define CreateResizableDialog  CreateResizableDialogA
#define CreateResizableDialogParam  CreateResizableDialogParamA
#define CreateResizableDialogIndirect  CreateResizableDialogIndirectA
#define CreateResizableDialogIndirectParam  CreateResizableDialogIndirectParamA
#define ResizableDialogBox  ResizableDialogBoxA
#define ResizableDialogBoxParam  ResizableDialogBoxParamA
#define ResizableDialogBoxIndirect  ResizableDialogBoxIndirectA
#define ResizableDialogBoxIndirectParam  ResizableDialogBoxIndirectParamA
#endif /* !UNICODE */

#endif /* _RESIZER_H */
