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
extern PROJECTITEM *workArea;

static char *szMenuDrawClassName = "xccMenuDrawClass";
static char *szUntitled = "Menu";
static HBITMAP arrImage;
static HCURSOR dragCur, noCur;

HANDLE ResGetHeap(PROJECTITEM *wa, struct resRes *data);
EXPRESSION *ResAllocateMenuId();

struct menuUndo
{
    struct menuUndo *next;
    enum { mu_changed, mu_insert, mu_delete, mu_move} type;
    MENUITEM **place;
    MENUITEM *item;
    WCHAR *text;
    EXPRESSION *id;
    EXPRESSION *state;
    EXPRESSION *help;
    int flags;
} ;
static void InsertMenuProperties(HWND lv, struct resRes *data);
static void GetMenuPropText(char *buf, HWND lv, struct resRes *data, int row);
static HWND MenuPropStartEdit(HWND lv, int row, struct resRes *data);
static void MenuPropEndEdit(HWND lv, int row, HWND editWnd, struct resRes *data);
struct propertyFuncs menuFuncs = //FIXME what is it ?
{ 
    InsertMenuProperties,
    GetMenuPropText,
    MenuPropStartEdit,
    MenuPropEndEdit
} ;
static void InsertMenuItemProperties(HWND lv, struct resRes *data);
static void GetMenuItemPropText(char *buf, HWND lv, struct resRes *data, int row);
static HWND MenuItemPropStartEdit(HWND lv, int row, struct resRes *data);
static void MenuItemPropEndEdit(HWND lv, int row, HWND editWnd, struct resRes *data);
struct propertyFuncs menuItemFuncs = 
{ 
    InsertMenuItemProperties,
    GetMenuItemPropText,
    MenuItemPropStartEdit,
    MenuItemPropEndEdit
} ;
void SetSeparatorFlag(MENU *menu, MENUITEM *mi, BOOL issep)
{
    if (menu->extended)
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
void RecurseSetSeparator(MENU *menu, MENUITEM *item, BOOL issep)
{
    if (item)
    {
        if (item->next)
            RecurseSetSeparator(menu, item->next, issep);
        if (item->popup)
            RecurseSetSeparator(menu, item->popup, issep);
        if (!item->text)
        {
            SetSeparatorFlag(menu, item, issep);
        }
    }    
}
static void InsertMenuProperties(HWND lv, struct resRes *data)
{
    PropSetGroup(lv, 101, L"Menu Characteristics");
    PropSetGroup(lv, 102, L"General Characteristics");
    PropSetItem(lv, 0, 101, "Extended (MENUEX)");
    PropSetItem(lv, 1, 102, "Resource Id");
    PropSetItem(lv, 2, 102, "Language");
    PropSetItem(lv, 3, 102, "SubLanguage");
    PropSetItem(lv, 4, 102, "Characteristics");
    PropSetItem(lv, 5, 102, "Version");    
}
void GetMenuPropText(char *buf, HWND lv, struct resRes *data, int row)
{
    if (row >= 1)
    {
        GetAccPropText(buf, lv, data, row-1);
    }
    else
    {
        if (data->resource->u.menu->extended)
            sprintf(buf, "Yes");
        else
            sprintf(buf, "No");
    }
}
HWND MenuPropStartEdit(HWND lv, int row, struct resRes *data)
{
    if (row >= 1)
    {
        return AccPropStartEdit(lv, row-1, data);
    }
    else
    {
        HWND rv = PropGetHWNDCombobox(lv);
        SendMessage(rv, CB_ADDSTRING, 0, (LPARAM)"No");
        SendMessage(rv, CB_ADDSTRING, 0, (LPARAM)"Yes");
        SendMessage(rv, CB_SETCURSEL, data->resource->u.menu->extended ? 1 : 0, 0);
        return rv;
    }
}
void MenuPropEndEdit(HWND lv, int row, HWND editWnd, struct resRes *data)
{
    if (row >= 1)
    {
        AccPropEndEdit(lv, row-1, editWnd, data);
    }
    else
    {
        int n = SendMessage(editWnd, CB_GETCURSEL, 0, 0);
        if (n != CB_ERR)
        {
            RecurseSetSeparator(data->resource->u.menu, data->resource->u.menu->items, FALSE);
            data->resource->u.menu->extended = !!n;
            RecurseSetSeparator(data->resource->u.menu, data->resource->u.menu->items, TRUE);
            ResSetDirty(data);
        }
    }
    DestroyWindow(editWnd);
    ListView_SetItemText(lv, row, 1, LPSTR_TEXTCALLBACK);
    SetFocus(data->activeHwnd);
}
static void InsertMenuItemProperties(HWND lv, struct resRes *data)
{
    PropSetGroup(lv, 101, L"Menu Item Characteristics");
    PropSetItem(lv, 0, 101, "Menu Identifier");
    PropSetItem(lv, 1, 101, "Text");
}
void GetMenuItemPropText(char *buf, HWND lv, struct resRes *data, int row)
{
    buf[0] = 0;
    switch (row)
    {
        case 0:
            FormatExp(buf, data->gd.selectedMenu->id);
            break;        
        case 1:
            StringWToA(buf, data->gd.selectedMenu->text, wcslen(data->gd.selectedMenu->text));
            break;
    }
}
HWND MenuItemPropStartEdit(HWND lv, int row, struct resRes *data)
{
    HWND rv = PropGetHWNDText(lv);
    if (rv)
    {
        char buf[256];
        GetMenuItemPropText(buf, lv, data, row);
        SendMessage(rv, WM_SETTEXT, 0, (LPARAM)buf);
    }
    return rv;
}
void MenuItemPropEndEdit(HWND lv, int row, HWND editWnd, struct resRes *data)
{
    char buf[256];
    char buf1[256];
    SendMessage(editWnd, WM_GETTEXT, sizeof(buf), (LPARAM)buf);
    GetMenuItemPropText(buf1, lv, data, row);
    if (strcmp(buf, buf1))
    {
        switch(row)
        {
            case 0:
                PropSetIdName(data, buf, &data->gd.selectedMenu->id, NULL);
                break;
            case 1:
                StringAsciiToWChar(& data->gd.selectedMenu->text, buf, strlen(buf));
                InvalidateRect(data->activeHwnd, 0, 0);
                break;
        }
        ResSetDirty(data);
    }
    DestroyWindow(editWnd);
    ListView_SetItemText(lv, row, 1, LPSTR_TEXTCALLBACK);
    SetFocus(data->activeHwnd);
}
static int CalculateMenuWidth(HDC hDC, MENUITEM *items)
{
    int maxWidth = 0;
    SIZE sz;
    while (items)
    {
        if (items->text)
        {
            GetTextExtentPoint32W(hDC, items->text, wcslen(items->text), &sz);
            if (sz.cx > maxWidth)
                maxWidth = sz.cx;
        }
        items = items->next;
    }
    if (maxWidth < 64)
        maxWidth = 64;
    return maxWidth;
}
static void GetSize(HDC hDC, MENUITEM *items, int x, int y, int fontHeight, LPSIZE sz)
{
    int width = CalculateMenuWidth(hDC, items) + 28;
    int pos = y;
    while (items)
    {
        if (items->popup && items->expanded)
        {
            GetSize(hDC, items->popup, x+width, pos, fontHeight, sz);
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
static void GetSizeTopRow(HDC hDC, MENUITEM *items, int x, int y, int fontHeight, LPSIZE sz)
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
    x += xx.cx; // for the shim
    if (x > sz->cx)
        sz->cx = x;
    if (fontHeight > sz->cy)
        sz->cy = fontHeight;
}
static void DrawItem(HDC hDC, WCHAR *text, BOOL selected, int bg, int x, int y, int width, int height)
{
    RECT r1;
    r1.left = x;
    r1.top = y;
    r1.right = x+width;
    r1.bottom = y + height;
    FillRect(hDC,&r1, (HBRUSH)(bg + 1));
    
    MoveToEx(hDC, x, y, NULL);
    LineTo(hDC, x+width, y);
    LineTo(hDC, x+width, y+height);
    LineTo(hDC, x, y+height);
    LineTo(hDC, x, y);
    
    if (text)
    {
        SetBkColor(hDC, GetSysColor(bg));
        TextOutW(hDC, x+3, y+1, text, wcslen(text));
    }
    else // separator
    {
        MoveToEx(hDC, x+5, y+height/2-1, NULL);
        LineTo(hDC, x+width-5, y+height/2-1);
        MoveToEx(hDC, x+5, y+height/2, NULL);
        LineTo(hDC, x+width-5, y+height/2);
    }
    if (selected)
    {
        HPEN *pen = CreatePen(PS_DOT, 0, 0);
        pen = SelectObject(hDC, pen);
        MoveToEx(hDC, x+1, y+1, NULL);
        LineTo(hDC, x+width-1, y+1);
        LineTo(hDC, x+width-1, y+height-1);
        LineTo(hDC, x+1, y+height-1);
        LineTo(hDC, x+1, y+1);
        pen = SelectObject(hDC, pen);
        DeleteObject(pen);
        
    }
}
static void PaintColumn(HDC hDC, HDC arrow, MENUITEM *items, MENUITEM *selected, int x, int y, HFONT font, int fontHeight)
{
    MENUITEM *orig = items;
    int width = CalculateMenuWidth(hDC, items) + 28;
    int pos = y;
    BOOL submenu = FALSE;
    while (items)
    {
        DrawItem(hDC, items->text, selected == items,COLOR_BTNFACE, x, pos, width, fontHeight);
        if (items->popup)
        {
            if (items->expanded)
                submenu = TRUE;
            BitBlt(hDC, x + width - 10, pos + (fontHeight - 8)/2, 8,8,arrow, 0, 0, SRCCOPY);
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
                PaintColumn(hDC, arrow, items->popup, selected, x+width, pos, font, fontHeight);
            }
            pos += fontHeight;
            items = items->next;
        }
    }
    else
    {
        DrawItem(hDC, L"<---->", FALSE, COLOR_WINDOW, x, pos, width, fontHeight);
        items = orig;
        pos = y;
        while (items)
        {
            if (items->expanded)
            {
                DrawItem(hDC, L"<---->", FALSE, COLOR_WINDOW, x+width, pos, width, fontHeight);
            }
            pos += fontHeight;
            items = items->next;
        }
    }
}
static void PaintTopRow(HDC hDC, HDC arrow, MENUITEM *items, MENUITEM *selected, int x, int y, HFONT font, int fontHeight)
{
    MENUITEM *orig = items;
    int orig_x = x;
    SIZE xx;
    xx.cx =40;
    while (items)
    {
        GetTextExtentPoint32W(hDC, items->text, wcslen(items->text), &xx);
        xx.cx += 16;
        DrawItem(hDC, items->text, selected == items, COLOR_BTNFACE, x, y, xx.cx, fontHeight);
        x += xx.cx;
        items = items->next;
    }
    DrawItem(hDC, L"<---->", FALSE, COLOR_WINDOW, x, y, xx.cx, fontHeight);    
    items = orig;
    x = orig_x;
    while (items)
    {
        SIZE xx;
        if (items->expanded)
            PaintColumn(hDC, arrow, items->popup, selected, x, y+fontHeight, font, fontHeight);
        GetTextExtentPoint32W(hDC, items->text, wcslen(items->text), &xx);
        xx.cx += 16;
        x += xx.cx;
        items = items->next;
    }
    
}
static void DoPaint(HWND hwnd, HDC hDC, LPPAINTSTRUCT lpPaint, RECT *r, struct resRes *menuData)
{
    RECT r1;
    NONCLIENTMETRICS m;
    HFONT font;
    SIZE sz;
    HDC compatDC, arrowDC;
    HBITMAP compatBitmap, arrowBitmap;
    int fontHeight;
    m.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS),&m, NULL);
    fontHeight = (m.lfMenuFont.lfHeight < 0 ? - m.lfMenuFont.lfHeight : m.lfMenuFont.lfHeight)+6;
    font = CreateFontIndirect(&m.lfMenuFont);
    compatDC = CreateCompatibleDC(hDC);
    arrowDC = CreateCompatibleDC(hDC);
    font = SelectObject(compatDC, font);
    sz.cx = 0; 
    sz.cy = 0;
    GetSizeTopRow(compatDC, menuData->resource->u.menu->items, 0, 0, fontHeight, &sz);
    sz.cx +=GetSystemMetrics(SM_CXBORDER) * 2;
    sz.cy +=GetSystemMetrics(SM_CYBORDER) * 2;
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
    FillRect(compatDC,&r1, (HBRUSH)(COLOR_APPWORKSPACE + 1));
    SetTextColor(compatDC, GetSysColor(COLOR_WINDOWTEXT));
    PaintTopRow(compatDC, arrowDC, menuData->resource->u.menu->items, menuData->gd.selectedMenu, 0, 0, font, fontHeight);
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
    BitBlt(hDC, r->left+10, r->top+10, r->right+10, r->bottom+10,
           compatDC, menuData->gd.scrollPos.x, menuData->gd.scrollPos.y, SRCCOPY);
    compatBitmap = SelectObject(compatDC, compatBitmap);
    arrowBitmap = SelectObject(arrowDC, arrowBitmap);
    DeleteObject(compatBitmap);
    DeleteDC(compatDC);
    DeleteDC(arrowDC);
    DeleteObject(font);
}
static void UndoChange(struct resRes *menuData, MENUITEM *item)
{
    struct menuUndo *newUndo = calloc(1, sizeof(struct menuUndo));
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
static void UndoDelete(struct resRes *menuData, MENUITEM **item)
{
    struct menuUndo *newUndo = calloc(1, sizeof(struct menuUndo));
    if (newUndo)
    {
        newUndo->type = mu_delete;
        newUndo->place = item;
        newUndo->item = *item;
        newUndo->next = menuData->gd.undoData;
        menuData->gd.undoData = newUndo;
    }
}
static void UndoInsert(struct resRes *menuData, MENUITEM **item)
{
    struct menuUndo *newUndo = calloc(1, sizeof(struct menuUndo));
    if (newUndo)
    {
        newUndo->type = mu_insert;
        newUndo->place = item;
        newUndo->next = menuData->gd.undoData;
        menuData->gd.undoData = newUndo;
    }
}
static MENUITEM **GetItemPlace(MENUITEM **top, MENUITEM *item)
{
    while (*top)
    {
        if (*top == item)
            return top;
        if ((*top)->popup)
        {
            MENUITEM **rv = GetItemPlace(&(*top)->popup, item);
            if (rv)
                return rv;
        }
        top = &(*top)->next;
    }
    return NULL;
}
static void UndoMove(struct resRes *menuData, MENUITEM **src, MENUITEM *item)
{
    struct menuUndo *newUndo = calloc(1, sizeof(struct menuUndo));
    if (newUndo)
    {
        newUndo->type = mu_move;
        newUndo->place = src;
        newUndo->item = item;
        newUndo->next = menuData->gd.undoData;
        menuData->gd.undoData = newUndo;
    }
}
static void DoUndo(struct resRes * menuData)
{
    struct menuUndo *undo= menuData->gd.undoData;
    if (undo)
    {
        menuData->gd.undoData = undo->next;
        switch (undo->type)
        {
            MENUITEM **place;
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
                place = GetItemPlace(&menuData->resource->u.menu->items, undo->item);
                *place = (*place)->next;
                undo->item->next = *undo->place;               
                *undo->place = undo->item;
                break;
        }
        
        InvalidateRect(menuData->activeHwnd, 0 , FALSE);
        free(undo);
        if (menuData->gd.cantClearUndo)
            ResSetDirty(menuData);
        else if (!menuData->gd.undoData)
            ResSetClean(menuData);
    }
}
static void MarkUnexpanded(MENUITEM *items)
{
    while (items)
    {
        if (items->popup)
            MarkUnexpanded(items->popup);
        items->expanded = FALSE;
        items = items->next;
    }
}
static void SelectItem(struct resRes *menuData, MENUITEM *orig, MENUITEM *selected, 
                       BOOL openEditor, int x, int y, int width, int height)
{
    MarkUnexpanded(orig);
    selected->expanded = TRUE;
    menuData->gd.selectedMenu = selected;
    if (openEditor)
    {
        char buf[512];
        NONCLIENTMETRICS m;
        HFONT font;
        m.cbSize = sizeof(NONCLIENTMETRICS);
        SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS),&m, NULL);
        font = CreateFontIndirect(&m.lfMenuFont);
        StringWToA(buf, selected->text);
        menuData->gd.editWindow = CreateWindow("edit", "", WS_VISIBLE |
            WS_CHILD | WS_CLIPSIBLINGS | WS_BORDER | ES_AUTOHSCROLL,
            x,y,width,height, menuData->activeHwnd, (HMENU)ID_EDIT,
            hInstance, NULL);
        SendMessage(menuData->gd.editWindow, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessage(menuData->gd.editWindow, WM_SETTEXT, 0, (LPARAM)buf);
        AccSubclassEditWnd(menuData->activeHwnd, menuData->gd.editWindow);
        SendMessage(menuData->gd.editWindow, EM_SETSEL, 0, -1);
        SetFocus(menuData->gd.editWindow);
    }
}
static BOOL SelectFromColumns(HDC hDC, struct resRes * menuData,
                              MENUITEM *items, int x, int y, int fontHeight, 
                              POINT mouse, BOOL makeEditor)
{
    MENUITEM *orig = items;
    int width = CalculateMenuWidth(hDC, orig) + 28;
    if (x +width < mouse.x)
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
static BOOL SelectFromTopRow(HDC hDC, struct resRes *menuData,
                             MENUITEM *items, int fontHeight, POINT mouse, BOOL makeEditor)
{
    MENUITEM *orig = items;
    int x = 10;
    SIZE xx;
    while (items)
    {

        GetTextExtentPoint32W(hDC, items->text, wcslen(items->text), &xx);
        if (items->expanded && mouse.y > 10 + fontHeight)
        {
            if (mouse.x >= x)
                return SelectFromColumns(hDC, menuData, items->popup, x, 10+fontHeight, fontHeight, mouse, makeEditor);
            x += xx.cx+16;
        }
        else if (mouse.y < 10 + fontHeight)
        {        
            if (mouse.x < x + xx.cx + 16)
            {
                break;
            }
            x += xx.cx+16;
        }
        else
        {
            x += xx.cx+16;
        }
        items = items->next;
    }
    if (items)
    {
        SelectItem(menuData, orig, items, makeEditor, x, 10, xx.cx+16, fontHeight);
        return TRUE;
    }
    return FALSE;
}
static void SelectSubmenu(HWND hwnd, struct resRes *menuData, POINT mouse, BOOL openEditor)
{
    NONCLIENTMETRICS m;
    HFONT font;
    int fontHeight;
    HDC hDC = GetDC(hwnd);
    m.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS),&m, NULL);
    fontHeight = (m.lfMenuFont.lfHeight < 0 ? - m.lfMenuFont.lfHeight : m.lfMenuFont.lfHeight)+6;
    font = CreateFontIndirect(&m.lfMenuFont);
    font = SelectObject(hDC, font);
    mouse.x += menuData->gd.scrollPos.x;
    mouse.y += menuData->gd.scrollPos.y;
    if (SelectFromTopRow(hDC, menuData, menuData->resource->u.menu->items, fontHeight, mouse, openEditor))
        InvalidateRect(hwnd, 0, FALSE);
    font = SelectObject(hDC, font);
    DeleteObject(font);
    ReleaseDC(hwnd, hDC);
}
static int HitTestFromColumns(HDC hDC, struct resRes *menuData, MENUITEM *items, int x, int y, int fontHeight, 
                              POINT mouse)
{
    MENUITEM *orig = items;
    int width = CalculateMenuWidth(hDC, orig) + 28;
    if (x +width < mouse.x)
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
static int HitTestFromTopRow(HDC hDC, struct resRes *menuData,MENUITEM *items, int fontHeight, POINT mouse)
{
    MENUITEM *orig = items;
    int x = 10;
    SIZE xx;
    while (items)
    {

        GetTextExtentPoint32W(hDC, items->text, wcslen(items->text), &xx);
        if (items->expanded && mouse.y > 10 + fontHeight)
        {
            if (mouse.x >= x)
                return HitTestFromColumns(hDC, menuData, items->popup, x, 10+fontHeight, fontHeight, mouse);
            x += xx.cx+16;
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
            x += xx.cx+16;
        }
        else
        {
            x += xx.cx+16;
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
int menuHitTest(HWND hwnd, struct resRes *menuData, POINT mouse)
{
    int rv;
    NONCLIENTMETRICS m;
    HFONT font;
    int fontHeight;
    HDC hDC = GetDC(hwnd);
    m.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS),&m, NULL);
    fontHeight = (m.lfMenuFont.lfHeight < 0 ? - m.lfMenuFont.lfHeight : m.lfMenuFont.lfHeight)+6;
    font = CreateFontIndirect(&m.lfMenuFont);
    font = SelectObject(hDC, font);
    mouse.x += menuData->gd.scrollPos.x;
    mouse.y += menuData->gd.scrollPos.y;
    rv = HitTestFromTopRow(hDC, menuData, menuData->resource->u.menu->items, fontHeight, mouse);
    font = SelectObject(hDC, font);
    DeleteObject(font);
    ReleaseDC(hwnd, hDC);
    return rv;
}
static void DoMove(struct resRes *menuData, MENUITEM *item, MENUITEM *dest)
{
    if (item != dest)
    {
        MENUITEM **itemplace = GetItemPlace(&menuData->resource->u.menu->items, item);
        MENUITEM **destplace;
        if (dest)
        {
            destplace = GetItemPlace(&menuData->resource->u.menu->items, dest);
        }
        else
        {
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
static void DoInsert(struct resRes *menuData, MENUITEM **items, int code, MENUITEM *parent)
{
    switch (code)
    {
        MENUITEM *newItem;
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
            SetSeparatorFlag(menuData->resource->u.menu, newItem, TRUE);
            *items = newItem;
            UndoInsert(menuData, items);
            ResSetDirty(menuData);
            InvalidateRect(menuData->activeHwnd, 0, FALSE);
            break;
    }
}
static int InsertDeleteFromColumns(HDC hDC, struct resRes *menuData,
                                   MENUITEM **items, int x, int y, int fontHeight, 
                                    int code)
{
    MENUITEM **orig = items;
    int width = CalculateMenuWidth(hDC, *orig) + 28;
    if (x +width < menuData->gd.selectedColumn)
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
static int InsertDeleteFromTopRow(HDC hDC, struct resRes *menuData,
                                  MENUITEM **items, int fontHeight, int code)
{
    MENUITEM **orig = (*items);
    int x = 10;
    SIZE xx;
    while (*items)
    {

        GetTextExtentPoint32W(hDC, (*items)->text, wcslen((*items)->text), &xx);
        if ((*items)->expanded && menuData->gd.selectedRow > 10 + fontHeight)
        {
            if (menuData->gd.selectedColumn >= x)
                return InsertDeleteFromColumns(hDC, menuData, &(*items)->popup, x, 10+fontHeight, fontHeight, code);
            x += xx.cx+16;
        }
        else if (menuData->gd.selectedRow < 10 + fontHeight)
        {        
            if (menuData->gd.selectedColumn < x + xx.cx + 16)
            {
                DoInsert(menuData, items, code, NULL);
                return 1;
            }
            x += xx.cx+16;
        }
        else
        {
            x += xx.cx+16;
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
void InsertDelete(HWND hwnd, struct resRes * menuData, int code)
{
    NONCLIENTMETRICS m;
    HFONT font;
    int fontHeight;
    HDC hDC = GetDC(hwnd);
    m.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS),&m, NULL);
    fontHeight = (m.lfMenuFont.lfHeight < 0 ? - m.lfMenuFont.lfHeight : m.lfMenuFont.lfHeight)+6;
    font = CreateFontIndirect(&m.lfMenuFont);
    font = SelectObject(hDC, font);
    InsertDeleteFromTopRow(hDC, menuData, &menuData->resource->u.menu->items, fontHeight, code);
    font = SelectObject(hDC, font);
    DeleteObject(font);
    ReleaseDC(hwnd, hDC);
}
LRESULT CALLBACK MenuDrawProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    static MENUITEM *start;
    static BOOL dragging;
    static HCURSOR oldcurs;
    static struct resRes *resData;
    static struct propertyFuncs *resFuncs;
    POINT mouse;
    RECT r;
    PAINTSTRUCT paint;
    HDC dc;
    LPCREATESTRUCT createStruct;
    struct resRes *menuData;
    int i;
    struct menuUndo *undo;
    switch (iMessage)
    {
        case WM_MDIACTIVATE:
            if ((HWND)lParam == hwnd)
            {
                doMaximize();
            }
            break;
        case WM_SETFOCUS:
            menuData = (struct resRes *)GetWindowLong(hwnd, 0);
            SetResourceProperties(resData, resFuncs);
            break;
        case EM_CANUNDO:
            menuData = (struct resRes *)GetWindowLong(hwnd, 0);
            return menuData->gd.undoData != NULL;
        case WM_KEYDOWN:
            menuData = (struct resRes *)GetWindowLong(hwnd, 0);
            switch (wParam)
            {
            case 'S':
                if (GetKeyState(VK_CONTROL) &0x80000000)
                {
                    PostMessage(hwnd, WM_COMMAND, IDM_SAVE, 0);
                }
                break;
            case 'Z':
                if (GetKeyState(VK_CONTROL) &0x80000000)
                {
                    PostMessage(hwnd, WM_COMMAND, IDM_UNDO, 0);
                }
                break;
            }
            break;
        case WM_CREATE:
            createStruct = (LPCREATESTRUCT)lParam;
            menuData = (struct resRes *)((LPMDICREATESTRUCT)(createStruct->lpCreateParams))->lParam;
            SetWindowLong(hwnd, 0, (long)menuData);
            menuData->activeHwnd = hwnd;
            resData = menuData;
            resFuncs = &menuFuncs;
            MarkUnexpanded(menuData->resource->u.menu->items);
            break;
        
        case WM_CLOSE:
            SendMessage(hwndSrcTab, TABM_REMOVE, 0, (LPARAM)hwnd);
            break;
        case WM_DESTROY:
            menuData = (struct resRes *)GetWindowLong(hwnd, 0);
            menuData->activeHwnd = NULL;
            undo = menuData->gd.undoData;
            menuData->gd.undoData = NULL;
            if (undo)
                menuData->gd.cantClearUndo = TRUE;
            while (undo)
            {
                struct menuUndo *next = undo->next;
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
        case WM_PAINT:
            menuData = (struct resRes *)GetWindowLong(hwnd, 0);
            GetClientRect(hwnd, &r);
            dc = BeginPaint(hwnd, &paint);
            DoPaint(hwnd, dc, &paint, &r, menuData);
            EndPaint(hwnd, &paint);
            break;
        case WM_RBUTTONDOWN:
            menuData = (struct resRes *)GetWindowLong(hwnd, 0);
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
                TrackPopupMenuEx(popup, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_LEFTBUTTON, mouse.x,
                    mouse.y, hwnd, NULL);
                DestroyMenu(menu);
            }
            return 1;
        case WM_LBUTTONDBLCLK:
            SendMessage(hwnd, WM_COMMAND, ID_EDIT + (EN_KILLFOCUS << 16), 0);
            SetFocus(hwnd);
            menuData = (struct resRes *)GetWindowLong(hwnd, 0);
            mouse.x = LOWORD(lParam);
            mouse.y = HIWORD(lParam);
            SelectSubmenu(hwnd, menuData, mouse, TRUE);
            break;
        case WM_MOUSEMOVE:
            if (dragging)
            {
                menuData = (struct resRes *)GetWindowLong(hwnd, 0);
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
            menuData = (struct resRes *)GetWindowLong(hwnd, 0);
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
            menuData = (struct resRes *)GetWindowLong(hwnd, 0);
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
            menuData = (struct resRes *)GetWindowLong(hwnd, 0);
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
            menuData = (struct resRes *)GetWindowLong(hwnd, 0);
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
            InvalidateRect(hwnd,0,FALSE);
            return 0;
        case WM_HSCROLL:
            menuData = (struct resRes *)GetWindowLong(hwnd, 0);
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
            SetWindowPos(menuData->gd.childWindow, NULL,
                         menuData->gd.origin.x - menuData->gd.scrollPos.x,
                         menuData->gd.origin.y - menuData->gd.scrollPos.y,
                         0,0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
                         
            InvalidateRect(hwnd,0,FALSE);
                       
            return 0;
        case WM_SIZE:
            InvalidateRect(hwnd, 0, FALSE);
            break;
        default:
            break;
    }
    return DefMDIChildProc(hwnd, iMessage, wParam, lParam);
}

void RegisterMenuDrawWindow(void)
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
void CreateMenuDrawWindow(struct resRes *info)
{
    char name[512];
    int maximized;
    HWND hwnd;
    sprintf(name, "%s - %s", szUntitled, info->name);
    SendMessage(hwndClient, WM_MDIGETACTIVE, 0, (LPARAM) &maximized);
    hwnd = CreateMDIWindow(szMenuDrawClassName, name, WS_VISIBLE |
           WS_CHILD | WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | MDIS_ALLCHILDSTYLES | 
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
        WS_SIZEBOX | (PropGetInt(NULL, "TABBED_WINDOWS") ? WS_MAXIMIZE : WS_SYSMENU),
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwndClient, hInstance, 
        (LPARAM)info); 
    if (hwnd)
    {
        SendMessage(hwndSrcTab, TABM_ADD, (WPARAM)name, (LPARAM)hwnd);
        if (info->resource->changed)
            ResRewriteTitle(info, TRUE);
    }
}