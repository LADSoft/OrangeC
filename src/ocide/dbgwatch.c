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
#include <ctype.h>
#include <richedit.h>
#include "header.h"
#include "dbgtype.h"
//#include <dir.h>

extern HWND hwndFrame, hwndClient;
extern HINSTANCE hInstance;
extern PROCESS* activeProcess;
extern THREAD* activeThread;
extern unsigned bitmask[];
extern POINT rightclickPos;
extern SCOPE* activeScope;

static char szWatchClassName[] = "xccWatchClass";
static char szWatchTitle[] = "Watch Window";

static HBITMAP valueBitmap, itemBitmap;

static char* nameTags[5] = {"Watch 1", "Watch 2", "Watch 3", "Watch 4"};

static int lastWatch = DID_WATCHWND;

typedef struct watchdata
{
    int DID;
    HWND hwndWatchTree;
    WATCHINFO* watchinfo_list;
    int watchinfo_max;
    int watchinfo_count;
    VARINFO* structNesting[100];
    int nestingCount;
    BOOL refreshNeeded;
} WATCHDATA;
static void AddTypeInfoToName(DEBUG_INFO* dbg_info, VARINFO* v)
{
    char buf[512], *p;
    strcpy(v->screenname, v->membername);
    p = v->screenname + strlen(v->screenname);
    sprintf(p, "(%s)", SymTypeName(buf, dbg_info, v));
}

//-------------------------------------------------------------------------

static WATCHINFO* MatchItem(VARINFO* var, WATCHDATA* ptr)
{
    int i;
    for (i = 0; i < ptr->watchinfo_count; i++)
        if (!strcmp(ptr->watchinfo_list[i].info->membername, var->membername))
            return &ptr->watchinfo_list[i];
    return 0;
}

//-------------------------------------------------------------------------

static void FreeTree(VARINFO* info, WATCHDATA* ptr)
{
    while (info)
    {
        FreeTree(info->subtype, ptr);
        if (info->hTreeItem)
            TreeView_DeleteItem(ptr->hwndWatchTree, info->hTreeItem);
        info = info->link;
    }
}
//-------------------------------------------------------------------------

static void RefreshAddresses(WATCHDATA* ptr, VARINFO* var, int address, THREAD* thread, int noscope)
{
    ptr->structNesting[ptr->nestingCount++] = var;
    while (var)
    {
        int unscope = noscope;
        var->scope = ptr->structNesting[0]->scope;
        if (noscope)
            var->outofscope = TRUE;
        else
        {
            int val;
            var->outofscope = FALSE;
            if (thread)
                var->thread = thread;
            if (var->offset == -1)
            {
                DEBUG_INFO* dbg;
                int i;
                char name[2048];
                name[0] = 0;
                for (i = 0; i < ptr->nestingCount - 1; i++)
                    sprintf(name + strlen(name), "%s", ptr->structNesting[i]->structtag);
                sprintf(name + strlen(name), "@%s", var->membername);
                dbg = findDebug(ptr->structNesting[0]->scope->address);
                val = var->address = GetSymbolAddress(dbg, name);
                // static member data
            }
            else
            {
                val = var->address = address + var->offset;
            }
            if (var->constant)
                val = var->address;
            else if (var->pointer)
            {
                unscope = ((val = var->derefaddress) == -1 && !ReadValue(var->address, &val, 4, var)) || !val;
            }
            RefreshAddresses(ptr, var->subtype, val, thread, unscope);
        }
        var = var->link;
    }
    ptr->nestingCount--;
}

//-------------------------------------------------------------------------

