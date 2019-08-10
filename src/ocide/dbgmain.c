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
#include <winerror.h>

#include "header.h"
#include <process.h>

extern HWND hwndClient;
extern HWND hwndFrame;
extern HWND hwndToolDebug;
extern DWINFO* editWindows;
extern PROJECTITEM* activeProject;
extern BOOL SingleStepping;
extern HWND hwndTbBuildType, hwndTbProfile;

enum DebugState uState = notDebugging;
HANDLE StartupSem, BreakpointSem;
PROCESS* activeProcess;
THREAD *activeThread, *stoppedThread;

PROCESS* debugProcessList;

static int Semaphores;
static int stopWinMain;
static HANDLE abortEvent;
static void DeleteDLLInfo(DWORD procId, DLL_INFO* info);
// this code sorta has the beginning of multiprocess debugging, but, the symbol
// table stuff needs to be revamped to handle it properly.  For right now,
// debugging is started on a single-process basis so there will only be one process
// even though multiple processes should be linked into a list in a reasonable way...
void RunProgram(PROJECTITEM* plist)
{
    char *exeName, *exeArgs;
    SetOutputNames(plist, TRUE);
    AddRootTables(plist, FALSE);
    exeName = Lookup("__DEBUG_EXECUTABLE", plist, NULL);
    exeArgs = Lookup("__DEBUG_ARGUMENTS", plist, NULL);
    ReleaseSymbolTables();
    if (PropGetBool(plist, "__SHOW_RETURN_CODE"))
        DosWindow(plist->realName, exeName, exeArgs, exeName, "Process complete.  Exit Code: %d\n");
    else
        DosWindow(plist->realName, exeName, exeArgs, NULL, NULL);
    free(exeName);
    free(exeArgs);
}
int initiateDebug(PROJECTITEM* pj, int stopimmediately)
{
    static char cmd[4096];
    if (uState == notDebugging && pj)
    {
        int val = 0;
        char *exe, *args, *wd;
        char wdbuf[MAX_PATH];
        SetOutputNames(pj, TRUE);
        AddRootTables(pj, FALSE);
        exe = Lookup("__DEBUG_EXECUTABLE", pj, NULL);
        args = Lookup("__DEBUG_ARGUMENTS", pj, NULL);
        wd = Lookup("__DEBUG_WORKING_DIR", pj, NULL);
        strcpy(wdbuf, wd);
        free(wd);
        if (wdbuf[0] == 0)
        {
            char* p;
            strcpy(wdbuf, exe);
            p = strrchr(wdbuf, '\\');
            if (p)
                *p = 0;
        }
        if (exe)
            val = IsPEFile(exe);
        if (val <= 0)
        {
            if (val == 0)
                ExtendedMessageBox("File Error", MB_SYSTEMMODAL | MB_SETFOREGROUND, "File %s was not found", cmd);
            else
                ExtendedMessageBox("File Error", MB_SYSTEMMODAL | MB_SETFOREGROUND, "File %s must be a win32 executable to debug.",
                                   cmd);
            ReleaseSymbolTables();
            free(exe);
            free(args);
            return uState;
        }
        EnableWindow(hwndTbProfile, FALSE);
        EnableWindow(hwndTbBuildType, FALSE);
        stopWinMain = !!stopimmediately;
        Semaphores = TRUE;
        StartupSem = CreateSemaphore(0, 0, 1, 0);
        BreakpointSem = CreateSemaphore(0, 0, 1, 0);
        sprintf(cmd, "\"%s\" %s\001%s", exe, args, wdbuf);
        *strchr(cmd, '\001') = 0;
        ReleaseSymbolTables();
        free(exe);
        free(args);
        _beginthread((BEGINTHREAD_FUNC)StartDebug, 0, (LPVOID)cmd);

        ReleaseSymbolTables();

        WaitForSingleObject(StartupSem, INFINITE);
        PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
    }
    return uState;
}

//-------------------------------------------------------------------------

BOOL CALLBACK topenumfunc(HWND wnd, LPARAM value)
{
    DWORD id;
    GetWindowThreadProcessId(wnd, &id);
    if (id == value)
    {
        SetForegroundWindow(wnd);
        if (IsIconic(wnd))
            ShowWindow(wnd, SW_RESTORE);
        return FALSE;
    }
    return TRUE;
}

/* if we don't do this, the debugger locks up sometimes */
void ProcessToTop(DWORD processid) { EnumWindows((WNDENUMPROC)topenumfunc, processid); }

