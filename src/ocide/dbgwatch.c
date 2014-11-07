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
#include <stdio.h>
#include <ctype.h>
#include <richedit.h>
#include "header.h"
#include "dbgtype.h"
//#include <dir.h>

extern HWND hwndFrame, hwndProject, hwndRegister, hwndClient,hwndTab;
extern HINSTANCE hInstance;
extern PROCESS *activeProcess;
extern THREAD *activeThread;
extern unsigned bitmask[];
extern POINT rightclickPos;
extern SCOPE *activeScope;

HWND hwndWatch;
static char szWatchClassName[] = "xccWatchClass";
static char szWatchTitle[] = "Watch Window";
static HWND hwndTabCtrl;

static HBITMAP valueBitmap, itemBitmap;

static char *nameTags[4] = { "Watch 1", "Watch 2", "Watch 3", "Watch 4" };
static HWND hwndTree[4];
static WATCHINFO *watchinfo_list[4];
static int watchinfo_max[4];
static int watchinfo_count[4];

void AddTypeInfoToName(DEBUG_INFO *dbg_info, VARINFO *v)
{
    char buf[256],  *p;
    strcpy(v->screenname, v->membername);
    p = v->screenname + strlen(v->screenname);
    sprintf(p, "(%s)", SymTypeName(buf, dbg_info, v));
}

//-------------------------------------------------------------------------

WATCHINFO *MatchItem(VARINFO *var, int page)
{
    int i;
    for (i = 0; i < watchinfo_count[page]; i++)
        if (!strcmp(watchinfo_list[page][i].info->membername, var->membername))
            return  &watchinfo_list[page][i];
    return 0;
}

//-------------------------------------------------------------------------

void FreeTree(VARINFO *info, int page)
{
    while (info)
    {
        FreeTree(info->subtype, page);
        if (info->hTreeItem)
            TreeView_DeleteItem(hwndTree[page], info->hTreeItem);
        info = info->link;
    }
}

//-------------------------------------------------------------------------

void RefreshAddresses(VARINFO *var, int address, THREAD *thread, int noscope)
{
    while (var)
    {
        int unscope = noscope;
        int val;
        if (noscope)
            var->outofscope = TRUE;
        else
        {
            var->outofscope = FALSE;
            if (thread)
                var->thread = thread;
            val = var->address = address + var->offset;
            if (var->pointer)
            {
                unscope = (val = var->derefaddress) == -1 && !ReadValue(var->address, &val, 4, var) || !val;
            }
        }
        RefreshAddresses(var->subtype, val, thread, unscope);
        var = var->link;
    }
}

//-------------------------------------------------------------------------

void WatchValue(DEBUG_INFO *dbg_info, char *buf, VARINFO *info, int onevalue)
{
    int i;
    if (info->outofscope || info->outofscopereg)
        sprintf(buf, "out of scope");
    else if (info->constant)
        sprintf(buf, "POINTER: %x", info->address);
    else if (info->structure)
    {
        sprintf(buf, "STRUCTURE: %x", info->address);
    }
    else if (info->unionx)
    {
        sprintf(buf, "UNION: %x", info->address);
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
                sprintf(buf, "POINTER: %x ", val);
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
        else
            if (signedtype)
                sprintf(buf, "%d(0x%x)", v, v);
            else
                sprintf(buf, "%u(0x%x)", v, v);
        info->editable = TRUE;
    }
    else if (info->array)
    {
        sprintf(buf, "ARRAY: %x ", info->address);
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
                        sprintf(buf, "0x%Lx", v);
                    else
                        if (signedtype)
                            sprintf(buf, "%Ld(0x%Lx)", v, v);
                        else
                            sprintf(buf, "%Ld(0x%Lx)", v, v);
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
                else
                    if (signedtype)
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
                *(double*)buf1 = *(long double*)buf1;
            case eLongDouble:
            case eImaginaryLongDouble:
                sprintf(buf, "%f", *(double*)buf1);
                break;
            case eComplex:
                sprintf(buf, "%f + %f * I", (double)*(float*)buf1, (double)*(float *)(buf1 + 4));
                break;
            case eComplexDouble:
                sprintf(buf, "%f + %f * I", *(double *)buf1, *(double *)(buf1 + 8));
                break;
            case eComplexLongDouble:
                sprintf(buf, "%f + %f * I", (double)*(long double *)buf1, (double)*(long double *)(buf1 + 10));
                break;
        }
    }
}

