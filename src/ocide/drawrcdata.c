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
#include <assert.h>

#include "header.h"
#include "rc.h"
#include "rcgui.h"

#define ID_EDIT 2001

extern HWND hwndSrcTab;
extern HWND hwndClient;
extern HINSTANCE hInstance;
extern PROJECTITEM* workArea;
extern struct propertyFuncs accFuncs;

static const int ALT = 0x80000000;
static const int SHFT = 0x40000000;
static const int CTL = 0x20000000;
static char* szrcDataDrawClassName = "xccrcDataDrawClass";
static char* szrcDataetKeyClassName = "xccrcDataetKeyClass";
static char* szUntitled = "RC Data";

static HCURSOR dragCur, noCur;
static WNDPROC oldEditProc;
HANDLE ResGetHeap(PROJECTITEM* wa, struct resRes* data);
EXPRESSION* ResReadExp(struct resRes* data, char* buf);

HWND PropGetHWNDNumeric(HWND parent);
HWND PropGetHWNDText(HWND parent);

struct rcDataUndo
{
    struct rcDataUndo* next;
    enum
    {
        ru_nop,
        ru_insert,
        ru_delete,
        ru_move,
        ru_setchars
    } type;
    union
    {
        struct
        {
            int index;
            RCDATA value;
        } values;
        struct
        {
            int from;
            int to;
        } move;
        struct
        {
            int index;
            RCDATA* data;
        } deleted;

    } u;
};
static void SetListViewColumns(HWND hwndParent, HWND hwnd)
{
    LV_COLUMN lvC;
    ListView_SetExtendedListViewStyle(hwnd, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
    lvC.cx = 180;
    lvC.iSubItem = 0;
    lvC.pszText = "Type";
    ListView_InsertColumn(hwnd, 0, &lvC);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
    lvC.cx = 400;
    lvC.iSubItem = 1;
    lvC.pszText = "Value";
    ListView_InsertColumn(hwnd, 1, &lvC);
}
static void PopulateItem(HWND hwnd, RCDATA* rcData, int items)
{
    LV_ITEMW item;
    char* key = "Unknown";
    switch (rcData->type)
    {
        case RCDATA_WORD:
            key = "Word";
            break;
        case RCDATA_DWORD:
            key = "DWord";
            break;
        case RCDATA_STRING:
            key = "ASCII String";
            break;
        case RCDATA_WSTRING:
            key = "Unicode String";
            break;
        case RCDATA_BUFFER:
            key = "Buffer";
            break;
    }
    memset(&item, 0, sizeof(LV_ITEM));
    item.iItem = items;
    item.iSubItem = 0;
    item.mask = LVIF_PARAM | LVIF_TEXT;
    item.lParam = (LPARAM)rcData;
    item.pszText = (unsigned short*)key;  // fixme
    ListView_InsertItem(hwnd, &item);
    item.iSubItem = 1;
    item.mask = LVIF_PARAM;
    item.lParam = (LPARAM)rcData;
    ListView_InsertItem(hwnd, &item);
}
static void PopulateRows(HWND hwnd, struct resRes* rcDataData)
{
    int items = 0;
    RCDATA* rcData = rcDataData->resource->u.rcdata;
    while (rcData)
    {
        PopulateItem(rcDataData->gd.childWindow, rcData, items++);
        rcData = rcData->next;
    }
}
// for delete, change
static void rcDataSetChanged(struct resRes* rcDataData, RCDATA* rcData)
{
    struct rcDataUndo* newUndo = calloc(1, sizeof(struct rcDataUndo));
    if (newUndo)
    {
        newUndo->u.values.index = rcDataData->gd.selectedRow;
        newUndo->type = ru_setchars;
        newUndo->u.values.value = *rcData;
        newUndo->next = rcDataData->gd.undoData;
        rcDataData->gd.undoData = newUndo;
        ResSetDirty(rcDataData);
    }
}
// for move, insert
static void rcDataSetMoved(int type, struct resRes* rcDataData, int from, int to)
{
    struct rcDataUndo* newUndo = calloc(1, sizeof(struct rcDataUndo));
    if (newUndo)
    {
        newUndo->type = type;
        newUndo->u.move.from = from;
        newUndo->u.move.to = to;
        newUndo->next = rcDataData->gd.undoData;
        rcDataData->gd.undoData = newUndo;
        ResSetDirty(rcDataData);
    }
}
static void rcDataSetDeleted(struct resRes* rcDataData, int from, RCDATA* data)
{
    struct rcDataUndo* newUndo = calloc(1, sizeof(struct rcDataUndo));
    if (newUndo)
    {
        newUndo->type = ru_delete;
        newUndo->u.deleted.index = from;
        newUndo->u.deleted.data = data;
        newUndo->next = rcDataData->gd.undoData;
        rcDataData->gd.undoData = newUndo;
        ResSetDirty(rcDataData);
    }
}
static void rcDataDoUndo(struct resRes* rcDataData)
{
    struct rcDataUndo* undo = rcDataData->gd.undoData;
    if (undo)
    {
        rcDataData->gd.undoData = undo->next;
        switch (undo->type)
        {
            RCDATA** p;
            RCDATA* hold;
            int i;
            case ru_nop:
                break;
            case ru_setchars:
                p = &rcDataData->resource->u.rcdata;
                for (i = 0; i < undo->u.values.index && *p; i++, p = &(*p)->next)
                    ;
                if (*p)
                {
                    *(*p) = undo->u.values.value;
                    ListView_DeleteItem(rcDataData->gd.childWindow, undo->u.values.index);
                    PopulateItem(rcDataData->gd.childWindow, *p, undo->u.values.index);
                }
                break;
            case ru_move:
                p = &rcDataData->resource->u.rcdata;
                for (i = 0; i < undo->u.move.to && *p; i++, p = &(*p)->next)
                    ;
                hold = *p;
                (*p) = (*p)->next;
                ListView_DeleteItem(rcDataData->gd.childWindow, undo->u.move.to);
                p = &rcDataData->resource->u.rcdata;
                for (i = 0; i < undo->u.move.from && *p; i++, p = &(*p)->next)
                    ;
                hold->next = *p;
                *p = hold;
                PopulateItem(rcDataData->gd.childWindow, hold, undo->u.move.from);
                break;
            case ru_insert:
                p = &rcDataData->resource->u.rcdata;
                for (i = 0; i < undo->u.move.from && *p; i++, p = &(*p)->next)
                    ;
                *p = (*p)->next;
                ListView_DeleteItem(rcDataData->gd.childWindow, undo->u.move.from);

                break;
            case ru_delete:
                p = &rcDataData->resource->u.rcdata;
                for (i = 0; i < undo->u.deleted.index && *p; i++, p = &(*p)->next)
                    ;
                undo->u.deleted.data->next = *p;
                *p = undo->u.deleted.data;
                PopulateItem(rcDataData->gd.childWindow, undo->u.deleted.data, undo->u.deleted.index);
                break;
        }
        free(undo);
        if (rcDataData->gd.cantClearUndo)
            ResSetDirty(rcDataData);
        else if (!rcDataData->gd.undoData)
            ResSetClean(rcDataData);
    }
}
static void rcDataInsert(struct resRes* rcDataData, int index)
{
    if (index != -1)
    {
        RCDATA* rcData = rcAlloc(sizeof(ACCELERATOR));
        ResGetHeap(workArea, rcDataData);
        if (rcData)
        {
            int origIndex = 0;
            RCDATA** p = &rcDataData->resource->u.rcdata;
            rcData->type = RCDATA_DWORD;
            rcData->u.dword = 0;
            for (; index && *p; index--, origIndex++, p = &(*p)->next)
                ;
            rcData->next = *p;
            *p = rcData;
            PopulateItem(rcDataData->gd.childWindow, rcData, origIndex);
            rcDataSetMoved(ru_insert, rcDataData, origIndex, 0);
        }
    }
}
static void rcDataDelete(struct resRes* rcDataData, int index)
{
    if (index != -1)
    {
        int origIndex = index;
        if (ExtendedMessageBox("RC Data Delete", MB_OKCANCEL, "This will delete the RC Data from the table") == IDOK)
        {
            RCDATA** p = &rcDataData->resource->u.rcdata;
            for (; index && *p; index--, p = &(*p)->next)
                ;
            if (*p)
            {
                RCDATA* old = *p;
                // we don't need to free the data because it is on a heap that will be freed later
                (*p) = (*p)->next;
                ListView_DeleteItem(rcDataData->gd.childWindow, origIndex);
                rcDataSetDeleted(rcDataData, origIndex, old);
            }
        }
    }
}
void FormatAsciiString(char* buf, char* string, int len)
{
    *buf++ = '\"';
    while (len--)
    {
        unsigned char ch = *string++;
        if (ch < 0x20)
        {
            *buf++ = '\\';
            switch (ch)
            {
                case 0:
                    *buf++ = '0';
                    break;
                case '\a':
                    *buf++ = 'a';
                    break;
                case '\b':
                    *buf++ = 'b';
                    break;
                case '\f':
                    *buf++ = 'f';
                    break;
                case '\n':
                    *buf++ = 'n';
                    break;
                case '\r':
                    *buf++ = 'r';
                    break;
                case '\t':
                    *buf++ = 't';
                    break;
                default:
                    sprintf(buf, "x%02X", ch);
                    buf += strlen(buf);
                    break;
            }
        }
        else
        {
            switch (ch)
            {
                case '\'':
                    *buf++ = '\\';
                    *buf++ = '\'';
                    break;
                case '\"':
                    *buf++ = '\\';
                    *buf++ = '\"';
                    break;
                case '\\':
                    *buf++ = '\\';
                    *buf++ = '\\';
                    break;
                default:
                    *buf++ = ch;
                    break;
            }
        }
    }
    *buf++ = '\"';
    *buf++ = 0;
}
static void rcDataSet(struct resRes* rcDataData, RCDATA* rcData, char* text)
{
    char* string;
    ResGetHeap(workArea, rcDataData);
    switch (rcData->type)
    {
        int i;
        case RCDATA_WORD:
            if (text[0] == '"')
                sscanf(text, "\"%d\"", &i);
            else
                i = atoi(text);
            rcData->u.word = i;
            break;
        case RCDATA_DWORD:
            if (text[0] == '"')
                sscanf(text, "\"%d\"", &i);
            else
                i = atoi(text);
            rcData->u.dword = i;
            break;
        case RCDATA_STRING:
            string = ParseVersionString(text, &rcData->u.string.length);
            rcData->u.string.s = rcAlloc(rcData->u.string.length + 1);
            memcpy(rcData->u.string.s, string, rcData->u.string.length);
            break;
        case RCDATA_WSTRING:
            string = ParseVersionString(text, &rcData->u.wstring.length);
            rcData->u.wstring.w = rcAlloc((rcData->u.wstring.length + 1) * sizeof(WCHAR));
            for (i = 0; i < rcData->u.wstring.length; i++)
                rcData->u.wstring.w[i] = string[i];
            break;
        case RCDATA_BUFFER:
            assert(0);
            break;
    }
}
static void rcDataGetValue(char* buf, RCDATA* rcData)
{
    switch (rcData->type)
    {
        case RCDATA_WORD:
            sprintf(buf, "%d", rcData->u.word);
            break;
        case RCDATA_DWORD:
            wsprintf(buf, "%d", rcData->u.dword);
            break;
        case RCDATA_STRING:
            FormatAsciiString(buf, (char*)rcData->u.string.s, rcData->u.string.length);
            break;
        case RCDATA_WSTRING:
            FormatVersionString(buf, rcData->u.wstring.w, rcData->u.wstring.length);
            break;
        case RCDATA_BUFFER:
        default:
            strcpy(buf, "<>");
            break;
    }
}
static void rcDataConvert(struct resRes* rcDataData, RCDATA* rcData, int sel)
{
    if (sel != CB_ERR)
    {
        switch (sel)
        {
            case 0:
                sel = RCDATA_WORD;
                break;
            default:
            case 1:
                sel = RCDATA_DWORD;
                break;
            case 2:
                sel = RCDATA_STRING;
                break;
            case 3:
                sel = RCDATA_WSTRING;
                break;
        }
        if (sel != rcData->type)
        {
            char buf[256];
            rcDataSetChanged(rcDataData, rcData);
            rcDataGetValue(buf, rcData);
            rcData->type = sel;
            rcDataSet(rcDataData, rcData, buf);
        }
    }
}
LRESULT CALLBACK rcDataDrawProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    static HCURSOR origCurs;
    RECT r;
    LPCREATESTRUCT createStruct;
    struct resRes* rcDataData;
    LVHITTESTINFO hittest;
    int i;
    struct rcDataUndo* undo;
    switch (iMessage)
    {
        case WM_MDIACTIVATE:
            if ((HWND)lParam == hwnd)
            {
                doMaximize();
            }
            break;
        case WM_SETFOCUS:
            rcDataData = (struct resRes*)GetWindowLong(hwnd, 0);
            SetFocus(rcDataData->gd.childWindow);
            break;
        case WM_NOTIFY:
            switch (((LPNMHDR)lParam)->code)
            {
                case LVN_GETDISPINFO:
                {
                    char buf[256];
                    LV_DISPINFO* plvdi = (LV_DISPINFO*)lParam;
                    RCDATA* rcData;
                    plvdi->item.mask |= LVIF_TEXT | LVIF_DI_SETITEM;
                    plvdi->item.mask &= ~LVIF_IMAGE;
                    rcData = (RCDATA*)plvdi->item.lParam;
                    switch (plvdi->item.iSubItem)
                    {
                        case 1:
                            rcDataGetValue(buf, rcData);
                            plvdi->item.pszText = buf;
                            break;
                        default:
                            plvdi->item.pszText = "";
                            break;
                    }
                    break;
                }
                case LVN_BEGINDRAG:
                {
                    rcDataData = (struct resRes*)GetWindowLong(hwnd, 0);
                    GetCursorPos(&hittest.pt);
                    ScreenToClient(rcDataData->gd.childWindow, &hittest.pt);
                    hittest.iItem = -1;
                    ListView_HitTest(rcDataData->gd.childWindow, &hittest);
                    if (hittest.flags & LVHT_ONITEM)
                    {
                        rcDataData->gd.dragStart = hittest.iItem;
                        rcDataData->gd.bDragging = TRUE;
                        rcDataData->gd.dragInView = TRUE;
                        SetCapture(hwnd);
                        origCurs = SetCursor(dragCur);
                    }
                    break;
                }
                case LVN_KEYDOWN:
                {
                    rcDataData = (struct resRes*)GetWindowLong(hwnd, 0);
                    SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_EDIT, CBN_KILLFOCUS), 0);
                    switch (((LPNMLVKEYDOWN)lParam)->wVKey)
                    {
                        case 'S':
                            if (GetKeyState(VK_CONTROL) & 0x80000000)
                            {
                                PostMessage(hwnd, WM_COMMAND, IDM_SAVE, 0);
                                return TRUE;
                            }

                            break;
                        case 'Z':
                            if (GetKeyState(VK_CONTROL) & 0x80000000)
                            {
                                PostMessage(hwnd, WM_COMMAND, IDM_UNDO, 0);
                                return TRUE;
                            }
                            break;
                        case VK_INSERT:
                            rcDataData->gd.selectedRow = ListView_GetNextItem(rcDataData->gd.childWindow, -1, LVNI_SELECTED);
                            PostMessage(hwnd, WM_COMMAND, IDM_INSERT, 0);
                            return TRUE;
                        case VK_DELETE:
                            rcDataData->gd.selectedRow = ListView_GetNextItem(rcDataData->gd.childWindow, -1, LVNI_SELECTED);
                            PostMessage(hwnd, WM_COMMAND, IDM_DELETE, 0);
                            return TRUE;
                    }
                    switch (KeyboardToAscii(((LPNMLVKEYDOWN)lParam)->wVKey, ((LPNMLVKEYDOWN)lParam)->flags, FALSE))
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
                }
                break;
                case NM_CLICK:
                {
                    SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_EDIT, CBN_KILLFOCUS), 0);
                }
                break;
                case NM_RCLICK:
                {
                    POINT pt;
                    rcDataData = (struct resRes*)GetWindowLong(hwnd, 0);
                    GetCursorPos(&hittest.pt);
                    pt = hittest.pt;
                    ScreenToClient(rcDataData->gd.childWindow, &hittest.pt);
                    if (ListView_HitTest(rcDataData->gd.childWindow, &hittest) < 0)
                    {
                        hittest.iItem = ListView_GetItemCount(rcDataData->gd.childWindow);
                        hittest.iSubItem = 0;
                    }
                    {
                        HMENU menu, popup;
                        SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_EDIT, CBN_KILLFOCUS), 0);
                        menu = LoadMenuGeneric(hInstance, "RESRCDATAMENU");
                        rcDataData = (struct resRes*)GetWindowLong(hwnd, 0);
                        rcDataData->gd.selectedRow = hittest.iItem;
                        rcDataData->gd.selectedColumn = hittest.iSubItem;
                        popup = GetSubMenu(menu, 0);
                        InsertBitmapsInMenu(popup);
                        TrackPopupMenuEx(popup, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, hwnd, NULL);
                        DestroyMenu(menu);
                    }
                    return 1;
                }
                break;
                case NM_DBLCLK:
                    rcDataData = (struct resRes*)GetWindowLong(hwnd, 0);
                    GetCursorPos(&hittest.pt);
                    ScreenToClient(rcDataData->gd.childWindow, &hittest.pt);
                    if (ListView_SubItemHitTest(rcDataData->gd.childWindow, &hittest) >= 0)
                    {
                        rcDataData = (struct resRes*)GetWindowLong(hwnd, 0);
                        rcDataData->gd.selectedRow = hittest.iItem;
                        rcDataData->gd.selectedColumn = hittest.iSubItem;
                        PostMessage(hwnd, WM_HANDLEDBLCLICK, 0, 0);
                    }
                    break;
                case NM_SETFOCUS:
                    rcDataData = (struct resRes*)GetWindowLong(hwnd, 0);
                    SetResourceProperties(rcDataData, &accFuncs);
                    break;
                case NM_KILLFOCUS:
                    //                        SetResourceProperties(NULL, NULL);
                    break;
            }
            break;
        case WM_HANDLEDBLCLICK:
        {
            RCDATA* rcData;
            rcDataData = (struct resRes*)GetWindowLong(hwnd, 0);
            if (rcDataData->gd.editWindow)
            {
                DestroyWindow(rcDataData->gd.editWindow);
                rcDataData->gd.editWindow = NULL;
            }
            rcData = rcDataData->resource->u.rcdata;
            i = 0;
            while (rcData && rcData->next && i < rcDataData->gd.selectedRow)
                rcData = rcData->next, i++;
            if (rcData && rcData->type != RCDATA_BUFFER && rcData->type != RCDATA_WSTRING)
            {
                switch (rcDataData->gd.selectedColumn)
                {
                    RECT r;
                    char buf[256];
                    HWND editwnd;
                    POINT pt;
                    int v;
                    case 0:
                        ListView_GetSubItemRect(rcDataData->gd.childWindow, rcDataData->gd.selectedRow,
                                                rcDataData->gd.selectedColumn, LVIR_BOUNDS, &r);
                        rcDataData->gd.editWindow =
                            CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | CBS_DROPDOWN | WS_CLIPSIBLINGS | WS_BORDER, r.left,
                                         r.top, 180, 50, hwnd, (HMENU)ID_EDIT, hInstance, NULL);
                        ApplyDialogFont(rcDataData->gd.editWindow);
                        SetParent(rcDataData->gd.editWindow, rcDataData->gd.childWindow);
                        pt.x = pt.y = 5;
                        editwnd = ChildWindowFromPoint(rcDataData->gd.editWindow, pt);
                        SendMessage(editwnd, EM_SETREADONLY, 0, 0);
                        AccSubclassEditWnd(hwnd, editwnd);
                        SendMessage(rcDataData->gd.editWindow, CB_ADDSTRING, 0, (LPARAM) "Word");
                        SendMessage(rcDataData->gd.editWindow, CB_ADDSTRING, 0, (LPARAM) "DWord");
                        SendMessage(rcDataData->gd.editWindow, CB_ADDSTRING, 0, (LPARAM) "ASCII String");
                        //                        SendMessage(rcDataData->gd.editWindow, CB_ADDSTRING, 0, (LPARAM)"Unicode String");
                        switch (rcData->type)
                        {
                            case RCDATA_WORD:
                                v = 0;
                                break;
                            default:
                            case RCDATA_DWORD:
                                v = 1;
                                break;
                            case RCDATA_STRING:
                                v = 2;
                                break;
                            case RCDATA_WSTRING:
                                v = 3;
                                break;
                        }
                        SendMessage(rcDataData->gd.editWindow, CB_SETCURSEL, v, 0);
                        break;
                    case 1:
                        ListView_GetSubItemRect(rcDataData->gd.childWindow, rcDataData->gd.selectedRow,
                                                rcDataData->gd.selectedColumn, LVIR_BOUNDS, &r);
                        rcDataData->gd.editWindow =
                            CreateWindow("edit", "", WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_BORDER, r.left, r.top,
                                         r.right - r.left, 16, hwnd, (HMENU)ID_EDIT, hInstance, NULL);
                        ApplyDialogFont(rcDataData->gd.editWindow);
                        SetParent(rcDataData->gd.editWindow, rcDataData->gd.childWindow);
                        rcDataGetValue(buf, rcData);
                        if (buf[0] == '"')
                        {
                            strcpy(buf, buf + 1);
                            buf[strlen(buf) - 1] = 0;
                        }
                        SendMessage(rcDataData->gd.editWindow, WM_SETTEXT, 0, (LPARAM)buf);
                        AccSubclassEditWnd(hwnd, rcDataData->gd.editWindow);
                        SendMessage(rcDataData->gd.editWindow, EM_SETSEL, 0, -1);
                        SetFocus(rcDataData->gd.editWindow);
                        break;
                }
            }
        }
        break;
        case WM_MOUSEMOVE:
            rcDataData = (struct resRes*)GetWindowLong(hwnd, 0);
            if (rcDataData->gd.bDragging)
            {
                POINT pt;
                GetClientRect(hwnd, &r);
                pt.x = (long)(short)LOWORD(lParam);
                pt.y = (long)(short)HIWORD(lParam);
                hittest.pt = pt;
                ClientToScreen(hwnd, &pt);
                if (WindowFromPoint(pt) == rcDataData->gd.childWindow)
                {
                    SetCursor(dragCur);
                    rcDataData->gd.dragInView = TRUE;
                }
                else
                {
                    SetCursor(noCur);
                    rcDataData->gd.dragInView = FALSE;
                }
                ListView_HitTest(rcDataData->gd.childWindow, &hittest);
                if (hittest.pt.y < 0)
                {
                    if (!rcDataData->gd.lvscroll)
                        SetTimer(hwnd, 100, 300, NULL);
                    rcDataData->gd.lvscroll = 1;
                }
                else if (hittest.pt.y >= r.bottom)
                {
                    if (!rcDataData->gd.lvscroll)
                        SetTimer(hwnd, 100, 300, NULL);
                    rcDataData->gd.lvscroll = 2;
                }
                else
                {
                    if (rcDataData->gd.lvscroll)
                        KillTimer(hwnd, 100);
                    rcDataData->gd.lvscroll = 0;
                }
            }
            break;
        case WM_TIMER:
            rcDataData = (struct resRes*)GetWindowLong(hwnd, 0);
            ListView_Scroll(rcDataData->gd.childWindow, 0, (rcDataData->gd.lvscroll & 1) ? -16 : 16);
            break;
        case WM_LBUTTONUP:
            rcDataData = (struct resRes*)GetWindowLong(hwnd, 0);
            if (rcDataData->gd.bDragging)
            {
                hittest.pt.x = (long)(short)LOWORD(lParam);
                hittest.pt.y = (long)(short)HIWORD(lParam);
                if (rcDataData->gd.lvscroll)
                    KillTimer(hwnd, 100);
                rcDataData->gd.lvscroll = 0;
                GetCursorPos(&hittest.pt);
                ScreenToClient(rcDataData->gd.childWindow, &hittest.pt);
                ReleaseCapture();
                SetCursor(origCurs);
                ListView_HitTest(rcDataData->gd.childWindow, &hittest);
                if (hittest.flags & LVHT_NOWHERE)
                {
                    hittest.iItem = ListView_GetItemCount(rcDataData->gd.childWindow);
                }
                if (hittest.flags & LVHT_ONITEM || ((hittest.flags & LVHT_NOWHERE) && rcDataData->gd.dragInView))
                {
                    int dragEnd = hittest.iItem;
                    rcDataData = (struct resRes*)GetWindowLong(hwnd, 0);
                    if (dragEnd != rcDataData->gd.dragStart)
                    {
                        RCDATA** p = &rcDataData->resource->u.rcdata;
                        int i;
                        for (i = 0; *p && i < rcDataData->gd.dragStart; i++, p = &(*p)->next)
                            ;
                        if (*p)
                        {
                            RCDATA* hold = *p;
                            *p = (*p)->next;
                            p = &rcDataData->resource->u.rcdata;
                            for (i = 0; *p && i < dragEnd; i++, p = &(*p)->next)
                                ;
                            hold->next = *p;
                            *p = hold;
                            ListView_DeleteItem(rcDataData->gd.childWindow, rcDataData->gd.dragStart);
                            PopulateItem(rcDataData->gd.childWindow, hold, dragEnd);
                            rcDataSetMoved(ru_move, rcDataData, rcDataData->gd.dragStart, dragEnd);
                        }
                    }
                }
            }
            break;
        case WM_COMMAND:
            rcDataData = (struct resRes*)GetWindowLong(hwnd, 0);
            switch (LOWORD(wParam))
            {
                case ID_EDIT:
                    if (HIWORD(wParam) == CBN_KILLFOCUS || HIWORD(wParam) == EN_KILLFOCUS)
                    {
                        static BOOL inKillFocus;
                        if (rcDataData->gd.editWindow && !inKillFocus)
                        {
                            RCDATA* rcData = rcDataData->resource->u.rcdata;
                            i = 0;
                            while (rcData && rcData->next && i < rcDataData->gd.selectedRow)
                                rcData = rcData->next, i++;
                            if (rcData)
                            {
                                if (rcDataData->gd.selectedColumn == 0)
                                {
                                    rcDataConvert(rcDataData, rcData, SendMessage(rcDataData->gd.editWindow, CB_GETCURSEL, 0, 0));
                                }
                                else if (rcDataData->gd.selectedColumn == 1)
                                {
                                    char buf[256];
                                    buf[GetWindowText(rcDataData->gd.editWindow, buf, sizeof(buf) - 1)] = 0;
                                    rcDataSetChanged(rcDataData, rcData);
                                    rcDataSet(rcDataData, rcData, buf);
                                }
                                ListView_DeleteItem(rcDataData->gd.childWindow, rcDataData->gd.selectedRow);
                                PopulateItem(rcDataData->gd.childWindow, rcData, rcDataData->gd.selectedRow);
                            }
                            inKillFocus = TRUE;
                            DestroyWindow(rcDataData->gd.editWindow);
                            inKillFocus = FALSE;
                            rcDataData->gd.editWindow = NULL;
                        }
                    }
                    break;
                case IDM_UNDO:
                    rcDataDoUndo(rcDataData);
                    break;
                case IDM_SAVE:
                    if (rcDataData->resource->changed)
                    {
                        ResSaveCurrent(workArea, rcDataData);
                    }
                    break;
                case IDM_INSERT:
                    rcDataInsert(rcDataData, rcDataData->gd.selectedRow);
                    break;
                case IDM_DELETE:
                    rcDataDelete(rcDataData, rcDataData->gd.selectedRow);
                    ListView_SetItemState(rcDataData->gd.childWindow, rcDataData->gd.selectedRow, LVIS_SELECTED, LVIS_SELECTED);
                    break;
            }
            break;
        case EM_CANUNDO:
            rcDataData = (struct resRes*)GetWindowLong(hwnd, 0);
            return rcDataData->gd.undoData != NULL;
        case WM_NCACTIVATE:
            PaintMDITitleBar(hwnd, iMessage, wParam, lParam);
            return TRUE;
        case WM_NCPAINT:
            return PaintMDITitleBar(hwnd, iMessage, wParam, lParam);
        case WM_CREATE:
            SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
            GetClientRect(hwnd, &r);
            createStruct = (LPCREATESTRUCT)lParam;
            rcDataData = (struct resRes*)((LPMDICREATESTRUCT)(createStruct->lpCreateParams))->lParam;
            SetWindowLong(hwnd, 0, (long)rcDataData);
            rcDataData->activeHwnd = hwnd;
            rcDataData->gd.childWindow = CreateWindowEx(0, WC_LISTVIEW, "", WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SINGLESEL, 0,
                                                        0, r.right, r.bottom, hwnd, (HMENU)ID_TREEVIEW, hInstance, NULL);
            SetListViewColumns(hwnd, rcDataData->gd.childWindow);
            PopulateRows(hwnd, rcDataData);
            InsertRCWindow(hwnd);
            break;
        case WM_CLOSE:
            RemoveRCWindow(hwnd);
            SendMessage(hwndSrcTab, TABM_REMOVE, 0, (LPARAM)hwnd);
            break;
        case WM_DESTROY:
            rcDataData = (struct resRes*)GetWindowLong(hwnd, 0);
            if (rcDataData->gd.editWindow)
                DestroyWindow(rcDataData->gd.editWindow);
            rcDataData->gd.editWindow = NULL;
            if (rcDataData->gd.bDragging)
                ReleaseCapture();
            undo = rcDataData->gd.undoData;
            if (undo)
                rcDataData->gd.cantClearUndo = TRUE;
            while (undo)
            {
                struct rcDataUndo* next = undo->next;
                free(undo);
                undo = next;
            }
            rcDataData->gd.undoData = NULL;
            rcDataData->activeHwnd = NULL;
            break;
        case WM_SIZE:
            rcDataData = (struct resRes*)GetWindowLong(hwnd, 0);
            MoveWindow(rcDataData->gd.childWindow, 0, 0, LOWORD(lParam), HIWORD(lParam), 1);
            break;
        default:
            break;
    }
    return DefMDIChildProc(hwnd, iMessage, wParam, lParam);
}

void RegisterRCDataDrawWindow(HINSTANCE hInstance)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = &rcDataDrawProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(void*) * 2;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szrcDataDrawClassName;
    RegisterClass(&wc);

    dragCur = LoadCursor(hInstance, "ID_DRAGCTL");
    noCur = LoadCursor(hInstance, "ID_NODRAGCUR");
}
void CreateRCDataDrawWindow(struct resRes* info)
{
    char name[512];
    int maximized;
    HWND hwnd;
    sprintf(name, "%s - %s", szUntitled, info->name);
    SendMessage(hwndClient, WM_MDIGETACTIVE, 0, (LPARAM)&maximized);
    hwnd =
        CreateMDIWindow(szrcDataDrawClassName, name,
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