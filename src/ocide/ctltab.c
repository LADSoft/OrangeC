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
#include <stdlib.h>
#include "math.h"
#include "header.h"
#include "ctl.h"

#define LEFTHALF 12
#define RIGHTBORDER (LEFTHALF)
#define TOPBORDER 1
#define BOTTOMBORDER 8
#define MINEND 3

#define CLOSEICONWIDTH 10
#define WINDOWICONWIDTH 16
#define WINDOWICONTOP -24
#define WINDOWICONLEFT -19

#define ROUNDWIDTH 3

#define MAX_MENU_ROW 15
#define ID_MENUITEMS (2000)

extern LOGFONT systemDialogFont;

static char* szTabClassName = "ladSoftTabWindow";
static char* szMenuClassName = "ladSoftMenuWindow";
static HBITMAP closeBitmap;
static HCURSOR dragCur, noCur;
static HIMAGELIST mainIml;

struct _singleTab* CreateItem(char* name, LPARAM lParam)
{
    struct _singleTab* newItem = calloc(1, sizeof(struct _singleTab));
    if (newItem)
    {
        newItem->text = strdup(name);
        newItem->iImage = -1;
        if (!newItem->text)
        {
            free(newItem);
            newItem = NULL;
        }
        else
        {
            newItem->lParam = lParam;
        }
    }
    return newItem;
}
void RemoveFromActive(struct _tabStruct* ptr, struct _singleTab* tabs)
{
    if (tabs->chain.prev)
    {
        if (tabs->chain.next)
            tabs->chain.next->chain.prev = tabs->chain.prev;
        else
            ptr->active.tail = tabs->chain.prev;
        tabs->chain.prev->chain.next = tabs->chain.next;
    }
    else if (tabs->chain.next)
    {
        tabs->chain.next->chain.prev = NULL;
        ptr->active.head = tabs->chain.next;
    }
    else
    {
        ptr->active.head = ptr->active.tail = NULL;
    }
}
void RemoveFromSelected(struct _tabStruct* ptr, struct _singleTab* tabs)
{
    if (tabs->selectedChain.prev)
    {
        if (tabs->selectedChain.next)
            tabs->selectedChain.next->selectedChain.prev = tabs->selectedChain.prev;
        else
            ptr->selected.tail = tabs->selectedChain.prev;
        tabs->selectedChain.prev->selectedChain.next = tabs->selectedChain.next;
    }
    else if (tabs->selectedChain.next)
    {
        tabs->selectedChain.next->selectedChain.prev = NULL;
        ptr->selected.head = tabs->selectedChain.next;
    }
    else
    {
        ptr->selected.head = ptr->selected.tail = NULL;
    }
}
void AddToActive(struct _tabStruct* ptr, int pos, struct _singleTab* tabs)
{
    if (ptr->active.head)
    {
        struct _singleTab* prev = NULL;
        struct _singleTab* chk = ptr->active.head;
        while (pos-- && chk)
        {
            prev = chk;
            chk = chk->chain.next;
        }
        tabs->chain.prev = prev;
        tabs->chain.next = chk;
        if (!prev)
        {
            ptr->active.head = tabs;
            chk->chain.prev = tabs;
        }
        else
        {
            if (prev->chain.next)
                prev->chain.next->chain.prev = tabs;
            prev->chain.next = tabs;
        }
        if (!chk)
            ptr->active.tail = tabs;
    }
    else
    {
        tabs->chain.next = NULL;
        tabs->chain.prev = NULL;
        ptr->active.head = ptr->active.tail = tabs;
    }
}
void AddToSelected(struct _tabStruct* ptr, int pos, struct _singleTab* tabs)
{
    if (ptr->selected.head)
    {
        struct _singleTab* prev = NULL;
        struct _singleTab* chk = ptr->selected.head;
        while (pos-- && chk)
        {
            prev = chk;
            chk = chk->selectedChain.next;
        }
        tabs->selectedChain.prev = prev;
        tabs->selectedChain.next = chk;
        if (!prev)
        {
            ptr->selected.head = tabs;
            chk->selectedChain.prev = tabs;
        }
        else
        {
            if (prev->selectedChain.next)
                prev->selectedChain.next->selectedChain.prev = tabs;
            prev->selectedChain.next = tabs;
        }
        if (!chk)
            ptr->selected.tail = tabs;
    }
    else
    {
        tabs->chain.next = NULL;
        tabs->chain.prev = NULL;
        ptr->selected.head = ptr->selected.tail = tabs;
    }
}
void MyDrawEdge(HDC hDc, struct _tabStruct* ptr, int xStart, int yStart, int xEnd, int yEnd, int inc)
{
    HPEN pen = SelectObject(hDc, ptr->greyPen);
    MoveToEx(hDc, xStart, yStart, NULL);
    LineTo(hDc, xEnd, yEnd);
    if (!ptr->flat)
    {
        SelectObject(hDc, pen);
        pen = SelectObject(hDc, ptr->whitePen);
        if (xStart == xEnd)
        {
            MoveToEx(hDc, xStart + inc, yStart, NULL);
            LineTo(hDc, xEnd + inc, yEnd);
        }
        else
        {
            MoveToEx(hDc, xStart, yStart + inc, NULL);
            LineTo(hDc, xEnd, yEnd + inc);
        }
        SelectObject(hDc, pen);
    }
}
void MyDrawArc(HDC hDc, struct _tabStruct* ptr, int x, int y, int inc)
{
    HPEN pen = SelectObject(hDc, ptr->greyPen);
    if (inc < 0)
    {
        Arc(hDc, x - ROUNDWIDTH, y - ROUNDWIDTH, x + ROUNDWIDTH, y + ROUNDWIDTH, x + ROUNDWIDTH, y, x, y - ROUNDWIDTH);
        if (!ptr->flat)
        {
            SelectObject(hDc, pen);
            pen = SelectObject(hDc, ptr->whitePen);
            Arc(hDc, x + 1 - ROUNDWIDTH, y - 1 - ROUNDWIDTH, x + 1 + ROUNDWIDTH, y - 1 + ROUNDWIDTH, x + 1 + ROUNDWIDTH, y - 1,
                x + 1, y - 1 - ROUNDWIDTH);
        }
    }
    else
    {
        Arc(hDc, x - ROUNDWIDTH, y - ROUNDWIDTH, x + ROUNDWIDTH, y + ROUNDWIDTH, x, y + ROUNDWIDTH, x + ROUNDWIDTH, y);
        if (!ptr->flat)
        {
            SelectObject(hDc, pen);
            pen = SelectObject(hDc, ptr->whitePen);
            Arc(hDc, x + 1 - ROUNDWIDTH, y + 1 - ROUNDWIDTH, x + 1 + ROUNDWIDTH, y + 1 + ROUNDWIDTH, x + 1, y + 1 + ROUNDWIDTH,
                x + 1 + ROUNDWIDTH, y + 1);
        }
    }
    if (!ptr->flat)
    {
        SelectObject(hDc, pen);
    }
}
void DrawWindowBmp(HWND hwnd, struct _tabStruct* ptr, HDC hDc, int bordertype)
{
    RECT r;
    HDC dcmem = CreateCompatibleDC(hDc);
    HBITMAP old;
    if (ptr->displayMenu)
        old = SelectObject(dcmem, ptr->menuBitmap2);
    else
        old = SelectObject(dcmem, ptr->menuBitmap);
    GetClientRect(hwnd, &r);
    BitBlt(hDc, r.right + WINDOWICONLEFT, r.bottom + WINDOWICONTOP, WINDOWICONWIDTH, WINDOWICONWIDTH, dcmem, 0, 0, SRCCOPY);
    SelectObject(dcmem, old);
    DeleteDC(dcmem);
    if (bordertype)
    {
        r.left = r.right + WINDOWICONLEFT;
        r.right = r.left + WINDOWICONWIDTH - 2;
        r.top = r.bottom + WINDOWICONTOP;
        r.bottom = r.top + WINDOWICONWIDTH - 2;
        DrawEdge(hDc, &r, bordertype, BF_RECT);
    }
}
static void DrawBar(HDC dc, RECT* r1)
{
    HPEN pen1, pen2, pen3, oldpen;
    RECT r = *r1;
    pen1 = CreatePen(PS_SOLID, 1, RetrieveSysColor(COLOR_WINDOW));
    pen2 = CreatePen(PS_SOLID, 1, RetrieveSysColor(COLOR_3DSHADOW));
    pen3 = CreatePen(PS_SOLID, 1, RetrieveSysColor(COLOR_3DDKSHADOW));
    oldpen = SelectObject(dc, pen2);
    /*
    MoveToEx(dc, r.right-3, r.top, 0);
    LineTo(dc, r.right-3, r.bottom - BARLEFT);
//    SelectObject(dc, pen2);
    MoveToEx(dc, r.right - 2, r.top-1, 0);
    LineTo(dc, r.right - 2, r.bottom - BARLEFT + 1);
    SelectObject(dc, pen3);
    */
    MoveToEx(dc, r.right - 1, r.top, 0);
    LineTo(dc, r.right - 1, r.bottom - 1);
    SelectObject(dc, oldpen);
    DeleteObject(pen1);
    DeleteObject(pen2);
    DeleteObject(pen3);
}
void DrawTabs(HWND hwnd, HDC hDc, RECT* r, struct _tabStruct* ptr)
{
    DWORD darkcolor = GetSysColor(COLOR_BTNHIGHLIGHT);
    HPEN darkpen = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));
    HBRUSH darkbrush;
    int bottom = GetWindowLong(hwnd, GWL_STYLE) & TABS_BOTTOM;
    struct _singleTab* tab = ptr->active.head;
    SIZE size;
    int right = r->right;
    int oldbk, oldfg;
    int x = LEFTHALF;
    int y = 0;
    int rightInset = RIGHTBORDER / 2;
    HPEN pen = GetStockObject(WHITE_PEN);
    HBRUSH oldbr;
    int n = darkcolor & 0xff;
    darkcolor &= 0xffff00;
    n = n * 9 / 10;
    darkcolor |= n;
    n = darkcolor & 0xff00;
    darkcolor &= 0xff00ff;
    n = n * 9 / 10;
    n &= 0xff00;
    darkcolor |= n;
    n = darkcolor & 0xff0000;
    darkcolor &= 0xffff;
    n = n * 9 / 10;
    n &= 0xff0000;
    darkcolor |= n;
    darkbrush = CreateSolidBrush(darkcolor);
    if (GetWindowLong(hwnd, GWL_STYLE) & TABS_CLOSEBTN)
    {
        rightInset += 12;
    }
    if (GetWindowLong(hwnd, GWL_STYLE) & TABS_WINDOWBTN)
    {
        right -= WINDOWICONWIDTH + 4;
    }
    if (!bottom)
    {
        y = r->bottom - r->top - ptr->fontHeight - BOTTOMBORDER;
        MyDrawEdge(hDc, ptr, r->left, r->bottom - BOTTOMBORDER + 1, r->right, r->bottom - BOTTOMBORDER + 1, 1);
        MyDrawEdge(hDc, ptr, r->left, r->bottom - 1, r->right, r->bottom - 1, 1);
    }
    else
    {
        y = BOTTOMBORDER - 4;
        /*
        MyDrawEdge(hDc, ptr, r->left, BOTTOMBORDER-1,
                   r->right, BOTTOMBORDER-1, 1);
        */
        MyDrawEdge(hDc, ptr, r->left, 0, r->right, 0, 1);
    }
    oldbr = CreateSolidBrush(RetrieveSysColor(COLOR_WINDOW));
    oldbr = SelectObject(hDc, oldbr);
    while (tab)
    {
        HFONT xfont;
        char* p = strrchr(tab->text, '\\');
        if (p)
            p++;
        else
            p = tab->text;

        if (ptr->selected.head == tab)
        {
            xfont = SelectObject(hDc, ptr->boldFont);
        }
        else
        {
            xfont = SelectObject(hDc, ptr->normalFont);
        }
        x += LEFTHALF;
        GetTextExtentPoint32(hDc, p, strlen(p), &size);
        if (!bottom && (x + size.cx + RIGHTBORDER + rightInset > right))
            break;
        if (!bottom)
        {
            int xfirst, yfirst;
            MyDrawEdge(hDc, ptr, x, y - TOPBORDER, x + size.cx + RIGHTBORDER + rightInset - ROUNDWIDTH, y - TOPBORDER, -1);
            xfirst = x - RIGHTBORDER;
            yfirst = y + (ptr->fontHeight + TOPBORDER) / 2;
            if (tab == ptr->selected.head)
            {
                HPEN pen = GetStockObject(WHITE_PEN);
                pen = SelectObject(hDc, pen);
                xfirst -= RIGHTBORDER - MINEND;
                yfirst = r->bottom - r->top - MINEND - BOTTOMBORDER;
                MoveToEx(hDc, xfirst, r->bottom - BOTTOMBORDER + 1, NULL);
                LineTo(hDc, x + size.cx + RIGHTBORDER + rightInset, r->bottom - BOTTOMBORDER + 1);
                pen = SelectObject(hDc, pen);
            }
            MyDrawEdge(hDc, ptr, xfirst, yfirst, x, y - TOPBORDER, -1);
            MyDrawEdge(hDc, ptr, xfirst, r->bottom - BOTTOMBORDER, xfirst, yfirst, 1);
            MyDrawArc(hDc, ptr, x + size.cx + RIGHTBORDER + rightInset - ROUNDWIDTH, y + TOPBORDER + 1, -1);
            tab->displayRect.left = xfirst;
            tab->displayRect.right = x + size.cx + RIGHTBORDER + rightInset;
            tab->displayRect.top = y;
            tab->displayRect.bottom = y + ptr->fontHeight + TOPBORDER;
        }
        else
        {
            int xfirst, yfirst;
            RECT r1;
            xfirst = x - RIGHTBORDER - LEFTHALF;
            yfirst = (ptr->fontHeight + TOPBORDER + BOTTOMBORDER) / 2;
            r1 = *r;
            r1.left = xfirst;
            r1.top += 1;
            r1.right = x + size.cx + RIGHTBORDER + rightInset - LEFTHALF;
            //            if (tab->iImage >= 0)
            //                r1.right += 20;
            if (tab == ptr->selected.head)
            {
                FillRect(hDc, &r1, (HBRUSH)(COLOR_WINDOW + 1));
                //                r1.left -= 10;
            }
            else
            {
                FillRect(hDc, &r1, darkbrush);
            }
            //            if (tab != ptr->active.head)
            DrawBar(hDc, &r1);
            darkpen = SelectObject(hDc, darkpen);
            MoveToEx(hDc, r1.left, r1.bottom - 1, NULL);
            LineTo(hDc, r1.right - 1, r1.bottom - 1);
            darkpen = SelectObject(hDc, darkpen);
            /*
            MyDrawEdge(hDc, ptr, x, ptr->fontHeight + TOPBORDER + BOTTOMBORDER, x + size.cx + RIGHTBORDER + rightInset - ROUNDWIDTH,
            ptr->fontHeight + TOPBORDER + BOTTOMBORDER,1); if (tab == ptr->selected.head)
            {
                HPEN pen = GetStockObject(WHITE_PEN);
                pen = SelectObject(hDc, pen);
                xfirst -= RIGHTBORDER-MINEND;
                yfirst = MINEND + BOTTOMBORDER;
                MoveToEx(hDc, xfirst, BOTTOMBORDER - 1, NULL);
                LineTo(hDc, x + size.cx + RIGHTBORDER + rightInset, BOTTOMBORDER - 1);
                pen = SelectObject(hDc, pen);
            }
            MyDrawEdge(hDc, ptr, xfirst,yfirst, x, ptr->fontHeight + TOPBORDER + BOTTOMBORDER,1);
            MyDrawEdge(hDc, ptr, xfirst, BOTTOMBORDER, xfirst, yfirst,1);
            MyDrawArc(hDc, ptr, x + size.cx + RIGHTBORDER + rightInset - ROUNDWIDTH, ptr->fontHeight - TOPBORDER + BOTTOMBORDER -1,
            1);
            */
            tab->displayRect.left = xfirst;
            tab->displayRect.right = x + size.cx + RIGHTBORDER + rightInset;
            tab->displayRect.top = y;
            tab->displayRect.bottom = y + ptr->fontHeight + TOPBORDER + BOTTOMBORDER;
        }
        if (tab->chain.next == ptr->selected.head)
            if (!bottom)
            {
                MyDrawEdge(hDc, ptr, x + size.cx + RIGHTBORDER + rightInset, y + ROUNDWIDTH, x + size.cx + RIGHTBORDER + rightInset,
                           y + (ptr->fontHeight + TOPBORDER) / 2, 1);
            }
            else
            {
                //                MyDrawEdge(hDc, ptr, x + size.cx + RIGHTBORDER + rightInset, y + (ptr->fontHeight + TOPBORDER)/2,
                //                    x + size.cx + RIGHTBORDER + rightInset, y + ptr->fontHeight + TOPBORDER - ROUNDWIDTH,1);
            }
        else if (!bottom)
        {

            MyDrawEdge(hDc, ptr, x + size.cx + RIGHTBORDER + rightInset, y - TOPBORDER + ROUNDWIDTH,
                       x + size.cx + RIGHTBORDER + rightInset, y + ptr->fontHeight + 1, 1);
        }
        else
        {
            //                MyDrawEdge(hDc, ptr, x + size.cx + RIGHTBORDER + rightInset, BOTTOMBORDER,
            //                    x + size.cx + RIGHTBORDER + rightInset, y + ptr->fontHeight + TOPBORDER - ROUNDWIDTH,1);
        }

        if (ptr->selected.head == tab)
        {
            oldbk = SetBkColor(hDc, RetrieveSysColor(COLOR_WINDOW));
            oldfg = SetTextColor(hDc, RetrieveSysColor(COLOR_WINDOWTEXT));
            // making an assumption the fill color is corrrect
            if (!bottom)
                ExtFloodFill(hDc, x + RIGHTBORDER / 2, y + 2, RetrieveSysColor(COLOR_WINDOWTEXT), FLOODFILLBORDER);
        }
        else
        {
            oldfg = SetTextColor(hDc, RetrieveSysColor(COLOR_BTNTEXT));
            oldbk = SetBkColor(hDc, bottom ? darkcolor : RetrieveSysColor(COLOR_BTNFACE));
        }
        TextOut(hDc, x + LEFTHALF / 2 - (bottom ? LEFTHALF - 2 : 0), y, p, strlen(p));
        if (tab->modified)
        {
            TextOut(hDc, x + 2 + (tab->iImage ? 20 : 0) + size.cx - (bottom ? LEFTHALF : 0) - LEFTHALF, y, "*", strlen("*"));
        }
        if (bottom && tab->iImage >= 0)
        {
            ImageList_Draw(ptr->hImageList, tab->iImage, hDc, x - RIGHTBORDER - LEFTHALF + 2, y, ILD_TRANSPARENT);
        }
        SelectObject(hDc, xfont);
        SetTextColor(hDc, oldfg);
        SetBkColor(hDc, oldbk);

        x += size.cx + RIGHTBORDER + rightInset;
        tab->displayed = TRUE;
        tab = tab->chain.next;
    }
    oldbr = SelectObject(hDc, oldbr);
    if (GetWindowLong(hwnd, GWL_STYLE) & TABS_WINDOWBTN)
    {
        ptr->displayMenu = tab != NULL;
        DrawWindowBmp(hwnd, ptr, hDc, ptr->windowBtnMode);
    }
    while (tab)
    {
        tab->displayed = FALSE;
        tab = tab->chain.next;
    }
    DeleteObject(darkbrush);
    DeleteObject(darkpen);
}
int GetItem(struct _tabStruct* ptr, int index)
{
    struct _singleTab* tabs = ptr->active.head;
    int i;
    for (i = 0; i < index && tabs; i++, tabs = tabs->chain.next)
        ;
    if (!tabs)
        return 0;
    return tabs->lParam;
}
struct _singleTab* AddTab(struct _tabStruct* ptr, int pos, char* name, LPARAM lParam)
{
    struct _singleTab* tabs = ptr->active.head;
    while (tabs)
    {
        if (tabs->lParam == lParam)
            return NULL;
        tabs = tabs->chain.next;
    }
    tabs = CreateItem(name, lParam);
    if (tabs)
    {
        AddToActive(ptr, pos, tabs);
        AddToSelected(ptr, 0, tabs);
    }
    return tabs;
}
struct _singleTab* RemoveTab(struct _tabStruct* ptr, LPARAM lParam)
{