static void WatchValue(DEBUG_INFO* dbg_info, char* buf, VARINFO* info, int onevalue)
{
    if (info->outofscope || info->outofscopereg)
        sprintf(buf, "out of scope");
    else if (info->constant)
        sprintf(buf, "POINTER: 0x%x", info->address);
    else if (info->structure)
    {
        sprintf(buf, "STRUCTURE: 0x%x", info->address);
    }
    else if (info->unionx)
    {
        sprintf(buf, "UNION: 0x%x", info->address);
    }
    else if (info->pointer)
    {
        int val;
        if ((val = info->derefaddress) != -1 || ReadValue(info->address, &val, 4, info))
        {
            info->editable = TRUE;
            if (onevalue)
                sprintf(buf, "0x%x ", val);
            else
                sprintf(buf, "POINTER: 0x%x ", val);
            GetStringValue(info, buf + strlen(buf), 32, val);
        }
        else
            sprintf(buf, "POINTER: <UNKNOWN>");
    }
    else if (info->enumx)
    {
        info->editable = TRUE;
        HintEnum(dbg_info, info, buf, FALSE, onevalue);
    }
    else if (info->bitfield)
    {
        int signedtype;
        int v = HintBf(info, &signedtype);
        if (onevalue)
            sprintf(buf, "0x%x", v);
        else if (signedtype)
            sprintf(buf, "%d(0x%x)", v, v);
        else
            sprintf(buf, "%u(0x%x)", v, v);
        info->editable = TRUE;
    }
    else if (info->array)
    {
        sprintf(buf, "ARRAY: 0x%x ", info->address);
        GetStringValue(info, buf + strlen(buf), 32, info->address);
    }
    else
    {
        int signedtype;
        char buf1[20];
        LLONG_TYPE v;
        info->editable = TRUE;
        switch (HintBasicValue(info, &signedtype, buf1))
        {
            case eLongLong:
            case eULongLong:
                v = *(LLONG_TYPE*)buf1;
                if (onevalue)
                    sprintf(buf, "0x%llx", v);
                else if (signedtype)
                    sprintf(buf, "%lld(0x%llx)", v, v);
                else
                    sprintf(buf, "%lld(0x%llx)", v, v);
                break;
            default:
                sprintf(buf, "unknown type");
                break;

            case eInt:
            case eUInt:
            case eLong:
            case eULong:
                v = *(int*)buf1;
                if (onevalue)
                    sprintf(buf, "0x%x", (int)v);
                else if (signedtype)
                    sprintf(buf, "%d(0x%x)", (int)v, (int)v);
                else
                    sprintf(buf, "%u(0x%x)", (int)v, (int)v);
                break;
            case eBool:
                if (buf1[0])
                    sprintf(buf, "True");
                else
                    sprintf(buf, "False");
                break;
            case eFloat:
            case eImaginary:
                sprintf(buf, "%f", (double)*(float*)buf1);
                break;
            case eDouble:
            case eImaginaryDouble:
            case eLongDouble:
            case eImaginaryLongDouble:
                sprintf(buf, "%f", *(long double*)buf1);
                break;
            case eComplex:
                sprintf(buf, "%f + %f * I", (double)*(float*)buf1, (double)*(float*)(buf1 + 4));
                break;
            case eComplexDouble:
                sprintf(buf, "%f + %f * I", *(double*)buf1, *(double*)(buf1 + 8));
                break;
            case eComplexLongDouble:
                sprintf(buf, "%f + %f * I", (double)*(long double*)buf1, (double)*(long double*)(buf1 + 10));
                break;
        }
    }
}

//-------------------------------------------------------------------------

static void RefreshData(DEBUG_INFO* dbg_info, VARINFO* var, BOOL adding)
{
    while (var)
    {
        char localbf[256];
        if (!adding)
            strcpy(localbf, var->value);
        if ((var->lref || var->rref) && var->type < eReservedTop && var->type > eVoid)
        {
            VARINFO q = *var;
            q.pointer = q.rref = q.lref = 0;
            if (!ReadValue(var->address, &q.address, 4, var))
            {
                sprintf(var->value, "REFERENCE: <UNKNOWN>");
            }
            else
            {
                WatchValue(dbg_info, var->value, &q, FALSE);
            }
        }
        else
        {
            RefreshData(dbg_info, var->subtype, adding);
            WatchValue(dbg_info, var->value, var, FALSE);
        }
        if (!adding && strcmp(localbf, var->value))
        {
            var->watchhead.col2Color = 0xff;
        }
        else
        {
            var->watchhead.col2Color = 0;
        }
        var = var->link;
    }
}

//-------------------------------------------------------------------------

