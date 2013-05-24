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
#include "header.h" 



#define TRANSPARENT_COLOR 0x872395
#define BOUNDING_COLOR 0x872497
typedef struct _ttip
{
    char text[256];
    BOOL enabled;
    BOOL showing;
    POINT mouse;
    DWORD initDelay;
    DWORD cancelDelay;
    HWND parent;
    HFONT font;
} TTIP;

struct _hook
{
    struct _hook *next;
    HWND hwndTT, parent;
} ;
#define IDT_START 1000
#define IDT_HIDE 1001

static char *szTTIPClassName = "xccTTIPWindow";
static HANDLE sem;
static struct _hook *hooks;

static void EnterTrack(HWND hwndTT, HWND parent)
{
    struct _hook *newHook = malloc(sizeof(struct _hook));
    if (newHook)
    {
        newHook->hwndTT = hwndTT;
        newHook->parent = parent;
        WaitForSingleObject(sem, INFINITE);
        newHook->next = hooks;
        hooks = newHook;
        SetEvent(sem);
    }
}
static void LeaveTrack(HWND hwndTT)
{
    struct _hook **oldhook = &hooks;
    WaitForSingleObject(sem, INFINITE);
    while (*oldhook)
    {
        if ((*oldhook)->hwndTT == hwndTT)
        {
            struct _hook *p = *oldhook;
            *oldhook = (*oldhook)->next;
            free(p);
            break;
        }
        oldhook = &(*oldhook)->next;
    }
    SetEvent(sem);
}
void HookMouseMovement(MSG *msg)
{
    if (msg->message == WM_MOUSEMOVE)
    {
        static POINT pt;
        static HWND tell = NULL, oldTell = NULL;
        POINT pt1;
        GetCursorPos(&pt1);
        if (pt.x != pt1.x || pt.y != pt1.y)
        {
            struct _hook *list;
            pt = pt1;
            WaitForSingleObject(sem, INFINITE);
            list = hooks;
            while (list)
            {
                if (list->parent == msg->hwnd)
                {
                    tell = list->hwndTT;
                    break;
                }
                list = list->next;
            }
            SetEvent(sem);	
            if (oldTell && tell != oldTell)
            {
                PostMessage(oldTell, WM_MOUSEINPARENT, 0,0);
                oldTell = 0;
            }
            if (tell)
            {
                PostMessage(tell, WM_MOUSEINPARENT, 1, 0);
                oldTell = tell;
            }
        }
    }
}

