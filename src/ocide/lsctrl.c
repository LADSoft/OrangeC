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
#include "lsctrl.h"

extern HWND hwndClient;
static char *szControlWindClassName = "ladSoftControlWindow";
static HCURSOR hcurs, vcurs, lcurs, rcurs, mcurs;
static HBITMAP closebitmap, uparr, downarr, leftarr, rightarr, uparrdis,
    leftarrdis;

static void DrawBorder(HDC dc, RECT *r1, int vertical, int active)
{
    HBRUSH brush = CreateSolidBrush(RetrieveSysColor(active ? COLOR_ACTIVEBORDER :
        COLOR_INACTIVEBORDER));
    RECT r2;
    r2 =  *r1;
    if (vertical)
        r2.right = r2.left + TITLEWIDTH;
    else
        r2.right = r2.left + EDGEWIDTH;
    FillGradientTB(dc, &r2, vertical);

    r2 =  *r1;
    r2.left = r2.right - EDGEWIDTH;
    FillRect(dc, &r2, brush);

    r2 =  *r1;
    if (vertical)
        r2.bottom = r2.top + EDGEWIDTH;
    else
        r2.bottom = r2.top + TITLEWIDTH;
    FillGradientTB(dc, &r2, vertical);

    r2 =  *r1;
    r2.top = r2.bottom - EDGEWIDTH;
    FillRect(dc, &r2, brush);

    DeleteObject(brush);

}

//-------------------------------------------------------------------------

static void DrawTitle(HDC dc, RECT *r, int vertical)
{
    HPEN pen1, pen2, pen3, oldpen;
    pen1 = CreatePen(PS_SOLID, 1, RetrieveSysColor(COLOR_3DLIGHT));
    pen2 = CreatePen(PS_SOLID, 2, RetrieveSysColor(COLOR_3DHILIGHT));
    pen3 = CreatePen(PS_SOLID, 1, RetrieveSysColor(COLOR_3DSHADOW));
    if (!vertical)
    {
        r->top = TITLETOP;
        r->left += EDGEWIDTH;
        r->right -= EDGEWIDTH;
        oldpen = SelectObject(dc, pen1);
        SelectObject(dc, pen1);
        MoveToEx(dc, r->left + BARLEFT, r->top + BARTOP, 0);
        LineTo(dc, r->right - BARRIGHT + 1, r->top + BARTOP);
        SelectObject(dc, pen2);
        MoveToEx(dc, r->left + BARLEFT - 1, r->top + BARTOP + 1, 0);
        LineTo(dc, r->right - BARRIGHT, r->top + BARTOP + 1);
        SelectObject(dc, pen3);
        MoveToEx(dc, r->left + BARLEFT, r->top + BARTOP + 3, 0);
        LineTo(dc, r->right - BARRIGHT + 1, r->top + BARTOP + 3);
        SelectObject(dc, pen2);
        MoveToEx(dc, r->left + BARLEFT - 1, r->top + BARTOP + 5, 0);
        LineTo(dc, r->right - BARRIGHT, r->top + BARTOP + 5);
        SelectObject(dc, pen3);
        MoveToEx(dc, r->left + BARLEFT, r->top + BARTOP + 7, 0);
        LineTo(dc, r->right - BARRIGHT + 1, r->top + BARTOP + 7);
    }
    else
    {
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
    }
    SelectObject(dc, oldpen);
    DeleteObject(pen1);
    DeleteObject(pen2);
    DeleteObject(pen3);
}

//-------------------------------------------------------------------------

#define RANGE(x,y) ((x) == (y) || (x)+1 == (y)+1)
static void SetFlexBmp(CCW_params *ptr)
{
    if (ptr->vertical)
    {
        if (ptr->u.cw.disabled)
            SendMessage(ptr->u.cw.flexbutton, BM_SETIMAGE, IMAGE_BITMAP, 
                (LPARAM)leftarrdis);
        else if (ptr->u.cw.flexed)
            SendMessage(ptr->u.cw.flexbutton, BM_SETIMAGE, IMAGE_BITMAP, 
                (LPARAM)rightarr);
        else
            SendMessage(ptr->u.cw.flexbutton, BM_SETIMAGE, IMAGE_BITMAP, 
                (LPARAM)leftarr);
    }
    else
    {
        if (ptr->u.cw.disabled)
            SendMessage(ptr->u.cw.flexbutton, BM_SETIMAGE, IMAGE_BITMAP, 
                (LPARAM)uparrdis);
        else if (ptr->u.cw.flexed)
            SendMessage(ptr->u.cw.flexbutton, BM_SETIMAGE, IMAGE_BITMAP, 
                (LPARAM)downarr);
        else
            SendMessage(ptr->u.cw.flexbutton, BM_SETIMAGE, IMAGE_BITMAP, 
                (LPARAM)uparr);
    }
}