    struct _singleTab* tabs = ptr->active.head;
    while (tabs)
    {
        if (tabs->lParam == lParam)
        {
            RemoveFromActive(ptr, tabs);
            RemoveFromSelected(ptr, tabs);

            free(tabs->text);
            free(tabs);
            if (ptr->selected.head && !ptr->selected.head->displayed)
            {
                RemoveFromActive(ptr, ptr->selected.head);
                AddToActive(ptr, 0, ptr->selected.head);
            }
            return NULL;
        }
        tabs = tabs->chain.next;
    }
    return NULL;
}
BOOL ModifyTab(struct _tabStruct* ptr, WPARAM modified, LPARAM lParam)
{
    BOOL rv = FALSE;
    struct _singleTab* tabs = ptr->active.head;
    while (tabs)
    {
        if (tabs->lParam == lParam)
        {
            if (tabs->modified != modified)
                rv = TRUE;
            tabs->modified = modified;
            break;
        }
        tabs = tabs->chain.next;
    }
    return rv;
}
void SetImage(struct _tabStruct* ptr, LPARAM lParam, int iImage)
{
    struct _singleTab* tabs = ptr->active.head;
    while (tabs)
    {
        if (tabs->lParam == lParam)
        {
            tabs->iImage = iImage;
        }
        tabs = tabs->chain.next;
    }
}
void SelectTab(struct _tabStruct* ptr, LPARAM lParam)
{
    struct _singleTab* tabs = ptr->active.head;
    while (tabs)
    {
        if (lParam == tabs->lParam)
        {
            if (!tabs->displayed)
            {
                RemoveFromActive(ptr, tabs);
                AddToActive(ptr, 0, tabs);
            }
            RemoveFromSelected(ptr, tabs);
            AddToSelected(ptr, 0, tabs);

            return;
        }
        tabs = tabs->chain.next;
    }
}
void RemoveAllTabs(struct _tabStruct* ptr)
{
    while (ptr->active.head)
    {
        struct _singleTab* next = ptr->active.head->chain.next;
        free(ptr->active.head->text);
        free(ptr->active.head);
        ptr->active.head = next;
    }
    ptr->active.head = ptr->active.tail = NULL;
    ptr->selected.head = ptr->selected.tail = NULL;
}
struct _singleTab* FindTabByPos(HWND hwnd, struct _tabStruct* ptr, LPARAM lParam)
{
    struct _singleTab* tabs = ptr->active.head;
    POINT pt;
    pt.x = (long)(short)LOWORD(lParam);
    pt.y = (long)(short)HIWORD(lParam);
    while (tabs)
    {
        if (tabs->displayed && PtInRect(&tabs->displayRect, pt))
        {
            return tabs;
        }
        tabs = tabs->chain.next;
    }
    return NULL;
}
BOOL IsCloseButton(HWND hwnd, struct _tabStruct* ptr, struct _singleTab* tab, LPARAM lParam)
{
    if (GetWindowLong(hwnd, GWL_STYLE) & TABS_CLOSEBTN)
    {
        RECT r = tab->displayRect;
        POINT pt;
        int mid;
        pt.x = (long)(short)LOWORD(lParam);
        pt.y = (long)(short)HIWORD(lParam);
        mid = (r.bottom + r.top) / 2;
        r.top = mid - CLOSEICONWIDTH / 2;
        r.bottom = mid + CLOSEICONWIDTH / 2;
        r.left = r.right - CLOSEICONWIDTH - 2;
        r.right -= 2;
        return PtInRect(&r, pt);
    }
    return FALSE;
}
void ShowCloseButton(HWND hwnd, struct _tabStruct* ptr, struct _singleTab* tab, BOOL open, LPARAM lParam)
{
    if (ptr->xBitmap)
    {
        if (open && tab && !ptr->xBack && IsCloseButton(hwnd, ptr, tab, lParam))
        {
            int mid;
            HDC dc, cdc;
            HBITMAP oldBitmap;
            dc = GetDC(hwnd);
            ptr->xRect = tab->displayRect;

            mid = (ptr->xRect.bottom + ptr->xRect.top) / 2;
            ptr->xRect.top = mid - CLOSEICONWIDTH / 2;
            ptr->xRect.bottom = mid + CLOSEICONWIDTH / 2;
            ptr->xRect.left = ptr->xRect.right - CLOSEICONWIDTH - 2;
            ptr->xRect.right -= 2;
            ptr->xBack = CreateCompatibleBitmap(dc, CLOSEICONWIDTH, CLOSEICONWIDTH);
            cdc = CreateCompatibleDC(dc);
            ptr->xBack = SelectObject(cdc, ptr->xBack);
            BitBlt(cdc, 0, 0, CLOSEICONWIDTH, CLOSEICONWIDTH, dc, ptr->xRect.left, ptr->xRect.top, SRCCOPY);
            ptr->xBack = SelectObject(cdc, ptr->xBack);
            if (tab == ptr->selected.head)
            {
                oldBitmap = SelectObject(cdc, ptr->xBitmapSelected);
            }
            else
            {
                oldBitmap = SelectObject(cdc, ptr->xBitmap);
            }
            BitBlt(dc, ptr->xRect.left, ptr->xRect.top, CLOSEICONWIDTH, CLOSEICONWIDTH, cdc, 0, 0, SRCCOPY);
            SelectObject(cdc, oldBitmap);
            DeleteDC(cdc);
            ReleaseDC(hwnd, dc);
        }
        else
        {
            if (ptr->xBack)
            {
                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(hwnd, &pt);
                if (!PtInRect(&ptr->xRect, pt))
                {
                    HDC dc, cdc;
                    dc = GetDC(hwnd);
                    cdc = CreateCompatibleDC(dc);
                    ptr->xBack = SelectObject(cdc, ptr->xBack);
                    BitBlt(dc, ptr->xRect.left, ptr->xRect.top, CLOSEICONWIDTH, CLOSEICONWIDTH, cdc, 0, 0, SRCCOPY);
                    ptr->xBack = SelectObject(cdc, ptr->xBack);
                    DeleteDC(cdc);
                    ReleaseDC(hwnd, dc);
                    DeleteObject(ptr->xBack);
                    ptr->xBack = NULL;
                }
            }
        }
    }
}
BOOL IsWindowButton(HWND hwnd, struct _tabStruct* ptr, LPARAM lParam)
{
    if (GetWindowLong(hwnd, GWL_STYLE) & TABS_WINDOWBTN)
    {
        RECT r;
        POINT pt;
        pt.x = (long)(short)LOWORD(lParam);
        pt.y = (long)(short)HIWORD(lParam);
        GetClientRect(hwnd, &r);
        r.left = r.right + WINDOWICONLEFT;
        r.top = r.bottom + WINDOWICONTOP;
        r.right = r.left + WINDOWICONWIDTH;
        r.bottom = r.top + WINDOWICONWIDTH;
        return PtInRect(&r, pt);
    }
    return FALSE;
}
void ShowWindowButton(HWND hwnd, struct _tabStruct* ptr, int flags)
{
    HDC dc;
    if (GetWindowLong(hwnd, GWL_STYLE) & TABS_WINDOWBTN)
    {
        dc = GetDC(hwnd);
        DrawWindowBmp(hwnd, ptr, dc, flags);
        ReleaseDC(hwnd, dc);
        ptr->windowBtnMode = flags;
    }
}
BOOL ChangeHighLight(HWND hwnd, struct _tabStruct* ptr, struct _singleTab* tab) { return FALSE; }
void SetFonts(HWND hwnd, struct _tabStruct* ptr, HFONT font)
{
    LOGFONT lf;
    HDC dc;
    HFONT xfont;
    TEXTMETRIC tm;
    ptr->normalFont = font;
    if (ptr->boldFont)
        DeleteObject(ptr->boldFont);
    GetObject(font, sizeof(LOGFONT), &lf);
    lf.lfItalic = FALSE;
    lf.lfWeight = FW_BOLD;
    ptr->boldFont = CreateFontIndirect(&lf);
    InvalidateRect(hwnd, 0, 1);
    dc = GetDC(hwnd);
    xfont = SelectObject(dc, ptr->boldFont);
    GetTextMetrics(dc, &tm);
    ptr->fontHeight = tm.tmHeight;
    SelectObject(dc, xfont);
    ReleaseDC(hwnd, dc);
}
int Notify(HWND hwnd, struct _singleTab* tab, int notifyCode)
{
    LSTABNOTIFY notify;
    notify.hdr.hwndFrom = hwnd;
    notify.hdr.idFrom = GetWindowLong(hwnd, GWL_ID);
    notify.hdr.code = notifyCode;
    notify.text = tab->text;
    notify.lParam = tab->lParam;
    return SendMessage((HWND)GetWindowLong(hwnd, GWL_HWNDPARENT), WM_NOTIFY, 0, (LPARAM)&notify);
}

