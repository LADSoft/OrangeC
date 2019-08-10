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

#define TF_BIT 0x100

#include "header.h"

extern PROCESS* activeProcess;
extern THREAD* activeThread;
extern HWND hwndClient;
extern HWND hwndASM;
extern enum DebugState uState;
extern HANDLE BreakpointSem;

BOOL SingleStepping;
static int LastSkipAddr;

//-------------------------------------------------------------------------

void SingleStep(DWORD procID, DWORD threadID)
{
    activeThread->regs.EFlags |= TF_BIT;
    SingleStepping = TRUE;
}

//-------------------------------------------------------------------------

void ClearTraceFlag(DWORD procID, DWORD threadID) { SingleStepping = FALSE; }

int StepOverIncrement(DEBUG_EVENT* dbe)
{
    unsigned char buf[16];
    int word = 0;
    int address;
    int skiplen = 0;
    ReadProcessMemory(activeProcess->hProcess, (LPVOID)(address = (int)dbe->u.Exception.ExceptionRecord.ExceptionAddress),
                      (LPVOID)buf, 16, 0);
    switch (buf[0])
    {
        case 0xCE:
            skiplen = 1;
            break;
        case 0xCD:
            skiplen = 2;
            break;
        case 0xE8:
            skiplen = 5;
            break;
        default:
            word = (*(short*)buf) & 0x38ff;
            if (word == 0x10ff || word == 0x18ff)
            {
                skiplen = 2;
                if ((word = (buf[1] & 0xc7)) < 0xc0)
                {
                    // not indirect through reg
                    if (word == 6 || word == 5)
                        // Offset
                        skiplen += 4;
                    else
                    {
                        if (word >= 8)
                        {
                            if (word >= 0x80)
                                skiplen += 4;
                            else
                                skiplen += 1;
                        }
                        word &= 7;
                        if (word == 4)
                        {
                            skiplen++; /* has a SIB */
                            if (*(buf + 1) < 0xc0)
                            {
                                word = *(buf + 2);
                                if ((word & 7) == 5)
                                {
                                    /* EBP special cases */
                                    if (*(buf + 1) & 0x40)
                                        skiplen += 4;
                                    else
                                        skiplen += 1;
                                }
                            }
                        }
                    }
                }
            }
            break;
    }
    if (skiplen)
        return address + skiplen;
    return 0;
}

//-------------------------------------------------------------------------

void DoStepOver(DEBUG_EVENT* dbe)
{
    int skipaddr = StepOverIncrement(dbe);
    if (skipaddr)
    {
        int* p = calloc(2, sizeof(int));
        p[0] = skipaddr;
        SetTempBreakPoint(dbe->dwProcessId, dbe->dwThreadId, p);
    }
    else
    {
        SingleStep(dbe->dwProcessId, dbe->dwThreadId);
    }
}

//-------------------------------------------------------------------------

void DoStepIn(DEBUG_EVENT* dbe)
{
    LastSkipAddr = StepOverIncrement(dbe);
    SingleStep(dbe->dwProcessId, dbe->dwThreadId);
}

//-------------------------------------------------------------------------

int IsStepping(DEBUG_EVENT* dbe)
{
    char module[256];
    int line;
    if (uState == SteppingOver)
    {
        int v;
        if ((v = GetBreakpointLine((int)dbe->u.Exception.ExceptionRecord.ExceptionAddress, module, &line, FALSE)) !=
            (int)dbe->u.Exception.ExceptionRecord.ExceptionAddress)
        {
            DoStepOver(dbe);
            return TRUE;
        }
        else
        {
            uState = Running;
            return FALSE;
        }
    }
    else if (uState == SteppingIn)
    {
        int addr = GetBreakpointLine((int)dbe->u.Exception.ExceptionRecord.ExceptionAddress, module, &line, FALSE);
        if (addr == (int)dbe->u.Exception.ExceptionRecord.ExceptionAddress)
        {
            if (LastSkipAddr && LastSkipAddr != addr)
            {
                // beginning of function, step over prologue...
                LastSkipAddr = 0;
                DoStepOver(dbe);
                return TRUE;
            }
            else
            {
                uState = Running;
                return FALSE;
            }
        }
        else if (LastSkipAddr)
        {
            if ((int)dbe->u.Exception.ExceptionRecord.ExceptionAddress != LastSkipAddr)
            {
                //                if (addr)
                {
                    //                    uState = SteppingOver;
                    DoStepOver(dbe);
                    return TRUE;
                }
                /*
                else
                {
                    int *p = calloc(2, sizeof(int));
                    p[0] = LastSkipAddr;
                    SetTempBreakPoint(dbe->dwProcessId, dbe->dwThreadId, p);
                    uState = StepInOut;
                    return TRUE;
                }
                */
            }
            else
            {
                DoStepIn(dbe);
                return TRUE;
            }
        }
        else
        {
            DoStepIn(dbe);
            return TRUE;
        }
    }
    else if (uState == StepInOut)
    {
        int addr = GetBreakpointLine((int)dbe->u.Exception.ExceptionRecord.ExceptionAddress, module, &line, FALSE);
        if (addr == (int)dbe->u.Exception.ExceptionRecord.ExceptionAddress)
        {
            uState = Running;
            return FALSE;
        }
        else
        {
            uState = SteppingIn;
            DoStepIn(dbe);
            return TRUE;
        }
    }
    else if (uState == FinishStepOut)
    {
        int addr = GetBreakpointLine((int)dbe->u.Exception.ExceptionRecord.ExceptionAddress, module, &line, TRUE);
        if (!addr || addr == (int)dbe->u.Exception.ExceptionRecord.ExceptionAddress)
        {
            uState = Running;
            return FALSE;
        }
        else
        {
            uState = SteppingIn;
            DoStepIn(dbe);
            return TRUE;
        }
    }
    return FALSE;
}

