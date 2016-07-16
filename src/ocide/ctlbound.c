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

static char *szFocusClassName = "xccFocusRect";
static RECT lastbound;
static int drawnbound;
static HWND hwndFocus;

#define TRANSPARENT_COLOR 0xffffff

#define SPI_GETFOCUSBORDERWIDTH 0x200e
#define SPI_GETFOCUSBORDERHEIGHT 0x2010
static LRESULT CALLBACK FocusWndProc(HWND hwnd, UINT iMessage,
    WPARAM wParam, LPARAM lParam)
{
    HDC dc;
    HBRUSH brush;
    HPEN pen;
    PAINTSTRUCT ps;
    RECT rect;
    int x,y;
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

void DrawBoundingRect(RECT *r1)
{
    SetWindowPos(hwndFocus, HWND_TOPMOST, r1->left, r1->top, r1->right - r1->left, r1->bottom - r1->top,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOCOPYBITS);
    lastbound =  *r1;
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
        hwndFocus = CreateWindowEx(WS_EX_LAYERED | WS_EX_NOACTIVATE, 
                          szFocusClassName, "", WS_CLIPSIBLINGS | WS_POPUP,
                          0,0,10,10, hwndFrame, 0, hInstance, 0);
    }
}
