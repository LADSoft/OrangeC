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
#include <stdlib.h>
#include "helpid.h"
#include "header.h"

#define PAGE_SIZE 4096

extern THREAD *activeThread;
extern enum DebugState uState;
extern HINSTANCE hInstance;
extern HWND hwndFrame;
extern POINT rightclickPos;
extern SCOPE *activeScope;


char *databphist[MAX_COMBO_HISTORY];
DATABREAK *dataBpList;

static struct _pages
{
    struct _pages *next;
    DWORD page;
    DWORD oldProtect;
} *activePages;

static int resolvenametoaddr(char *name, int doErrors, DWORD *size, DWORD *addr)
{
    DEBUG_INFO *dbg;
    VARINFO *var;
    var = EvalExpr(&dbg, NULL, name, doErrors);
    if (var)
    {
        if (var->constant)
        {
            *addr = var->ival;
            *size = 4;
        }
        else if (var->address < 0x1000)
        {
            char data[20];
            //if (!var->explicitreg)
                //ExtendedMessageBox("Address error", MB_SETFOREGROUND |
                    //MB_SYSTEMMODAL, 
                    //"Address is a register.  Using its value as the address.");
            ReadValue(var->address, &data, 4, var);
            *addr = *(int*)data;
            *size = 4;
        } 
        else
        {
            *addr = var->address;
            *size = var->size;
        }
        FreeVarInfo(var);
        return 1;
    }
    else
    {
        return 0;
    }
}
void databpInit(void)
{
    while (dataBpList)
    {
        DATABREAK *next = dataBpList->next;
        free(dataBpList);
        dataBpList = next;
    }
}
void databpRemove(void)
{
    while (dataBpList)
    {
        DATABREAK *next = dataBpList->next;
        free(dataBpList);
        dataBpList = next;
    }
}
void databpSetBP(HANDLE hProcess)
{
    // may be called multiple time during DLL load..
    if (!activePages)
    {
        DATABREAK *search = dataBpList;
        struct _pages **prot, *toAdd;
        while (search)
        {
            if (search->active)
            {
                if (resolvenametoaddr(search->name, FALSE, &search->size, &search->address))
                {
                    DWORD base = search->address & - PAGE_SIZE;
                    DWORD pages = ((search->address + search->size + PAGE_SIZE - 1 ) - base)/PAGE_SIZE;
                    prot = &activePages;	
                    while (pages)
                    {
                        while (*prot && (*prot)->page < base)
                            prot = &(*prot)->next;
                        if (!*prot || (*prot)->page != base)
                        {
                            toAdd = calloc(1,sizeof(struct _pages));
                            if (toAdd)
                            {
                                toAdd->next = *prot;
                                *prot = toAdd;
                                toAdd->page = base;
                            }
                        }
                        prot = & (*prot)->next;
                        base += PAGE_SIZE;
                        pages--;
                    }
                }
            }
            search = search->next;
        }
        toAdd= activePages;
        while (toAdd)
        {
            VirtualProtectEx(hProcess, (LPVOID)toAdd->page, PAGE_SIZE, PAGE_NOACCESS, &toAdd->oldProtect);
            toAdd = toAdd->next;
        }
    }
}
void databpResetBP(HANDLE hProcess)
{
    struct _pages *prot = activePages;
    while (prot)
    {
        DWORD old;
        VirtualProtectEx(hProcess, (LPVOID)prot->page, PAGE_SIZE, prot->oldProtect, &old);
        prot = prot->next;
    }
    while (activePages)
    {
        prot = activePages->next;
        free(activePages);
        activePages = prot;
    }
}
void databpEnd(void)
{
}
int databpCheck(DEBUG_EVENT *stDE)
{
    int rv = 0; // real exception
    DWORD address = stDE->u.Exception.ExceptionRecord.ExceptionInformation[1];
    DATABREAK *search = dataBpList;
    while (search)
    {
        if (search->active && (search->address & - PAGE_SIZE) <= address && address < ((search->address + search->size + PAGE_SIZE - 1) & - PAGE_SIZE))
        {
            rv = 2; // in a paged out page, but assume not a real bp
            if (search->address <= address && address < search->address + search->size)
            {
                rv = 1; // ok a real bp, quit the search and let the UI know...
                break;
            }
        }
        search = search->next;
    }
    return rv;
}
int LoadDataBreakpoints(HWND hwnd)
{
    int items = 0;
    int i;
    LV_ITEM item;
    RECT r;
    HWND hwndLV = GetDlgItem(hwnd, IDC_BPLIST);
    LV_COLUMN lvC;
    DATABREAK *search;
    ListView_SetExtendedListViewStyle(hwndLV, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

    GetWindowRect(hwndLV, &r);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM;
    lvC.cx = 20;
    lvC.iSubItem = 0;
    ListView_InsertColumn(hwndLV, 0, &lvC);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM;
    lvC.cx = 32;
    lvC.iSubItem = 1;
    ListView_InsertColumn(hwndLV, 1, &lvC);
    lvC.mask = LVCF_FMT | LVCF_WIDTH | LVCF_SUBITEM;
    lvC.fmt = LVCFMT_LEFT;
    lvC.cx = r.right - r.left - 56;
    lvC.iSubItem = 2;
    ListView_InsertColumn(hwndLV, 2, &lvC);


    search = dataBpList;
    while (search)
    {
        int v;
        BOOL b = search->active;
        memset(&item, 0, sizeof(item));
        item.iItem = 10000;
        item.iSubItem = 0;
        item.mask = LVIF_PARAM;
        item.lParam = (LPARAM)search;
        v = ListView_InsertItem(hwndLV, &item);
        ListView_SetCheckState(hwndLV, v, b);
        search = search->next;
    }
    if (items)
    {
        ListView_SetSelectionMark(hwndLV, 0);
        ListView_SetItemState(hwndLV, 0, LVIS_SELECTED, LVIS_SELECTED);
    }

    return items;
}
void RemoveDataBp(HWND hwnd)
{
    HWND hwndLV = GetDlgItem(hwnd, IDC_BPLIST);
    int k = ListView_GetSelectionMark(hwndLV);
    int n = ListView_GetItemCount(hwndLV);
    int i,j;
    for (i=n-1; i >=0; i--)
    {
        DATABREAK **search = &dataBpList, *found;
        LVITEM item;
        memset(&item, 0, sizeof(item));
        item.iItem = i;
        item.iSubItem = 0;
        item.mask = LVIF_STATE;
        item.stateMask = LVIS_SELECTED;	
        ListView_GetItem(hwndLV, &item);
        if (item.state & LVIS_SELECTED)
        {
            for (j=0; j <i; j++)
                search = &(*search)->next;
            found = *search;
            *search = found->next;
            free(found);
            ListView_DeleteItem(hwndLV, i);
        }
        ListView_SetSelectionMark(hwndLV, i);
        ListView_SetItemState(hwndLV, i, LVIS_SELECTED, LVIS_SELECTED);
    }
}
INT_PTR CALLBACK DataBpAddProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    static char buf[256];
    HWND editwnd;
    switch (iMessage)
    {
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
            case IDOK: 
                editwnd = GetDlgItem(hwnd, IDC_EDDATABP);
                GetWindowText(editwnd, buf, 256);
                SendMessage(editwnd, WM_SAVEHISTORY, 0, 0);
                EndDialog(hwnd, (int)buf);
                break;
            case IDCANCEL:
                EndDialog(hwnd, 0);
                break;

            }
            switch (HIWORD(wParam))
            {
            case CBN_SELCHANGE:
                EnableWindow(GetDlgItem(hwnd, IDOK), TRUE);
                break;
            case CBN_EDITCHANGE:
                EnableWindow(GetDlgItem(hwnd, IDOK), GetWindowText((HWND)lParam,
                    buf, 2));
                break;
            }
            break;
        case WM_CLOSE:
            PostMessage(hwnd, WM_COMMAND, IDCANCEL, 0);
            break;
        case WM_INITDIALOG:
            CenterWindow(hwnd);
            editwnd = GetDlgItem(hwnd, IDC_EDDATABP);
            SubClassHistoryCombo(editwnd);
            SendMessage(editwnd, WM_SETHISTORY, 0, (LPARAM)databphist);
            EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);
            return TRUE;
    }
    return 0;
}
void AddDataBp(HWND hwnd)
{
    char *name = (char *)DialogBoxParam(hInstance, "ADDDATABPDIALOG", hwnd, 
        (DLGPROC)DataBpAddProc, 0);
    if (name)
    {
        DATABREAK **search = &dataBpList;
        DATABREAK *b = calloc(1, sizeof(DATABREAK));
        int v;
        HWND hwndLV = GetDlgItem(hwnd, IDC_BPLIST);
        LV_ITEM item;
        int i = ListView_GetItemCount(hwndLV);
        memset(&item, 0, sizeof(item));
        strcpy(b->name, name);
        while (*search)
            search = &(*search)->next;
        b->active = TRUE;
        *search = b;
        item.iItem = 10000;
        item.iSubItem = 0;
        item.mask = LVIF_PARAM;
        item.lParam = (LPARAM)b;
        v = ListView_InsertItem(hwndLV, &item);
        ListView_SetCheckState(hwndLV, v, TRUE);
        ListView_SetSelectionMark(hwndLV, i);
        ListView_SetItemState(hwndLV, i, LVIS_SELECTED, LVIS_SELECTED);
        
    }
}
LRESULT CALLBACK DataBpProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    switch (iMessage)
    {
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDOK:
                    EndDialog(hwnd, 0);
                    break;
                case IDC_REMOVEDATABP:
                    RemoveDataBp(hwnd);
                    break;
                case IDC_ADDDATABP:
                    AddDataBp(hwnd);
                    break;
                case IDHELP:
                ContextHelp(IDH_DATA_BREAKPOINTS_DIALOG);
                    break;
            }
            break;
        case WM_CLOSE:
            EndDialog(hwnd, 0);
            break;
        case WM_INITDIALOG:
            LoadDataBreakpoints(hwnd);
            CenterWindow(hwnd);
            break;
        case WM_NOTIFY:
            if (((LPNMHDR)lParam)->code == LVN_KEYDOWN)
            {
                switch (((LPNMLVKEYDOWN)lParam)->wVKey)
                {
                    case VK_INSERT:
                        if (GetKeyState(VK_CONTROL) & 0x80000000)
                        {
                            HWND hwndLV = GetDlgItem(hwnd, IDC_BPLIST);
                            ListView_SetCheckState(hwndLV, -1, TRUE);
                        }
                        else
                        {
                            HWND hwndLV = GetDlgItem(hwnd, IDC_BPLIST);
                            int i = ListView_GetSelectionMark(hwndLV);
                            ListView_SetCheckState(hwndLV, i, TRUE);
                        }
                        break;
                    case VK_DELETE:
                        if (GetKeyState(VK_CONTROL) & 0x80000000)
                        {
                            HWND hwndLV = GetDlgItem(hwnd, IDC_BPLIST);
                            ListView_SetCheckState(hwndLV, -1, FALSE);
                        }
                        else
                        {
                            HWND hwndLV = GetDlgItem(hwnd, IDC_BPLIST);
                            int i = ListView_GetSelectionMark(hwndLV);
                            ListView_SetCheckState(hwndLV, i, FALSE);
                        }
                        break;
                }
            }
            if (wParam == IDC_BPLIST)
            {
                if (((LPNMHDR)lParam)->code == LVN_GETDISPINFO)
                {
                    LV_DISPINFO *plvdi = (LV_DISPINFO*)lParam;
                    DWINFO *ptr;
                    plvdi->item.mask |= LVIF_TEXT | LVIF_DI_SETITEM;
                    plvdi->item.mask &= ~LVIF_STATE;
                    switch (plvdi->item.iSubItem)
                    {
                    case 2:
                        plvdi->item.pszText = ((DATABREAK *)plvdi->item.lParam)->name;
                        break;
                    default:
                        plvdi->item.pszText = "";
                        break;
                    }
                }
                else if (((LPNMHDR)lParam)->code == LVN_ITEMCHANGED)
                {
                    LPNMLISTVIEW  lpnmListView = (LPNMLISTVIEW)lParam;
                    HWND hwndLV = GetDlgItem(hwnd, IDC_BPLIST);
                    ((DATABREAK *)lpnmListView->lParam)->active = ListView_GetCheckState(hwndLV, lpnmListView->iItem);
                }
            }
            break;
    }
    return 0;
}
void databp(HWND hwnd)
{
    if (hwnd == NULL)
    {
        DialogBoxParam(hInstance, "DATABPDLG", hwndFrame, 
            (DLGPROC)DataBpProc, 0);
    }
    else
    {
        DATABREAK *b = (DATABREAK *)calloc(sizeof(DATABREAK),1);
           BOOL doit = SendMessage(hwnd, WM_WORDUNDERPOINT, (WPARAM)&rightclickPos, (LPARAM)b->name);
        if (doit)
        {
            DATABREAK **search = &dataBpList;
            while (*search)
                search = &(*search)->next;
            b->active = TRUE;
            *search = b;
        }
        else
        {
            free(b);
        }
    }
}