//-------------------------------------------------------------------------

void RefreshData(DEBUG_INFO *dbg_info, VARINFO *var)
{
    while (var)
    {
        WatchValue(dbg_info, var->value, var, FALSE);
        RefreshData(dbg_info, var->subtype);
        var = var->link;
    }
}

//-------------------------------------------------------------------------

void RefreshItem(WATCHINFO *var, int address, THREAD *thread)
{
    RefreshAddresses(var->info, address, thread, var->info->outofscope);
    RefreshData(var->dbg_info, var->info);
}

//-------------------------------------------------------------------------

HTREEITEM InsertItem(HTREEITEM hParent, HTREEITEM after, VARINFO *var, int page)
{
    HTREEITEM rv;
    TV_INSERTSTRUCT t;
    memset(&t, 0, sizeof(t));
    t.hParent = hParent;
    t.hInsertAfter = after;
    t.UNNAMED_UNION item.mask = 0;
    t.UNNAMED_UNION item.lParam = (int)var;
    rv = TreeView_InsertItem(hwndTree[page], &t);
    return rv;
}

//-------------------------------------------------------------------------

void InsertSubTree(HTREEITEM parent, HTREEITEM after, VARINFO *var, int index, int page)
{
    while (var)
    {
        AddTypeInfoToName(watchinfo_list[page][index].dbg_info, var);
        var->hTreeItem = InsertItem(parent, after, var, page);
        var->watchindex = index;
        var->watchhead.col1Text = &var->screenname;
        var->watchhead.col2Text = &var->value;
        if (var->pointer && !var->subtype)
        {
            var->hTreeHolder = InsertItem(var->hTreeItem, TVI_LAST, var, page);
            TreeView_Expand(hwndTree[page], var->hTreeItem, TVE_COLLAPSE);
        }
        else
            InsertSubTree(var->hTreeItem, 0, var->subtype, index, page);
        after = var->hTreeItem;
        var = var->link;
    }
}

//-------------------------------------------------------------------------

void AddItem(DEBUG_INFO *dbg, VARINFO *var, 
             int cursoreip, int page)
{
    WATCHINFO *x = MatchItem(var, page);
    if (x)
    {
        FreeVarInfo(var);
    }
    else
    {
        HTREEITEM previous = 0;
        if (watchinfo_count[page] >= watchinfo_max[page])
        {
            if (watchinfo_max[page] >= 128)
            {
                ExtendedMessageBox("Watch Error", MB_SETFOREGROUND |
                    MB_SYSTEMMODAL, 
                    "There are too many items in the watch window\nNot adding the current selection");
                return ;
            }
            watchinfo_max[page] += 64;
            watchinfo_list[page] = realloc(watchinfo_list[page], watchinfo_max[page] *sizeof
                (WATCHINFO));
        }
        if (watchinfo_count[page])
            previous = watchinfo_list[page][watchinfo_count[page] - 1].info->hTreeItem;
        memset(&watchinfo_list[page][watchinfo_count[page]], 0, sizeof(watchinfo_list[page][0]));
        watchinfo_list[page][watchinfo_count[page]].info = var;
        watchinfo_list[page][watchinfo_count[page]].dbg_info = dbg;
        watchinfo_list[page][watchinfo_count[page]++].cursoreip = cursoreip;
        RefreshItem(&watchinfo_list[page][watchinfo_count[page] - 1], var->address, NULL);
        InsertSubTree(0, previous, var, watchinfo_count[page] - 1, page);
    }
}

//-------------------------------------------------------------------------

void RenumberDeleteItems(VARINFO *v)
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

void DeleteItem(POINT *pt, int page)
{
    int i;
    WATCHINFO *x;
    TV_HITTESTINFO t;
    HTREEITEM titem;
    TV_ITEM item;
    VARINFO *v;

    ScreenToClient(hwndTree[page], pt);
    t.pt =  *pt;
    titem = TreeView_HitTest(hwndTree[page], &t);
    if (titem)
    {
        int c;
        item.mask = TVIF_PARAM;
        item.hItem = titem;
        TreeView_GetItem(hwndTree[page], &item);
        v = (VARINFO*)item.lParam;
        x = &watchinfo_list[page][c = v->watchindex];
        FreeTree(x->info, page);
        FreeVarInfo(x->info);
        for (i = c; i < watchinfo_count[page] - 1; i++)
        {
            watchinfo_list[page][i] = watchinfo_list[page][i + 1];
            RenumberDeleteItems(watchinfo_list[page][i].info);
        }
        watchinfo_count[page]--;
    }
}

