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
#include "winconst.h"
#include "lsctrl.h"

#define INDENT 0

extern HWND hwndFrame;

static char *szToolBarWindClassName = "ladSoftToolBarWindow";
static TBBUTTON sep = 
{
    0, 0, TBSTATE_WRAP, TBSTYLE_SEP | TBSTYLE_FLAT, {0}, 0, -1
};
static char *GetTipText(CCW_params *ptr, int id)
{
    int i;
    for (i = 0; i < ptr->u.tb.btncount; i++)
        if (ptr->u.tb.buttons[i].idCommand == id)
        {
            return ptr->u.tb.hints[i];
        }
    return "";
}

//-------------------------------------------------------------------------

static void FormatToolBar(char *buf, HWND hwnd)
{
    TBBUTTON button;
    char *p = buf;
    int i = 0;
    buf[0] = 0;
    while (SendMessage(hwnd, TB_GETBUTTON, i++, (LPARAM) &button))
    {
        if (button.fsStyle == BTNS_SEP)
        {
            sprintf(p, "%d ", -button.idCommand * 1000 - button.iBitmap);
        }
        else
        {
            sprintf(p, "%d ", button.idCommand);
        }
        p += strlen(p);
    }
}

//-------------------------------------------------------------------------

static int ParseToolBar(char *buf, HWND hwnd, CCW_params *ptr, TBBUTTON
    *buttons)
{
    int rv = 0, i, id, count;
    char *p = buf;
    while (*p)
    {
        if (!isdigit(*p) &&  *p != '-')
            break;
        sscanf(p, "%d %n", &id, &count);
        p += count;
        if (id < 0)
        {
            buttons[rv] = sep;
            if (hwnd == ptr->u.tb.hWnd)
                buttons[rv].fsState &= ~TBSTATE_WRAP;
            buttons[rv].idCommand = -id/1000;
            id = -(id) % 1000;
            buttons[rv++].iBitmap = id;
        }
        else
        {
            for (i = 0; i < ptr->u.tb.btncount; i++)
                if (ptr->u.tb.buttons[i].idCommand == id)
                {
                    int state;
                    if (!IsWindow(hwnd))
                        state = FALSE;
                    else
                        state = SendMessage(hwnd, TB_ISBUTTONENABLED, id, 0) ? TBSTATE_ENABLED : 0;
                    buttons[rv] = ptr->u.tb.buttons[i];
                    buttons[rv].fsState = state;
                    if (hwnd == ptr->u.tb.hWnd)
                        buttons[rv].fsState &= ~TBSTATE_WRAP;
                    
                    rv++;
    
                }
        }
        while (isspace(*p))
            p++;
    }
    return rv;

}


//-------------------------------------------------------------------------

