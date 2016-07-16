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
#include <richedit.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "header.h"
#include "rc.h"
#include "rcgui.h"
#include "props.h"  
extern HWND hwndPropsTabCtrl;
extern HINSTANCE hInstance;

static HWND hwndToolbox;

#define IMAGECOUNT 13

struct arrangement
{
    int index;
    char *text;
} ;
static struct arrangement rows[] = {
    { IL_PUSHBUTTON, "Push Button" },
    { IL_PUSHBUTTON, "Default Push Button" },
    { IL_CHECKBOX, "Check Box" },
    { IL_RADIOBUTTON, "Radio Button" },
    { IL_STATICTEXT, "Static Text" },
    { IL_ICON, "Icon" },
    { IL_EDIT, "Edit Text" },
    { IL_COMBOBOX, "Combo Box" },
    { IL_LISTBOX, "List Box" },
    { IL_GROUPBOX, "Group Box" },
    { IL_FRAME, "Frame" },
    { IL_RECTANGLE, "Rectangle" },
    { IL_ETCHED, "Etched" },
    { IL_HSCROLL, "Horizontal Scrollbar" },
    { IL_VSCROLL, "Vertical Scrollbar" },
} ;
static HIMAGELIST lvIml;
static HBITMAP mainIml;
static HCURSOR dragCur, noCur;
static HWND lvwindow;
static PROPDESC *desc;

static char *szCtlTbClassName = "xccCtlTb";
static char *szCtlTbTitle = "Toolbox";

static void SetListViewRows()
{
    int i;
    LV_ITEM item;
    for (i=0; i < sizeof(rows)/sizeof(rows[0]); i++)
    {
        memset(&item, 0, sizeof(item));
        item.iItem = i;
        item.iSubItem = 0;
        item.mask = LVIF_PARAM | LVIF_TEXT | LVIF_IMAGE;
        item.lParam = (LPARAM)&rows[i];
        item.iImage = rows[i].index;
        item.pszText = rows[i].text;
        ListView_InsertItem(lvwindow, &item);
    }
}
LRESULT CALLBACK CtlTbProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    static HCURSOR origCurs;
    static BOOL dragging;
    static BOOL inView;
    static int dragSrc;
    int i;
    NM_TREEVIEW *nm;
    RECT rs;
    TVHITTESTINFO hittest;
    switch (iMessage)
    {
        case WM_NOTIFY:
            nm = (NM_TREEVIEW*)lParam;
            switch (nm->hdr.code)
            {
                // don't allow selection
                 case LVN_ITEMCHANGING:
                    return 1;
                case LVN_BEGINDRAG:
                    dragSrc = ((LPNMLISTVIEW)lParam)->iItem;
                    dragging = TRUE;
                    SetCapture(hwnd);
                    origCurs = SetCursor(noCur);
                    inView = FALSE;
                    break;
                case NM_DBLCLK:
                    GetCursorPos(&hittest.pt);
                    ScreenToClient(lvwindow, &hittest.pt);
                    return 0;
                case NM_RCLICK:
                    GetCursorPos(&hittest.pt);
                    ScreenToClient(lvwindow, &hittest.pt);
                    break;
            }
            break;
        case WM_MOUSEMOVE:
            if (dragging)
            {
                HWND wnd;
                POINT pt;
                pt.x = (long)(short)LOWORD(lParam);
                pt.y = (long)(short)HIWORD(lParam);
                ClientToScreen(hwnd, &pt);
                lParam = MAKELPARAM(pt.x, pt.y);
                wnd = WindowFromPoint(pt);
                if (IsWindow(wnd))
                {
                    BOOL enabled = SendMessage(wnd, WM_QUERYCTLDROPTARGET, 0, lParam);
                    if (enabled ^ inView)
                    {
                        SetCursor(enabled ? dragCur : noCur);
                        inView = enabled;
                    }
                }
            }
            break;
        case WM_LBUTTONUP:
            if (dragging)
            {
                SetCursor(origCurs);
                ReleaseCapture();
                dragging = FALSE;
                if (inView)
                {
                    POINT pt;
                    HWND wnd;
                    pt.x = (long)(short)LOWORD(lParam);
                    pt.y = (long)(short)HIWORD(lParam);
                    ClientToScreen(hwnd, &pt);
                    wnd = WindowFromPoint(pt);
                    lParam = MAKELPARAM(pt.x, pt.y);
                    SendMessage(wnd, WM_CTLDROP, dragSrc, lParam);
                }
            }
            break;
        case WM_CREATE:
            dragCur = LoadCursor(hInstance, "ID_DRAGCTL");
            noCur = LoadCursor(hInstance, "ID_NODRAGCUR");
            GetClientRect(hwnd, &rs);
            lvwindow = CreateWindowEx(0, WC_LISTVIEW, "", WS_VISIBLE |
                WS_CHILD | LVS_SMALLICON,
                0, 0, rs.right, rs.bottom, hwnd, (HMENU)ID_TREEVIEW,
                hInstance, NULL);
            ListView_SetBkColor(lvwindow, RetrieveSysColor(COLOR_BTNFACE));
            ListView_SetTextBkColor(lvwindow, RetrieveSysColor(COLOR_BTNFACE));
            ListView_SetExtendedListViewStyle(lvwindow, LVS_EX_DOUBLEBUFFER);
            lvIml = ImageList_Create(32, 32, ILC_COLOR24, IMAGECOUNT, 0);
            
            mainIml = LoadBitmap(hInstance, "ID_CONTROLS");
            ChangeBitmapColor(mainIml, 0xffffff, RetrieveSysColor(COLOR_BTNFACE));
            ImageList_Add(lvIml, mainIml, NULL);
            ListView_SetImageList(lvwindow, lvIml, LVSIL_SMALL);
            SetListViewRows();
            break;
        case WM_SIZE:
            MoveWindow(lvwindow, 0, 0, LOWORD(lParam), HIWORD(lParam), 0);
            break;
        case WM_DESTROY:
            DestroyWindow(lvwindow);
            DestroyCursor(dragCur);
            DestroyCursor(noCur);
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
void RegisterCtlTbWindow(HINSTANCE hInstance)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = 0;
    wc.lpfnWndProc = &CtlTbProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(void*);
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szCtlTbClassName;
    RegisterClass(&wc);
}

//-------------------------------------------------------------------------

HWND CreateCtlTbWindow(void)
{
    hwndToolbox = CreateInternalWindow(DID_CTLTBWND, szCtlTbClassName, szCtlTbTitle);
    return hwndToolbox;
}
