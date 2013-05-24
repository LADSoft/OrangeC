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
#include "helpid.h"
#include "header.h"

extern PROCESS *activeProcess;
extern THREAD *activeThread;
extern enum DebugState uState;
extern HINSTANCE hInstance;
extern HWND hwndFrame;

static int bkReg;
static int currentState;

#define HBP_WRITE 2
#define HBP_READ 1
#define HBP_EXECUTE 0

#define HBP_BYTE 0
#define HBP_WORD 1
#define HBP_DWORD 3

char hbpNames[4][256];
int hbpAddresses[4];
char hbpModes[4], hbpSizes[4], hbpEnables;

static int resolvenametoaddr(int index, int doErrors)
{
    int offset, addr, linoffs;
    DEBUG_INFO *dbg;
    VARINFO *var;
    LDT_ENTRY entry;
    char buf[256];
    if (!GetThreadSelectorEntry(activeThread->hThread, activeThread
        ->regs.SegDs, &entry))
        return 0;
    linoffs = entry.BaseLow + (entry.HighWord.Bytes.BaseMid << 16) + 
        (entry.HighWord.Bytes.BaseHi << 24);
    strcpy(buf, hbpNames[index]);
    var = EvalExpr(&dbg, NULL, buf, doErrors);
    if (var)
    {
        if (var->constant)
            addr = var->ival;
        else if (var->address < 0x1000)
        {
            char data[20];
            //if (!var->explicitreg)
                //ExtendedMessageBox("Address error", MB_SETFOREGROUND |
                    //MB_SYSTEMMODAL, 
                    //"Address is a register.  Using its value as the address.");
            ReadValue(var->address, &data, 4, var);
            addr = *(int*)data;
        } 
        else
            addr = var->address;
        FreeVarInfo(var);
        hbpAddresses[index] = addr + linoffs;
        return 1;
    }
    else
    {
        return 0;
    }
}

/* if a new project is loaded or closed, reset the breakpoint settings */
void hbpInit(void)
{
    int i;
    hbpResetBP();
    hbpEnables = 0;
    for (i = 0; i < 4; i++)
    {
        hbpNames[i][0] = 0;
        hbpModes[i] = HBP_READ | HBP_WRITE;
        hbpSizes[i] = HBP_DWORD;
    }
}

void hbpDisable(void)
{
    hbpEnables = 0;
}
/* called when bp are setting */
void hbpSetBP(void)
{
    int xdr0 = 0, xdr1 = 0, xdr2 = 0, xdr3 = 0, xdr6 = 0, xdr7 = 0x0000;
    int i, offset;
    THREAD *t;
    int flag = FALSE;
    for (i = 0; i < 4; i++)
    if (hbpEnables &(1 << i))
    {
        if (!resolvenametoaddr(i, FALSE))
        {
            ExtendedMessageBox("Hardware Breakpoint", 0, 
                "Could not locate symbol %s, resetting hardware BP",
                hbpNames[i]);
            hbpEnables &= ~(1 << i);
        }
    }
    if (1)
    {
        // proj && (proj->buildFlags & BF_HWBP)) {
        if ((hbpEnables &1))
        {
            xdr0 = hbpAddresses[0];
            xdr7 |= 1+(((hbpSizes[0] << 2) + hbpModes[0]) << 16);
        }
        if ((hbpEnables &2))
        {
            xdr1 = hbpAddresses[1];
            xdr7 |= 4+(((hbpSizes[1] << 2) + hbpModes[1]) << 20);
        }
        if ((hbpEnables &4))
        {
            xdr2 = hbpAddresses[2];
            xdr7 |= 16+(((hbpSizes[2] << 2) + hbpModes[2]) << 24);
        }
        if ((hbpEnables &8))
        {
            xdr3 = hbpAddresses[3];
            xdr7 |= 64+(((hbpSizes[3] << 2) + hbpModes[3]) << 28);
        }
    }
    t = activeProcess->threads;
    while (t)
    {
        t->regs.Dr0 = xdr0;
        t->regs.Dr1 = xdr1;
        t->regs.Dr2 = xdr2;
        t->regs.Dr3 = xdr3;
        t->regs.Dr6 = xdr6;
        t->regs.Dr7 = xdr7;
        t = t->next;
    }
}

/* called when bp are resetting */
void hbpResetBP(void){}
/* called when debugger is ending */
void hbpEnd(void)
{
    hbpResetBP();
}