//-------------------------------------------------------------------------

void DeleteAllItems(int page)
{
    int i;
    TreeView_DeleteAllItems(hwndTree[page]);
    for (i = 0; i < watchinfo_count[page]; i++)
        FreeVarInfo(watchinfo_list[page][i].info);
    watchinfo_count[page] = 0;
}

//-------------------------------------------------------------------------

void Unscope(WATCHINFO *wi, int page)
{
    if (!wi->info->outofscope)
    {
        wi->info->outofscope = TRUE;
        //      FreeTree(wi->info->subtype, page) ;
    }
}

//-------------------------------------------------------------------------

void Rescope(WATCHINFO *wi, int index, int page)
{
    if (wi->info->outofscope)
    {
        wi->info->outofscope = FALSE;
        //      InsertSubTree(wi->info->hTreeItem,0,wi->info->subtype, index, page) ;
    }
}

//-------------------------------------------------------------------------

void RefreshItems(int page)
{
    int i;
    char *types,  *syms;
    int offset;
    DEBUG_INFO *dbg;
    for (i = 0; i < watchinfo_count[page]; i++)
    {
        WATCHINFO *wi = &watchinfo_list[page][i];
        VARINFO *var;
        int level;
        var = EvalExpr(&wi->dbg_info, activeScope,
                               (char*)wi->info->membername, FALSE);
        if (!var)
        {
            Unscope(wi, page);
            offset = 0;
        }
        else
        {
            wi->info->outofscopereg = var->outofscopereg;
            if (var->outofscopereg)
            {
                Unscope(wi, page);
                offset = 0;
            }
            else
            {
                int ebp, level;
                Rescope(wi, i, page);
                offset = var->address;
                wi->cursoreip = activeScope->address;
                if (var->pointer)
                {
                    wi->info->derefaddress = var->derefaddress;
                }
            }
            FreeVarInfo(var);
        }
        RefreshItem(wi, offset, activeThread);
    }
    InvalidateRect(hwndTree[page], 0, 0);
}

//-------------------------------------------------------------------------

void ExpandPointer(VARINFO *v, int code, int page)
{
    if (v->pointer)
    {
        if (code == TVE_EXPAND)
        {
            int val;
            int outofscope = (val = v->derefaddress) == -1 && !ReadValue(v->address, &val, 4, v) || !val;
            if (!v->subtype && watchinfo_list[page][v->watchindex].dbg_info)
            {
                TreeView_DeleteItem(hwndTree[page], v->hTreeHolder);
                ExpandPointerInfo(watchinfo_list[page][v->watchindex].dbg_info, v);
                if (v->subtype->structure)
                {
                    InsertSubTree(v->hTreeItem, 0, v->subtype->subtype, v->watchindex, page);
                }
                else
                    InsertSubTree(v->hTreeItem, 0, v->subtype, v->watchindex, page);
            }
            RefreshAddresses(v->subtype, val, NULL, outofscope);
            RefreshData(watchinfo_list[page][v->watchindex].dbg_info, v);
        }
        else if (code == TVE_COLLAPSE)
        {
            if (v->subtype)
            {
                FreeTree(v->subtype, page);
                FreeVarInfo(v->subtype);
                v->subtype = 0;
                v->hTreeHolder = InsertItem(v->hTreeItem, TVI_LAST, v, page);
            }
        }
    }
}


//-------------------------------------------------------------------------


//-------------------------------------------------------------------------

