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
#include "rc.h"
#include "rcgui.h"

#define ID_EDIT 2001

extern HWND hwndSrcTab;
extern HWND hwndClient;
extern HINSTANCE hInstance;
extern PROJECTITEM* workArea;
extern LOGFONT systemMenuFont;

static char* szMenuDrawClassName = "xccMenuDrawClass";
static char* szUntitled = "Menu";
static HBITMAP arrImage;
static HCURSOR dragCur, noCur;

HANDLE ResGetHeap(PROJECTITEM* wa, struct resRes* data);
EXPRESSION* ResAllocateMenuId();
static void UndoChange(struct resRes* menuData, MENUITEM* item);

struct menuUndo
{
    struct menuUndo* next;
    enum
    {
        mu_changed,
        mu_insert,
        mu_delete,
        mu_move
    } type;
    MENUITEM** place;
    MENUITEM* item;
    WCHAR* text;
    EXPRESSION* id;
    EXPRESSION* state;
    EXPRESSION* help;
    int flags;
};
static void InsertMenuProperties(HWND lv, struct resRes* data);
static void GetMenuPropText(char* buf, HWND lv, struct resRes* data, int row);
static HWND MenuPropStartEdit(HWND lv, int row, struct resRes* data);
static void MenuPropEndEdit(HWND lv, int row, HWND editWnd, struct resRes* data);
// callbacks to populate the properties window for this resource editor
static struct propertyFuncs menuFuncs = {InsertMenuProperties, GetMenuPropText, MenuPropStartEdit, MenuPropEndEdit};
static void InsertMenuItemProperties(HWND lv, struct resRes* data);
static void GetMenuItemPropText(char* buf, HWND lv, struct resRes* data, int row);
static HWND MenuItemPropStartEdit(HWND lv, int row, struct resRes* data);
static void MenuItemPropEndEdit(HWND lv, int row, HWND editWnd, struct resRes* data);
struct propertyFuncs menuItemFuncs = {InsertMenuItemProperties, GetMenuItemPropText, MenuItemPropStartEdit, MenuItemPropEndEdit};
static void SetSeparatorFlag(MENU* menu, MENUITEM* mi, BOOL issep, BOOL extended)
{
    if (extended)
    {
        if (issep)
            AddToStyle(&mi->type, "MFT_SEPARATOR", MFT_SEPARATOR);
        else
            RemoveFromStyle(&mi->type, MFT_SEPARATOR);
    }
    else
    {
        if (issep)
            mi->flags |= MI_SEPARATOR;
        else
            mi->flags &= ~MI_SEPARATOR;
    }
}
static void InsertMenuProperties(HWND lv, struct resRes* data)
{
    PropSetGroup(lv, 102, L"General Characteristics");
    PropSetItem(lv, 0, 102, "Resource Id");
    PropSetItem(lv, 1, 102, "Language");
    PropSetItem(lv, 2, 102, "SubLanguage");
    PropSetItem(lv, 3, 102, "Characteristics");
    PropSetItem(lv, 4, 102, "Version");
}
void GetMenuPropText(char* buf, HWND lv, struct resRes* data, int row) { GetAccPropText(buf, lv, data, row); }
HWND MenuPropStartEdit(HWND lv, int row, struct resRes* data) { return AccPropStartEdit(lv, row, data); }
void MenuPropEndEdit(HWND lv, int row, HWND editWnd, struct resRes* data)
{
    AccPropEndEdit(lv, row, editWnd, data);
    DestroyWindow(editWnd);
    ListView_SetItemText(lv, row, 1, LPSTR_TEXTCALLBACK);
    SetFocus(data->activeHwnd);
}
static void InsertMenuItemProperties(HWND lv, struct resRes* data)
{
    PropSetGroup(lv, 101, L"Menu Item Characteristics");
    PropSetItem(lv, 0, 101, "Menu Identifier");
    PropSetItem(lv, 1, 101, "Text");
    PropSetItem(lv, 2, 101, "Grayed");
    PropSetItem(lv, 3, 101, "Checked");
}
void GetMenuItemPropText(char* buf, HWND lv, struct resRes* data, int row)
{
    buf[0] = 0;
    switch (row)
    {
        int flag;
        case 0:
            FormatExp(buf, data->gd.selectedMenu->id);
            break;
        case 1:
            if (data->gd.selectedMenu->text)
                StringWToA(buf, data->gd.selectedMenu->text, wcslen(data->gd.selectedMenu->text));
            else
                strcpy(buf, "<SEPARATOR>");
            break;
        case 2:
            if (data->resource->extended)
            {
                flag = Eval(data->gd.selectedMenu->state) & MFS_GRAYED;
            }
            else
            {
                flag = data->gd.selectedMenu->flags & MI_GRAYED;
            }
            if (flag)
                strcpy(buf, "Yes");
            else
                strcpy(buf, "No");
            break;
        case 3:
            if (data->resource->extended)
            {
                flag = Eval(data->gd.selectedMenu->state) & MFS_CHECKED;
            }
            else
            {
                flag = data->gd.selectedMenu->flags & MI_CHECKED;
            }
            if (flag)
                strcpy(buf, "Yes");
            else
                strcpy(buf, "No");
            break;
    }
}
HWND MenuItemPropStartEdit(HWND lv, int row, struct resRes* data)
{
    HWND rv;
    if (row < 2)
    {
        rv = PropGetHWNDText(lv);
        if (rv)
        {
            char buf[256];
            GetMenuItemPropText(buf, lv, data, row);
            SendMessage(rv, WM_SETTEXT, 0, (LPARAM)buf);
        }
    }
    else
    {
        rv = PropGetHWNDCombobox(lv, FALSE);
        if (rv)
        {
            char buf[256];
            int v;
            GetMenuItemPropText(buf, lv, data, row);
            v = SendMessage(rv, CB_ADDSTRING, 0, (LPARAM) "No");
            SendMessage(rv, CB_SETITEMDATA, v, 0);
            v = SendMessage(rv, CB_ADDSTRING, 0, (LPARAM) "Yes");
            SendMessage(rv, CB_SETITEMDATA, v, 1);
            SendMessage(rv, CB_SETCURSEL, buf[0] == 'Y' ? 1 : 0, 0);
        }
    }
    return rv;
}
void MenuItemPropEndEdit(HWND lv, int row, HWND editWnd, struct resRes* data)
{
    char buf[256];
    char buf1[256];
    switch (row)
    {
        int n;
        case 0:
            GetWindowText(editWnd, buf, 256);
            if (strcmp(buf, buf1))
            {
                GetMenuItemPropText(buf1, lv, data, row);
                PropSetIdName(data, buf, &data->gd.selectedMenu->id, NULL, FALSE);
            }
            break;
        case 1:
            GetWindowText(editWnd, buf, 256);
            GetMenuItemPropText(buf1, lv, data, row);
            if (strcmp(buf, buf1))
            {
                if (data->gd.selectedMenu->id)
                    ResGetMenuItemName(data->gd.selectedMenu->id, buf);
                StringAsciiToWChar(&data->gd.selectedMenu->text, buf, strlen(buf));
                InvalidateRect(data->activeHwnd, 0, 0);
            }
            break;
        case 2:
            UndoChange(data, data->gd.selectedMenu);
            n = SendMessage(editWnd, CB_GETCURSEL, 0, 0);
            if (data->resource->extended)
            {
                int flag = Eval(data->gd.selectedMenu->state) & MFS_GRAYED;
                if (n && !flag)
                    AddToStyle(&data->gd.selectedMenu->state, "MFS_GRAYED", MFS_GRAYED);
                else if (!n && flag)
                    RemoveFromStyle(&data->gd.selectedMenu->state, MFS_GRAYED);
            }
            else
            {
                if (n)
                    data->gd.selectedMenu->flags |= MI_GRAYED;
                else
                    data->gd.selectedMenu->flags &= ~MI_GRAYED;
            }
            InvalidateRect(data->activeHwnd, 0, 0);
            break;
        case 3:
            UndoChange(data, data->gd.selectedMenu);
            n = SendMessage(editWnd, CB_GETCURSEL, 0, 0);
            if (data->resource->extended)
            {
                int flag = Eval(data->gd.selectedMenu->state) & MFS_CHECKED;
                if (n && !flag)
                    AddToStyle(&data->gd.selectedMenu->state, "MFS_CHECKED", MFS_CHECKED);
                else if (!n && flag)
                    RemoveFromStyle(&data->gd.selectedMenu->state, MFS_CHECKED);
            }
            else
            {
                if (n)
                    data->gd.selectedMenu->flags |= MI_CHECKED;
                else
                    data->gd.selectedMenu->flags &= ~MI_CHECKED;
            }
            InvalidateRect(data->activeHwnd, 0, 0);
            break;
    }
    ResSetDirty(data);
    DestroyWindow(editWnd);
    ListView_SetItemText(lv, row, 1, LPSTR_TEXTCALLBACK);
    SetFocus(data->activeHwnd);
}
static int CalculateMenuWidth(HDC hDC, MENUITEM* items)
{
    int textWidth = 0, cmdWidth = 0;
    int maxWidth;
    SIZE sz;
    while (items)
    {
        if (items->text)
        {
            WCHAR* n = wcschr(items->text, L'\t');
            int m = wcslen(items->text);
            if (n)
            {
                GetTextExtentPoint32W(hDC, items->text, n - items->text, &sz);
                if (sz.cx > textWidth)
                    textWidth = sz.cx;
                n++;
                GetTextExtentPoint32W(hDC, n, wcslen(n), &sz);
                if (sz.cx > cmdWidth)
                    cmdWidth = sz.cx;
            }
            else
            {
                GetTextExtentPoint32W(hDC, items->text, m, &sz);
                if (sz.cx > textWidth)
                    textWidth = sz.cx;
            }
        }
        items = items->next;
    }
    maxWidth = textWidth + cmdWidth;
    if (cmdWidth)
        maxWidth += 16;
    if (maxWidth < 64)
        maxWidth = 64;
    return maxWidth;
}
static void GetSize(HDC hDC, MENUITEM* items, int x, int y, int fontHeight, LPSIZE sz)
{
    int width = CalculateMenuWidth(hDC, items) + 28;
    int pos = y;
    while (items)
    {
        if (items->popup && items->expanded)
        {
            GetSize(hDC, items->popup, x + width, pos, fontHeight, sz);
        }
        pos += fontHeight;
        items = items->next;
    }
    // this adds a row at the bottom and at the right for the shims...
    if (x + width + width > sz->cx)
        sz->cx = x + width + width;
    pos += fontHeight;
    if (pos > sz->cy)
        sz->cy = pos;
}
static void GetSizeTopRow(HDC hDC, MENUITEM* items, int x, int y, int fontHeight, LPSIZE sz)
{
    SIZE xx;
    xx.cx = 40;
    while (items)
    {
        if (items->popup && items->expanded)
            GetSize(hDC, items->popup, x, y + fontHeight, fontHeight, sz);
        GetTextExtentPoint32W(hDC, items->text, wcslen(items->text), &xx);
        xx.cx += 16;
        x += xx.cx;
        items = items->next;
    }
    x += xx.cx;  // for the shim
    if (x > sz->cx)
        sz->cx = x;
    if (fontHeight > sz->cy)
        sz->cy = fontHeight;
}
static void DrawItem(HDC hDC, WCHAR* text, BOOL selected, int bg, int x, int y, int width, int height, BOOL checked, BOOL grayed)
{
    RECT r1;
    r1.left = x;
    r1.top = y;
    r1.right = x + width;
    r1.bottom = y + height;
    FillRect(hDC, &r1, (HBRUSH)(bg + 1));

    MoveToEx(hDC, x, y, NULL);
    LineTo(hDC, x + width, y);
    LineTo(hDC, x + width, y + height);
    LineTo(hDC, x, y + height);
    LineTo(hDC, x, y);

    if (text)
    {
        char buf[256];
        WCHAR wbuf[256];
        WCHAR* n;
        int old = SetTextColor(hDC, grayed ? GetSysColor(COLOR_BTNSHADOW) : GetSysColor(COLOR_WINDOWTEXT));
        StringWToA(buf, text, wcslen(text));
        SetBkColor(hDC, GetSysColor(bg));
        wbuf[MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buf, strlen(buf), wbuf, 256)] = 0;
        n = wcschr(wbuf, L'\t');
        if (n)
        {
            SIZE sz;
            TextOutW(hDC, x + 3, y + 1, wbuf, n - wbuf);
            n++;
            GetTextExtentPoint32W(hDC, n, wcslen(n), &sz);
            TextOutW(hDC, x + width - 10 - sz.cx, y + 1, n, wcslen(n));
        }
        else
        {
            TextOutW(hDC, x + 3, y + 1, wbuf, wcslen(wbuf));
        }
        SetTextColor(hDC, old);
    }
    else  // separator
    {
        MoveToEx(hDC, x + 5, y + height / 2 - 1, NULL);
        LineTo(hDC, x + width - 5, y + height / 2 - 1);
        MoveToEx(hDC, x + 5, y + height / 2, NULL);
        LineTo(hDC, x + width - 5, y + height / 2);
    }
    if (selected)
    {
        HPEN pen = CreatePen(PS_DOT, 0, 0);
        pen = SelectObject(hDC, pen);
        MoveToEx(hDC, x + 1, y + 1, NULL);
        LineTo(hDC, x + width - 1, y + 1);
        LineTo(hDC, x + width - 1, y + height - 1);
        LineTo(hDC, x + 1, y + height - 1);
        LineTo(hDC, x + 1, y + 1);
        pen = SelectObject(hDC, pen);
        DeleteObject(pen);
    }
}
static void PaintColumn(HDC hDC, HDC arrow, MENUITEM* items, MENUITEM* selected, int x, int y, HFONT font, int fontHeight,
                        BOOL extended)
{
    MENUITEM* orig = items;
    int width = CalculateMenuWidth(hDC, items) + 28;
    int pos = y;
    BOOL submenu = FALSE;
    while (items)
    {
        int checked;
        int grayed;
        if (extended)
        {
            checked = Eval(items->state) & MFS_CHECKED;
            grayed = Eval(items->state) & MFS_GRAYED;
        }
        else
        {
            checked = items->flags & MI_CHECKED;
            grayed = items->flags & MI_GRAYED;
        }
        DrawItem(hDC, items->text, selected == items, COLOR_BTNFACE, x, pos, width, fontHeight, checked, grayed);
        if (items->popup)
        {
            if (items->expanded)
                submenu = TRUE;
            BitBlt(hDC, x + width - 10, pos + (fontHeight - 8) / 2, 8, 8, arrow, 0, 0, SRCCOPY);
        }
        pos += fontHeight;
        items = items->next;
    }
    if (submenu)
    {

        items = orig;
        pos = y;
        while (items)
        {
            if (items->popup && items->expanded)
            {
                PaintColumn(hDC, arrow, items->popup, selected, x + width, pos, font, fontHeight, extended);
            }
            pos += fontHeight;
            items = items->next;
        }
    }
    else
    {
        DrawItem(hDC, L"<---->", FALSE, COLOR_WINDOW, x, pos, width, fontHeight, FALSE, FALSE);
        items = orig;
        pos = y;
        while (items)
        {
            if (items->expanded)
            {
                DrawItem(hDC, L"<---->", FALSE, COLOR_WINDOW, x + width, pos, width, fontHeight, FALSE, FALSE);
            }
            pos += fontHeight;
            items = items->next;
        }
    }
}
static void PaintTopRow(HDC hDC, HDC arrow, MENUITEM* items, MENUITEM* selected, int x, int y, HFONT font, int fontHeight,
                        int extended)
{
    MENUITEM* orig = items;
    int orig_x = x;
    SIZE xx;
    xx.cx = 40;
    while (items)
    {
        int checked;
        int grayed;
        if (extended)
        {
            checked = Eval(items->state) & MFS_CHECKED;
            grayed = Eval(items->state) & MFS_GRAYED;
        }
        else
        {
            checked = items->flags & MI_CHECKED;
            grayed = items->flags & MI_GRAYED;
        }
        GetTextExtentPoint32W(hDC, items->text, wcslen(items->text), &xx);
        xx.cx += 16;
        DrawItem(hDC, items->text, selected == items, COLOR_BTNFACE, x, y, xx.cx, fontHeight, checked, grayed);
        x += xx.cx;
        items = items->next;
    }
    GetTextExtentPoint32W(hDC, L"<---->", 6, &xx);
    xx.cx += 16;
    DrawItem(hDC, L"<---->", FALSE, COLOR_WINDOW, x, y, xx.cx, fontHeight, FALSE, FALSE);
    items = orig;
    x = orig_x;
    while (items)
    {
        SIZE xx;
        if (items->expanded)
            PaintColumn(hDC, arrow, items->popup, selected, x, y + fontHeight, font, fontHeight, extended);
        GetTextExtentPoint32W(hDC, items->text, wcslen(items->text), &xx);
        xx.cx += 16;
        x += xx.cx;
        items = items->next;
    }
}
static void DoPaint(HWND hwnd, HDC hDC, LPPAINTSTRUCT lpPaint, RECT* r, struct resRes* menuData)
{
    RECT r1;
    HFONT font;
    SIZE sz;
    HDC compatDC, arrowDC;
    HBITMAP compatBitmap, arrowBitmap;
    int fontHeight;
    fontHeight = (systemMenuFont.lfHeight < 0 ? -systemMenuFont.lfHeight : systemMenuFont.lfHeight) + 6;
    font = CreateFontIndirect(&systemMenuFont);
    compatDC = CreateCompatibleDC(hDC);
    arrowDC = CreateCompatibleDC(hDC);
    font = SelectObject(compatDC, font);
    sz.cx = 0;
    sz.cy = 0;
    GetSizeTopRow(compatDC, menuData->resource->u.menu->items, 0, 0, fontHeight, &sz);
    sz.cx += GetSystemMetrics(SM_CXBORDER) * 2;
    sz.cy += GetSystemMetrics(SM_CYBORDER) * 2;
    if (sz.cx < r->right)
        sz.cx = r->right;
    if (sz.cy < r->bottom)
        sz.cy = r->bottom;
    arrowBitmap = arrImage;
    arrowBitmap = SelectObject(arrowDC, arrowBitmap);
    compatBitmap = CreateCompatibleBitmap(hDC, 1000, 1000);
    compatBitmap = SelectObject(compatDC, compatBitmap);
    r1.left = r1.top = 0;
    r1.right = 1000;
    r1.bottom = 1000;
    FillRect(compatDC, &r1, (HBRUSH)(COLOR_APPWORKSPACE + 1));
    SetTextColor(compatDC, GetSysColor(COLOR_WINDOWTEXT));
    PaintTopRow(compatDC, arrowDC, menuData->resource->u.menu->items, menuData->gd.selectedMenu, 0, 0, font, fontHeight,
                menuData->resource->extended);
    font = SelectObject(compatDC, font);
    menuData->gd.scrollMax.x = sz.cx;
    menuData->gd.scrollMax.y = sz.cy;
    if (sz.cx <= r->right)
    {
        menuData->gd.scrollPos.x = 0;
    }
    else
    {
        SetScrollRange(hwnd, SB_HORZ, 0, sz.cx, FALSE);
        r->bottom -= GetSystemMetrics(SM_CYHSCROLL);
    }
    ShowScrollBar(hwnd, SB_HORZ, sz.cx > r->right);
    ShowScrollBar(hwnd, SB_VERT, sz.cy > r->bottom);
    if (sz.cy <= r->bottom)
    {
        menuData->gd.scrollPos.y = 0;
    }
    else
    {
        SetScrollRange(hwnd, SB_VERT, 0, sz.cy, FALSE);
        r->right -= GetSystemMetrics(SM_CXVSCROLL);
    }
    menuData->gd.scrollMax.x = sz.cx;
    menuData->gd.scrollMax.y = sz.cy;
    BitBlt(hDC, r->left + 10, r->top + 10, r->right + 10, r->bottom + 10, compatDC, menuData->gd.scrollPos.x,
           menuData->gd.scrollPos.y, SRCCOPY);
    compatBitmap = SelectObject(compatDC, compatBitmap);
    arrowBitmap = SelectObject(arrowDC, arrowBitmap);
    DeleteObject(compatBitmap);
    DeleteDC(compatDC);
    DeleteDC(arrowDC);
    DeleteObject(font);
}
static void UndoChange(struct resRes* menuData, MENUITEM* item)
{
    struct menuUndo* newUndo = calloc(1, sizeof(struct menuUndo));
    if (newUndo)
    {
        newUndo->type = mu_changed;
        newUndo->item = item;
        newUndo->text = item->text;
        newUndo->id = item->id;
        newUndo->state = item->state;
        newUndo->help = item->help;
        newUndo->flags = item->flags;
        newUndo->next = menuData->gd.undoData;
        menuData->gd.undoData = newUndo;
    }
}
static void UndoDelete(struct resRes* menuData, MENUITEM** item)
{
    struct menuUndo* newUndo = calloc(1, sizeof(struct menuUndo));
    if (newUndo)
    {
        newUndo->type = mu_delete;
        newUndo->place = item;
        newUndo->item = *item;
        newUndo->next = menuData->gd.undoData;
        menuData->gd.undoData = newUndo;
    }
}
static void UndoInsert(struct resRes* menuData, MENUITEM** item)
{
    struct menuUndo* newUndo = calloc(1, sizeof(struct menuUndo));
    if (newUndo)
    {
        newUndo->type = mu_insert;
        newUndo->place = item;
        newUndo->next = menuData->gd.undoData;
        menuData->gd.undoData = newUndo;
    }
}
static MENUITEM** GetItemPlace(MENUITEM** top, MENUITEM* src, MENUITEM* item)
{
    MENUITEM** last = NULL;
    while (*top)
    {
        if (*top == item)
        {
            if (last && *last == src)
            {
                return &(*top)->next;
            }
            return top;
        }
        if ((*top)->popup)
        {
            MENUITEM** rv = GetItemPlace(&(*top)->popup, src, item);
            if (rv)
                return rv;
        }
        last = top;
        top = &(*top)->next;
    }
    return NULL;
}
static void UndoMove(struct resRes* menuData, MENUITEM** src, MENUITEM* item)
{
    struct menuUndo* newUndo = calloc(1, sizeof(struct menuUndo));
    if (newUndo)
    {
        newUndo->type = mu_move;
        newUndo->place = src;
        newUndo->item = item;
        newUndo->next = menuData->gd.undoData;
        menuData->gd.undoData = newUndo;
    }
}
static void DoUndo(struct resRes* menuData)
{
    struct menuUndo* undo = menuData->gd.undoData;
    if (undo)
    {
        menuData->gd.undoData = undo->next;
        switch (undo->type)
        {
            MENUITEM** place;
            case mu_changed:
                undo->item->text = undo->text;
                undo->item->id = undo->id;
                undo->item->state = undo->state;
                undo->item->help = undo->help;
                undo->item->flags = undo->flags;
                break;
            case mu_insert:
                *undo->place = (*undo->place)->next;
                break;
            case mu_delete:
                undo->item->next = *undo->place;
                *undo->place = undo->item;
                break;
            case mu_move:
                place = GetItemPlace(&menuData->resource->u.menu->items, undo->place, undo->item);
                *place = (*place)->next;
                undo->item->next = *undo->place;
                *undo->place = undo->item;
                break;
        }

        InvalidateRect(menuData->activeHwnd, 0, FALSE);
        free(undo);
        if (menuData->gd.cantClearUndo)
            ResSetDirty(menuData);
        else if (!menuData->gd.undoData)
            ResSetClean(menuData);
    }
}
static void MarkUnexpanded(MENUITEM* items)
{
    while (items)
    {
        if (items->popup)
            MarkUnexpanded(items->popup);
        items->expanded = FALSE;
        items = items->next;
    }
}
static void SelectItem(struct resRes* menuData, MENUITEM* orig, MENUITEM* selected, BOOL openEditor, int x, int y, int width,
                       int height)
{
    if (selected->text)
    {
        MarkUnexpanded(orig);
        selected->expanded = TRUE;
        menuData->gd.selectedMenu = selected;
        if (openEditor)
        {
            char buf[512];
            HFONT font;
            font = CreateFontIndirect(&systemMenuFont);
            StringWToA(buf, selected->text, wcslen(selected->text));
            menuData->gd.editWindow =
                CreateWindow("edit", "", WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE, x, y,
                             width, height, menuData->activeHwnd, (HMENU)ID_EDIT, hInstance, NULL);
            SendMessage(menuData->gd.editWindow, WM_SETFONT, (WPARAM)font, TRUE);
            SendMessage(menuData->gd.editWindow, WM_SETTEXT, 0, (LPARAM)buf);
            AccSubclassEditWnd(menuData->activeHwnd, menuData->gd.editWindow);
            SendMessage(menuData->gd.editWindow, EM_SETSEL, 0, -1);
            SetFocus(menuData->gd.editWindow);
        }
    }
}
static BOOL SelectFromColumns(HDC hDC, struct resRes* menuData, MENUITEM* items, int x, int y, int fontHeight, POINT mouse,
                              BOOL makeEditor)
{
    MENUITEM* orig = items;
    int width = CalculateMenuWidth(hDC, orig) + 28;
    if (x + width < mouse.x)
    {
        while (items)
        {
            if (items->expanded)
            {
                return SelectFromColumns(hDC, menuData, items->popup, x + width, y, fontHeight, mouse, makeEditor);
            }
            y += fontHeight;
            items = items->next;
        }
    }
    else
    {
        while (items)
        {
            if (y + fontHeight > mouse.y)
            {
                //                if (items->popup)
                {
                    SelectItem(menuData, orig, items, makeEditor, x, y, width, fontHeight);
                    return TRUE;
                }
                return FALSE;
            }
            y += fontHeight;
            items = items->next;
        }
    }
    return FALSE;
}
static BOOL SelectFromTopRow(HDC hDC, struct resRes* menuData, MENUITEM* items, int fontHeight, POINT mouse, BOOL makeEditor)
{
    MENUITEM* orig = items;
    int x = 10;
    SIZE xx;
    while (items)
    {

        GetTextExtentPoint32W(hDC, items->text, wcslen(items->text), &xx);
        if (items->expanded && mouse.y > 10 + fontHeight)
        {
            if (mouse.x >= x)
                return SelectFromColumns(hDC, menuData, items->popup, x, 10 + fontHeight, fontHeight, mouse, makeEditor);
            x += xx.cx + 16;
        }
        else if (mouse.y < 10 + fontHeight)
        {
            if (mouse.x < x + xx.cx + 16)
            {
                break;
            }
            x += xx.cx + 16;
        }
        else
        {
            x += xx.cx + 16;
        }
        items = items->next;
    }
    if (items)
    {
        SelectItem(menuData, orig, items, makeEditor, x, 10, xx.cx + 16, fontHeight);
        return TRUE;
    }
    return FALSE;
}
static void SelectSubmenu(HWND hwnd, struct resRes* menuData, POINT mouse, BOOL openEditor)
{
    HFONT font;
    int fontHeight;
    HDC hDC = GetDC(hwnd);
    fontHeight = (systemMenuFont.lfHeight < 0 ? -systemMenuFont.lfHeight : systemMenuFont.lfHeight) + 6;
    font = CreateFontIndirect(&systemMenuFont);
    font = SelectObject(hDC, font);
    mouse.x += menuData->gd.scrollPos.x;
    mouse.y += menuData->gd.scrollPos.y;
    if (SelectFromTopRow(hDC, menuData, menuData->resource->u.menu->items, fontHeight, mouse, openEditor))
        InvalidateRect(hwnd, 0, FALSE);
    font = SelectObject(hDC, font);
    DeleteObject(font);
    ReleaseDC(hwnd, hDC);
}
static int HitTestFromColumns(HDC hDC, struct resRes* menuData, MENUITEM* items, int x, int y, int fontHeight, POINT mouse)
{
    MENUITEM* orig = items;
    int width = CalculateMenuWidth(hDC, orig) + 28;
    if (x + width < mouse.x)
    {
        while (items)
        {
            if (items->expanded)
            {
                if (items->popup)
                {
                    return HitTestFromColumns(hDC, menuData, items->popup, x + width, y, fontHeight, mouse);
                }
                else
                {
                    if (x + width + width > mouse.x)
                        if (y + fontHeight > mouse.y && y <= mouse.y)
                            return 2;
                    return 0;
                }
            }
            y += fontHeight;
            items = items->next;
        }
    }
    else
    {
        while (items)
        {
            if (y + fontHeight > mouse.y)
            {
                while (orig)
                {
                    if (orig == menuData->gd.selectedMenu)
                        break;
                    orig = orig->next;
                }
                menuData->gd.selectedMenu = items;
                return 1 | (orig ? 8 : 0);
            }
            y += fontHeight;
            items = items->next;
        }
        if (y + fontHeight > mouse.y)
        {
            while (orig)
            {
                if (orig == menuData->gd.selectedMenu)
                    break;
                orig = orig->next;
            }
            menuData->gd.selectedMenu = items;
            return 2 | (orig ? 8 : 0);
        }
    }
    return 0;
}
static int HitTestFromTopRow(HDC hDC, struct resRes* menuData, MENUITEM* items, int fontHeight, POINT mouse)
{
    MENUITEM* orig = items;
    int x = 10;
    SIZE xx;
    while (items)
    {

        GetTextExtentPoint32W(hDC, items->text, wcslen(items->text), &xx);
        if (items->expanded && mouse.y > 10 + fontHeight)
        {
            if (mouse.x >= x)
                return HitTestFromColumns(hDC, menuData, items->popup, x, 10 + fontHeight, fontHeight, mouse);
            x += xx.cx + 16;
        }
        else if (mouse.y < 10 + fontHeight)
        {
            if (mouse.x < x + xx.cx + 16)
            {
                while (orig)
                {
                    if (orig == menuData->gd.selectedMenu)
                        break;
                    orig = orig->next;
                }
                menuData->gd.selectedMenu = items;
                return 1 + 4 + (orig ? 8 : 0);
            }
            x += xx.cx + 16;
        }
        else
        {
            x += xx.cx + 16;
        }
        items = items->next;
    }
    if (mouse.x < x + xx.cx + 16)
    {
        while (orig)
        {
            if (orig == menuData->gd.selectedMenu)
                break;
            orig = orig->next;
        }
        menuData->gd.selectedMenu = items;
        return 2 + 4 + (orig ? 8 : 0);
    }
    return 0;
}
int menuHitTest(HWND hwnd, struct resRes* menuData, POINT mouse)
{
    int rv;
    HFONT font;
    int fontHeight;
    HDC hDC = GetDC(hwnd);
    fontHeight = (systemMenuFont.lfHeight < 0 ? -systemMenuFont.lfHeight : systemMenuFont.lfHeight) + 6;
    font = CreateFontIndirect(&systemMenuFont);
    font = SelectObject(hDC, font);
    mouse.x += menuData->gd.scrollPos.x;
    mouse.y += menuData->gd.scrollPos.y;
    rv = HitTestFromTopRow(hDC, menuData, menuData->resource->u.menu->items, fontHeight, mouse);
    font = SelectObject(hDC, font);
    DeleteObject(font);
    ReleaseDC(hwnd, hDC);
    return rv;
}
static void DoMove(struct resRes* menuData, MENUITEM* item, MENUITEM* dest)
{
    if (item != dest)
    {
        MENUITEM** itemplace = GetItemPlace(&menuData->resource->u.menu->items, NULL, item);
        MENUITEM** destplace;
        if (dest)
        {
            destplace = GetItemPlace(&menuData->resource->u.menu->items, item, dest);
        }
        else
        {
            if (!item->next)
                return;
            destplace = itemplace;
            while (*destplace)
                destplace = &(*destplace)->next;
        }
        if (itemplace && destplace)
        {
            UndoMove(menuData, itemplace, item);
            *itemplace = (*itemplace)->next;
            item->next = *destplace;
            *destplace = item;
            ResSetDirty(menuData);
            InvalidateRect(menuData->activeHwnd, 0, FALSE);
        }
    }
}
static void DoInsert(struct resRes* menuData, MENUITEM** items, int code, MENUITEM* parent)
{
    switch (code)
    {
        MENUITEM* newItem;
        case IDM_DELETE:
            UndoDelete(menuData, items);
            (*items)->expanded = FALSE;
            *items = (*items)->next;
            ResSetDirty(menuData);
            InvalidateRect(menuData->activeHwnd, 0, FALSE);
            break;
        case IDM_INSERT:
            ResGetHeap(workArea, menuData);
            newItem = rcAlloc(sizeof(MENUITEM));
            newItem->next = *items;
            *items = newItem;
            StringAsciiToWChar(&newItem->text, "New Item", 8);
            if (parent)
            {
                newItem->id = parent->id;
                parent->id = NULL;
            }
            else
            {
                newItem->id = ResAllocateMenuId();
            }
            UndoInsert(menuData, items);
            ResSetDirty(menuData);
            InvalidateRect(menuData->activeHwnd, 0, FALSE);
            break;
        case IDM_INSERT_SEPARATOR:
            ResGetHeap(workArea, menuData);
            newItem = rcAlloc(sizeof(MENUITEM));
            newItem->next = *items;
            SetSeparatorFlag(menuData->resource->u.menu, newItem, TRUE, menuData->resource->extended);
            *items = newItem;
            UndoInsert(menuData, items);
            ResSetDirty(menuData);
            InvalidateRect(menuData->activeHwnd, 0, FALSE);
            break;
    }
}
static int InsertDeleteFromColumns(HDC hDC, struct resRes* menuData, MENUITEM** items, int x, int y, int fontHeight, int code)
{
    MENUITEM** orig = items;
    int width = CalculateMenuWidth(hDC, *orig) + 28;
    if (x + width < menuData->gd.selectedColumn)
    {
        while (*items)
        {
            if ((*items)->expanded)
            {
                if ((*items)->popup)
                {
                    return InsertDeleteFromColumns(hDC, menuData, &(*items)->popup, x + width, y, fontHeight, code);
                }
                else
                {
                    if (x + width + width > menuData->gd.selectedColumn)
                        if (y + fontHeight > menuData->gd.selectedRow && y <= menuData->gd.selectedRow)
                        {
                            DoInsert(menuData, &(*items)->popup, code, *items);
                            return 2;
                        }
                    return 0;
                }
            }
            y += fontHeight;
            items = &(*items)->next;
        }
    }
    else
    {
        while (*items)
        {
            if (y + fontHeight > menuData->gd.selectedRow)
            {
                DoInsert(menuData, items, code, NULL);
                return 1;
            }
            y += fontHeight;
            items = &(*items)->next;
        }
        if (y + fontHeight > menuData->gd.selectedRow)
        {
            DoInsert(menuData, items, code, NULL);
            return 2;
        }
    }
    return 0;
}
static int InsertDeleteFromTopRow(HDC hDC, struct resRes* menuData, MENUITEM** items, int fontHeight, int code)
{
    int x = 10;
    SIZE xx;
    while (*items)
    {

        GetTextExtentPoint32W(hDC, (*items)->text, wcslen((*items)->text), &xx);
        if ((*items)->expanded && menuData->gd.selectedRow > 10 + fontHeight)
        {
            if (menuData->gd.selectedColumn >= x)
                return InsertDeleteFromColumns(hDC, menuData, &(*items)->popup, x, 10 + fontHeight, fontHeight, code);
            x += xx.cx + 16;
        }
        else if (menuData->gd.selectedRow < 10 + fontHeight)
        {
            if (menuData->gd.selectedColumn < x + xx.cx + 16)
            {
                DoInsert(menuData, items, code, NULL);
                return 1;
            }
            x += xx.cx + 16;
        }
        else
        {
            x += xx.cx + 16;
        }
        items = &(*items)->next;
    }
    if (menuData->gd.selectedColumn < x + xx.cx + 16)
    {
        DoInsert(menuData, items, code, NULL);
        return 2;
    }
    return 0;
}
void InsertDelete(HWND hwnd, struct resRes* menuData, int code)
{
    HFONT font;
    int fontHeight;
    HDC hDC = GetDC(hwnd);
    fontHeight = (systemMenuFont.lfHeight < 0 ? -systemMenuFont.lfHeight : systemMenuFont.lfHeight) + 6;
    font = CreateFontIndirect(&systemMenuFont);
    font = SelectObject(hDC, font);
    InsertDeleteFromTopRow(hDC, menuData, &menuData->resource->u.menu->items, fontHeight, code);
    font = SelectObject(hDC, font);
    DeleteObject(font);
    ReleaseDC(hwnd, hDC);
}
LRESULT CALLBACK MenuDrawProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    static MENUITEM* start;
    static BOOL dragging;
    static HCURSOR oldcurs;
    static struct resRes* resData;
    static struct propertyFuncs* resFuncs;
    POINT mouse;
    PAINTSTRUCT paint;
    LPCREATESTRUCT createStruct;
    struct resRes* menuData;
    int i;
    struct menuUndo* undo;
    switch (iMessage)
    {
        case WM_MDIACTIVATE:
            if ((HWND)lParam == hwnd)
            {
                doMaximize();
            }
            break;
        case WM_SETFOCUS:
            menuData = (struct resRes*)GetWindowLong(hwnd, 0);
            SetResourceProperties(resData, resFuncs);
            break;
        case EM_CANUNDO:
            menuData = (struct resRes*)GetWindowLong(hwnd, 0);
            return menuData->gd.undoData != NULL;
        case WM_KEYDOWN:
            menuData = (struct resRes*)GetWindowLong(hwnd, 0);
            switch (wParam)
            {
                case 'S':
                    if (GetKeyState(VK_CONTROL) & 0x80000000)
                    {
                        PostMessage(hwnd, WM_COMMAND, IDM_SAVE, 0);
                    }
                    break;
                case 'Z':
                    if (GetKeyState(VK_CONTROL) & 0x80000000)
                    {
                        PostMessage(hwnd, WM_COMMAND, IDM_UNDO, 0);
                    }
                    break;
            }
            switch (KeyboardToAscii(wParam, lParam, FALSE))
            {
                case '[':
                    if (GetKeyState(VK_CONTROL) & 0x80000000)
                    {
                        PopupResFullScreen(hwnd);
                        return 0;
                    }
                    break;
                case ']':
                    if (GetKeyState(VK_CONTROL) & 0x80000000)
                    {
                        ReleaseResFullScreen(hwnd);
                        return 0;
                    }
                    break;
            }
            break;
        case WM_NCACTIVATE:
            PaintMDITitleBar(hwnd, iMessage, wParam, lParam);
            return TRUE;
        case WM_NCPAINT:
            return PaintMDITitleBar(hwnd, iMessage, wParam, lParam);
        case WM_CREATE:
            SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
            createStruct = (LPCREATESTRUCT)lParam;
            menuData = (struct resRes*)((LPMDICREATESTRUCT)(createStruct->lpCreateParams))->lParam;
            SetWindowLong(hwnd, 0, (long)menuData);
            menuData->activeHwnd = hwnd;
            resData = menuData;
            resFuncs = &menuFuncs;
            MarkUnexpanded(menuData->resource->u.menu->items);
            InsertRCWindow(hwnd);
            break;
        case WM_CLOSE:
            RemoveRCWindow(hwnd);
            SendMessage(hwndSrcTab, TABM_REMOVE, 0, (LPARAM)hwnd);
            break;
        case WM_DESTROY:
            menuData = (struct resRes*)GetWindowLong(hwnd, 0);
            menuData->activeHwnd = NULL;
            undo = menuData->gd.undoData;
            menuData->gd.undoData = NULL;
            if (undo)
                menuData->gd.cantClearUndo = TRUE;
            while (undo)
            {
                struct menuUndo* next = undo->next;
                free(undo);
                undo = next;
            }
            break;
            //        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
        case WM_RBUTTONUP:
            SetFocus(hwnd);
            return 1;
        case WM_ERASEBKGND:
            return 1;
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
            FillRect(hdouble, &rect, (HBRUSH)(COLOR_APPWORKSPACE + 1));
            menuData = (struct resRes*)GetWindowLong(hwnd, 0);
            DoPaint(hwnd, hdouble, &paint, &rect, menuData);
            BitBlt(hDC, 0, 0, rect.right, rect.bottom, hdouble, 0, 0, SRCCOPY);
            DeleteObject(bitmap);
            DeleteObject(hdouble);
            EndPaint(hwnd, &ps);
        }
            return 0;
        case WM_RBUTTONDOWN:
            menuData = (struct resRes*)GetWindowLong(hwnd, 0);
            SendMessage(hwnd, WM_COMMAND, ID_EDIT + (EN_KILLFOCUS << 16), 0);
            mouse.x = LOWORD(lParam);
            mouse.y = HIWORD(lParam);
            i = menuHitTest(hwnd, menuData, mouse);
            if (i)
            {
                HMENU menu, popup;
                SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_EDIT, CBN_KILLFOCUS), 0);
                menu = LoadMenuGeneric(hInstance, "RESMENUMENU");
                if (i & 2)
                    EnableMenuItem(menu, IDM_DELETE, MF_BYCOMMAND | MF_GRAYED);
                if (i & 4)
                    EnableMenuItem(menu, IDM_INSERT_SEPARATOR, MF_BYCOMMAND | MF_GRAYED);
                menuData->gd.selectedRow = mouse.y + menuData->gd.scrollPos.y;
                menuData->gd.selectedColumn = mouse.x + menuData->gd.scrollPos.x;
                popup = GetSubMenu(menu, 0);
                ClientToScreen(hwnd, &mouse);
                InsertBitmapsInMenu(popup);
                TrackPopupMenuEx(popup, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_LEFTBUTTON, mouse.x, mouse.y, hwnd, NULL);
                DestroyMenu(menu);
            }
            return 1;
        case WM_LBUTTONDBLCLK:
            SendMessage(hwnd, WM_COMMAND, ID_EDIT + (EN_KILLFOCUS << 16), 0);
            SetFocus(hwnd);
            menuData = (struct resRes*)GetWindowLong(hwnd, 0);
            mouse.x = LOWORD(lParam);
            mouse.y = HIWORD(lParam);
            SelectSubmenu(hwnd, menuData, mouse, TRUE);
            break;
        case WM_MOUSEMOVE:
            if (dragging)
            {
                menuData = (struct resRes*)GetWindowLong(hwnd, 0);
                mouse.x = LOWORD(lParam);
                mouse.y = HIWORD(lParam);
                menuData->gd.selectedMenu = start;
                i = menuHitTest(hwnd, menuData, mouse);
                if (!oldcurs)
                    oldcurs = GetCursor();
                SetCursor(i & 8 ? dragCur : noCur);
            }
            break;
        case WM_LBUTTONDOWN:
            SetFocus(hwnd);
            menuData = (struct resRes*)GetWindowLong(hwnd, 0);
            SendMessage(hwnd, WM_COMMAND, ID_EDIT + (EN_KILLFOCUS << 16), 0);
            mouse.x = LOWORD(lParam);
            mouse.y = HIWORD(lParam);
            i = menuHitTest(hwnd, menuData, mouse);
            if (i & 1)
            {
                resData = menuData;
                resFuncs = &menuItemFuncs;
                start = menuData->gd.selectedMenu;
                dragging = TRUE;
                SetCapture(hwnd);
            }
            else
            {
                resData = menuData;
                resFuncs = &menuFuncs;
            }
            SetResourceProperties(resData, resFuncs);
            break;
        case WM_LBUTTONUP:
            if (dragging)
            {
                dragging = FALSE;
                ReleaseCapture();
            }
            menuData = (struct resRes*)GetWindowLong(hwnd, 0);
            if (oldcurs)
            {
                if (GetCursor() == dragCur)
                {
                    mouse.x = LOWORD(lParam);
                    mouse.y = HIWORD(lParam);
                    i = menuHitTest(hwnd, menuData, mouse);
                    DoMove(menuData, start, menuData->gd.selectedMenu);
                }
                SetCursor(oldcurs);
                oldcurs = NULL;
            }
            else
            {
                SendMessage(hwnd, WM_COMMAND, ID_EDIT + (EN_KILLFOCUS << 16), 0);
                SetFocus(hwnd);
                mouse.x = LOWORD(lParam);
                mouse.y = HIWORD(lParam);
                SelectSubmenu(hwnd, menuData, mouse, FALSE);
            }
            break;
        case WM_COMMAND:
            menuData = (struct resRes*)GetWindowLong(hwnd, 0);
            switch (LOWORD(wParam))
            {
                case ID_EDIT:
                    if (HIWORD(wParam) == EN_KILLFOCUS)
                    {
                        static BOOL inKillFocus;
                        if (menuData->gd.editWindow && !inKillFocus)
                        {
                            char buf[256];
                            char buf2[256];
                            UndoChange(menuData, menuData->gd.selectedMenu);
                            buf[GetWindowText(menuData->gd.editWindow, buf, sizeof(buf))] = 0;
                            StringWToA(buf2, menuData->gd.selectedMenu->text, wcslen(menuData->gd.selectedMenu->text));
                            if (strcmp(buf, buf2))
                            {
                                ResGetHeap(workArea, menuData);
                                if (menuData->gd.selectedMenu->id)
                                    ResGetMenuItemName(menuData->gd.selectedMenu->id, buf);
                                StringAsciiToWChar(&menuData->gd.selectedMenu->text, buf, strlen(buf));
                                ResSetDirty(menuData);
                            }
                            InvalidateRect(hwnd, 0, FALSE);
                            inKillFocus = TRUE;
                            DestroyWindow(menuData->gd.editWindow);
                            inKillFocus = FALSE;
                            menuData->gd.editWindow = NULL;
                        }
                    }
                    break;
                case IDM_DELETE:
                case IDM_INSERT:
                case IDM_INSERT_SEPARATOR:
                    InsertDelete(hwnd, menuData, LOWORD(wParam));
                    break;
                case IDM_SAVE:
                    if (menuData->resource->changed)
                    {
                        ResSaveCurrent(workArea, menuData);
                    }
                    break;
                case IDM_UNDO:
                    DoUndo(menuData);
                    break;
            }
            break;
        case WM_VSCROLL:
            menuData = (struct resRes*)GetWindowLong(hwnd, 0);
            switch (LOWORD(wParam))
            {
                case SB_BOTTOM:
                    menuData->gd.scrollPos.y = menuData->gd.scrollMax.y;
                    break;
                case SB_TOP:
                    menuData->gd.scrollPos.y = 0;
                    break;
                case SB_LINEDOWN:
                    menuData->gd.scrollPos.y += 8;
                    break;
                case SB_LINEUP:
                    menuData->gd.scrollPos.y -= 8;
                    break;
                case SB_PAGEDOWN:
                    menuData->gd.scrollPos.y += 64;
                    break;
                case SB_PAGEUP:
                    menuData->gd.scrollPos.y -= 64;
                    break;
                case SB_ENDSCROLL:
                    return 0;
                case SB_THUMBPOSITION:
                case SB_THUMBTRACK:
                {
                    SCROLLINFO si;
                    memset(&si, 0, sizeof(si));
                    si.cbSize = sizeof(si);
                    si.fMask = SIF_TRACKPOS;
                    GetScrollInfo(hwnd, SB_VERT, &si);
                    menuData->gd.scrollPos.y = si.nTrackPos;
                }
                break;
                default:
                    return 0;
            }
            if (menuData->gd.scrollPos.y < 0)
                menuData->gd.scrollPos.y = 0;
            if (menuData->gd.scrollPos.y >= menuData->gd.scrollMax.y)
                menuData->gd.scrollPos.y = menuData->gd.scrollMax.y;
            SetScrollPos(hwnd, SB_VERT, menuData->gd.scrollPos.y, TRUE);
            InvalidateRect(hwnd, 0, FALSE);
            return 0;
        case WM_HSCROLL:
            menuData = (struct resRes*)GetWindowLong(hwnd, 0);
            switch (LOWORD(wParam))
            {
                case SB_LEFT:
                    menuData->gd.scrollPos.x = 0;
                    break;
                case SB_RIGHT:
                    menuData->gd.scrollPos.x = menuData->gd.scrollMax.x;
                    break;
                case SB_LINELEFT:
                    menuData->gd.scrollPos.x -= 8;
                    break;
                case SB_LINERIGHT:
                    menuData->gd.scrollPos.x += 8;
                    break;
                case SB_PAGERIGHT:
                    menuData->gd.scrollPos.x += 64;
                    break;
                case SB_PAGELEFT:
                    menuData->gd.scrollPos.x -= 64;
                    break;
                case SB_ENDSCROLL:
                    return 0;
                case SB_THUMBPOSITION:
                case SB_THUMBTRACK:
                {
                    SCROLLINFO si;
                    memset(&si, 0, sizeof(si));
                    si.cbSize = sizeof(si);
                    si.fMask = SIF_TRACKPOS;
                    GetScrollInfo(hwnd, SB_HORZ, &si);
                    menuData->gd.scrollPos.x = si.nTrackPos;
                }
                break;
            }
            if (menuData->gd.scrollPos.x < 0)
                menuData->gd.scrollPos.x = 0;
            if (menuData->gd.scrollPos.x >= menuData->gd.scrollMax.x)
                menuData->gd.scrollPos.x = menuData->gd.scrollMax.x;
            SetScrollPos(hwnd, SB_HORZ, menuData->gd.scrollPos.x, TRUE);
            SetWindowPos(menuData->gd.childWindow, NULL, menuData->gd.origin.x - menuData->gd.scrollPos.x,
                         menuData->gd.origin.y - menuData->gd.scrollPos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

            InvalidateRect(hwnd, 0, FALSE);

            return 0;
        case WM_SIZE:
            InvalidateRect(hwnd, 0, FALSE);
            break;
        default:
            break;
    }
    return DefMDIChildProc(hwnd, iMessage, wParam, lParam);
}

