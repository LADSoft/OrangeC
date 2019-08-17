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
#include <richedit.h>

#include "header.h"

extern HINSTANCE hInstance;
extern HWND hwndFrame;
extern PROCESS* activeProcess;
extern HWND hwndASM;
extern DWINFO* editWindows;
extern enum DebugState uState;
extern SCOPE* activeScope;

char* funcbphist[MAX_COMBO_HISTORY];
//-------------------------------------------------------------------------

void SetTempBreakPoint(int procid, int threadid, int* addresses)
{
    ClearTempBreakPoint(procid);
    activeProcess->breakpoints.addresses = addresses;
    activeProcess->idTempBpThread = threadid;
}

//-------------------------------------------------------------------------

void ClearTempBreakPoint(int procid)
{
    free(activeProcess->breakpoints.addresses);
    free(activeProcess->breakpoints.tempvals);
    activeProcess->breakpoints.addresses = NULL;
    activeProcess->breakpoints.tempvals = NULL;
}

//-------------------------------------------------------------------------

int SittingOnBreakPoint(DEBUG_EVENT* dbe) { return isBreakPoint((int)dbe->u.Exception.ExceptionRecord.ExceptionAddress); }

//-------------------------------------------------------------------------

void WriteBreakPoint(HANDLE hProcess, int address, int value)
{
    int bf = value;
    MEMORY_BASIC_INFORMATION mbi;
    DWORD dwOldProtect;

    //	if (!IsNT() && address >= 0x80000000)
    //		return ;
    if (!VirtualQueryEx(hProcess, (LPVOID)address, &mbi, sizeof(mbi)))
        ExtendedMessageBox("Debugger", MB_SYSTEMMODAL, "Could not query pages %d", GetLastError());
    // the following fails on win98
    VirtualProtectEx(hProcess, mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_WRITECOPY, &mbi.Protect);

    if (!WriteProcessMemory(hProcess, (LPVOID)address, (LPVOID)&bf, 1, 0))
        ExtendedMessageBox("Debugger", MB_SYSTEMMODAL, "Could not write breakpoint address %x %d", address, GetLastError());

    VirtualProtectEx(hProcess, mbi.BaseAddress, mbi.RegionSize, mbi.Protect, &dwOldProtect);
    FlushInstructionCache(hProcess, (LPVOID)address, 1);
}

//-------------------------------------------------------------------------

void allocBreakPoint(HANDLE hProcess, BREAKPOINT* pt)
{
    unsigned char bf;
    if (!pt->active && pt->addresses)
    {
        int i = 0;
        int* p;
        for (p = pt->addresses; *p != 0; ++p)
            i++;
        if (!pt->tempvals)
        {
            pt->tempvals = calloc(i, sizeof(int));
        }
        strcpy(pt->name, "unknown");
        for (i = 0, p = pt->addresses; *p != 0; ++p, ++i)
        {
            DWORD len;
            if (ReadProcessMemory(hProcess, (LPVOID)pt->addresses[i], (LPVOID)&bf, 1, &len))
            {
                if (!len)
                    return;
                if (bf == 0xcc)
                {
                    pt->tempvals[i] = -1;
                }
                else
                {
                    WriteBreakPoint(hProcess, pt->addresses[i], 0xcc);
                    pt->active = TRUE;
                    pt->tempvals[i] = bf;
                }
            }
            if (i == 0)
            {
                char name[256];
                DWORD n = FindFunctionName(name, pt->addresses[i], NULL, NULL);
                if (n)
                    sprintf(pt->name, "%s + 0x%x", name, pt->addresses[i] - n);
            }
        }
    }
}

//-------------------------------------------------------------------------

void freeBreakPoint(HANDLE hProcess, BREAKPOINT* pt)
{
    if (pt->active)
    {
        int i;
        int* p;
        for (i = 0, p = pt->addresses; p && *p != 0; ++p, ++i)
        {
            if (pt->tempvals != (int*)-1 && pt->addresses && pt->tempvals)
            {
                WriteBreakPoint(hProcess, pt->addresses[i], pt->tempvals[i]);
            }
        }
        pt->active = FALSE;
    }
}

//-------------------------------------------------------------------------

