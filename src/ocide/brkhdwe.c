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
#include "helpid.h"
#include "header.h"

extern PROCESS* activeProcess;
extern THREAD* activeThread;
extern enum DebugState uState;
extern HINSTANCE hInstance;
extern HWND hwndFrame;
extern SCOPE lastScope;

static int bkReg;
static int currentState;

#define HBP_WRITE 2
#define HBP_READ 1
#define HBP_EXECUTE 0

#define HBP_BYTE 0
#define HBP_WORD 1
#define HBP_DWORD 3

HDWEBKPT hdwebp[4];

static int resolvenametoaddr(int index, int doErrors)
{
    int addr;
    DEBUG_INFO* dbg;
    VARINFO* var;
    char buf[256];
    strcpy(buf, hdwebp[index].name);
    var = EvalExpr(&dbg, &lastScope, buf, doErrors);
    if (var)
    {
        if (var->constant)
            addr = var->ival;
        else if (var->address < 0x1000)
        {
            char data[20];
            ReadValue(var->address, &data, 4, var);
            addr = *(int*)data;
        }
        else
            addr = var->address;
        FreeVarInfo(var);
        hdwebp[index].address = addr;
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
    for (i = 0; i < 4; i++)
    {
        hdwebp[i].active = 0;
        hdwebp[i].name[0] = 0;
        hdwebp[i].mode = HBP_READ | HBP_WRITE;
        hdwebp[i].size = HBP_DWORD;
    }
}

void hbpDisable(void)
{
    int i;
    for (i = 0; i < 4; i++)
        hdwebp[i].active = 0;
}
BOOL hbpAnyBreakpoints(void)
{
    int i;
    for (i = 0; i < 4; i++)
        if (hdwebp[i].active)
            return TRUE;
    return FALSE;
}
BOOL hbpAnyDisabledBreakpoints(void)
{
    int i;
    for (i = 0; i < 4; i++)
        if (hdwebp[i].active && hdwebp[i].disable)
            return TRUE;
    return FALSE;
}
void hbpEnableAllBreakpoints(BOOL enableState)
{
    int i;
    for (i = 0; i < 4; i++)
        if (hdwebp[i].active)
            hdwebp[i].disable = !enableState;
    // they will have to stop running to get the update...
}
/* called when bp are setting */
void hbpSetBP(void)
{
    int xdr0 = 0, xdr1 = 0, xdr2 = 0, xdr3 = 0, xdr6 = 0, xdr7 = 0x0000;
    int i;
    THREAD* t;
    int flag = FALSE;
    for (i = 0; i < 4; i++)
        if (hdwebp[i].active)
        {
            if (!resolvenametoaddr(i, FALSE))
            {
                ExtendedMessageBox("Hardware Breakpoint", 0, "Could not locate symbol %s, resetting hardware BP", hdwebp[i].name);
                hdwebp[i].active = FALSE;
            }
        }
    {
        // proj && (proj->buildFlags & BF_HWBP)) {
        if ((hdwebp[0].active) && !hdwebp[0].disable)
        {
            xdr0 = hdwebp[0].address;
            xdr7 |= 1 + (((hdwebp[0].size << 2) + hdwebp[0].mode) << 16);
        }
        if ((hdwebp[1].active) && !hdwebp[1].disable)
        {
            xdr1 = hdwebp[1].address;
            xdr7 |= 4 + (((hdwebp[1].size << 2) + hdwebp[1].mode) << 20);
        }
        if ((hdwebp[2].active) && !hdwebp[2].disable)
        {
            xdr2 = hdwebp[2].address;
            xdr7 |= 16 + (((hdwebp[2].size << 2) + hdwebp[2].mode) << 24);
        }
        if ((hdwebp[3].active) && !hdwebp[3].disable)
        {
            xdr3 = hdwebp[3].address;
            xdr7 |= 64 + (((hdwebp[3].size << 2) + hdwebp[3].mode) << 28);
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
void hbpResetBP(void) {}
/* called when debugger is ending */
void hbpEnd(void) { hbpResetBP(); }

/* called from int1 handler, returns true if a hardware breakpoint was triggered in the current task */
int hbpCheck(THREAD* tThread)
{
    CONTEXT ctx;
    if (!tThread)
        return 0;
    ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    GetThreadContext(tThread->hThread, &ctx);
    if (ctx.Dr6 & 15)
    {
        int i;
        for (i = 0; i < 4; i++)
            if (ctx.Dr6 & (1 << i))
                ExtendedMessageBox("Hardware Breakpoint", MB_SYSTEMMODAL | MB_SETFOREGROUND, "Hardware breakpoint for %s triggered",
                                   hdwebp[i].name);
    }
    return ctx.Dr6 & 15;
}

//-------------------------------------------------------------------------

static void SetHDWEDBFields(HWND hwnd, int startField, int index)
{
    AddComboString(hwnd, startField + 1, "Write");
    //    AddComboString(hwnd, startField + 1, "Read");
    AddComboString(hwnd, startField + 1, "Access");
    SetComboSel(hwnd, startField + 1, hdwebp[index].mode == 3 ? 1 : 0);
    AddComboString(hwnd, startField + 2, "Byte");
    AddComboString(hwnd, startField + 2, "Word");
    AddComboString(hwnd, startField + 2, "Dword");
    SetComboSel(hwnd, startField + 2, hdwebp[index].size == 3 ? 2 : hdwebp[index].size);
    SetCBField(hwnd, startField + 3, hdwebp[index].active);
    SetEditField(hwnd, startField, hdwebp[index].name);
    SendDlgItemMessage(hwnd, startField, EM_LIMITTEXT, 250, 0);
}

//-------------------------------------------------------------------------

static void GetHDWEDBFields(HWND hwnd, int startField, int index)
{
    int i;
    hdwebp[index].mode = GetComboSel(hwnd, startField + 1) + 1;
    if (hdwebp[index].mode == 2)
        hdwebp[index].mode++;
    i = GetComboSel(hwnd, startField + 2);
    if (i == 2)
        i++;
    hdwebp[index].size = i;
    GetEditField(hwnd, startField, hdwebp[index].name);
    if (GetCBField(hwnd, startField + 3))
        if (resolvenametoaddr(index, TRUE))
            hdwebp[index].active = TRUE;
        else
            hdwebp[index].active = FALSE;
    else
        hdwebp[index].active = FALSE;
    if (hdwebp[index].active)
        hdwebp[index].disable = FALSE;
}

//-------------------------------------------------------------------------

static int FAR PASCAL hbpDlgProc(HWND hwnd, UINT wmsg, WPARAM wParam, LPARAM lParam)
{
    switch (wmsg)
    {
        case WM_INITDIALOG:
            // NewFocus(hwnd,IDC_BPSSENABLE);
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
                SendDIDMessage(DID_BREAKWND, WM_RESTACK, 0, 0);
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

void hbpDialog(void) { DialogBox(hInstance, "HBPDLG", hwndFrame, (DLGPROC)&hbpDlgProc); }