static void RefreshItem(WATCHDATA* ptr, WATCHINFO* var, int address, THREAD* thread, BOOL adding)
{
    RefreshAddresses(ptr, var->info, address, thread, var->info->outofscope);
    RefreshData(var->dbg_info, var->info, adding);
}

//-------------------------------------------------------------------------

static HTREEITEM InsertItem(HTREEITEM hParent, HTREEITEM after, VARINFO* var, WATCHDATA* ptr)
{
    HTREEITEM rv;
    TV_INSERTSTRUCT t;
    memset(&t, 0, sizeof(t));
    t.hParent = hParent;
    t.hInsertAfter = after;
    t.UNNAMED_UNION item.mask = 0;
    t.UNNAMED_UNION item.lParam = (int)var;
    rv = TreeView_InsertItem(ptr->hwndWatchTree, &t);
    return rv;
}

//-------------------------------------------------------------------------

static void InsertSubTree(HTREEITEM parent, HTREEITEM after, VARINFO* var, int index, WATCHDATA* ptr)
{
    while (var)
    {
        AddTypeInfoToName(ptr->watchinfo_list[index].dbg_info, var);
        var->hTreeItem = InsertItem(parent, after, var, ptr);
        var->watchindex = index;
        var->watchhead.col1Text = &var->screenname[0];
        var->watchhead.col2Text = &var->value[0];
        if (var->pointer && !var->subtype && ((!var->lref && !var->rref) || var->type > eReservedTop))
        {
            var->hTreeHolder = InsertItem(var->hTreeItem, TVI_LAST, var, ptr);
            TreeView_Expand(ptr->hwndWatchTree, var->hTreeItem, TVE_COLLAPSE);
        }
        else
            InsertSubTree(var->hTreeItem, 0, var->subtype, index, ptr);
        after = var->hTreeItem;
        var = var->link;
    }
}

//-------------------------------------------------------------------------

static void AddItem(DEBUG_INFO* dbg, VARINFO* var, int cursoreip, WATCHDATA* ptr)
{
    WATCHINFO* x = MatchItem(var, ptr);
    if (x)
    {
        x->marked = FALSE;
        FreeVarInfo(var);
    }
    else
    {
        HTREEITEM previous = 0;
        if (ptr->watchinfo_count >= ptr->watchinfo_max)
        {
            if (ptr->watchinfo_max >= 128)
            {
                ExtendedMessageBox("Watch Error", MB_SETFOREGROUND | MB_SYSTEMMODAL,
                                   "There are too many items in the watch window\nNot adding the current selection");
                return;
            }
            ptr->watchinfo_max += 64;
            ptr->watchinfo_list = realloc(ptr->watchinfo_list, ptr->watchinfo_max * sizeof(WATCHINFO));
        }
        if (ptr->watchinfo_count)
            previous = ptr->watchinfo_list[ptr->watchinfo_count - 1].info->hTreeItem;
        memset(&ptr->watchinfo_list[ptr->watchinfo_count], 0, sizeof(ptr->watchinfo_list[0]));
        ptr->watchinfo_list[ptr->watchinfo_count].info = var;
        ptr->watchinfo_list[ptr->watchinfo_count].dbg_info = dbg;
        ptr->watchinfo_list[ptr->watchinfo_count++].cursoreip = cursoreip;
        RefreshItem(ptr, &ptr->watchinfo_list[ptr->watchinfo_count - 1], var->address, NULL, TRUE);
        InsertSubTree(0, previous, var, ptr->watchinfo_count - 1, ptr);
    }
}

//-------------------------------------------------------------------------

static void RenumberDeleteItems(VARINFO* v)
{
    while (v)
    {
        v->watchindex--;
        if (v->subtype)
            RenumberDeleteItems(v->subtype);
        v = v->link;
    }
}

//-------------------------------------------------------------------------