void SetBreakPoints(int procid)
{
    BREAKPOINT* p = &activeProcess->breakpoints;
    hbpSetBP();
    databpSetBP(activeProcess->hProcess);
    while (p)
    {
        if (p->addresses)
            allocBreakPoint(activeProcess->hProcess, p);
        p = p->next;
    }
}

//-------------------------------------------------------------------------

void ClearBreakPoints(int procid)
{
    BREAKPOINT* p = &activeProcess->breakpoints;
    THREAD* th = activeProcess->threads;
    hbpResetBP();
    databpResetBP(activeProcess->hProcess);
    while (p)
    {
        freeBreakPoint(activeProcess->hProcess, p);
        p = p->next;
    }
    while (th)
    {
        freeBreakPoint(activeProcess->hProcess, &th->breakpoint);
        th = th->next;
    }
}

//-------------------------------------------------------------------------
int inTempBreakPoint(int addr)
{
    if (activeProcess->breakpoints.addresses)
    {
        int* p;
        for (p = activeProcess->breakpoints.addresses; *p; ++p)
            if (*p == addr)
                return TRUE;
    }
    return FALSE;
}

int isBreakPoint(int addr)
{
    BREAKPOINT* p = activeProcess->breakpoints.next;
    while (p)
    {
        int* q;
        for (q = p->addresses; q && *q != 0; ++q)
        {
            if (addr == *q)
                return TRUE;
        }
        p = p->next;
    }
    return FALSE;
}

//-------------------------------------------------------------------------

int isLocalBreakPoint(int addr)
{
    THREAD* th = activeProcess->threads;
    while (th)
    {
        int* q;
        for (q = th->breakpoint.addresses; q && *q != 0; ++q)
        {
            if (addr == *q)
                return TRUE;
        }
        th = th->next;
    }
    return isBreakPoint(addr);
}

int dbgSetBreakPoint(char* name, int linenum, char* extra)
{
    BREAKPOINT** p = &activeProcess->breakpoints.next;
    if (uState == notDebugging)
        return 1;
    // will be checked when entering debugger
    if (!name)
    {
        // it was from the assembly window
        int addr = linenum;
        while (*p)
        {
            int* q;
            for (q = (*p)->addresses; q && *q != 0; ++q)
            {
                if (addr == *q)
                    return 1;
            }
            p = &(*p)->next;
        }
        *p = calloc(1, sizeof(BREAKPOINT));
        if (*p)
        {
            (*p)->addresses = calloc(2, sizeof(int));
            (*p)->addresses[0] = addr;
            (*p)->extra = extra;
            GetBreakpointLine(addr, &(*p)->module[0], &(*p)->linenum, FALSE);
            if (hwndASM)
                InvalidateRect(hwndASM, 0, 0);
            //            Tag(TAG_BP, (*p)->module, (*p)->linenum, 0, 0, 0, 0);
            if (uState == Running)
            {
                allocBreakPoint(activeProcess->hProcess, *p);
            }
            else
            {
                char name[256];
                DWORD n = FindFunctionName(name, (*p)->addresses[0], NULL, NULL);
                if (n)
                    sprintf((*p)->name, "%s + 0x%x", name, (*p)->addresses[0] - n);
            }
        }
        SendDIDMessage(DID_BREAKWND, WM_RESTACK, 0, 0);
        return 1;
    }
    else
    {
        // it was from a source module
        int* addresses = GetBreakpointAddresses(name, &linenum);
        if (addresses)
        {
            int* t;
            for (t = addresses; *t != 0; ++t)
            {
                p = &activeProcess->breakpoints.next;
                while (*p)
                {
                    int* q;
                    for (q = (*p)->addresses; q && *q != 0; ++q)
                    {
                        if (*t == *q)
                            return 1;
                    }
                    p = &(*p)->next;
                }
            }
            *p = calloc(1, sizeof(BREAKPOINT));
            if (*p)
            {
                (*p)->addresses = addresses;
                (*p)->extra = extra;
                strcpy((*p)->module, name);
                (*p)->linenum = linenum;
                if (hwndASM)
                    InvalidateRect(hwndASM, 0, 0);
                if (uState == Running)
                {
                    allocBreakPoint(activeProcess->hProcess, *p);
                }
                else
                {
                    char name[256];
                    DWORD n = FindFunctionName(name, (*p)->addresses[0], NULL, NULL);
                    if (n)
                        sprintf((*p)->name, "%s + 0x%x", name, (*p)->addresses[0] - n);
                }
            }
            else
            {
                free(addresses);
            }
            return 1;
        }
        else
            return 0;
        // couldn't locate line, invalid line...
    }
}

