/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <richedit.h>
#include "header.h"

static char* szFrameClassName = "ladSoftFrameWindow";
static HCURSOR hcurs, vcurs;

#define FRAMEBODY 2
#define FRAMEWIDTH 5

static void DrawFrame(HDC dc, RECT* r, int vertical)
{
    HBRUSH brush, oldbrush;
    HPEN pen1, pen2;
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

static LRESULT CALLBACK FrameWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
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
            if (r.bottom - r.top > r.right - r.left)
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
            DrawFrame(dc, &r, r.bottom - r.top > r.right - r.left);
            EndPaint(hwnd, &ps);
            return 0;
        case WM_CREATE:
            break;
        case WM_DESTROY:
            break;
        case WM_CLOSE:
            break;
        case WM_SIZE:
            break;
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
    return CreateWindow(szFrameClassName, 0, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CHILD, 0, 0, 10, 10, parent, 0,
                        (HINSTANCE)GetWindowLong(parent, GWL_HINSTANCE), NULL);
}
