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

#define STRICT
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <richedit.h>
#include <stdio.h>
#include <ctype.h>

#include "header.h"

extern enum DebugState uState;

extern PROCESS* activeProcess;
extern DATABREAK* dataBpList;
extern HDWEBKPT hdwebp[4];
extern HANDLE hInstance;
extern HWND hwndClient, hwndFrame;
extern LOGFONT systemDialogFont;
extern struct tagfile* tagFileList;

static char szBreakClassName[] = "xccBreakClass";
static HWND hwndLV;
static BOOL loading;

static char* szBreakTitle = "Breakpoints";

typedef struct _bpdata
{
    struct _bpdata* next;
    enum
    {
        bt_code,
        bt_data,
        bt_hdwe
    } type;
    union
    {
        struct tag* c;
        HDWEBKPT* h;
        DATABREAK* d;
    } u;
    char size[20];
    char name[256];
    char module[256];
} BPDATA;

static BPDATA* bpData;

static void ClearBPWind()
{
    while (bpData)
    {
        BPDATA* next = bpData->next;
        free(bpData);
        bpData = next;
    }
    ListView_DeleteAllItems(hwndLV);
}
static void LoadBPWind(void)
{
    int i, v;
    LV_ITEM item;
    DATABREAK* q = dataBpList;
    struct tagfile* p = tagFileList;
    int curSel = 0;
    loading = TRUE;
    while (p)
    {
        if (p->tagArray[TAG_BP])
        {
            struct tag* t = p->tagArray[TAG_BP];
            while (t)
            {
                BPDATA* cur = (BPDATA*)calloc(sizeof(BPDATA), 1);
                char* str = strrchr(p->name, '\\');
                int linenum = t->debugLineno;
                int n = GetBreakpointNearestLine(p->name, t->debugLineno, TRUE);
                int* addresses = uState != notDebugging ? GetBreakpointAddresses(p->name, &linenum) : 0;
                int eip;
                if (addresses)
                {
                    eip = addresses[0];
                    free(addresses);
                    n = FindFunctionName(cur->name, eip, NULL, NULL);
                    sprintf(cur->name + strlen(cur->name), " + 0x%x", eip - n);
                }
                else
                {
                    eip = 0;
                    strcpy(cur->name, "unknown");
                }
                if (!str)
                    str = p->name;
                else
                    str++;
                strcpy(cur->module, p->name);
                cur->u.c = t;
                cur->type = bt_code;
                cur->next = bpData;
                bpData = cur;

                item.iItem = curSel;
                item.iSubItem = 0;
                item.mask = LVIF_PARAM;
                item.lParam = (LPARAM)cur;
                item.pszText = "";
                v = ListView_InsertItem(hwndLV, &item);
                ListView_SetCheckState(hwndLV, v, !t->disable);

                item.iItem = curSel;
                item.iSubItem = 1;
                item.mask = LVIF_TEXT;
                item.pszText = cur->name;

                ListView_SetItem(hwndLV, &item);
                item.iItem = curSel;
                item.iSubItem = 2;
                item.mask = LVIF_TEXT;
                item.pszText = "code";

                ListView_SetItem(hwndLV, &item);
                item.iItem = curSel;
                item.iSubItem = 3;
                item.mask = LVIF_TEXT;
                item.pszText = "1";  // size

                ListView_SetItem(hwndLV, &item);
                item.iItem = curSel;
                item.iSubItem = 4;
                ;
                item.mask = LVIF_TEXT;
                sprintf(cur->size, "%d", t->drawnLineno);
                item.pszText = cur->size;  // line

                ListView_SetItem(hwndLV, &item);
                item.iItem = curSel;
                item.iSubItem = 5;
                item.mask = LVIF_TEXT;
                item.pszText = str;
                ListView_SetItem(hwndLV, &item);
                curSel++;
                t = t->next;
            }
        }
        p = p->next;
    }
    while (q)
    {
        BPDATA* cur = (BPDATA*)calloc(sizeof(BPDATA), 1);
        cur->u.c = q;
        cur->type = bt_data;
        cur->next = bpData;
        bpData = cur;
        item.iItem = curSel;
        item.iSubItem = 0;
        item.mask = LVIF_PARAM;
        item.lParam = (LPARAM)cur;
        item.pszText = "";
        v = ListView_InsertItem(hwndLV, &item);
        ListView_SetCheckState(hwndLV, v, !q->disable);

        item.iItem = curSel;
        item.iSubItem = 1;
        item.mask = LVIF_TEXT;
        item.pszText = q->name;

        ListView_SetItem(hwndLV, &item);
        item.iItem = curSel;
        item.iSubItem = 2;
        item.mask = LVIF_TEXT;
        item.pszText = "data";

        ListView_SetItem(hwndLV, &item);
        item.iItem = curSel;
        item.iSubItem = 3;
        item.mask = LVIF_TEXT;
        sprintf(cur->size, "%d", q->size);
        item.pszText = cur->size;  // size
        ListView_SetItem(hwndLV, &item);
        curSel++;
        q = q->next;
    }
    for (i = 0; i < 4; i++)
    {
        if (hdwebp[i].active)
        {
            BPDATA* cur = (BPDATA*)calloc(sizeof(BPDATA), 1);
            cur->u.c = &hdwebp[i];
            cur->type = bt_hdwe;
            cur->next = bpData;
            bpData = cur;
            item.iItem = curSel;
            item.iSubItem = 0;
            item.mask = LVIF_PARAM;
            item.lParam = (LPARAM)cur;
            item.pszText = "";
            v = ListView_InsertItem(hwndLV, &item);
            ListView_SetCheckState(hwndLV, v, !hdwebp[i].disable);

            item.iItem = curSel;
            item.iSubItem = 1;
            item.mask = LVIF_TEXT;
            item.pszText = hdwebp[i].name;

            ListView_SetItem(hwndLV, &item);
            item.iItem = curSel;
            item.iSubItem = 2;
            item.mask = LVIF_TEXT;
            item.pszText = "hdwe";

            ListView_SetItem(hwndLV, &item);
            item.iItem = curSel;
            item.iSubItem = 3;
            item.mask = LVIF_TEXT;
            sprintf(cur->size, "%d", hdwebp[i].size + 1);
            item.pszText = cur->size;  // size
            ListView_SetItem(hwndLV, &item);
            curSel++;
        }
    }
    loading = FALSE;
}
//-------------------------------------------------------------------------