//-------------------------------------------------------------------------

static LRESULT CALLBACK ControlWindWndProc(HWND hwnd, UINT iMessage,
    WPARAM wParam, LPARAM lParam)
{
    RECT r,  *pr;
    PAINTSTRUCT ps;
    HDC dc;
    CCW_params *ptr;
    static int dragging;
    static HCURSOR oldCursor;
    POINT temppt;
    switch (iMessage)
    {
        case WM_NOTIFY:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            return SendMessage(ptr->child->hwnd, iMessage, wParam, lParam);
        case WM_SYSCOMMAND:
            if (wParam == SC_CLOSE)
                SendMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        case WM_COMMAND:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            if (HIWORD(wParam) == BN_CLICKED && (LOWORD(wParam) == 64371 ||
                LOWORD(wParam) == 64373))
            {
                SendMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
            }
            else if (HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == 64372)
            {
                SetFlexBmp(ptr);
                SetFocus(hwnd);
                dmgrFlex(ptr);
            }
            else
            {
                return SendMessage(ptr->child->hwnd, iMessage, wParam, lParam);
            }
            break;
        case WM_ERASEBKGND:
            return 0;

        case WM_LBUTTONDOWN:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            temppt.x = (long)(short)LOWORD(lParam);
            temppt.y = (long)(short)HIWORD(lParam);
            if ((ptr->vertical && temppt.x < 17) || (!ptr->vertical &&
                temppt.y < 17))
            {
                ClientToScreen(hwnd, &temppt);
                dmgrStartMoveClient(ptr, &temppt);
                oldCursor = SetCursor(mcurs);
                SetCapture(hwnd);
                dragging = TRUE;
            }
            break;
        case WM_LBUTTONUP:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            if (dragging)
            {
                SetCursor(oldCursor);
                oldCursor = 0;
            }
            if (dragging)
                ReleaseCapture();
            dragging = FALSE;
            dmgrEndMoveClient(ptr, NULL);
            break;
        case WM_MOUSEMOVE:
            temppt.x = (long)(short)LOWORD(lParam);
            temppt.y = (long)(short)HIWORD(lParam);
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            if (dragging)
            {
                ClientToScreen(hwnd, &temppt);
                dmgrMoveClient(ptr, &temppt, FALSE);
            }
            break;
        case WM_PAINT:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            dc = BeginPaint(hwnd, &ps);
            GetClientRect(hwnd, &r);
            DrawBorder(dc, &r, ptr->vertical, TRUE);
            DrawTitle(dc, &r, ptr->vertical);
            EndPaint(hwnd, &ps);
            break;
        case WM_CREATE:
            ptr = (CCW_params*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
            SetWindowLong(hwnd, 0, (DWORD)ptr);
            ptr->u.cw.active = 0;
            GetClientRect(hwnd, &r);
            ptr->u.cw.closebutton = CreateWindow("BUTTON", "", WS_VISIBLE |
                WS_CHILD | BS_BITMAP | BS_FLAT, (!ptr->vertical ? r.right
                - BUTTONCORNER - BUTTONWIDTH: BUTTONCORNER), BUTTONCORNER,
                BUTTONWIDTH, BUTTONWIDTH, hwnd, (HMENU)64371, (HINSTANCE)
                GetWindowLong(hwnd, GWL_HINSTANCE), 0);
            ptr->u.cw.flexbutton = CreateWindow("BUTTON", "", WS_VISIBLE |
                WS_CHILD | BS_BITMAP | BS_FLAT, (!ptr->vertical ? r.right
                - BUTTONCORNER - 2 * BUTTONWIDTH - 2: BUTTONCORNER), (!ptr
                ->vertical ? BUTTONCORNER : r.top + BUTTONCORNER + 2+2 *
                BUTTONWIDTH), BUTTONWIDTH, BUTTONWIDTH, hwnd, (HMENU)64372, 
                (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 0);
            ptr->u.cw.disabled = TRUE;
            SetFlexBmp(ptr);
            SendMessage(ptr->u.cw.closebutton, BM_SETIMAGE, IMAGE_BITMAP, 
                (LPARAM)closebitmap);
            return 0;
        case WM_DESTROY:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            DestroyWindow(ptr->u.cw.closebutton);
            DestroyWindow(ptr->u.cw.flexbutton);
            free(ptr);
            break;
        case WM_CLOSE:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            dmgrHideWindow(ptr->child->id, TRUE);
            return 0;
        case LCF_ADJUSTRECT:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            pr = (RECT*)lParam;
            if (ptr->vertical)
            {
                pr->right -= EDGEWIDTH;
                pr->left += TITLEWIDTH;
                pr->top += EDGEWIDTH;
                pr->bottom -= EDGEWIDTH;
            }
            else
            {
                pr->bottom -= EDGEWIDTH;
                pr->top += TITLEWIDTH;
                pr->left += EDGEWIDTH;
                pr->right -= EDGEWIDTH;

            }
            return 0;
        case LCF_SETFLEX:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            ptr->u.cw.disabled = !(int)lParam;
            ptr->u.cw.flexed = (int)wParam;
            SetFlexBmp(ptr);
            return 0;
        case LCF_SETVERTICAL:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            ptr->vertical = (int)lParam;
            InvalidateRect(hwnd, 0, 1);
            return 0;
        case LCF_SETACTIVE:
            return TRUE;
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLORSCROLLBAR:
        case WM_CTLCOLORSTATIC:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            return SendMessage(ptr->child->hwnd, iMessage, wParam, lParam);
        case WM_SIZE:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            SetFlexBmp(ptr);
            if (ptr->vertical)
            {
                MoveWindow(ptr->u.cw.closebutton, BUTTONCORNER, BUTTONCORNER,
                    BUTTONWIDTH, BUTTONWIDTH, 1);
                MoveWindow(ptr->u.cw.flexbutton, BUTTONCORNER, BUTTONCORNER +
                    BUTTONWIDTH + 2, BUTTONWIDTH, BUTTONWIDTH, 1);
            }
            else
            {
                MoveWindow(ptr->u.cw.closebutton, LOWORD(lParam) - BUTTONCORNER
                    - BUTTONWIDTH, BUTTONCORNER, BUTTONWIDTH, BUTTONWIDTH, 1);
                MoveWindow(ptr->u.cw.flexbutton, LOWORD(lParam) - BUTTONCORNER 
                    - 2 * BUTTONWIDTH - 2, BUTTONCORNER, BUTTONWIDTH,
                    BUTTONWIDTH, 1);
            }
            r.left = r.top = 0;
            r.right = LOWORD(lParam);
            r.bottom = HIWORD(lParam);
            SendMessage(hwnd, LCF_ADJUSTRECT,0, (LPARAM)&r);
            if (ptr->child)
                MoveWindow(ptr->child->hwnd, r.left, r.top, r.right - r.left, r.bottom-r.top, 1);
            return 0;
        case WM_DRAWITEM:
        case WM_MEASUREITEM:
            return SendMessage(GetParent(hwnd), iMessage, wParam, lParam);
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}

//-------------------------------------------------------------------------

void RegisterControlWindow(HINSTANCE hInstance)
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
    wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szControlWindClassName;
    RegisterClass(&wc);

    closebitmap = LoadBitmap(hInstance, "ID_CLOSEBMP");
    ChangeBitmapColor(closebitmap, 0xc0c0c0, RetrieveSysColor(COLOR_MENU));
    uparr = LoadBitmap(hInstance, "ID_UPARR");
    ChangeBitmapColor(uparr, 0xc0c0c0, RetrieveSysColor(COLOR_MENU));
    downarr = LoadBitmap(hInstance, "ID_DOWNARR");
    ChangeBitmapColor(downarr, 0xc0c0c0, RetrieveSysColor(COLOR_MENU));
    leftarr = LoadBitmap(hInstance, "ID_LEFTARR");
    ChangeBitmapColor(leftarr, 0xc0c0c0, RetrieveSysColor(COLOR_MENU));
    rightarr = LoadBitmap(hInstance, "ID_RIGHTARR");
    ChangeBitmapColor(rightarr, 0xc0c0c0, RetrieveSysColor(COLOR_MENU));
    uparrdis = LoadBitmap(hInstance, "ID_UPARRDIS");
    ChangeBitmapColor(uparrdis, 0xc0c0c0, RetrieveSysColor(COLOR_MENU));
    leftarrdis = LoadBitmap(hInstance, "ID_LEFTARRDIS");
    ChangeBitmapColor(leftarrdis, 0xc0c0c0, RetrieveSysColor(COLOR_MENU));
    hcurs = LoadCursor(NULL, IDC_SIZENS);
    vcurs = LoadCursor(NULL, IDC_SIZENS);
    lcurs = LoadCursor(NULL, IDC_SIZEWE);
    rcurs = LoadCursor(NULL, IDC_SIZEWE);
    mcurs = LoadCursor(NULL, IDC_SIZEALL);

}

//-------------------------------------------------------------------------

CCW_params *CreateControlWindow(HWND parent)
{
    CCW_params *p = calloc(sizeof(CCW_params), 1);
    if (p)
    {
        p->hwnd = CreateWindow(szControlWindClassName, 0, WS_CLIPSIBLINGS |
            WS_CLIPCHILDREN | WS_CHILD, 0, 0, 10, 10, parent, 0, 
            (HINSTANCE)GetWindowLong(parent, GWL_HINSTANCE), p);
        p->type = LSCONTROL;
    }
    return p;
}
