/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

#include <errno.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <process.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

extern char _RTL_DATA** _environ;

void __WaitForInputIdle(HANDLE hProcess)
{
    typedef void WINAPI Func(HANDLE, UINT);
    HMODULE aa = LoadLibrary("User32.dll");
    if (aa)
    {
        Func* ptr = (Func*)GetProcAddress(aa, "WaitForInputIdle");
        if (ptr)
        {
            ptr(hProcess, INFINITE);
            FreeLibrary(aa);
        }
        else
            _abort();
    }
    else
    {
        _abort();
    }
}

static char* createenviron(char** env)
{
    int len = 0;
    char **dummy, *rv = 0;
    if (!env)
        env = _environ;
    dummy = env;
    while (*dummy)
    {
        len += strlen(*dummy) + 1;
        dummy++;
    }
    if (len)
    {
        len += 1;
        rv = malloc(len);
        if (rv)
        {
            char* p = rv;
            while (*env)
            {
                strcpy(p, *env);
                p += strlen(p) + 1;
                env++;
            }
            *p = 0;
        }
    }
    return rv;
}
int __ll_spawn(char* file, char* parms, char** env, int mode)
{
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    DWORD rv = -1;
    char buf[1000], *block = createenviron(env);
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = (HANDLE)__uiohandle(fileno(stdin));
    si.hStdOutput = (HANDLE)__uiohandle(fileno(stdout));
    si.hStdError = (HANDLE)__uiohandle(fileno(stderr));
    sprintf(buf, "\"%s\" %s", file, parms);
    if (CreateProcess(file, buf, 0, 0, TRUE, NORMAL_PRIORITY_CLASS | (DETACHED_PROCESS * (mode == P_DETACH)), (LPVOID)block, 0, &si,
                      &pi))
    {
        rv = (DWORD)pi.hProcess;
        if (mode == P_WAIT || mode == P_OVERLAY)
        {
            __WaitForInputIdle(pi.hProcess);
            WaitForSingleObject(pi.hProcess, INFINITE);
            GetExitCodeProcess(pi.hProcess, &rv);
        }
        if (mode != P_NOWAIT && mode != P_DETACH)
        {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
        free(block);
        return rv;
    }
    else
    {
        free(block);
        return -1;
    }
}
