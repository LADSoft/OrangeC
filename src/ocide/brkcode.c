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
#include <richedit.h>

#include "header.h"

extern PROCESS *activeProcess;
extern HWND hwndASM;
extern DWINFO *editWindows;
extern enum DebugState uState;

//-------------------------------------------------------------------------

void SetTempBreakPoint(int procid, int threadid, int *addresses)
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

int SittingOnBreakPoint(DEBUG_EVENT *dbe)
{
    return isBreakPoint((int)dbe->u.Exception.ExceptionRecord.ExceptionAddress);
}

//-------------------------------------------------------------------------

void WriteBreakPoint(HANDLE hProcess, int address, int value)
{
    int bf = value;
    MEMORY_BASIC_INFORMATION mbi;
    DWORD dwOldProtect;

    //	if (!IsNT() && address >= 0x80000000)
    //		return ;
    if (!VirtualQueryEx(hProcess, (LPVOID)address, &mbi, sizeof(mbi)))
        ExtendedMessageBox("Debugger", MB_SYSTEMMODAL, "Could not query pages %d",
            GetLastError());
    // the following fails on win98
    /*if*/ (!VirtualProtectEx(hProcess, mbi.BaseAddress, mbi.RegionSize,
        PAGE_EXECUTE_WRITECOPY, &mbi.Protect));
//        ExtendedMessageBox("Debugger", MB_SYSTEMMODAL, 
//            "Could not protect pages %d", GetLastError());

    if (!WriteProcessMemory(hProcess, (LPVOID)address, (LPVOID) &bf, 1, 0))
        ExtendedMessageBox("Debugger", MB_SYSTEMMODAL, 
            "Could not write breakpoint address %x %d", address, GetLastError())
            ;

    VirtualProtectEx(hProcess, mbi.BaseAddress, mbi.RegionSize, mbi.Protect,
        &dwOldProtect);
    FlushInstructionCache(hProcess, (LPVOID)address, 1);
}

//-------------------------------------------------------------------------

