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

static char *szFrameClassName = "ladSoftFrameWindow";
static HCURSOR hcurs, vcurs;

#define FRAMEBODY 2
#define FRAMEWIDTH 5

static void DrawFrame(HDC dc, RECT *r, int vertical)
{
    HBRUSH brush, oldbrush;
    HPEN pen1, pen2, oldpen;
    pen1 = CreatePen(PS_SOLID, 2, RetrieveSysColor(COLOR_BTNHIGHLIGHT));
    pen2 = CreatePen(PS_SOLID, 1, RetrieveSysColor(COLOR_BTNSHADOW));
    brush = CreateSolidBrush(RetrieveSysColor(COLOR_BTNFACE));
    oldbrush = SelectObject(dc, brush);
    FillRect(dc, r, brush);

/*
    oldpen = SelectObject(dc, pen1);
    MoveToEx(dc, r->left, r->top, 0);
    LineTo(dc, r->right, r->top);

    SelectObject(dc, pen2);


    if (vertical)
    {
        SelectObject(dc, pen1);
        MoveToEx(dc, r->left, r->top + 1, 0);
        LineTo(dc, r->left, r->bottom - 2);

        SelectObject(dc, pen2);
        MoveToEx(dc, r->right - 1, r->top, 0);
        LineTo(dc, r->right - 1, r->bottom - 1);
    }
    else
    {
        SelectObject(dc, pen1);
        MoveToEx(dc, r->left + 1, r->top, 0);
        LineTo(dc, r->right - 1, r->top);

        SelectObject(dc, pen2);
        MoveToEx(dc, r->left + 1, r->bottom - 1, 0);
        LineTo(dc, r->right - 1, r->bottom - 1);
    }
    SelectObject(dc, oldpen);
    SelectObject(dc, oldbrush);
    */
    DeleteObject(pen1);
    DeleteObject(pen2);
    DeleteObject(brush);
}

//-------------------------------------------------------------------------

static LRESULT CALLBACK FrameWndProc(HWND hwnd, UINT iMessage,
    WPARAM wParam, LPARAM lParam)
{
    static BOOL captured;
    static POINT origin;
    static RECT bound;
    static HCURSOR oldCursor;
    static POINT last;
    RECT r;
    PAINTSTRUCT ps;
    HDC dc;
    static int dragging;
    POINT temppt;
    switch (iMessage)
    {
        case WM_SETCURSOR:
            GetClientRect(hwnd, &r);
            if (r.bottom-r.top > r.right-r.left)
                oldCursor = SetCursor(hcurs);
            else
                oldCursor = SetCursor(vcurs);
            break;
        case WM_TIMER:
            KillTimer(hwnd, 100);
            captured = 1;
            SetCapture(hwnd);
            break;
        case WM_LBUTTONDOWN:
            if (!captured)
            {
                // this is to let other windows settle
                SetTimer(hwnd, 100, 100, 0);
                origin.x = LOWORD(lParam);
                origin.y = HIWORD(lParam);
                GetWindowRect(hwnd, &bound);
                last.x = last.y = 0;
            }
            break;
        case WM_LBUTTONUP:
            if (captured)
            {
                captured = 0;
                HideBoundingRect();
                ReleaseCapture();
            }
            break;
        case WM_MOUSEMOVE:
            if (captured)
            {
                temppt.x = (int)(short)LOWORD(lParam);
                temppt.y = (int)(short)HIWORD(lParam);
                if (last.x == temppt.x && last.y == temppt.y)
                    break;
                last = temppt;
                ResizeDockWindow(hwnd, origin.x, origin.y, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
            }
            break;
        case WM_NOTIFY:
            break;
        case WM_COMMAND:
            break;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT:
            dc = BeginPaint(hwnd, &ps);
            GetClientRect(hwnd, &r);
            DrawFrame(dc, &r, r.bottom-r.top > r.right-r.left);
            EndPaint(hwnd, &ps);
            return 0;
        case WM_CREATE:
            break;
        case WM_DESTROY:
            break;
        case WM_CLOSE:
            break;
        case WM_SIZE:
            break ;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}

//-------------------------------------------------------------------------

void RegisterFrameWindow(HINSTANCE hInstance)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = 0;
    wc.lpfnWndProc = &FrameWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(LPVOID);
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = NULL;
    wc.hbrBackground = RetrieveSysBrush(COLOR_BTNFACE);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szFrameClassName;
    RegisterClass(&wc);

    hcurs = LoadCursor(0, IDC_SIZEWE);
    vcurs = LoadCursor(0, IDC_SIZENS);
}

//-------------------------------------------------------------------------

HWND CreateFrameWindow(HWND parent)
{
    return CreateWindow(szFrameClassName, 0, WS_CLIPSIBLINGS |
            WS_CLIPCHILDREN | WS_CHILD, 0, 0, 10, 10, parent, 0, 
            (HINSTANCE)GetWindowLong(parent, GWL_HINSTANCE), NULL);
}