BOOL CALLBACK consoleMenuFunc(HWND wnd, LPARAM value)
{
    char buf[256];
    GetWindowText(wnd, buf, 256);
    if (!strcmp(buf, (char*)value))
    {
        HMENU hMenu = GetSystemMenu(wnd, FALSE);
        DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
        return FALSE;
    }
    return TRUE;
}
void DisableConsoleCloseButton(char* cmd) { EnumWindows((WNDENUMPROC)consoleMenuFunc, (LPARAM)cmd); }
THREAD* GetThread(DWORD procId, DWORD threadId)
{
    PROCESS* p = GetProcess(procId);
    if (p)
    {
        THREAD* thread = p->threads;
        while (thread)
        {
            if (thread->idThread == threadId)
                return thread;
            thread = thread->next;
        }
    }
    return NULL;
}
static THREAD* NewThread(DWORD procId, DWORD threadId)
{
    PROCESS* p = GetProcess(procId);
    if (p)
    {
        THREAD** thread = &p->threads;
        while (*thread)
            thread = &(*thread)->next;
        *thread = calloc(1, sizeof(THREAD));
        if (*thread)
        {
            (*thread)->idThread = threadId;
            (*thread)->process = p;
        }
        return *thread;
    }
    return NULL;
}
static void DeleteThread(DWORD procId, DWORD threadId)
{
    PROCESS* p = GetProcess(procId);
    if (p)
    {
        THREAD** thread = &p->threads;
        while (*thread && (*thread)->idThread != threadId)
            thread = &(*thread)->next;
        if (*thread)
        {
            THREAD* v = *thread;
            (*thread) = (*thread)->next;
            free(v);
        }
    }
}
PROCESS* GetProcess(DWORD procId)
{
    PROCESS* process = debugProcessList;
    while (process)
    {
        if (process->idProcess == procId)
            return process;
        process = process->next;
    }
    return NULL;
}
static PROCESS* NewProcess(DWORD procId)
{
    PROCESS** process = &debugProcessList;
    while (*process)
        process = &(*process)->next;
    *process = calloc(1, sizeof(PROCESS));
    if (*process)
    {
        (*process)->idProcess = procId;
    }
    return *process;
}
static void DeleteProcess(DWORD procId)
{
    PROCESS** process = &debugProcessList;
    while (*process && (*process)->idProcess != procId)
        process = &(*process)->next;
    if (*process)
    {
        PROCESS* v = *process;
        BREAKPOINT** bp;
        // Created threads may not exit gracefully
        // in fact one at least will always be left
        while (v->threads)
        {
            THREAD* th = v->threads;
            DeleteThread(v->idProcess, th->idThread);
        }
        // System dlls don't get an unload message
        while (v->dll_info)
        {
            DLL_INFO* d = v->dll_info;
            DeleteDLLInfo(procId, d);
        }
        bp = &v->breakpoints.next;
        while (*bp)
        {
            BREAKPOINT* q = *bp;
            *bp = (*bp)->next;
            free(q);
        }
        (*process) = (*process)->next;
        FreeDebugInfo(v->dbg_info);
        free(v);
    }
}
DLL_INFO* GetDLLInfo(DWORD procId, DWORD addr)
{
    PROCESS* pr = GetProcess(procId);
    if (pr)
    {
        DLL_INFO* d = pr->dll_info;
        while (d && d->base != addr)
            d = d->next;
        return d;
    }
    return NULL;
}
static DLL_INFO* NewDLLInfo(DWORD procId, DWORD base)
{
    PROCESS* pr = GetProcess(procId);
    if (pr)
    {
        DLL_INFO **d = &pr->dll_info, *x;
        while (*d && (*d)->base < base)
            d = &(*d)->next;
        x = calloc(1, sizeof(DLL_INFO));
        if (x)
        {
            x->next = *d;
            x->base = base;
            *d = x;
        }
        return x;
    }
    return NULL;
}
static void DeleteDLLInfo(DWORD procId, DLL_INFO* info)
{
    PROCESS* pr = GetProcess(procId);
    if (pr)
    {
        DLL_INFO** d = &pr->dll_info;
        while (*d && (*d) != info)
            d = &(*d)->next;
        if (*d)
        {
            (*d) = (*d)->next;
            FreeDebugInfo(info->dbg_info);
            free(info);
        }
    }
}
//-------------------------------------------------------------------------

static int HandleBreakpoint(DEBUG_EVENT* info, char* cmd)
{
    if (uState != Aborting)
    {
        SetForegroundWindow(hwndFrame);
        //    ProcessToTop(GetCurrentProcessId());
        if (uState != Aborting)
            uState = atBreakpoint;
        PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
        PostDIDMessage(DID_STACKWND, WM_RESTACK, (WPARAM)1, 0);
        PostDIDMessage(DID_REGWND, WM_COMMAND, ID_SETADDRESS, (LPARAM)GetThread(info->dwProcessId, info->dwThreadId)->hThread);
        PostDIDMessage(DID_WATCHWND, WM_COMMAND, ID_SETADDRESS, 0);
        PostDIDMessage(DID_WATCHWND + 1, WM_COMMAND, ID_SETADDRESS, 0);
        PostDIDMessage(DID_WATCHWND + 2, WM_COMMAND, ID_SETADDRESS, 0);
        PostDIDMessage(DID_WATCHWND + 3, WM_COMMAND, ID_SETADDRESS, 0);
        PostDIDMessage(DID_LOCALSWND, WM_COMMAND, ID_SETADDRESS, 0);
        PostDIDMessage(DID_THREADWND, WM_RESTACK, (WPARAM)1, 0);
        PostDIDMessage(DID_MEMWND, WM_RESTACK, 0, 0);
        PostDIDMessage(DID_MEMWND + 1, WM_RESTACK, 0, 0);
        PostDIDMessage(DID_MEMWND + 2, WM_RESTACK, 0, 0);
        PostDIDMessage(DID_MEMWND + 3, WM_RESTACK, 0, 0);
        SendDIDMessage(DID_THREADWND, WM_RESTACK, 0, 0);
        PostMessage(hwndFrame, WM_BREAKPOINT, 0, (LPARAM)info);
        PostDIDMessage(DID_BREAKWND, WM_DOENABLE, 1, 0);
    }
    if (uState != Aborting && uState != notDebugging)
    {
        WaitForSingleObject(BreakpointSem, INFINITE);
    }
    PostDIDMessage(DID_STACKWND, WM_RESTACK, (WPARAM)0, 0);
    PostDIDMessage(DID_THREADWND, WM_RESTACK, (WPARAM)0, 0);
    PostDIDMessage(DID_BREAKWND, WM_DOENABLE, 0, 0);
    ApplyBreakAddress(0, 0);  // get rid of the break line
    if (uState == notDebugging || uState == Aborting)
    {
        PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
        return DBG_TERMINATE_PROCESS;
    }
    else
    {
        if (uState != SteppingOut && uState != SteppingOver && uState != SteppingIn && uState != StepInOut)
            uState = Running;
        PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
        ProcessToTop(info->dwProcessId);
        return DBG_CONTINUE;
    }
}

