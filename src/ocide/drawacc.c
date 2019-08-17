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

extern BOOL ignoreAccel;
extern HWND hwndSrcTab;
extern HWND hwndClient;
extern HINSTANCE hInstance;
extern PROJECTITEM* workArea;
extern RESOURCE_DATA* currentResData;

static const int ALT = 0x80000000;
static const int SHFT = 0x40000000;
static const int CTL = 0x20000000;
static char* szAcceleratorDrawClassName = "xccAcceleratorDrawClass";
static char* szAcceleratorSetKeyClassName = "xccAcceleratorSetKeyClass";
static char* szUntitled = "Accelerator";

static HCURSOR dragCur, noCur;
static WNDPROC oldEditProc;
EXPRESSION* ResAllocateMenuId();
HANDLE ResGetHeap(PROJECTITEM* wa, struct resRes* data);
EXPRESSION* ResReadExp(struct resRes* data, char* buf);

static void InsertAccProperties(HWND lv, struct resRes* data);
void GetAccPropText(char* buf, HWND lv, struct resRes* data, int row);
HWND AccPropStartEdit(HWND lv, int row, struct resRes* data);
void AccPropEndEdit(HWND lv, int row, HWND editWnd, struct resRes* data);
// callbacks to populate the properties window for this resource editor
struct propertyFuncs accFuncs = {InsertAccProperties, GetAccPropText, AccPropStartEdit, AccPropEndEdit};

HWND PropGetHWNDNumeric(HWND parent);
HWND PropGetHWNDText(HWND parent);