static void DeleteItem(POINT* pt, WATCHDATA* ptr)
{
    int i;
    WATCHINFO* x;
    TV_HITTESTINFO t;
    HTREEITEM titem;
    TV_ITEM item;
    VARINFO* v;

    ScreenToClient(ptr->hwndWatchTree, pt);
    t.pt = *pt;
    titem = TreeView_HitTest(ptr->hwndWatchTree, &t);
    if (titem)
    {
        int c;
        item.mask = TVIF_PARAM;
        item.hItem = titem;
        TreeView_GetItem(ptr->hwndWatchTree, &item);
        v = (VARINFO*)item.lParam;
        x = &ptr->watchinfo_list[c = v->watchindex];
        FreeTree(x->info, ptr);
        FreeVarInfo(x->info);
        for (i = c; i < ptr->watchinfo_count - 1; i++)
        {
            ptr->watchinfo_list[i] = ptr->watchinfo_list[i + 1];
            RenumberDeleteItems(ptr->watchinfo_list[i].info);
        }
        ptr->watchinfo_count--;
    }
}

//-------------------------------------------------------------------------

static void DeleteAllItems(WATCHDATA* ptr)
{
    int i;
    TreeView_DeleteAllItems(ptr->hwndWatchTree);
    for (i = 0; i < ptr->watchinfo_count; i++)
        FreeVarInfo(ptr->watchinfo_list[i].info);
    ptr->watchinfo_count = 0;
}

//-------------------------------------------------------------------------

static void Unscope(WATCHINFO* wi, WATCHDATA* ptr)
{
    if (!wi->info->outofscope)
    {
        wi->info->outofscope = TRUE;
        //      FreeTree(wi->info->subtype, ptr) ;
    }
}

//-------------------------------------------------------------------------

static void Rescope(WATCHINFO* wi, int index, WATCHDATA* ptr)
{
    if (wi->info->outofscope)
    {
        wi->info->outofscope = FALSE;
    }
}

//-------------------------------------------------------------------------

static void RefreshItems(WATCHDATA* ptr)
{
    int i;
    int offset;
    for (i = 0; i < ptr->watchinfo_count; i++)
    {
        WATCHINFO* wi = &ptr->watchinfo_list[i];
        VARINFO* var;
        var = EvalExpr(&wi->dbg_info, activeScope, (char*)wi->info->membername, FALSE);
        if (!var)
        {
            Unscope(wi, ptr);
            offset = 0;
        }
        else
        {
            wi->info->outofscopereg = var->outofscopereg;
            if (var->outofscopereg)
            {
                Unscope(wi, ptr);
                offset = 0;
            }
            else
            {
                Rescope(wi, i, ptr);
                offset = var->address;
                wi->cursoreip = activeScope->address;
                if (var->pointer)
                {
                    wi->info->derefaddress = var->derefaddress;
                }
            }
            FreeVarInfo(var);
        }
        RefreshItem(ptr, wi, offset, activeThread, FALSE);
    }
    InvalidateRect(ptr->hwndWatchTree, 0, 0);
}

//-------------------------------------------------------------------------