//-------------------------------------------------------------------------

static int HandleException(DEBUG_EVENT* info, char* cmd)
{
    if (uState == notDebugging || uState == Aborting)
        return DBG_EXCEPTION_NOT_HANDLED;
    if (info->u.Exception.ExceptionRecord.ExceptionCode != DBG_CONTROL_C &&
        info->u.Exception.ExceptionRecord.ExceptionCode != DBG_CONTROL_BREAK &&
        !(PropGetBool(activeProject, "__STOP_FIRST_CHANCE")))
        if (info->u.Exception.dwFirstChance)
            return DBG_EXCEPTION_NOT_HANDLED;
    activeThread = stoppedThread = GetThread(info->dwProcessId, info->dwThreadId);
    SetForegroundWindow(hwndFrame);
    //    ProcessToTop(GetCurrentProcessId());
    uState = atException;
    PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
    PostDIDMessage(DID_STACKWND, WM_RESTACK, (WPARAM)1, 0);
    PostDIDMessage(DID_WATCHWND, WM_COMMAND, ID_SETADDRESS, 0);
    PostDIDMessage(DID_WATCHWND + 1, WM_COMMAND, ID_SETADDRESS, 0);
    PostDIDMessage(DID_WATCHWND + 2, WM_COMMAND, ID_SETADDRESS, 0);
    PostDIDMessage(DID_WATCHWND + 3, WM_COMMAND, ID_SETADDRESS, 0);
    PostDIDMessage(DID_LOCALSWND, WM_COMMAND, ID_SETADDRESS, 0);
    PostDIDMessage(DID_THREADWND, WM_RESTACK, (WPARAM)1, 0);
    PostDIDMessage(DID_REGWND, WM_COMMAND, ID_SETADDRESS, (LPARAM)GetThread(info->dwProcessId, info->dwThreadId)->hThread);
    PostDIDMessage(DID_MEMWND, WM_RESTACK, 0, 0);
    PostDIDMessage(DID_MEMWND + 1, WM_RESTACK, 0, 0);
    PostDIDMessage(DID_MEMWND + 2, WM_RESTACK, 0, 0);
    PostDIDMessage(DID_MEMWND + 3, WM_RESTACK, 0, 0);
    PostMessage(hwndFrame, WM_EXCEPTION, 0, (LPARAM)info);
    PostDIDMessage(DID_BREAKWND, WM_DOENABLE, 1, 0);
    WaitForSingleObject(BreakpointSem, INFINITE);
    //   PostMessage(hwndRegister,WM_COMMAND,ID_SETCONTEXT,0) ;
    PostDIDMessage(DID_STACKWND, WM_RESTACK, (WPARAM)0, 0);
    PostDIDMessage(DID_THREADWND, WM_RESTACK, (WPARAM)0, 0);
    PostDIDMessage(DID_BREAKWND, WM_DOENABLE, 0, 0);
    if (uState == notDebugging || uState == Aborting)
    {
        PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
        return DBG_TERMINATE_PROCESS;
    }
    if (uState != SteppingOut && uState != SteppingOver && uState != SteppingIn && uState != StepInOut)
        uState = Running;
    PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
    ProcessToTop(info->dwProcessId);
    return DBG_EXCEPTION_NOT_HANDLED;
}

//-------------------------------------------------------------------------

void BlastExitProcFunc(DWORD procID, DWORD threadID, DWORD address)
{
    THREAD* thread = GetThread(procID, threadID);
    int buf = 0;  // exit code
    thread->regs.Eip = address;
    // write exit code
    thread->regs.Esp -= 4;  // for exit code
    WriteProcessMemory(thread->process->hProcess, (LPVOID)thread->regs.Esp, (LPVOID)&buf, 4, 0);
    thread->regs.Esp -= 4;  // for return address (n/c since this is ExitProcess)
}

//-------------------------------------------------------------------------

void GetRegs(DWORD procID)
{
    PROCESS* p = debugProcessList;
    while (p)
    {
        if (p->idProcess == procID || procID == 0)
        {
            THREAD* thread = p->threads;
            while (thread)
            {
                thread->regs.ContextFlags = CONTEXT_FULL | CONTEXT_FLOATING_POINT | 0;  // CONTEXT_DEBUG_REGISTERS;
                GetThreadContext(thread->hThread, &thread->regs);
                thread = thread->next;
            }
        }
        p = p->next;
    }
}

//-------------------------------------------------------------------------

void SetRegs(DWORD procID)
{
    PROCESS* p = debugProcessList;
    while (p)
    {
        if (p->idProcess == procID || procID == 0)
        {
            THREAD* thread = p->threads;
            while (thread)
            {
                SetThreadContext(thread->hThread, &thread->regs);
                thread = thread->next;
            }
        }
        p = p->next;
    }
}