void RegisterMenuDrawWindow(HINSTANCE hInstance)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = &MenuDrawProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(void*) * 2;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE + 1);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szMenuDrawClassName;
    RegisterClass(&wc);
    arrImage = LoadImage(hInstance, "ID_RIGHTARR", IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
    ChangeBitmapColor(arrImage, 0xc0c0c0, RetrieveSysColor(COLOR_BTNFACE));
    dragCur = LoadCursor(hInstance, "ID_DRAGCTL");
    noCur = LoadCursor(hInstance, "ID_NODRAGCUR");
}
void CreateMenuDrawWindow(struct resRes* info)
{
    char name[512];
    int maximized;
    HWND hwnd;
    sprintf(name, "%s - %s", szUntitled, info->name);
    SendMessage(hwndClient, WM_MDIGETACTIVE, 0, (LPARAM)&maximized);
    hwnd =
        CreateMDIWindow(szMenuDrawClassName, name,
                        WS_VISIBLE | WS_CHILD | WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | MDIS_ALLCHILDSTYLES | WS_CLIPSIBLINGS |
                            WS_CLIPCHILDREN | WS_SIZEBOX | (PropGetInt(NULL, "TABBED_WINDOWS") ? WS_MAXIMIZE : WS_SYSMENU),
                        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwndClient, hInstance, (LPARAM)info);
    if (hwnd)
    {
        SendMessage(hwndSrcTab, TABM_ADD, (WPARAM)name, (LPARAM)hwnd);
        if (info->resource->changed)
            ResRewriteTitle(info, TRUE);
    }
}