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
extern struct propertyFuncs accFuncs;

static char *szStringTableDrawClassName = "xccStringTableDrawClass";
static char *szUntitled = "StringTable";

static HCURSOR dragCur, noCur;
static WNDPROC oldEditProc;
EXPRESSION *ResAllocateStringId();
HANDLE ResGetHeap(PROJECTITEM *wa, struct resRes *data);
EXPRESSION *ResReadExp(struct resRes *data, char *buf);

struct stringTableUndo
{
    struct stringUndo *next;
    enum { au_nop, au_insert, au_delete, au_setchars } type;
    union
    {
        struct
        {
            int index;
            EXPRESSION *id;
            WCHAR *text;
            int length;
        } values;
        struct {
            int index;
            STRINGS *data;
        } insdel;
        
    } u;
};

static void SetListViewColumns(HWND hwndParent, HWND hwnd)
{
    RECT r;
    LV_COLUMN lvC;
    ListView_SetExtendedListViewStyle(hwnd, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

    GetClientRect(hwnd, &r);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM  | LVCF_TEXT;
    lvC.cx = 180;
    lvC.iSubItem = 0;
    lvC.pszText = "Id";
    ListView_InsertColumn(hwnd, 0, &lvC);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM  | LVCF_TEXT;
    lvC.cx = r.right - 180;
    lvC.iSubItem = 1;
    lvC.pszText = "String";
    ListView_InsertColumn(hwnd, 1, &lvC);
}
static void PopulateItem(HWND hwnd, STRINGS *strings, int items)
{
    LV_ITEM item;
    static char id[256];
    id[0] = 0;
    FormatExp(id, strings->id);
    memset(&item, 0, sizeof(LV_ITEM));
    item.iItem = items;
    item.iSubItem = 0;
    item.mask = LVIF_PARAM | LVIF_TEXT;
    item.lParam = (LPARAM)strings;
    item.pszText = id;
    ListView_InsertItem(hwnd, &item);
    item.iSubItem = 1;
    item.mask = LVIF_PARAM;
    item.lParam = (LPARAM)strings;
    ListView_SetItem(hwnd, &item);
}
static void PopulateStrings(HWND hwnd, struct resRes *stringTableData)
{
    int items = 0;
    STRINGS *strings = stringTableData->resource->u.stringtable;
    while (strings)
    {
        PopulateItem(stringTableData->gd.childWindow, strings, items++);
        strings = strings->next;
    }
}
static void StringTableSetChanged(struct resRes *stringTableData, STRINGS *strings)
{
    struct stringTableUndo *newUndo = calloc(1, sizeof(struct stringTableUndo));
    if (newUndo)
    {
        newUndo->type = au_setchars;
        newUndo->u.values.index = stringTableData->gd.selectedRow;
        newUndo->u.values.id = strings->id;
        newUndo->u.values.text = strings->string;
        newUndo->u.values.length = strings->length;
        newUndo->next = stringTableData->gd.undoData;
        stringTableData->gd.undoData = newUndo;
        ResSetDirty(stringTableData);
    }
}
// for move, insert
static void StringTableSetInserted(struct resRes *stringTableData, int at)
{
    struct stringTableUndo *newUndo = calloc(1, sizeof(struct stringTableUndo));
    if (newUndo)
    {
        newUndo->type = au_insert;
        newUndo->u.insdel.index = at;
        newUndo->next = stringTableData->gd.undoData;
        stringTableData->gd.undoData = newUndo;
        ResSetDirty(stringTableData);
    }
}
static void StringTableSetDeleted(struct resRes * stringTableData, int from, STRINGS *data)
{
    struct stringTableUndo *newUndo = calloc(1, sizeof(struct stringTableUndo));
    if (newUndo)
    {
        newUndo->type = au_delete;
        newUndo->u.insdel.index = from;
        newUndo->u.insdel.data = data;
        newUndo->next = stringTableData->gd.undoData;
        stringTableData->gd.undoData = newUndo;
        ResSetDirty(stringTableData);
    }
}
static void StringTableInsert(struct resRes *stringTableData, int index)
{
    if (index != -1)
    {
        STRINGS *strings = rcAlloc(sizeof(STRINGS));
        ResGetHeap(workArea, stringTableData);
        if (strings)
        {
            int origIndex = 0;
            STRINGS **p = &stringTableData->resource->u.stringtable;
            strings->id = ResAllocateStringId();
            strings->length = StringAsciiToWChar(&strings->string, " ", 1);
            for ( ; index && *p; index--, origIndex++, p = &(*p)->next);
            strings->next = *p;
            *p = strings;
            PopulateItem(stringTableData->gd.childWindow, strings, origIndex);
            StringTableSetInserted(stringTableData, origIndex);
        }
    }
}
static void StringTableDelete(struct resRes *stringTableData, int index)
{
    if (index != -1)
    {
        int origIndex = index;
        if (ExtendedMessageBox("String Delete", MB_OKCANCEL, "This will delete the string from the table") == IDOK)
        {
            STRINGS **p = &stringTableData->resource->u.stringtable;
            for ( ; index && *p; index--, p = &(*p)->next);
            if (*p)
            {
                STRINGS *old = *p;
                // we don't need to free the data because it is on a heap that will be freed later
                (*p) = (*p)->next;
                ListView_DeleteItem(stringTableData->gd.childWindow, origIndex);
                StringTableSetDeleted(stringTableData, origIndex, old);
            }
        }
    }
}
static void StringTableUndo(HWND hwnd, struct resRes *stringTableData)
{
    struct stringTableUndo *undo = stringTableData->gd.undoData;
    if (undo)
    {
        stringTableData->gd.undoData = undo->next;
        switch(undo->type)
        {
            STRINGS **p;
            STRINGS *hold;
            int i;
            case au_setchars:
                p = &stringTableData->resource->u.stringtable;
                for(i=0; i < undo->u.values.index && *p; i++, p = &(*p)->next);
                if (*p)
                {
                    (*p)->id = undo->u.values.id;
                    (*p)->string = undo->u.values.text;
                    (*p)->length = undo->u.values.length;
                    ListView_DeleteItem(stringTableData->gd.childWindow, undo->u.values.index);
                    PopulateItem(stringTableData->gd.childWindow, *p, undo->u.values.index);
                }
                break;
            case au_insert:
                p = &stringTableData->resource->u.stringtable;
                for(i=0; i < undo->u.insdel.index && *p; i++, p = &(*p)->next);
                *p = (*p)->next;
                 ListView_DeleteItem(stringTableData->gd.childWindow, undo->u.insdel.index);
                
                break;
            case au_delete:
                p = &stringTableData->resource->u.stringtable;
                for(i=0; i < undo->u.insdel.index && *p; i++, p = &(*p)->next);
                undo->u.insdel.data->next = *p;
                *p = undo->u.insdel.data;
                PopulateItem(stringTableData->gd.childWindow, undo->u.insdel.data, undo->u.insdel.index);
                break;
            
        }
        free(undo);
        if (stringTableData->gd.cantClearUndo)
            ResSetDirty(stringTableData);
        else if (!stringTableData->gd.undoData)
            ResSetClean(stringTableData);
    }
}
LRESULT CALLBACK StringTableDrawProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    static HCURSOR origCurs;
    RECT r;
    LPCREATESTRUCT createStruct;
    struct resRes *stringTableData;
    LVHITTESTINFO hittest;
    int i;
    struct stringTableUndo *undo;
    switch (iMessage)
    {
        case WM_MDIACTIVATE:
            if ((HWND)lParam == hwnd)
            {
                doMaximize();
            }
            break;
        case WM_SETFOCUS:
            stringTableData = (struct resRes *)GetWindowLong(hwnd, 0);
            SetFocus(stringTableData->gd.childWindow);
            break;
        case WM_NOTIFY:
                switch (((LPNMHDR)lParam)->code)
                {
                    case LVN_GETDISPINFO:
                    {
                        LV_DISPINFO *plvdi = (LV_DISPINFO*)lParam;
                        STRINGS *strings;
                        plvdi->item.mask |= LVIF_TEXT | LVIF_DI_SETITEM;
                        plvdi->item.mask &= ~LVIF_IMAGE;
                        strings = (STRINGS *)plvdi->item.lParam;
                        switch (plvdi->item.iSubItem)
                        {
                            char *id;
                            case 1:
								id = (char *)LocalAlloc(0, 256);
                                FormatVersionString(id, strings->string, strings->length);
                                plvdi->item.pszText = id;
                                break;
                        }
                        break;
                    }
                    case LVN_KEYDOWN:
                    {
                        stringTableData = (struct resRes *)GetWindowLong(hwnd, 0);
                        SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_EDIT, CBN_KILLFOCUS), 0);
                        switch (((LPNMLVKEYDOWN)lParam)->wVKey)
                        {
                            case 'S':
                                if (GetKeyState(VK_CONTROL) &0x80000000)
                                {
                                    PostMessage(hwnd, WM_COMMAND, IDM_SAVE, 0);
                                    return TRUE;
                                }
                                
                                break;
                            case 'Z':
                                if (GetKeyState(VK_CONTROL) &0x80000000)
                                {
                                    PostMessage(hwnd, WM_COMMAND, IDM_UNDO, 0);
                                    return TRUE;
                                }
                                break;
                            case VK_INSERT:
                                stringTableData->gd.selectedRow = ListView_GetNextItem(stringTableData->gd.childWindow, -1, LVNI_SELECTED);
                                PostMessage(hwnd, WM_COMMAND, IDM_INSERT, 0);
                                return TRUE;
                            case VK_DELETE:
                                stringTableData->gd.selectedRow = ListView_GetNextItem(stringTableData->gd.childWindow, -1, LVNI_SELECTED);
                                PostMessage(hwnd, WM_COMMAND, IDM_DELETE, 0);
                                return TRUE;
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
                        stringTableData = (struct resRes *)GetWindowLong(hwnd, 0);
                        GetCursorPos(&hittest.pt);
                        pt = hittest.pt;
                        ScreenToClient(stringTableData->gd.childWindow, &hittest.pt);
                        if (ListView_HitTest(stringTableData->gd.childWindow, &hittest) < 0)
                        {
                            hittest.iItem = ListView_GetItemCount(stringTableData->gd.childWindow);
                            hittest.iSubItem = 0;
                        }
                        {
                            HMENU menu, popup;
                            SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_EDIT, CBN_KILLFOCUS), 0);
                            menu = LoadMenuGeneric(hInstance, "RESSTRINGSMENU");
                            stringTableData = (struct resRes *)GetWindowLong(hwnd, 0);
                            stringTableData->gd.selectedRow = hittest.iItem;
                            stringTableData->gd.selectedColumn = hittest.iSubItem;
                            popup = GetSubMenu(menu, 0);
                            InsertBitmapsInMenu(popup);
                            TrackPopupMenuEx(popup, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x,
                                pt.y, hwnd, NULL);
                            DestroyMenu(menu);
                        }
                        return 1;
                    }
                        break;
                    case NM_DBLCLK:
                        stringTableData = (struct resRes *)GetWindowLong(hwnd, 0);
                        GetCursorPos(&hittest.pt);
                        ScreenToClient(stringTableData->gd.childWindow, &hittest.pt);
                        if (ListView_SubItemHitTest(stringTableData->gd.childWindow, &hittest) >= 0)
                        {
                            stringTableData = (struct resRes *)GetWindowLong(hwnd, 0);
                            stringTableData->gd.selectedRow = hittest.iItem;
                            stringTableData->gd.selectedColumn = hittest.iSubItem;
                            PostMessage(hwnd, WM_HANDLEDBLCLICK, 0, 0);
                        }
                        break;
                    case NM_SETFOCUS:
                        stringTableData = (struct resRes *)GetWindowLong(hwnd, 0);
                        SetResourceProperties(stringTableData, &accFuncs);
                        break;
                    case NM_KILLFOCUS:
//                        SetResourceProperties(NULL, NULL);
                        break;
                }
            break;
        case WM_HANDLEDBLCLICK:
        {
            STRINGS *strings;
            RECT r;
            stringTableData = (struct resRes *)GetWindowLong(hwnd, 0);
            if (stringTableData->gd.editWindow)
            {
                DestroyWindow(stringTableData->gd.editWindow);
                stringTableData->gd.editWindow = NULL;
            }
            strings = stringTableData->resource->u.stringtable;
            i = 0;
            while (strings && strings->next && i < stringTableData->gd.selectedRow)
                strings = strings->next, i++;
            if (strings)
            {
                ListView_GetSubItemRect(stringTableData->gd.childWindow, 
                                        stringTableData->gd.selectedRow,
                                        stringTableData->gd.selectedColumn,
                                        LVIR_BOUNDS, &r);
                stringTableData->gd.editWindow = CreateWindow("edit", "", WS_VISIBLE |
                    WS_CHILD | WS_CLIPSIBLINGS | WS_BORDER,
                    r.left,r.top,r.right-r.left,16, hwnd, (HMENU)ID_EDIT,
                    hInstance, NULL);
				ApplyDialogFont(stringTableData->gd.editWindow);
                SetParent(stringTableData->gd.editWindow, stringTableData->gd.childWindow);
                AccSubclassEditWnd(hwnd, stringTableData->gd.editWindow);
                switch(stringTableData->gd.selectedColumn)
                {
                    char buf[256];
                    case 0:
                        buf[0] = 0;
                        FormatExp(buf, strings->id);
                        SendMessage(stringTableData->gd.editWindow, WM_SETTEXT, 0, (LPARAM)buf);
                        break;
                    case 1:
                        FormatVersionString(buf, strings->string, strings->length);
                        buf[strlen(buf)-1] = 0;
                        SendMessage(stringTableData->gd.editWindow, WM_SETTEXT, 0, (LPARAM)buf+1);
                        break;
                }
                SendMessage(stringTableData->gd.editWindow, EM_SETSEL, 0, -1);
                SetFocus(stringTableData->gd.editWindow);
            }
        }
            break;
        case WM_TIMER:
            stringTableData = (struct resRes *)GetWindowLong(hwnd, 0);
            ListView_Scroll(stringTableData->gd.childWindow, 0,
                           (stringTableData->gd.lvscroll & 1) ? -16 : 16);
            break;
        case WM_COMMAND:
            stringTableData = (struct resRes *)GetWindowLong(hwnd, 0);
            switch (LOWORD(wParam))
            {
                case ID_EDIT:
                    if (HIWORD(wParam) == CBN_KILLFOCUS || HIWORD(wParam) == EN_KILLFOCUS)
                    {
                        static BOOL inKillFocus;
                        if (stringTableData->gd.editWindow && !inKillFocus)
                        {
                            STRINGS *strings = stringTableData->resource->u.stringtable;
                            i = 0;
                            while (strings && strings->next && i < stringTableData->gd.selectedRow)
                                strings = strings->next,i++;
                            if (strings)
                            {
                                char buf[256];
                                buf[GetWindowText(stringTableData->gd.editWindow, buf, sizeof(buf)-1)] = 0;
                                StringTableSetChanged(stringTableData, strings);
                                if (stringTableData->gd.selectedColumn == 0)
                                {
                                    PropSetIdName(stringTableData, buf, &strings->id, NULL);
								    ResAddNewDef(buf, strings->id->val);
                                }
                                else
                                {
                                    int len;
                                    char *p = ParseVersionString(buf, &len);
                                    strings->length = StringAsciiToWChar(&strings->string, p, len);
                                    ResGetStringItemName(strings->id, p);
                                }
                                ListView_DeleteItem(stringTableData->gd.childWindow, stringTableData->gd.selectedRow);
                                PopulateItem(stringTableData->gd.childWindow, strings, stringTableData->gd.selectedRow);
                            }
                            i = 0;
                            inKillFocus = TRUE;
                            DestroyWindow(stringTableData->gd.editWindow);
                            inKillFocus = FALSE;
                            stringTableData->gd.editWindow = NULL;
                        }
                    }
                    break;
                case IDM_INSERT:
                    StringTableInsert(stringTableData, stringTableData->gd.selectedRow);
                    break;
                case IDM_DELETE:
                    StringTableDelete(stringTableData, stringTableData->gd.selectedRow);
					ListView_SetItemState(stringTableData->gd.childWindow, stringTableData->gd.selectedRow, LVIS_SELECTED, LVIS_SELECTED);
                    break;
                case IDM_SAVE:
                    if (stringTableData->resource->changed)
                    {
                         ResSaveCurrent(workArea, stringTableData);
                    }
                    break;
                case IDM_UNDO:
                    StringTableUndo(hwnd, stringTableData);
                    break;
            }
            break;
        case EM_CANUNDO:
            stringTableData = (struct resRes *)GetWindowLong(hwnd, 0);
            return stringTableData->gd.undoData != NULL;
        case WM_CREATE:
            GetClientRect(hwnd, &r);
            createStruct = (LPCREATESTRUCT)lParam;
            stringTableData = (struct resRes *)((LPMDICREATESTRUCT)(createStruct->lpCreateParams))->lParam;
            SetWindowLong(hwnd, 0, (long)stringTableData);
            stringTableData->activeHwnd = hwnd;
            stringTableData->gd.childWindow = CreateWindowEx(0, WC_LISTVIEW, "", WS_VISIBLE |
                WS_CHILD | LVS_REPORT | LVS_SINGLESEL,
                0, 0, r.right, r.bottom, hwnd, (HMENU)ID_TREEVIEW,
                hInstance, NULL);
            SetListViewColumns(hwnd, stringTableData->gd.childWindow);
            PopulateStrings(hwnd, stringTableData );
            break;
        case WM_CLOSE:
            SendMessage(hwndSrcTab, TABM_REMOVE, 0, (LPARAM)hwnd);
            break;
        case WM_DESTROY:
            stringTableData = (struct resRes *)GetWindowLong(hwnd, 0);
            if (stringTableData->gd.editWindow)
                DestroyWindow(stringTableData->gd.editWindow);
            stringTableData->gd.editWindow = NULL;
            undo = stringTableData->gd.undoData;
            if (undo)
                stringTableData->gd.cantClearUndo = TRUE;
            while (undo)
            {
                struct stringTableUndo *next = undo->next;
                free(undo);
                undo = next;
            }
            stringTableData->gd.undoData = NULL;
            stringTableData->activeHwnd = NULL;
            break;
        case WM_SIZE:
            stringTableData = (struct resRes *)GetWindowLong(hwnd, 0);
            MoveWindow(stringTableData->gd.childWindow, 0, 0, LOWORD(lParam), HIWORD(lParam), 1);
            break;
        default:
            break;
    }
    return DefMDIChildProc(hwnd, iMessage, wParam, lParam);
}

void RegisterStringTableDrawWindow(void)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = &StringTableDrawProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(void*) * 2;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szStringTableDrawClassName;
    RegisterClass(&wc);
    
    
}
void CreateStringTableDrawWindow(struct resRes *info)
{
    char name[512];
    int maximized;
    HWND hwnd;
    sprintf(name, "%s - %s", szUntitled, info->name);
    SendMessage(hwndClient, WM_MDIGETACTIVE, 0, (LPARAM) &maximized);
    hwnd = CreateMDIWindow(szStringTableDrawClassName, name, WS_VISIBLE |
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