static void SetRectSize(HWND hwnd, CCW_params *ptr)
{
    RECT r;
    TBBUTTON buttons[60];
    int num = 0;
    int i;
    BOOL found = FALSE;
    while (SendMessage(hwnd, TB_GETBUTTON, num, (LPARAM) &buttons[num]))
    {
        if (!(buttons[num].fsStyle & TBSTYLE_SEP))
            found = TRUE;
        num++;
    }
    if (hwnd == ptr->u.tb.hWnd)
    {
        SendMessage(hwnd, TB_SETROWS, MAKEWPARAM(1, FALSE), (LPARAM) &r);
        SendMessage(hwnd, TB_GETITEMRECT, num - 1, (LPARAM) &r);
        ptr->u.tb.hsize.cx = r.right ;
        ptr->u.tb.hsize.cy = r.bottom + GetSystemMetrics(SM_CYBORDER) * 2 ;
        if (!ptr->vertical)
            MoveWindow(ptr->hwnd, GRIPWIDTH, 0, ptr->u.tb.hsize.cx, ptr->u.tb.hsize.cy, 1);
        MoveWindow(hwnd, 0, 0, ptr->u.tb.hsize.cx, ptr->u.tb.hsize.cy, 1);
    }
    else
    {
        if (found)
            SendMessage(hwnd, TB_SETROWS, MAKEWPARAM(num-1, FALSE), (LPARAM) &r);
        else
            SendMessage(hwnd, TB_GETITEMRECT, num - 1, (LPARAM) &r);
//        if (num == 9)
//            ExtendedMessageBox("hi",0,"%d %d", r.right, r.bottom);
//        SendMessage(hwnd, TB_AUTOSIZE, 0, 0);
//	    SendMessage(hwnd,TB_GETITEMRECT,num-1, (LPARAM) & r) ;
        ptr->u.tb.vsize.cy = r.bottom ;
        ptr->u.tb.vsize.cx = r.right + GetSystemMetrics(SM_CXBORDER) * 2;
        if (ptr->vertical)
            MoveWindow(ptr->hwnd, 0, GRIPWIDTH, ptr->u.tb.vsize.cx, ptr->u.tb.vsize.cy, 1);
        MoveWindow(hwnd, 0, 0, ptr->u.tb.vsize.cx, ptr->u.tb.vsize.cy, 1);
    }
    if (ptr->vertical && hwnd == ptr->u.tb.vWnd || !ptr->vertical && hwnd == ptr->u.tb.hWnd)
    {
        for (i=0; i < ptr->u.tb.btncount; i++)
            if (ptr->u.tb.children[i])
            {
                SetWindowPos(ptr->u.tb.children[i], 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_HIDEWINDOW);
            }
        for (i=0; i < num; i++)
            if ((buttons[i].fsStyle & TBSTYLE_SEP) && buttons[i].idCommand)
            {
                int j;
                for (j=0; j < ptr->u.tb.btncount; j++)
                {

                    if (buttons[i].idCommand == ptr->u.tb.buttons[j].idCommand)
                    {
                        if (ptr->u.tb.children[j])
                        {
                            RECT r;
                            SendMessage(hwnd, TB_GETITEMRECT, i, (LPARAM)&r);
                            SetWindowPos(ptr->u.tb.children[j], 0, r.left, r.top, r.right- r.left, r.bottom - r.top, SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
                        }
                    }
                }
            }
    }
}

//-------------------------------------------------------------------------

static void ChangeButtons(int num, HWND hwnd, TBBUTTON *buttons, CCW_params
    *ptr)
{
    if (num)
    {
        while (SendMessage(hwnd, TB_DELETEBUTTON, 0, 0))
            ;
        SendMessage(hwnd, TB_ADDBUTTONS, num, (LPARAM) buttons);
        SetRectSize(hwnd, ptr);
    }
}

//-------------------------------------------------------------------------

static void CopyButtons(CCW_params *ptr)
{
    TBBUTTON buttons[60];
    int i = 0;
    HWND hwnds, hwndd;

    if (ptr->vertical)
    {
        hwnds = ptr->u.tb.vWnd;
        hwndd = ptr->u.tb.hWnd;
    }
    else
    {
        hwnds = ptr->u.tb.hWnd;
        hwndd = ptr->u.tb.vWnd;
    }
    while (SendMessage(hwnds, TB_GETBUTTON, i, (LPARAM) &buttons[i]))
        i++;
    ChangeButtons(i, hwndd, buttons, ptr);
}

//-------------------------------------------------------------------------

int GetToolBarData(HWND hwnd, char *horiz, char *vert)
{
    char hbuf[512], vbuf[512];
    CCW_params *ptr = (CCW_params*)GetWindowLong(hwnd, 0);
    FormatToolBar(horiz, ptr->u.tb.hWnd);
    FormatToolBar(vert, ptr->u.tb.vWnd);
    return ptr->id;
}

//-------------------------------------------------------------------------

void SetToolBarData(HWND hwnd, char *horiz, char *vert)
{
    TBBUTTON buttons[60];
    CCW_params *ptr = (CCW_params*)GetWindowLong(hwnd, 0);
//    RECT c;
    int i;
//	ShowWindow(hwnd, SW_HIDE);
//    GetClientRect(ptr->hwnd, &c);
//    MoveWindow(ptr->hwnd, GRIPWIDTH, 0, 1000, 1000, 0);

    i = ParseToolBar(horiz, ptr->u.tb.hWnd, ptr, buttons);
    ChangeButtons(i, ptr->u.tb.hWnd, buttons, ptr);
    i = ParseToolBar(vert, ptr->u.tb.vWnd, ptr, buttons);
    ChangeButtons(i, ptr->u.tb.vWnd, buttons, ptr);
}

//-------------------------------------------------------------------------

static void ResizeContainer(CCW_params *ptr)
{
    CalculateLayout( - 1, 0);
}
//-------------------------------------------------------------------------

static LRESULT CALLBACK ControlWindWndProc(HWND hwnd, UINT iMessage,
    WPARAM wParam, LPARAM lParam)
{
    TBBUTTON button;
    LPTOOLTIPTEXT lpt;
    HBITMAP bmp;
    TBNOTIFY *tb;
    RECT r;
    PAINTSTRUCT ps;
    HDC dc;
    CCW_params *ptr;
    char *p;
    LPNMTOOLBAR lpnmtoolbar;
    int i,j,n;
    switch (iMessage)
    {
        case WM_NOTIFY:
            switch (((LPNMHDR)lParam)->code)
            {
            case TBN_BEGINADJUST:
                break;
            case TBN_QUERYINSERT:
                return TRUE;
            case TBN_QUERYDELETE:
                lpnmtoolbar = (LPNMTOOLBAR)lParam;
                return lpnmtoolbar->tbButton.idCommand != ID_TOOLBARCUSTOM;
            case TBN_GETBUTTONINFO:
                ptr = (CCW_params*)GetWindowLong(hwnd, 0);
                lpnmtoolbar = (LPNMTOOLBAR)lParam;
                if (lpnmtoolbar->iItem < ptr->u.tb.btncount)
                {
                    lpnmtoolbar->tbButton = ptr->u.tb.buttons[lpnmtoolbar->iItem];
                    lpnmtoolbar->tbButton.iString = ptr->u.tb.whints[lpnmtoolbar->iItem];
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
                return FALSE;
            case TBN_BEGINDRAG:
                break;
            case TBN_ENDDRAG:
                break;
            case TBN_RESET:
                ptr = (CCW_params*)GetWindowLong(hwnd, 0);
                ChangeButtons(ptr->u.tb.btncount, ptr->u.tb.hWnd, ptr
                    ->u.tb.buttons, ptr);
                ChangeButtons(ptr->u.tb.btncount, ptr->u.tb.vWnd, ptr
                    ->u.tb.buttons, ptr);
                ResizeContainer(ptr);
                PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
                
                break;
            case TBN_CUSTHELP:
                ptr = (CCW_params*)GetWindowLong(hwnd, 0);
                if (ptr->u.tb.helpitem)
                    ContextHelp(ptr->u.tb.helpitem);
                break;
//            case TBN_DRAGOVER:
//                return FALSE;
            case TBN_ENDADJUST:
            case TBN_TOOLBARCHANGE:
                ptr = (CCW_params*)GetWindowLong(hwnd, 0);
                CopyButtons(ptr);
                SetRectSize(ptr->u.tb.hWnd, ptr);
                SetRectSize(ptr->u.tb.vWnd, ptr);
                ResizeContainer(ptr);
                PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
                break;
            case TTN_NEEDTEXT:
                ptr = (CCW_params*)GetWindowLong(hwnd, 0);
                lpt = (LPTOOLTIPTEXT)lParam;
                p = GetTipText(ptr, lpt->hdr.idFrom);
                if (p)
                {
                    strcpy(lpt->szText, p);
                }
                break;
            case TBN_DROPDOWN:
                ptr = (CCW_params*)GetWindowLong(hwnd, 0);
                lpnmtoolbar = (LPNMTOOLBAR)lParam;
                return SendMessage(ptr->u.tb.notifyparent, WM_TOOLBARDROPDOWN, lpnmtoolbar->iItem, MAKELPARAM(lpnmtoolbar->rcButton.left, lpnmtoolbar->rcButton.bottom))
                            ? TBDDRET_DEFAULT : TBDDRET_NODEFAULT ;
//            default:
//                return SendMessage(ptr->u.tb.notifyparent, iMessage, wParam,
//                    lParam);
            }
            break;
        case WM_PAINT:
            dc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            break;
        case WM_COMMAND:

            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            if (wParam == ID_TOOLBARCUSTOM)
            {
                if (ptr->vertical)
                    PostMessage(ptr->u.tb.vWnd, TB_CUSTOMIZE, 0, 0);
                else
                    PostMessage(ptr->u.tb.hWnd, TB_CUSTOMIZE, 0, 0);
            }
            else
            {
                PostMessage(ptr->u.tb.notifyparent, iMessage, wParam, lParam);
            }
            return 0;
        case WM_CREATE:
            ptr = (CCW_params*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
            SetWindowLong(hwnd, 0, (DWORD)ptr);
            ptr->u.tb.btncount = 0;
            n = 10000;
            while (ptr->u.tb.buttons[ptr->u.tb.btncount].fsStyle || ptr
                ->u.tb.buttons[ptr->u.tb.btncount].idCommand)
            {
                ptr->u.tb.btncount++;
            }

            ptr->u.tb.whints = calloc(ptr->u.tb.btncount, sizeof(WCHAR *));
            ptr->u.tb.children = calloc(ptr->u.tb.btncount, sizeof(HWND));
            for (i=0; i <ptr->u.tb.btncount; i++)
            {
                n = strlen(ptr->u.tb.hints[i]) + 1;
                ptr->u.tb.whints[i] = calloc(sizeof(WCHAR), n);
                for (j=0; j < n; j++)
                   ptr->u.tb.whints[i][j] = ptr->u.tb.hints[i][j];
            }
            ptr->u.tb.children = calloc(ptr->u.tb.btncount, sizeof(HWND));
            ptr->u.tb.imageList = ImageList_Create(16, 16, ILC_COLOR32 , ptr->u.tb.bmpcount+1, ptr->u.tb.bmpcount+1);
            ptr->u.tb.disabledImageList = ImageList_Create(16, 16, ILC_COLOR32, ptr->u.tb.bmpcount+1, ptr->u.tb.bmpcount+1);
            
            bmp = LoadBitmap((HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), MAKEINTRESOURCE(ptr->u.tb.bmpid));
            bmp = ConvertToTransparent(bmp, 0xc0c0c0);
            ImageList_Add(ptr->u.tb.imageList, bmp, NULL);
//			ConvertToGray(bmp);
            Tint(bmp, RetrieveSysColor(COLOR_BTNSHADOW));
            ImageList_Add(ptr->u.tb.disabledImageList, bmp, NULL);
            DeleteObject(bmp);
            
            ptr->u.tb.hWnd = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | TBSTYLE_ALTDRAG
                | (ptr->u.tb.hints ? TBSTYLE_TOOLTIPS : 0) | TBSTYLE_FLAT | TBSTYLE_TRANSPARENT | CCS_ADJUSTABLE | CCS_NODIVIDER,
                0,0,0,0, hwnd,NULL, HINST_COMMCTRL, NULL);
            SendMessage(ptr->u.tb.hWnd, TB_SETIMAGELIST, (WPARAM)ptr->u.tb.bmpid, (LPARAM) ptr->u.tb.imageList);
            SendMessage(ptr->u.tb.hWnd, TB_SETDISABLEDIMAGELIST, (WPARAM)ptr->u.tb.bmpid, (LPARAM) ptr->u.tb.disabledImageList);
            SendMessage(ptr->u.tb.hWnd, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON),
                0);
            SendMessage(ptr->u.tb.hWnd, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);
            ChangeButtons(ptr->u.tb.btncount, ptr->u.tb.hWnd, ptr
                    ->u.tb.buttons, ptr);
            ptr->u.tb.vWnd = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD //| TBSTYLE_WRAPABLE
                | (ptr->u.tb.hints ? TBSTYLE_TOOLTIPS : 0) | TBSTYLE_FLAT | TBSTYLE_TRANSPARENT | CCS_NODIVIDER | CCS_ADJUSTABLE | CCS_VERT,
                0,0,0,0, hwnd,NULL, HINST_COMMCTRL, NULL);
            SendMessage(ptr->u.tb.vWnd, TB_SETIMAGELIST, (WPARAM)ptr->u.tb.bmpid, (LPARAM) ptr->u.tb.imageList);
            SendMessage(ptr->u.tb.vWnd, TB_SETDISABLEDIMAGELIST, (WPARAM)ptr->u.tb.bmpid, (LPARAM) ptr->u.tb.disabledImageList);

            SendMessage(ptr->u.tb.vWnd, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON),
                0);
            SendMessage(ptr->u.tb.hWnd, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);
            ChangeButtons(ptr->u.tb.btncount, ptr->u.tb.vWnd, ptr
                ->u.tb.buttons, ptr);
            SendMessage(hwnd, LCF_SETVERTICAL, 0, ptr->vertical);
            break;
        case WM_DESTROY:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            for (i=0; i <ptr->u.tb.btncount; i++)
            {
                free(ptr->u.tb.whints[i]);
            }
            free(ptr->u.tb.whints);
            free(ptr->u.tb.children);
            ImageList_Destroy(ptr->u.tb.imageList);
            ImageList_Destroy(ptr->u.tb.disabledImageList);
            free(ptr);
            break;
        case LCF_SETVERTICAL:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            ptr->vertical = (int)lParam;
            if (ptr->vertical)
            {
                ShowWindow(ptr->u.tb.hWnd, SW_HIDE);
                ShowWindow(ptr->u.tb.vWnd, SW_SHOW);
                SetRectSize(ptr->u.tb.vWnd, ptr);
                
            }
            else
            {
                ShowWindow(ptr->u.tb.vWnd, SW_HIDE);
                ShowWindow(ptr->u.tb.hWnd, SW_SHOW);
                MoveWindow(ptr->u.tb.hWnd, 0, INDENT, ptr->u.tb.hsize.cx, ptr
                    ->u.tb.hsize.cy, 1);
                SetRectSize(ptr->u.tb.hWnd, ptr);
            }
            return 0;
        case LCF_ADDCONTROL:
        {
            int i;
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            
            for (i=0; i < ptr->u.tb.btncount; i++)
            {
                if (ptr->u.tb.buttons[i].fsStyle & BTNS_SEP)
                {
                    if (ptr->u.tb.buttons[i].idCommand)
                        if (!wParam--)
                            break;
                }
            }
            if (i < ptr->u.tb.btncount)
            {
                RECT r;
                HWND hwndCtrl = (HWND)lParam;
                SendMessage(ptr->u.tb.hWnd, TB_GETITEMRECT, i, (LPARAM)&r);
                SetParent(hwndCtrl, ptr->u.tb.hWnd);
                MoveWindow(hwndCtrl, r.left, r.top, r.right- r.left, r.bottom - r.top, 1);
                ptr->u.tb.children[i] = hwndCtrl;
            }
        }
            return 0;
        case WM_CLOSE:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            dmgrHideWindow(ptr->child->id, TRUE);
            return 0;
        case WM_SIZE:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            if (ptr->vertical)
            {
                MoveWindow(ptr->u.tb.vWnd, INDENT, 0, ptr->u.tb.vsize.cx, ptr
                    ->u.tb.vsize.cy, 1);
//                MoveWindow(ptr->u.tb.vWnd, INDENT, 0, LOWORD(lParam)-INDENT, HIWORD
//                    (lParam), 1);
            }
            else
            {
                MoveWindow(ptr->u.tb.hWnd, 0, INDENT, ptr->u.tb.hsize.cx, ptr
                    ->u.tb.hsize.cy, 1);
//                MoveWindow(ptr->u.tb.hWnd, 0, INDENT, LOWORD(lParam), HIWORD
//                    (lParam)-INDENT, 1);
            }
            break;
        case WM_ERASEBKGND:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            {
                HBRUSH hbr = RetrieveSysBrush(COLOR_BTNFACE);
                GetClientRect(hwnd, &r);
                FillRect((HDC)wParam, &r, hbr);
                DeleteObject(hbr);
                if (ptr->parent && ptr->parent->type == LSGRIP)
                {
                    // in toolbar, make a nice gradient and round the right-hand side corners
                    HRGN rgn = CreateRoundRectRgn(r.left-10, r.top, r.right+1, r.bottom+1, 5, 5);
                    SelectClipRgn((HDC)wParam, rgn);
                    FillGradientTB((HDC)wParam, &r, ptr->vertical);
                    DeleteObject(rgn);
                }
            }
            return 1;
        default:
            if (iMessage >= WM_USER && iMessage <= WM_USER + 100)
            {
                ptr = (CCW_params*)GetWindowLong(hwnd, 0);
                if (ptr->vertical)
                {
                    if (iMessage == TB_SETSTATE || iMessage == TB_ENABLEBUTTON)
                        PostMessage(ptr->u.tb.hWnd, iMessage, wParam, lParam);
                    return SendMessage(ptr->u.tb.vWnd, iMessage, wParam, lParam);
                }
                else
                {
                    if (iMessage == TB_SETSTATE || iMessage == TB_ENABLEBUTTON)
                        PostMessage(ptr->u.tb.vWnd, iMessage, wParam, lParam);
                    return SendMessage(ptr->u.tb.hWnd, iMessage, wParam, lParam);
                }
            }
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}

