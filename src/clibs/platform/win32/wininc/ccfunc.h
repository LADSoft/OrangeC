/* 
   ccfunc.h

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
void        PASCAL WINBASEAPI InitCommonControls(void);
WINBOOL     PASCAL WINBASEAPI InitCommonControlsEx(LPINITCOMMONCONTROLSEX);
HIMAGELIST  PASCAL WINBASEAPI ImageList_Create(int cx, int cy, UINT flags, int cInitial, int cGrow);
WINBOOL     PASCAL WINBASEAPI ImageList_Destroy(HIMAGELIST himl);
int         PASCAL WINBASEAPI ImageList_GetImageCount(HIMAGELIST himl);
WINBOOL     PASCAL WINBASEAPI ImageList_SetImageCount(HIMAGELIST himl, UINT uNewCount);
int         PASCAL WINBASEAPI ImageList_Add(HIMAGELIST himl, HBITMAP hbmImage, HBITMAP hbmMask);
int         PASCAL WINBASEAPI ImageList_ReplaceIcon(HIMAGELIST himl, int i, HICON hicon);
COLORREF    PASCAL WINBASEAPI ImageList_SetBkColor(HIMAGELIST himl, COLORREF clrBk);
COLORREF    PASCAL WINBASEAPI ImageList_GetBkColor(HIMAGELIST himl);
WINBOOL     PASCAL WINBASEAPI ImageList_SetOverlayImage(HIMAGELIST himl, int iImage, int iOverlay);
WINBOOL     PASCAL WINBASEAPI ImageList_Draw(HIMAGELIST himl, int i, HDC hdcDst, int x, int y, UINT fStyle);
WINBOOL        PASCAL WINBASEAPI ImageList_Replace(HIMAGELIST himl, int i, HBITMAP hbmImage, HBITMAP hbmMask);
int         PASCAL WINBASEAPI ImageList_AddMasked(HIMAGELIST himl, HBITMAP hbmImage, COLORREF crMask);
WINBOOL        PASCAL WINBASEAPI ImageList_DrawEx(HIMAGELIST himl, int i, HDC hdcDst, int x, int y, int dx, int dy, COLORREF rgbBk, COLORREF rgbFg, UINT fStyle);
WINBOOL        PASCAL WINBASEAPI ImageList_DrawIndirect(IMAGELISTDRAWPARAMS* pimldp);
WINBOOL        PASCAL WINBASEAPI ImageList_Remove(HIMAGELIST himl, int i);
HICON       PASCAL WINBASEAPI ImageList_GetIcon(HIMAGELIST himl, int i, UINT flags);


WINBOOL        PASCAL WINBASEAPI ImageList_Copy(HIMAGELIST himlDst, int iDst, HIMAGELIST himlSrc, int iSrc, UINT uFlags);

WINBOOL        PASCAL WINBASEAPI ImageList_BeginDrag(HIMAGELIST himlTrack, int iTrack, int dxHotspot, int dyHotspot);
void        PASCAL WINBASEAPI ImageList_EndDrag();
WINBOOL        PASCAL WINBASEAPI ImageList_DragEnter(HWND hwndLock, int x, int y);
WINBOOL        PASCAL WINBASEAPI ImageList_DragLeave(HWND hwndLock);
WINBOOL        PASCAL WINBASEAPI ImageList_DragMove(int x, int y);
WINBOOL        PASCAL WINBASEAPI ImageList_SetDragCursorImage(HIMAGELIST himlDrag, int iDrag, int dxHotspot, int dyHotspot);

WINBOOL        PASCAL WINBASEAPI ImageList_DragShowNolock(WINBOOL fShow);
HIMAGELIST  PASCAL WINBASEAPI ImageList_GetDragImage(POINT FAR* ppt,POINT FAR* pptHotspot);

/*HIMAGELIST PASCAL WINBASEAPI ImageList_Read(LPSTREAM pstm);
WINBOOL       PASCAL WINBASEAPI ImageList_Write(HIMAGELIST himl, LPSTREAM pstm);
*/
WINBOOL        PASCAL WINBASEAPI ImageList_GetIconSize(HIMAGELIST himl, int FAR *cx, int FAR *cy);
WINBOOL        PASCAL WINBASEAPI ImageList_SetIconSize(HIMAGELIST himl, int cx, int cy);
WINBOOL        PASCAL WINBASEAPI ImageList_GetImageInfo(HIMAGELIST himl, int i, IMAGEINFO FAR* pImageInfo);
HIMAGELIST  PASCAL WINBASEAPI ImageList_Merge(HIMAGELIST himl1, int i1, HIMAGELIST himl2, int i2, int dx, int dy);
HIMAGELIST  PASCAL WINBASEAPI ImageList_Duplicate(HIMAGELIST himl);