void allocBreakPoint(HANDLE hProcess, BREAKPOINT *pt)
{
    unsigned char bf;
    if (!pt->active && pt->addresses)
    {
        int i =0,j;
        int *p;
        for (p = pt->addresses; *p !=0; ++p)
            i++;
        if (!pt->tempvals)
        {
             pt->tempvals = calloc(i, sizeof(int));
        }
        for (i=0, p = pt->addresses; *p !=0; ++p, ++i)
        {
            DWORD len;
        
            if (ReadProcessMemory(hProcess, (LPVOID)pt->addresses[i], (LPVOID) &bf, 1,
                &len))
            {
                if (!len)
                    return ;
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
        }
    }
}

//-------------------------------------------------------------------------

void freeBreakPoint(HANDLE hProcess, BREAKPOINT *pt)
{
    if (pt->active)
    {
        int i;
        int *p;
        for (i=0, p = pt->addresses; p && *p !=0; ++p, ++i)
        {
            if (pt->tempvals != -1)
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
    BREAKPOINT *p = &activeProcess->breakpoints;
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
    BREAKPOINT *p = &activeProcess->breakpoints;
    THREAD *th = activeProcess->threads;
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
        int *p;
        for (p = activeProcess->breakpoints.addresses; *p; ++p)
            if (*p == addr)
                return TRUE;
    }
    return FALSE;
}


int isBreakPoint(int addr)
{
    BREAKPOINT *p = activeProcess->breakpoints.next;
    while (p)
    {
        int *q;
        for (q = p->addresses; q && *q !=0; ++q)
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
    THREAD *th = activeProcess->threads;
    while (th)
    {
        int *q;
        for (q = th->breakpoint.addresses; q && *q !=0; ++q)
        {
            if (addr == *q)
                return TRUE;
        }
        th = th->next;
    }
    return isBreakPoint(addr);
}

//-------------------------------------------------------------------------

int dbgSetBreakPoint(char *name, int linenum, char *extra)
{
    BREAKPOINT **p = &activeProcess->breakpoints.next;
    if (uState == notDebugging)
        return 1;
    // will be checked when entering debugger
    if (!name)
    {
        // it was from the assembly window
        int addr = linenum;
        while (*p)
        {
            int *q;
            for (q = (*p)->addresses; q && *q !=0; ++q)
            {
                if (addr == *q)
                    return 1;
            }
            p = &(*p)->next;
        }
        *p = calloc(1,sizeof(BREAKPOINT));
        if (*p)
        {
            (*p)->addresses = calloc(2, sizeof(int ));
            (*p)->addresses[0] = addr;
            (*p)->extra = extra;
            GetBreakpointLine(addr, &(*p)->module, &(*p)->linenum, FALSE);
            if (hwndASM)
                InvalidateRect(hwndASM, 0, 0);
//            Tag(TAG_BP, (*p)->module, (*p)->linenum, 0, 0, 0, 0);
            if (uState == Running)
                allocBreakPoint(activeProcess->hProcess,  *p);
        }
        return 1;
    }
    else
    {
        // it was from a source module
        int *addresses = GetBreakpointAddresses(name, &linenum);
        if (addresses)
        {
            int *t;
            for (t = addresses; *t !=0; ++t)
            {
                p = &activeProcess->breakpoints.next;
                while (*p)
                {
                    int *q;
                    for (q = (*p)->addresses; q && *q !=0; ++q)
                    {
                        if (*t == *q)
                            return 1;
                    }
                    p = &(*p)->next;
                }
            }
            *p = calloc(1,sizeof(BREAKPOINT));
            if (*p)
            {
                (*p)->addresses = addresses;
                (*p)->extra = extra;
                strcpy((*p)->module, name);
                (*p)->linenum = linenum;
                InvalidateRect(hwndASM, 0, 0);
                if (uState == Running)
                    allocBreakPoint(activeProcess->hProcess,  *p);
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

void dbgClearBreakPoint(char *name, int linenum)
{
    BREAKPOINT **p = &activeProcess->breakpoints.next;
    if (uState == notDebugging)
        return ;
    if (!name)
    {
        // from ASM window
        int addr = linenum;
        while (*p)
        {
            int *t;
            for (t = (*p)->addresses; t && *t !=0; ++t)
            {
                if (*t == addr)
                    break;
            }
            if (*t)
            {
                BREAKPOINT *q =  *p;

                *p = (*p)->next;
//                Tag(TAG_BP, q->module, q->linenum, 0, 0, 0, 0);
                if (uState == Running)
                    freeBreakPoint(activeProcess->hProcess, q);
                free(q->addresses);
                free(q->tempvals);
                free(q->extra);
                free(q);
                InvalidateRect(hwndASM, 0, 0);
                return ;
            }
            p = &(*p)->next;
        }
    }
    else
    {
        int *addresses = GetBreakpointAddresses(name, &linenum);
        if (addresses)
        {
            int addr = addresses[0];
            while (*p)
            {
                int *t;
                for (t = (*p)->addresses; t && *t !=0; ++t)
                {
                    if (*t == addr)
                        break;
                }
                if (*t)
                {
                    BREAKPOINT *q =  *p;
                    *p = (*p)->next;
                    if (uState == Running)
                        freeBreakPoint(activeProcess->hProcess, q);
                    free(q->addresses);
                    free(q->tempvals);
                    free(q->extra);
                    free(q);
                    if (hwndASM)
                        InvalidateRect(hwndASM, 0, 0);
                    return ;
                }
                p = &(*p)->next;
            }
            free(addresses);
        }
    }
}

//-------------------------------------------------------------------------

void SetBP(DEBUG_EVENT *dbe)
{
    BREAKPOINT **p = &activeProcess->breakpoints.next;
    if (hwndASM && GetFocus() == hwndASM)
    {
        int addr = SendMessage(hwndASM, WM_GETCURSORADDRESS, 0, 0);
        Tag(TAG_BP, 0, addr, 0, 0, 0, 0);
        if (uState == Running)
            allocBreakPoint(activeProcess->hProcess,  *p);
    }
    else
    {
        HWND hWnd = GetFocus();
        DWINFO *ptr = editWindows;
        while (ptr)
        {
            if (ptr->dwHandle == hWnd)
            {
                int addr, linenum;
                SendMessage(ptr->dwHandle, EM_GETSEL, (WPARAM) &linenum, 0);
                linenum = SendMessage(ptr->dwHandle, EM_EXLINEFROMCHAR, 0,
                    linenum) + 1;
                Tag(TAG_BP, ptr->dwName, linenum, 0, 0, 0, 0);
                break;
            }
            ptr = ptr->next;
        }
    }
    if (hwndASM)
        InvalidateRect(hwndASM, 0, 0);
}

