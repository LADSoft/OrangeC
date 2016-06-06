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
#include "idewinconst.h"

#define INDENT 0

#define TITLEWIDTH 15
#define RIGHTWIDTH 5
#define EDGEWIDTH 1
#define TITLETOP 5
#define BARLEFT 3
#define BARRIGHT (BUTTONCORNER + 2 * BUTTONWIDTH + 4)
#define BARTOP 0
#define BUTTONCORNER 1
#define BUTTONWIDTH 12

extern HWND hwndFrame;

static WNDPROC oldproc;
static char *szToolBarWindClassName = "ctlToolBar";
static char *szextToolBarWindClassName = "ctlToolBarExt";

typedef struct {
    DWORD id;
    HWND hWnd;
    RECT pos;
    char **hints;
    WCHAR **whints;
    TBBUTTON *buttons;
    int btncount;
    int bmpid;
    int bmpcount;
    HWND *children;
    DWORD helpItem;
    HWND notifyParent;
    HIMAGELIST imageList, disabledImageList;
} TB_Params;
static TBBUTTON sep = 
{
    0, 0, TBSTATE_WRAP, TBSTYLE_SEP | TBSTYLE_FLAT, {0}, 0, -1
};
static void GradientFillTB(HDC dc, RECT *r)
{
    TRIVERTEX vertex[2] ;
    GRADIENT_RECT gRect;
    vertex[0].x     = r->left;
    vertex[0].y     = r->top;
    vertex[0].Red   = (RetrieveSysColor(COLOR_BTNHIGHLIGHT) & 0xff) << 8;
    vertex[0].Green = RetrieveSysColor(COLOR_BTNHIGHLIGHT) & 0xff00;
    vertex[0].Blue  = (RetrieveSysColor(COLOR_BTNHIGHLIGHT) & 0xff0000) >> 8;
    vertex[0].Alpha = 0x0000;
    
    vertex[1].x     = r->right;
    vertex[1].y     = r->bottom; 
    vertex[1].Red   = (RetrieveSysColor(COLOR_BTNSHADOW) & 0xff) << 8;
    vertex[1].Green = RetrieveSysColor(COLOR_BTNSHADOW) & 0xff00;
    vertex[1].Blue  = (RetrieveSysColor(COLOR_BTNSHADOW) & 0xff0000) >> 8;
    vertex[1].Alpha = 0x0000;
    
    gRect.UpperLeft  = 0;
    gRect.LowerRight = 1;
    
    GradientFill(dc, vertex, 2, &gRect, 1, GRADIENT_FILL_RECT_V);
}
static void DrawTitle(HDC dc, RECT *r)
{
    HPEN pen1, pen2, pen3, oldpen;


    pen1 = CreatePen(PS_SOLID, 1, RetrieveSysColor(COLOR_3DLIGHT));
    pen2 = CreatePen(PS_SOLID, 2, RetrieveSysColor(COLOR_3DHILIGHT));
    pen3 = CreatePen(PS_SOLID, 1, RetrieveSysColor(COLOR_3DSHADOW));
    r->left = TITLETOP;
    r->top += EDGEWIDTH;
    r->bottom -= EDGEWIDTH;
    oldpen = SelectObject(dc, pen1);
    MoveToEx(dc, r->left + BARTOP, r->top + BARRIGHT - 1, 0);
    LineTo(dc, r->left + BARTOP, r->bottom - BARLEFT);
    SelectObject(dc, pen2);
    MoveToEx(dc, r->left + BARTOP + 1, r->top + BARRIGHT, 0);
    LineTo(dc, r->left + BARTOP + 1, r->bottom - BARLEFT + 1);
    SelectObject(dc, pen3);
    MoveToEx(dc, r->left + BARTOP + 3, r->top + BARRIGHT - 1, 0);
    LineTo(dc, r->left + BARTOP + 3, r->bottom - BARLEFT);
    SelectObject(dc, pen2);
    MoveToEx(dc, r->left + BARTOP + 5, r->top + BARRIGHT, 0);
    LineTo(dc, r->left + BARTOP + 5, r->bottom - BARLEFT + 1);
    SelectObject(dc, pen3);
    MoveToEx(dc, r->left + BARTOP + 7, r->top + BARRIGHT - 1, 0);
    LineTo(dc, r->left + BARTOP + 7, r->bottom - BARLEFT);
    SelectObject(dc, oldpen);
    DeleteObject(pen1);
    DeleteObject(pen2);
    DeleteObject(pen3);
}
static char *GetTipText(TB_Params *ptr, int id)
{
    int i;
    for (i = 0; i < ptr->btncount; i++)
        if (ptr->buttons[i].idCommand == id)
        {
            return ptr->hints[i];
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

static int ParseToolBar(char *buf, HWND hwnd, TB_Params *ptr, TBBUTTON
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
            if (hwnd == ptr->hWnd)
                buttons[rv].fsState &= ~TBSTATE_WRAP;
            buttons[rv].idCommand = -id/1000;
            id = -(id) % 1000;
            buttons[rv++].iBitmap = id;
        }
        else
        {
            for (i = 0; i < ptr->btncount; i++)
                if (ptr->buttons[i].idCommand == id)
                {
                    int state;
                    if (!IsWindow(hwnd))
                        state = FALSE;
                    else
                        state = SendMessage(hwnd, TB_ISBUTTONENABLED, id, 0) ? TBSTATE_ENABLED : 0;
                    buttons[rv] = ptr->buttons[i];
                    buttons[rv].fsState = state;
                    if (hwnd == ptr->hWnd)
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

static void SetRectSize(HWND hwnd, TB_Params *ptr)
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
    SendMessage(hwnd, TB_SETROWS, MAKEWPARAM(1, FALSE), (LPARAM) &r);
    SendMessage(hwnd, TB_GETITEMRECT, num - 1, (LPARAM) &r);
    ptr->pos.right = r.right + GRIPWIDTH + RIGHTWIDTH;
    ptr->pos.bottom = r.bottom + GetSystemMetrics(SM_CYBORDER) * 2;
    MoveWindow(ptr->hWnd, GRIPWIDTH, 0, ptr->pos.right-GRIPWIDTH-RIGHTWIDTH, ptr->pos.bottom, 1);
    MoveWindow(hwnd, 0, 0, ptr->pos.right, ptr->pos.bottom, 1);
    for (i=0; i < ptr->btncount; i++)
        if (ptr->children[i])
        {
            SetWindowPos(ptr->children[i], 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_HIDEWINDOW);
        }
    for (i=0; i < num; i++)
        if ((buttons[i].fsStyle & TBSTYLE_SEP) && buttons[i].idCommand)
        {
            int j;
            for (j=0; j < ptr->btncount; j++)
            {

                if (buttons[i].idCommand == ptr->buttons[j].idCommand)
                {
                    if (ptr->children[j])
                    {
                        RECT r;
                        SendMessage(hwnd, TB_GETITEMRECT, i, (LPARAM)&r);
                        SetWindowPos(ptr->children[j], 0, r.left, r.top, r.right- r.left, r.bottom - r.top, SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
                    }
                }
            }
        }
}

//-------------------------------------------------------------------------

static void ChangeButtons(int num, HWND hwnd, TBBUTTON *buttons, TB_Params
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


//-------------------------------------------------------------------------

int GetToolBarData2(HWND hwnd, char *horiz)
{
    TB_Params *ptr = (TB_Params*)GetWindowLong(hwnd, 0);
    FormatToolBar(horiz, ptr->hWnd);
    return ptr->id;
}

//-------------------------------------------------------------------------

void SetToolBarData2(HWND hwnd, char *horiz)
{
    TBBUTTON buttons[60];
    TB_Params *ptr = (TB_Params*)GetWindowLong(hwnd, 0);
//    RECT c;
    int i;
//	ShowWindow(hwnd, SW_HIDE);
//    GetClientRect(ptr->hWnd, &c);
//    MoveWindow(ptr->hWnd, GRIPWIDTH, 0, 1000, 1000, 0);

    i = ParseToolBar(horiz, ptr->hWnd, ptr, buttons);
    ChangeButtons(i, ptr->hWnd, buttons, ptr);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
static LRESULT CALLBACK extToolBarWndProc(HWND hwnd, UINT iMessage,
    WPARAM wParam, LPARAM lParam)
{
    
    switch (iMessage)
    {
        case WM_PAINT:
        {
            RECT rect;
            HDC hDC, hdouble;
            HBITMAP bitmap;
            PAINTSTRUCT ps;
            GetClientRect(hwnd, &rect);
            hDC = BeginPaint(hwnd, &ps);
            hdouble = CreateCompatibleDC(hDC);
            bitmap = CreateCompatibleBitmap(hDC, rect.right, rect.bottom);
            SelectObject(hdouble, bitmap);
            SendMessage(hwnd, WM_PRINTCLIENT, (WPARAM)hdouble, 0);
            BitBlt(hDC, 0, 0, rect.right, rect.bottom, hdouble, 0, 0, SRCCOPY);
            DeleteObject(bitmap);
            DeleteObject(hdouble);
            EndPaint(hwnd, &ps);
        }
        return 0;
    }
    return CallWindowProc(oldproc, hwnd, iMessage, wParam, lParam);
}

static LRESULT CALLBACK ControlWindWndProc(HWND hwnd, UINT iMessage,
    WPARAM wParam, LPARAM lParam)
{
    LPTOOLTIPTEXT lpt;
    HBITMAP bmp;
    RECT r;
    PAINTSTRUCT ps;
    HDC dc;
    TB_Params *ptr;
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
                ptr = (TB_Params*)GetWindowLong(hwnd, 0);
                lpnmtoolbar = (LPNMTOOLBAR)lParam;
                if (lpnmtoolbar->iItem < ptr->btncount)
                {
                    lpnmtoolbar->tbButton = ptr->buttons[lpnmtoolbar->iItem];
                    lpnmtoolbar->tbButton.iString = (int)ptr->whints[lpnmtoolbar->iItem];
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
                ptr = (TB_Params*)GetWindowLong(hwnd, 0);
                ChangeButtons(ptr->btncount, ptr->hWnd, ptr
                    ->buttons, ptr);
                PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
                
                break;
            case TBN_CUSTHELP:
                ptr = (TB_Params*)GetWindowLong(hwnd, 0);
                if (ptr->helpItem)
                    ContextHelp(ptr->helpItem);
                break;
//            case TBN_DRAGOVER:
//                return FALSE;
            case TBN_ENDADJUST:
            case TBN_TOOLBARCHANGE:
                ptr = (TB_Params*)GetWindowLong(hwnd, 0);
                SetRectSize(ptr->hWnd, ptr);
                PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
                break;
            case TTN_NEEDTEXT:
                ptr = (TB_Params*)GetWindowLong(hwnd, 0);
                lpt = (LPTOOLTIPTEXT)lParam;
                p = GetTipText(ptr, lpt->hdr.idFrom);
                if (p)
                {
                    strcpy(lpt->szText, p);
                }
                break;
            case TBN_DROPDOWN:
                ptr = (TB_Params*)GetWindowLong(hwnd, 0);
                lpnmtoolbar = (LPNMTOOLBAR)lParam;
                return SendMessage(ptr->notifyParent, WM_TOOLBARDROPDOWN, lpnmtoolbar->iItem, MAKELPARAM(lpnmtoolbar->rcButton.left, lpnmtoolbar->rcButton.bottom))
                            ? TBDDRET_DEFAULT : TBDDRET_NODEFAULT ;
//            default:
//                return SendMessage(ptr->t, iMessage, wParam,
//                    lParam);
            }
            break;
        case WM_PAINT:
            dc = BeginPaint(hwnd, &ps);
            GetClientRect(hwnd, &r);
            DrawTitle(hwnd, &r);
            EndPaint(hwnd, &ps);
            break;
        case WM_COMMAND:

            ptr = (TB_Params*)GetWindowLong(hwnd, 0);
            if (wParam == ID_TOOLBARCUSTOM)
            {
                PostMessage(ptr->hWnd, TB_CUSTOMIZE, 0, 0);
            }
            else
            {
                PostMessage(ptr->notifyParent, iMessage, wParam, lParam);
            }
            return 0;
        case WM_CREATE:
            ptr = (TB_Params*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
            SetWindowLong(hwnd, 0, (DWORD)ptr);
            ptr->btncount = 0;
            n = 10000;
            while (ptr->buttons[ptr->btncount].fsStyle || ptr
                ->buttons[ptr->btncount].idCommand)
            {
                ptr->btncount++;
            }

            ptr->whints = calloc(ptr->btncount, sizeof(WCHAR *));
            ptr->children = calloc(ptr->btncount, sizeof(HWND));
            for (i=0; i <ptr->btncount; i++)
            {
                n = strlen(ptr->hints[i]) + 1;
                ptr->whints[i] = calloc(sizeof(WCHAR), n);
                for (j=0; j < n; j++)
                   ptr->whints[i][j] = ptr->hints[i][j];
            }
            ptr->children = calloc(ptr->btncount, sizeof(HWND));
            ptr->imageList = ImageList_Create(16, 16, ILC_COLOR32 , ptr->bmpcount+1, ptr->bmpcount+1);
            ptr->disabledImageList = ImageList_Create(16, 16, ILC_COLOR32, ptr->bmpcount+1, ptr->bmpcount+1);
            
            bmp = LoadBitmap((HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), MAKEINTRESOURCE(ptr->bmpid));
            bmp = ConvertToTransparent(bmp, 0xc0c0c0);
            ImageList_Add(ptr->imageList, bmp, NULL);
//			ConvertToGray(bmp);
            Tint(bmp, RetrieveSysColor(COLOR_BTNSHADOW));
            ImageList_Add(ptr->disabledImageList, bmp, NULL);
            DeleteObject(bmp);
            
            ptr->hWnd = CreateWindowEx(0, szextToolBarWindClassName, NULL, WS_CHILD | TBSTYLE_ALTDRAG
                | (ptr->hints ? TBSTYLE_TOOLTIPS : 0) | TBSTYLE_FLAT | TBSTYLE_TRANSPARENT | CCS_ADJUSTABLE | CCS_NODIVIDER,
                0,0,0,0, hwnd,NULL, HINST_COMMCTRL, NULL);
            SendMessage(ptr->hWnd, TB_SETIMAGELIST, (WPARAM)ptr->bmpid, (LPARAM) ptr->imageList);
            SendMessage(ptr->hWnd, TB_SETDISABLEDIMAGELIST, (WPARAM)ptr->bmpid, (LPARAM) ptr->disabledImageList);
            SendMessage(ptr->hWnd, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON),
                0);
            SendMessage(ptr->hWnd, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);
            ChangeButtons(ptr->btncount, ptr->hWnd, ptr
                    ->buttons, ptr);
            break;
        case WM_DESTROY:
            ptr = (TB_Params*)GetWindowLong(hwnd, 0);
            for (i=0; i <ptr->btncount; i++)
            {
                free(ptr->whints[i]);
            }
            free(ptr->whints);
            free(ptr->children);
            ImageList_Destroy(ptr->imageList);
            ImageList_Destroy(ptr->disabledImageList);
            free(ptr);
            break;
        case LCF_ADDCONTROL:
        {
            int i;
            ptr = (TB_Params*)GetWindowLong(hwnd, 0);
            
            for (i=0; i < ptr->btncount; i++)
            {
                if (ptr->buttons[i].fsStyle & BTNS_SEP)
                {
                    if (ptr->buttons[i].idCommand)
                        if (!wParam--)
                            break;
                }
            }
            if (i < ptr->btncount)
            {
                RECT r;
                HWND hwndCtrl = (HWND)lParam;
                SendMessage(ptr->hWnd, TB_GETITEMRECT, i, (LPARAM)&r);
                SetParent(hwndCtrl, ptr->hWnd);
                MoveWindow(hwndCtrl, r.left, r.top, r.right- r.left, r.bottom - r.top, 1);
                ptr->children[i] = hwndCtrl;
                BringWindowToTop(hwndCtrl);
            }
        }
            return 0;
        case WM_CLOSE:
            ptr = (TB_Params*)GetWindowLong(hwnd, 0);
            return 0;
        case WM_SIZE:
            ptr = (TB_Params*)GetWindowLong(hwnd, 0);
            MoveWindow(ptr->hWnd, 0, GRIPWIDTH, ptr->pos.right, ptr
                ->pos.bottom, 1);
            break;
        case WM_ERASEBKGND:
            ptr = (TB_Params*)GetWindowLong(hwnd, 0);
            {
                HBRUSH hbr = RetrieveSysBrush(COLOR_BTNFACE);
                HRGN rgn;
                GetClientRect(hwnd, &r);
                FillRect((HDC)wParam, &r, hbr);
                DeleteObject(hbr);
                // in toolbar, make a nice gradient and round the right-hand side corners
                rgn = CreateRoundRectRgn(r.left, r.top, r.right+1, r.bottom+1, 5, 5);
                SelectClipRgn((HDC)wParam, rgn);
                GradientFillTB((HDC)wParam, &r);
                DeleteObject(rgn);
            }
           return 1;
        default:
            if (iMessage >= WM_USER && iMessage <= WM_USER + 100)
            {
                ptr = (TB_Params*)GetWindowLong(hwnd, 0);
                return SendMessage(ptr->hWnd, iMessage, wParam, lParam);
            }
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}

//-------------------------------------------------------------------------

void RegisterToolBarWindow2(HINSTANCE hInstance)
{
    WNDCLASS wc;
    GetClassInfo(0, TOOLBARCLASSNAME, &wc);
    oldproc = wc.lpfnWndProc;
    wc.lpfnWndProc = &extToolBarWndProc;
    wc.lpszClassName = szextToolBarWindClassName;
    wc.hInstance = hInstance;
    RegisterClass(&wc);
    
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

HWND CreateToolBarWindow2(int id, HWND notify, HWND parent, int bmp, int bmpcount, TBBUTTON *buttons, char **hints,
    char *title, int helpItem)
{
    TB_Params *p = calloc(sizeof(TB_Params), 1);
    if (p)
    {
        p->id = id;
        p->buttons = buttons;
        p->hints = hints;
        p->bmpid = bmp;
        p->bmpcount = bmpcount;
        p->notifyParent = notify;
        p->helpItem = helpItem;
        p->hWnd = CreateWindowEx(0, szToolBarWindClassName, title, WS_CLIPSIBLINGS | WS_CHILD | WS_CLIPCHILDREN,
            CW_USEDEFAULT, CW_USEDEFAULT, 1000, 30, parent, (HMENU)0,
            (HINSTANCE)GetWindowLong(parent, GWL_HINSTANCE), p);
    }
    return p->hWnd;
}