void ChangeData(VARINFO *info, char *text, int page)
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
        sscanf(text, "%f + %f * I", &v[0],&v[1]);
        switch (info->type)
        {
            case eComplex:
                WriteValue(info->address, &v[0], 8, &info->thread->regs);
                break ;
            case eComplexDouble:
                v2[0] = v[0];
                v2[1] = v[1];
                WriteValue(info->address, &v2[0], 16, &info->thread->regs);
                break ;
            case eComplexLongDouble:
                v4[0] = v[0];
                v4[1] = v[1];
                WriteValue(info->address, &v4[0], 20, &info->thread->regs);
                break ;
        }        
    }
    else
    {
        LLONG_TYPE value;
        int size;
        if (info->enumx && !isdigit(text[0]))
        {
            value = GetEnumValue(watchinfo_list[page][info->watchindex].dbg_info, info,
                text);
            size = 4;
        }
        else
        {
            if (text[0] == '0' && text[1] == 'x')
                sscanf(text + 2, "%Lx", &value);
            else if ((text[strlen(text) - 1] & 0xDF) == 'H')
                sscanf(text, "%Lx", &value);
            else
                sscanf(text, "%Ld", &value);
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
            HintBasicValue(info, &signedx, &data);
            v = *(int*)data;
            v &= ~(bitmask[info->bitlength - 1] << info->bitstart);
            value = v | ((value &bitmask[info->bitlength - 1]) << info
                ->bitstart);
        }
        WriteValue(info->address, &value, size, &info->thread->regs);
        if (info->pointer)
        {
            if (!value && info->subtype)
            {
                VARINFO *inf = info->subtype;
                info->subtype = 0;
                FreeTree(inf, page);
                FreeVarInfo(inf);
            }
            else
                RefreshAddresses(info->subtype, value, NULL, info->outofscope);
        }
    }
    RefreshData(watchinfo_list[page][info->watchindex].dbg_info, info);
}

//-------------------------------------------------------------------------

