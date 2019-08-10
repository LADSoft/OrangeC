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

static char* szVersionDrawClassName = "xccVersionDrawClass";
static char* szUntitled = "Version";

LRESULT CALLBACK EditorSubclassProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
HANDLE ResGetHeap(PROJECTITEM* wa, struct resRes* data);
EXPRESSION* ResReadExp(struct resRes* data, char* buf);

struct versUndo
{
    struct versUndo* next;
    enum
    {
        vu_none,
        vu_insert,
        vu_delete,
        vu_changetext,
        vu_changevalue
    } type;
    int index;
    union
    {
        struct
        {
            struct ver_stringinfo* info;
        } byDelete;
        struct
        {
            WCHAR* key;
            WCHAR* value;
            int length;
        } byText;
        struct
        {
            DWORD low;
            DWORD high;
        } byValue;
    } u;
};
static void SetListViewColumns(HWND parent, HWND hwnd, RECT* r)
{
    LV_COLUMN lvC;
    LVGROUP lvG;
    ListView_EnableGroupView(hwnd, TRUE);
    ListView_SetExtendedListViewStyle(hwnd, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
    lvC.cx = (r->right - r->left) / 2;
    lvC.iSubItem = 0;
    lvC.pszText = "Property";
    ListView_InsertColumn(hwnd, 0, &lvC);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
    lvC.cx = (r->right - r->left) / 2;
    lvC.iSubItem = 1;
    lvC.pszText = "Value";
    ListView_InsertColumn(hwnd, 1, &lvC);

    lvG.cbSize = sizeof(lvG);
    lvG.mask = LVGF_HEADER | LVGF_GROUPID;
    lvG.iGroupId = 101;
    lvG.pszHeader = L"Fixed Parameters";
    ListView_InsertGroup(hwnd, -1, &lvG);

    lvG.mask = LVGF_HEADER | LVGF_GROUPID;
    lvG.iGroupId = 102;
    lvG.pszHeader = L"String Parameters";
    ListView_InsertGroup(hwnd, -1, &lvG);
}
static void ResizeListViewColumns(HWND hwnd, int width)
{
    ListView_SetColumnWidth(hwnd, 0, width / 2);
    ListView_SetColumnWidth(hwnd, 1, width / 2);
}
static void SetItem(struct resRes* versionData, int index, char* key, char* value)
{
    struct keypair* keys = calloc(1, sizeof(struct keypair));
    if (keys)
    {
        LVITEMA_x item;
        struct keypair** prev = &versionData->gd.versionKeys;
        keys->key = strdup(key);
        keys->value = strdup(value);
        keys->index = index;
        while (*prev)
            prev = &(*prev)->next;
        *prev = keys;

        memset(&item, 0, sizeof(LVITEMA_x));
        item.iItem = index;
        item.iSubItem = 0;
        item.mask = LVIF_PARAM | LVIF_TEXT | LVIF_GROUPID | LVIF_COLUMNS;
        item.lParam = (LPARAM)keys;
        item.pszText = keys->key;
        item.iGroupId = index < 7 ? 101 : 102;
        ListView_InsertItem(versionData->gd.childWindow, &item);
        item.iSubItem = 1;
        item.mask = LVIF_PARAM | LVIF_GROUPID | LVIF_COLUMNS;
        item.lParam = (LPARAM)keys;
        item.iGroupId = index < 7 ? 101 : 102;
        ListView_SetItem(versionData->gd.childWindow, &item);
    }
}
char* ParseVersionString(char* string, int* returnedLength)
{
    // this should be ok since the user isn't going to trigger an end-edit
    // on two windows simultaneously
    static char buf[512];
    char* q = buf;
    while (*string)
    {
        char ch = *string++;
        if (ch == '\\')
        {
            ch = *string++;
            switch (ch)
            {
                case 0:
                    return NULL;
                case 'a':
                    *q++ = '\a';
                    break;
                case 'b':
                    *q++ = '\b';
                    break;
                case 'f':
                    *q++ = '\f';
                    break;
                case 'n':
                    *q++ = '\n';
                    break;
                case 'r':
                    *q++ = '\r';
                    break;
                case 't':
                    *q++ = '\t';
                    break;
                case 'x':
                {
                    int val = 0;
                    while (isxdigit(*string))
                    {
                        int n = *string & ~0x20;
                        if (n > '0')
                            n -= 7;
                        n -= '0';
                        val = val * 16 + n;
                    }
                    *q++ = val;
                    break;
                }
                default:
                    if (isdigit(ch))
                    {
                        int val = 0;
                        while (isdigit(ch))
                        {
                            val = val * 8 + (ch - '0');
                            ch = *string++;
                        }
                        string--;
                        *q++ = val;
                    }
                    else
                    {
                        *q++ = '\\';
                        *q++ = ch;
                    }
                    break;
            }
        }
        else
        {
            *q++ = ch;
        }
    }
    *q = 0;
    *returnedLength = q - buf;
    return buf;
}
void FormatVersionString(char* buf, WCHAR* string, int len)
{
    *buf++ = '\"';
    while (len--)
    {
        WCHAR ch = *string++;
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
static void PopulateVersion(HWND hwnd, struct resRes* versionData)
{
    int index;
    char buf[256];
    VERSIONINFO* version = versionData->resource->u.versioninfo;
    struct variable* var;
    sprintf(buf, "%d, %d, %d, %d", version->fixed->file_version_ms >> 16, version->fixed->file_version_ms & 0xffff,
            version->fixed->file_version_ls >> 16, version->fixed->file_version_ls & 0xffff);
    SetItem(versionData, 0, "File Version", buf);
    sprintf(buf, "%d, %d, %d, %d", version->fixed->product_version_ms >> 16, version->fixed->product_version_ms & 0xffff,
            version->fixed->product_version_ls >> 16, version->fixed->product_version_ls & 0xffff);
    SetItem(versionData, 1, "Product Version", buf);
    buf[0] = 0;
    FormatExp(buf, version->fixed->file_flags);
    SetItem(versionData, 2, "File Flags", buf);
    buf[0] = 0;
    FormatExp(buf, version->fixed->file_flags_mask);
    SetItem(versionData, 3, "File Flags Mask", buf);
    buf[0] = 0;
    FormatExp(buf, version->fixed->file_os);
    SetItem(versionData, 4, "File OS", buf);
    buf[0] = 0;
    FormatExp(buf, version->fixed->file_type);
    SetItem(versionData, 5, "File Type", buf);
    buf[0] = 0;
    FormatExp(buf, version->fixed->file_subtype);
    SetItem(versionData, 6, "File Subtype", buf);
    index = 7;
    var = version->var;
    while (var)
    {
        if (var->type == VERINFO_STRING)
        {
            struct ver_stringinfo* strings = var->u.string.strings;
            while (strings)
            {
                char key[256], value[256];
                StringWToA(key, strings->key, wcslen(strings->key));
                strcpy(value, "\"\"");
                FormatVersionString(value, strings->value, strings->length);
                SetItem(versionData, index++, key, value);
                strings = strings->next;
            }
        }
        var = var->next;
    }
}
static void DeleteKeys(struct keypair* keys)
{
    while (keys)
    {
        struct keypair* next = keys->next;
        free(keys->key);
        free(keys->value);
        free(keys);
        keys = next;
    }
}
static int CALLBACK sortFunc(LPARAM left, LPARAM right, LPARAM id)
{
    if (((struct keypair*)left)->index < ((struct keypair*)right)->index)
        return -1;
    return (((struct keypair*)left)->index > ((struct keypair*)right)->index);
}
static void Renumber(HWND hwnd)
{
    int n = ListView_GetItemCount(hwnd);
    int i;
    LVITEMA_x item = {0};
    for (i = 0; i < n; i++)
    {
        struct keypair* c;
        item.iItem = i;
        item.iSubItem = 0;
        item.mask = LVIF_PARAM;
        ListView_GetItem(hwnd, &item);
        c = (struct keypair*)item.lParam;
        c->index = i;
    }
}
static void VersInsertItem(struct resRes* versionData, int index, char* key, char* value)
{
    // this is because when groups are enabled ListView_InsertItem always inserts at
    // the end of the group.  So we sort things after the fact...  but then the
    // sort indices are off so we have to renumber them
    SetItem(versionData, index, key, value);
    ListView_SortItems(versionData->gd.childWindow, sortFunc, 0);
    Renumber(versionData->gd.childWindow);
}
static void VersDeleteItem(HWND hwnd, int index)
{
    ListView_DeleteItem(hwnd, index);
    Renumber(hwnd);
}
static void VersInsert(struct resRes* versionData)
{
    if (versionData->gd.selectedRow >= 7)
    {
        VERSIONINFO* version = versionData->resource->u.versioninfo;
        struct variable* var;
        var = version->var;
        while (var)
        {
            if (var->type == VERINFO_STRING)
            {
                int i = 7;
                char key[256], value[256];
                struct ver_stringinfo **p = &var->u.string.strings, *strings;
                ResGetHeap(workArea, versionData);
                while (*p && i < versionData->gd.selectedRow)
                {
                    i = i + 1;
                    p = &(*p)->next;
                }
                strings = rcAlloc(sizeof(struct ver_stringinfo));
                if (strings)
                {
                    static WCHAR insertKey[] = L"New Key";
                    static WCHAR insertValue[] = L"New Value";
                    struct versUndo* undo;
                    strings->next = *p;
                    *p = strings;
                    strings->key = rcAlloc(sizeof(insertKey));
                    wcscpy(strings->key, insertKey);
                    strings->value = rcAlloc(sizeof(insertValue));
                    wcscpy(strings->value, insertValue);
                    StringWToA(key, strings->key, wcslen(strings->key));
                    strcpy(value, "\"\"");
                    FormatVersionString(value, strings->value, strings->length);
                    VersInsertItem(versionData, versionData->gd.selectedRow, key, value);
                    undo = calloc(1, sizeof(struct versUndo));
                    if (undo)
                    {
                        int n = ListView_GetItemCount(versionData->gd.childWindow);
                        if (n <= versionData->gd.selectedRow)
                            versionData->gd.selectedRow = n - 1;
                        undo->next = versionData->gd.undoData;
                        versionData->gd.undoData = undo;
                        undo->type = vu_insert;
                        undo->index = versionData->gd.selectedRow;
                    }
                    ResSetDirty(versionData);
                }
                break;
            }
            var = var->next;
        }
    }
}
static void VersDelete(struct resRes* versionData)
{
    if (versionData->gd.selectedRow >= 7)
    {
        if (ExtendedMessageBox("Version Row Delete", MB_OKCANCEL, "This will delete the row from the table") == IDOK)
        {
            VERSIONINFO* version = versionData->resource->u.versioninfo;
            struct variable* var;
            var = version->var;
            while (var)
            {
                if (var->type == VERINFO_STRING)
                {
                    int i = 7;
                    struct ver_stringinfo** p = &var->u.string.strings;
                    while (*p && i < versionData->gd.selectedRow)
                    {
                        i++;
                        p = &(*p)->next;
                    }
                    if (*p)
                    {
                        struct versUndo* undo;
                        undo = calloc(1, sizeof(struct versUndo));
                        if (undo)
                        {
                            undo->next = versionData->gd.undoData;
                            versionData->gd.undoData = undo;
                            undo->type = vu_delete;
                            undo->index = versionData->gd.selectedRow;
                            undo->u.byDelete.info = *p;
                        }
                        *p = (*p)->next;
                        VersDeleteItem(versionData->gd.childWindow, versionData->gd.selectedRow);
                        ResSetDirty(versionData);
                    }
                    break;
                }
                var = var->next;
            }
        }
    }
}
static void DoUndoChanged(struct resRes* versionData, DWORD low, DWORD high)
{
    struct versUndo* undo;
    undo = calloc(1, sizeof(struct versUndo));
    if (undo)
    {
        undo->next = versionData->gd.undoData;
        versionData->gd.undoData = undo;
        undo->type = vu_changevalue;
        undo->index = versionData->gd.selectedRow;
        undo->u.byValue.low = low;
        undo->u.byValue.high = high;
    }
}
static void VersChanged(struct resRes* versionData, char* buf)
{
    VERSIONINFO* version = versionData->resource->u.versioninfo;
    ResGetHeap(workArea, versionData);
    ResSetDirty(versionData);
    if (versionData->gd.selectedRow < 7)
    {
        switch (versionData->gd.selectedRow)
        {
            int one, two, three, four;
            EXPRESSION* exp;
            case 0:
                if (sscanf(buf, "%d, %d, %d, %d", &one, &two, &three, &four) == 4)
                {
                    DoUndoChanged(versionData, version->fixed->file_version_ls, version->fixed->file_version_ms);
                    ListView_DeleteItem(versionData->gd.childWindow, 0);
                    VersInsertItem(versionData, 0, "File Version", buf);
                    version->fixed->file_version_ms = (one << 16) + two;
                    version->fixed->file_version_ls = (three << 16) + four;
                }
                break;
            case 1:
                if (sscanf(buf, "%d, %d, %d, %d", &one, &two, &three, &four) == 4)
                {
                    DoUndoChanged(versionData, version->fixed->product_version_ls, version->fixed->product_version_ms);
                    ListView_DeleteItem(versionData->gd.childWindow, 1);
                    VersInsertItem(versionData, 1, "Product Version", buf);
                    version->fixed->product_version_ms = (one << 16) + two;
                    version->fixed->product_version_ls = (three << 16) + four;
                }
                break;
            case 2:
                if ((exp = ResReadExp(versionData, buf)))
                {
                    DoUndoChanged(versionData, (DWORD)version->fixed->file_flags, 0);
                    version->fixed->file_flags = exp;
                    ListView_DeleteItem(versionData->gd.childWindow, 2);
                    VersInsertItem(versionData, 2, "File Flags", buf);
                }
                break;
            case 3:
                if ((exp = ResReadExp(versionData, buf)))
                {
                    DoUndoChanged(versionData, (DWORD)version->fixed->file_flags_mask, 0);
                    version->fixed->file_flags_mask = exp;
                    ListView_DeleteItem(versionData->gd.childWindow, 3);
                    VersInsertItem(versionData, 3, "File Flags Mask", buf);
                }
                break;
            case 4:
                if ((exp = ResReadExp(versionData, buf)))
                {
                    DoUndoChanged(versionData, (DWORD)version->fixed->file_os, 0);
                    version->fixed->file_os = exp;
                    ListView_DeleteItem(versionData->gd.childWindow, 4);
                    VersInsertItem(versionData, 4, "File OS", buf);
                }
                break;
            case 5:
                if ((exp = ResReadExp(versionData, buf)))
                {
                    DoUndoChanged(versionData, (DWORD)version->fixed->file_type, 0);
                    version->fixed->file_type = exp;
                    ListView_DeleteItem(versionData->gd.childWindow, 5);
                    VersInsertItem(versionData, 5, "File Type", buf);
                }
                break;
            case 6:
                if ((exp = ResReadExp(versionData, buf)))
                {
                    DoUndoChanged(versionData, (DWORD)version->fixed->file_subtype, 0);
                    version->fixed->file_subtype = exp;
                    ListView_DeleteItem(versionData->gd.childWindow, 6);
                    VersInsertItem(versionData, 6, "File Subtype", buf);
                }
                break;
        }
    }
    else
    {
        struct variable* var;
        var = version->var;
        while (var)
        {
            if (var->type == VERINFO_STRING)
            {
                int i = 7;
                struct ver_stringinfo** p = &var->u.string.strings;
                int len = 0;
                char key[512], value[512];
                buf = ParseVersionString(buf, &len);
                if (buf)
                {
                    while (*p && i < versionData->gd.selectedRow)
                    {
                        i++;
                        p = &(*p)->next;
                    }
                    if (*p)
                    {
                        struct versUndo* undo;
                        WCHAR *newText, *q;
                        undo = calloc(1, sizeof(struct versUndo));
                        if (undo)
                        {
                            undo->next = versionData->gd.undoData;
                            versionData->gd.undoData = undo;
                            undo->type = vu_changetext;
                            undo->index = versionData->gd.selectedRow;
                            undo->u.byText.key = (*p)->key;
                            undo->u.byText.value = (*p)->value;
                            undo->u.byText.length = (*p)->length;
                        }
                        newText = rcAlloc((strlen(buf) + 1) * sizeof(WCHAR));
                        q = newText;
                        i = 0;
                        while (i < len)
                        {
                            *q++ = *buf++;
                            i++;
                        }
                        *q = 0;
                        if (versionData->gd.selectedColumn == 0)
                        {
                            (*p)->key = newText;
                        }
                        else
                        {
                            (*p)->value = newText;
                            (*p)->length = len;
                        }
                        ListView_DeleteItem(versionData->gd.childWindow, versionData->gd.selectedRow);
                        StringWToA(key, (*p)->key, wcslen((*p)->key));
                        strcpy(value, "\"\"");
                        FormatVersionString(value, (*p)->value, (*p)->length);
                        VersInsertItem(versionData, versionData->gd.selectedRow, key, value);
                    }
                }
                break;
            }
            var = var->next;
        }
    }
}
static void VersUndo(struct resRes* versionData)
{
    struct versUndo* undo = versionData->gd.undoData;
    VERSIONINFO* version = versionData->resource->u.versioninfo;
    if (undo)
    {
        struct variable* var;
        struct ver_stringinfo** string = NULL;
        versionData->gd.undoData = undo->next;
        if (!versionData->gd.undoData && !versionData->gd.cantClearUndo)
            ResSetClean(versionData);
        var = version->var;
        while (var)
        {
            if (var->type == VERINFO_STRING)
            {
                int i;
                string = &var->u.string.strings;
                for (i = 7; i < undo->index && *string; i++, string = &(*string)->next)
                    ;
                break;
            }
            var = var->next;
        }
        if (*string)
        {
            switch (undo->type)
            {
                char key[256], value[256], buf[256];
                case vu_none:
                    break;
                case vu_insert:
                    *string = (*string)->next;
                    VersDeleteItem(versionData->gd.childWindow, undo->index);
                    break;
                case vu_delete:
                {
                    struct ver_stringinfo* strings = undo->u.byDelete.info;
                    strings->next = *string;
                    *string = strings;
                    StringWToA(key, strings->key, wcslen(strings->key));
                    strcpy(value, "\"\"");
                    FormatVersionString(value, strings->value, strings->length);
                    VersInsertItem(versionData, undo->index, key, value);
                }
                break;
                case vu_changetext:

                    (*string)->length = undo->u.byText.length;
                    (*string)->key = undo->u.byText.key;
                    (*string)->value = undo->u.byText.value;
                    ListView_DeleteItem(versionData->gd.childWindow, undo->index);
                    StringWToA(key, (*string)->key, wcslen((*string)->key));
                    strcpy(value, "\"\"");
                    FormatVersionString(value, (*string)->value, (*string)->length);
                    VersInsertItem(versionData, versionData->gd.selectedRow, key, value);
                    break;
                case vu_changevalue:
                    switch (versionData->gd.selectedRow)
                    {
                        case 0:
                            version->fixed->file_version_ms = undo->u.byValue.high;
                            version->fixed->file_version_ls = undo->u.byValue.low;
                            sprintf(buf, "%d, %d, %d, %d", version->fixed->file_version_ms >> 16,
                                    version->fixed->file_version_ms & 0xffff, version->fixed->file_version_ls >> 16,
                                    version->fixed->file_version_ls & 0xffff);
                            ListView_DeleteItem(versionData->gd.childWindow, 0);
                            VersInsertItem(versionData, 0, "File Version", buf);
                            break;
                        case 1:
                            version->fixed->product_version_ms = undo->u.byValue.high;
                            version->fixed->product_version_ls = undo->u.byValue.low;
                            sprintf(buf, "%d, %d, %d, %d", version->fixed->product_version_ms >> 16,
                                    version->fixed->product_version_ms & 0xffff, version->fixed->product_version_ls >> 16,
                                    version->fixed->product_version_ls & 0xffff);
                            ListView_DeleteItem(versionData->gd.childWindow, 1);
                            VersInsertItem(versionData, 1, "Product Version", buf);
                            break;
                        case 2:
                            buf[0] = 0;
                            version->fixed->file_flags = (EXPRESSION*)undo->u.byValue.low;
                            FormatExp(buf, version->fixed->file_flags);
                            ListView_DeleteItem(versionData->gd.childWindow, 2);
                            VersInsertItem(versionData, 2, "File Flags", buf);
                            break;
                        case 3:
                            buf[0] = 0;
                            version->fixed->file_flags_mask = (EXPRESSION*)undo->u.byValue.low;
                            FormatExp(buf, version->fixed->file_flags_mask);
                            ListView_DeleteItem(versionData->gd.childWindow, 3);
                            VersInsertItem(versionData, 3, "File Flags Mask", buf);
                            break;
                        case 4:
                            buf[0] = 0;
                            version->fixed->file_os = (EXPRESSION*)undo->u.byValue.low;
                            FormatExp(buf, version->fixed->file_os);
                            ListView_DeleteItem(versionData->gd.childWindow, 4);
                            VersInsertItem(versionData, 4, "File OS", buf);
                            break;
                        case 5:
                            buf[0] = 0;
                            version->fixed->file_type = (EXPRESSION*)undo->u.byValue.low;
                            FormatExp(buf, version->fixed->file_type);
                            ListView_DeleteItem(versionData->gd.childWindow, 5);
                            VersInsertItem(versionData, 5, "File Type", buf);
                            break;
                        case 6:
                            buf[0] = 0;
                            version->fixed->file_subtype = (EXPRESSION*)undo->u.byValue.low;
                            FormatExp(buf, version->fixed->file_subtype);
                            ListView_DeleteItem(versionData->gd.childWindow, 6);
                            VersInsertItem(versionData, 6, "File Subtype", buf);
                            break;
                    }
            }
        }
        free(undo);
        if (versionData->gd.cantClearUndo)
            ResSetDirty(versionData);
        else if (!versionData->gd.undoData)
            ResSetClean(versionData);
    }
}
static int CustomDraw(HWND hwnd, struct resRes* versionData, LPNMLVCUSTOMDRAW draw)
{
    switch (draw->nmcd.dwDrawStage)
    {
        POINT pt;
        HPEN oldPen;
        case CDDS_PREPAINT:
        case CDDS_ITEMPREPAINT:
            return CDRF_NOTIFYSUBITEMDRAW | CDRF_NOTIFYPOSTPAINT;
        case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
            return CDRF_NOTIFYSUBITEMDRAW | CDRF_NOTIFYPOSTPAINT;
        case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM:
            // well you can't have groups and grids both, so we opt for groups
            // then draw the grids by hand...
            oldPen = SelectObject(draw->nmcd.hdc, CreatePen(PS_SOLID, 0, 0xe0e0e0));
            MoveToEx(draw->nmcd.hdc, draw->iSubItem == 0 ? 0 : draw->nmcd.rc.left, draw->nmcd.rc.bottom, &pt);
            LineTo(draw->nmcd.hdc, draw->nmcd.rc.right, draw->nmcd.rc.bottom);
            MoveToEx(draw->nmcd.hdc, draw->iSubItem == 0 ? 0 : draw->nmcd.rc.left, draw->nmcd.rc.top - 1, NULL);
            LineTo(draw->nmcd.hdc, draw->nmcd.rc.right, draw->nmcd.rc.top - 1);
            if (draw->iSubItem == 1)
            {
                MoveToEx(draw->nmcd.hdc, draw->nmcd.rc.left - 1, draw->nmcd.rc.top - 1, NULL);
                LineTo(draw->nmcd.hdc, draw->nmcd.rc.left - 1, draw->nmcd.rc.bottom);
            }
            MoveToEx(draw->nmcd.hdc, pt.x, pt.y, NULL);
            oldPen = SelectObject(draw->nmcd.hdc, oldPen);
            DeleteObject(oldPen);
            return CDRF_SKIPDEFAULT;
        default:
            return CDRF_DODEFAULT;
    }
}
LRESULT CALLBACK VersionDrawProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    RECT r;
    LPCREATESTRUCT createStruct;
    struct resRes* versionData;
    struct versUndo* release;
    LVHITTESTINFO_x hittest;
    switch (iMessage)
    {
        case WM_MDIACTIVATE:
            if ((HWND)lParam == hwnd)
            {
                doMaximize();
            }
            break;
        case WM_NOTIFY:
            versionData = (struct resRes*)GetWindowLong(hwnd, 0);
            switch (((LPNMHDR)lParam)->code)
            {
                case LVN_GETDISPINFO:
                {
                    LV_DISPINFO* plvdi = (LV_DISPINFO*)lParam;
                    struct keypair* keypair;
                    plvdi->item.mask |= LVIF_TEXT | LVIF_DI_SETITEM;
                    plvdi->item.mask &= ~LVIF_IMAGE;
                    keypair = (struct keypair*)plvdi->item.lParam;
                    switch (plvdi->item.iSubItem)
                    {
                        case 1:
                            plvdi->item.pszText = keypair->value;
                            break;
                        default:
                            break;
                    }
                }
                break;

                case LVN_KEYDOWN:
                {
                    versionData = (struct resRes*)GetWindowLong(hwnd, 0);
                    SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_EDIT, EN_KILLFOCUS), 0);
                    switch (((LPNMLVKEYDOWN)lParam)->wVKey)
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
                        case VK_INSERT:
                            versionData->gd.selectedRow = ListView_GetNextItem(versionData->gd.childWindow, -1, LVNI_SELECTED);
                            PostMessage(hwnd, WM_COMMAND, IDM_INSERT, 0);
                            break;
                        case VK_DELETE:
                            versionData->gd.selectedRow = ListView_GetNextItem(versionData->gd.childWindow, -1, LVNI_SELECTED);
                            PostMessage(hwnd, WM_COMMAND, IDM_DELETE, 0);
                            break;
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
                case NM_CUSTOMDRAW:
                    versionData = (struct resRes*)GetWindowLong(hwnd, 0);
                    return CustomDraw(hwnd, versionData, (LPNMLVCUSTOMDRAW)lParam);

                case NM_CLICK:
                {
                    SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_EDIT, EN_KILLFOCUS), 0);
                }
                break;
                case NM_RCLICK:
                {
                    POINT pt;
                    versionData = (struct resRes*)GetWindowLong(hwnd, 0);
                    GetCursorPos(&hittest.pt);
                    pt = hittest.pt;
                    ScreenToClient(versionData->gd.childWindow, &hittest.pt);
                    if (ListView_HitTest(versionData->gd.childWindow, &hittest) < 0)
                    {
                        hittest.iItem = ListView_GetItemCount(versionData->gd.childWindow);
                        hittest.iSubItem = 0;
                    }
                    {
                        if (hittest.iItem >= 7)
                        {
                            HMENU menu, popup;
                            SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_EDIT, EN_KILLFOCUS), 0);
                            menu = LoadMenuGeneric(hInstance, "RESVERSMENU");
                            versionData = (struct resRes*)GetWindowLong(hwnd, 0);
                            versionData->gd.selectedRow = hittest.iItem;
                            versionData->gd.selectedColumn = hittest.iSubItem;
                            popup = GetSubMenu(menu, 0);
                            InsertBitmapsInMenu(popup);
                            TrackPopupMenuEx(popup, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, hwnd, NULL);
                            DestroyMenu(menu);
                        }
                        else
                        {
                            return 1;
                        }
                    }
                }
                break;
                case NM_DBLCLK:
                    versionData = (struct resRes*)GetWindowLong(hwnd, 0);
                    GetCursorPos(&hittest.pt);
                    ScreenToClient(versionData->gd.childWindow, &hittest.pt);
                    if (ListView_SubItemHitTest(versionData->gd.childWindow, &hittest) >= 0)
                    {
                        versionData = (struct resRes*)GetWindowLong(hwnd, 0);
                        versionData->gd.selectedRow = hittest.iItem;
                        versionData->gd.selectedColumn = hittest.iSubItem;
                        PostMessage(hwnd, WM_HANDLEDBLCLICK, 0, 0);
                    }
                    break;
            }
            break;
        case WM_HANDLEDBLCLICK:
        {
            versionData = (struct resRes*)GetWindowLong(hwnd, 0);
            if (versionData->gd.selectedColumn == 1 || versionData->gd.selectedRow >= 7)
            {
                RECT r;
                LV_ITEM x;
                char buf[256];
                struct keypair* keypair;
                if (versionData->gd.editWindow)
                {
                    DestroyWindow(versionData->gd.editWindow);
                    versionData->gd.editWindow = NULL;
                }
                memset(&x, 0, sizeof(x));
                x.mask = LVIF_PARAM;
                x.iItem = versionData->gd.selectedRow;
                x.iSubItem = versionData->gd.selectedColumn;
                ListView_GetItem(versionData->gd.childWindow, &x);
                keypair = (struct keypair*)x.lParam;
                ListView_GetSubItemRect(versionData->gd.childWindow, versionData->gd.selectedRow, versionData->gd.selectedColumn,
                                        LVIR_BOUNDS, &r);
                // gets rid of quotes...
                if (versionData->gd.selectedColumn == 0)
                {
                    strcpy(buf, keypair->key);
                }
                else if (versionData->gd.selectedRow < 7)
                {
                    strcpy(buf, keypair->value);
                }
                else
                {
                    strcpy(buf, keypair->value + 1);
                    buf[strlen(buf) - 1] = 0;
                }
                versionData->gd.editWindow = CreateWindow("edit", buf, WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS, r.left, r.top,
                                                          r.right - r.left, 16, hwnd, (HMENU)ID_EDIT, hInstance, NULL);
                ApplyDialogFont(versionData->gd.editWindow);
                SetParent(versionData->gd.editWindow, versionData->gd.childWindow);
                AccSubclassEditWnd(hwnd, versionData->gd.editWindow);
                SendMessage(versionData->gd.editWindow, EM_SETSEL, 0, -1);
                SetFocus(versionData->gd.editWindow);
            }
        }
        break;
        case EM_CANUNDO:
            versionData = (struct resRes*)GetWindowLong(hwnd, 0);
            return versionData->gd.undoData != NULL;
        case WM_COMMAND:
            versionData = (struct resRes*)GetWindowLong(hwnd, 0);
            switch (LOWORD(wParam))
            {
                case ID_EDIT:
                    if (HIWORD(wParam) == EN_KILLFOCUS)
                    {
                        static BOOL inKillFocus;
                        if (versionData->gd.editWindow && !inKillFocus)
                        {
                            char buf[256];
                            buf[GetWindowText(versionData->gd.editWindow, buf, sizeof(buf))] = 0;
                            VersChanged(versionData, buf);
                            inKillFocus = TRUE;
                            DestroyWindow(versionData->gd.editWindow);
                            inKillFocus = FALSE;
                            versionData->gd.editWindow = NULL;
                        }
                    }
                    break;
                case IDM_INSERT:
                    VersInsert(versionData);
                    break;
                case IDM_DELETE:
                    VersDelete(versionData);
                    ListView_SetItemState(versionData->gd.childWindow, versionData->gd.selectedRow, LVIS_SELECTED, LVIS_SELECTED);
                    break;
                case IDM_SAVE:
                    if (versionData->resource->changed)
                    {
                        ResSaveCurrent(workArea, versionData);
                    }
                    break;
                case IDM_UNDO:
                    VersUndo(versionData);
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
            GetClientRect(hwnd, &r);
            createStruct = (LPCREATESTRUCT)lParam;
            versionData = (struct resRes*)((LPMDICREATESTRUCT)(createStruct->lpCreateParams))->lParam;
            SetWindowLong(hwnd, 0, (long)versionData);
            versionData->activeHwnd = hwnd;
            versionData->gd.versionKeys = NULL;
            versionData->gd.childWindow = CreateWindowEx(0, WC_LISTVIEW, "", WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SINGLESEL, 0,
                                                         0, r.right, r.bottom, hwnd, (HMENU)ID_TREEVIEW, hInstance, NULL);
            SetListViewColumns(hwnd, versionData->gd.childWindow, &r);
            PopulateVersion(hwnd, versionData);
            InsertRCWindow(hwnd);
            break;
        case WM_CLOSE:
            RemoveRCWindow(hwnd);
            SendMessage(hwndSrcTab, TABM_REMOVE, 0, (LPARAM)hwnd);
            break;
        case WM_DESTROY:
            versionData = (struct resRes*)GetWindowLong(hwnd, 0);
            release = versionData->gd.undoData;
            if (release)
                versionData->gd.cantClearUndo = TRUE;
            while (release)
            {
                struct versUndo* next = release->next;
                free(release);
                release = next;
            }
            versionData->gd.undoData = NULL;
            versionData->activeHwnd = NULL;
            DeleteKeys(versionData->gd.versionKeys);
            break;
        case WM_SIZE:
            versionData = (struct resRes*)GetWindowLong(hwnd, 0);
            ResizeListViewColumns(versionData->gd.childWindow, LOWORD(lParam));
            MoveWindow(versionData->gd.childWindow, 0, 0, LOWORD(lParam), HIWORD(lParam), 1);
            break;
        default:
            break;
    }
    return DefMDIChildProc(hwnd, iMessage, wParam, lParam);
}

void RegisterVersionDrawWindow(HINSTANCE hInstance)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = &VersionDrawProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(void*) * 2;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szVersionDrawClassName;
    RegisterClass(&wc);
}
void CreateVersionDrawWindow(struct resRes* info)
{
    char name[512];
    int maximized;
    HWND hwnd;
    sprintf(name, "%s - %s", szUntitled, info->name);
    SendMessage(hwndClient, WM_MDIGETACTIVE, 0, (LPARAM)&maximized);
    hwnd =
        CreateMDIWindow(szVersionDrawClassName, name,
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