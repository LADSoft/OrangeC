/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2012, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/

#include <windows.h>                             
#include <commctrl.h>
#include <stdio.h>
#include "header.h"
//#include <dir.h>

extern HWND hwndFrame, hwndClient, hwndProject, hwndRes, hwndWatch;
extern HINSTANCE hInstance;

HWND hwndTab;
HWND hwndTabCtrl;
static char szTabClassName[] = "xccTabClass";
static char szTabTitle[] = "Workspace";
static char *nameTags[] = 
{
    "Files", "Resources"
};
static HBITMAP projBitmap, resBitmap;
static HBITMAP *bitmaps[] = 
{
     &projBitmap, &resBitmap
};

void GetTabRect(RECT *rect)
{
    GetClientRect(hwndTabCtrl, rect);
    //   SendMessage(hwndCtrl,LCF_ADJUSTRECT,0,(LPARAM)rect) ;
    TabCtrl_AdjustRect(hwndTabCtrl, FALSE, rect);

}

void SelectResourceWindow()
{
    TabCtrl_SetCurSel(hwndTabCtrl, 1);
    ShowWindow(hwndProject, SW_HIDE) ;
    ShowWindow(hwndRes, SW_SHOW) ;
}
//-------------------------------------------------------------------------

LRESULT CALLBACK TabWndWndProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    static int selected, sizingbottom;
    static HIMAGELIST tabIml;
    static int ilProj, ilRes;
    RECT r;
    TC_ITEM tie;
    NMHDR *h;
    DRAWITEMSTRUCT *dr;
    HFONT font;
    HBITMAP hbmp;
    HDC hMemDC;
    PAINTSTRUCT paint;
    HDC dc;
    switch (iMessage)
    {
        case WM_SYSCOMMAND:
            if (wParam == SC_CLOSE)
            {
                PostMessage(hwnd, WM_CLOSE, 0, 0);
                return 0;
            }
            break;
        case WM_NOTIFY:
            h = (NMHDR*)lParam;
            switch (h->code)
            {
            case TCN_SELCHANGE:
                switch (selected = TabCtrl_GetCurSel(hwndTabCtrl))
                {
                case 0:
                    ShowWindow(hwndRes, SW_HIDE) ;
                    ShowWindow(hwndProject, SW_SHOW) ;
                    break;
                case 1:
                    ShowWindow(hwndProject, SW_HIDE) ;
                    ShowWindow(hwndRes, SW_SHOW) ;
                    break;
                }
            }
            break;
        case WM_SETTEXT:
            return SendMessage(hwndTabCtrl, iMessage, wParam, lParam);
        case WM_DRAWITEM:
            dr = (DRAWITEMSTRUCT*)lParam;
            hMemDC = CreateCompatibleDC(dr->hDC);
            hbmp = SelectObject(hMemDC,  *bitmaps[dr->itemID]);
            BitBlt(dr->hDC, dr->rcItem.left + 2, dr->rcItem.bottom - 18, 16, 16,
                hMemDC, 0, 0, SRCCOPY);
            TextOut(dr->hDC, dr->rcItem.left + 18, dr->rcItem.bottom - 16,
                nameTags[dr->itemID], strlen(nameTags[dr->itemID]));
            SelectObject(dr->hDC, hbmp);
            DeleteDC(hMemDC);
            break;
        case WM_CREATE:
            hwndTab = hwnd;
            GetClientRect(hwnd, &r);
            hwndTabCtrl = CreateWindow(WC_TABCONTROL, 0, WS_CHILD +
                WS_CLIPSIBLINGS + WS_VISIBLE + TCS_FLATBUTTONS /*+ TCS_OWNERDRAWFIXED */
                + TCS_FOCUSNEVER /*+ TCS_FIXEDWIDTH*/ + TCS_BOTTOM, r.left, r.top,
                r.right - r.left, r.bottom - r.top, hwnd, 0, hInstance, 0);
            ApplyDialogFont(hwndTabCtrl);
            projBitmap = LoadImage(hInstance, "ID_PROJBMP", IMAGE_BITMAP,0,0,LR_DEFAULTSIZE | LR_LOADMAP3DCOLORS);
            resBitmap = LoadImage(hInstance, "ID_RESBMP", IMAGE_BITMAP,0,0,LR_DEFAULTSIZE | LR_LOADMAP3DCOLORS);
            tabIml = ImageList_Create(16, 16, ILC_COLOR32, 1, 0);
            ilProj = ImageList_Add(tabIml, projBitmap, 0);
            ilRes = ImageList_Add(tabIml, resBitmap, 0);
            TabCtrl_SetImageList(hwndTabCtrl, tabIml);

            CreateProjectWindow();
            CreateResourceWindow();
            tie.mask = TCIF_TEXT | TCIF_IMAGE;
            tie.iImage = ilProj;
            tie.pszText = nameTags[0];
            TabCtrl_InsertItem(hwndTabCtrl, 0, &tie);
            tie.mask = TCIF_TEXT | TCIF_IMAGE ;
            tie.iImage = ilRes ;
            tie.pszText  = nameTags[1] ;
            TabCtrl_InsertItem(hwndTabCtrl,1, &tie) ;
            ShowWindow(hwndRes,SW_HIDE) ;
            return 0;
        case WM_COMMAND:
            break;
        case WM_PAINT:
            dc = BeginPaint(hwnd, &paint);
            EndPaint(hwnd, &paint);
            break;
        case WM_DESTROY:
            DestroyWindow(hwndRes) ;
            DestroyWindow(hwndProject);
            DestroyWindow(hwndTabCtrl);
            DeleteObject(projBitmap);
            DeleteObject(resBitmap);
            hwndTab = 0;
            break;
        case WM_SIZE:
            r.right = LOWORD(lParam);
            r.bottom = HIWORD(lParam);
            r.left = r.top = 0;
            MoveWindow(hwndTabCtrl, r.left, r.top, r.right - r.left, r.bottom -
                r.top, 1);
            GetTabRect(&r);
            MoveWindow(hwndProject, r.left, r.top, r.right - r.left, r.bottom -
                r.top, 1);
            MoveWindow( hwndRes,r.left,r.top,r.right-r.left,r.bottom-r.top,TRUE) ;
            break;
        case WM_CLOSE:
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}

//-------------------------------------------------------------------------

void RegisterTabWindow(void)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = 0;
    wc.lpfnWndProc = &TabWndWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szTabClassName;
    RegisterClass(&wc);

}

//-------------------------------------------------------------------------

void CreateTabWindow(void)
{
    hwndTab = CreateDockableWindow(DID_TABWND, szTabClassName, szTabTitle, hInstance, 200, 200);
}
