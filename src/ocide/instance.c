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

#define STRICT
#include <windows.h>
#include <commctrl.h>
#include <richedit.h>
#include <stdio.h>
#include <float.h>
#include "helpid.h"
#include "header.h"
#include <ctype.h>
#include <process.h>
#include <stdlib.h>
// extern int __argc;
// extern char **__argv;
extern HWND hwndFrame;

LIST* nameList;
static LPSTR lpszSlotName = "\\\\.\\mailslot\\ocide_mailslot";
static HANDLE ghMailSlot;
static CRITICAL_SECTION critical;
static BOOL started;
int SendFileName(char* msg)
{
    DWORD cbWritten;
    HANDLE hFile = CreateFile(lpszSlotName, GENERIC_WRITE, FILE_SHARE_READ, /* required to write to a mailslot */
                              (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    WriteFile(hFile, msg, (DWORD)strlen(msg) + 1, &cbWritten, (LPOVERLAPPED)NULL);

    CloseHandle(hFile);
    return TRUE;
}
void PassFilesToInstance(void)
{
    int argc = __argc;
    char** argv = __argv;
    AllowSetForegroundWindow(ASFW_ANY);
    //	argv = CmdLineToC(&argc, GetCommandLineA());
    if (argv)
    {
        if (argc <= 1)
        {
            SendFileName("###TOP###");
        }
        else
        {
            int i;
            for (i = 1; i < argc; i++)
            {
                char buf[260], *p = buf;
                strcpy(buf, argv[i]);
                while (isspace(*p))
                    p++;
                if (*p != '/' && *p != '-')
                {
                    abspath(p, NULL);
                    SendFileName(p);
                }
            }
        }
    }
}
int RetrieveInstanceFile(DWINFO* info)
{
    int rv = 0;
    if (nameList)
    {
        LIST* l;
        char* p;
        EnterCriticalSection(&critical);
        l = nameList;
        nameList = nameList->next;
        LeaveCriticalSection(&critical);
        if (started)
        {
            strncpy(info->dwName, l->data, 260);
            info->dwName[259] = 0;
            p = strrchr(info->dwName, '\\');
            if (!p)
                p = info->dwName;
            else
                p++;
            strcpy(info->dwTitle, p);
            info->dwLineNo = -1;
            info->logMRU = FALSE;
            info->newFile = FALSE;
            rv = 1;
        }
        free(l->data);
        free(l);
    }
    return rv;
}
int msThread(void* aa)
{
    HANDLE hMailSlot = (HANDLE)aa;
    InitializeCriticalSection(&critical);
    started = TRUE;
    while (TRUE)
    {
        char buffer[260], *name;
        DWORD cbRead;
        DWORD fResult = ReadFile(hMailSlot, buffer, sizeof(buffer), &cbRead, (LPOVERLAPPED)NULL);

        if (!fResult)
        {
            started = FALSE;
            DeleteCriticalSection(&critical);
            return FALSE;
        }
        SetForegroundWindow(hwndFrame);
        if (strcmp(buffer, "###TOP###"))
        {
            name = strdup(buffer);
            if (name)
            {
                LIST* l = calloc(sizeof(LIST), 1);
                if (l)
                {
                    l->data = name;
                    EnterCriticalSection(&critical);
                    l->next = nameList;
                    nameList = l;
                    LeaveCriticalSection(&critical);
                }
                else
                    free(name);
            }
        }
    }
    return TRUE;
}
int StartInstanceComms(void)
{
    DWORD id;
    HANDLE hMailSlot = CreateMailslot(lpszSlotName, 0,              /* no maximum message size         */
                                      MAILSLOT_WAIT_FOREVER,        /* no time-out for read operations */
                                      (LPSECURITY_ATTRIBUTES)NULL); /* no security attributes      */

    if (hMailSlot == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }
    ghMailSlot = hMailSlot;
    _beginthread((BEGINTHREAD_FUNC)msThread, 0, (LPVOID)hMailSlot);

    return TRUE;
}
void StopInstanceComms(void) { CloseHandle(ghMailSlot); }