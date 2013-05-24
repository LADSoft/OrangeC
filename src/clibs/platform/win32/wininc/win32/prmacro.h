/* 
   prmacro.h

   Base definitions

   Copyright (C) 1996 Free Software Foundation, Inc.

   This file is part of the Windows32 API Library.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   If you are interested in a warranty or support for this source code,
   contact Scott Christley <scottc@net-community.com> for more information.
   
   You should have received a copy of the GNU Library General Public
   License along with this library; see the file COPYING.LIB.
   If not, write to the Free Software Foundation, 
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   -----------
   DAL 2003 - this file modified extensively for my compiler.  New
   definitionswnwn added as well.
*/ 
#ifdef __cplusplus
extern "C" {
#endif

#ifndef CCSIZEOF_STRUCT
#define CCSIZEOF_STRUCT(structname, member) \
    (((int)((LPBYTE)(&((structname*)0)->member) - ((LPBYTE)((structname*)0)))) + sizeof(((structname*)0)->member))
#endif

#define PropSheet_AddPage(hDlg, hpage) \
    SNDMSG(hDlg, PSM_ADDPAGE, 0, (LPARAM)hpage)
#define PropSheet_Apply(hDlg) \
    SNDMSG(hDlg, PSM_APPLY, 0, 0L)
#define PropSheet_CancelToClose(hDlg) \
    PostMessage(hDlg, PSM_CANCELTOCLOSE, 0, 0L)
#define PropSheet_Changed(hDlg, hwnd) \
    SNDMSG(hDlg, PSM_CHANGED, (WPARAM)hwnd, 0L)
#define PropSheet_GetCurrentPageHwnd(hDlg) \
    (HWND)SNDMSG(hDlg, PSM_GETCURRENTPAGEHWND, 0, 0L)
#define PropSheet_GetResult(hDlg) \
    SNDMSG(hDlg, PSM_GETRESULT, 0, 0)
#define PropSheet_GetTabControl(hDlg) \
    (HWND)SNDMSG(hDlg, PSM_GETTABCONTROL, 0, 0)
#define PropSheet_HwndToIndex(hDlg, hwnd) \
    (int)SNDMSG(hDlg, PSM_HWNDTOINDEX, (WPARAM)(hwnd), 0)
#define PropSheet_IdToIndex(hDlg, id) \
    (int)SNDMSG(hDlg, PSM_IDTOINDEX, 0, (LPARAM)(id))
#define PropSheet_IndexToHwnd(hDlg, i) \
    (HWND)SNDMSG(hDlg, PSM_INDEXTOHWND, (WPARAM)(i), 0)
#define PropSheet_IndexToId(hDlg, i) \
    SNDMSG(hDlg, PSM_INDEXTOID, (WPARAM)(i), 0)
#define PropSheet_IndexToPage(hDlg, i) \
    (HPROPSHEETPAGE)SNDMSG(hDlg, PSM_INDEXTOPAGE, (WPARAM)(i), 0)
#define PropSheet_InsertPage(hDlg, index, hpage) \
    SNDMSG(hDlg, PSM_INSERTPAGE, (WPARAM)(index), (LPARAM)(hpage))
#define PropSheet_IsDialogMessage(hDlg, pMsg) \
    (BOOL)SNDMSG(hDlg, PSM_ISDIALOGMESSAGE, 0, (LPARAM)pMsg)
#define PropSheet_PageToIndex(hDlg, hpage) \
    (int)SNDMSG(hDlg, PSM_PAGETOINDEX, 0, (LPARAM)(hpage))
#define PropSheet_PressButton(hDlg, iButton) \
    PostMessage(hDlg, PSM_PRESSBUTTON, (WPARAM)iButton, 0)
#define PropSheet_QuerySiblings(hDlg, wParam, lParam) \
    SNDMSG(hDlg, PSM_QUERYSIBLINGS, wParam, lParam)
#define PropSheet_RebootSystem(hDlg) \
    SNDMSG(hDlg, PSM_REBOOTSYSTEM, 0, 0L)
#define PropSheet_RecalcPageSizes(hDlg) \
    SNDMSG(hDlg, PSM_RECALCPAGESIZES, 0, 0)
#define PropSheet_RemovePage(hDlg, index, hpage) \
    SNDMSG(hDlg, PSM_REMOVEPAGE, index, (LPARAM)hpage)
#define PropSheet_RestartWindows(hDlg) \
    SNDMSG(hDlg, PSM_RESTARTWINDOWS, 0, 0L)
#define PropSheet_SetCurSel(hDlg, hpage, index) \
    SNDMSG(hDlg, PSM_SETCURSEL, (WPARAM)index, (LPARAM)hpage)
#define PropSheet_SetCurSelByID(hDlg, id) \
    SNDMSG(hDlg, PSM_SETCURSELID, 0, (LPARAM)id)
#define PropSheet_SetFinishText(hDlg, lpszText) \
    SNDMSG(hDlg, PSM_SETFINISHTEXT, 0, (LPARAM)lpszText)
#define PropSheet_SetHeaderSubTitle(hDlg, index, lpszText) \
    SNDMSG(hDlg, PSM_SETHEADERSUBTITLE, (WPARAM)(index), (LPARAM)(lpszText))
#define PropSheet_SetHeaderTitle(hDlg, index, lpszText) \
    SNDMSG(hDlg, PSM_SETHEADERTITLE, (WPARAM)(index), (LPARAM)(lpszText))
#define PropSheet_SetTitle(hDlg, wStyle, lpszText) \
    SNDMSG(hDlg, PSM_SETTITLE, wStyle, (LPARAM)(LPCTSTR)lpszText)
#define PropSheet_SetWizButtons(hDlg, dwFlags) \
    PostMessage(hDlg, PSM_SETWIZBUTTONS, 0, (LPARAM)dwFlags)
#define PropSheet_UnChanged(hDlg, hwnd) \
    SNDMSG(hDlg, PSM_UNCHANGED, (WPARAM)hwnd, 0L)

#ifdef __cplusplus
} ;
#endif