static void ExpandPointer(VARINFO* v, int code, WATCHDATA* ptr)
{
    if (v->pointer)
    {
        if (code == TVE_EXPAND)
        {
            int val;
            int outofscope;
            outofscope = ((val = v->derefaddress) == -1 && !ReadValue(v->address, &val, 4, v)) || !val;
            if (!v->subtype && ptr->watchinfo_list[v->watchindex].dbg_info)
            {
                TreeView_DeleteItem(ptr->hwndWatchTree, v->hTreeHolder);
                ExpandPointerInfo(ptr->watchinfo_list[v->watchindex].dbg_info, v);
                if (v->subtype->structure)
                {
                    InsertSubTree(v->hTreeItem, 0, v->subtype->subtype, v->watchindex, ptr);
                }
                else
                    InsertSubTree(v->hTreeItem, 0, v->subtype, v->watchindex, ptr);
            }
            ptr->structNesting[ptr->nestingCount++] = v;
            RefreshAddresses(ptr, v->subtype, val, NULL, outofscope);
            ptr->nestingCount--;
            RefreshData(ptr->watchinfo_list[v->watchindex].dbg_info, v, TRUE);
        }
        else if (code == TVE_COLLAPSE)
        {
            if (v->subtype && !v->derefaddress)
            {
                FreeTree(v->subtype, ptr);
                FreeVarInfo(v->subtype);
                v->subtype = 0;
                v->hTreeHolder = InsertItem(v->hTreeItem, TVI_LAST, v, ptr);
            }
        }
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

static void ChangeData(VARINFO* info, char* text, WATCHDATA* ptr)
{
    if (info->type >= eFloat && info->type <= eImaginaryLongDouble)
    {
        float v;
        double v2;
        char data[10];
        sscanf(text, "%f", &v);
        switch (info->type)
        {
            case eFloat:
            case eImaginary:
                WriteValue(info->address, &v, 4, &info->thread->regs);
                break;
            case eDouble:
            case eImaginaryDouble:
                v2 = v;
                WriteValue(info->address, &v2, 8, &info->thread->regs);
                break;
            case eLongDouble:
            case eImaginaryLongDouble:
                *(long double*)data = v;
                WriteValue(info->address, data, 10, &info->thread->regs);
                break;
            default:
                break;
        }
    }
    else if (info->type >= eComplex && info->type <= eComplexLongDouble)
    {
        float v[2];
        double v2[2];
        long double v4[2];
        sscanf(text, "%f + %f * I", &v[0], &v[1]);
        switch (info->type)
        {
            case eComplex:
                WriteValue(info->address, &v[0], 8, &info->thread->regs);
                break;
            case eComplexDouble:
                v2[0] = v[0];
                v2[1] = v[1];
                WriteValue(info->address, &v2[0], 16, &info->thread->regs);
                break;
            case eComplexLongDouble:
                v4[0] = v[0];
                v4[1] = v[1];
                WriteValue(info->address, &v4[0], 20, &info->thread->regs);
                break;
        }
    }
    else
    {
        LLONG_TYPE value;
        int size;
        if (info->enumx && !isdigit(text[0]))
        {
            value = GetEnumValue(ptr->watchinfo_list[info->watchindex].dbg_info, info, text);
            size = 4;
        }
        else
        {
            if (text[0] == '0' && text[1] == 'x')
                sscanf(text + 2, "%llx", &value);
            else if ((text[strlen(text) - 1] & 0xDF) == 'H')
                sscanf(text, "%llx", &value);
            else
                sscanf(text, "%lld", &value);
            switch (info->type)
            {
                case eBool:
                case eBit:
                case eChar:
                case eUChar:
                    size = 1;
                    break;
                case eShort:
                case eUShort:
                case eChar16T:
                case eWcharT:
                    size = 2;
                    break;
                case eInt:
                case eLong:
                case eUInt:
                case eULong:
                    size = 4;
                    break;
                case eLongLong:
                case eULongLong:
                    size = 8;
                    break;
            }
        }
        if (info->bitfield)
        {
            char data[10];
            int signedx;
            int v;
            HintBasicValue(info, &signedx, &data[0]);
            v = *(int*)data;
            v &= ~(bitmask[info->bitlength - 1] << info->bitstart);
            value = v | ((value & bitmask[info->bitlength - 1]) << info->bitstart);
        }
        WriteValue(info->address, &value, size, &info->thread->regs);
        if (info->pointer)
        {
            if (!value && info->subtype)
            {
                VARINFO* inf = info->subtype;
                info->subtype = 0;
                FreeTree(inf, ptr);
                FreeVarInfo(inf);
            }
            else
                RefreshAddresses(ptr, info->subtype, value, NULL, info->outofscope);
        }
    }
    RefreshData(ptr->watchinfo_list[info->watchindex].dbg_info, info, TRUE);
}

//-------------------------------------------------------------------------
static void LoadLocals(WATCHDATA* ptr)
{
    NAMELIST* names = FindEnclosedAutos(activeScope);
    WATCHINFO* p = ptr->watchinfo_list;
    int i;
    for (i = 0; i < ptr->watchinfo_count; i++)
        if (!strcmp(ptr->watchinfo_list[i].info->membername, "this"))
        {
            WATCHINFO* x = &ptr->watchinfo_list[i];
            int j;
            FreeTree(x->info, ptr);
            FreeVarInfo(x->info);
            for (j = i; j < ptr->watchinfo_count - 1; j++)

            {
                ptr->watchinfo_list[j] = ptr->watchinfo_list[j + 1];
                RenumberDeleteItems(ptr->watchinfo_list[j].info);
            }
            ptr->watchinfo_count--;
            i--;
        }
        else
        {
            p[i].marked = TRUE;
        }
    while (names)
    {
        NAMELIST* next = names->next;
        DEBUG_INFO* dbg;
        VARINFO* var = EvalExpr(&dbg, activeScope, (char*)names->data + 1, FALSE);
        if (var)
        {
            AddItem(dbg, var, activeScope->address, ptr);
        }
        free(names->data);
        free(names);
        names = next;
    }
    p = ptr->watchinfo_list;
    for (i = 0; i < ptr->watchinfo_count; i++)
        if (p[i].marked)
        {
            WATCHINFO* x = &ptr->watchinfo_list[i];
            int j;
            FreeTree(x->info, ptr);
            FreeVarInfo(x->info);
            for (j = i; j < ptr->watchinfo_count - 1; j++)

            {
                ptr->watchinfo_list[j] = ptr->watchinfo_list[j + 1];
                RenumberDeleteItems(ptr->watchinfo_list[j].info);
            }
            ptr->watchinfo_count--;
            i--;
        }
}
void ReloadVars(WATCHDATA* ptr)
{
    int i;
    int len = ptr->watchinfo_count;
    for (i = 0; i < ptr->watchinfo_count; i++)
    {
        FreeTree(ptr->watchinfo_list[i].info, ptr);
    }
    ptr->watchinfo_count = 0;
    for (i = 0; i < len; i++)
    {
        DEBUG_INFO* dbg;
        VARINFO* var = EvalExpr(&dbg, activeScope, (char*)ptr->watchinfo_list[i].info->membername, FALSE);
        if (var)
            FreeVarInfo(ptr->watchinfo_list[i].info);
        else
            var = ptr->watchinfo_list[i].info;
        AddItem(dbg, var, activeScope->address, ptr);
    }
}
LRESULT CALLBACK WatchWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    WATCHDATA* ptr;
    static POINT menupos;
    static char buf[256];
    RECT r;
    NMHDR* h;
    TCHeader tch;
    TV_ITEM item;
    static int sizingbottom;
    int offset;
    DEBUG_INFO* dbg;
    NM_TREEVIEW* nmt;
    VARINFO* var;
    HWND win;
    int doit;
    CHARRANGE charrange;
    switch (iMessage)
    {
        case WM_SYSCOMMAND:
            if (wParam == SC_CLOSE)
            {
                PostMessage(hwnd, WM_CLOSE, 0, 0);
                return 0;
            }
            break;
        case WM_NOTIFY:
            ptr = (WATCHDATA*)GetWindowLong(hwnd, 0);
            h = (NMHDR*)lParam;
            switch (h->code)
            {
                case NM_RCLICK:
                {
                    if (ptr->DID != DID_LOCALSWND)
                    {
                        HMENU menu = LoadMenuGeneric(hInstance, "WATCHMENU");
                        HMENU popup = GetSubMenu(menu, 0);
                        TV_HITTESTINFO t;
                        HTREEITEM titem;
                        GetCursorPos(&t.pt);
                        ScreenToClient(ptr->hwndWatchTree, &t.pt);
                        titem = TreeView_HitTest(ptr->hwndWatchTree, &t);
                        if (!titem || TreeView_GetParent(ptr->hwndWatchTree, titem))
                        {
                            EnableMenuItem(popup, IDM_DELETEWATCH, MF_GRAYED);
                        }
                        InsertBitmapsInMenu(popup);
                        GetCursorPos(&menupos);
                        TrackPopupMenuEx(popup, TPM_BOTTOMALIGN | TPM_LEFTBUTTON, menupos.x, menupos.y, hwndFrame, NULL);
                        DestroyMenu(menu);
                        lastWatch = ptr->DID;
                    }
                    SetFocus(ptr->hwndWatchTree);
                }
                    return 0;
                case TVN_ITEMEXPANDING:
                    nmt = (LPNMTREEVIEW)h;
                    if (nmt->action)
                    {
                        ExpandPointer((VARINFO*)nmt->itemNew.lParam, nmt->action, ptr);
                    }
                    return 0;
                case TCN_EDITQUERY:
                    nmt = (LPNMTREEVIEW)h;
                    item.mask = TVIF_PARAM;
                    item.hItem = (HTREEITEM)nmt->itemNew.hItem;
                    TreeView_GetItem(ptr->hwndWatchTree, &item);
                    var = (VARINFO*)item.lParam;
                    if (var->editable)
                    {
                        WatchValue(ptr->watchinfo_list[var->watchindex].dbg_info, buf, var, TRUE);
                        return buf;
                    }
                    return 0;
                case TCN_EDITDONE:
                    nmt = (LPNMTREEVIEW)h;
                    item.mask = TVIF_PARAM;
                    item.hItem = (HTREEITEM)nmt->itemNew.hItem;
                    TreeView_GetItem(ptr->hwndWatchTree, &item);
                    var = (VARINFO*)item.lParam;
                    ChangeData(var, nmt->itemNew.pszText, ptr);
                    RefreshItems(ptr);
                    return 0;
            }
            break;
        case WM_CREATE:
            ptr = (WATCHDATA*)calloc(sizeof(WATCHDATA), 1);
            SetWindowLong(hwnd, 0, (long)ptr);
            GetClientRect(hwnd, &r);
            tch.colText1 = "Item";
            tch.colText2 = "Value";
            tch.colBmp1 = itemBitmap;
            tch.colBmp2 = valueBitmap;
            ptr->hwndWatchTree = CreateextTreeWindow(hwnd, WS_DLGFRAME | TCS_LINE | WS_VISIBLE, &r, &tch);
            return 0;
        case WM_ADDWATCHINDIRECT:
            ptr = (WATCHDATA*)GetWindowLong(hwnd, 0);
            win = (HWND)wParam;
            doit = FALSE;
            SendMessage(win, EM_EXGETSEL, (WPARAM)0, (LPARAM)&charrange);
            if (charrange.cpMin == charrange.cpMax)
            {
                doit = SendMessage(win, WM_WORDUNDERPOINT, (WPARAM)&rightclickPos, (LPARAM)buf);
                if (!doit)
                {
                    PostMessage(hwndFrame, WM_COMMAND, IDM_ADDWATCH, 0);
                    break;
                }
            }
            else
            {
                if (charrange.cpMax - charrange.cpMin < sizeof(buf) || charrange.cpMin - charrange.cpMax < sizeof(buf))
                {
                    SendMessage(win, EM_GETSELTEXT, 0, (LPARAM)buf);
                    doit = TRUE;
                }
            }
            if (doit)
            {
                var = EvalExpr(&dbg, activeScope, (char*)buf, TRUE);
                if (var)
                {
                    AddItem(dbg, var, activeScope->address, ptr);
                    SelectWindow(ptr->DID);
                    break;
                }
            }
            ExtendedMessageBox("Error", MB_SETFOREGROUND | MB_SYSTEMMODAL, "Symbol does not exist in this scope");
            break;
        case WM_ADDWATCH:
            ptr = (WATCHDATA*)GetWindowLong(hwnd, 0);
            offset = wParam;
            if (!offset)
            {
                DeleteAllItems(ptr);
            }
            else
            {
                var = EvalExpr(&dbg, activeScope, (char*)lParam, TRUE);
                if (var)
                {
                    AddItem(dbg, var, activeScope->address, ptr);
                    SelectWindow(ptr->DID);
                }
            }
            break;
        case WM_INITIALSTACK:
            ptr = (WATCHDATA*)GetWindowLong(hwnd, 0);
            ptr->refreshNeeded = TRUE;
            break;
        case WM_COMMAND:
            ptr = (WATCHDATA*)GetWindowLong(hwnd, 0);
            switch (wParam)
            {
                case ID_SETADDRESS:
                    if (ptr->DID == DID_LOCALSWND)
                        LoadLocals(ptr);
                    if (ptr->refreshNeeded)
                        ReloadVars(ptr);
                    else
                        RefreshItems(ptr);
                    ptr->refreshNeeded = FALSE;
                    InvalidateRect(ptr->hwndWatchTree, 0, 0);
                    break;
                case IDM_DELETEWATCH:
                    DeleteItem(&menupos, ptr);
                    break;
                case IDM_DELETEALLWATCH:
                    DeleteAllItems(ptr);
                    break;
            }
            break;
        case WM_DESTROY:
            ptr = (WATCHDATA*)GetWindowLong(hwnd, 0);
            TreeView_DeleteAllItems(ptr->hwndWatchTree);
            DestroyWindow(ptr->hwndWatchTree);
            DeleteObject(valueBitmap);
            DeleteObject(itemBitmap);
            break;

        case WM_SIZE:
            ptr = (WATCHDATA*)GetWindowLong(hwnd, 0);
            r.left = 0;
            r.right = LOWORD(lParam);
            r.top = 0;
            r.bottom = HIWORD(lParam);
            MoveWindow(ptr->hwndWatchTree, r.left, r.top, r.right - r.left, r.bottom - r.top, 0);
            return 0;
        case WM_CLOSE:
            break;
        case WM_ACTIVATEME:
            ptr = (WATCHDATA*)GetWindowLong(hwnd, 0);
            if (ptr->DID != DID_LOCALSWND)
                lastWatch = ptr->DID;
            SendMessage(GetParent(hwnd), WM_ACTIVATEME, 0, 0);
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}

//-------------------------------------------------------------------------

void RegisterWatchWindow(HINSTANCE hInstance)
{
    static BOOL registered;
    if (!registered)
    {
        WNDCLASS wc;
        memset(&wc, 0, sizeof(wc));
        wc.style = 0;
        wc.lpfnWndProc = &WatchWndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = sizeof(DWORD);
        wc.hInstance = hInstance;
        wc.hIcon = LoadIcon(0, IDI_APPLICATION);
        wc.hCursor = LoadCursor(0, IDC_ARROW);
        wc.hbrBackground = GetStockObject(WHITE_BRUSH);
        wc.lpszMenuName = 0;
        wc.lpszClassName = szWatchClassName;
        RegisterClass(&wc);
        valueBitmap = LoadImage(hInstance, "ID_VALUEBMP", IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
        itemBitmap = LoadImage(hInstance, "ID_ITEMBMP", IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);

        registered = TRUE;
    }
}

//-------------------------------------------------------------------------

HWND CreateWatch1Window(void)
{
    HWND rv = CreateInternalWindow(DID_WATCHWND, szWatchClassName, "Watch 1");
    WATCHDATA* data = (WATCHDATA*)GetWindowLong(rv, 0);
    data->DID = DID_WATCHWND;
    return rv;
}
HWND CreateWatch2Window(void)
{
    HWND rv = CreateInternalWindow(DID_WATCHWND + 1, szWatchClassName, "Watch 2");
    WATCHDATA* data = (WATCHDATA*)GetWindowLong(rv, 0);
    data->DID = DID_WATCHWND + 1;
    return rv;
}
HWND CreateWatch3Window(void)
{
    HWND rv = CreateInternalWindow(DID_WATCHWND + 2, szWatchClassName, "Watch 3");
    WATCHDATA* data = (WATCHDATA*)GetWindowLong(rv, 0);
    data->DID = DID_WATCHWND + 2;
    return rv;
}
HWND CreateWatch4Window(void)
{
    HWND rv = CreateInternalWindow(DID_WATCHWND + 3, szWatchClassName, "Watch 4");
    WATCHDATA* data = (WATCHDATA*)GetWindowLong(rv, 0);
    data->DID = DID_WATCHWND + 3;
    return rv;
}
HWND CreateLocalsWindow(void)
{
    HWND rv = CreateInternalWindow(DID_LOCALSWND, szWatchClassName, "Locals");
    WATCHDATA* data = (WATCHDATA*)GetWindowLong(rv, 0);
    data->DID = DID_LOCALSWND;
    return rv;
}
int SendToLastWatch(unsigned iMessage, WPARAM wParam, LPARAM lParam) { return SendDIDMessage(lastWatch, iMessage, wParam, lParam); }