void StopRunning(int newState)
{
    THREAD* th = debugProcessList->threads;  // use first process, will usually be the
                                             // one we invoked unless it exited...
    if (uState == Running || uState == SteppingOut || uState == StepInOut)
    {
        HANDLE hThisThread = GetCurrentThread();
        int iOldPriority = GetThreadPriority(hThisThread);
        int* p = calloc(2, sizeof(int));
        p[0] = debugProcessList->base;
        SetForegroundWindow(hwndFrame);
        //        ProcessToTop(GetCurrentProcessId());
        SetThreadPriority(hThisThread, REALTIME_PRIORITY_CLASS + THREAD_PRIORITY_BELOW_NORMAL);
        while (th)
        {
            SuspendThread(th->hThread);
            th = th->next;
        }
        GetRegs(0);
        th = debugProcessList->threads;
        th->oldaddr = th->regs.Eip;
        th->regs.Eip = debugProcessList->base;
        th->breakpoint.addresses = p;
        th->breakpoint.active = FALSE;
        allocBreakPoint(debugProcessList->hProcess, &th->breakpoint);
        SetRegs(0);
        PostThreadMessage(th->idThread, WM_NULL, 0, 0);
        th->suspcount = ResumeThread(th->hThread) - 1;
        if ((int)th->suspcount < 0)
            th->suspcount = 0;
        if (th->suspcount)
            while (ResumeThread(th->hThread) > 1)
                ;
        th = th->next;
        while (th)
        {
            ResumeThread(th->hThread);
            th = th->next;
        }
        if (newState != nullState)
            uState = newState;

        /* At this point it still may not stop if there is a deadlock
         * condition, requires further work
         */
        SetThreadPriority(hThisThread, iOldPriority);
        ProcessToTop(debugProcessList->idProcess);
        th = debugProcessList->threads;
        free(th->breakpoint.tempvals);
        free(th->breakpoint.addresses);
        th->breakpoint.tempvals = NULL;
        th->breakpoint.addresses = NULL;
    }
}

//-------------------------------------------------------------------------

static void abortDebugThread(void)
{
    int i = 0;
    if (uState != notDebugging)
    {
        if (uState == atBreakpoint || uState == atException)
        {
            uState = Aborting;
            SaveRegisterContext();
            ReleaseSemaphore(BreakpointSem, 1, 0);
        }
        else
        {
            StopRunning(Aborting);
            uState = Aborting;
            SaveRegisterContext();
            ReleaseSemaphore(BreakpointSem, 1, 0);
        }
        while (uState != notDebugging)
        {
            MSG msg;
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                ProcessMessage(&msg);
                if (msg.message == WM_QUIT)
                    break;
            }
            if (++i >= 100)
            {
                ExtendedMessageBox("Debugger", MB_SETFOREGROUND | MB_SYSTEMMODAL, "Internal error - debugger is not stopping");
                SetEvent(abortEvent);
                return;
            }
            Sleep(100);
        }
    }
    SetEvent(abortEvent);
}

//-------------------------------------------------------------------------

void abortDebug(void)
{
    if (!abortEvent)
        abortEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    _beginthread((BEGINTHREAD_FUNC)abortDebugThread, 0, NULL);
    WaitForSingleObject(abortEvent, INFINITE);
}

void TranslateFilename(char* szFilename)
{
    char szWin32Name[MAX_PATH];
    // sanity checks
    char* pszInfo;
    if (szFilename == NULL || szFilename[0] == 0)
    {
        return;
    }

    // check for "strange" filenames
    pszInfo = strstr(szFilename, "\\SystemRoot\\");
    if (pszInfo == szFilename)
    {
        //       \SystemRoot\System32\smss.exe
        // -->   c:\winnt\System32\smss.exe  using GetWindowsDirectory()
        UINT Len = GetWindowsDirectory(szWin32Name, MAX_PATH);
        if (Len != 0)
        {
            strcat(szWin32Name, "\\");
            strcat(szWin32Name, &szFilename[strlen("\\SystemRoot\\")]);
            strcpy(szFilename, szWin32Name);
            return;
        }
    }
    else
    {
        //       \??\C:\WINNT\system32\winlogon.exe
        // -->   C:\WINNT\system32\winlogon.exe
        pszInfo = strstr(szFilename, "\\??\\");
        if (pszInfo == szFilename)
        {
            strcpy(szWin32Name, &szFilename[strlen("\\??\\")]);
            strcpy(szFilename, szWin32Name);
        }
    }
}
BOOL GetFileNameOfDLL(HMODULE hpsapiLib, HANDLE hProcess, DWORD base, DWORD nameBase, BOOL fUnicode, char* outName)
{
    BOOL found = FALSE;
    char name[MAX_PATH * sizeof(int)];
    if (nameBase)
    {
        DWORD len;
        ReadProcessMemory(hProcess, (LPVOID)nameBase, (LPVOID)&nameBase, 4, &len);

        if (nameBase)
        {
            ReadProcessMemory(hProcess, (LPVOID)nameBase, (LPVOID)name, MAX_PATH * sizeof(WCHAR), &len);
            name[len] = 0;
            if (fUnicode)
            {
                int i;
                for (i = 0; i < len; i += 2)
                    name[i / 2] = name[i];
                name[i] = 0;
            }
            strcpy(outName, name);
            found = TRUE;
        }
    }
    if (hpsapiLib && !found)
    {
        FARPROC func = GetProcAddress(hpsapiLib, "GetModuleFileNameExA");
        if (func)
        {
            (*func)(hProcess, base, name, MAX_PATH);
            TranslateFilename(name);
            found = TRUE;
        }
    }
    return found;
}
//-------------------------------------------------------------------------

