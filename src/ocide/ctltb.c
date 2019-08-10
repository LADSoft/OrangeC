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
#include <richedit.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "header.h"
#include "idewinconst.h"

extern HWND hwndFrame;

static WNDPROC oldproc;
static char* szToolBarWindClassName = "ctlToolBar";
static char* szToolBarInternalClassName = "ctlToolBarInternal";
static char* szextToolBarWindClassName = "ctlToolBarExt";

static TBBUTTON sep = {0, 0, TBSTATE_WRAP, TBSTYLE_SEP | TBSTYLE_FLAT, {0}, 0, -1};
void GradientFillTB(HDC dc, RECT* r)
{
    TRIVERTEX vertex[2];
    GRADIENT_RECT gRect;
    vertex[0].x = r->left;
    vertex[0].y = r->top;
    vertex[0].Red = (RetrieveSysColor(COLOR_BTNHIGHLIGHT) & 0xff) << 8;
    vertex[0].Green = RetrieveSysColor(COLOR_BTNHIGHLIGHT) & 0xff00;
    vertex[0].Blue = (RetrieveSysColor(COLOR_BTNHIGHLIGHT) & 0xff0000) >> 8;
    vertex[0].Alpha = 0x0000;

    vertex[1].x = r->right;
    vertex[1].y = r->bottom;
    vertex[1].Red = (RetrieveSysColor(COLOR_BTNSHADOW) & 0xff) << 8;
    vertex[1].Green = RetrieveSysColor(COLOR_BTNSHADOW) & 0xff00;
    vertex[1].Blue = (RetrieveSysColor(COLOR_BTNSHADOW) & 0xff0000) >> 8;
    vertex[1].Alpha = 0x0000;

    gRect.UpperLeft = 0;
    gRect.LowerRight = 1;

    GradientFill(dc, vertex, 2, &gRect, 1, GRADIENT_FILL_RECT_V);
}
static void DrawTitle(HDC dc, RECT* r)
{
    HPEN pen1, pen2, pen3, oldpen;

    pen1 = CreatePen(PS_SOLID, 1, RetrieveSysColor(COLOR_3DLIGHT));
    pen2 = CreatePen(PS_SOLID, 2, RetrieveSysColor(COLOR_3DHILIGHT));
    pen3 = CreatePen(PS_SOLID, 1, RetrieveSysColor(COLOR_3DSHADOW));
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
    SelectObject(dc, oldpen);
    DeleteObject(pen1);
    DeleteObject(pen2);
    DeleteObject(pen3);
}
static char* GetTipText(TBData* ptr, int id)
{
    int i;
    for (i = 0; i < ptr->btncount; i++)
        if (ptr->buttons[i].idCommand == id)
        {
            return ptr->hints[i];
        }
    return "";
}

//-------------------------------------------------------------------------

static void FormatToolBar(char* buf, HWND hwnd)
{
    TBBUTTON button;
    char* p = buf;
    int i = 0;
    buf[0] = 0;
    while (SendMessage(hwnd, TB_GETBUTTON, i++, (LPARAM)&button))
    {
        if (button.fsStyle == BTNS_SEP)
        {
            sprintf(p, "%d ", -button.idCommand * 1000 - button.iBitmap);
        }
        else
        {
            sprintf(p, "%d ", button.idCommand);
        }
        p += strlen(p);
    }
}

//-------------------------------------------------------------------------

static int ParseToolBar(char* buf, HWND hwnd, TBData* ptr, TBBUTTON* buttons)
{
    int rv = 0, i, id, count;
    char* p = buf;
    while (*p)
    {
        if (!isdigit(*p) && *p != '-')
            break;
        sscanf(p, "%d %n", &id, &count);
        p += count;
        if (id < 0)
        {
            buttons[rv] = sep;
            if (hwnd == ptr->hWnd)
                buttons[rv].fsState &= ~TBSTATE_WRAP;
            buttons[rv].idCommand = -id / 1000;
            id = -(id) % 1000;
            buttons[rv++].iBitmap = id;
        }
        else
        {
            for (i = 0; i < ptr->btncount; i++)
                if (ptr->buttons[i].idCommand == id)
                {
                    int state;
                    if (!IsWindow(hwnd))
                        state = FALSE;
                    else
                        state = SendMessage(hwnd, TB_ISBUTTONENABLED, id, 0) ? TBSTATE_ENABLED : 0;
                    buttons[rv] = ptr->buttons[i];
                    buttons[rv].fsState = state;
                    if (hwnd == ptr->hWnd)
                        buttons[rv].fsState &= ~TBSTATE_WRAP;

                    rv++;
                }
        }
        while (isspace(*p))
            p++;
    }
    return rv;
}