static void DeleteMenuList(struct _tabStruct* ptr)
{
    if (ptr->menuList)
    {
        free(ptr->menuList);
        ptr->menuList = NULL;
    }
}
static BOOL InMenuWnd(HWND hwnd)
{
    POINT pt;
    RECT r;
    GetCursorPos(&pt);
    GetWindowRect(hwnd, &r);
    return PtInRect(&r, pt);
}
static int tabimageof(char* name)
{
    name = strrchr(name, '.');
    if (!name)
        return IL_FILES;
    if (!xstricmpz(name, ".exe"))
        return IL_GUI;
    if (!xstricmpz(name, ".lib"))
        return IL_LIB;
    if (!xstricmpz(name, ".dll"))
        return IL_DLL;
    if (!xstricmpz(name, ".asm"))
        return IL_ASM;
    if (!xstricmpz(name, ".c"))
        return IL_C;
    if (!xstricmpz(name, ".cpp") || !xstricmpz(name, ".cxx") || !xstricmpz(name, ".cc"))
        return IL_CPP;
    if (!xstricmpz(name, ".rc") || !xstricmpz(name, ".bmp") || !xstricmpz(name, ".cur") || !xstricmpz(name, ".ico"))
        return IL_RES;
    if (!xstricmpz(name, ".h"))
        return IL_H;
    if (!xstricmpz(name, ".cwa"))
        return IL_CWA;
    return IL_FILES;
}
static void DrawMenuItem(HDC hDC, struct ttrack* pTrack, int index, BOOL highlighted)
{
    MENUITEMINFO info;
    int oldbk, oldtx;
    RECT r;
    int row, col;
    HBRUSH hbr = RetrieveSysBrush(COLOR_MENU);
    memset(&info, 0, sizeof(info));
    info.cbSize = sizeof(info);
    info.fMask = MIIM_STRING;
    col = index / pTrack->rows;
    row = index - col * pTrack->rows;
    r.left = col * pTrack->rowWidth;
    r.top = row * pTrack->rowHeight;
    r.right = r.left + pTrack->rowWidth;
    r.bottom = r.top + pTrack->rowHeight;
    r.left += 21;
    FillRect(hDC, &r, hbr);
    DeleteObject(hbr);
    r.left -= 21;
    if (GetMenuItemInfo(pTrack->menu, index, TRUE, &info))
    {
        char text[1024];
        info.dwTypeData = text;
        info.cch++;
        if (GetMenuItemInfo(pTrack->menu, index, TRUE, &info))
        {
            if (highlighted)
            {
                HPEN pen = CreatePen(PS_SOLID, 0, RetrieveSysColor(COLOR_HIGHLIGHT));
                HBRUSH brush = CreateSolidBrush(RetrieveSysColor(COLOR_HIGHLIGHT));
                pen = SelectObject(hDC, pen);
                brush = SelectObject(hDC, brush);
                r.left += 25;
                r.right -= 4;
                r.top += 1;
                r.bottom -= 1;
                RoundRect(hDC, r.left, r.top, r.right, r.bottom, 5, 5);
                r.left += 2;
                r.right -= 1;
                r.top += 1;
                r.bottom -= 1;
                pen = SelectObject(hDC, pen);
                brush = SelectObject(hDC, brush);
                DeleteObject(pen);
                DeleteObject(brush);
                oldbk = SetBkColor(hDC, RetrieveSysColor(COLOR_HIGHLIGHT));
                oldtx = SetTextColor(hDC, RetrieveSysColor(COLOR_HIGHLIGHTTEXT));
            }
            else
            {
                r.left += 27;
                r.right -= 5;
                r.top += 2;
                r.bottom -= 2;
                oldbk = SetBkColor(hDC, RetrieveSysColor(COLOR_MENU));
                oldtx = SetTextColor(hDC, RetrieveSysColor(COLOR_WINDOWTEXT));
            }
            DrawText(hDC, text, strlen(text), &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            SetBkColor(hDC, oldbk);
            SetTextColor(hDC, oldtx);
        }
    }
}
static void DrawMenu(HDC hDC, struct ttrack* pTrack)
{
    int i;
    int itemCount = GetMenuItemCount(pTrack->menu);
    HBRUSH old;
    HFONT font = CreateFontIndirect(&systemDialogFont);
    font = SelectObject(hDC, font);
    for (i = 0; i < itemCount; i++)
    {
        DrawMenuItem(hDC, pTrack, i, FALSE);
    }
    font = SelectObject(hDC, font);
    DeleteObject(font);
    old = CreateSolidBrush(RetrieveSysColor(COLOR_3DSHADOW));
    old = SelectObject(hDC, old);
    for (i = 0; i < (itemCount + pTrack->rows - 1) / pTrack->rows; i++)
    {
        RECT r;
        r.left = i * pTrack->rowWidth;
        r.top = 0;
        r.right = i * pTrack->rowWidth;
        r.bottom = r.top + pTrack->rowHeight * pTrack->rows;
        MoveToEx(hDC, r.left, r.top, NULL);
        LineTo(hDC, r.left, r.bottom);
        MoveToEx(hDC, r.left + 1, r.top, NULL);
        LineTo(hDC, r.left + 1, r.bottom);
        MoveToEx(hDC, r.left + 20, r.top, NULL);
        LineTo(hDC, r.left + 20, r.bottom);
        ExtFloodFill(hDC, r.left + 3, r.top + 2, 0, FLOODFILLBORDER);
    }
    old = SelectObject(hDC, old);
    DeleteObject(old);
    for (i = 0; i < itemCount; i++)
    {
        MENUITEMINFO info;
        char text[1024];
        memset(&info, 0, sizeof(info));
        info.cbSize = sizeof(info);
        info.fMask = MIIM_STRING;
        if (GetMenuItemInfo(pTrack->menu, i, TRUE, &info))
        {
            info.dwTypeData = text;
            info.cch++;
            if (GetMenuItemInfo(pTrack->menu, i, TRUE, &info))
            {
                RECT r;
                int col = i / pTrack->rows;
                int row = i - col * pTrack->rows;
                r.left = col * pTrack->rowWidth;
                r.top = row * pTrack->rowHeight;
                r.right = r.left + pTrack->rowWidth;
                r.bottom = r.top + pTrack->rowHeight;
                ImageList_Draw(mainIml, tabimageof(text), hDC, r.left + 2, r.top + 2, ILD_NORMAL);
            }
        }
    }
}
static LRESULT CALLBACK MenuWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    struct ttrack* pTrack;
    HDC hDC;
    switch (iMessage)
    {
        case WM_MOUSEMOVE:
            pTrack = (struct ttrack*)GetWindowLong(hwnd, 0);
            if (pTrack->inWindow)
            {
                if (!InMenuWnd(hwnd))
                {
                    pTrack->inWindow = FALSE;
                    //                  ReleaseCapture(hwnd);
                    //					DestroyWindow(hwnd);
                }
            }
            else
            {
                if (InMenuWnd(hwnd))
                {
                    pTrack->inWindow = TRUE;
                }
            }
            if (pTrack->inWindow)
            {
                POINT pt;
                int row, col;
                GetCursorPos(&pt);
                ScreenToClient(hwnd, &pt);
                row = pt.y / pTrack->rowHeight;
                col = pt.x / pTrack->rowWidth;
                row = col * pTrack->rows + row;
                hDC = GetDC(hwnd);
                if (pTrack->oldTrack != -1)
                    DrawMenuItem(hDC, pTrack, pTrack->oldTrack, FALSE);
                pTrack->oldTrack = -1;
                if (row < GetMenuItemCount(pTrack->menu))
                {
                    pTrack->oldTrack = row;
                    DrawMenuItem(hDC, pTrack, row, TRUE);
                }
                ReleaseDC(hwnd, hDC);
            }
            else
            {
                hDC = GetDC(hwnd);
                if (pTrack->oldTrack != -1)
                    DrawMenuItem(hDC, pTrack, pTrack->oldTrack, FALSE);
                ReleaseDC(hwnd, hDC);
                pTrack->oldTrack = -1;
            }
            break;
        case WM_LBUTTONUP:
            pTrack = (struct ttrack*)GetWindowLong(hwnd, 0);
            {
                POINT pt;
                if (InMenuWnd(hwnd))
                {
                    MENUITEMINFO info;
                    int row, col;
                    GetCursorPos(&pt);
                    ScreenToClient(hwnd, &pt);
                    row = pt.y / pTrack->rowHeight;
                    col = pt.x / pTrack->rowWidth;
                    row = col * pTrack->rows + row;
                    memset(&info, 0, sizeof(info));
                    info.cbSize = sizeof(info);
                    info.fMask = MIIM_ID;
                    if (GetMenuItemInfo(pTrack->menu, row, TRUE, &info))
                    {
                        pTrack->id = info.wID;
                        ReleaseCapture();
                        DestroyWindow(hwnd);
                    }
                    return 0;
                }
            }
            // fall through
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
            if (!InMenuWnd(hwnd))
            {
                POINT pt;
                pTrack = (struct ttrack*)GetWindowLong(hwnd, 0);
                pTrack->inWindow = FALSE;
                pt.x = (long)(short)LOWORD(lParam);
                pt.y = (long)(short)HIWORD(lParam);
                ClientToScreen(hwnd, &pt);
                ReleaseCapture();
                DestroyWindow(hwnd);
            }
            return 0;

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
            pTrack = (struct ttrack*)GetWindowLong(hwnd, 0);
            FillRect(hdouble, &rect, (HBRUSH)(COLOR_MENU + 1));
            DrawMenu(hdouble, pTrack);
            BitBlt(hDC, 0, 0, rect.right, rect.bottom, hdouble, 0, 0, SRCCOPY);
            DeleteObject(bitmap);
            DeleteObject(hdouble);
            EndPaint(hwnd, &ps);
        }
            return 0;
            break;
        case WM_CREATE:
            pTrack = (struct ttrack*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
            SetWindowLong(hwnd, 0, (long)pTrack);
            pTrack->oldTrack = -1;
            SetCapture(hwnd);
            break;
        case WM_DESTROY:
            pTrack = (struct ttrack*)GetWindowLong(hwnd, 0);
            SetEvent(pTrack->tWait);
            break;
        case WM_SIZE:
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
static void TrackTabMenuEx(HMENU hMenu, int flags, int x, int y, HWND hwnd, int reserved)
{
    static struct ttrack xx;
    int itemCount = GetMenuItemCount(hMenu);
    int startX, startY, width, height;
    int i;
    HFONT font = CreateFontIndirect(&systemDialogFont);
    TEXTMETRIC tm;
    HDC dc;
    BOOL done = FALSE;
    if (xx.hwnd)
        return;
    memset(&xx, 0, sizeof(xx));
    xx.id = -1;
    xx.menu = hMenu;
    xx.tWait = CreateEvent(0, 0, 0, 0);
    xx.hwnd = CreateWindowEx(0, szMenuClassName, "", WS_POPUP | WS_BORDER, 0, 0, 10, 10, hwnd, 0,
                             (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), (LPVOID)&xx);
    font = (HFONT)SendMessage(xx.hwnd, WM_SETFONT, (WPARAM)font, 0);
    dc = GetDC(xx.hwnd);
    font = SelectObject(dc, font);
    GetTextMetrics(dc, &tm);
    xx.rowHeight = tm.tmHeight + 6;
    for (i = 0; i < itemCount; i++)
    {
        MENUITEMINFO info;
        memset(&info, 0, sizeof(info));
        info.cbSize = sizeof(info);
        info.fMask = MIIM_STRING;
        if (GetMenuItemInfo(xx.menu, i, TRUE, &info))
        {
            char text[1024];
            info.dwTypeData = text;
            info.cch++;
            if (GetMenuItemInfo(xx.menu, i, TRUE, &info))
            {
                SIZE sz;
                char* text = info.dwTypeData;
                GetTextExtentPoint32(dc, text, strlen(text), &sz);
                if (sz.cx + 40 > xx.rowWidth)
                    xx.rowWidth = sz.cx + 40;
            }
        }
    }
    ReleaseDC(hwnd, dc);
    font = SelectObject(dc, font);
    for (xx.rows = 1; xx.rows <= itemCount; xx.rows++)
    {
        xx.cols = (itemCount + xx.rows - 1) / xx.rows;
        if (xx.rows * xx.rowHeight >= xx.cols * xx.rowWidth)
        {
            break;
        }
    }
    if (xx.rows > itemCount)
        xx.rows = itemCount;
    width = xx.cols * xx.rowWidth;
    height = xx.rows * xx.rowHeight + 2;
    // ignoring flags member, assuming top right & lbutton for now...
    startX = x - width;
    startY = y;
    //	MoveWindow(xx.hwnd, startX, startY, width, height, 0);
    //	ShowWindow(xx.hwnd, SW_SHOW);
    SetWindowPos(xx.hwnd, 0, startX, startY, width, height, SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
    while (!done)
    {
        switch (MsgWaitForMultipleObjects(1, &xx.tWait, FALSE, INFINITE, QS_ALLEVENTS))
        {
            case WAIT_OBJECT_0:
                done = TRUE;
                break;
            case WAIT_OBJECT_0 + 1:
            {
                MSG msg;
                GetMessage(&msg, NULL, 0, 0);
                ProcessMessage(&msg);
                break;
            }
        }
    }
    CloseHandle(xx.tWait);

    if (xx.id != -1)
        PostMessage(hwnd, WM_COMMAND, xx.id, 0);
    xx.hwnd = NULL;
}
static int SortFunc(const struct _singleTab** left, const struct _singleTab** right)
{
    char* pleft = strrchr((*left)->text, '\\');
    char* pright = strrchr((*right)->text, '\\');
    if (pleft)
        pleft++;
    else
        pleft = (*left)->text;
    if (pright)
        pright++;
    else
        pright = (*right)->text;
    return stricmp(pleft, pright);
}
static int CreateMenuList(struct _tabStruct* ptr)
{
    int count = 0;
    struct _singleTab* p = ptr->active.head;
    while (p)
        count++, p = p->chain.next;
    DeleteMenuList(ptr);
    if (count)
    {
        ptr->menuList = calloc(count, sizeof(struct _tabStruct*));
        if (ptr->menuList)
        {

            p = ptr->active.head;
            count = 0;
            while (p)
                ptr->menuList[count++] = p, p = p->chain.next;
            qsort(ptr->menuList, count, sizeof(struct _singleItem*), (int (*)(const void*, const void*))SortFunc);
        }
    }
    return count;
}
static HMENU MyCreateMenu(HWND hwnd, struct _tabStruct* ptr)
{
    HMENU rv = NULL;
    if (ptr->menuList)
    {
        rv = CreatePopupMenu();
        if (rv)
        {
            int count = 0, i;
            struct _singleTab* p = ptr->active.head;
            while (p)
                p = p->chain.next, count++;
            for (i = 0; i < count; i++)
            {
                char* q;
                struct _singleTab* s;
                s = ptr->menuList[i];
                q = strrchr(s->text, '\\');
                if (q)
                    q++;
                else
                    q = s->text;
                AppendMenu(rv, MF_STRING, ID_MENUITEMS + i, q);
            }
        }
    }
    return rv;
}
static void DisplayTabMenu(HWND hwnd, struct _tabStruct* ptr)
{
    HMENU hMenu;
    if (CreateMenuList(ptr))
    {
        hMenu = MyCreateMenu(hwnd, ptr);
        if (hMenu)
        {
            POINT pos;
            GetCursorPos(&pos);
            TrackTabMenuEx(hMenu, TPM_TOPALIGN | TPM_RIGHTALIGN | TPM_LEFTBUTTON, pos.x, pos.y, hwnd, 0);
            DestroyMenu(hMenu);
        }
    }
}
static void tabRename(HWND hwnd, struct _tabStruct* ptr, char* newName, LPARAM lParam)
{
    struct _singleTab* tabs = ptr->active.head;
    while (tabs)
    {
        if (tabs->lParam == lParam)
        {
            char* p = strdup(newName);
            if (p)
            {
                free(tabs->text);
                tabs->text = p;
                if (tabs->displayed)
                    InvalidateRect(hwnd, 0, 1);
                break;
            }
        }
        tabs = tabs->chain.next;
    }
}
static void DragTo(HWND hwnd, struct _tabStruct* ptr, struct _singleTab* tab)
{
    struct _singleTab* find = ptr->dragSrc;
    BOOL left;
    while (find)
    {
        if (find == tab)
            break;
        find = find->chain.next;
    }
    left = find == NULL;
    RemoveFromActive(ptr, ptr->dragSrc);
    if (left)
    {
        if (tab->chain.prev == NULL)
        {
            AddToActive(ptr, 0, ptr->dragSrc);
        }
        else
        {
            ptr->dragSrc->chain.next = tab->chain.prev->chain.next;
            ptr->dragSrc->chain.prev = tab->chain.prev;
            tab->chain.prev->chain.next = ptr->dragSrc;
            tab->chain.prev = ptr->dragSrc;
        }
    }
    else
    {
        ptr->dragSrc->chain.next = tab->chain.next;
        ptr->dragSrc->chain.prev = tab;
        if (tab->chain.next)
            tab->chain.next->chain.prev = ptr->dragSrc;
        tab->chain.next = ptr->dragSrc;
    }
    InvalidateRect(hwnd, 0, 1);
}
static LRESULT CALLBACK TabWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    LRESULT rv;
    BOOLEAN notDragging;
    struct _tabStruct* ptr;
    struct _singleTab* tab;
    switch (iMessage)
    {
        TOOLTIPTEXT* lpnhead;
        case WM_NOTIFY:
            lpnhead = (TOOLTIPTEXT*)lParam;
            switch (lpnhead->hdr.code)
            {
                case TTN_SHOW:
                case TTN_POP:
                    break;
                case TTN_NEEDTEXT:
                {
                    // tooltip
                    POINT pt;
                    ptr = (struct _tabStruct*)GetWindowLong(hwnd, 0);
                    GetCursorPos(&pt);
                    ScreenToClient(hwnd, &pt);
                    tab = FindTabByPos(hwnd, ptr, MAKEWORD(pt.y, pt.x));
                    if (tab)
                    {
                        strcpy(lpnhead->lpszText, tab->text);
                    }
                    break;
                }
            }
            break;
        case TABM_SETIMAGE:
            ptr = (struct _tabStruct*)GetWindowLong(hwnd, 0);
            SetImage(ptr, lParam, wParam);
            break;
        case TABM_SETIMAGELIST:
            ptr = (struct _tabStruct*)GetWindowLong(hwnd, 0);
            ptr->hImageList = (HIMAGELIST)lParam;
            break;
        case TABM_GETITEM:
            ptr = (struct _tabStruct*)GetWindowLong(hwnd, 0);
            return GetItem(ptr, wParam);
        case TABM_ADD:
            ptr = (struct _tabStruct*)GetWindowLong(hwnd, 0);
            AddTab(ptr, 0, (char*)wParam, lParam);
            InvalidateRect(hwnd, 0, 1);
            break;
        case TABM_INSERT:
        {
            char buf[256];
            GetWindowText((HWND)lParam, buf, sizeof(buf));
            ptr = (struct _tabStruct*)GetWindowLong(hwnd, 0);
            AddTab(ptr, wParam, buf, lParam);
            InvalidateRect(hwnd, 0, 1);
            break;
        }
        case TABM_REMOVE:
            ptr = (struct _tabStruct*)GetWindowLong(hwnd, 0);
            RemoveTab(ptr, lParam);
            InvalidateRect(hwnd, 0, 1);
            break;
        case TABM_REMOVEALL:
            ptr = (struct _tabStruct*)GetWindowLong(hwnd, 0);
            RemoveAllTabs(ptr);
            InvalidateRect(hwnd, 0, 1);
            break;
        case TABM_SELECT:
            ptr = (struct _tabStruct*)GetWindowLong(hwnd, 0);
            SelectTab(ptr, lParam);
            InvalidateRect(hwnd, 0, 1);
            break;
        case TABM_GETCURSEL:
            ptr = (struct _tabStruct*)GetWindowLong(hwnd, 0);
            return (LRESULT)ptr->selected.head;
        case TABM_RENAME:
            ptr = (struct _tabStruct*)GetWindowLong(hwnd, 0);
            tabRename(hwnd, ptr, (char*)wParam, lParam);
            return 0;
        case TABM_SETMODIFY:
            ptr = (struct _tabStruct*)GetWindowLong(hwnd, 0);
            if (ModifyTab(ptr, wParam, lParam))
                InvalidateRect(hwnd, 0, 1);
            return 0;
        case WM_LBUTTONDOWN:
            ptr = (struct _tabStruct*)GetWindowLong(hwnd, 0);
            if (IsWindowButton(hwnd, ptr, lParam))
            {
                ShowWindowButton(hwnd, ptr, BDR_SUNKEN);
            }
            tab = FindTabByPos(hwnd, ptr, lParam);
            if (tab && (GetWindowLong(hwnd, GWL_STYLE) & TABS_DRAGABLE))
            {
                ptr->dragging = TRUE;
                ptr->dragSrc = tab;
                if (!ptr->captured)
                {
                    SetCapture(hwnd);
                    ptr->captured = TRUE;
                }
            }
            return 0;
        case WM_LBUTTONUP:
            ptr = (struct _tabStruct*)GetWindowLong(hwnd, 0);
            tab = FindTabByPos(hwnd, ptr, lParam);
            notDragging = TRUE;
            if (ptr->dragging && ptr->oldCursor)
            {
                if (tab && tab != ptr->dragSrc)
                {
                    notDragging = FALSE;
                    DragTo(hwnd, ptr, tab);
                }
                //                SetCursor(ptr->oldCursor);
                ptr->oldCursor = NULL;
            }
            if (tab && notDragging)
            {
                if (IsCloseButton(hwnd, ptr, tab, lParam))
                {
                    ShowCloseButton(hwnd, ptr, tab, FALSE, lParam);
                    Notify(hwnd, tab, TABN_CLOSED);
                }
                else
                {
                    SelectTab(ptr, tab->lParam);
                    Notify(hwnd, tab, TABN_SELECTED);
                    InvalidateRect(hwnd, 0, 1);
                }
            }
            else if (IsWindowButton(hwnd, ptr, lParam))
            {
                ShowWindowButton(hwnd, ptr, 0);
                DisplayTabMenu(hwnd, ptr);
            }
            ptr->dragging = FALSE;
            if (ptr->captured)
            {
                ptr->captured = FALSE;
                ReleaseCapture();
            }
            return 0;
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            ptr = (struct _tabStruct*)GetWindowLong(hwnd, 0);
            return 0;
        case WM_MOUSEMOVE:
            ptr = (struct _tabStruct*)GetWindowLong(hwnd, 0);
            tab = FindTabByPos(hwnd, ptr, lParam);
            if (GetWindowLong(hwnd, GWL_STYLE) & TABS_HOTTRACK)
            {
                if (tab)
                {
                    if (tab != ptr->highlighted)
                    {
                        if (ChangeHighLight(hwnd, ptr, tab))
                            InvalidateRect(hwnd, 0, 0);
                    }
                }
            }
            if (ptr->dragging)
            {
                if (tab)
                {
                    //                    old = SetCursor(dragCur);
                }
                else
                {
                    POINT pt;
                    RECT r;
                    //                    old = SetCursor(noCur);
                    GetCursorPos(&pt);
                    GetWindowRect(hwnd, &r);
                    if (!PtInRect(&r, pt))
                    {
                        if (Notify(hwnd, ptr->dragSrc, TABN_STARTDRAG))
                        {
                            // user has to release the capture...
                            ptr->dragging = FALSE;
                            ptr->captured = FALSE;
                        }
                    }
                }
                //                if (!ptr->oldCursor)
                //                    ptr->oldCursor = old;
            }
            ShowCloseButton(hwnd, ptr, tab, TRUE, lParam);
            ShowWindowButton(hwnd, ptr, IsWindowButton(hwnd, ptr, lParam) ? BDR_RAISED : 0);
            break;
        case WM_PAINT:
            ptr = (struct _tabStruct*)GetWindowLong(hwnd, 0);
            if (ptr->xBack)
            {
                DeleteObject(ptr->xBack);
                ptr->xBack = NULL;
            }
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
                FillRect(hdouble, &rect, (HBRUSH)(COLOR_MENUBAR + 1));
                DrawTabs(hwnd, hdouble, &rect, ptr);
                BitBlt(hDC, 0, 0, rect.right, rect.bottom, hdouble, 0, 0, SRCCOPY);
                DeleteObject(bitmap);
                DeleteObject(hdouble);
                EndPaint(hwnd, &ps);
            }
            return 0;
        case WM_CREATE:
            ptr = calloc(1, sizeof(struct _tabStruct));
            if (!ptr)
                return 1;
            SetWindowLong(hwnd, 0, (DWORD)ptr);
            rv = DefWindowProc(hwnd, iMessage, wParam, lParam);
            if (rv)
                return rv;
            ptr->whitePen = CreatePen(PS_SOLID, 0, RetrieveSysColor(COLOR_WINDOW));
            ptr->greyPen = CreatePen(PS_SOLID, 0, RetrieveSysColor(COLOR_WINDOWTEXT));
            SetFonts(hwnd, ptr, CreateFontIndirect(&systemDialogFont));
            if (GetWindowLong(hwnd, GWL_STYLE) & TABS_HINT)
            {
                ptr->toolTip = CreateTTIPWindow(hwnd, 0);
                SendMessage(ptr->toolTip, WM_SETFONT, (WPARAM)ptr->normalFont, 0);
            }
            if (GetWindowLong(hwnd, GWL_STYLE) & TABS_CLOSEBTN)
            {
                ptr->xBitmap = LoadBitmap((HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), "ID_CLOSEBMPSRC");
                ChangeBitmapColor(ptr->xBitmap, 0xc0c0c0, RetrieveSysColor(COLOR_MENUBAR));
                ptr->xBitmapSelected = LoadBitmap((HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), "ID_CLOSEBMPSRC");
                ChangeBitmapColor(ptr->xBitmapSelected, 0xc0c0c0, RetrieveSysColor(COLOR_WINDOW));
            }
            if (GetWindowLong(hwnd, GWL_STYLE) & TABS_WINDOWBTN)
            {
                ptr->menuBitmap = LoadBitmap((HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), "ID_DOWNSRC");
                ChangeBitmapColor(ptr->menuBitmap, 0xc0c0c0, RetrieveSysColor(COLOR_MENUBAR));
                ptr->menuBitmap2 = LoadBitmap((HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), "ID_DOWNSRC2");
                ChangeBitmapColor(ptr->menuBitmap2, 0xc0c0c0, RetrieveSysColor(COLOR_3DFACE));
            }
            ptr->flat = !!(GetWindowLong(hwnd, GWL_STYLE) & TABS_FLAT);
            return rv;
        case WM_DESTROY:
            ptr = (struct _tabStruct*)GetWindowLong(hwnd, 0);
            RemoveAllTabs(ptr);
            DeleteMenuList(ptr);
            DeleteObject(ptr->boldFont);
            DeleteObject(ptr->greyPen);
            DeleteObject(ptr->whitePen);
            if (ptr->xBitmap)
            {
                DeleteObject(ptr->xBitmap);
                DeleteObject(ptr->xBitmapSelected);
            }
            if (ptr->menuBitmap)
                DeleteObject(ptr->menuBitmap);
            if (ptr->toolTip)
                DestroyWindow(ptr->toolTip);
            free(ptr);
            break;
        case WM_SETFONT:
            ptr = (struct _tabStruct*)GetWindowLong(hwnd, 0);
            SetFonts(hwnd, ptr, (HFONT)wParam);
            if (ptr->toolTip)
                SendMessage(ptr->toolTip, WM_SETFONT, (WPARAM)ptr->normalFont, 0);
            return 0;
        case WM_GETFONT:
            ptr = (struct _tabStruct*)GetWindowLong(hwnd, 0);
            if (ptr->normalFont)
                return (LRESULT)ptr->normalFont;
            // otherwise we get the original font e.g. for the create message
            break;
        case WM_CLOSE:
            break;
        case WM_SIZE:
            //			InvalidateRect(hwnd, 0, 1);
            break;
        case WM_MOVE:
            break;
        case WM_COMMAND:
            if (wParam >= ID_MENUITEMS)
            {
                struct _singleTab* tab;
                ptr = (struct _tabStruct*)GetWindowLong(hwnd, 0);
                tab = ptr->menuList[wParam - ID_MENUITEMS];
                SelectTab(ptr, tab->lParam);
                Notify(hwnd, tab, TABN_SELECTED);
                InvalidateRect(hwnd, 0, 1);
                DeleteMenuList(ptr);
            }
            break;
        default:
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
void RegisterLsTabWindow(HINSTANCE hInstance)
{
    WNDCLASS wc;
    HBITMAP bmp;
    memset(&wc, 0, sizeof(wc));
    wc.style = 0;
    wc.lpfnWndProc = &TabWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(LPVOID);
    wc.hInstance = hInstance;
    wc.hIcon = 0;  // LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = 0;  // RetrieveSysBrush(COLOR_MENUBAR);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szTabClassName;
    RegisterClass(&wc);

    wc.lpszClassName = szMenuClassName;
    wc.lpfnWndProc = &MenuWndProc;
    wc.hbrBackground = (HBRUSH)(COLOR_MENU + 1);
    RegisterClass(&wc);

    dragCur = LoadCursor(hInstance, "ID_DRAGCUR");
    noCur = LoadCursor(hInstance, "ID_NODRAGCUR");

    mainIml = ImageList_Create(16, 16, ILC_COLOR24, IL_IMAGECOUNT, 0);
    bmp = LoadBitmap(hInstance, "ID_FILES");
    ChangeBitmapColor(bmp, 0xffffff, RetrieveSysColor(COLOR_3DSHADOW));
    ImageList_Add(mainIml, bmp, NULL);
    DeleteObject(bmp);
}
HWND CreateLsTabWindow(HWND parent, int style)
{
    return CreateWindow(szTabClassName, 0, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CHILD | style, 0, 0, 10, 10, parent, 0,
                        (HINSTANCE)GetWindowLong(parent, GWL_HINSTANCE), NULL);
}