struct accUndo
{
    struct accUndo* next;
    enum
    {
        au_nop,
        au_insert,
        au_delete,
        au_move,
        au_setchars
    } type;
    union
    {
        struct
        {
            int index;
            EXPRESSION* key;
            EXPRESSION* id;
            int skey;
            int flags;
        } values;
        struct
        {
            int from;
            int to;
        } move;
        struct
        {
            int index;
            ACCELERATOR* data;
        } deleted;

    } u;
};
static void InsertAccProperties(HWND lv, struct resRes* data)
{
    PropSetGroup(lv, 101, L"General Characteristics");
    PropSetItem(lv, 0, 101, "Resource Identifier");
    PropSetItem(lv, 1, 101, "Language");
    PropSetItem(lv, 2, 101, "SubLanguage");
    PropSetItem(lv, 3, 101, "Characteristics");
    PropSetItem(lv, 4, 101, "Version");
}
void GetAccPropText(char* buf, HWND lv, struct resRes* data, int row)
{
    buf[0] = 0;
    switch (row)
    {
        case 0:
            FormatResId(buf, &data->resource->id);
            break;
        case 1:
            FormatExp(buf, data->resource->info.language_high);
            break;
        case 2:
            FormatExp(buf, data->resource->info.language_low);
            break;
        case 3:
            FormatExp(buf, data->resource->info.characteristics);
            break;
        case 4:
            FormatExp(buf, data->resource->info.version);
            break;
    }
}
HWND AccPropStartEdit(HWND lv, int row, struct resRes* data)
{
    HWND rv = PropGetHWNDText(lv);
    if (rv)
    {
        char buf[256];
        GetAccPropText(buf, lv, data, row);
        SendMessage(rv, WM_SETTEXT, 0, (LPARAM)buf);
    }
    return rv;
}
void AccPropEndEdit(HWND lv, int row, HWND editWnd, struct resRes* data)
{
    char buf[256];
    char buf1[256];
    SendMessage(editWnd, WM_GETTEXT, sizeof(buf), (LPARAM)buf);
    GetAccPropText(buf1, lv, data, row);
    if (strcmp(buf, buf1))
    {
        switch (row)
        {
            case 0:
                PropSetIdName(data, buf, &data->resource->id.u.id, NULL, TRUE);
                break;
            case 1:
                PropSetExp(data, buf, &data->resource->info.language_high);
                break;
            case 2:
                PropSetExp(data, buf, &data->resource->info.language_low);
                break;
            case 3:
                PropSetExp(data, buf, &data->resource->info.characteristics);
                break;
            case 4:
                PropSetExp(data, buf, &data->resource->info.version);
                break;
        }
        ResSetDirty(data);
    }
    DestroyWindow(editWnd);
    ListView_SetItemText(lv, row, 1, LPSTR_TEXTCALLBACK);
    SetFocus(data->activeHwnd);
}
static void SetListViewColumns(HWND hwndParent, HWND hwnd)
{
    LV_COLUMN lvC;
    ListView_SetExtendedListViewStyle(hwnd, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
    lvC.cx = 180;
    lvC.iSubItem = 0;
    lvC.pszText = "Key";
    ListView_InsertColumn(hwnd, 0, &lvC);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
    lvC.cx = 180;
    lvC.iSubItem = 1;
    lvC.pszText = "Id";
    ListView_InsertColumn(hwnd, 1, &lvC);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
    lvC.cx = 60;
    lvC.iSubItem = 2;
    lvC.pszText = "Type";
    ListView_InsertColumn(hwnd, 2, &lvC);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
    lvC.cx = 60;
    lvC.iSubItem = 3;
    lvC.pszText = "Shift";
    ListView_InsertColumn(hwnd, 3, &lvC);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
    lvC.cx = 60;
    lvC.iSubItem = 4;
    lvC.pszText = "Alt";
    ListView_InsertColumn(hwnd, 4, &lvC);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
    lvC.cx = 60;
    lvC.iSubItem = 5;
    lvC.pszText = "Control";
    ListView_InsertColumn(hwnd, 5, &lvC);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
    lvC.cx = 60;
    lvC.iSubItem = 6;
    lvC.pszText = "Noinvert";
    ListView_InsertColumn(hwnd, 6, &lvC);
}
static void PopulateItem(HWND hwnd, ACCELERATOR* accelerators, int items)
{
    LV_ITEM item;
    static char key[256];
    key[0] = 0;
    if (accelerators->key)
    {
        FormatExp(key, accelerators->key);
    }
    else
    {
        if (accelerators->skey < 0x20)
        {
            sprintf(key, "\"^%c\"", (int)(char)accelerators->skey + 0x40);
        }
        else
        {
            sprintf(key, "\"%c\"", (int)(char)accelerators->skey);
        }
    }
    memset(&item, 0, sizeof(LV_ITEM));
    item.iItem = items;
    item.iSubItem = 0;
    item.mask = LVIF_PARAM | LVIF_TEXT;
    item.lParam = (LPARAM)accelerators;
    item.pszText = key;
    ListView_InsertItem(hwnd, &item);
    item.iSubItem = 1;
    item.mask = LVIF_PARAM;
    item.lParam = (LPARAM)accelerators;
    ListView_InsertItem(hwnd, &item);
    item.iSubItem = 2;
    item.mask = LVIF_PARAM;
    item.lParam = (LPARAM)accelerators;
    ListView_InsertItem(hwnd, &item);
    item.iSubItem = 3;
    item.mask = LVIF_PARAM;
    item.lParam = (LPARAM)accelerators;
    ListView_InsertItem(hwnd, &item);
    item.iSubItem = 4;
    item.mask = LVIF_PARAM;
    item.lParam = (LPARAM)accelerators;
    ListView_InsertItem(hwnd, &item);
    item.iSubItem = 5;
    item.mask = LVIF_PARAM;
    item.lParam = (LPARAM)accelerators;
    ListView_InsertItem(hwnd, &item);
    item.iSubItem = 6;
    item.mask = LVIF_PARAM;
    item.lParam = (LPARAM)accelerators;
    ListView_InsertItem(hwnd, &item);
}
static void PopulateKeys(HWND hwnd, struct resRes* acceleratorData)
{
    int items = 0;
    ACCELERATOR* accelerators = acceleratorData->resource->u.accelerator;
    while (accelerators)
    {
        PopulateItem(acceleratorData->gd.childWindow, accelerators, items++);
        accelerators = accelerators->next;
    }
}
// for delete, change
static void AccSetChanged(struct resRes* acceleratorData, ACCELERATOR* accelerator)
{
    struct accUndo* newUndo = calloc(1, sizeof(struct accUndo));
    if (newUndo)
    {
        newUndo->type = au_setchars;
        newUndo->u.values.index = acceleratorData->gd.selectedRow;
        newUndo->u.values.key = accelerator->key;
        newUndo->u.values.skey = accelerator->skey;
        newUndo->u.values.flags = accelerator->flags;
        newUndo->u.values.id = accelerator->id;
        newUndo->next = acceleratorData->gd.undoData;
        acceleratorData->gd.undoData = newUndo;
        ResSetDirty(acceleratorData);
    }
}
// for move, insert
static void AccSetMoved(int type, struct resRes* acceleratorData, int from, int to)
{
    struct accUndo* newUndo = calloc(1, sizeof(struct accUndo));
    if (newUndo)
    {
        newUndo->type = type;
        newUndo->u.move.from = from;
        newUndo->u.move.to = to;
        newUndo->next = acceleratorData->gd.undoData;
        acceleratorData->gd.undoData = newUndo;
        ResSetDirty(acceleratorData);
    }
}
static void AccSetDeleted(struct resRes* acceleratorData, int from, ACCELERATOR* data)
{
    struct accUndo* newUndo = calloc(1, sizeof(struct accUndo));
    if (newUndo)
    {
        newUndo->type = au_delete;
        newUndo->u.deleted.index = from;
        newUndo->u.deleted.data = data;
        newUndo->next = acceleratorData->gd.undoData;
        acceleratorData->gd.undoData = newUndo;
        ResSetDirty(acceleratorData);
    }
}
static void AccDoUndo(struct resRes* acceleratorData)
{
    struct accUndo* undo = acceleratorData->gd.undoData;
    if (undo)
    {
        acceleratorData->gd.undoData = undo->next;
        switch (undo->type)
        {
            ACCELERATOR** p;
            ACCELERATOR* hold;
            int i;
            case au_nop:
                break;
            case au_setchars:
                p = &acceleratorData->resource->u.accelerator;
                for (i = 0; i < undo->u.values.index && *p; i++, p = &(*p)->next)
                    ;
                if (*p)
                {
                    (*p)->id = undo->u.values.id;
                    (*p)->key = undo->u.values.key;
                    (*p)->skey = undo->u.values.skey;
                    (*p)->flags = undo->u.values.flags;
                    ListView_DeleteItem(acceleratorData->gd.childWindow, undo->u.values.index);
                    PopulateItem(acceleratorData->gd.childWindow, *p, undo->u.values.index);
                }
                break;
            case au_move:
                p = &acceleratorData->resource->u.accelerator;
                for (i = 0; i < undo->u.move.to && *p; i++, p = &(*p)->next)
                    ;
                hold = *p;
                (*p) = (*p)->next;
                ListView_DeleteItem(acceleratorData->gd.childWindow, undo->u.move.to);
                p = &acceleratorData->resource->u.accelerator;
                for (i = 0; i < undo->u.move.from && *p; i++, p = &(*p)->next)
                    ;
                hold->next = *p;
                *p = hold;
                PopulateItem(acceleratorData->gd.childWindow, hold, undo->u.move.from);
                break;
            case au_insert:
                p = &acceleratorData->resource->u.accelerator;
                for (i = 0; i < undo->u.move.from && *p; i++, p = &(*p)->next)
                    ;
                *p = (*p)->next;
                ListView_DeleteItem(acceleratorData->gd.childWindow, undo->u.move.from);

                break;
            case au_delete:
                p = &acceleratorData->resource->u.accelerator;
                for (i = 0; i < undo->u.deleted.index && *p; i++, p = &(*p)->next)
                    ;
                undo->u.deleted.data->next = *p;
                *p = undo->u.deleted.data;
                PopulateItem(acceleratorData->gd.childWindow, undo->u.deleted.data, undo->u.deleted.index);
                break;
        }
        free(undo);
        if (acceleratorData->gd.cantClearUndo)
            ResSetDirty(acceleratorData);
        else if (!acceleratorData->gd.undoData)
            ResSetClean(acceleratorData);
    }
}
LRESULT CALLBACK EditorSubclassProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    switch (iMessage)
    {
        case WM_KEYDOWN:
            if (wParam == VK_RETURN)
            {
                PostMessage((HWND)GetWindowLong(hwnd, GWL_USERDATA), WM_COMMAND, MAKEWPARAM(ID_EDIT, EN_KILLFOCUS), 0);
                return 0;
            }
            break;
        case WM_KEYUP:
            if (wParam == VK_RETURN)
                return 0;
            break;
    }
    return CallWindowProc(oldEditProc, hwnd, iMessage, wParam, lParam);
}
void AccSubclassEditWnd(HWND parent, HWND hwnd)
{
    oldEditProc = (WNDPROC)GetWindowLong(hwnd, GWL_WNDPROC);
    SetWindowLong(hwnd, GWL_WNDPROC, (long)EditorSubclassProc);
    SetWindowLong(hwnd, GWL_USERDATA, (long)parent);
}
static void AccInsert(struct resRes* acceleratorData, int index)
{
    if (index != -1)
    {
        ACCELERATOR* accelerator = rcAlloc(sizeof(ACCELERATOR));
        ResGetHeap(workArea, acceleratorData);
        if (accelerator)
        {
            int origIndex = 0;
            ACCELERATOR** p = &acceleratorData->resource->u.accelerator;
            accelerator->flags = 0;
            accelerator->id = ResAllocateMenuId();
            accelerator->key = 0;
            accelerator->skey = 1;  // CTRL-A
            for (; index && *p; index--, origIndex++, p = &(*p)->next)
                ;
            accelerator->next = *p;
            *p = accelerator;
            PopulateItem(acceleratorData->gd.childWindow, accelerator, origIndex);
            AccSetMoved(au_insert, acceleratorData, origIndex, 0);
        }
    }
}
static void AccDelete(struct resRes* acceleratorData, int index)
{
    if (index != -1)
    {
        int origIndex = index;
        if (ExtendedMessageBox("Accelerator Delete", MB_OKCANCEL, "This will delete the accelerator from the table") == IDOK)
        {
            ACCELERATOR** p = &acceleratorData->resource->u.accelerator;
            for (; index && *p; index--, p = &(*p)->next)
                ;
            if (*p)
            {
                ACCELERATOR* old = *p;
                // we don't need to free the data because it is on a heap that will be freed later
                (*p) = (*p)->next;
                ListView_DeleteItem(acceleratorData->gd.childWindow, origIndex);
                AccSetDeleted(acceleratorData, origIndex, old);
            }
        }
    }
}
LRESULT CALLBACK AccSetKeyProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    RECT r;
    HWND child;
    PAINTSTRUCT ps;
    HDC hDC;
    SIZE sz;
    HFONT xfont;
    extern LOGFONT systemDialogFont;
    static char* text = "Press the key combination to use";
    switch (iMessage)
    {
        case WM_CREATE:
            CenterWindow(hwnd);
            child = CreateWindowEx(0, "button", "Cancel", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 40, 70, 120, 40, hwnd,
                                   (HMENU)IDCANCEL, hInstance, NULL);
            ApplyDialogFont(child);
            SetWindowLong(hwnd, 0, (long)child);
            ignoreAccel = TRUE;
            break;
        case WM_DESTROY:
            DestroyWindow((HWND)GetWindowLong(hwnd, 0));
            ignoreAccel = FALSE;
            break;
        case WM_KILLFOCUS:
            break;
        case WM_PAINT:
            GetClientRect(hwnd, &r);
            hDC = BeginPaint(hwnd, &ps);
            xfont = CreateFontIndirect(&systemDialogFont);
            xfont = SelectObject(hDC, xfont);
            GetTextExtentPoint32(hDC, text, strlen(text), &sz);
            SetBkColor(hDC, GetSysColor(COLOR_BTNFACE));
            SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
            TextOut(hDC, (r.right - sz.cx) / 2, 40, text, strlen(text));
            xfont = SelectObject(hDC, xfont);
            DeleteObject(xfont);
            EndPaint(hwnd, &ps);
            break;
        case WM_KEYDOWN:
            switch (wParam)
            {
                case VK_SHIFT:
                case VK_CONTROL:
                case VK_MENU:
                    return 0;
                default:
                    if (GetKeyState(VK_CONTROL) & 0x80000000)
                        wParam |= CTL;
                    if (GetKeyState(VK_SHIFT) & 0x80000000)
                        wParam |= SHFT;
                    if (GetKeyState(VK_MENU) & 0x80000000)
                        wParam |= ALT;
                    SendMessage((HWND)GetWindowLong(hwnd, GWL_USERDATA), WM_COMMAND, IDM_ACCSETKEY, wParam);
                    SendMessage(hwnd, WM_CLOSE, 0, 0);
                    break;
            }
            return 0;
        case WM_KEYUP:
        case WM_CHAR:
            return 0;
        case WM_COMMAND:
            if (LOWORD(wParam) == IDCANCEL)
            {
                SendMessage((HWND)GetWindowLong(hwnd, GWL_USERDATA), WM_COMMAND, IDM_ACCSETKEY, -1);
                SendMessage(hwnd, WM_CLOSE, 0, 0);
            }
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
struct key
{
    int val;
    char* name;
} vkList[] = {
    {0x01, "VK_LBUTTON"},
    {0x02, "VK_RBUTTON"},
    {0x03, "VK_CANCEL"},
    {0x04, "VK_MBUTTON"},
    {0x05, "VK_XBUTTON1"},
    {0x06, "VK_XBUTTON2"},
    {0x08, "VK_BACK"},
    {0x09, "VK_TAB"},
    {0x0C, "VK_CLEAR"},
    {0x0D, "VK_RETURN"},
    {0x10, "VK_SHIFT"},
    {0x11, "VK_CONTROL"},
    {0x12, "VK_MENU"},
    {0x13, "VK_PAUSE"},
    {0x14, "VK_CAPITAL"},
    {0x15, "VK_KANA"},
    {0x15, "VK_HANGEUL"},
    {0x15, "VK_HANGUL"},
    {0x17, "VK_JUNJA"},
    {0x18, "VK_FINAL"},
    {0x19, "VK_HANJA"},
    {0x19, "VK_KANJI"},
    {0x1B, "VK_ESCAPE"},
    {0x1C, "VK_CONVERT"},
    {0x1D, "VK_NONCONVERT"},
    {0x1E, "VK_ACCEPT"},
    {0x1F, "VK_MODECHANGE"},
    {0x20, "VK_SPACE"},
    {0x21, "VK_PRIOR"},
    {0x22, "VK_NEXT"},
    {0x23, "VK_END"},
    {0x24, "VK_HOME"},
    {0x25, "VK_LEFT"},
    {0x26, "VK_UP"},
    {0x27, "VK_RIGHT"},
    {0x28, "VK_DOWN"},
    {0x29, "VK_SELECT"},
    {0x2A, "VK_PRINT"},
    {0x2B, "VK_EXECUTE"},
    {0x2C, "VK_SNAPSHOT"},
    {0x2D, "VK_INSERT"},
    {0x2E, "VK_DELETE"},
    {0x2F, "VK_HELP"},
    {0x5B, "VK_LWIN"},
    {0x5C, "VK_RWIN"},
    {0x5D, "VK_APPS"},
    {0x5F, "VK_SLEEP"},
    {0x60, "VK_NUMPAD0"},
    {0x61, "VK_NUMPAD1"},
    {0x62, "VK_NUMPAD2"},
    {0x63, "VK_NUMPAD3"},
    {0x64, "VK_NUMPAD4"},
    {0x65, "VK_NUMPAD5"},
    {0x66, "VK_NUMPAD6"},
    {0x67, "VK_NUMPAD7"},
    {0x68, "VK_NUMPAD8"},
    {0x69, "VK_NUMPAD9"},
    {0x6A, "VK_MULTIPLY"},
    {0x6B, "VK_ADD"},
    {0x6C, "VK_SEPARATOR"},
    {0x6D, "VK_SUBTRACT"},
    {0x6E, "VK_DECIMAL"},
    {0x6F, "VK_DIVIDE"},
    {0x70, "VK_F1"},
    {0x71, "VK_F2"},
    {0x72, "VK_F3"},
    {0x73, "VK_F4"},
    {0x74, "VK_F5"},
    {0x75, "VK_F6"},
    {0x76, "VK_F7"},
    {0x77, "VK_F8"},
    {0x78, "VK_F9"},
    {0x79, "VK_F10"},
    {0x7A, "VK_F11"},
    {0x7B, "VK_F12"},
    {0x7C, "VK_F13"},
    {0x7D, "VK_F14"},
    {0x7E, "VK_F15"},
    {0x7F, "VK_F16"},
    {0x80, "VK_F17"},
    {0x81, "VK_F18"},
    {0x82, "VK_F19"},
    {0x83, "VK_F20"},
    {0x84, "VK_F21"},
    {0x85, "VK_F22"},
    {0x86, "VK_F23"},
    {0x87, "VK_F24"},
    {0x90, "VK_NUMLOCK"},
    {0x91, "VK_SCROLL"},
    {0x92, "VK_OEM_NEC_EQUAL"},
    {0x92, "VK_OEM_FJ_JISHO"},
    {0x93, "VK_OEM_FJ_MASSHOU"},
    {0x94, "VK_OEM_FJ_TOUROKU"},
    {0x95, "VK_OEM_FJ_LOYA"},
    {0x96, "VK_OEM_FJ_ROYA"},
    {0xA0, "VK_LSHIFT"},
    {0xA1, "VK_RSHIFT"},
    {0xA2, "VK_LCONTROL"},
    {0xA3, "VK_RCONTROL"},
    {0xA4, "VK_LMENU"},
    {0xA5, "VK_RMENU"},
    {0xA6, "VK_BROWSER_BACK"},
    {0xA7, "VK_BROWSER_FORWARD"},
    {0xA8, "VK_BROWSER_REFRESH"},
    {0xA9, "VK_BROWSER_STOP"},
    {0xAA, "VK_BROWSER_SEARCH"},
    {0xAB, "VK_BROWSER_FAVORITES"},
    {0xAC, "VK_BROWSER_HOME"},
    {0xAD, "VK_VOLUME_MUTE"},
    {0xAE, "VK_VOLUME_DOWN"},
    {0xAF, "VK_VOLUME_UP"},
    {0xB0, "VK_MEDIA_NEXT_TRACK"},
    {0xB1, "VK_MEDIA_PREV_TRACK"},
    {0xB2, "VK_MEDIA_STOP"},
    {0xB3, "VK_MEDIA_PLAY_PAUSE"},
    {0xB4, "VK_LAUNCH_MAIL"},
    {0xB5, "VK_LAUNCH_MEDIA_SELECT"},
    {0xB6, "VK_LAUNCH_APP1"},
    {0xB7, "VK_LAUNCH_APP2"},
    {0xBA, "VK_OEM_1"},
    {0xBB, "VK_OEM_PLUS"},
    {0xBC, "VK_OEM_COMMA"},
    {0xBD, "VK_OEM_MINUS"},
    {0xBE, "VK_OEM_PERIOD"},
    {0xBF, "VK_OEM_2"},
    {0xC0, "VK_OEM_3"},
    {0xDB, "VK_OEM_4"},
    {0xDC, "VK_OEM_5"},
    {0xDD, "VK_OEM_6"},
    {0xDE, "VK_OEM_7"},
    {0xDF, "VK_OEM_8"},
    {0xE1, "VK_OEM_AX"},
    {0xE2, "VK_OEM_102"},
    {0xE3, "VK_ICO_HELP"},
    {0xE4, "VK_ICO_00"},
    {0xE5, "VK_PROCESSKEY"},
    {0xE6, "VK_ICO_CLEAR"},
    {0xE7, "VK_PACKET"},
    {0xE9, "VK_OEM_RESET"},
    {0xEA, "VK_OEM_JUMP"},
    {0xEB, "VK_OEM_PA1"},
    {0xEC, "VK_OEM_PA2"},
    {0xED, "VK_OEM_PA3"},
    {0xEE, "VK_OEM_WSCTRL"},
    {0xEF, "VK_OEM_CUSEL"},
    {0xF0, "VK_OEM_ATTN"},
    {0xF1, "VK_OEM_FINISH"},
    {0xF2, "VK_OEM_COPY"},
    {0xF3, "VK_OEM_AUTO"},
    {0xF4, "VK_OEM_ENLW"},
    {0xF5, "VK_OEM_BACKTAB"},
    {0xF6, "VK_ATTN"},
    {0xF7, "VK_CRSEL"},
    {0xF8, "VK_EXSEL"},
    {0xF9, "VK_EREOF"},
    {0xFA, "VK_PLAY"},
    {0xFB, "VK_ZOOM"},
    {0xFC, "VK_NONAME"},
    {0xFD, "VK_PA1"},
    {0xFE, "VK_OEM_CLEAR"},
};
char* GetVKName(int val)
{
    int top = sizeof(vkList) / sizeof(vkList[0]);
    int bottom = -1;
    while (top - bottom > 1)
    {
        int mid = (top + bottom) / 2;
        if (val < vkList[mid].val)
        {
            top = mid;
        }
        else
        {
            bottom = mid;
        }
    }
    if (bottom == -1)
        return 0;
    if (val != vkList[bottom].val)
        return 0;
    return vkList[bottom].name;
}
EXPRESSION* GetVKExpression(int val)
{
    EXPRESSION* exp = rcAlloc(sizeof(EXPRESSION));
    exp->type = e_int;
    exp->val = val & 0xfffffff;
    exp->rendition = GetVKName(val);
    return exp;
}
static void AccSetKey(struct resRes* acceleratorData, int val)
{
    if (acceleratorData->gd.editWindow)
    {
        if (val != -1)
        {
            int index = acceleratorData->gd.selectedRow;
            ACCELERATOR** p = &acceleratorData->resource->u.accelerator;
            ResGetHeap(workArea, acceleratorData);
            for (; index && *p; index--, p = &(*p)->next)
                ;
            if (*p)
            {
                AccSetChanged(acceleratorData, *p);
                if (!(((val & 0xfffffff) >= 'A' && (val & 0xfffffff) <= 'Z') ||
                      ((val & 0xfffffff) >= '0' && (val & 0xfffffff) <= '9')) ||
                    (val & ALT))
                {
                    EXPRESSION* exp = GetVKExpression(val & 0xfffffff);
                    (*p)->key = exp;
                    (*p)->flags |= ACC_VIRTKEY;
                    if (val & ALT)
                        (*p)->flags |= ACC_ALT;
                    else
                        (*p)->flags &= ~ACC_ALT;
                    if (val & CTL)
                        (*p)->flags |= ACC_CONTROL;
                    else
                        (*p)->flags &= ~ACC_CONTROL;
                    if (val & SHFT)
                        (*p)->flags |= ACC_SHIFT;
                    else
                        (*p)->flags &= ~ACC_SHIFT;
                    (*p)->skey = 0;
                }
                else
                {
                    (*p)->flags &= ~(ACC_SHIFT | ACC_CONTROL | ACC_VIRTKEY);
                    if (val & ALT)
                        (*p)->flags |= ACC_ALT;
                    else
                        (*p)->flags &= ~ACC_ALT;
                    if (val & CTL)
                        val &= 0x1f;
                    else if (val & SHFT)
                        val |= 0x20;
                    val &= 0xff;
                    (*p)->skey = val;
                    (*p)->key = NULL;
                }
                ListView_DeleteItem(acceleratorData->gd.childWindow, acceleratorData->gd.selectedRow);
                PopulateItem(acceleratorData->gd.childWindow, *p, acceleratorData->gd.selectedRow);
            }
        }
        acceleratorData->gd.editWindow = NULL;
    }
    else
    {
        // no window is up, make one...
        acceleratorData->gd.editWindow = CreateWindowEx(0, szAcceleratorSetKeyClassName, "", WS_VISIBLE | WS_POPUP | WS_BORDER, 0,
                                                        0, 200, 150, NULL, NULL, hInstance, NULL);
        ApplyDialogFont(acceleratorData->gd.editWindow);
        SetWindowLong(acceleratorData->gd.editWindow, GWL_USERDATA, (long)acceleratorData->activeHwnd);
    }
}
LRESULT CALLBACK AcceleratorDrawProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    static HCURSOR origCurs;
    RECT r;
    LPCREATESTRUCT createStruct;
    struct resRes* acceleratorData;
    LVHITTESTINFO hittest;
    int i;
    struct accUndo* undo;
    switch (iMessage)
    {
        case WM_MDIACTIVATE:
            if ((HWND)lParam == hwnd)
            {
                doMaximize();
            }
            break;
        case WM_SETFOCUS:
            acceleratorData = (struct resRes*)GetWindowLong(hwnd, 0);
            SetFocus(acceleratorData->gd.childWindow);
            break;
        case WM_NOTIFY:
            switch (((LPNMHDR)lParam)->code)
            {
                case LVN_GETDISPINFO:
                {
                    static char* yes = "YES";
                    static char* no = "NO";
                    static char* virtkey = "VIRTKEY";
                    static char* ascii = "ASCII";
                    static char id[256];
                    LV_DISPINFO* plvdi = (LV_DISPINFO*)lParam;
                    ACCELERATOR* accelerator;
                    plvdi->item.mask |= LVIF_TEXT | LVIF_DI_SETITEM;
                    plvdi->item.mask &= ~LVIF_IMAGE;
                    accelerator = (ACCELERATOR*)plvdi->item.lParam;
                    switch (plvdi->item.iSubItem)
                    {
                        case 1:
                            id[0] = 0;
                            FormatExp(id, accelerator->id);
                            plvdi->item.pszText = id;
                            break;
                        case 2:
                            plvdi->item.pszText = accelerator->flags & ACC_VIRTKEY ? virtkey : ascii;
                            break;
                        case 3:
                            plvdi->item.pszText = accelerator->flags & ACC_SHIFT ? yes : no;
                            break;
                        case 4:
                            plvdi->item.pszText = accelerator->flags & ACC_ALT ? yes : no;
                            break;
                        case 5:
                            plvdi->item.pszText = accelerator->flags & ACC_CONTROL ? yes : no;
                            break;
                        case 6:
                            plvdi->item.pszText = accelerator->flags & ACC_NOINVERT ? yes : no;
                            break;
                        default:
                            plvdi->item.pszText = "";
                            break;
                    }
                    break;
                }
                case LVN_BEGINDRAG:
                {
                    acceleratorData = (struct resRes*)GetWindowLong(hwnd, 0);
                    GetCursorPos(&hittest.pt);
                    ScreenToClient(acceleratorData->gd.childWindow, &hittest.pt);
                    hittest.iItem = -1;
                    ListView_HitTest(acceleratorData->gd.childWindow, &hittest);
                    if (hittest.flags & LVHT_ONITEM)
                    {
                        acceleratorData->gd.dragStart = hittest.iItem;
                        acceleratorData->gd.bDragging = TRUE;
                        acceleratorData->gd.dragInView = TRUE;
                        SetCapture(hwnd);
                        origCurs = SetCursor(dragCur);
                    }
                    break;
                }
                case LVN_KEYDOWN:
                {
                    acceleratorData = (struct resRes*)GetWindowLong(hwnd, 0);
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
                            acceleratorData->gd.selectedRow =
                                ListView_GetNextItem(acceleratorData->gd.childWindow, -1, LVNI_SELECTED);
                            PostMessage(hwnd, WM_COMMAND, IDM_INSERT, 0);
                            return TRUE;
                        case VK_DELETE:
                            acceleratorData->gd.selectedRow =
                                ListView_GetNextItem(acceleratorData->gd.childWindow, -1, LVNI_SELECTED);
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
                    break;
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
                    acceleratorData = (struct resRes*)GetWindowLong(hwnd, 0);
                    GetCursorPos(&hittest.pt);
                    pt = hittest.pt;
                    ScreenToClient(acceleratorData->gd.childWindow, &hittest.pt);
                    if (ListView_HitTest(acceleratorData->gd.childWindow, &hittest) < 0)
                    {
                        hittest.iItem = ListView_GetItemCount(acceleratorData->gd.childWindow);
                        hittest.iSubItem = 0;
                    }
                    {
                        HMENU menu, popup;
                        SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_EDIT, CBN_KILLFOCUS), 0);
                        menu = LoadMenuGeneric(hInstance, "RESACCMENU");
                        acceleratorData = (struct resRes*)GetWindowLong(hwnd, 0);
                        acceleratorData->gd.selectedRow = hittest.iItem;
                        acceleratorData->gd.selectedColumn = hittest.iSubItem;
                        popup = GetSubMenu(menu, 0);
                        InsertBitmapsInMenu(popup);
                        TrackPopupMenuEx(popup, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, hwnd, NULL);
                        DestroyMenu(menu);
                    }
                    return 1;
                }
                break;
                case NM_DBLCLK:
                    acceleratorData = (struct resRes*)GetWindowLong(hwnd, 0);
                    GetCursorPos(&hittest.pt);
                    ScreenToClient(acceleratorData->gd.childWindow, &hittest.pt);
                    if (ListView_SubItemHitTest(acceleratorData->gd.childWindow, &hittest) >= 0)
                    {
                        acceleratorData = (struct resRes*)GetWindowLong(hwnd, 0);
                        acceleratorData->gd.selectedRow = hittest.iItem;
                        acceleratorData->gd.selectedColumn = hittest.iSubItem;
                        PostMessage(hwnd, WM_HANDLEDBLCLICK, 0, 0);
                    }
                    break;
                case NM_SETFOCUS:
                    acceleratorData = (struct resRes*)GetWindowLong(hwnd, 0);
                    SetResourceProperties(acceleratorData, &accFuncs);
                    break;
                case NM_KILLFOCUS:
                    //                        SetResourceProperties(NULL, NULL);
                    break;
            }
            break;
        case WM_HANDLEDBLCLICK:
        {
            ACCELERATOR* accelerator;
            acceleratorData = (struct resRes*)GetWindowLong(hwnd, 0);
            if (acceleratorData->gd.editWindow)
            {
                DestroyWindow(acceleratorData->gd.editWindow);
                acceleratorData->gd.editWindow = NULL;
            }
            accelerator = acceleratorData->resource->u.accelerator;
            i = 0;
            while (accelerator && accelerator->next && i < acceleratorData->gd.selectedRow)
                accelerator = accelerator->next, i++;
            if (accelerator)
            {
                switch (acceleratorData->gd.selectedColumn)
                {
                    RECT r;
                    char buf[256];
                    HWND editwnd;
                    POINT pt;
                    case 0:
                        PostMessage(hwnd, WM_COMMAND, IDM_ACCSETKEY, 0);
                        break;
                    case 1:
                        ListView_GetSubItemRect(acceleratorData->gd.childWindow, acceleratorData->gd.selectedRow,
                                                acceleratorData->gd.selectedColumn, LVIR_BOUNDS, &r);
                        acceleratorData->gd.editWindow =
                            CreateWindow("edit", "", WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_BORDER, r.left, r.top,
                                         r.right - r.left, 16, hwnd, (HMENU)ID_EDIT, hInstance, NULL);
                        ApplyDialogFont(acceleratorData->gd.editWindow);
                        SetParent(acceleratorData->gd.editWindow, acceleratorData->gd.childWindow);
                        buf[0] = 0;
                        FormatExp(buf, accelerator->id);
                        SendMessage(acceleratorData->gd.editWindow, WM_SETTEXT, 0, (LPARAM)buf);
                        AccSubclassEditWnd(hwnd, acceleratorData->gd.editWindow);
                        SendMessage(acceleratorData->gd.editWindow, EM_SETSEL, 0, -1);
                        SetFocus(acceleratorData->gd.editWindow);
                        break;
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                        ListView_GetSubItemRect(acceleratorData->gd.childWindow, acceleratorData->gd.selectedRow,
                                                acceleratorData->gd.selectedColumn, LVIR_BOUNDS, &r);
                        acceleratorData->gd.editWindow =
                            CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | CBS_DROPDOWN | WS_CLIPSIBLINGS | WS_BORDER, r.left,
                                         r.top, r.right - r.left, 50, hwnd, (HMENU)ID_EDIT, hInstance, NULL);
                        ApplyDialogFont(acceleratorData->gd.editWindow);
                        SetParent(acceleratorData->gd.editWindow, acceleratorData->gd.childWindow);
                        pt.x = pt.y = 5;
                        editwnd = ChildWindowFromPoint(acceleratorData->gd.editWindow, pt);
                        SendMessage(editwnd, EM_SETREADONLY, 0, 0);
                        AccSubclassEditWnd(hwnd, editwnd);
                        switch (acceleratorData->gd.selectedColumn)
                        {
                            case 2:
                                SendMessage(acceleratorData->gd.editWindow, CB_ADDSTRING, 0, (LPARAM) "ASCII");
                                SendMessage(acceleratorData->gd.editWindow, CB_ADDSTRING, 0, (LPARAM) "VIRTKEY");
                                SendMessage(acceleratorData->gd.editWindow, CB_SETCURSEL, accelerator->flags & ACC_VIRTKEY ? 1 : 0,
                                            0);
                                break;
                            case 3:
                            case 4:
                            case 5:
                            case 6:
                                SendMessage(acceleratorData->gd.editWindow, CB_ADDSTRING, 0, (LPARAM) "No");
                                SendMessage(acceleratorData->gd.editWindow, CB_ADDSTRING, 0, (LPARAM) "Yes");
                                switch (acceleratorData->gd.selectedColumn)
                                {
                                    case 3:
                                        SendMessage(acceleratorData->gd.editWindow, CB_SETCURSEL,
                                                    accelerator->flags & ACC_SHIFT ? 1 : 0, 0);
                                        break;
                                    case 4:
                                        SendMessage(acceleratorData->gd.editWindow, CB_SETCURSEL,
                                                    accelerator->flags & ACC_ALT ? 1 : 0, 0);
                                        break;
                                    case 5:
                                        SendMessage(acceleratorData->gd.editWindow, CB_SETCURSEL,
                                                    accelerator->flags & ACC_CONTROL ? 1 : 0, 0);
                                        break;
                                    case 6:
                                        SendMessage(acceleratorData->gd.editWindow, CB_SETCURSEL,
                                                    accelerator->flags & ACC_NOINVERT ? 1 : 0, 0);
                                        break;
                                }
                                break;
                        }
                        break;
                }
            }
        }
        break;
        case WM_MOUSEMOVE:
            acceleratorData = (struct resRes*)GetWindowLong(hwnd, 0);
            if (acceleratorData->gd.bDragging)
            {
                POINT pt;
                GetClientRect(hwnd, &r);
                pt.x = (long)(short)LOWORD(lParam);
                pt.y = (long)(short)HIWORD(lParam);
                hittest.pt = pt;
                ClientToScreen(hwnd, &pt);
                if (WindowFromPoint(pt) == acceleratorData->gd.childWindow)
                {
                    SetCursor(dragCur);
                    acceleratorData->gd.dragInView = TRUE;
                }
                else
                {
                    SetCursor(noCur);
                    acceleratorData->gd.dragInView = FALSE;
                }
                ListView_HitTest(acceleratorData->gd.childWindow, &hittest);
                if (hittest.pt.y < 0)
                {
                    if (!acceleratorData->gd.lvscroll)
                        SetTimer(hwnd, 100, 300, NULL);
                    acceleratorData->gd.lvscroll = 1;
                }
                else if (hittest.pt.y >= r.bottom)
                {
                    if (!acceleratorData->gd.lvscroll)
                        SetTimer(hwnd, 100, 300, NULL);
                    acceleratorData->gd.lvscroll = 2;
                }
                else
                {
                    if (acceleratorData->gd.lvscroll)
                        KillTimer(hwnd, 100);
                    acceleratorData->gd.lvscroll = 0;
                }
            }
            break;
        case WM_TIMER:
            acceleratorData = (struct resRes*)GetWindowLong(hwnd, 0);
            ListView_Scroll(acceleratorData->gd.childWindow, 0, (acceleratorData->gd.lvscroll & 1) ? -16 : 16);
            break;
        case WM_LBUTTONUP:
            acceleratorData = (struct resRes*)GetWindowLong(hwnd, 0);
            if (acceleratorData->gd.bDragging)
            {
                hittest.pt.x = (long)(short)LOWORD(lParam);
                hittest.pt.y = (long)(short)HIWORD(lParam);
                if (acceleratorData->gd.lvscroll)
                    KillTimer(hwnd, 100);
                acceleratorData->gd.lvscroll = 0;
                GetCursorPos(&hittest.pt);
                ScreenToClient(acceleratorData->gd.childWindow, &hittest.pt);
                ReleaseCapture();
                SetCursor(origCurs);
                ListView_HitTest(acceleratorData->gd.childWindow, &hittest);
                if (hittest.flags & LVHT_NOWHERE)
                {
                    hittest.iItem = ListView_GetItemCount(acceleratorData->gd.childWindow);
                }
                if (hittest.flags & LVHT_ONITEM || ((hittest.flags & LVHT_NOWHERE) && acceleratorData->gd.dragInView))
                {
                    int dragEnd = hittest.iItem;
                    acceleratorData = (struct resRes*)GetWindowLong(hwnd, 0);
                    if (dragEnd != acceleratorData->gd.dragStart)
                    {
                        ACCELERATOR** p = &acceleratorData->resource->u.accelerator;
                        int i;
                        for (i = 0; *p && i < acceleratorData->gd.dragStart; i++, p = &(*p)->next)
                            ;
                        if (*p)
                        {
                            ACCELERATOR* hold = *p;
                            *p = (*p)->next;
                            p = &acceleratorData->resource->u.accelerator;
                            for (i = 0; *p && i < dragEnd; i++, p = &(*p)->next)
                                ;
                            hold->next = *p;
                            *p = hold;
                            ListView_DeleteItem(acceleratorData->gd.childWindow, acceleratorData->gd.dragStart);
                            PopulateItem(acceleratorData->gd.childWindow, hold, dragEnd);
                            AccSetMoved(au_move, acceleratorData, acceleratorData->gd.dragStart, dragEnd);
                        }
                    }
                }
            }
            break;
        case WM_COMMAND:
            acceleratorData = (struct resRes*)GetWindowLong(hwnd, 0);
            switch (LOWORD(wParam))
            {
                case ID_EDIT:
                    if (HIWORD(wParam) == CBN_KILLFOCUS || HIWORD(wParam) == EN_KILLFOCUS)
                    {
                        static BOOL inKillFocus;
                        if (acceleratorData->gd.editWindow && !inKillFocus)
                        {
                            ACCELERATOR* accelerator = acceleratorData->resource->u.accelerator;
                            i = 0;
                            while (accelerator && accelerator->next && i < acceleratorData->gd.selectedRow)
                                accelerator = accelerator->next, i++;
                            if (accelerator)
                            {
                                if (acceleratorData->gd.selectedColumn == 1)
                                {
                                    char buf[256];
                                    buf[GetWindowText(acceleratorData->gd.editWindow, buf, sizeof(buf) - 1)] = 0;
                                    AccSetChanged(acceleratorData, accelerator);
                                    PropSetIdName(acceleratorData, buf, &accelerator->id, NULL, TRUE);
                                    ResAddNewDef(buf, accelerator->id->val);
                                }
                                else if (acceleratorData->gd.selectedColumn >= 2)
                                {
                                    i = SendMessage(acceleratorData->gd.editWindow, CB_GETCURSEL, 0, 0);
                                    if (i != CB_ERR)
                                    {
                                        int flags = accelerator->flags;
                                        switch (acceleratorData->gd.selectedColumn)
                                        {
                                            case 2:
                                                if (i)
                                                    flags |= ACC_VIRTKEY;
                                                else
                                                    flags &= ~ACC_VIRTKEY;
                                                break;
                                            case 3:
                                                if (i)
                                                    flags |= ACC_SHIFT;
                                                else
                                                    flags &= ~ACC_SHIFT;
                                                break;
                                            case 4:
                                                if (i)
                                                    flags |= ACC_ALT;
                                                else
                                                    flags &= ~ACC_ALT;
                                                break;
                                            case 5:
                                                if (i)
                                                    flags |= ACC_CONTROL;
                                                else
                                                    flags &= ~ACC_CONTROL;
                                                break;
                                            case 6:
                                                if (i)
                                                    flags |= ACC_NOINVERT;
                                                else
                                                    flags &= ~ACC_NOINVERT;
                                                break;
                                        }
                                        if (flags != accelerator->flags)
                                        {
                                            AccSetChanged(acceleratorData, accelerator);
                                            accelerator->flags = flags;
                                        }
                                    }
                                }
                                ListView_DeleteItem(acceleratorData->gd.childWindow, acceleratorData->gd.selectedRow);
                                PopulateItem(acceleratorData->gd.childWindow, accelerator, acceleratorData->gd.selectedRow);
                            }
                            inKillFocus = TRUE;
                            DestroyWindow(acceleratorData->gd.editWindow);
                            inKillFocus = FALSE;
                            acceleratorData->gd.editWindow = NULL;
                        }
                    }
                    break;
                case IDM_UNDO:
                    AccDoUndo(acceleratorData);
                    break;
                case IDM_SAVE:
                    if (acceleratorData->resource->changed)
                    {
                        ResSaveCurrent(workArea, acceleratorData);
                    }
                    break;
                case IDM_INSERT:
                    AccInsert(acceleratorData, acceleratorData->gd.selectedRow);
                    break;
                case IDM_DELETE:
                    AccDelete(acceleratorData, acceleratorData->gd.selectedRow);
                    ListView_SetItemState(acceleratorData->gd.childWindow, acceleratorData->gd.selectedRow, LVIS_SELECTED,
                                          LVIS_SELECTED);
                    break;
                case IDM_ACCSETKEY:
                    AccSetKey(acceleratorData, lParam);
                    break;
            }
            break;
        case EM_CANUNDO:
            acceleratorData = (struct resRes*)GetWindowLong(hwnd, 0);
            return acceleratorData->gd.undoData != NULL;
        case WM_NCACTIVATE:
            PaintMDITitleBar(hwnd, iMessage, wParam, lParam);
            return TRUE;
        case WM_NCPAINT:
            return PaintMDITitleBar(hwnd, iMessage, wParam, lParam);
        case WM_CREATE:
            SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
            GetClientRect(hwnd, &r);
            createStruct = (LPCREATESTRUCT)lParam;
            acceleratorData = (struct resRes*)((LPMDICREATESTRUCT)(createStruct->lpCreateParams))->lParam;
            SetWindowLong(hwnd, 0, (long)acceleratorData);
            acceleratorData->activeHwnd = hwnd;
            acceleratorData->gd.childWindow = CreateWindowEx(0, WC_LISTVIEW, "", WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SINGLESEL,
                                                             0, 0, r.right, r.bottom, hwnd, (HMENU)ID_TREEVIEW, hInstance, NULL);
            SetListViewColumns(hwnd, acceleratorData->gd.childWindow);
            PopulateKeys(hwnd, acceleratorData);
            InsertRCWindow(hwnd);
            break;
        case WM_CLOSE:
            RemoveRCWindow(hwnd);
            SendMessage(hwndSrcTab, TABM_REMOVE, 0, (LPARAM)hwnd);
            break;
        case WM_DESTROY:
            acceleratorData = (struct resRes*)GetWindowLong(hwnd, 0);
            if (acceleratorData->gd.editWindow)
                DestroyWindow(acceleratorData->gd.editWindow);
            acceleratorData->gd.editWindow = NULL;
            if (acceleratorData->gd.bDragging)
                ReleaseCapture();
            undo = acceleratorData->gd.undoData;
            if (undo)
                acceleratorData->gd.cantClearUndo = TRUE;
            while (undo)
            {
                struct accUndo* next = undo->next;
                free(undo);
                undo = next;
            }
            acceleratorData->gd.undoData = NULL;
            acceleratorData->activeHwnd = NULL;
            break;
        case WM_SIZE:
            acceleratorData = (struct resRes*)GetWindowLong(hwnd, 0);
            MoveWindow(acceleratorData->gd.childWindow, 0, 0, LOWORD(lParam), HIWORD(lParam), 1);
            break;
        default:
            break;
    }
    return DefMDIChildProc(hwnd, iMessage, wParam, lParam);
}

void RegisterAcceleratorDrawWindow(HINSTANCE hInstance)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = &AcceleratorDrawProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(void*) * 2;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szAcceleratorDrawClassName;
    RegisterClass(&wc);

    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = &AccSetKeyProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(void*) * 2;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szAcceleratorSetKeyClassName;
    RegisterClass(&wc);

    dragCur = LoadCursor(hInstance, "ID_DRAGCTL");
    noCur = LoadCursor(hInstance, "ID_NODRAGCUR");
}
void CreateAcceleratorDrawWindow(struct resRes* info)
{
    char name[512];
    int maximized;
    HWND hwnd;
    sprintf(name, "%s - %s", szUntitled, info->name);
    SendMessage(hwndClient, WM_MDIGETACTIVE, 0, (LPARAM)&maximized);
    hwnd =
        CreateMDIWindow(szAcceleratorDrawClassName, name,
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