void StartDebug(char* cmd)
{
    STARTUPINFO stStartInfo;
    PROCESS_INFORMATION stProcessInfo;
    DEBUG_EVENT stDE;
    BOOL bShownExitCode = FALSE;
    BOOL bSeenInitialBP = FALSE;
    BOOL bContinue = TRUE;
    DWORD dwContinueStatus;
    BOOL bRet;
    BOOL ContinueStep = FALSE;
    HMODULE hpsapiLib;
    HANDLE hExitHandle = NULL;
    int hbp;
    BOOL isTerminating = FALSE;
    char buf[512], *pwd = cmd + strlen(cmd) + 1;
    BOOL sittingAtDataBreakpoint = FALSE;

    memset(&stStartInfo, 0, sizeof(STARTUPINFO));
    memset(&stProcessInfo, 0, sizeof(PROCESS_INFORMATION));

    stStartInfo.cb = sizeof(STARTUPINFO);

    LPTCH env = GetEnv();
    bRet = CreateProcess(NULL, cmd, NULL, NULL, FALSE,
                         CREATE_NEW_PROCESS_GROUP | DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS |
                             (PropGetInt(activeProject, "__PROJECTTYPE") == BT_CONSOLE ? CREATE_NEW_CONSOLE : 0),
                         env, pwd, &stStartInfo, &stProcessInfo);
    if (!bRet)
    {
        ReleaseSemaphore(StartupSem, 1, 0);
        LPVOID msg;
        DWORD le = GetLastError();

        if (le == ERROR_DIRECTORY)
        {
            ExtendedMessageBox("Debugger", MB_SETFOREGROUND | MB_SYSTEMMODAL,
                               "Could not execute %s:\n\n The specified working directory is invalid", cmd);
        }
        else
        {
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, le,
                          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&msg, 0, NULL);

            ExtendedMessageBox("Debugger", MB_SETFOREGROUND | MB_SYSTEMMODAL, "Could not execute %s:\n\n %s", cmd, msg);

            LocalFree(msg);
        }
        FreeEnv(env);
        return;
    }
    Sleep(500); /* Needed to make things happy */
    uState = Running;

    ReleaseSemaphore(StartupSem, 1, 0);

    // this is used to retrieve the names of DLLs as they load...
    hpsapiLib = LoadLibrary("PSAPI.DLL");

    PostMessage(hwndFrame, WM_HIDEDEBUGWINDOWS, 0, 0);
    PostDIDMessage(DID_BREAKWND, WM_DOENABLE, 0, 0);

    SelectInfoWindow(ERR_DEBUG_WINDOW);
    SetStatusMessage("Starting Debugger", FALSE);
    // Loop until told to stop.
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
    while (bContinue)
    {
        // Pause until a debug event notification happens.
        bContinue = WaitForDebugEvent(&stDE, 200);
        if (bContinue)
        {
            switch (stDE.dwDebugEventCode)
            {
                case CREATE_PROCESS_DEBUG_EVENT:
                {
                    PROCESS* process = NewProcess(stDE.dwProcessId);
                    THREAD* thread = NewThread(stDE.dwProcessId, stDE.dwThreadId);
                    if (!process->next)
                        activeProcess = process;
                    //                    	if (PropGetInt(activeProject, "__PROJECTTYPE") == BT_CONSOLE)
                    //							SetConsoleCtrlHandler(NULL, TRUE);
                    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
                    // Save the handle information needed for later.
                    process->hProcess = stDE.u.CreateProcessInfo.hProcess;
                    thread->hThread = stDE.u.CreateProcessInfo.hThread;
                    strcpy(thread->name, "main thread");
                    dwContinueStatus = DBG_CONTINUE;
                    process->dbg_info = GetDebugInfo(process->hProcess, (DWORD)stDE.u.CreateProcessInfo.lpBaseOfImage);
                    process->base = (DWORD)stDE.u.CreateProcessInfo.lpBaseOfImage;
                    if (GetFileNameOfDLL(hpsapiLib, process->hProcess, process->base, (DWORD)stDE.u.CreateProcessInfo.lpImageName,
                                         (DWORD)stDE.u.CreateProcessInfo.fUnicode, &process->name[0]))
                    {
                        char* ava = "";
                        if (process->dbg_info)
                            ava = " ...Debug info loaded";
                        sprintf(buf, "Process Loading: %s%s\r\n", process->name, ava);
                        SendInfoMessage(ERR_DEBUG_WINDOW, buf);
                    }
                    /* next line has to be deferred to the init bp */
                    TagRegenBreakPoints(FALSE);
                    GetRegs(0);
                    thread->regs.Dr7 = 0;
                    SetBreakPoints(stDE.dwProcessId);
                    SetRegs(0);
                    CloseHandle(stDE.u.CreateProcessInfo.hFile);
                    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
                    dwContinueStatus = DBG_CONTINUE;
                }
                break;

                case EXIT_PROCESS_DEBUG_EVENT:
                {
                    activeProcess = 0;  // tells the edit window painter to abstain from drawing line data
                    PROCESS* pr = GetProcess(stDE.dwProcessId);
                    PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);

                    if (!debugProcessList->next)
                    {
                        DuplicateHandle(GetCurrentProcess(), debugProcessList->hProcess, GetCurrentProcess(), &hExitHandle, 0,
                                        FALSE, DUPLICATE_SAME_ACCESS);
                    }
                    if (pr)
                    {
                        char buf[512];
                        DWORD exitCode;
                        GetExitCodeProcess(pr->hProcess, &exitCode);
                        sprintf(buf, "Process Unloading: %s with exit code %d\r\n", pr->name, exitCode);
                        SendInfoMessage(ERR_DEBUG_WINDOW, buf);
                        DeleteProcess(pr->idProcess);
                    }
                    if (!debugProcessList)
                    {
                        bContinue = FALSE;
                        hbpEnd();
                        databpEnd();
                    }
                    dwContinueStatus = DBG_CONTINUE;
                }
                break;

                case LOAD_DLL_DEBUG_EVENT:
                {
                    DLL_INFO* dllInfo = NewDLLInfo(stDE.dwProcessId, (DWORD)stDE.u.LoadDll.lpBaseOfDll);
                    if (dllInfo)
                    {
                        char* ava = "";
                        PROCESS* pr = GetProcess(stDE.dwProcessId);
                        dllInfo->name[0] = 0;
                        dllInfo->dbg_info = GetDebugInfo(pr->hProcess, (DWORD)stDE.u.LoadDll.lpBaseOfDll);
                        dllInfo->fUnicode = stDE.u.LoadDll.fUnicode;
                        dllInfo->hFile = stDE.u.LoadDll.hFile;
                        CloseHandle(dllInfo->hFile);
                        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
                        if (GetFileNameOfDLL(hpsapiLib, pr->hProcess, dllInfo->base, (DWORD)stDE.u.LoadDll.lpImageName,
                                             stDE.u.LoadDll.fUnicode, &dllInfo->name[0]))
                        {
                            char* ava = "";
                            if (dllInfo->dbg_info)
                                ava = " ...Debug info loaded";
                            sprintf(buf, "DLL Loading: %s%s\r\n", dllInfo->name, ava);
                            SendInfoMessage(ERR_DEBUG_WINDOW, buf);
                        }

                        if (dllInfo->dbg_info && PropGetBool(activeProject, "__BREAK_DLL"))
                        {
                            int addr = GetMainAddress(dllInfo->dbg_info);
                            if (addr)
                            {
                                PROCESS* pr = GetProcess(stDE.dwProcessId);
                                if (pr)
                                {
                                    BREAKPOINT** p = &pr->breakpoints.next;
                                    while (*p)
                                        p = &(*p)->next;
                                    *p = calloc(sizeof(BREAKPOINT), 1);
                                    if (*p)
                                    {
                                        DWINFO* ptr = editWindows;
                                        int* q = calloc(2, sizeof(int));
                                        q[0] = addr;
                                        (*p)->addresses = q;
                                        GetBreakpointLine(addr, &(*p)->module[0], &(*p)->linenum, FALSE);
                                        dllInfo->breakpoint = addr;
                                        while (ptr)
                                        {
                                            if (ptr->active)
                                                InvalidateRect(ptr->self, 0, 0);
                                            ptr = ptr->next;
                                        }
                                    }
                                }
                            }
                        }
                        // set breakpoints for recently loaded DLL.
                        TagRegenBreakPoints(FALSE);
                        GetRegs(0);
                        SetBreakPoints(stDE.dwProcessId);
                        SetRegs(0);
                        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
                    }
                    dwContinueStatus = DBG_CONTINUE;
                }
                break;
                case UNLOAD_DLL_DEBUG_EVENT:
                {
                    DLL_INFO* info = GetDLLInfo(stDE.dwProcessId, (DWORD)stDE.u.UnloadDll.lpBaseOfDll);
                    if (info)
                    {
                        GetRegs(0);
                        dbgClearBreakpointsForModule(info);  // undefined in this file
                        ClearBreakPoints(stDE.dwProcessId);
                        DeleteDLLInfo(stDE.dwProcessId, info);
                        SetBreakPoints(stDE.dwProcessId);
                        SetRegs(0);
                        sprintf(buf, "DLL Unloading: %s\r\n", info->name);
                        SendInfoMessage(ERR_DEBUG_WINDOW, buf);
                    }
                    dwContinueStatus = DBG_CONTINUE;
                }
                break;

                case CREATE_THREAD_DEBUG_EVENT:
                {
                    char name[256];
                    THREAD* th = NewThread(stDE.dwProcessId, stDE.dwThreadId);
                    strcpy(name, "Win32 Thread");
                    if (th)
                    {
                        th->hThread = stDE.u.CreateThread.hThread;
                        FindFunctionName(name, (int)stDE.u.CreateThread.lpStartAddress, NULL, NULL);
                        strcpy(th->name, name);
                    }
                    sprintf(buf, "Thread creation: %d %08x %s\r\n", stDE.dwThreadId, stDE.u.CreateThread.lpStartAddress, name);
                    SendInfoMessage(ERR_DEBUG_WINDOW, buf);
                    if (isTerminating)
                        SuspendThread(stDE.u.CreateThread.hThread);
                    GetRegs(0);
                    th->regs.Dr7 = 0;
                    SetRegs(0);
                    dwContinueStatus = DBG_CONTINUE;
                }
                break;
                case EXIT_THREAD_DEBUG_EVENT:
                {
                    THREAD* th = GetThread(stDE.dwProcessId, stDE.dwThreadId);
                    if (th)
                    {
                        if (th->name && th->name[0])
                        {
                            sprintf(buf, "Thread exit: %d %s with exit code %d\r\n", th->idThread, th->name,
                                    stDE.u.ExitProcess.dwExitCode);
                        }
                        else
                        {
                            sprintf(buf, "Thread exit: %d with exit code %d\r\n", th->idThread, stDE.u.ExitProcess.dwExitCode);
                        }
                        SendInfoMessage(ERR_DEBUG_WINDOW, buf);
                        DeleteThread(stDE.dwProcessId, stDE.dwThreadId);
                    }
                    dwContinueStatus = DBG_CONTINUE;
                }
                break;

                case OUTPUT_DEBUG_STRING_EVENT:
                {
                    PROCESS* pr = GetProcess(stDE.dwProcessId);
                    char buf2[256];
                    int len = stDE.u.DebugString.nDebugStringLength;
                    if (len > 255)
                        len = 255;
                    memset(buf2, 0, 256);
                    ReadProcessMemory(pr->hProcess, stDE.u.DebugString.lpDebugStringData, buf2, len, 0);
                    SetInfoColor(ERR_DEBUG_WINDOW, 0xff0000);
                    SendInfoMessage(ERR_DEBUG_WINDOW, buf2);
                    dwContinueStatus = DBG_CONTINUE;
                }
                break;

                case RIP_EVENT:
                {
                    dwContinueStatus = DBG_CONTINUE;
                }
                break;

                case EXCEPTION_DEBUG_EVENT:
                {
                    switch (stDE.u.Exception.ExceptionRecord.ExceptionCode)
                    {
                        case DBG_CONTROL_C:
                        case DBG_CONTROL_BREAK:
                        case EXCEPTION_SINGLE_STEP:
                            activeProcess = GetProcess(stDE.dwProcessId);
                            activeThread = stoppedThread = GetThread(stDE.dwProcessId, stDE.dwThreadId);
                            ClearTraceFlag(stDE.dwProcessId, stDE.dwThreadId);
                            if ((hbp = hbpCheck(activeThread)))
                            {
                                ContinueStep = FALSE;
                            }
                            else if (ContinueStep)
                            {
                                if (!SittingOnBreakPoint(&stDE))
                                {
                                    ContinueStep = FALSE;
                                    GetRegs(0);
                                    SetBreakPoints(stDE.dwProcessId);
                                    SetRegs(0);
                                    dwContinueStatus = DBG_CONTINUE;
                                    break;
                                }
                            }
                            // fall through
                        case EXCEPTION_BREAKPOINT:
                            activeProcess = GetProcess(stDE.dwProcessId);
                            activeThread = stoppedThread = GetThread(stDE.dwProcessId, stDE.dwThreadId);
                            GetRegs(0);
                            ClearBreakPoints(stDE.dwProcessId);
                            if (!inTempBreakPoint((int)stDE.u.Exception.ExceptionRecord.ExceptionAddress) ||
                                stDE.dwThreadId == activeProcess->idTempBpThread)
                            {
                                if (bSeenInitialBP)
                                {
                                    if (stDE.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_BREAKPOINT &&
                                        (isLocalBreakPoint((int)stDE.u.Exception.ExceptionRecord.ExceptionAddress) ||
                                         inTempBreakPoint((int)stDE.u.Exception.ExceptionRecord.ExceptionAddress)))
                                    {
                                        activeThread->regs.Eip--;
                                    }
                                    if (activeThread->oldaddr)
                                    {
                                        activeThread->regs.Eip = activeThread->oldaddr;

                                        stDE.u.Exception.ExceptionRecord.ExceptionAddress = (LPVOID)activeThread->oldaddr;
                                        while (activeThread->suspcount)
                                        {
                                            activeThread->suspcount--;
                                            SuspendThread(activeThread->hThread);
                                        }
                                        activeThread->oldaddr = 0;
                                        freeBreakPoint(activeProcess->hProcess, &activeThread->breakpoint);
                                    }
                                    ClearTempBreakPoint(stDE.dwProcessId);
                                    if (!isTerminating && uState != SteppingOut && !IsStepping(&stDE))
                                    {
                                        dwContinueStatus = HandleBreakpoint(&stDE, cmd);
                                    }
                                    else
                                        dwContinueStatus = DBG_CONTINUE;
                                }
                                else
                                {
                                    DWORD addr;
                                    bSeenInitialBP = TRUE;
                                    /* This had to be deferred for win2K */

                                    if (activeProcess)
                                        activeProcess->ExitAddr =
                                            FindExitProcessAddress(activeProcess->hProcess, activeProcess->base);
                                    if ((addr = GetMainAddress(activeProcess->dbg_info)))
                                    {
                                        if (stopWinMain)
                                            if (stDE.dwProcessId == stProcessInfo.dwProcessId)
                                            {
                                                int* p = calloc(2, sizeof(int));
                                                p[0] = addr;
                                                SetTempBreakPoint(stDE.dwProcessId, stDE.dwThreadId, p);
                                            }
                                    }
                                    else
                                    {
                                        ClearTempBreakPoint(stDE.dwProcessId);
                                        if (stopWinMain)
                                        {
                                            int* p = calloc(2, sizeof(int));
                                            p[0] = GetEntryPoint();
                                            SetTempBreakPoint(stDE.dwProcessId, stDE.dwThreadId, p);
                                        }
                                    }
                                    SendDIDMessage(DID_BREAKWND, WM_RESTACK, 0, 0);
                                    TagRegenBreakPoints(TRUE);
                                    dwContinueStatus = DBG_CONTINUE;
                                }
                                isSteppingOut(&stDE);
                            }
                            else
                            {
                                dwContinueStatus = DBG_CONTINUE;
                            }
                        // WIN32 doesn't support the DBG_TERMINATE_PROCESS flag, so we
                        // have to go to some effort to get the process terminated.
                        // This started at process creation time, when we found the address
                        // of the ExitProcess function if there was one.
                        doterm:
                            if (dwContinueStatus == DBG_TERMINATE_PROCESS)
                            {
                                isTerminating = TRUE;
                                dwContinueStatus = DBG_CONTINUE;
                                sittingAtDataBreakpoint = FALSE;
                                ContinueStep = FALSE;
                                if (debugProcessList->ExitAddr)
                                {
                                    // Blast an exit process call into the main thread, and resume it
                                    GetRegs(0);
                                    BlastExitProcFunc(activeProcess->idProcess, activeThread->idThread, debugProcessList->ExitAddr);
                                    SetRegs(0);
                                    while ((int)ResumeThread(activeThread->hThread) > 1)
                                        ;
                                    PostThreadMessage(activeThread->idThread, WM_NULL, 0, 0);
                                }
                                else
                                {
                                    // This should never happen, unless we don't have ExitProcess
                                    // in the executable file.  Our tools don't do that.
                                    TerminateProcess(debugProcessList->hProcess, 0);
                                    ExtendedMessageBox(
                                        "Debugger", MB_SETFOREGROUND | MB_SYSTEMMODAL,
                                        "A forced termination has been used.\n  This may leave the system in an unstable state.");
                                }
                            }
                            else if (!SingleStepping && !isTerminating)
                            {
                                if (SittingOnBreakPoint(&stDE) || sittingAtDataBreakpoint)
                                {
                                    sittingAtDataBreakpoint = FALSE;
                                    ContinueStep = TRUE;
                                    SingleStep(stDE.dwProcessId, stDE.dwThreadId);
                                }
                                else
                                {
                                    SetBreakPoints(stDE.dwProcessId);
                                }
                            }
                            SetRegs(0);

                            break;
                        default:
                            activeProcess = GetProcess(stDE.dwProcessId);
                            activeThread = stoppedThread = GetThread(stDE.dwProcessId, stDE.dwThreadId);
                            if (!isTerminating)
                            {
                                switch (databpCheck(&stDE))
                                {
                                    case 0:
                                        /* real exception */
                                        GetRegs(0);
                                        ClearBreakPoints(stDE.dwProcessId);
                                        dwContinueStatus = HandleException(&stDE, cmd);
                                        SetBreakPoints(stDE.dwProcessId);
                                        if (dwContinueStatus == DBG_TERMINATE_PROCESS)
                                            goto doterm;
                                        SetRegs(0);
                                        break;
                                    case 1:
                                        /* breakpoint */
                                        GetRegs(0);
                                        ClearBreakPoints(stDE.dwProcessId);
                                        ClearTempBreakPoint(stDE.dwProcessId);
                                        if (!isTerminating && uState != SteppingOut && !IsStepping(&stDE))
                                        {
                                            sittingAtDataBreakpoint = TRUE;
                                            dwContinueStatus = HandleBreakpoint(&stDE, cmd);
                                            isSteppingOut(&stDE);
                                            goto doterm;
                                        }
                                        else
                                            dwContinueStatus = DBG_CONTINUE;
                                        break;
                                    case 2:
                                        /* singlestep & continue */
                                        GetRegs(0);
                                        ClearBreakPoints(stDE.dwProcessId);
                                        ContinueStep = TRUE;
                                        SingleStep(stDE.dwProcessId, stDE.dwThreadId);
                                        SetRegs(0);
                                        break;
                                }
                            }
                            break;
                    }
                }
                break;

                    // For any other events, just continue on.
                default:
                {
                    dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED;
                }
                break;
            }
            // Pass on to the operating system.
            ContinueDebugEvent(stDE.dwProcessId, stDE.dwThreadId, dwContinueStatus);
        }
        else
        {

            bContinue = TRUE;
            if (hExitHandle && WaitForSingleObject(hExitHandle, 0 == WAIT_OBJECT_0))
            {
                CloseHandle(hExitHandle);
                bContinue = FALSE;
            }
        }
    }
    activeProcess = NULL;
    CloseHandle(stProcessInfo.hThread);
    CloseHandle(stProcessInfo.hProcess);
    CloseHandle(StartupSem);
    CloseHandle(BreakpointSem);
    SetForegroundWindow(hwndFrame);
    PostMessage(hwndFrame, WM_HIDEDEBUGWINDOWS, 1, 0);

    PostMessage(hwndFrame, WM_INITMENUPOPUP, 0, 0);

    //    ProcessToTop(GetCurrentProcessId());
    uState = notDebugging;
    PostDIDMessage(DID_REGWND, WM_COMMAND, ID_SETADDRESS, 0);
    RedrawAllBreakpoints();

    PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
    PostDIDMessage(DID_PROJWND, WM_COMMAND, IDM_RESETPROFILECOMBOS, 0);
    PostDIDMessage(DID_BREAKWND, WM_DOENABLE, 1, 0);
    SendDIDMessage(DID_WATCHWND, WM_INITIALSTACK, 0, 0);
    SendDIDMessage(DID_WATCHWND + 1, WM_INITIALSTACK, 0, 0);
    SendDIDMessage(DID_WATCHWND + 2, WM_INITIALSTACK, 0, 0);
    SendDIDMessage(DID_WATCHWND + 3, WM_INITIALSTACK, 0, 0);
    SetStatusMessage("Stopping Debugger", FALSE);
    if (hpsapiLib)
        FreeLibrary(hpsapiLib);
    FreeEnv(env);
}
