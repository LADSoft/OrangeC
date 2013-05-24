/* 
   ccmacro.h

   Declarations for all the Windows32 API Functions that do not have
   separate ANSI and UNICODE versions.

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

#define FORWARD_WM_NOTIFY(hwnd, idFrom, pnmhdr, fn) \
    (LRESULT)(fn)((hwnd), WM_NOTIFY, (WPARAM)(int)(idFrom), (LPARAM)(NMHDR FAR*)(pnmhdr))


#define HANDLE_WM_NOTIFY(hwnd, wParam, lParam, fn) \
    (fn)((hwnd), (int)(wParam), (NMHDR FAR*)(lParam))

#define INDEXTOOVERLAYMASK(i) \
    ((i) << 8)

#define INDEXTOSTATEIMAGEMASK(i) \
    ((i) << 12)

#define MAKEIPADDRESS(b1,b2,b3,b4) \
    ((LPARAM)(((DWORD)(b1)<<24)+((DWORD)(b2)<<16)+((DWORD)(b3)<<8)+((DWORD)(b4))))
#define MAKEIPRANGE(low, high) \
    ((LPARAM)(WORD)(((BYTE)(high) << 8) + (BYTE)(low)))

#define FIRST_IPADDRESS(x) \
    ((x>>24) & 0xff)
#define SECOND_IPADDRESS(x) \
    ((x>>16) & 0xff)
#define THIRD_IPADDRESS(x) \
    ((x>>8) & 0xff)
#define FOURTH_IPADDRESS(x) \
    (x & 0xff)

#define Animate_Close(hwnd) \
    Animate_Open(hwnd, NULL)
#define Animate_Create(hwndP, id, dwStyle, hInstance) \
    CreateWindow(ANIMATE_CLASS, NULL, dwStyle, 0, 0, 0, 0, hwndP, (HMENU)(id), hInstance, NULL)
#define Animate_Open(hwnd, szName) \
   (BOOL)SNDMSG(hwnd, ACM_OPEN, 0, (LPARAM)(LPTSTR)(szName))
#define Animate_OpenEx(hwnd, hInst, szName) \
   (BOOL)SNDMSG(hwnd, ACM_OPEN, (WPARAM)(hInst), (LPARAM)(LPTSTR)(szName))
#define Animate_Play(hwnd, from, to, rep) \
   (BOOL)SNDMSG(hwnd, ACM_PLAY, (WPARAM)(rep), (LPARAM)MAKELONG(from, to))
#define Animate_Seek(hwnd, frame) \
    Animate_Play(hwnd, frame, frame, 1)
#define Animate_Stop(hwnd) \
   (BOOL)SNDMSG(hwnd, ACM_STOP, 0, 0)

#define DateTime_GetMonthCal(hdp) \
   (HWND)SNDMSG(hdp, DTM_GETMONTHCAL, 0, 0)
#define DateTime_GetMonthCalColor(hdp, iColor) \
   SNDMSG(hdp, DTM_GETMCCOLOR, iColor, 0)
#define DateTime_GetMonthCalFont(hdp) \
   SNDMSG(hdp, DTM_GETMCFONT, 0, 0)
#define DateTime_GetRange(hdp, rgst) \
   (DWORD)SNDMSG(hdp, DTM_GETRANGE, 0, (LPARAM)(rgst))
#define DateTime_GetSystemtime(hdp, pst) \
   (DWORD)SNDMSG(hdp, DTM_GETSYSTEMTIME, 0, (LPARAM)(pst))
#define DateTime_SetFormat(hdp, sz) \
   (BOOL)SNDMSG(hdp, DTM_SETFORMAT, 0, (LPARAM)(sz))
#define DateTime_SetMonthCalColor(hdp, iColor, clr) \
   SNDMSG(hdp, DTM_SETMCCOLOR, iColor, clr)
#define DateTime_SetMonthCalFont(hdp, hfont, fRedraw) \
   SNDMSG(hdp, DTM_SETMCFONT, (WPARAM)(hfont), (LPARAM)(fRedraw))
#define DateTime_SetRange(hdp, gd, rgst) \
   (BOOL)SNDMSG(hdp, DTM_SETRANGE, (WPARAM)(gd), (LPARAM)(rgst))
#define DateTime_SetSystemtime(hdp, gd, pst) \
   (BOOL)SNDMSG(hdp, DTM_SETSYSTEMTIME, (WPARAM)(gd), (LPARAM)(pst))

#define Header_ClearAllFilters(hwnd) \
   (int)SNDMSG((hwnd), HDM_CLEARFILTER, (WPARAM)-1, 0)
#define Header_ClearFilter(hwnd, i) \
   (int)SNDMSG((hwnd), HDM_CLEARFILTER, (WPARAM)(i), 0)
#define Header_CreateDragImage(hwnd, i) \
   (HIMAGELIST)SNDMSG((hwnd), HDM_CREATEDRAGIMAGE, (WPARAM)(i), 0)
#define Header_DeleteItem(hwndHD, i) \
   (BOOL)SNDMSG((hwndHD), HDM_DELETEITEM, (WPARAM)(int)(i), 0L)
#define Header_EditFilter(hwnd, i, fDiscardChanges) \
   (int)SNDMSG((hwnd), HDM_EDITFILTER, (WPARAM)(i), MAKELPARAM(fDiscardChanges, 0))
#define Header_GetBitmapMargin(hwnd) \
   (int)SNDMSG((hwnd), HDM_GETBITMAPMARGIN, 0, 0)
#define Header_GetImageList(hwnd) \
   (HIMAGELIST)SNDMSG((hwnd), HDM_GETIMAGELIST, 0, 0)
#define Header_GetItem(hwndHD, i, phdi) \
   (BOOL)SNDMSG((hwndHD), HDM_GETITEM, (WPARAM)(int)(i), (LPARAM)(HD_ITEM FAR*)(phdi))
#define Header_GetItemCount(hwndHD) \
   (int)SNDMSG((hwndHD), HDM_GETITEMCOUNT, 0, 0L)
#define Header_GetItemRect(hwnd, iItem, lprc) \
   (BOOL)SNDMSG((hwnd), HDM_GETITEMRECT, (WPARAM)(iItem), (LPARAM)(lprc))
#define Header_GetOrderArray(hwnd, iCount, lpi) \
   (BOOL)SNDMSG((hwnd), HDM_GETORDERARRAY, (WPARAM)(iCount), (LPARAM)(lpi))
#define Header_GetUnicodeFormat(hwnd) \
   (BOOL)SNDMSG((hwnd), HDM_GETUNICODEFORMAT, 0, 0)
#define Header_InsertItem(hwndHD, i, phdi) \
   (int)SNDMSG((hwndHD), HDM_INSERTITEM, (WPARAM)(int)(i), (LPARAM)(const HD_ITEM FAR*)(phdi))
#define Header_Layout(hwndHD, playout) \
   (BOOL)SNDMSG((hwndHD), HDM_LAYOUT, 0, (LPARAM)(HD_LAYOUT FAR*)(playout))
#define Header_OrderToIndex(hwnd, i) \
   (int)SNDMSG((hwnd), HDM_ORDERTOINDEX, (WPARAM)(i), 0)
#define Header_SetBitmapMargin(hwnd, iWidth) \
   (int)SNDMSG((hwnd), HDM_SETBITMAPMARGIN, (WPARAM)(iWidth), 0)
#define Header_SetFilterChangeTimeout(hwnd, i) \
   (int)SNDMSG((hwnd), HDM_SETFILTERCHANGETIMEOUT, 0, (LPARAM)(i))
#define Header_SetHotDivider(hwnd, fPos, dw) \
   (int)SNDMSG((hwnd), HDM_SETHOTDIVIDER, (WPARAM)(fPos), (LPARAM)(dw))
#define Header_SetImageList(hwnd, himl) \
   (HIMAGELIST)SNDMSG((hwnd), HDM_SETIMAGELIST, 0, (LPARAM)(himl))
#define Header_SetItem(hwndHD, i, phdi) \
   (BOOL)SNDMSG((hwndHD), HDM_SETITEM, (WPARAM)(int)(i), (LPARAM)(const HD_ITEM FAR*)(phdi))
#define Header_SetOrderArray(hwnd, iCount, lpi) \
   (BOOL)SNDMSG((hwnd), HDM_SETORDERARRAY, (WPARAM)(iCount), (LPARAM)(lpi))
#define Header_SetUnicodeFormat(hwnd, fUnicode) \
   (BOOL)SNDMSG((hwnd), HDM_SETUNICODEFORMAT, (WPARAM)(fUnicode), 0)

#define ImageList_AddIcon(himl, hicon) \
    ImageList_ReplaceIcon(himl, -1, hicon)
#define ImageList_ExtractIcon(hi, himl, i) \
    ImageList_GetIcon(himl, i, 0)
#define ImageList_LoadBitmap(hi, lpbmp, cx, cGrow, crMask) \
    ImageList_LoadImage(hi, lpbmp, cx, cGrow, crMask, IMAGE_BITMAP, 0)
#define ImageList_RemoveAll(himl) \
    ImageList_Remove(himl, -1)

#define ListView_ApproximateViewRect(hwnd, iWidth, iHeight, iCount) \
   (DWORD)SNDMSG((hwnd), LVM_APPROXIMATEVIEWRECT, iCount, MAKELPARAM(iWidth, iHeight))
#define ListView_Arrange(hwndLV, code) \
   (BOOL)SNDMSG((hwndLV), LVM_ARRANGE, (WPARAM)(UINT)(code), 0L)
#define ListView_CreateDragImage(hwnd, i, lpptUpLeft) \
   (HIMAGELIST)SNDMSG((hwnd), LVM_CREATEDRAGIMAGE, (WPARAM)(int)(i), (LPARAM)(LPPOINT)(lpptUpLeft))
#define ListView_DeleteAllItems(hwnd) \
   (BOOL)SNDMSG((hwnd), LVM_DELETEALLITEMS, 0, 0L)
#define ListView_DeleteColumn(hwnd, iCol) \
   (BOOL)SNDMSG((hwnd), LVM_DELETECOLUMN, (WPARAM)(int)(iCol), 0)
#define ListView_DeleteItem(hwnd, i) \
   (BOOL)SNDMSG((hwnd), LVM_DELETEITEM, (WPARAM)(int)(i), 0L)
#define ListView_EditLabel(hwndLV, i) \
   (HWND)SNDMSG((hwndLV), LVM_EDITLABEL, (WPARAM)(int)(i), 0L)
#define ListView_EnsureVisible(hwndLV, i, fPartialOK) \
   (BOOL)SNDMSG((hwndLV), LVM_ENSUREVISIBLE, (WPARAM)(int)(i), MAKELPARAM((fPartialOK), 0))
#define ListView_FindItem(hwnd, iStart, plvfi) \
   (int)SNDMSG((hwnd), LVM_FINDITEM, (WPARAM)(int)(iStart), (LPARAM)(const LV_FINDINFO FAR*)(plvfi))
#define ListView_GetBkColor(hwnd) \
   (COLORREF)SNDMSG((hwnd), LVM_GETBKCOLOR, 0, 0L)
#define ListView_GetBkImage(hwnd, plvbki) \
   (BOOL)SNDMSG((hwnd), LVM_GETBKIMAGE, 0, (LPARAM)(plvbki))
#define ListView_GetCallbackMask(hwnd) \
   (BOOL)SNDMSG((hwnd), LVM_GETCALLBACKMASK, 0, 0)
#define ListView_GetCheckState(hwndLV, i) \
   ((((UINT)(SNDMSG((hwndLV), LVM_GETITEMSTATE, (WPARAM)(i), LVIS_STATEIMAGEMASK))) >> 12) -1)
#define ListView_GetColumn(hwnd, iCol, pcol) \
   (BOOL)SNDMSG((hwnd), LVM_GETCOLUMN, (WPARAM)(int)(iCol), (LPARAM)(LV_COLUMN FAR*)(pcol))
#define ListView_GetColumnOrderArray(hwnd, iCount, pi) \
   (BOOL)SNDMSG((hwnd), LVM_GETCOLUMNORDERARRAY, (WPARAM)(iCount), (LPARAM)(LPINT)(pi))
#define ListView_GetColumnWidth(hwnd, iCol) \
   (int)SNDMSG((hwnd), LVM_GETCOLUMNWIDTH, (WPARAM)(int)(iCol), 0)
#define ListView_GetCountPerPage(hwndLV) \
   (int)SNDMSG((hwndLV), LVM_GETCOUNTPERPAGE, 0, 0)
#define ListView_GetEditControl(hwndLV) \
   (HWND)SNDMSG((hwndLV), LVM_GETEDITCONTROL, 0, 0L)
#define ListView_GetExtendedListViewStyle(hwndLV) \
   (DWORD)SNDMSG((hwndLV), LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0)
#define ListView_GetHeader(hwnd) \
   (HWND)SNDMSG((hwnd), LVM_GETHEADER, 0, 0L)
#define ListView_GetHotCursor(hwnd) \
   (HCURSOR)SNDMSG((hwnd), LVM_GETHOTCURSOR, 0, 0)
#define ListView_GetHotItem(hwnd) \
   (int)SNDMSG((hwnd), LVM_GETHOTITEM, 0, 0)
#define ListView_GetHoverTime(hwndLV) \
   (DWORD)SNDMSG((hwndLV), LVM_GETHOVERTIME, 0, 0)
#define ListView_GetISearchString(hwndLV, lpsz) \
   (BOOL)SNDMSG((hwndLV), LVM_GETISEARCHSTRING, 0, (LPARAM)(LPTSTR)(lpsz))
#define ListView_GetImageList(hwnd, iImageList) \
   (HIMAGELIST)SNDMSG((hwnd), LVM_GETIMAGELIST, (WPARAM)(INT)(iImageList), 0L)
#define ListView_GetItem(hwnd, pitem) \
   (BOOL)SNDMSG((hwnd), LVM_GETITEM, 0, (LPARAM)(LV_ITEM FAR*)(pitem))
#define ListView_GetItemCount(hwnd) \
   (int)SNDMSG((hwnd), LVM_GETITEMCOUNT, 0, 0L)
#define ListView_GetItemPosition(hwndLV, i, ppt) \
   (BOOL)SNDMSG((hwndLV), LVM_GETITEMPOSITION, (WPARAM)(int)(i), (LPARAM)(POINT FAR*)(ppt))
#define ListView_GetItemRect(hwnd, i, prc, code) \
   (BOOL)SNDMSG((hwnd), LVM_GETITEMRECT, (WPARAM)(int)(i), ((prc) ? (((RECT FAR *)(prc))->left = (code),(LPARAM)(RECT FAR*)(prc)) : (LPARAM)(RECT FAR*)NULL))
#define ListView_GetItemSpacing(hwndLV, fSmall) \
   (DWORD)SNDMSG((hwndLV), LVM_GETITEMSPACING, fSmall, 0L)
#define ListView_GetItemState(hwndLV, i, mask) \
   (UINT)SNDMSG((hwndLV), LVM_GETITEMSTATE, (WPARAM)(i), (LPARAM)(mask))
#define ListView_GetItemText(hwndLV, i, iSubItem_, pszText_, cchTextMax_) \
   { LV_ITEM _ms_lvi; _ms_lvi.iSubItem = iSubItem_; _ms_lvi.cchTextMax = cchTextMax_; _ms_lvi.pszText = pszText_; SNDMSG((hwndLV), LVM_GETITEMTEXT, (WPARAM)(i), (LPARAM)(LV_ITEM FAR *)&_ms_lvi); }
#define ListView_GetNextItem(hwnd, i, flags) \
   (int)SNDMSG((hwnd), LVM_GETNEXTITEM, (WPARAM)(int)(i), MAKELPARAM((flags), 0))
#define ListView_GetNumberOfWorkAreas(hwnd, pnWorkAreas) \
   (BOOL)SNDMSG((hwnd), LVM_GETNUMBEROFWORKAREAS, 0, (LPARAM)(UINT *)(pnWorkAreas))
#define ListView_GetOrigin(hwndLV, ppt) \
   (BOOL)SNDMSG((hwndLV), LVM_GETORIGIN, (WPARAM)0, (LPARAM)(POINT FAR*)(ppt))
#define ListView_GetSelectedCount(hwndLV) \
   (UINT)SNDMSG((hwndLV), LVM_GETSELECTEDCOUNT, 0, 0L)
#define ListView_GetSelectionMark(hwnd) \
   (int)SNDMSG((hwnd), LVM_GETSELECTIONMARK, 0, 0)
#define ListView_GetStringWidth(hwndLV, psz) \
   (int)SNDMSG((hwndLV), LVM_GETSTRINGWIDTH, 0, (LPARAM)(LPCTSTR)(psz))
#define ListView_GetSubItemRect(hwnd, iItem, iSubItem, code, prc) \
   (BOOL)SNDMSG((hwnd), LVM_GETSUBITEMRECT, (WPARAM)(int)(iItem), ((prc) ? ((((LPRECT)(prc))->top = iSubItem), (((LPRECT)(prc))->left = code), (LPARAM)(prc)) : (LPARAM)(LPRECT)NULL))
#define ListView_GetTextBkColor(hwnd) \
   (COLORREF)SNDMSG((hwnd), LVM_GETTEXTBKCOLOR, 0, 0L)
#define ListView_GetTextColor(hwnd) \
   (COLORREF)SNDMSG((hwnd), LVM_GETTEXTCOLOR, 0, 0L)
#define ListView_GetToolTips(hwndLV) \
   (HWND)SNDMSG((hwndLV), LVM_GETTOOLTIPS, 0, 0)
#define ListView_GetTopIndex(hwndLV) \
   (int)SNDMSG((hwndLV), LVM_GETTOPINDEX, 0, 0)
#define ListView_GetUnicodeFormat(hwnd) \
   (BOOL)SNDMSG((hwnd), LVM_GETUNICODEFORMAT, 0, 0)
#define ListView_GetViewRect(hwnd, prc) \
   (BOOL)SNDMSG((hwnd), LVM_GETVIEWRECT, 0, (LPARAM)(RECT FAR*)(prc))
#define ListView_GetWorkAreas(hwnd, nWorkAreas, prc) \
   (BOOL)SNDMSG((hwnd), LVM_GETWORKAREAS, (WPARAM)(int)(nWorkAreas), (LPARAM)(RECT FAR*)(prc))
#define ListView_HitTest(hwndLV, pinfo) \
   (int)SNDMSG((hwndLV), LVM_HITTEST, 0, (LPARAM)(LV_HITTESTINFO FAR*)(pinfo))
#define ListView_InsertColumn(hwnd, iCol, pcol) \
   (int)SNDMSG((hwnd), LVM_INSERTCOLUMN, (WPARAM)(int)(iCol), (LPARAM)(const LV_COLUMN FAR*)(pcol))
#define ListView_InsertItem(hwnd, pitem) \
   (int)SNDMSG((hwnd), LVM_INSERTITEM, 0, (LPARAM)(const LV_ITEM FAR*)(pitem))
#define ListView_RedrawItems(hwndLV, iFirst, iLast) \
   (BOOL)SNDMSG((hwndLV), LVM_REDRAWITEMS, (WPARAM)(int)(iFirst), (LPARAM)(int)(iLast))
#define ListView_Scroll(hwndLV, dx, dy) \
   (BOOL)SNDMSG((hwndLV), LVM_SCROLL, (WPARAM)(int)(dx), (LPARAM)(int)(dy))
#define ListView_SetBkColor(hwnd, clrBk) \
   (BOOL)SNDMSG((hwnd), LVM_SETBKCOLOR, 0, (LPARAM)(COLORREF)(clrBk))
#define ListView_SetBkImage(hwnd, plvbki) \
   (BOOL)SNDMSG((hwnd), LVM_SETBKIMAGE, 0, (LPARAM)(plvbki))
#define ListView_SetCallbackMask(hwnd, mask) \
   (BOOL)SNDMSG((hwnd), LVM_SETCALLBACKMASK, (WPARAM)(UINT)(mask), 0)
#define ListView_SetCheckState(hwndLV, i, fCheck) \
    ListView_SetItemState(hwndLV, i, INDEXTOSTATEIMAGEMASK((fCheck)?2:1), LVIS_STATEIMAGEMASK)
#define ListView_SetColumn(hwnd, iCol, pcol) \
   (BOOL)SNDMSG((hwnd), LVM_SETCOLUMN, (WPARAM)(int)(iCol), (LPARAM)(const LV_COLUMN FAR*)(pcol))
#define ListView_SetColumnOrderArray(hwnd, iCount, pi) \
   (BOOL)SNDMSG((hwnd), LVM_SETCOLUMNORDERARRAY, (WPARAM)(iCount), (LPARAM)(LPINT)(pi))
#define ListView_SetColumnWidth(hwnd, iCol, cx) \
   (BOOL)SNDMSG((hwnd), LVM_SETCOLUMNWIDTH, (WPARAM)(int)(iCol), MAKELPARAM((cx), 0))
#define ListView_SetExtendedListViewStyle(hwndLV, dw) \
   (DWORD)SNDMSG((hwndLV), LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dw)
#define ListView_SetExtendedListViewStyleEx(hwndLV, dwMask, dw) \
   (DWORD)SNDMSG((hwndLV), LVM_SETEXTENDEDLISTVIEWSTYLE, dwMask, dw)
#define ListView_SetHotCursor(hwnd, hcur) \
   (HCURSOR)SNDMSG((hwnd), LVM_SETHOTCURSOR, 0, (LPARAM)(hcur))
#define ListView_SetHotItem(hwnd, i) \
   (int)SNDMSG((hwnd), LVM_SETHOTITEM, (WPARAM)(i), 0)
#define ListView_SetHoverTime(hwndLV, dwHoverTimeMs) \
   (DWORD)SNDMSG((hwndLV), LVM_SETHOVERTIME, 0, (LPARAM)(dwHoverTimeMs))
#define ListView_SetIconSpacing(hwndLV, cx, cy) \
   (DWORD)SNDMSG((hwndLV), LVM_SETICONSPACING, 0, MAKELONG(cx,cy))
#define ListView_SetImageList(hwnd, himl, iImageList) \
   (HIMAGELIST)SNDMSG((hwnd), LVM_SETIMAGELIST, (WPARAM)(iImageList), (LPARAM)(HIMAGELIST)(himl))
#define ListView_SetItem(hwnd, pitem) \
   (BOOL)SNDMSG((hwnd), LVM_SETITEM, 0, (LPARAM)(const LV_ITEM FAR*)(pitem))
#define ListView_SetItemCount(hwndLV, cItems) \
   SNDMSG((hwndLV), LVM_SETITEMCOUNT, (WPARAM)(cItems), 0)
#define ListView_SetItemCountEx(hwndLV, cItems, dwFlags) \
   SNDMSG((hwndLV), LVM_SETITEMCOUNT, (WPARAM)(cItems), (LPARAM)(dwFlags))
#define ListView_SetItemPosition(hwndLV, i, x, y) \
   (BOOL)SNDMSG((hwndLV), LVM_SETITEMPOSITION, (WPARAM)(int)(i), MAKELPARAM((x), (y)))
#define ListView_SetItemPosition32(hwndLV, i, x0, y0) \
   { POINT ptNewPos; ptNewPos.x = x0; ptNewPos.y = y0; SNDMSG((hwndLV), LVM_SETITEMPOSITION32, (WPARAM)(int)(i), (LPARAM)&ptNewPos); }
#define ListView_SetItemState(hwndLV, i, data, mask) \
   { LV_ITEM _ms_lvi; _ms_lvi.stateMask = mask; _ms_lvi.state = data; SNDMSG((hwndLV), LVM_SETITEMSTATE, (WPARAM)(i), (LPARAM)(LV_ITEM FAR *)&_ms_lvi); }
#define ListView_SetItemText(hwndLV, i, iSubItem_, pszText_) \
   { LV_ITEM _ms_lvi; _ms_lvi.iSubItem = iSubItem_; _ms_lvi.pszText = pszText_; SNDMSG((hwndLV), LVM_SETITEMTEXT, (WPARAM)(i), (LPARAM)(LV_ITEM FAR *)&_ms_lvi); }
#define ListView_SetSelectionMark(hwnd, i) \
   (int)SNDMSG((hwnd), LVM_SETSELECTIONMARK, 0, (LPARAM)(i))
#define ListView_SetTextBkColor(hwnd, clrTextBk) \
   (BOOL)SNDMSG((hwnd), LVM_SETTEXTBKCOLOR, 0, (LPARAM)(COLORREF)(clrTextBk))
#define ListView_SetTextColor(hwnd, clrText) \
   (BOOL)SNDMSG((hwnd), LVM_SETTEXTCOLOR, 0, (LPARAM)(COLORREF)(clrText))
#define ListView_SetToolTips(hwndLV, hwndNewHwnd) \
   (HWND)SNDMSG((hwndLV), LVM_SETTOOLTIPS, (WPARAM)(hwndNewHwnd), 0)
#define ListView_SetUnicodeFormat(hwnd, fUnicode) \
   (BOOL)SNDMSG((hwnd), LVM_SETUNICODEFORMAT, (WPARAM)(fUnicode), 0)
#define ListView_SetWorkAreas(hwnd, nWorkAreas, prc) \
   (BOOL)SNDMSG((hwnd), LVM_SETWORKAREAS, (WPARAM)(int)(nWorkAreas), (LPARAM)(RECT FAR*)(prc))
#define ListView_SortItems(hwndLV, _pfnCompare, _lPrm) \
   (BOOL)SNDMSG((hwndLV), LVM_SORTITEMS, (WPARAM)(LPARAM)(_lPrm), (LPARAM)(PFNLVCOMPARE)(_pfnCompare))
#define ListView_SortItemsEx(hwndLV, _pfnCompare, _lPrm) \
   (BOOL)SNDMSG((hwndLV), LVM_SORTITEMSEX, (WPARAM)(LPARAM)(_lPrm), (LPARAM)(PFNLVCOMPARE)(_pfnCompare))
#define ListView_SubItemHitTest(hwnd, plvhti) \
   (int)SNDMSG((hwnd), LVM_SUBITEMHITTEST, 0, (LPARAM)(LPLVHITTESTINFO)(plvhti))
#define ListView_Update(hwndLV, i) \
   (BOOL)SNDMSG((hwndLV), LVM_UPDATE, (WPARAM)(i), 0L)

#define MonthCal_GetColor(hmc, iColor) \
   SNDMSG(hmc, MCM_GETCOLOR, iColor, 0)
#define MonthCal_GetCurSel(hmc, pst) \
   (BOOL)SNDMSG(hmc, MCM_GETCURSEL, 0, (LPARAM)(pst))
#define MonthCal_GetFirstDayOfWeek(hmc) \
   (DWORD)SNDMSG(hmc, MCM_GETFIRSTDAYOFWEEK, 0, 0)
#define MonthCal_GetMaxSelCount(hmc) \
   (DWORD)SNDMSG(hmc, MCM_GETMAXSELCOUNT, 0, 0L)
#define MonthCal_GetMaxTodayWidth(hmc) \
   (DWORD)SNDMSG(hmc, MCM_GETMAXTODAYWIDTH, 0, 0)
#define MonthCal_GetMinReqRect(hmc, prc) \
   SNDMSG(hmc, MCM_GETMINREQRECT, 0, (LPARAM)(prc))
#define MonthCal_GetMonthDelta(hmc) \
   (int)SNDMSG(hmc, MCM_GETMONTHDELTA, 0, 0)
#define MonthCal_GetMonthRange(hmc, gmr, rgst) \
   (DWORD)SNDMSG(hmc, MCM_GETMONTHRANGE, (WPARAM)(gmr), (LPARAM)(rgst))
#define MonthCal_GetRange(hmc, rgst) \
   (DWORD)SNDMSG(hmc, MCM_GETRANGE, 0, (LPARAM)(rgst))
#define MonthCal_GetSelRange(hmc, rgst) \
   SNDMSG(hmc, MCM_GETSELRANGE, 0, (LPARAM)(rgst))
#define MonthCal_GetToday(hmc, pst) \
   (BOOL)SNDMSG(hmc, MCM_GETTODAY, 0, (LPARAM)(pst))
#define MonthCal_GetUnicodeFormat(hwnd) \
   (BOOL)SNDMSG((hwnd), MCM_GETUNICODEFORMAT, 0, 0)
#define MonthCal_HitTest(hmc, pinfo) \
   SNDMSG(hmc, MCM_HITTEST, 0, (LPARAM)(PMCHITTESTINFO)(pinfo))
#define MonthCal_SetColor(hmc, iColor, clr) \
   SNDMSG(hmc, MCM_SETCOLOR, iColor, clr)
#define MonthCal_SetCurSel(hmc, pst) \
   (BOOL)SNDMSG(hmc, MCM_SETCURSEL, 0, (LPARAM)(pst))
#define MonthCal_SetDayState(hmc, cbds, rgds) \
   SNDMSG(hmc, MCM_SETDAYSTATE, (WPARAM)(cbds), (LPARAM)(rgds))
#define MonthCal_SetFirstDayOfWeek(hmc, iDay) \
   SNDMSG(hmc, MCM_SETFIRSTDAYOFWEEK, 0, iDay)
#define MonthCal_SetMaxSelCount(hmc, n) \
   (BOOL)SNDMSG(hmc, MCM_SETMAXSELCOUNT, (WPARAM)(n), 0L)
#define MonthCal_SetMonthDelta(hmc, n) \
   (int)SNDMSG(hmc, MCM_SETMONTHDELTA, n, 0)
#define MonthCal_SetRange(hmc, gd, rgst) \
   (BOOL)SNDMSG(hmc, MCM_SETRANGE, (WPARAM)(gd), (LPARAM)(rgst))
#define MonthCal_SetSelRange(hmc, rgst) \
   SNDMSG(hmc, MCM_SETSELRANGE, 0, (LPARAM)(rgst))
#define MonthCal_SetToday(hmc, pst) \
   SNDMSG(hmc, MCM_SETTODAY, 0, (LPARAM)(pst))
#define MonthCal_SetUnicodeFormat(hwnd, fUnicode) \
   (BOOL)SNDMSG((hwnd), MCM_SETUNICODEFORMAT, (WPARAM)(fUnicode), 0)

#define Pager_ForwardMouse(hwnd, bForward) \
   (void)SNDMSG((hwnd), PGM_FORWARDMOUSE, (WPARAM)(bForward), 0)
#define Pager_GetBkColor(hwnd) \
   (COLORREF)SNDMSG((hwnd), PGM_GETBKCOLOR, 0, 0)
#define Pager_GetBorder(hwnd) \
   (int)SNDMSG((hwnd), PGM_GETBORDER, 0, 0)
#define Pager_GetButtonSize(hwnd) \
   (int)SNDMSG((hwnd), PGM_GETBUTTONSIZE, 0,0)
#define Pager_GetButtonState(hwnd, iButton) \
   (DWORD)SNDMSG((hwnd), PGM_GETBUTTONSTATE, 0, (LPARAM)(iButton))
#define Pager_GetDropTarget(hwnd, ppdt) \
   (void)SNDMSG((hwnd), PGM_GETDROPTARGET, 0, (LPARAM)(ppdt))
#define Pager_GetPos(hwnd) \
   (int)SNDMSG((hwnd), PGM_GETPOS, 0, 0)
#define Pager_RecalcSize(hwnd) \
   (void)SNDMSG((hwnd), PGM_RECALCSIZE, 0, 0)
#define Pager_SetBkColor(hwnd, clr) \
   (COLORREF)SNDMSG((hwnd), PGM_SETBKCOLOR, 0, (LPARAM)(clr))
#define Pager_SetBorder(hwnd, iBorder) \
   (int)SNDMSG((hwnd), PGM_SETBORDER, 0, (LPARAM)(iBorder))
#define Pager_SetButtonSize(hwnd, iSize) \
   (int)SNDMSG((hwnd), PGM_SETBUTTONSIZE, 0, (LPARAM)(iSize))
#define Pager_SetChild(hwnd, hwndChild) \
   (void)SNDMSG((hwnd), PGM_SETCHILD, 0, (LPARAM)(hwndChild))
#define Pager_SetPos(hwnd, iPos) \
   (int)SNDMSG((hwnd), PGM_SETPOS, 0, (LPARAM)(iPos))


#define TabCtrl_AdjustRect(hwnd, bLarger, prc) \
   (int)SNDMSG(hwnd, TCM_ADJUSTRECT, (WPARAM)(BOOL)(bLarger), (LPARAM)(RECT FAR *)prc)
#define TabCtrl_DeleteAllItems(hwnd) \
   (BOOL)SNDMSG((hwnd), TCM_DELETEALLITEMS, 0, 0L)
#define TabCtrl_DeleteItem(hwnd, i) \
   (BOOL)SNDMSG((hwnd), TCM_DELETEITEM, (WPARAM)(int)(i), 0L)
#define TabCtrl_DeselectAll(hwnd, fExcludeFocus) \
   (void)SNDMSG((hwnd), TCM_DESELECTALL, fExcludeFocus, 0)
#define TabCtrl_GetCurFocus(hwnd) \
   (int)SNDMSG((hwnd), TCM_GETCURFOCUS, 0, 0)
#define TabCtrl_GetCurSel(hwnd) \
   (int)SNDMSG((hwnd), TCM_GETCURSEL, 0, 0)
#define TabCtrl_GetExtendedStyle(hwnd) \
   (DWORD)SNDMSG((hwnd), TCM_GETEXTENDEDSTYLE, 0, 0)
#define TabCtrl_GetImageList(hwnd) \
   (HIMAGELIST)SNDMSG((hwnd), TCM_GETIMAGELIST, 0, 0L)
#define TabCtrl_GetItem(hwnd, iItem, pitem) \
   (BOOL)SNDMSG((hwnd), TCM_GETITEM, (WPARAM)(int)(iItem), (LPARAM)(TC_ITEM FAR*)(pitem))
#define TabCtrl_GetItemCount(hwnd) \
   (int)SNDMSG((hwnd), TCM_GETITEMCOUNT, 0, 0L)
#define TabCtrl_GetItemRect(hwnd, i, prc) \
   (BOOL)SNDMSG((hwnd), TCM_GETITEMRECT, (WPARAM)(int)(i), (LPARAM)(RECT FAR*)(prc))
#define TabCtrl_GetRowCount(hwnd) \
   (int)SNDMSG((hwnd), TCM_GETROWCOUNT, 0, 0L)
#define TabCtrl_GetToolTips(hwnd) \
   (HWND)SNDMSG((hwnd), TCM_GETTOOLTIPS, 0, 0L)
#define TabCtrl_GetUnicodeFormat(hwnd) \
   (BOOL)SNDMSG((hwnd), TCM_GETUNICODEFORMAT, 0, 0)
#define TabCtrl_HighlightItem(hwnd, i, fHighlight) \
   (BOOL)SNDMSG((hwnd), TCM_HIGHLIGHTITEM, (WPARAM)(i), (LPARAM)MAKELONG (fHighlight, 0))
#define TabCtrl_HitTest(hwndTC, pinfo) \
   (int)SNDMSG((hwndTC), TCM_HITTEST, 0, (LPARAM)(TC_HITTESTINFO FAR*)(pinfo))
#define TabCtrl_InsertItem(hwnd, iItem, pitem) \
   (int)SNDMSG((hwnd), TCM_INSERTITEM, (WPARAM)(int)(iItem), (LPARAM)(const TC_ITEM FAR*)(pitem))
#define TabCtrl_RemoveImage(hwnd, i) \
   (void)SNDMSG((hwnd), TCM_REMOVEIMAGE, i, 0L)
#define TabCtrl_SetCurFocus(hwnd, i) \
   SNDMSG((hwnd),TCM_SETCURFOCUS, i, 0)
#define TabCtrl_SetCurSel(hwnd, i) \
   (int)SNDMSG((hwnd), TCM_SETCURSEL, (WPARAM)(i), 0)
#define TabCtrl_SetExtendedStyle(hwnd, dw) \
   (DWORD)SNDMSG((hwnd), TCM_SETEXTENDEDSTYLE, 0, dw)
#define TabCtrl_SetImageList(hwnd, himl) \
   (HIMAGELIST)SNDMSG((hwnd), TCM_SETIMAGELIST, 0, (LPARAM)(HIMAGELIST)(himl))
#define TabCtrl_SetItem(hwnd, iItem, pitem) \
   (BOOL)SNDMSG((hwnd), TCM_SETITEM, (WPARAM)(int)(iItem), (LPARAM)(TC_ITEM FAR*)(pitem))
#define TabCtrl_SetItemExtra(hwndTC, cb) \
   (BOOL)SNDMSG((hwndTC), TCM_SETITEMEXTRA, (WPARAM)(cb), 0L)
#define TabCtrl_SetItemSize(hwnd, x, y) \
   (DWORD)SNDMSG((hwnd), TCM_SETITEMSIZE, 0, MAKELPARAM(x,y))
#define TabCtrl_SetMinTabWidth(hwnd, x) \
   (int)SNDMSG((hwnd), TCM_SETMINTABWIDTH, 0, x)
#define TabCtrl_SetPadding(hwnd, cx, cy) \
   (void)SNDMSG((hwnd), TCM_SETPADDING, 0, MAKELPARAM(cx, cy))
#define TabCtrl_SetToolTips(hwnd, hwndTT) \
   (void)SNDMSG((hwnd), TCM_SETTOOLTIPS, (WPARAM)(hwndTT), 0L)
#define TabCtrl_SetUnicodeFormat(hwnd, fUnicode) \
   (BOOL)SNDMSG((hwnd), TCM_SETUNICODEFORMAT, (WPARAM)(fUnicode), 0)

#define TreeView_CreateDragImage(hwnd, hitem) \
   (HIMAGELIST)SNDMSG((hwnd), TVM_CREATEDRAGIMAGE, 0, (LPARAM)(HTREEITEM)(hitem))
#define TreeView_DeleteAllItems(hwnd) \
   (BOOL)SNDMSG((hwnd), TVM_DELETEITEM, 0, (LPARAM)TVI_ROOT)
#define TreeView_DeleteItem(hwnd, hitem) \
   (BOOL)SNDMSG((hwnd), TVM_DELETEITEM, 0, (LPARAM)(HTREEITEM)(hitem))
#define TreeView_EditLabel(hwnd, hitem) \
   (HWND)SNDMSG((hwnd), TVM_EDITLABEL, 0, (LPARAM)(HTREEITEM)(hitem))
#define TreeView_EndEditLabelNow(hwnd, fCancel) \
   (BOOL)SNDMSG((hwnd), TVM_ENDEDITLABELNOW, (WPARAM)(fCancel), 0)
#define TreeView_EnsureVisible(hwnd, hitem) \
   (BOOL)SNDMSG((hwnd), TVM_ENSUREVISIBLE, 0, (LPARAM)(HTREEITEM)(hitem))
#define TreeView_Expand(hwnd, hitem, code) \
   (BOOL)SNDMSG((hwnd), TVM_EXPAND, (WPARAM)(code), (LPARAM)(HTREEITEM)(hitem))
#define TreeView_GetBkColor(hwnd) \
   (COLORREF)SNDMSG((hwnd), TVM_GETBKCOLOR, 0, 0)
#define TreeView_GetCheckState(hwndTV, hti) \
   ((((UINT)(SNDMSG((hwndTV), TVM_GETITEMSTATE, (WPARAM)(hti), TVIS_STATEIMAGEMASK))) >> 12) -1)
#define TreeView_GetChild(hwnd, hitem) \
    TreeView_GetNextItem(hwnd, hitem, TVGN_CHILD)
#define TreeView_GetCount(hwnd) \
   (UINT)SNDMSG((hwnd), TVM_GETCOUNT, 0, 0)
#define TreeView_GetDropHilight(hwnd) \
    TreeView_GetNextItem(hwnd, NULL, TVGN_DROPHILITE)
#define TreeView_GetEditControl(hwnd) \
   (HWND)SNDMSG((hwnd), TVM_GETEDITCONTROL, 0, 0)
#define TreeView_GetFirstVisible(hwnd) \
    TreeView_GetNextItem(hwnd, NULL, TVGN_FIRSTVISIBLE)
#define TreeView_GetISearchString(hwndTV, lpsz) \
   (BOOL)SNDMSG((hwndTV), TVM_GETISEARCHSTRING, 0, (LPARAM)(LPTSTR)(lpsz))
#define TreeView_GetImageList(hwnd, iImage) \
   (HIMAGELIST)SNDMSG((hwnd), TVM_GETIMAGELIST, iImage, 0)
#define TreeView_GetIndent(hwnd) \
   (UINT)SNDMSG((hwnd), TVM_GETINDENT, 0, 0)
#define TreeView_GetInsertMarkColor(hwnd) \
   (COLORREF)SNDMSG((hwnd), TVM_GETINSERTMARKCOLOR, 0, 0)
#define TreeView_GetItem(hwnd, pitem) \
   (BOOL)SNDMSG((hwnd), TVM_GETITEM, 0, (LPARAM)(TV_ITEM FAR*)(pitem))
#define TreeView_GetItemHeight(hwnd) \
   (int)SNDMSG((hwnd), TVM_GETITEMHEIGHT, 0, 0)
#define TreeView_GetItemRect(hwnd, hitem, prc, code) \
   (*(HTREEITEM *)prc = (hitem), (BOOL)SNDMSG((hwnd), TVM_GETITEMRECT, (WPARAM)(code), (LPARAM)(RECT FAR*)(prc)))
#define TreeView_GetItemState(hwndTV, hti, mask) \
   (UINT)SNDMSG((hwndTV), TVM_GETITEMSTATE, (WPARAM)(hti), (LPARAM)(mask))
#define TreeView_GetLastVisible(hwnd) \
    TreeView_GetNextItem(hwnd, NULL, TVGN_LASTVISIBLE)
#define TreeView_GetLineColor(hwnd) \
   (COLORREF)SNDMSG((hwnd), TVM_GETLINECOLOR, 0, 0)
#define TreeView_GetNextItem(hwnd, hitem, code) \
   (HTREEITEM)SNDMSG((hwnd), TVM_GETNEXTITEM, (WPARAM)(code), (LPARAM)(HTREEITEM)(hitem))
#define TreeView_GetNextSibling(hwnd, hitem) \
    TreeView_GetNextItem(hwnd, hitem, TVGN_NEXT)
#define TreeView_GetNextVisible(hwnd, hitem) \
    TreeView_GetNextItem(hwnd, hitem, TVGN_NEXTVISIBLE)
#define TreeView_GetParent(hwnd, hitem) \
    TreeView_GetNextItem(hwnd, hitem, TVGN_PARENT)
#define TreeView_GetPrevSibling(hwnd, hitem) \
    TreeView_GetNextItem(hwnd, hitem, TVGN_PREVIOUS)
#define TreeView_GetPrevVisible(hwnd, hitem) \
    TreeView_GetNextItem(hwnd, hitem, TVGN_PREVIOUSVISIBLE)
#define TreeView_GetRoot(hwnd) \
    TreeView_GetNextItem(hwnd, NULL, TVGN_ROOT)
#define TreeView_GetScrollTime(hwnd) \
   (UINT)SNDMSG((hwnd), TVM_GETSCROLLTIME, 0, 0)
#define TreeView_GetSelection(hwnd) \
    TreeView_GetNextItem(hwnd, NULL, TVGN_CARET)
#define TreeView_GetTextColor(hwnd) \
   (COLORREF)SNDMSG((hwnd), TVM_GETTEXTCOLOR, 0, 0)
#define TreeView_GetToolTips(hwnd) \
   (HWND)SNDMSG((hwnd), TVM_GETTOOLTIPS, 0, 0)
#define TreeView_GetUnicodeFormat(hwnd) \
   (BOOL)SNDMSG((hwnd), TVM_GETUNICODEFORMAT, 0, 0)
#define TreeView_GetVisibleCount(hwnd) \
   (UINT)SNDMSG((hwnd), TVM_GETVISIBLECOUNT, 0, 0)
#define TreeView_HitTest(hwnd, lpht) \
   (HTREEITEM)SNDMSG((hwnd), TVM_HITTEST, 0, (LPARAM)(LPTV_HITTESTINFO)(lpht))
#define TreeView_InsertItem(hwnd, lpis) \
   (HTREEITEM)SNDMSG((hwnd), TVM_INSERTITEM, 0, (LPARAM)(LPTV_INSERTSTRUCT)(lpis))
#define TreeView_Select(hwnd, hitem, code) \
   (BOOL)SNDMSG((hwnd), TVM_SELECTITEM, (WPARAM)(code), (LPARAM)(HTREEITEM)(hitem))
#define TreeView_SelectDropTarget(hwnd, hitem) \
    TreeView_Select(hwnd, hitem, TVGN_DROPHILITE)
#define TreeView_SelectItem(hwnd, hitem) \
    TreeView_Select(hwnd, hitem, TVGN_CARET)
#define TreeView_SelectSetFirstVisible(hwnd, hitem) \
    TreeView_Select(hwnd, hitem, TVGN_FIRSTVISIBLE)
#define TreeView_SetBkColor(hwnd, clr) \
   (COLORREF)SNDMSG((hwnd), TVM_SETBKCOLOR, 0, (LPARAM)(clr))
#define TreeView_SetCheckState(hwndTV, hti, fCheck) \
    TreeView_SetItemState(hwndTV, hti, INDEXTOSTATEIMAGEMASK((fCheck)?2:1), TVIS_STATEIMAGEMASK)
#define TreeView_SetImageList(hwnd, himl, iImage) \
   (HIMAGELIST)SNDMSG((hwnd), TVM_SETIMAGELIST, iImage, (LPARAM)(HIMAGELIST)(himl))
#define TreeView_SetIndent(hwnd, indent) \
   (BOOL)SNDMSG((hwnd), TVM_SETINDENT, (WPARAM)(indent), 0)
#define TreeView_SetInsertMark(hwnd, hItem, fAfter) \
   (BOOL)SNDMSG((hwnd), TVM_SETINSERTMARK, (WPARAM) (fAfter), (LPARAM) (hItem))
#define TreeView_SetInsertMarkColor(hwnd, clr) \
   (COLORREF)SNDMSG((hwnd), TVM_SETINSERTMARKCOLOR, 0, (LPARAM)(clr))
#define TreeView_SetItem(hwnd, pitem) \
   (BOOL)SNDMSG((hwnd), TVM_SETITEM, 0, (LPARAM)(const TV_ITEM FAR*)(pitem))
#define TreeView_SetItemHeight(hwnd, iHeight) \
   (int)SNDMSG((hwnd), TVM_SETITEMHEIGHT, (WPARAM)(iHeight), 0)
#define TreeView_SetItemState(hwndTV, hti, data, _mask) \
   { TVITEM _ms_TVi; _ms_TVi.mask = TVIF_STATE; _ms_TVi.hItem = hti; _ms_TVi.stateMask = _mask; _ms_TVi.state = data; SNDMSG((hwndTV), TVM_SETITEM, 0, (LPARAM)(TV_ITEM FAR *)&_ms_TVi); }
#define TreeView_SetLineColor(hwnd, clr) \
   (COLORREF)SNDMSG((hwnd), TVM_SETLINECOLOR, 0, (LPARAM)(clr))
#define TreeView_SetScrollTime(hwnd, uTime) \
   (UINT)SNDMSG((hwnd), TVM_SETSCROLLTIME, uTime, 0)
#define TreeView_SetTextColor(hwnd, clr) \
   (COLORREF)SNDMSG((hwnd), TVM_SETTEXTCOLOR, 0, (LPARAM)(clr))
#define TreeView_SetToolTips(hwnd, hwndTT) \
   (HWND)SNDMSG((hwnd), TVM_SETTOOLTIPS, (WPARAM)(hwndTT), 0)
#define TreeView_SetUnicodeFormat(hwnd, fUnicode) \
   (BOOL)SNDMSG((hwnd), TVM_SETUNICODEFORMAT, (WPARAM)(fUnicode), 0)
#define TreeView_SortChildren(hwnd, hitem, recurse) \
   (BOOL)SNDMSG((hwnd), TVM_SORTCHILDREN, (WPARAM)(recurse), (LPARAM)(HTREEITEM)(hitem))
#define TreeView_SortChildrenCB(hwnd, psort, recurse) \
   (BOOL)SNDMSG((hwnd), TVM_SORTCHILDRENCB, (WPARAM)(recurse), (LPARAM)(LPTV_SORTCB)(psort))


#ifdef __cplusplus
} ;
#endif

