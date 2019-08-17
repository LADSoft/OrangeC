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

extern HWND hwndClient, hwndFrame;
extern LOGFONT systemDialogFont;
extern HWND handles[MAX_HANDLES];

static char* szControlWindClassName = "ctlContainerWindow";
static HCURSOR noCur, dragCur;
static HIMAGELIST tagImageList;
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

void GradientFillCaption(HDC dc, RECT* r, BOOL active)
{
    TRIVERTEX vertex[2];
    GRADIENT_RECT gRect;
    vertex[0].x = r->left;
    vertex[0].y = r->top;
    vertex[0].Red = (RetrieveSysColor(active ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION) & 0xff) << 8;
    vertex[0].Green = RetrieveSysColor(active ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION) & 0xff00;
    vertex[0].Blue = (RetrieveSysColor(active ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION) & 0xff0000) >> 8;
    vertex[0].Alpha = 0x0000;

    vertex[1].x = r->right;
    vertex[1].y = r->bottom;
    vertex[1].Red = (RetrieveSysColor(active ? COLOR_GRADIENTACTIVECAPTION : COLOR_GRADIENTINACTIVECAPTION) & 0xff) << 8;
    vertex[1].Green = RetrieveSysColor(active ? COLOR_GRADIENTACTIVECAPTION : COLOR_GRADIENTINACTIVECAPTION) & 0xff00;
    vertex[1].Blue = (RetrieveSysColor(active ? COLOR_GRADIENTACTIVECAPTION : COLOR_GRADIENTINACTIVECAPTION) & 0xff0000) >> 8;
    vertex[1].Alpha = 0x0000;

    gRect.UpperLeft = 0;
    gRect.LowerRight = 1;

    GradientFill(dc, vertex, 2, &gRect, 1, GRADIENT_FILL_RECT_V);
}
static LRESULT CALLBACK ControlWindWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    static BOOL captured;
    static POINT origin;
    RECT r;
    PAINTSTRUCT ps;
    HDC dc;
    TWData* ptr;
    int i;
    LPNMHDR nmh;
    char buf[256];
    switch (iMessage)
    {
        case WM_NCLBUTTONDOWN:

            if (!captured && wParam == HTCAPTION)
            {
                origin.x = LOWORD(lParam);
                origin.y = HIWORD(lParam);
                MapWindowPoints(HWND_DESKTOP, hwnd, &origin, 1);
                captured = TRUE;
                SetCapture(hwnd);
                return 0;
            }
            break;
        case WM_LBUTTONUP:
            if (captured)
            {
                captured = FALSE;
                DockEndDrag(hwnd, origin.x, origin.y, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
                ReleaseCapture();
            }
            return 0;
        case WM_MOUSEMOVE:
            if (captured)
                DockDrag(hwnd, origin.x, origin.y, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
            break;
        case WM_NOTIFY:
            nmh = (LPNMHDR)lParam;
            if (nmh->code == TABN_SELECTED)
            {
                LSTABNOTIFY* p = (LSTABNOTIFY*)nmh;
                SendMessage(hwnd, TW_SELECT, 0, p->lParam);

                return 0;
            }
            else if (nmh->code == TABN_STARTDRAG)
            {
                POINT pt;
                RECT r;
                LSTABNOTIFY* p = (LSTABNOTIFY*)nmh;
                GetWindowRect(hwnd, &r);
                GetCursorPos(&pt);
                if (!PtInRect(&r, pt))
                {
                    HWND newWnd = CreateControlWindow(hwndFrame);

                    SetCursor(LoadCursor(0, IDC_ARROW));
                    ReleaseCapture();
                    MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), &pt, 1);
                    SetWindowPos(newWnd, HWND_TOP, pt.x - 100, pt.y - GetSystemMetrics(SM_CYCAPTION) / 2, 200, 400, SWP_SHOWWINDOW);
                    SendMessage(hwnd, TW_REMOVE, 0, p->lParam);
                    SendMessage(newWnd, TW_ADD, 0, p->lParam);
                    SendMessage(newWnd, TW_CAPTURE, 0, 0);
                    return 1;
                }
                return 0;
            }
            break;
        case WM_SYSCOMMAND:
            if (wParam == SC_CLOSE)
            {
                PostMessage(hwnd, TW_CLOSE, 0, 0);
                return 0;
            }
            break;
        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT:
            ptr = (TWData*)GetWindowLong(hwnd, 0);
            dc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            break;
        case WM_NCPAINT:
            if (GetWindowLong(hwnd, GWL_STYLE) & WS_POPUP)
            {
                break;
            }
            {
                RECT r1;
                char buf[256];
                int rv;
                HDC hdc;
                RECT r;
                HBRUSH brush = GetStockObject(NULL_BRUSH);
                HPEN pen = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));
                HFONT xfont = CreateFontIndirect(&systemDialogFont);
                HRGN rgn;
                GetWindowRect(hwnd, &r);
                r.right -= r.left;
                r.left -= r.left;
                r.bottom -= r.top;
                r.top -= r.top;
                r1 = r;
                r1.left = r1.right - 40;
                r1.top = 2;
                r1.bottom = GetSystemMetrics(SM_CYSMCAPTION) + 2;
                hdc = GetWindowDC(hwnd);
                DPtoLP(hdc, (LPPOINT)&r1, 2);
                rgn = CreateRectRgn(r1.left, r1.top, r1.right, r1.bottom);
                rv = DefWindowProc(hwnd, iMessage, wParam, lParam);
                DeleteObject(rgn);
                //            MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&r, 2);
                //            hdc = GetDCEx(hwnd, (HRGN)wParam, DCX_WINDOW|DCX_INTERSECTRGN);
                brush = SelectObject(hdc, brush);
                pen = SelectObject(hdc, pen);
                Rectangle(hdc, r.left, r.top, r.right, r.bottom);
                r.top += 2;
                r.left += 2;
                r.bottom = r.top + GetSystemMetrics(SM_CYSMCAPTION);
                r.right -= 24;
                GradientFillCaption(hdc, &r, FALSE);
                ptr = (TWData*)GetWindowLong(hwnd, 0);
                if (ptr->children)
                    GetWindowText(ptr->children[ptr->activeIndex], buf, sizeof(buf));
                else
                    buf[0] = 0;
                xfont = SelectObject(hdc, xfont);
                SetBkMode(hdc, TRANSPARENT);
                TextOut(hdc, 8, 6, buf, strlen(buf));
                SetBkMode(hdc, OPAQUE);
                xfont = SelectObject(hdc, xfont);

                brush = SelectObject(hdc, brush);
                pen = SelectObject(hdc, pen);
                DeleteObject(pen);
                DeleteObject(xfont);
                ReleaseDC(hwnd, hdc);
                return rv;
                break;
            }
        case WM_CREATE:
            ptr = (TWData*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
            SetWindowLong(hwnd, 0, (DWORD)ptr);
            ptr->tabCtl = CreateLsTabWindow(hwnd, TABS_BOTTOM | TABS_HOTTRACK | TABS_FLAT | TABS_DRAGABLE);
            SendMessage(ptr->tabCtl, TABM_SETIMAGELIST, 0, (LPARAM)tagImageList);
            ApplyDialogFont(ptr->tabCtl);
            ApplyDialogFont(hwnd);
            return 0;
        case WM_DESTROY:
            ptr = (TWData*)GetWindowLong(hwnd, 0);
            DestroyWindow(ptr->tabCtl);
            free(ptr);
            break;
        case WM_CLOSE:
            ptr = (TWData*)GetWindowLong(hwnd, 0);
            return 0;
        case TW_TRANSFERCHILDREN:
            ptr = (TWData*)GetWindowLong(hwnd, 0);
            for (i = 0; i < ptr->childCount; i++)
            {
                SendMessage((HWND)lParam, TW_ADD, TRUE, (LPARAM)ptr->children[i]);
            }
            ptr->childCount = 0;
            InvalidateRect((HWND)lParam, 0, 0);
            DockRemoveWindow(hwnd);  // undefined in local context
            break;
        case TW_MAKEPOPUP:
            ptr = (TWData*)GetWindowLong(hwnd, 0);
            SetWindowLong(hwnd, GWL_STYLE, (GetWindowLong(hwnd, GWL_STYLE) & ~WS_CHILD) | WS_POPUP | WS_SIZEBOX);
            if (ptr->childCount)
            {
                GetWindowText(ptr->children[ptr->activeIndex], buf, sizeof(buf));
                SetWindowText(hwnd, buf);
            }
            break;
        case TW_ISCONT:
            return TW_ISCONT;
        case TW_SELECT:
            ptr = (TWData*)GetWindowLong(hwnd, 0);
            for (i = 0; i < ptr->childCount; i++)
                if (lParam == (LPARAM)ptr->children[i])
                {
                    if (i == ptr->activeIndex && ptr->childCount != 1)
                        break;
                    ShowWindow(ptr->children[ptr->activeIndex], SW_HIDE);
                    ptr->activeIndex = i;
                    ShowWindow(ptr->children[ptr->activeIndex], SW_SHOW);
                    GetClientRect(hwnd, &r);
                    if (GetWindowLong(hwnd, GWL_STYLE) & WS_POPUP)
                    {
                        GetWindowText(ptr->children[ptr->activeIndex], buf, sizeof(buf));
                        SetWindowText(hwnd, buf);
                    }
                    r.bottom = GetSystemMetrics(SM_CYSMCAPTION);
                    SendMessage(hwnd, WM_NCPAINT, 1, 0);
                    if (wParam)
                        SendMessage(ptr->tabCtl, TABM_SELECT, 0, lParam);
                    GetClientRect(hwnd, &r);
                    if (ptr->childCount > 1)
                        r.bottom -= 26;
                    MoveWindow(ptr->children[ptr->activeIndex], r.left, r.top, r.right - r.left, r.bottom - r.top, 1);
                    break;
                }
            break;
        case TW_ADD:
            ptr = (TWData*)GetWindowLong(hwnd, 0);
            ptr->childCount++;
            ptr->children = realloc(ptr->children, ptr->childCount * sizeof(HWND));
            ptr->children[ptr->childCount - 1] = (HWND)lParam;
            SetParent((HWND)lParam, hwnd);
            GetWindowText((HWND)lParam, buf, sizeof(buf));
            SendMessage(ptr->tabCtl, TABM_INSERT, ptr->childCount - 1, lParam);
            for (i = 0; i < MAX_HANDLES; i++)
            {
                if (handles[i] == (HWND)lParam)
                {
                    SendMessage(ptr->tabCtl, TABM_SETIMAGE, i, lParam);
                    break;
                }
            }
            if (ptr->childCount == 2)
            {
                ShowWindow(ptr->tabCtl, SW_SHOW);
                GetClientRect(hwnd, &r);
                MoveWindow(ptr->tabCtl, r.left, r.bottom - 26, r.right - r.left, 26, 1);
            }
            if (wParam || ptr->childCount == 1)
            {
                SendMessage(hwnd, TW_SELECT, ptr->childCount - 1, lParam);
            }
            else
            {
                SendMessage(ptr->tabCtl, TABM_SELECT, 0, (LPARAM)ptr->children[ptr->activeIndex]);
            }

            break;
        case WM_GETDOCKLIST:
        {
            int* rv = 0;
            ptr = (TWData*)GetWindowLong(hwnd, 0);
            rv = (int*)calloc(sizeof(int), ptr->childCount + 1);
            for (i = 0; i < ptr->childCount; i++)
            {
                int j;
                for (j = 0; j < MAX_HANDLES; j++)
                    if (handles[j] == ptr->children[i])
                    {
                        rv[i] = j;
                    }
            }
            return (LRESULT)rv;
            break;
        }
        case TW_GETACTIVE:
            ptr = (TWData*)GetWindowLong(hwnd, 0);
            return (LRESULT)ptr->children[ptr->activeIndex];
        case TW_REMOVE:
            ptr = (TWData*)GetWindowLong(hwnd, 0);
            SendMessage(ptr->tabCtl, TABM_REMOVE, 0, lParam);
            for (i = 0; i < ptr->childCount; i++)
                if (lParam == (LPARAM)ptr->children[i])
                {
                    ShowWindow((HWND)lParam, SW_HIDE);
                    SetParent((HWND)lParam, hwndFrame);
                    if (ptr->activeIndex && ptr->activeIndex >= ptr->childCount - 1)
                        ptr->activeIndex--;
                    if (i != ptr->childCount - 1)
                    {
                        memcpy(ptr->children + i, ptr->children + i + 1, (ptr->childCount - 1 - i) * sizeof(HWND));
                    }
                    ptr->childCount--;
                    if (ptr->childCount == 1)
                    {
                        ShowWindow(ptr->tabCtl, SW_HIDE);
                    }
                    if (ptr->childCount)
                    {
                        ShowWindow(ptr->children[ptr->activeIndex], SW_SHOW);
                        GetClientRect(hwnd, &r);
                        if (ptr->childCount > 1)
                            r.bottom -= 26;
                        MoveWindow(ptr->children[ptr->activeIndex], r.left, r.top, r.right - r.left, r.bottom - r.top, 1);
                        SendMessage(hwnd, WM_NCPAINT, 0, 0);
                    }
                    else
                    {
                        DockRemoveWindow(hwnd);
                    }
                    break;
                }
            break;
        case TW_HAS:
            ptr = (TWData*)GetWindowLong(hwnd, 0);
            for (i = 0; i < ptr->childCount; i++)
                if (lParam == (LPARAM)ptr->children[i])
                {
                    return 1;
                }
            return 0;
        case TW_CLOSE:
            ptr = (TWData*)GetWindowLong(hwnd, 0);
            if (ptr->childCount)
            {
                SendMessage(hwnd, TW_REMOVE, 0, (LPARAM)ptr->children[ptr->activeIndex]);
            }
            break;
        case TW_CAPTURE:
            if (!captured)
            {
                POINT pt;
                captured = TRUE;

                SetCapture(hwnd);
                GetCursorPos(&pt);
                origin.x = pt.x;
                origin.y = pt.y;
                MapWindowPoints(HWND_DESKTOP, hwnd, &origin, 1);
            }
            break;
        case WM_SHOWWINDOW:
            if (wParam)
            {
                ptr = (TWData*)GetWindowLong(hwnd, 0);
                for (i = 0; i < ptr->childCount; i++)
                {
                    SetParent(ptr->children[i], hwnd);
                    if (i == ptr->activeIndex)
                        ShowWindow(ptr->children[i], SW_SHOW);
                    else
                        ShowWindow(ptr->children[i], SW_HIDE);
                }
                GetClientRect(hwnd, &r);
                if (ptr->childCount > 1)
                {
                    r.bottom -= 21;
                }
                if (ptr->childCount)
                    MoveWindow(ptr->children[ptr->activeIndex], r.left, r.top, r.right - r.left, r.bottom - r.top, 1);
                MoveWindow(ptr->tabCtl, r.left, r.bottom, r.right - r.left, 26, 1);
            }
            break;
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLORSCROLLBAR:
        case WM_CTLCOLORSTATIC:
            ptr = (TWData*)GetWindowLong(hwnd, 0);
            if (ptr->childCount)
                return SendMessage(ptr->children[ptr->activeIndex], iMessage, wParam, lParam);
            break;
        case WM_SIZE:
            ptr = (TWData*)GetWindowLong(hwnd, 0);
            r.left = 0;
            r.right = LOWORD(lParam);
            r.top = 0;
            r.bottom = HIWORD(lParam);
            if (ptr->childCount > 1)
            {
                r.bottom -= 26;
            }
            MoveWindow(ptr->tabCtl, r.left, r.bottom, r.right - r.left, 26, 1);
            if (ptr->childCount)
                MoveWindow(ptr->children[ptr->activeIndex], r.left, r.top, r.right - r.left, r.bottom - r.top, 0);
            return 0;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}

//-------------------------------------------------------------------------

void RegisterControlWindow(HINSTANCE hInstance)
{
    WNDCLASS wc;
    HBITMAP bmp;
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

    dragCur = LoadCursor(hInstance, "ID_DRAGCUR");
    noCur = LoadCursor(hInstance, "ID_NODRAGCUR");

    bmp = LoadBitmap(hInstance, "ID_DID");
    bmp = ConvertToTransparent(bmp, 0xc0c0c0);
    tagImageList = ImageList_Create(16, 16, ILC_COLOR32, 3, 0);
    ImageList_Add(tagImageList, bmp, NULL);
    DeleteObject(bmp);
}

//-------------------------------------------------------------------------

HWND CreateControlWindow(HWND parent)
{
    static int pos = 100;
    TWData* p = calloc(sizeof(TWData), 1);
    if (p)
    {
        p->hwnd = CreateWindowEx(WS_EX_TOOLWINDOW, szControlWindClassName, "",
                                 WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CHILD | WS_CAPTION | WS_SYSMENU, pos, pos, 300, 300, parent,
                                 0, (HINSTANCE)GetWindowLong(parent, GWL_HINSTANCE), p);
        pos += 20;
        return p->hwnd;
    }
    return NULL;
}