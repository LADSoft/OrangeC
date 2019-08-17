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

static char* szFocusClassName = "xccFocusRect";
static RECT lastbound;
static int drawnbound;
static HWND hwndFocus;

#define TRANSPARENT_COLOR 0xffffff

#define SPI_GETFOCUSBORDERWIDTH 0x200e
#define SPI_GETFOCUSBORDERHEIGHT 0x2010
static LRESULT CALLBACK FocusWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    HDC dc;
    HBRUSH brush;
    PAINTSTRUCT ps;
    RECT rect;
    int x, y;
    switch (iMessage)
    {
        case WM_CREATE:
            SetLayeredWindowAttributes(hwnd, TRANSPARENT_COLOR, 0xff, LWA_COLORKEY);
            break;
        case WM_PAINT:
            GetClientRect(hwnd, &rect);
            SystemParametersInfo(SPI_GETFOCUSBORDERWIDTH, 0, &x, 0);
            SystemParametersInfo(SPI_GETFOCUSBORDERHEIGHT, 0, &y, 0);
            dc = BeginPaint(hwnd, &ps);
            brush = CreateSolidBrush(TRANSPARENT_COLOR);
            FillRect(dc, &rect, brush);
            DeleteObject(brush);
            DrawFocusRect(dc, &rect);
            EndPaint(hwnd, &ps);
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
//-------------------------------------------------------------------------

void DrawBoundingRect(RECT* r1)
{
    SetWindowPos(hwndFocus, HWND_TOPMOST, r1->left, r1->top, r1->right - r1->left, r1->bottom - r1->top,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOCOPYBITS);
    lastbound = *r1;
    drawnbound = TRUE;
}
void HideBoundingRect(void)
{
    ShowWindow(hwndFocus, SW_HIDE);
    drawnbound = FALSE;
}
void CreateBoundingRect(HINSTANCE hInstance, HWND hwndFrame)
{
    static BOOL registered;

    if (!registered)
    {
        WNDCLASS wc;
        registered = TRUE;
        memset(&wc, 0, sizeof(wc));
        wc.style = 0;
        wc.lpfnWndProc = FocusWndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInstance;
        wc.hIcon = LoadIcon(0, IDI_APPLICATION);
        wc.hCursor = 0;
        wc.hbrBackground = 0;
        wc.lpszMenuName = 0;
        wc.lpszClassName = szFocusClassName;
        RegisterClass(&wc);
        hwndFocus = CreateWindowEx(WS_EX_LAYERED | WS_EX_NOACTIVATE, szFocusClassName, "", WS_CLIPSIBLINGS | WS_POPUP, 0, 0, 10, 10,
                                   hwndFrame, 0, hInstance, 0);
    }
}