static void DrawItem(TTIP *ptr, HWND hwnd, HDC dc, RECT *r)
{
    HPEN pen;
    HFONT font;
    int oldbk, oldfg;
    HBRUSH brush = CreateSolidBrush(TRANSPARENT_COLOR);
    FillRect(dc, r, brush);
    DeleteObject(brush);
    
    brush = CreateSolidBrush(RetrieveSysColor(COLOR_INFOBK));
    brush = SelectObject(dc, brush);	
    pen = CreatePen(PS_SOLID, 0, RetrieveSysColor(COLOR_INFOTEXT));
    pen = SelectObject(dc, pen); 
    RoundRect(dc, r->left, r->top, r->right, r->bottom, 5, 5);
    brush = SelectObject(dc, brush);	
    pen = SelectObject(dc, pen); 
    DeleteObject(pen);
    DeleteObject(brush);

    oldbk = SetBkColor(dc, RetrieveSysColor(COLOR_INFOBK));
    oldfg = SetTextColor(dc, RetrieveSysColor(COLOR_INFOTEXT));
    font = SelectObject(dc, (HFONT)SendMessage(hwnd, WM_GETFONT, 0, 0));
    DrawText(dc, ptr->text, strlen(ptr->text), r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(dc, font);
    SetTextColor(dc, oldfg);
    SetBkColor(dc, oldbk);
}
static void CalculateWindowPos(HWND hwnd, TTIP *ptr)
{
    RECT r;
    POINT size, mouse;
    int sx = GetSystemMetrics(SM_CXSCREEN);
    int sy = GetSystemMetrics(SM_CYSCREEN);
    HDC dc = GetDC(hwnd);
    HFONT font = SelectObject(dc, (HFONT)SendMessage(hwnd, WM_GETFONT, 0, 0));
    mouse = ptr->mouse;
    GetTextExtentPoint32(dc, ptr->text, strlen(ptr->text), &size);
    SelectObject(dc, font);
    ReleaseDC(hwnd, dc);
    size.x += 5;
    size.y += 5;
    
    r.top = mouse.y + 10;
    r.left = mouse.x  + 10;
    r.right = r.left + size.x;
    r.bottom = r.top + size.y;
    if (r.right > sx)
    {
        r.left -= r.right - sx;
        r.right = sx;
    }
    if (r.left < 0)
        r.left = 0;
    MoveWindow(hwnd, r.left, r.top, r.right - r.left ,r.bottom - r.top, 0);	
}
static LRESULT CALLBACK TTIPWndProc(HWND hwnd, UINT iMessage,
    WPARAM wParam, LPARAM lParam)
{
    HDC dc;
    RECT r;
    PAINTSTRUCT ps;
    TTIP *ptr;
    TRACKMOUSEEVENT tracker;
    TOOLTIPTEXT notify;
    POINT mouse;
    LRESULT rv;
    switch(iMessage)
    {
        case WM_PAINT:
            ptr = (TTIP *)GetWindowLong(hwnd, 0);
            dc = BeginPaint(hwnd, &ps);
            GetClientRect(hwnd, &r);
            DrawItem(ptr, hwnd, dc, &r);
            EndPaint(hwnd, &ps);
            return 0;
        case WM_MOUSEINPARENT:			
            ptr = (TTIP *)GetWindowLong(hwnd, 0);
            KillTimer(hwnd, IDT_START);
            if (IsWindowVisible(hwnd))
            {
                KillTimer(hwnd, IDT_HIDE);
                SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOMOVE | SWP_HIDEWINDOW);
                ShowWindow(hwnd, SW_HIDE);
            }
            if (wParam)
            {
                SetTimer(hwnd, IDT_START, ptr->initDelay, NULL);
            }
            break;
        case WM_TIMER:
            ptr = (TTIP *)GetWindowLong(hwnd, 0);
            switch (wParam)
            {
                case IDT_START:
                    KillTimer(hwnd, IDT_START);
                    memset(&notify, 0, sizeof(notify));
                    notify.hdr.code = TTN_NEEDTEXT;
                    notify.hdr.hwndFrom = hwnd;
                    notify.lpszText = ptr->text;
                    ptr->text[0] = 0;
                    SendMessage(ptr->parent, WM_NOTIFY, 0, (LPARAM)&notify);
                    if (ptr->text[0])
                    {
                        GetCursorPos(&ptr->mouse);
                        CalculateWindowPos(hwnd, ptr);
                        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOMOVE | SWP_SHOWWINDOW);
                        SetTimer(hwnd, IDT_HIDE, ptr->cancelDelay, 0);
                    }
                    break;
                case IDT_HIDE:
                    KillTimer(hwnd, IDT_HIDE);
                    ShowWindow(hwnd, SW_HIDE);
                    break;
            }
            break;
        case WM_ERASEBKGND:
            return 1;
        case WM_CREATE:
            ptr = (TTIP *)calloc(1, sizeof(TTIP));
            if (!ptr)
                return 1;
            SetWindowLong(hwnd, 0, (DWORD)ptr);
            rv = DefWindowProc(hwnd, iMessage, wParam, lParam);
            if (rv)
                return rv;
            ptr->enabled = TRUE;
            ptr->initDelay = 400;
            ptr->cancelDelay = 5000;
            ptr->parent = ((LPCREATESTRUCT)lParam)->hwndParent;
            EnterTrack(hwnd, ptr->parent);
            SetLayeredWindowAttributes(hwnd, TRANSPARENT_COLOR, 0xff, LWA_COLORKEY);
            break;
        case WM_GETFONT:
            ptr = (TTIP *)GetWindowLong(hwnd, 0);
            if (ptr->font != NULL)
                return (LRESULT)ptr->font;
            break;
        case WM_SETFONT:
            ptr = (TTIP *)calloc(1, sizeof(TTIP));
            ptr->font = (HFONT)wParam;
            break;			
        case WM_DESTROY:
            ptr = (TTIP *)GetWindowLong(hwnd, 0);
            LeaveTrack(hwnd);
            free(ptr);
            break;
                    
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
void RegisterTTIPWindow(HINSTANCE hInstance)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = 0;
    wc.lpfnWndProc = &TTIPWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(LPVOID);
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = 0; // use parent cursor LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = 0;
    wc.lpszMenuName = 0;
    wc.lpszClassName = szTTIPClassName;
    RegisterClass(&wc);
    sem = CreateEvent(0,0,1,0);
}
HWND CreateTTIPWindow(HWND hwndParent, int style)
{
    HWND rv = CreateWindowEx(WS_EX_LAYERED | WS_EX_NOACTIVATE, 
                              szTTIPClassName, "", WS_CLIPSIBLINGS | WS_POPUP | style,
                              0,0,10,10, hwndParent, 0, (HINSTANCE)GetWindowLong(hwndParent, GWL_HINSTANCE), 0);
    SetWindowPos(rv, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOMOVE);
    return rv;
}