LRESULT CALLBACK WatchWndProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    
    static int selected;
    static POINT menupos;
    static char buf[256];
    RECT r,  *pr;
    WINDOWPOS wp;
    HD_ITEM hie;
    HD_LAYOUT hdl;
    NMHDR *h;
    DRAWITEMSTRUCT *dr;
    HBITMAP hbmp;
    HDC hMemDC;
    TCHeader tch;
    TV_ITEM item;
    TV_INSERTSTRUCT t;
    static int sizingbottom;
    int offset;
    DEBUG_INFO *dbg;
    NM_TREEVIEW *nmt;
    VARINFO *var;
    HWND win;
    int level;
    int offset1;
    int doit;
    int i;
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
            h = (NMHDR*)lParam;
            switch (h->code)
            {
            case TABN_SELECTED:
                {
                    LSTABNOTIFY *p = (LSTABNOTIFY *)h;
                    ShowWindow(hwndTree[selected], SW_HIDE);
                    for (i=0; i < 4; i++)
                        if (p->lParam == (LPARAM)hwndTree[i])
                            selected = i;
                    ShowWindow(hwndTree[selected], SW_SHOW);
                    break;
                }
            case NM_RCLICK:
                {
                    HMENU menu = LoadMenuGeneric(hInstance, "WATCHMENU");
                    HMENU popup = GetSubMenu(menu, 0);
                    InsertBitmapsInMenu(popup);
                    GetCursorPos(&menupos);
                    TrackPopupMenuEx(popup, TPM_BOTTOMALIGN | TPM_LEFTBUTTON,
                        menupos.x, menupos.y, hwndFrame, NULL);
                    DestroyMenu(menu);
                    SetFocus(hwndTree[selected]);
                }
                return 0;
            case TVN_ITEMEXPANDING:
                nmt = h;
                if (nmt->action)
                {
                    ExpandPointer((VARINFO*)nmt->itemNew.lParam, nmt->action, selected);
                }
                return 0;
            case TCN_EDITQUERY:
                nmt = h;
                item.mask = TVIF_PARAM;
                item.hItem = (HTREEITEM)nmt->itemNew.hItem;
                TreeView_GetItem(hwndTree[selected], &item);
                var = (VARINFO*)item.lParam;
                if (var->editable)
                {
                    WatchValue(watchinfo_list[selected][var->watchindex].dbg_info, buf,
                        var, TRUE);
                    return buf;
                }
                return 0;
            case TCN_EDITDONE:
                nmt = h;
                item.mask = TVIF_PARAM;
                item.hItem = (HTREEITEM)nmt->itemNew.hItem;
                TreeView_GetItem(hwndTree[selected], &item);
                var = (VARINFO*)item.lParam;
                ChangeData(var, nmt->itemNew.pszText, selected);
                RefreshItems(selected);
                return 0;
           }
            break;
        case WM_CREATE:
            hwndWatch = hwnd;
            GetClientRect(hwnd, &r);
            valueBitmap = LoadImage(hInstance, "ID_VALUEBMP", IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
            itemBitmap = LoadImage(hInstance, "ID_ITEMBMP", IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
            tch.colText1 = "Item";
            tch.colText2 = "Value";
            tch.colBmp1 = itemBitmap;
            tch.colBmp2 = valueBitmap;
            hwndTabCtrl = CreateLsTabWindow(hwnd, TABS_BOTTOM | TABS_HOTTRACK | TABS_FLAT | WS_VISIBLE);
            ApplyDialogFont(hwndTabCtrl);
            r.bottom -= 25;
            for (i=3; i >0 ; i--)
                hwndTree[i] = CreateextTreeWindow(hwnd, WS_DLGFRAME | TCS_LINE, &r, &tch);
            hwndTree[0] = CreateextTreeWindow(hwnd, WS_DLGFRAME | TCS_LINE | WS_VISIBLE, &r, &tch);
            for (i=3; i >=0 ; i--)
                SendMessage(hwndTabCtrl, TABM_ADD, (WPARAM)nameTags[i], (LPARAM)hwndTree[i]);
            return 0;
        case WM_ADDWATCHINDIRECT:
            win = (HWND)wParam;
            doit = FALSE;
            SendMessage(win, EM_EXGETSEL, (WPARAM)0, (LPARAM) &charrange);
            if (charrange.cpMin == charrange.cpMax)
            {
                doit = SendMessage(win, WM_WORDUNDERPOINT, (WPARAM)&rightclickPos, (LPARAM)buf);
                if (!doit)
                {
                    SendMessage(hwndFrame, IDM_ADDWATCH, 0, 0);
                    break;
                }
            }
            else
            {
                if (charrange.cpMax - charrange.cpMin < sizeof(buf))
                {
                    SendMessage(win, EM_GETSELTEXT, 0, (LPARAM)buf);
                    doit = TRUE ;
                }
            }
            if (doit)
            {
                var = EvalExpr(&dbg, activeScope, (char*) buf, TRUE);
                if (var)
                {
                    AddItem(dbg, var, activeScope->address, selected);
                    dmgrHideWindow(DID_WATCHWND, FALSE);
                    break ;
                }
            }
            ExtendedMessageBox("Error", MB_SETFOREGROUND |
                MB_SYSTEMMODAL, "Symbol does not exist in this scope");
            break;
        case WM_ADDWATCH:
            offset = wParam;
            if (!offset)
            {
                DeleteAllItems(selected);
            }
            else
            {
                var = EvalExpr(&dbg, activeScope, (char*)
                    lParam, TRUE);
                if (var)
                {
                    AddItem(dbg, var, activeScope->address, selected);
                    dmgrHideWindow(DID_WATCHWND, FALSE);
                }
            }
            break;
        case WM_COMMAND:
            switch (wParam)
            {
            case ID_SETADDRESS:
                for (i=3; i >=0; i--)
                    RefreshItems(i);
                InvalidateRect(hwndTree[selected], 0, 0);
                break;
            case IDM_DELETEWATCH:
                DeleteItem(&menupos, selected);
                break;
            case IDM_DELETEALLWATCH:
                DeleteAllItems(selected);
                break;
            }
            break;
        case WM_DESTROY:
            for (i=3; i >=0 ; i--)
                TreeView_DeleteAllItems(hwndTree[i]);
            for (i=3; i >=0 ; i--)
                DestroyWindow(hwndTree[i]);
            DeleteObject(valueBitmap);
            DeleteObject(itemBitmap);
            hwndWatch = 0;
            break;

        case WM_SIZE:
            r.left = 0;
            r.right = LOWORD(lParam);
            r.top = 0;
            r.bottom = HIWORD(lParam);
            MoveWindow(hwndTabCtrl, r.left, r.bottom - 24, r.right - r.left, 24, 1);
            for (i=3; i >=0 ; i--)
                MoveWindow(hwndTree[i], r.left, r.top, r.right - r.left, r.bottom -
                    r.top - 25, 0);
            return 0;
        case WM_CLOSE:
            break;
        case WM_ACTIVATEME:
            SendMessage(GetParent(hwnd), WM_ACTIVATEME, 0, 0);
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}

//-------------------------------------------------------------------------

void RegisterWatchWindow(void)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = 0;
    wc.lpfnWndProc = &WatchWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szWatchClassName;
    RegisterClass(&wc);
}

//-------------------------------------------------------------------------

void CreateWatchWindow(void)
{
    hwndWatch = CreateDockableWindow(DID_WATCHWND, szWatchClassName, szWatchTitle, hInstance, 200, 200);
}