//-------------------------------------------------------------------------
void dbgClearBreakpointsForModule(DLL_INFO* dllinfo)
{
    // this doesn't actually unwrite the breakpoint.
    // it is meant to be called at DLL unload time, when the DLL is already gone...
    BREAKPOINT** p = &activeProcess->breakpoints.next;
    if (uState == notDebugging)
        return;
    while (*p)
    {
        int* t;
        BOOL found = FALSE;
        for (t = (*p)->addresses; t && *t != 0; ++t)
        {
            if (*t >= dllinfo->base &&
                ((dllinfo->next && *t < dllinfo->next->base) || (!dllinfo->next && dllinfo->base > activeProcess->base)) &&
                (dllinfo->base > activeProcess->base || *t < activeProcess->base))
            {
                BREAKPOINT* q = *p;
                found = TRUE;
                *p = (*p)->next;
                free(q->addresses);
                free(q->tempvals);
                free(q->extra);
                free(q);
                break;
            }
        }
        if (!found)
            p = &(*p)->next;
    }
}
void dbgClearBreakPoint(char* name, int linenum)
{
    BREAKPOINT** p = &activeProcess->breakpoints.next;
    if (uState == notDebugging)
        return;
    if (!name)
    {
        // from ASM window
        int addr = linenum;
        while (*p)
        {
            int* t;
            for (t = (*p)->addresses; t && *t != 0; ++t)
            {
                if (*t == addr)
                    break;
            }
            if (*t)
            {
                BREAKPOINT* q = *p;

                *p = (*p)->next;
                //                Tag(TAG_BP, q->module, q->linenum, 0, 0, 0, 0);
                if (uState == Running)
                    freeBreakPoint(activeProcess->hProcess, q);
                free(q->addresses);
                free(q->tempvals);
                free(q->extra);
                free(q);
                if (hwndASM)
                    InvalidateRect(hwndASM, 0, 0);
                SendDIDMessage(DID_BREAKWND, WM_RESTACK, 0, 0);
                return;
            }
            p = &(*p)->next;
        }
    }
    else
    {
        int* addresses = GetBreakpointAddresses(name, &linenum);
        if (addresses)
        {
            int addr = addresses[0];
            while (*p)
            {
                int* t;
                for (t = (*p)->addresses; t && *t != 0; ++t)
                {
                    if (*t == addr)
                        break;
                }
                if (*t)
                {
                    BREAKPOINT* q = *p;
                    *p = (*p)->next;
                    if (uState == Running)
                        freeBreakPoint(activeProcess->hProcess, q);
                    free(q->addresses);
                    free(q->tempvals);
                    free(q->extra);
                    free(q);
                    if (hwndASM)
                        InvalidateRect(hwndASM, 0, 0);
                    SendDIDMessage(DID_BREAKWND, WM_RESTACK, 0, 0);
                    return;
                }
                p = &(*p)->next;
            }
            free(addresses);
        }
    }
}

//-------------------------------------------------------------------------

