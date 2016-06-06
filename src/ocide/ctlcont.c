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
static char *szControlWindClassName = "ctlContainerWindow";

#define TW_SELECT 10000
#define TW_ADD 10001
#define TW_REMOVE 10002
#define TW_GETACTIVE 10003
#define TW_CLOSE 10004
typedef struct
{
    HWND hwnd;
    RECT pos;
    HWND *children;
    int childCount;
    int activeIndex;
    HWND tabCtl;
} TW_Params;
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

static LRESULT CALLBACK ControlWindWndProc(HWND hwnd, UINT iMessage,
    WPARAM wParam, LPARAM lParam)
{
    RECT r,  *pr;
    PAINTSTRUCT ps;
    HDC dc;
    TW_Params *ptr;
    int i;
    LPNMHDR nmh;
    switch (iMessage)
    {
        case WM_NOTIFY:
            nmh = (LPNMHDR)lParam;
            if (nmh->code == TABN_SELECTED)
            {
                LSTABNOTIFY *p = (LSTABNOTIFY *)nmh;
                ptr = (TW_Params*)GetWindowLong(hwnd, 0);
                ShowWindow(ptr->children[ptr->activeIndex], SW_HIDE);
                for (i=0; i < ptr->childCount; i++)
                    if (p->lParam == (LPARAM)ptr->children[i])
                        ptr->activeIndex = i;
                ShowWindow(ptr->children[ptr->activeIndex], SW_SHOW);
                GetClientRect(hwnd, &r);
                if (ptr->childCount > 1)
                    r.bottom -= 25;
                MoveWindow(ptr->children[ptr->activeIndex], r.left, r.top, r.right - r.left, r.bottom - r.top, 1);
                break;
            }
            break;
        case WM_SYSCOMMAND:
            if (wParam == SC_CLOSE)
                SendMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT:
            ptr = (TW_Params*)GetWindowLong(hwnd, 0);
            dc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            break;
        case WM_CREATE:
            ptr = (TW_Params*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
            SetWindowLong(hwnd, 0, (DWORD)ptr);
            ptr->tabCtl = CreateLsTabWindow(hwnd,TABS_BOTTOM | TABS_HOTTRACK | TABS_FLAT);
            ApplyDialogFont(ptr->tabCtl);
            return 0;
        case WM_DESTROY:
            ptr = (TW_Params*)GetWindowLong(hwnd, 0);
            DestroyWindow(ptr->tabCtl);
            free(ptr);
            break;
        case WM_CLOSE:
            ptr = (TW_Params*)GetWindowLong(hwnd, 0);
            return 0;
        case TW_SELECT:
            ptr = (TW_Params*)GetWindowLong(hwnd, 0);
            for (i=0; i < ptr->childCount; i++)
                if (lParam == (LPARAM)ptr->children[i])
                {
                    ShowWindow(ptr->children[ptr->activeIndex], SW_HIDE);
                    ptr->activeIndex = i;
                    ShowWindow(ptr->children[ptr->activeIndex], SW_SHOW);
                    GetClientRect(hwnd, &r);
                    if (ptr->childCount > 1)
                        r.bottom -= 25;
                    MoveWindow(ptr->children[ptr->activeIndex], r.left, r.top, r.right - r.left, r.bottom - r.top, 1);
                    break;
                }
            break;
        case TW_ADD:
            ptr = (TW_Params*)GetWindowLong(hwnd, 0);
            ptr->childCount++;
            ptr->children = realloc(ptr->children, ptr->childCount * sizeof(HWND));
            ptr->children[ptr->childCount-1] = (HWND)lParam;
            if (ptr->childCount == 2)
            {
                ShowWindow(ptr->tabCtl, SW_SHOW);
            }
            SendMessage(hwnd, TW_SELECT, 0, lParam);
            break;
        case TW_GETACTIVE:
            ptr = (TW_Params*)GetWindowLong(hwnd, 0);
            return (LRESULT)ptr->children[ptr->activeIndex];
        case TW_REMOVE:
            ptr = (TW_Params*)GetWindowLong(hwnd, 0);
            for (i=0; i < ptr->childCount; i++)
                if (lParam == (LPARAM)ptr->children[i])
                {
                    if (i < ptr->activeIndex)
                        ptr->activeIndex--;
                    if (i != ptr->childCount -1)
                    {
                        memcpy(ptr->children+i, &ptr->children + i + 1, (ptr->childCount-1-i) * sizeof(HWND));
                    }
                    ptr->childCount--;
                    if (ptr->childCount == 1)
                    {
                        ShowWindow(ptr->tabCtl, SW_HIDE);
                    }
                    if (ptr->childCount)
                        SendMessage(hwnd, TW_SELECT, 0, (LPARAM)ptr->children[ptr->activeIndex] );
                    break;
                }
            break;
        case TW_CLOSE:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLORSCROLLBAR:
        case WM_CTLCOLORSTATIC:
            ptr = (TW_Params*)GetWindowLong(hwnd, 0);
            if (ptr->childCount)
                return SendMessage(ptr->children[ptr->activeIndex], iMessage, wParam, lParam);
            break;
        case WM_SIZE:
            ptr = (TW_Params*)GetWindowLong(hwnd, 0);
            r.left =0; r.right = LOWORD(lParam);
            r.top = 0; r.bottom = HIWORD(lParam);
            if (ptr->childCount > 1)
            {
                r.bottom -= 25;
            }
            if (ptr->childCount)
                MoveWindow(ptr->children[ptr->activeIndex], r.left, r.top, r.right - r.left, r.bottom - r.top, 1);
            return 0;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}

//-------------------------------------------------------------------------

void RegisterControlWindow2(HINSTANCE hInstance)
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


}

//-------------------------------------------------------------------------

TW_Params *CreateControlWindow2(HWND parent)
{
    TW_Params *p = calloc(sizeof(TW_Params), 1);
    if (p)
    {
        p->hwnd = CreateWindowEx(WS_EX_TOOLWINDOW, szControlWindClassName, 0, WS_CLIPSIBLINGS |
            WS_CLIPCHILDREN | WS_CHILD, 0, 0, 10, 10, parent, 0, 
            (HINSTANCE)GetWindowLong(parent, GWL_HINSTANCE), p);
    }
    return p;
}
    