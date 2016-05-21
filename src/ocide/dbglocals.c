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

extern HWND hwndWatchTree[5];
extern WATCHINFO *watchinfo_list[5];
extern int watchinfo_max[5];
extern int watchinfo_count[5];

HWND hwndLocals;
static HBITMAP valueBitmap, itemBitmap;
static char szLocalsClassName[] = "xccLocalsClass";
static char szLocalsTitle[] = "Locals Window";
static HWND hwndTabCtrl;

void LoadLocals(void)
{
    NAMELIST *names = FindEnclosedAutos(activeScope);
    WATCHINFO *p = watchinfo_list[4];
    int i;
    for (i=0; i < watchinfo_count[4]; i++)
        p[i].marked = TRUE;
        
    while (names)
    {
        NAMELIST *next = names->next;
        DEBUG_INFO *dbg;
        VARINFO *var = EvalExpr(&dbg, activeScope, (char*) names->data+1, FALSE);
        if (var)
        {
            AddItem(dbg, var, activeScope->address, 4);
        }
        free(names->data);
        free(names);
        names = next;
    }
    p = watchinfo_list[4];
    for (i=0; i < watchinfo_count[4]; i++)
        if (p[i].marked)
        {
            WATCHINFO *x = &watchinfo_list[4][i];
            int j;
            FreeTree(x->info, 4);
            FreeVarInfo(x->info);
            for (j = i; j < watchinfo_count[4] - 1; j++)
            {
                watchinfo_list[4][j] = watchinfo_list[4][j + 1];
                RenumberDeleteItems(watchinfo_list[4][j].info);
            }
            watchinfo_count[4]--;
            i--;
        }
}
//-------------------------------------------------------------------------

LRESULT CALLBACK LocalsWndProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    
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
            case TVN_ITEMEXPANDING:
                nmt = (LPNMTREEVIEW)h;
                if (nmt->action)
                {
                    ExpandPointer((VARINFO*)nmt->itemNew.lParam, nmt->action, 4);
                }
                return 0;
            case TCN_EDITQUERY:
                nmt = (LPNMTREEVIEW)h;
                item.mask = TVIF_PARAM;
                item.hItem = (HTREEITEM)nmt->itemNew.hItem;
                TreeView_GetItem(hwndWatchTree[4], &item);
                var = (VARINFO*)item.lParam;
                if (var->editable)
                {
                    WatchValue(watchinfo_list[4][var->watchindex].dbg_info, buf,
                        var, TRUE);
                    return buf;
                }
                return 0;
            case TCN_EDITDONE:
                nmt = (LPNMTREEVIEW)h;
                item.mask = TVIF_PARAM;
                item.hItem = (HTREEITEM)nmt->itemNew.hItem;
                TreeView_GetItem(hwndWatchTree[4], &item);
                var = (VARINFO*)item.lParam;
                ChangeData(var, nmt->itemNew.pszText, 4);
                RefreshItems(4);
                return 0;
           }
            break;
        case WM_CREATE:
            hwndLocals = hwnd;
            GetClientRect(hwnd, &r);
            valueBitmap = LoadImage(hInstance, "ID_VALUEBMP", IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
            itemBitmap = LoadImage(hInstance, "ID_ITEMBMP", IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
            tch.colText1 = "Item";
            tch.colText2 = "Value";
            tch.colBmp1 = itemBitmap;
            tch.colBmp2 = valueBitmap;
            hwndWatchTree[4] = CreateextTreeWindow(hwnd, WS_DLGFRAME | TCS_LINE | WS_VISIBLE, &r, &tch);
            return 0;
        case WM_COMMAND:
            switch (wParam)
            {
            case ID_SETADDRESS:
                LoadLocals();
                RefreshItems(4);
                InvalidateRect(hwndWatchTree[4], 0, 0);
                break;
            }
            break;
        case WM_DESTROY:
            TreeView_DeleteAllItems(hwndWatchTree[4]);
            DestroyWindow(hwndWatchTree[4]);
            DeleteObject(valueBitmap);
            DeleteObject(itemBitmap);
            hwndLocals = 0;
            break;

        case WM_SIZE:
            r.left = 0;
            r.right = LOWORD(lParam);
            r.top = 0;
            r.bottom = HIWORD(lParam);
            MoveWindow(hwndWatchTree[4], r.left, r.top, r.right - r.left, r.bottom -
                r.top, 1);
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

void RegisterLocalsWindow(void)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = 0;
    wc.lpfnWndProc = &LocalsWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szLocalsClassName;
    RegisterClass(&wc);
}

//-------------------------------------------------------------------------

void CreateLocalsWindow(void)
{
    hwndLocals = CreateDockableWindow(DID_LOCALSWND, szLocalsClassName, szLocalsTitle, hInstance, 200, 200);
}