/* called from int1 handler, returns true if a hardware breakpoint was triggered in the current task */
int hbpCheck(THREAD *tThread)
{
    CONTEXT ctx;
    if (!tThread)
        return 0;
    ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    GetThreadContext(tThread->hThread, &ctx);
    if (ctx.Dr6 &15)
    {
        int i;
        for (i = 0; i < 4; i++)
            if (ctx.Dr6 &(1 << i))
                ExtendedMessageBox("Hardware Breakpoint", MB_SYSTEMMODAL |
                    MB_SETFOREGROUND, "Hardware breakpoint for %s triggered",
                    hbpNames[i]);
    }
    return ctx.Dr6 &15;
}

//-------------------------------------------------------------------------

static void SetHDWEDBFields(HWND hwnd, int startField, int index)
{
    AddComboString(hwnd, startField + 1, "Write");
//    AddComboString(hwnd, startField + 1, "Read");
    AddComboString(hwnd, startField + 1, "Access");
    SetComboSel(hwnd, startField + 1, hbpModes[index] == 3 ? 1 : 0);
    AddComboString(hwnd, startField + 2, "Byte");
    AddComboString(hwnd, startField + 2, "Word");
    AddComboString(hwnd, startField + 2, "Dword");
    SetComboSel(hwnd, startField + 2, hbpSizes[index] == 3 ? 2 :
        hbpSizes[index]);
    SetCBField(hwnd, startField + 3, hbpEnables &(1 << index));
    SetEditField(hwnd, startField, hbpNames[index]);
    SendDlgItemMessage(hwnd, startField, EM_LIMITTEXT, 250, 0);
}

//-------------------------------------------------------------------------

static void GetHDWEDBFields(HWND hwnd, int startField, int index)
{
    int i;
    hbpModes[index] = GetComboSel(hwnd, startField + 1) + 1;
    if (hbpModes[index] == 2)
        hbpModes[index]++;
    i = GetComboSel(hwnd, startField + 2);
    if (i == 2)
        i++;
    hbpSizes[index] = i;
    GetEditField(hwnd, startField, hbpNames[index]);
    if (GetCBField(hwnd, startField + 3))
        if (resolvenametoaddr(index, TRUE))
            hbpEnables |= 1 << index;
        else
            hbpEnables &= ~(1 << index);
    else
        hbpEnables &= ~(1 << index);
}

//-------------------------------------------------------------------------

static int FAR PASCAL hbpDlgProc(HWND hwnd, UINT wmsg, WPARAM wParam, LPARAM
    lParam)

{
    NMHDR *nmhead;
    int disable_state;
    switch (wmsg)
    {
        case WM_INITDIALOG:
            //NewFocus(hwnd,IDC_BPSSENABLE);
            CenterWindow(hwnd);
            
            SetHDWEDBFields(hwnd, IDC_BPEDIT1, 0);
            SetHDWEDBFields(hwnd, IDC_BPEDIT2, 1);
            SetHDWEDBFields(hwnd, IDC_BPEDIT3, 2);
            SetHDWEDBFields(hwnd, IDC_BPEDIT4, 3);
            //               SetCBField(hwnd,IDC_BPSSENABLE,!!(workProj.buildFlags & BF_HWBP)) ;
            break;
        case WM_COMMAND:
            if (wParam == IDOK)
            {
                GetHDWEDBFields(hwnd, IDC_BPEDIT1, 0);
                GetHDWEDBFields(hwnd, IDC_BPEDIT2, 1);
                GetHDWEDBFields(hwnd, IDC_BPEDIT3, 2);
                GetHDWEDBFields(hwnd, IDC_BPEDIT4, 3);
                #ifdef XXXXX
                    if (GetCBField(hwnd, IDC_BPSSENABLE))
                        workProj.buildFlags |= BF_HWBP;
                    else
                        workProj.buildFlags &= ~BF_HWBP;
                #endif 
                EndDialog(hwnd, 1);
            }
            else if (wParam == IDCANCEL)
            {
                EndDialog(hwnd, 0);
                break;
            }
            else if (wParam == IDHELP)
            {
                ContextHelp(IDH_HARDWARE_BREAKPOINTS_DIALOG);
                break;
            }
            break;
        case WM_CLOSE:
            EndDialog(hwnd, 0);
            break;
    }
    return 0;
}

//-------------------------------------------------------------------------

void hbpDialog(void)
{
    DialogBox(hInstance, "HBPDLG", hwndFrame, (DLGPROC) &hbpDlgProc);
}