HWND PASCAL WINBASEAPI CreateToolbarEx(HWND hwnd, DWORD ws, UINT wID, int nBitmaps,
                        HINSTANCE hBMInst, UINT wBMID, LPCTBBUTTON lpButtons,
                        int iNumButtons, int dxButton, int dyButton,
                        int dxBitmap, int dyBitmap, UINT uStructSize);

HBITMAP PASCAL WINBASEAPI CreateMappedBitmap(HINSTANCE hInstance, INT_PTR idBitmap,
                                  UINT wFlags, LPCOLORMAP lpColorMap,
                                  int iNumMaps);


void PASCAL WINBASEAPI MenuHelp(UINT uMsg, WPARAM wParam, LPARAM lParam, HMENU hMainMenu, HINSTANCE hInst, HWND hwndStatus, UINT FAR *lpwIDs);
WINBOOL PASCAL WINBASEAPI ShowHideMenuCtl(HWND hWnd, UINT uFlags, LPINT lpInfo);
void PASCAL WINBASEAPI GetEffectiveClientRect(HWND hWnd, LPRECT lprc, LPINT lpInfo);
WINBOOL PASCAL WINBASEAPI MakeDragList(HWND hLB);
void PASCAL WINBASEAPI DrawInsert(HWND handParent, HWND hLB, int nItem);
int PASCAL WINBASEAPI LBItemFromPt(HWND hLB, POINT pt, WINBOOL bAutoScroll);
HWND PASCAL WINBASEAPI CreateUpDownControl(DWORD dwStyle, int x, int y, int cx, int cy,
                                HWND hParent, int nID, HINSTANCE hInst,
                                HWND hBuddy,
                                int nUpper, int nLower, int nPos);
WINBOOL PASCAL WINBASEAPI FlatSB_EnableScrollBar(HWND, int, UINT);
WINBOOL PASCAL WINBASEAPI FlatSB_ShowScrollBar(HWND, int code, WINBOOL);

WINBOOL PASCAL WINBASEAPI FlatSB_GetScrollRange(HWND, int code, LPINT, LPINT);
WINBOOL PASCAL WINBASEAPI FlatSB_GetScrollInfo(HWND, int code, LPSCROLLINFO);
int PASCAL WINBASEAPI FlatSB_GetScrollPos(HWND, int code);
WINBOOL PASCAL WINBASEAPI FlatSB_GetScrollProp(HWND, int propIndex, LPINT);
#define FlatSB_GetScrollPropPtr  FlatSB_GetScrollProp

int PASCAL WINBASEAPI FlatSB_SetScrollPos(HWND, int code, int pos, WINBOOL fRedraw);
int PASCAL WINBASEAPI FlatSB_SetScrollInfo(HWND, int code, LPSCROLLINFO, WINBOOL fRedraw);

int PASCAL WINBASEAPI FlatSB_SetScrollRange(HWND, int code, int min, int max, WINBOOL fRedraw);
WINBOOL PASCAL WINBASEAPI FlatSB_SetScrollProp(HWND, UINT index, INT_PTR newValue, WINBOOL);
#define FlatSB_SetScrollPropPtr FlatSB_SetScrollProp

WINBOOL PASCAL WINBASEAPI InitializeFlatSB(HWND);
HRESULT PASCAL WINBASEAPI UninitializeFlatSB(HWND);

HWND PASCAL WINBASEAPI CreateStatusWindowW(LONG style, LPCWSTR lpszText, HWND hwndParent, UINT wID);
void PASCAL WINBASEAPI DrawStatusTextW(HDC hDC, LPRECT lprc, LPCWSTR pszText, UINT uFlags);
HIMAGELIST  PASCAL WINBASEAPI ImageList_LoadImageW(HINSTANCE hi, LPCWSTR lpbmp, int cx, int cGrow, COLORREF crMask, UINT uType, UINT uFlags);

HWND PASCAL WINBASEAPI CreateStatusWindowA(LONG style, LPCSTR lpszText, HWND hwndParent, UINT wID);
void PASCAL WINBASEAPI DrawStatusTextA(HDC hDC, LPRECT lprc, LPCSTR pszText, UINT uFlags);
HIMAGELIST  PASCAL WINBASEAPI ImageList_LoadImageA(HINSTANCE hi, LPCSTR lpbmp, int cx, int cGrow, COLORREF crMask, UINT uType, UINT uFlags);

#ifdef UNICODE

#define CreateStatusWindow      CreateStatusWindowW
#define DrawStatusText          DrawStatusTextW
#define ImageList_LoadImage     ImageList_LoadImageW
#else

#define CreateStatusWindow      CreateStatusWindowA
#define DrawStatusText          DrawStatusTextA
#define ImageList_LoadImage     ImageList_LoadImageA
#endif

void PASCAL WINBASEAPI InitMUILanguage(LANGID uiLang);

LANGID PASCAL WINBASEAPI GetMUILanguage(void);
WINBOOL PASCAL WINBASEAPI TrackMouseEvent(LPTRACKMOUSEEVENT lpEventTrack);

#ifdef __cplusplus
} ;
#endif