//-------------------------------------------------------------------------

static void SetRectSize(HWND hwnd, TBData* ptr)
{
    RECT r;
    TBBUTTON buttons[60];
    int num = 0;
    int i;
    BOOL found = FALSE;
    while (SendMessage(ptr->hWnd, TB_GETBUTTON, num, (LPARAM)&buttons[num]))
    {
        if (!(buttons[num].fsStyle & TBSTYLE_SEP))
            found = TRUE;
        num++;
    }
    MoveWindow(ptr->hWnd, 0, 0, 1000, 30, 0);
    SendMessage(ptr->hWnd, TB_SETROWS, MAKEWPARAM(1, FALSE), (LPARAM)&r);
    SendMessage(ptr->hWnd, TB_GETITEMRECT, num - 1, (LPARAM)&r);
    ptr->pos.right = r.right + GRIPWIDTH;
    ptr->pos.bottom = r.bottom + GetSystemMetrics(SM_CYBORDER) * 2;
    MoveWindow(ptr->grip, 0, 0, ptr->pos.right, ptr->pos.bottom, 0);
    for (i = 0; i < ptr->btncount; i++)
        if (ptr->children[i])
        {
            SetWindowPos(ptr->children[i], 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_HIDEWINDOW);
        }
    for (i = 0; i < num; i++)
        if ((buttons[i].fsStyle & TBSTYLE_SEP) && buttons[i].idCommand)
        {
            int j;
            for (j = 0; j < ptr->btncount; j++)
            {

                if (buttons[i].idCommand == ptr->buttons[j].idCommand)
                {
                    if (ptr->children[j])
                    {
                        RECT r;
                        SendMessage(ptr->hWnd, TB_GETITEMRECT, i, (LPARAM)&r);
                        SetWindowPos(ptr->children[j], HWND_TOP, r.left, r.top, r.right - r.left - 3, r.bottom - r.top,
                                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
                    }
                }
            }
        }
}

//-------------------------------------------------------------------------

static void ChangeButtons(int num, HWND hwnd, TBBUTTON* buttons, TBData* ptr)
{
    if (num)
    {
        while (SendMessage(ptr->hWnd, TB_DELETEBUTTON, 0, 0))
            ;
        SendMessage(ptr->hWnd, TB_ADDBUTTONS, num, (LPARAM)buttons);
        SetRectSize(hwnd, ptr);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

int GetToolBarData(HWND hwnd, char* define)
{
    TBData* ptr = (TBData*)GetWindowLong(hwnd, 0);
    FormatToolBar(define, ptr->hWnd);
    return ptr->id;
}

//-------------------------------------------------------------------------

void SetToolBarData(HWND hwnd, char* define)
{
    TBBUTTON buttons[60];
    TBData* ptr = (TBData*)GetWindowLong(hwnd, 0);
    //    RECT c;
    int i;
    //	ShowWindow(hwnd, SW_HIDE);
    //    GetClientRect(ptr->hWnd, &c);
    //    MoveWindow(ptr->hWnd, GRIPWIDTH, 0, 1000, 1000, 0);

    i = ParseToolBar(define, ptr->hWnd, ptr, buttons);
    ChangeButtons(i, hwnd, buttons, ptr);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
static LRESULT CALLBACK extToolBarWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{

    switch (iMessage)
    {
        case WM_PAINT:
        {
            RECT rect;
            HDC hDC, hdouble;
            HBITMAP bitmap;
            PAINTSTRUCT ps;
            GetClientRect(hwnd, &rect);
            hDC = BeginPaint(hwnd, &ps);
            hdouble = CreateCompatibleDC(hDC);
            bitmap = CreateCompatibleBitmap(hDC, rect.right, rect.bottom);
            SelectObject(hdouble, bitmap);
            SendMessage(hwnd, WM_PRINTCLIENT, (WPARAM)hdouble, 0);
            BitBlt(hDC, 0, 0, rect.right, rect.bottom, hdouble, 0, 0, SRCCOPY);
            DeleteObject(bitmap);
            DeleteObject(hdouble);
            EndPaint(hwnd, &ps);
        }
            return 0;
    }
    return CallWindowProc(oldproc, hwnd, iMessage, wParam, lParam);
}

static LRESULT CALLBACK tbInternalWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    TBData* ptr;
    LPTOOLTIPTEXT lpt;
    switch (iMessage)
    {
        case WM_CREATE:
            ptr = (TBData*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
            SetWindowLong(hwnd, 0, (DWORD)ptr);
            ptr->hWnd =
                CreateWindowEx(0, szextToolBarWindClassName, NULL,
                               WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | TBSTYLE_ALTDRAG | (ptr->hints ? TBSTYLE_TOOLTIPS : 0) |
                                   TBSTYLE_FLAT | TBSTYLE_TRANSPARENT | CCS_ADJUSTABLE | CCS_NODIVIDER,

                               0, 0, 0, 0, hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            SendMessage(ptr->hWnd, TB_SETIMAGELIST, (WPARAM)ptr->bmpid, (LPARAM)ptr->imageList);
            SendMessage(ptr->hWnd, TB_SETDISABLEDIMAGELIST, (WPARAM)ptr->bmpid, (LPARAM)ptr->disabledImageList);
            SendMessage(ptr->hWnd, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
            SendMessage(ptr->hWnd, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);
            ChangeButtons(ptr->btncount, ptr->grip, ptr->buttons, ptr);
            break;
        case WM_NOTIFY:
            if (((LPNMHDR)lParam)->code >= TBN_LAST && ((LPNMHDR)lParam)->code <= TBN_FIRST)
                return SendMessage(GetParent(hwnd), iMessage, wParam, lParam);
            switch (((NMHDR*)lParam)->code)
            {
                char* p;
                case TTN_NEEDTEXT:
                    ptr = (TBData*)GetWindowLong(hwnd, 0);
                    lpt = (LPTOOLTIPTEXT)lParam;
                    p = GetTipText(ptr, lpt->hdr.idFrom);
                    if (p)
                    {
                        strcpy(lpt->szText, p);
                    }
                    break;
            }
            break;
        case WM_COMMAND:
            return SendMessage(GetParent(hwnd), iMessage, wParam, lParam);

        case WM_ERASEBKGND:
            // the check is to make sure we are being called from the toolbar
            // if we don't put it in we get flickering when the window gets redraw
            // independently of the toolbar
            if (GetObjectType((HGDIOBJ)wParam) == OBJ_MEMDC)
            {
                ptr = (TBData*)GetWindowLong(hwnd, 0);
                {
                    HBRUSH hbr = RetrieveSysBrush(COLOR_BTNFACE);
                    HRGN rgn;
                    RECT r;
                    GetClientRect(hwnd, &r);
                    FillRect((HDC)wParam, &r, hbr);
                    DeleteObject(hbr);
                    if (!ptr->nonfloating)
                    {
                        // in toolbar, make a nice gradient and round the right-hand side corners
                        rgn = CreateRoundRectRgn(r.left - 15, r.top, r.right + 1, r.bottom + 1, 5, 5);
                        SelectClipRgn((HDC)wParam, rgn);
                        GradientFillTB((HDC)wParam, &r);
                        DeleteObject(rgn);
                    }
                }
            }
            return 1;
        case WM_SIZE:
            ptr = (TBData*)GetWindowLong(hwnd, 0);
            MoveWindow(ptr->hWnd, 0, 0, LOWORD(lParam), HIWORD(lParam), 1);
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
static LRESULT CALLBACK tbWindWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    static RECT focusPos, captureArea, originalPos;
    static BOOL captured;
    HBITMAP bmp;
    RECT r;
    PAINTSTRUCT ps;
    HDC dc;
    TBData* ptr;
    LPNMTOOLBAR lpnmtoolbar;
    int i, j, n;
    if (iMessage >= WM_USER && iMessage < WM_USER + 200)
    {
        ptr = (TBData*)GetWindowLong(hwnd, 0);
        if (ptr)
            return SendMessage(ptr->hWnd, iMessage, wParam, lParam);
    }
    switch (iMessage)
    {
        case WM_NOTIFY:
            switch (((LPNMHDR)lParam)->code)
            {
                case TBN_BEGINADJUST:
                    break;
                case TBN_QUERYINSERT:
                    return TRUE;
                case TBN_QUERYDELETE:
                    lpnmtoolbar = (LPNMTOOLBAR)lParam;
                    return lpnmtoolbar->tbButton.idCommand != ID_TOOLBARCUSTOM;
                case TBN_GETBUTTONINFO:
                    ptr = (TBData*)GetWindowLong(hwnd, 0);
                    lpnmtoolbar = (LPNMTOOLBAR)lParam;
                    if (lpnmtoolbar->iItem < ptr->btncount)
                    {
                        lpnmtoolbar->tbButton = ptr->buttons[lpnmtoolbar->iItem];
                        lpnmtoolbar->tbButton.iString = (int)ptr->whints[lpnmtoolbar->iItem];
                        return TRUE;
                    }
                    else
                    {
                        return FALSE;
                    }
                    return FALSE;
                case TBN_BEGINDRAG:
                    break;
                case TBN_ENDDRAG:
                    break;
                case TBN_RESET:
                    ptr = (TBData*)GetWindowLong(hwnd, 0);
                    ChangeButtons(ptr->btncount, hwnd, ptr->buttons, ptr);
                    PostMessage(GetParent(hwnd), WM_REDRAWTOOLBAR, 0, 0);

                    break;
                case TBN_CUSTHELP:
                    ptr = (TBData*)GetWindowLong(hwnd, 0);
                    if (ptr->helpItem)
                        ContextHelp(ptr->helpItem);
                    break;
                    //            case TBN_DRAGOVER:
                    //                return FALSE;
                case TBN_ENDADJUST:
                case TBN_TOOLBARCHANGE:
                    ptr = (TBData*)GetWindowLong(hwnd, 0);
                    SetRectSize(hwnd, ptr);
                    PostMessage(GetParent(hwnd), WM_REDRAWTOOLBAR, 0, 0);
                    SavePreferences();
                    break;
                case TBN_DROPDOWN:
                    ptr = (TBData*)GetWindowLong(hwnd, 0);
                    lpnmtoolbar = (LPNMTOOLBAR)lParam;
                    return SendMessage(ptr->notifyParent, WM_TOOLBARDROPDOWN, lpnmtoolbar->iItem,
                                       MAKELPARAM(lpnmtoolbar->rcButton.left, lpnmtoolbar->rcButton.bottom))
                               ? TBDDRET_DEFAULT
                               : TBDDRET_NODEFAULT;
                    //            default:
                    //                return SendMessage(ptr->t, iMessage, wParam,
                    //                    lParam);
            }
            break;
        case WM_LBUTTONDOWN:
            if (LOWORD(lParam) < GRIPWIDTH)
            {
                captured = TRUE;
                SetCapture(hwnd);
                GetWindowRect(GetParent(hwnd), &captureArea);
                GetWindowRect(hwnd, &focusPos);
                GetWindowRect(hwnd, &originalPos);
                captureArea.bottom += originalPos.bottom - originalPos.top;
                DrawBoundingRect(&focusPos);
            }
            break;
        case WM_LBUTTONUP:
            if (captured)
            {
                POINT pt;
                HideBoundingRect();
                ptr = (TBData*)GetWindowLong(hwnd, 0);
                ReleaseCapture();
                captured = FALSE;
                pt.x = LOWORD(lParam);
                pt.y = HIWORD(lParam);
                ClientToScreen(hwnd, &pt);
                PostMessage(GetParent(hwnd), WM_REDRAWTOOLBAR, ptr->id, MAKELPARAM(pt.x, pt.y));
            }
            break;
        case WM_MOUSEMOVE:
            if (captured)
            {
                int newX = (int)(short)LOWORD(lParam);
                int newY = (int)(short)HIWORD(lParam);
                int n = focusPos.bottom - focusPos.top + GetSystemMetrics(SM_CYBORDER) * 2;
                newX += originalPos.left;
                newY += originalPos.top;
                newY = ((newY - originalPos.top) / n) * n + originalPos.top;
                if ((int)(short)HIWORD(lParam) < 0)
                {
                    newY -= n;
                }

                if (newX < captureArea.left)
                    newX = captureArea.left;
                if (newY < captureArea.top)
                    newY = captureArea.top;
                if (newX < captureArea.right && newY < captureArea.bottom)
                {

                    focusPos.right = focusPos.right - focusPos.left + newX;
                    focusPos.left = newX;
                    focusPos.bottom = focusPos.bottom - focusPos.top + newY;
                    focusPos.top = newY;
                    DrawBoundingRect(&focusPos);
                }
            }
            break;
        case WM_SHOWWINDOW:
            PostMessage(GetParent(hwnd), WM_REDRAWTOOLBAR, 0, 0);
            break;
        case WM_ERASEBKGND:
            ptr = (TBData*)GetWindowLong(hwnd, 0);
            {
                HBRUSH hbr = RetrieveSysBrush(COLOR_BTNFACE);
                HRGN rgn;
                RECT r;
                GetClientRect(hwnd, &r);
                FillRect((HDC)wParam, &r, hbr);
                DeleteObject(hbr);
                ptr = (TBData*)GetWindowLong(hwnd, 0);
                if (!ptr->nonfloating)
                {
                    // in toolbar, make a nice gradient and round the right-hand side corners
                    rgn = CreateRoundRectRgn(r.left + 2, r.top, r.right + 1, r.bottom + 1, 5, 5);
                    SelectClipRgn((HDC)wParam, rgn);
                    GradientFillTB((HDC)wParam, &r);
                    DeleteObject(rgn);
                }
            }
            return 1;
        case WM_PAINT:
            ptr = (TBData*)GetWindowLong(hwnd, 0);
            dc = BeginPaint(hwnd, &ps);
            GetClientRect(hwnd, &r);
            if (!ptr->nonfloating)
                DrawTitle(dc, &r);
            EndPaint(hwnd, &ps);
            return 0;
        case WM_COMMAND:
            ptr = (TBData*)GetWindowLong(hwnd, 0);

            if (wParam == ID_TOOLBARCUSTOM)
            {
                PostMessage(ptr->hWnd, TB_CUSTOMIZE, 0, 0);
            }
            else
            {
                PostMessage(ptr->notifyParent, iMessage, wParam, lParam);
            }
            return 0;
        case WM_CREATE:
            ptr = (TBData*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
            SetWindowLong(hwnd, 0, (DWORD)ptr);
            ptr->grip = hwnd;
            ptr->btncount = 0;
            n = 10000;
            while (ptr->buttons[ptr->btncount].fsStyle || ptr->buttons[ptr->btncount].idCommand)
            {
                ptr->btncount++;
            }

            ptr->whints = calloc(ptr->btncount, sizeof(WCHAR*));
            ptr->children = calloc(ptr->btncount, sizeof(HWND));
            for (i = 0; i < ptr->btncount; i++)
            {
                n = strlen(ptr->hints[i]) + 1;
                ptr->whints[i] = calloc(sizeof(WCHAR), n);
                for (j = 0; j < n; j++)
                    ptr->whints[i][j] = ptr->hints[i][j];
            }
            ptr->children = calloc(ptr->btncount, sizeof(HWND));
            ptr->imageList = ImageList_Create(16, 16, ILC_COLOR32, ptr->bmpcount + 1, ptr->bmpcount + 1);
            ptr->disabledImageList = ImageList_Create(16, 16, ILC_COLOR32, ptr->bmpcount + 1, ptr->bmpcount + 1);

            bmp = LoadBitmap((HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), MAKEINTRESOURCE(ptr->bmpid));
            bmp = ConvertToTransparent(bmp, 0xc0c0c0);
            ImageList_Add(ptr->imageList, bmp, NULL);
            //			ConvertToGray(bmp);
            Tint(bmp, RetrieveSysColor(COLOR_BTNSHADOW));
            ImageList_Add(ptr->disabledImageList, bmp, NULL);
            DeleteObject(bmp);

            ptr->blank = CreateWindowEx(0, szToolBarInternalClassName, "", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, NULL,
                                        (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), ptr);
            break;
        case WM_DESTROY:
            ptr = (TBData*)GetWindowLong(hwnd, 0);
            for (i = 0; i < ptr->btncount; i++)
            {
                free(ptr->whints[i]);
            }
            free(ptr->whints);
            free(ptr->children);
            ImageList_Destroy(ptr->imageList);
            ImageList_Destroy(ptr->disabledImageList);
            free(ptr);
            break;
        case LCF_ADDCONTROL:
        {
            int i;
            ptr = (TBData*)GetWindowLong(hwnd, 0);

            for (i = 0; i < ptr->btncount; i++)
            {
                if (ptr->buttons[i].fsStyle & BTNS_SEP)
                {
                    if (ptr->buttons[i].idCommand)
                        if (!wParam--)
                            break;
                }
            }
            if (i < ptr->btncount)
            {
                RECT r;
                HWND hwndCtrl = (HWND)lParam;
                SendMessage(ptr->hWnd, TB_GETITEMRECT, i, (LPARAM)&r);
                SetParent(hwndCtrl, ptr->hWnd);
                SetWindowPos(hwndCtrl, HWND_TOP, r.left, r.top, r.right - r.left - 3, r.bottom - r.top,
                             SWP_NOACTIVATE | SWP_SHOWWINDOW);
                ptr->children[i] = hwndCtrl;
            }
        }
            return 0;
        case LCF_FLOATINGTOOL:
            ptr = (TBData*)GetWindowLong(hwnd, 0);
            ptr->nonfloating = !wParam;
            InvalidateRect(hwnd, 0, 0);
            return 0;
        case WM_CLOSE:
            ptr = (TBData*)GetWindowLong(hwnd, 0);
            return 0;
        case WM_SIZE:
            ptr = (TBData*)GetWindowLong(hwnd, 0);
            MoveWindow(ptr->blank, GRIPWIDTH, 0, ptr->pos.right - GRIPWIDTH, ptr->pos.bottom, 1);
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}

//-------------------------------------------------------------------------

void RegisterToolBarWindow(HINSTANCE hInstance)
{
    WNDCLASS wc;
    GetClassInfo(0, TOOLBARCLASSNAME, &wc);
    oldproc = wc.lpfnWndProc;
    wc.lpfnWndProc = &extToolBarWndProc;
    wc.lpszClassName = szextToolBarWindClassName;
    wc.hInstance = hInstance;
    RegisterClass(&wc);

    memset(&wc, 0, sizeof(wc));
    wc.style = 0;
    wc.lpfnWndProc = &tbWindWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(LPVOID);
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = RetrieveSysBrush(COLOR_BTNFACE);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szToolBarWindClassName;
    RegisterClass(&wc);
    wc.style = 0;
    wc.lpfnWndProc = &tbInternalWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(LPVOID);
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = RetrieveSysBrush(COLOR_BTNFACE);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szToolBarInternalClassName;
    RegisterClass(&wc);
}

//-------------------------------------------------------------------------

HWND CreateToolBarWindow(int id, HWND notify, HWND parent, int bmp, int bmpcount, TBBUTTON* buttons, char** hints, char* title,
                         int helpItem, BOOL visible)
{
    TBData* p = calloc(sizeof(TBData), 1);
    if (p)
    {
        p->id = id;
        p->buttons = buttons;
        p->hints = hints;
        p->bmpid = bmp;
        p->bmpcount = bmpcount;
        p->notifyParent = notify;
        p->helpItem = helpItem;
        return CreateWindowEx(0, szToolBarWindClassName, title,
                              WS_CLIPSIBLINGS | WS_CHILD | WS_CLIPCHILDREN | (visible ? WS_VISIBLE : 0), CW_USEDEFAULT,
                              CW_USEDEFAULT, 1000, 30, parent, (HMENU)0, (HINSTANCE)GetWindowLong(parent, GWL_HINSTANCE), p);
    }
    return NULL;
}