//-------------------------------------------------------------------------

void RegisterToolBarWindow(HINSTANCE hInstance)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = 0;
    wc.lpfnWndProc = &ControlWindWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(LPVOID);
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = RetrieveSysBrush(COLOR_BTNFACE);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szToolBarWindClassName;
    RegisterClass(&wc);

}

//-------------------------------------------------------------------------

HWND CreateToolBarWindow(int id, HWND notify, HWND parent, int width, int
    height, int bmp, int bmpcount, TBBUTTON *buttons, char **hints, int
    vertical, char *title, int helpitem)
{
    CCW_params *p = calloc(sizeof(CCW_params), 1);
    if (p)
    {
        HWND hwnd;
        RECT r1, r2;
        p->type = LSTOOLBAR;
        p->id = id;
        p->vertical = vertical;
        p->u.tb.width = width;
        p->u.tb.height = height;
        p->u.tb.buttons = buttons;
        p->u.tb.hints = hints;
        p->u.tb.bmpid = bmp;
        p->u.tb.bmpcount = bmpcount;
        p->u.tb.notifyparent = notify;
        p->u.tb.helpitem = helpitem;
        p->hwnd = CreateWindowEx(0, szToolBarWindClassName, title, WS_CLIPSIBLINGS | WS_CHILD,
            CW_USEDEFAULT, CW_USEDEFAULT, 1000, 30, parent, (HMENU)0,
            (HINSTANCE)GetWindowLong(parent, GWL_HINSTANCE), p);
        if (id != -1)
            AddDockableToolbarWindow(p);
    }
    return p->hwnd;
}
