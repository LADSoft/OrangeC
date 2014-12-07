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
static char *szBlankClassName = "ladSoftBlankWindow";

//-------------------------------------------------------------------------

static LRESULT CALLBACK BlankWindowWndProc(HWND hwnd, UINT iMessage,
    WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC dc;
    CCW_params *ptr;
    switch (iMessage)
    {
        case WM_NOTIFY:
            SendMessage(GetParent(hwnd), iMessage, wParam, lParam);
            break;
        case WM_COMMAND:
            SendMessage(GetParent(hwnd), iMessage, wParam, lParam);
            break;
        case WM_PAINT:
            ptr = (CCW_params*)GetWindowLong(hwnd, 0);
            dc = BeginPaint(hwnd, &ps);
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
            break;
        case WM_SIZE:
            break;
        case WM_MOVE:
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}

//-------------------------------------------------------------------------

void RegisterBlankWindow(HINSTANCE hInstance)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_PARENTDC;
    wc.lpfnWndProc = &BlankWindowWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(LPVOID);
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = NULL;
    wc.hbrBackground = (HBRUSH)RetrieveSysBrush(COLOR_BTNFACE);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szBlankClassName;
    RegisterClass(&wc);
}

//-------------------------------------------------------------------------

CCW_params *CreateBlankWindow(HWND parent)
{
    CCW_params *p = calloc(sizeof(CCW_params), 1);
    if (p)
    {
        p->hwnd = CreateWindow(szBlankClassName, 0, WS_CLIPSIBLINGS |
            WS_CLIPCHILDREN | WS_CHILD, 0, 0, 10, 10, parent, 0, 
            (HINSTANCE)GetWindowLong(parent, GWL_HINSTANCE), p);
        p->type = LSBLANK;
    }
    return p;
}