static void CopyText(HWND hwnd)
{
    int n = ListView_GetItemCount(hwndLV);
    int i;
    char* data = calloc(n + 1, 1024);
    if (data)
    {
        for (i = 0; i < n; i++)
        {
            int j;
            LVITEM item;
            if (ListView_GetCheckState(hwndLV, i))
                strcat(data, "X");
            for (j = 1; j <= 5; j++)
            {
                char buf[512];
                memset(&item, 0, sizeof(item));
                item.iItem = i;
                item.iSubItem = j;
                item.mask = LVIF_TEXT;
                item.pszText = buf;
                item.cchTextMax = sizeof(buf);
                ListView_GetItem(hwndLV, &item);
                sprintf(data + strlen(data), "\t%s", item.pszText);
            }
            sprintf(data + strlen(data), "\n");
        }
        TextToClipBoard(hwnd, data);
        free(data);
    }
}
//-------------------------------------------------------------------------

LRESULT CALLBACK breakProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    static LVHITTESTINFO hittest;
    RECT r;
    LV_COLUMN lvC;
    switch (iMessage)
    {
        case WM_SYSCOMMAND:
            if (wParam == SC_CLOSE)
            {
                SendMessage(hwnd, WM_CLOSE, 0, 0);
            }
            break;
        case WM_RESTACK:
            ClearBPWind();
            LoadBPWind();
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                LVITEM item;
                BPDATA* bpData;
                case IDM_REMOVEBREAKPOINT:
                    memset(&item, 0, sizeof(item));
                    item.iItem = hittest.iItem;
                    item.iSubItem = hittest.iSubItem;
                    item.mask = LVIF_PARAM;
                    ListView_GetItem(hwndLV, &item);
                    bpData = ((BPDATA*)item.lParam);
                    switch (bpData->type)
                    {
                        case bt_code:
                            Tag(TAG_BP, bpData->module, bpData->u.c->drawnLineno, 0, 0, 0, 0);
                            break;
                        case bt_data:
                            databpDelete(bpData->u.d);
                            break;
                        case bt_hdwe:
                            bpData->u.h->active = FALSE;
                            break;
                    }
                    PostMessage(hwnd, WM_RESTACK, 0, 0);
                    break;
                default:
                    SendMessage(hwndFrame, iMessage, wParam, lParam);
                    PostMessage(hwnd, WM_RESTACK, 0, 0);
                    break;
            }
            break;
        case WM_NOTIFY:
            if (((LPNMHDR)lParam)->code == LVN_ITEMCHANGED)
            {
                LPNMLISTVIEW lpnmListView = (LPNMLISTVIEW)lParam;
                BPDATA* bpData = ((BPDATA*)lpnmListView->lParam);
                if (!loading)
                {
                    BOOL state = !ListView_GetCheckState(hwndLV, lpnmListView->iItem);
                    switch (bpData->type)
                    {
                        case bt_code:
                            bpData->u.c->disable = state;
                            InvalidateByName(bpData->module);
                            break;
                        case bt_data:
                            bpData->u.d->disable = state;
                            break;
                        case bt_hdwe:
                            bpData->u.h->disable = state;
                            break;
                    }
                }
            }
            else if (((LPNMHDR)lParam)->code == NM_RCLICK)
            {
                HMENU menu = LoadMenuGeneric(hInstance, "BRKPOINTMENU");
                POINT pos;
                HMENU popup = GetSubMenu(menu, 0);
                GetCursorPos(&pos);

                InsertBitmapsInMenu(popup);
                hittest.pt = pos;
                ScreenToClient(hwndLV, &hittest.pt);
                if (ListView_HitTest(hwndLV, &hittest) < 0)
                {
                    EnableMenuItem(popup, IDM_REMOVEBREAKPOINT, MF_GRAYED);
                    hittest.iItem = -1;
                    hittest.iSubItem = 0;
                }
                TrackPopupMenuEx(popup, TPM_BOTTOMALIGN | TPM_LEFTBUTTON, pos.x, pos.y, hwnd, NULL);
                DestroyMenu(menu);
            }
            else if (((LPNMHDR)lParam)->code == NM_DBLCLK)
            {
                GetCursorPos(&hittest.pt);
                ScreenToClient(hwndLV, &hittest.pt);
                if (ListView_SubItemHitTest(hwndLV, &hittest) >= 0)
                {
                    BPDATA* bpdata;
                    LVITEM lvitem;
                    lvitem.iItem = hittest.iItem;
                    lvitem.iSubItem = 0;
                    lvitem.mask = LVIF_PARAM;
                    ListView_GetItem(hwndLV, &lvitem);
                    bpdata = (BPDATA*)lvitem.lParam;
                    if (bpdata->type == bt_code)
                    {
                        DWINFO info;
                        char* q;
                        strcpy(info.dwName, bpdata->module);
                        q = strrchr(info.dwName, '\\');
                        if (q)
                            strcpy(info.dwTitle, q + 1);
                        else
                            strcpy(info.dwTitle, info.dwName);
                        info.dwLineNo = TagNewLine(info.dwName, atoi(bpdata->size));
                        info.logMRU = FALSE;
                        info.newFile = FALSE;
                        CreateDrawWindow(&info, TRUE);
                        return 0;
                    }
                }
            }
            else if (((LPNMHDR)lParam)->code == LVN_KEYDOWN)
            {
                switch (((LPNMLVKEYDOWN)lParam)->wVKey)
                {
                    case 'C':
                        if (GetKeyState(VK_CONTROL) & 0x80000000)
                        {
                            CopyText(hwnd);
                        }
                        break;
                }
            }
            break;
        case WM_VSCROLL:
            break;
        case WM_SETFOCUS:
            PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
            SendMessage(GetParent(hwnd), WM_ACTIVATEME, 0, 0);
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_CREATE:
            GetClientRect(hwnd, &r);
            hwndLV = CreateWindowEx(0, WC_LISTVIEW, "", LVS_REPORT | LVS_SINGLESEL | WS_CHILD | WS_VISIBLE | WS_BORDER, 0, 0,
                                    r.right - r.left, r.bottom - r.top, hwnd, 0, hInstance, 0);
            ListView_SetExtendedListViewStyle(hwndLV,
                                              LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
            ApplyDialogFont(hwndLV);
            lvC.mask = LVCF_WIDTH | LVCF_SUBITEM;
            lvC.cx = 20;
            lvC.iSubItem = 0;
            ListView_InsertColumn(hwndLV, 0, &lvC);
            lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
            lvC.cx = 200;
            lvC.iSubItem = 1;
            lvC.pszText = "Name";
            ListView_InsertColumn(hwndLV, 1, &lvC);
            lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
            lvC.cx = 50;
            lvC.iSubItem = 2;
            lvC.pszText = "Type";
            ListView_InsertColumn(hwndLV, 2, &lvC);
            lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
            lvC.cx = 50;
            lvC.iSubItem = 3;
            lvC.pszText = "Size";
            ListView_InsertColumn(hwndLV, 3, &lvC);
            lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
            lvC.cx = 50;
            lvC.iSubItem = 4;
            lvC.pszText = "Line";
            ListView_InsertColumn(hwndLV, 4, &lvC);
            lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
            lvC.cx = 300;
            lvC.iSubItem = 5;
            lvC.pszText = "Module";
            ListView_InsertColumn(hwndLV, 5, &lvC);
            //            ListView_SetImageList(hwndLV, tagImageList, LVSIL_SMALL);
            SendMessage(hwnd, WM_DOENABLE, 1, 0);
            return 0;
        case WM_DOENABLE:
            EnableWindow(hwndLV, wParam);
            break;
        case WM_CLOSE:
            break;
        case WM_DESTROY:
            break;
        case WM_SIZE:
            r.left = 0;
            r.right = LOWORD(lParam);
            r.top = 0;
            r.bottom = HIWORD(lParam);
            MoveWindow(hwndLV, r.left, r.top, r.right - r.left, r.bottom - r.top, 0);
            return 0;
        default:
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}

//-------------------------------------------------------------------------

void RegisterBreakWindow(HINSTANCE hInstance)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = 0;
    wc.lpfnWndProc = &breakProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szBreakClassName;
    RegisterClass(&wc);
}

//-------------------------------------------------------------------------

HWND CreateBreakWindow(void) { return CreateInternalWindow(DID_BREAKWND, szBreakClassName, szBreakTitle); }
