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
#define GRIP_DEFINES
#include "lsctrl.h"


extern HWND hwndClient;
static char *szGripClassName = "ladSoftGripWindow";

static HCURSOR mcurs;

static void DrawBorder(HWND hwnd, HDC dc)
{
//    RECT r;
//    GetClientRect(hwnd, &r);
//    r.right += GetSystemMetrics(SM_CXBORDER) *2;
//    r.bottom += GetSystemMetrics(SM_CYBORDER) *2;
//    DrawEdge(dc, &r, BDR_RAISEDOUTER | BDR_SUNKENINNER, BF_RECT | BF_FLAT);
}

void FillGradientTB(HDC dc, RECT *r, int vertical)
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
    
    GradientFill(dc, vertex, 2, &gRect, 1, vertical ? GRADIENT_FILL_RECT_H : GRADIENT_FILL_RECT_V);
}
//-------------------------------------------------------------------------

static void DrawTitle(HDC dc, RECT *r, int vertical)
{
    HPEN pen1, pen2, pen3, oldpen;


    pen1 = CreatePen(PS_SOLID, 1, RetrieveSysColor(COLOR_3DLIGHT));
    pen2 = CreatePen(PS_SOLID, 2, RetrieveSysColor(COLOR_3DHILIGHT));
    pen3 = CreatePen(PS_SOLID, 1, RetrieveSysColor(COLOR_3DSHADOW));
//    pen1 = CreatePen(PS_SOLID, 1, RetrieveSysColor(COLOR_ACTIVEBORDER));
//    pen2 = CreatePen(PS_SOLID, 2, RetrieveSysColor(COLOR_INACTIVEBORDER));
//    pen3 = CreatePen(PS_SOLID, 1, RetrieveSysColor(COLOR_GRADIENTACTIVECAPTION));
    if (vertical)
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

static LRESULT CALLBACK GripWndProc(HWND hwnd, UINT iMessage,
    WPARAM wParam, LPARAM lParam)
{
    RECT r;
    PAINTSTRUCT ps;
    HDC dc;
    CCW_params *ptr;
    static int dragging;
    static HCURSOR oldCursor;
    POINT temppt;
    int cx, cy;
    switch (iMessage)
    {
        case WM_SETTEXT:
            break;
        case WM_COMMAND:
            break;
        case WM_NOTIFY:
            break;
        case WM_LBUTTONDOWN:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            temppt.x = (long)(short)LOWORD(lParam);
            temppt.y = (long)(short)HIWORD(lParam);
            if ((ptr->vertical && temppt.y < 17) || (!ptr->vertical && temppt.x
                < 17))
            {
                ClientToScreen(hwnd, &temppt);
                dmgrStartMoveGrip(ptr, &temppt);
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
            dmgrEndMoveGrip(ptr);
            break;
        case WM_MOUSEMOVE:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            if (dragging)
            {
                temppt.x = (long)(short)LOWORD(lParam);
                temppt.y = (long)(short)HIWORD(lParam);
                ClientToScreen(hwnd, &temppt);
                dmgrMoveGrip(ptr, &temppt);
            }
            break;
        case WM_PAINT:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            dc = BeginPaint(hwnd, &ps);
            GetClientRect(hwnd, &r);
            DrawBorder(hwnd, dc);
            DrawTitle(dc, &r, ptr->vertical);
            EndPaint(hwnd, &ps);
            break;
        case WM_CREATE:
            ptr = (CCW_params*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
            SetWindowLong(hwnd, 0, (DWORD)ptr);
            return 0;
        case WM_DESTROY:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            free(ptr);
            break;
        case WM_CLOSE:
            return 0;
        case WM_SETFOCUS:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            SetFocus(ptr->child->hwnd);
            return 0;
        case LCF_SETVERTICAL:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            ptr->vertical = (int)lParam;
            InvalidateRect(hwnd, 0, 0);
            return 0;
        case WM_SIZE:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            cx = 0; // GetSystemMetrics(SM_CXFRAME) ;
            cy = 0; // GetSystemMetrics(SM_CYFRAME) ;
            if (ptr->child)
            {
                if (ptr->vertical)
                    MoveWindow(ptr->child->hwnd, cx, TITLEWIDTH + cy, LOWORD(lParam)
                        - cx, HIWORD(lParam) - TITLEWIDTH - cy, 1);
                else
                    MoveWindow(ptr->child->hwnd, TITLEWIDTH + cx, cy, LOWORD(lParam)
                        - TITLEWIDTH - cx, HIWORD(lParam) - cy, 1);
            }
            return 0;
        case WM_ERASEBKGND:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            {
                HRGN rgn;
                HBRUSH hbr = RetrieveSysBrush(COLOR_BTNFACE);
                GetClientRect(hwnd, &r);
                FillRect((HDC)wParam, &r, hbr);
                rgn = CreateRoundRectRgn(r.left+2, r.top, r.right+1, r.bottom+1, 5, 5);
                SelectClipRgn((HDC)wParam, rgn);
                FillGradientTB((HDC)wParam, &r, ptr->vertical);
                DeleteObject(rgn);
                DeleteObject(hbr);
            }
            return TRUE;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}

//-------------------------------------------------------------------------

void RegisterGripWindow(HINSTANCE hInstance)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_PARENTDC;
    wc.lpfnWndProc = &GripWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(LPVOID);
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = RetrieveSysBrush(COLOR_BTNFACE); //CreateSolidBrush(RetrieveSysColor(COLOR_3DFACE));
    wc.lpszMenuName = 0;
    wc.lpszClassName = szGripClassName;
    RegisterClass(&wc);

    mcurs = LoadCursor(hInstance, "ID_MOVECUR");

}

//-------------------------------------------------------------------------

CCW_params *CreateGripWindow(HWND parent)
{
    CCW_params *p = calloc(sizeof(CCW_params), 1);
    if (p)
    {
        p->hwnd = CreateWindow(szGripClassName, 0, WS_CLIPSIBLINGS |
            WS_CHILD, 0, 0, 10, 10, parent, 0, 
            (HINSTANCE)GetWindowLong(parent, GWL_HINSTANCE), p);
        p->type = LSGRIP;
    }
    return p;
}