void SetBP(DEBUG_EVENT* dbe)
{
    BREAKPOINT** p = &activeProcess->breakpoints.next;
    if (hwndASM && GetFocus() == hwndASM)
    {
        int addr = SendMessage(hwndASM, WM_GETCURSORADDRESS, 0, 0);
        Tag(TAG_BP, 0, addr, 0, 0, 0, 0);
        if (uState == Running)
            allocBreakPoint(activeProcess->hProcess, *p);
    }
    else
    {
        HWND hWnd = GetFocus();
        DWINFO* ptr = editWindows;
        while (ptr)
        {
            if (ptr->active && ptr->dwHandle == hWnd)
            {
                int linenum;
                SendMessage(ptr->dwHandle, EM_GETSEL, (WPARAM)&linenum, 0);
                linenum = SendMessage(ptr->dwHandle, EM_EXLINEFROMCHAR, 0, linenum) + 1;
                Tag(TAG_BP, ptr->dwName, linenum, 0, 0, 0, 0);
                break;
            }
            ptr = ptr->next;
        }
    }
    if (hwndASM)
        InvalidateRect(hwndASM, 0, 0);
}
LRESULT CALLBACK FunctionBPProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    static char buf[256];
    HWND editwnd;
    switch (iMessage)
    {
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDOK:
                    editwnd = GetDlgItem(hwnd, IDC_EDFUNC);
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
                    EnableWindow(GetDlgItem(hwnd, IDOK), GetWindowText((HWND)lParam, buf, 2));
                    break;
            }
            break;
        case WM_CLOSE:
            PostMessage(hwnd, WM_COMMAND, IDCANCEL, 0);
            break;
        case WM_INITDIALOG:
            CenterWindow(hwnd);
            editwnd = GetDlgItem(hwnd, IDC_EDWATCH);
            SubClassHistoryCombo(editwnd);
            SendMessage(editwnd, WM_SETHISTORY, 0, (LPARAM)funcbphist);
            if (!funcbphist[0])
                EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);
            return TRUE;
    }
    return 0;
}
LRESULT CALLBACK FunctionBPSelectProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    FUNCTIONLIST* list;
    LV_COLUMN lvC;
    LV_ITEM item;
    RECT r;
    HWND hwndlb;
    int i;
    switch (iMessage)
    {
        case WM_NOTIFY:
            if (wParam == IDC_FBPLISTBOX)
            {
                if (((LPNMHDR)lParam)->code == NM_DBLCLK)
                {
                    SendMessage(hwnd, WM_COMMAND, IDC_BMGOTO, 0);
                }
            }
            break;
        case WM_COMMAND:
            if (wParam == IDCANCEL)
            {
                EndDialog(hwnd, 0);
            }
            else if (wParam == IDC_BMGOTO)
            {

                int sel = ListView_GetSelectionMark(GetDlgItem(hwnd, IDC_FBPLISTBOX));
                if (sel == -1)
                {
                    EndDialog(hwnd, 0);
                    break;
                }
                item.iItem = sel;
                item.iSubItem = 0;
                item.mask = LVIF_PARAM;
                ListView_GetItem(GetDlgItem(hwnd, IDC_FBPLISTBOX), &item);
                EndDialog(hwnd, item.lParam);
            }
            break;
        case WM_INITDIALOG:
            list = (FUNCTIONLIST*)lParam;
            CenterWindow(hwnd);
            hwndlb = GetDlgItem(hwnd, IDC_FBPLISTBOX);
            ApplyDialogFont(hwndlb);
            GetWindowRect(hwndlb, &r);
            lvC.mask = LVCF_WIDTH | LVCF_SUBITEM;
            lvC.cx = r.right - r.left;
            lvC.iSubItem = 0;
            ListView_InsertColumn(hwndlb, 0, &lvC);
            i = 0;
            while (list)
            {
                item.iItem = i++;
                item.iSubItem = 0;
                item.mask = LVIF_PARAM | LVIF_TEXT;
                item.lParam = (LPARAM)list;
                item.pszText = list->name;
                ListView_InsertItem(hwndlb, &item);
                list = list->next;
            }
            break;
    }
    return 0;
}
void functionbp(void)
{
    char* name = (char*)DialogBox(hInstance, "FUNCTIONBPDIALOG", hwndFrame, (DLGPROC)FunctionBPProc);
    if (name)
    {
        char buf[2048];
        GetQualifiedName(buf, &name, FALSE, TRUE);
        // we aren't handling cast operators for now...
        if (*name)
        {
            name = NULL;
        }
        else
        {
            int count = 0;
            FUNCTIONLIST *list, *selected = NULL;
            DEBUG_INFO* dbg = findDebug(activeScope->address);
            list = GetFunctionList(dbg, activeScope, buf);
            if (list && list->next)
            {
                selected = (FUNCTIONLIST*)DialogBoxParam(hInstance, "FUNCTIONBPSELECTDIALOG", hwndFrame,
                                                         (DLGPROC)FunctionBPSelectProc, (LPARAM)list);
            }
            else
            {
                selected = list;
            }
            if (selected)
            {
                int line;
                char module[MAX_PATH];
                if (GetBreakpointLine(selected->address, module, &line, FALSE))
                {
                    Tag(TAG_BP, module, line, 0, 0, 0, 0);
                }
            }
            if (list)
            {
                while (list)
                {
                    FUNCTIONLIST* l = list;
                    list = list->next;
                    free(l);
                }
            }
        }
    }
}