//-------------------------------------------------------------------------

void StepOver(DEBUG_EVENT* dbe)
{
    if ((HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0) != hwndASM)
    {
        uState = SteppingOver;
    }
    else
        SetStatusMessage("Disassembly window open - stepping via assembly", FALSE);
    SaveRegisterContext();
    DoStepOver(dbe);
    ReleaseSemaphore(BreakpointSem, 1, 0);
}

//-------------------------------------------------------------------------

void StepOut(DEBUG_EVENT* dbe)
{
    uState = SteppingOut;
    SaveRegisterContext();
    ReleaseSemaphore(BreakpointSem, 1, 0);
}

//-------------------------------------------------------------------------

void StepIn(DEBUG_EVENT* dbe)
{
    if ((HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0) != hwndASM)
    {
        uState = SteppingIn;
    }
    else
        SetStatusMessage("Disassembly window open - stepping via assembly", FALSE);
    SaveRegisterContext();
    DoStepIn(dbe);
    ReleaseSemaphore(BreakpointSem, 1, 0);
}

//-------------------------------------------------------------------------

int RunTo(DEBUG_EVENT* dbe)
{
    HWND wnd = GetFocus();
    if (wnd == hwndASM)
    {
        int* p;
        int addr;
        if (!(addr = SendMessage(hwndASM, WM_GETCURSORADDRESS, 0, 0)))
            return FALSE;
        p = calloc(2, sizeof(int));
        p[0] = addr;
        SetTempBreakPoint(dbe->dwProcessId, dbe->dwThreadId, p);
        SaveRegisterContext();
        ReleaseSemaphore(BreakpointSem, 1, 0);
    }
    else
    {
        wnd = GetParent(wnd);
        if (IsEditWindow(wnd))
        {
            DWINFO* ptr = (DWINFO*)GetWindowLong(wnd, 0);
            int linenum;
            int* addresses;
            SendMessage(ptr->dwHandle, EM_GETSEL, (WPARAM)&linenum, 0);
            linenum = SendMessage(ptr->dwHandle, EM_EXLINEFROMCHAR, 0, linenum) + 1;
            linenum = TagOldLine(ptr->dwName, linenum);
            addresses = GetBreakpointAddresses(ptr->dwName, &linenum);
            if (addresses)
            {
                SetTempBreakPoint(dbe->dwProcessId, dbe->dwThreadId, addresses);
                SaveRegisterContext();
                ReleaseSemaphore(BreakpointSem, 1, 0);
            }
            else
                return FALSE;
        }
        else
            return FALSE;
    }
    return TRUE;
}

//-------------------------------------------------------------------------

int isSteppingOut(DEBUG_EVENT* dbe)
{
    if (uState == SteppingOut)
    {
        unsigned char bf = 0;
        ReadProcessMemory(activeProcess->hProcess, (LPVOID)activeThread->regs.Eip, (LPVOID)&bf, 1, 0);
        if (bf == 0xc2 || bf == 0xc3)
        {
            SingleStep(dbe->dwProcessId, dbe->dwThreadId);
            uState = FinishStepOut;
        }
        else
            DoStepOver(dbe);
        return TRUE;
    }
    return FALSE;
}
